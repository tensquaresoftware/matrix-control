#include <cstring>

#include <juce_core/juce_core.h>

#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchMutator/MutationHistoryStore.h"

class MutationHistoryStoreTests : public juce::UnitTest
{
public:
    MutationHistoryStoreTests() : juce::UnitTest("MutationHistoryStore") {}

    void runTest() override
    {
        emptyStore_queries();
        insertFirstRoot_indexZero();
        sortedRoots_numericOrder();
        insertRetry_underRoot();
        deleteRetry_preservesGap();
        deleteRoot_preservesGap();
        deleteRoot_cascadesRetries();
        insertRejection_paths();
        deleteRejection_missingTargets();
        maxRoots_rejects101st();
        maxRetries_rejects101st();
        gapExhausted_singleAt99();
        gapExhausted_retryAt99();
        clear_keepsInitialSnapshot();
        clearInitialSnapshot_clearsSnapshot();
        getEntry_returnsCopy();
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

    void emptyStore_queries()
    {
        beginTest("emptyStore_queries");

        Core::MutationHistoryStore store;
        expect(store.isEmpty());
        expectEquals(store.rootCount(), 0);
        expect(! store.hasInitialSnapshot());
        expect(store.getSortedRootIndices().isEmpty());
    }

    void insertFirstRoot_indexZero()
    {
        beginTest("insertFirstRoot_indexZero");

        Core::MutationHistoryStore store;
        const auto result = makeDistinctBuffer(1);
        const auto parent = makeParentBuffer(1);

        expect(store.insertRoot(0, result, parent));
        expect(store.hasRoot(0));
        expectEquals(store.rootCount(), 1);
        expect(! store.isEmpty());
        expectEquals(store.peekNextRootIndex().value_or(-1), 1);
    }

    void sortedRoots_numericOrder()
    {
        beginTest("sortedRoots_numericOrder");

        Core::MutationHistoryStore store;

        expect(store.insertRoot(5, makeDistinctBuffer(5), makeParentBuffer(5)));
        expect(store.insertRoot(0, makeDistinctBuffer(0), makeParentBuffer(0)));
        expect(store.insertRoot(2, makeDistinctBuffer(2), makeParentBuffer(2)));

        const auto sorted = store.getSortedRootIndices();
        expectEquals(sorted.size(), 3);
        expectEquals(sorted[0], 0);
        expectEquals(sorted[1], 2);
        expectEquals(sorted[2], 5);
    }

    void insertRetry_underRoot()
    {
        beginTest("insertRetry_underRoot");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(3, makeDistinctBuffer(3), makeParentBuffer(3)));
        expect(store.insertRetry(3, 0, makeDistinctBuffer(30), makeParentBuffer(30)));
        expect(store.insertRetry(3, 1, makeDistinctBuffer(31), makeParentBuffer(31)));

        expectEquals(store.retryCount(3), 2);
        expect(store.hasRetry(3, 0));
        expect(store.hasRetry(3, 1));

        const auto sorted = store.getSortedRetryIndices(3);
        expectEquals(sorted.size(), 2);
        expectEquals(sorted[0], 0);
        expectEquals(sorted[1], 1);
    }

    void deleteRetry_preservesGap()
    {
        beginTest("deleteRetry_preservesGap");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(0, makeDistinctBuffer(0), makeParentBuffer(0)));
        expect(store.insertRetry(0, 0, makeDistinctBuffer(10), makeParentBuffer(10)));
        expect(store.insertRetry(0, 2, makeDistinctBuffer(12), makeParentBuffer(12)));

        expect(store.deleteRetry(0, 0));
        expect(! store.hasRetry(0, 0));
        expect(store.hasRetry(0, 2));

