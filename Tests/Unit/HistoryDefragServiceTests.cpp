#include <cstring>

#include <juce_core/juce_core.h>

#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchMutator/HistoryDefragService.h"
#include "Core/Services/PatchMutator/MutationHistoryStore.h"
#include "Core/Services/PatchMutator/MutationNaming.h"

class HistoryDefragServiceTests : public juce::UnitTest
{
public:
    HistoryDefragServiceTests() : juce::UnitTest("HistoryDefragService") {}

    void runTest() override
    {
        defrag_emptyStore_fails();
        defrag_singleAt99_renumbersToZero();
        defrag_rootsCompactInListOrder();
        defrag_retriesCompactPerRoot();
        defrag_preservesBufferPayload();
        defrag_updatesPatchNames();
        defrag_keepsInitialSnapshot();
        defrag_retryGapAt99_unblocksPeek();
        defrag_remapsSelectedRoot();
        defrag_remapsSelectedRetry();
    }

private:
    static Core::PatchModel makeDistinctBuffer(int seed)
    {
        Core::PatchModel model;
        const auto marker = static_cast<juce::uint8>(seed & 0xFF);
        std::memset(model.data(), marker, Core::PatchModel::kBufferSize);
        model.data()[8] = marker;
        return model;
    }

    static Core::PatchModel makeParentBuffer(int seed)
    {
        Core::PatchModel model;
        const auto marker = static_cast<juce::uint8>((seed + 100) & 0xFF);
        std::memset(model.data(), marker, Core::PatchModel::kBufferSize);
        model.data()[9] = marker;
        return model;
    }

    static Core::PatchModel namedResult(int rootIndex,
                                        int retryIndex,
                                        int seed)
    {
        auto model = makeDistinctBuffer(seed);
        Core::MutationNaming::applyPatchName(model, rootIndex, retryIndex);
        return model;
    }

    void defrag_emptyStore_fails()
    {
        beginTest("defrag_emptyStore_fails");

        Core::MutationHistoryStore store;
        const auto result = Core::HistoryDefragService::defrag(
            store,
            { -1, Core::MutationHistoryStore::kRootOnly });

        expect(! result.success);
    }

    void defrag_singleAt99_renumbersToZero()
    {
        beginTest("defrag_singleAt99_renumbersToZero");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(99,
                                namedResult(99, Core::MutationHistoryStore::kRootOnly, 99),
                                makeParentBuffer(99)));

        expect(store.isRootIndexExhausted());
        expect(! store.peekNextRootIndex().has_value());

