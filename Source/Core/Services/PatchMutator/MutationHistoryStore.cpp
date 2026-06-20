#include "MutationHistoryStore.h"

#include <algorithm>
#include <cstring>

namespace Core
{

namespace
{

    std::optional<int> peekNextIndex(int existingCount, int maxExisting, int maxIndexValue)
    {
        if (existingCount >= maxIndexValue + 1)
            return std::nullopt;

        const int nextIndex = maxExisting + 1;

        if (nextIndex > maxIndexValue)
            return std::nullopt;

        return nextIndex;
    }

} // namespace

void MutationHistoryStore::setInitialSnapshot(const PatchModel& snapshot)
{
    std::memcpy(initialSnapshot_.data(), snapshot.data(), initialSnapshot_.size());
    hasInitialSnapshot_ = true;
}

PatchModel MutationHistoryStore::getInitialSnapshot() const
{
    PatchModel snapshot;
    snapshot.loadFrom(initialSnapshot_.data());
    return snapshot;
}

bool MutationHistoryStore::hasInitialSnapshot() const noexcept
{
    return hasInitialSnapshot_;
}

std::optional<int> MutationHistoryStore::peekNextRootIndex() const noexcept
{
    int maxRoot = -1;

    for (const auto& [index, bucket] : roots_)
    {
        if (bucket.hasRootEntry)
            maxRoot = std::max(maxRoot, index);
    }

    return peekNextIndex(rootCount(), maxRoot, kMaxRoots - 1);
}

std::optional<int> MutationHistoryStore::peekNextRetryIndex(int rootIndex) const noexcept
{
    const auto* bucket = findBucket(rootIndex);

    if (bucket == nullptr || ! bucket->hasRootEntry)
        return std::nullopt;

    int maxRetry = -1;

    for (const auto& [index, entry] : bucket->retries)
    {
        juce::ignoreUnused(entry);
        maxRetry = std::max(maxRetry, index);
    }

    return peekNextIndex(static_cast<int>(bucket->retries.size()),
                         maxRetry,
                         kMaxRetriesPerRoot - 1);
}

bool MutationHistoryStore::isRootSlotsFull() const noexcept
{
    return rootCount() == kMaxRoots;
}

bool MutationHistoryStore::isRetrySlotsFull(int rootIndex) const noexcept
{
    return retryCount(rootIndex) == kMaxRetriesPerRoot;
}

bool MutationHistoryStore::isRootIndexExhausted() const noexcept
{
    return rootCount() < kMaxRoots && ! peekNextRootIndex().has_value();
}

bool MutationHistoryStore::isRetryIndexExhausted(int rootIndex) const noexcept
{
    if (! hasRoot(rootIndex))
        return false;

    return retryCount(rootIndex) < kMaxRetriesPerRoot
           && ! peekNextRetryIndex(rootIndex).has_value();
}

bool MutationHistoryStore::insertRoot(int rootIndex,
                                      const PatchModel& result,
                                      const PatchModel& parentSnapshot)
{
    if (! isValidRootIndex(rootIndex))
        return false;

    if (hasRoot(rootIndex))
        return false;

    if (rootCount() >= kMaxRoots)
        return false;

    auto& bucket = roots_[rootIndex];
    bucket.rootEntry = makeEntry(rootIndex, kRootOnly, result, parentSnapshot);
    bucket.hasRootEntry = true;
    return true;
}

bool MutationHistoryStore::insertRetry(int rootIndex,
                                       int retryIndex,
                                       const PatchModel& result,
                                       const PatchModel& parentSnapshot)
{
    if (! isValidRootIndex(rootIndex) || ! isValidRetryIndex(retryIndex))
        return false;

    auto* bucket = findBucket(rootIndex);

    if (bucket == nullptr || ! bucket->hasRootEntry)
        return false;

    if (bucket->retries.find(retryIndex) != bucket->retries.end())
        return false;

    if (static_cast<int>(bucket->retries.size()) >= kMaxRetriesPerRoot)
        return false;

    bucket->retries.emplace(retryIndex,
                              makeEntry(rootIndex, retryIndex, result, parentSnapshot));
    return true;
}

bool MutationHistoryStore::deleteRoot(int rootIndex)
{
    return roots_.erase(rootIndex) > 0;
}

bool MutationHistoryStore::deleteRetry(int rootIndex, int retryIndex)
{
    auto* bucket = findBucket(rootIndex);

    if (bucket == nullptr)
        return false;

    return bucket->retries.erase(retryIndex) > 0;
}

void MutationHistoryStore::clear() noexcept
{
    roots_.clear();
}

void MutationHistoryStore::replaceRootsForDefrag(std::map<int, RootBucket>&& newRoots) noexcept
{
    roots_ = std::move(newRoots);
}

bool MutationHistoryStore::isEmpty() const noexcept
{
    return roots_.empty();
}

int MutationHistoryStore::rootCount() const noexcept
{
    int count = 0;

    for (const auto& [index, bucket] : roots_)
    {
        juce::ignoreUnused(index);

        if (bucket.hasRootEntry)
            ++count;
    }

    return count;
}

int MutationHistoryStore::retryCount(int rootIndex) const noexcept
{
    const auto* bucket = findBucket(rootIndex);

    if (bucket == nullptr)
        return 0;

    return static_cast<int>(bucket->retries.size());
}

juce::Array<int> MutationHistoryStore::getSortedRootIndices() const
{
    juce::Array<int> indices;

    for (const auto& [index, bucket] : roots_)
    {
        if (bucket.hasRootEntry)
            indices.add(index);
    }

    std::sort(indices.begin(), indices.end());
    return indices;
}

juce::Array<int> MutationHistoryStore::getSortedRetryIndices(int rootIndex) const
{
    juce::Array<int> indices;
    const auto* bucket = findBucket(rootIndex);

    if (bucket == nullptr)
        return indices;

    for (const auto& [index, entry] : bucket->retries)
    {
        juce::ignoreUnused(entry);
        indices.add(index);
    }

    std::sort(indices.begin(), indices.end());
    return indices;
}

std::optional<MutationEntry> MutationHistoryStore::getEntry(int rootIndex, int retryIndex) const
{
    const auto* bucket = findBucket(rootIndex);

    if (bucket == nullptr)
        return std::nullopt;

    if (retryIndex == kRootOnly)
    {
        if (! bucket->hasRootEntry)
            return std::nullopt;

        return bucket->rootEntry;
    }

    const auto retryIt = bucket->retries.find(retryIndex);

    if (retryIt == bucket->retries.end())
        return std::nullopt;

    return retryIt->second;
}

bool MutationHistoryStore::hasRoot(int rootIndex) const noexcept
{
    const auto* bucket = findBucket(rootIndex);
    return bucket != nullptr && bucket->hasRootEntry;
}

bool MutationHistoryStore::hasRetry(int rootIndex, int retryIndex) const noexcept
{
    const auto* bucket = findBucket(rootIndex);

    if (bucket == nullptr)
        return false;

    return bucket->retries.find(retryIndex) != bucket->retries.end();
}

bool MutationHistoryStore::isValidRootIndex(int rootIndex) noexcept
{
    return rootIndex >= 0 && rootIndex < kMaxRoots;
}

bool MutationHistoryStore::isValidRetryIndex(int retryIndex) noexcept
{
    return retryIndex >= 0 && retryIndex < kMaxRetriesPerRoot;
}

MutationEntry MutationHistoryStore::makeEntry(int rootIndex,
                                              int retryIndex,
                                              const PatchModel& result,
                                              const PatchModel& parentSnapshot)
{
    MutationEntry entry;
    entry.rootIndex = rootIndex;
    entry.retryIndex = retryIndex;
    std::memcpy(entry.result.data(), result.data(), entry.result.size());
    std::memcpy(entry.parentSnapshot.data(), parentSnapshot.data(), entry.parentSnapshot.size());
    return entry;
}

const MutationHistoryStore::RootBucket* MutationHistoryStore::findBucket(int rootIndex) const noexcept
{
    const auto it = roots_.find(rootIndex);
    return it != roots_.end() ? &it->second : nullptr;
}

MutationHistoryStore::RootBucket* MutationHistoryStore::findBucket(int rootIndex) noexcept
{
    const auto it = roots_.find(rootIndex);

    if (it == roots_.end())
        return nullptr;

    return &it->second;
}

} // namespace Core
