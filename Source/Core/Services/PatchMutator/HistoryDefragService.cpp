#include "Core/Services/PatchMutator/HistoryDefragService.h"

#include <cstring>
#include <map>

#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchMutator/MutationHistoryStore.h"
#include "Core/Services/PatchMutator/MutationNaming.h"

namespace Core
{

namespace
{

    PatchModel bufferToModel(const std::array<juce::uint8, PatchModel::kBufferSize>& buffer)
    {
        PatchModel model;
        model.loadFrom(buffer.data());
        return model;
    }

    void applyNamedResult(MutationEntry& entry, int rootIndex, int retryIndex)
    {
        auto model = bufferToModel(entry.result);
        MutationNaming::applyPatchName(model, rootIndex, retryIndex);
        std::memcpy(entry.result.data(), model.data(), entry.result.size());
    }

    std::optional<std::pair<int, int>> captureSelectedIndices(const MutationHistoryStore& store,
                                                              int selectedMutateRootIndex,
                                                              int selectedRetryIndex)
    {
        if (selectedMutateRootIndex < 0 || ! store.hasRoot(selectedMutateRootIndex))
            return std::nullopt;

        if (selectedRetryIndex == MutationHistoryStore::kRootOnly)
            return std::make_pair(selectedMutateRootIndex, MutationHistoryStore::kRootOnly);

        if (store.hasRetry(selectedMutateRootIndex, selectedRetryIndex))
            return std::make_pair(selectedMutateRootIndex, selectedRetryIndex);

        return std::make_pair(selectedMutateRootIndex, MutationHistoryStore::kRootOnly);
    }

    HistoryDefragResult remapSelection(const std::optional<std::pair<int, int>>& selectedIndices,
                                       const std::map<int, int>& rootMap,
                                       const std::map<std::pair<int, int>, int>& retryMap)
    {
        HistoryDefragResult result;
        result.success = true;
        result.remappedRootIndex = -1;
        result.remappedRetryIndex = MutationHistoryStore::kRootOnly;

        if (! selectedIndices.has_value())
            return result;

        const auto [oldRoot, oldRetry] = *selectedIndices;
        const auto rootIt = rootMap.find(oldRoot);

        if (rootIt == rootMap.end())
            return result;

        result.remappedRootIndex = rootIt->second;

        if (oldRetry == MutationHistoryStore::kRootOnly)
        {
            result.remappedRetryIndex = MutationHistoryStore::kRootOnly;
            return result;
        }

        const auto retryIt = retryMap.find({ oldRoot, oldRetry });

        if (retryIt != retryMap.end())
            result.remappedRetryIndex = retryIt->second;

        return result;
    }

} // namespace

HistoryDefragResult HistoryDefragService::defrag(MutationHistoryStore& store,
                                                 std::pair<int, int> selectedIndices)
{
    HistoryDefragResult failure;
    failure.remappedRetryIndex = MutationHistoryStore::kRootOnly;

    if (store.isEmpty())
        return failure;

    const auto selectedBeforeDefrag = captureSelectedIndices(store,
                                                             selectedIndices.first,
                                                             selectedIndices.second);

    const auto oldRoots = store.getSortedRootIndices();
    std::map<int, int> rootMap;
    std::map<std::pair<int, int>, int> retryMap;
    std::map<int, MutationHistoryStore::RootBucket> newRoots;

    for (int i = 0; i < oldRoots.size(); ++i)
    {
        const int oldRoot = oldRoots[i];
        const int newRoot = i;
        rootMap.emplace(oldRoot, newRoot);

        MutationHistoryStore::RootBucket bucket;
        const auto oldRootEntry = store.getEntry(oldRoot, MutationHistoryStore::kRootOnly);

        if (! oldRootEntry.has_value())
            continue;

        bucket.rootEntry = *oldRootEntry;
        bucket.rootEntry.rootIndex = newRoot;
        bucket.rootEntry.retryIndex = MutationHistoryStore::kRootOnly;
        applyNamedResult(bucket.rootEntry, newRoot, MutationHistoryStore::kRootOnly);
        bucket.hasRootEntry = true;

        const auto oldRetries = store.getSortedRetryIndices(oldRoot);

        for (int j = 0; j < oldRetries.size(); ++j)
        {
            const int oldRetry = oldRetries[j];
            const int newRetry = j;
            retryMap.emplace(std::make_pair(oldRoot, oldRetry), newRetry);

            const auto retryEntry = store.getEntry(oldRoot, oldRetry);

            if (! retryEntry.has_value())
                continue;

            auto entry = *retryEntry;
            entry.rootIndex = newRoot;
            entry.retryIndex = newRetry;
            applyNamedResult(entry, newRoot, newRetry);
            bucket.retries.emplace(newRetry, entry);
        }

        newRoots.emplace(newRoot, std::move(bucket));
    }

    store.replaceRootsForDefrag(std::move(newRoots));
    return remapSelection(selectedBeforeDefrag, rootMap, retryMap);
}

} // namespace Core