        const auto result = Core::HistoryDefragService::defrag(
            store,
            { 99, Core::MutationHistoryStore::kRootOnly });
        expect(result.success);
        expect(store.hasRoot(0));
        expect(! store.hasRoot(99));
        expectEquals(store.peekNextRootIndex().value_or(-1), 1);
        expect(! store.isRootIndexExhausted());
    }

    void defrag_rootsCompactInListOrder()
    {
        beginTest("defrag_rootsCompactInListOrder");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(0, namedResult(0, Core::MutationHistoryStore::kRootOnly, 0), makeParentBuffer(0)));
        expect(store.insertRoot(5, namedResult(5, Core::MutationHistoryStore::kRootOnly, 5), makeParentBuffer(5)));
        expect(store.insertRoot(99, namedResult(99, Core::MutationHistoryStore::kRootOnly, 99), makeParentBuffer(99)));

        const auto result = Core::HistoryDefragService::defrag(
            store,
            { 5, Core::MutationHistoryStore::kRootOnly });
        expect(result.success);
        expectEquals(result.remappedRootIndex, 1);
        expectEquals(result.remappedRetryIndex, Core::MutationHistoryStore::kRootOnly);

        const auto roots = store.getSortedRootIndices();
        expectEquals(roots.size(), 3);
        expectEquals(roots[0], 0);
        expectEquals(roots[1], 1);
        expectEquals(roots[2], 2);
    }

    void defrag_retriesCompactPerRoot()
    {
        beginTest("defrag_retriesCompactPerRoot");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(0, namedResult(0, Core::MutationHistoryStore::kRootOnly, 0), makeParentBuffer(0)));
        expect(store.insertRetry(0, 0, namedResult(0, 0, 10), makeParentBuffer(10)));
        expect(store.insertRetry(0, 3, namedResult(0, 3, 13), makeParentBuffer(13)));
        expect(store.insertRetry(0, 7, namedResult(0, 7, 17), makeParentBuffer(17)));

        expect(Core::HistoryDefragService::defrag(store, { 0, 7 }).success);

        const auto retries = store.getSortedRetryIndices(0);
        expectEquals(retries.size(), 3);
        expectEquals(retries[0], 0);
        expectEquals(retries[1], 1);
        expectEquals(retries[2], 2);
    }

    void defrag_preservesBufferPayload()
    {
        beginTest("defrag_preservesBufferPayload");

        Core::MutationHistoryStore store;
        auto result = namedResult(99, Core::MutationHistoryStore::kRootOnly, 42);
        result.data()[12] = static_cast<juce::uint8>(0xDE);
        auto parent = makeParentBuffer(42);
        parent.data()[13] = static_cast<juce::uint8>(0xBE);
        expect(store.insertRoot(99, result, parent));

        expect(Core::HistoryDefragService::defrag(store, { 99, Core::MutationHistoryStore::kRootOnly }).success);

        const auto entry = store.getEntry(0);
        expect(entry.has_value());
        expectEquals(static_cast<int>(entry->result[12]), static_cast<int>(0xDE));
        expectEquals(static_cast<int>(entry->parentSnapshot[13]), static_cast<int>(0xBE));
    }

    void defrag_updatesPatchNames()
    {
        beginTest("defrag_updatesPatchNames");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(99, namedResult(99, Core::MutationHistoryStore::kRootOnly, 99), makeParentBuffer(99)));
        expect(store.insertRoot(5, namedResult(5, Core::MutationHistoryStore::kRootOnly, 5), makeParentBuffer(5)));
        expect(store.insertRetry(5, 3, namedResult(5, 3, 53), makeParentBuffer(53)));

        expect(Core::HistoryDefragService::defrag(store, { 5, 3 }).success);

        const auto rootEntry = store.getEntry(0);
        expect(rootEntry.has_value());
        expectEquals(bufferToModel(rootEntry->result).getName(), juce::String("M00"));

        const auto retryEntry = store.getEntry(0, 0);
        expect(retryEntry.has_value());
        expectEquals(bufferToModel(retryEntry->result).getName(), juce::String("M00-R00"));

        const auto rootAtOne = store.getEntry(1);
        expect(rootAtOne.has_value());
        expectEquals(bufferToModel(rootAtOne->result).getName(), juce::String("M01"));
    }

    void defrag_keepsInitialSnapshot()
    {
        beginTest("defrag_keepsInitialSnapshot");

        Core::MutationHistoryStore store;
        const auto initial = makeDistinctBuffer(7);
        store.setInitialSnapshot(initial);

        expect(store.insertRoot(5, namedResult(5, Core::MutationHistoryStore::kRootOnly, 5), makeParentBuffer(5)));
        expect(store.insertRoot(99, namedResult(99, Core::MutationHistoryStore::kRootOnly, 99), makeParentBuffer(99)));

        expect(Core::HistoryDefragService::defrag(store, { 99, Core::MutationHistoryStore::kRootOnly }).success);

        expect(store.hasInitialSnapshot());
        const auto restored = store.getInitialSnapshot();
        expect(std::memcmp(restored.data(), initial.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void defrag_retryGapAt99_unblocksPeek()
    {
        beginTest("defrag_retryGapAt99_unblocksPeek");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(0, namedResult(0, Core::MutationHistoryStore::kRootOnly, 0), makeParentBuffer(0)));
        expect(store.insertRetry(0, 99, namedResult(0, 99, 99), makeParentBuffer(99)));

        expect(store.isRetryIndexExhausted(0));
        expect(! store.peekNextRetryIndex(0).has_value());

        expect(Core::HistoryDefragService::defrag(store, { 0, 99 }).success);

        expect(store.hasRetry(0, 0));
        expect(! store.hasRetry(0, 99));
        expectEquals(store.peekNextRetryIndex(0).value_or(-1), 1);
        expect(! store.isRetryIndexExhausted(0));
    }

    void defrag_remapsSelectedRoot()
    {
        beginTest("defrag_remapsSelectedRoot");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(99, namedResult(99, Core::MutationHistoryStore::kRootOnly, 99), makeParentBuffer(99)));

        const auto result = Core::HistoryDefragService::defrag(
            store,
            { 99, Core::MutationHistoryStore::kRootOnly });

        expect(result.success);
        expectEquals(result.remappedRootIndex, 0);
        expectEquals(result.remappedRetryIndex, Core::MutationHistoryStore::kRootOnly);
    }

    void defrag_remapsSelectedRetry()
    {
        beginTest("defrag_remapsSelectedRetry");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(0, namedResult(0, Core::MutationHistoryStore::kRootOnly, 0), makeParentBuffer(0)));
        expect(store.insertRetry(0, 99, namedResult(0, 99, 99), makeParentBuffer(99)));

        const auto result = Core::HistoryDefragService::defrag(store, { 0, 99 });

        expect(result.success);
        expectEquals(result.remappedRootIndex, 0);
        expectEquals(result.remappedRetryIndex, 0);
    }

    static Core::PatchModel bufferToModel(const std::array<juce::uint8, Core::PatchModel::kBufferSize>& buffer)
    {
        Core::PatchModel model;
        model.loadFrom(buffer.data());
        return model;
    }
};

static HistoryDefragServiceTests historyDefragServiceTests;