        const auto nextRetry = store.peekNextRetryIndex(0);
        expect(nextRetry.has_value());
        expectEquals(nextRetry.value(), 3);
    }

    void deleteRoot_preservesGap()
    {
        beginTest("deleteRoot_preservesGap");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(0, makeDistinctBuffer(0), makeParentBuffer(0)));
        expect(store.insertRoot(5, makeDistinctBuffer(5), makeParentBuffer(5)));

        expect(store.deleteRoot(0));
        expect(! store.hasRoot(0));
        expect(store.hasRoot(5));

        const auto nextRoot = store.peekNextRootIndex();
        expect(nextRoot.has_value());
        expectEquals(nextRoot.value(), 6);
    }

    void deleteRoot_cascadesRetries()
    {
        beginTest("deleteRoot_cascadesRetries");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(4, makeDistinctBuffer(4), makeParentBuffer(4)));
        expect(store.insertRetry(4, 0, makeDistinctBuffer(40), makeParentBuffer(40)));
        expect(store.insertRetry(4, 1, makeDistinctBuffer(41), makeParentBuffer(41)));
        expect(store.insertRetry(4, 2, makeDistinctBuffer(42), makeParentBuffer(42)));

        expect(store.deleteRoot(4));
        expect(! store.hasRoot(4));
        expectEquals(store.retryCount(4), 0);
        expect(! store.hasRetry(4, 0));
        expect(! store.hasRetry(4, 1));
        expect(! store.hasRetry(4, 2));
    }

    void insertRejection_paths()
    {
        beginTest("insertRejection_paths");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(1, makeDistinctBuffer(1), makeParentBuffer(1)));

        expect(! store.insertRoot(1, makeDistinctBuffer(2), makeParentBuffer(2)));
        expect(! store.insertRoot(-1, makeDistinctBuffer(3), makeParentBuffer(3)));
        expect(! store.insertRoot(100, makeDistinctBuffer(4), makeParentBuffer(4)));
        expect(! store.insertRetry(99, 0, makeDistinctBuffer(5), makeParentBuffer(5)));
        expect(! store.insertRetry(1, -1, makeDistinctBuffer(6), makeParentBuffer(6)));
        expect(! store.insertRetry(1, 100, makeDistinctBuffer(7), makeParentBuffer(7)));
        expect(store.insertRetry(1, 0, makeDistinctBuffer(8), makeParentBuffer(8)));
        expect(! store.insertRetry(1, 0, makeDistinctBuffer(9), makeParentBuffer(9)));
    }

    void deleteRejection_missingTargets()
    {
        beginTest("deleteRejection_missingTargets");

        Core::MutationHistoryStore store;
        expect(! store.deleteRoot(0));
        expect(! store.deleteRetry(0, 0));

        expect(store.insertRoot(2, makeDistinctBuffer(2), makeParentBuffer(2)));
        expect(! store.deleteRoot(99));
        expect(! store.deleteRetry(2, 0));
        expect(! store.deleteRetry(99, 0));
    }

    void maxRoots_rejects101st()
    {
        beginTest("maxRoots_rejects101st");

        Core::MutationHistoryStore store;

        for (int index = 0; index < Core::MutationHistoryStore::kMaxRoots; ++index)
            expect(store.insertRoot(index, makeDistinctBuffer(index), makeParentBuffer(index)));

        expectEquals(store.rootCount(), Core::MutationHistoryStore::kMaxRoots);
        expect(store.isRootSlotsFull());
        expect(! store.peekNextRootIndex().has_value());
        expect(! store.insertRoot(0, makeDistinctBuffer(999), makeParentBuffer(999)));
    }

    void maxRetries_rejects101st()
    {
        beginTest("maxRetries_rejects101st");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(0, makeDistinctBuffer(0), makeParentBuffer(0)));

        for (int index = 0; index < Core::MutationHistoryStore::kMaxRetriesPerRoot; ++index)
        {
            expect(store.insertRetry(0,
                                     index,
                                     makeDistinctBuffer(100 + index),
                                     makeParentBuffer(100 + index)));
        }

        expectEquals(store.retryCount(0), Core::MutationHistoryStore::kMaxRetriesPerRoot);
        expect(store.isRetrySlotsFull(0));
        expect(! store.peekNextRetryIndex(0).has_value());
        expect(! store.insertRetry(0, 0, makeDistinctBuffer(999), makeParentBuffer(999)));
    }

    void gapExhausted_retryAt99()
    {
        beginTest("gapExhausted_retryAt99");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(0, makeDistinctBuffer(0), makeParentBuffer(0)));
        expect(store.insertRetry(0, 99, makeDistinctBuffer(99), makeParentBuffer(99)));

        expectEquals(store.retryCount(0), 1);
        expect(! store.isRetrySlotsFull(0));
        expect(! store.peekNextRetryIndex(0).has_value());
        expect(store.isRetryIndexExhausted(0));
        expect(! store.isRetryIndexExhausted(99));
    }

    void gapExhausted_singleAt99()
    {
        beginTest("gapExhausted_singleAt99");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(99, makeDistinctBuffer(99), makeParentBuffer(99)));

        expectEquals(store.rootCount(), 1);
        expect(! store.isRootSlotsFull());
        expect(! store.peekNextRootIndex().has_value());
        expect(store.isRootIndexExhausted());
    }

    void clear_keepsInitialSnapshot()
    {
        beginTest("clear_keepsInitialSnapshot");

        Core::MutationHistoryStore store;
        const auto initial = makeDistinctBuffer(7);
        store.setInitialSnapshot(initial);

        expect(store.insertRoot(1, makeDistinctBuffer(1), makeParentBuffer(1)));
        expect(store.insertRoot(2, makeDistinctBuffer(2), makeParentBuffer(2)));

        store.clear();

        expect(store.isEmpty());
        expectEquals(store.rootCount(), 0);
        expect(store.hasInitialSnapshot());

        const auto restored = store.getInitialSnapshot();
        expect(std::memcmp(restored.data(), initial.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void clearInitialSnapshot_clearsSnapshot()
    {
        beginTest("clearInitialSnapshot_clearsSnapshot");

        Core::MutationHistoryStore store;
        store.setInitialSnapshot(makeDistinctBuffer(8));
        expect(store.hasInitialSnapshot());

        store.clearInitialSnapshot();

        expect(! store.hasInitialSnapshot());

        const auto restored = store.getInitialSnapshot();
        const auto zeroed = Core::PatchModel();
        expect(std::memcmp(restored.data(), zeroed.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void getEntry_returnsCopy()
    {
        beginTest("getEntry_returnsCopy");

        Core::MutationHistoryStore store;
        const auto result = makeDistinctBuffer(20);
        const auto parent = makeParentBuffer(20);
        expect(store.insertRoot(6, result, parent));

        auto entry = store.getEntry(6);
        expect(entry.has_value());
        entry->result[10] = static_cast<juce::uint8>(0xAB);

        const auto stored = store.getEntry(6);
        expect(stored.has_value());
        expectEquals(static_cast<int>(stored->result[10]), static_cast<int>(result.data()[10]));
    }
};

static MutationHistoryStoreTests mutationHistoryStoreTests;
