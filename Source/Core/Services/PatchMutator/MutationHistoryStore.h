#pragma once

#include <array>
#include <map>
#include <optional>

#include <juce_core/juce_core.h>

#include "Core/Models/PatchModel.h"

namespace Core
{

    class HistoryDefragService;

    struct MutationEntry
    {
        int rootIndex = 0;
        int retryIndex = -1; // MutationHistoryStore::kRootOnly
        std::array<juce::uint8, PatchModel::kBufferSize> result {};
        std::array<juce::uint8, PatchModel::kBufferSize> parentSnapshot {};
    };

    // Session-only two-level mutation history (M00–M99 / R00–R99).
    // Message-thread access only — same contract as ClipboardService.
    class MutationHistoryStore
    {
    public:
        static constexpr int kMaxRoots = 100;
        static constexpr int kMaxRetriesPerRoot = 100;
        static constexpr int kRootOnly = -1;

        void setInitialSnapshot(const PatchModel& snapshot);
        PatchModel getInitialSnapshot() const;
        bool hasInitialSnapshot() const noexcept;

        // Export session folder basename frozen at the first MUTATE alongside the initial
        // snapshot. Lifetime is tied to the snapshot: cleared by clearInitialSnapshot().
        void setFrozenExportBasename(const juce::String& basename);
        juce::String getFrozenExportBasename() const;
        bool hasFrozenExportBasename() const noexcept;

        std::optional<int> peekNextRootIndex() const noexcept;
        std::optional<int> peekNextRetryIndex(int rootIndex) const noexcept;
        bool isRootSlotsFull() const noexcept;
        bool isRetrySlotsFull(int rootIndex) const noexcept;
        bool isRootIndexExhausted() const noexcept;
        bool isRetryIndexExhausted(int rootIndex) const noexcept;

        bool insertRoot(int rootIndex, const PatchModel& result, const PatchModel& parentSnapshot);
        bool insertRetry(int rootIndex,
                         int retryIndex,
                         const PatchModel& result,
                         const PatchModel& parentSnapshot);

        bool deleteRoot(int rootIndex);
        bool deleteRetry(int rootIndex, int retryIndex);
        void clear() noexcept;
        void clearInitialSnapshot() noexcept;

        bool isEmpty() const noexcept;
        int rootCount() const noexcept;
        int retryCount(int rootIndex) const noexcept;
        juce::Array<int> getSortedRootIndices() const;
        juce::Array<int> getSortedRetryIndices(int rootIndex) const;
        std::optional<MutationEntry> getEntry(int rootIndex, int retryIndex = kRootOnly) const;
        bool hasRoot(int rootIndex) const noexcept;
        bool hasRetry(int rootIndex, int retryIndex) const noexcept;

    private:
        friend class HistoryDefragService;

        struct RootBucket
        {
            MutationEntry rootEntry {};
            bool hasRootEntry = false;
            std::map<int, MutationEntry> retries;
        };

        void replaceRootsForDefrag(std::map<int, RootBucket>&& newRoots) noexcept;

        static bool isValidRootIndex(int rootIndex) noexcept;
        static bool isValidRetryIndex(int retryIndex) noexcept;
        static MutationEntry makeEntry(int rootIndex,
                                       int retryIndex,
                                       const PatchModel& result,
                                       const PatchModel& parentSnapshot);

        const RootBucket* findBucket(int rootIndex) const noexcept;
        RootBucket* findBucket(int rootIndex) noexcept;

        std::map<int, RootBucket> roots_;
        std::array<juce::uint8, PatchModel::kBufferSize> initialSnapshot_ {};
        bool hasInitialSnapshot_ = false;
        juce::String frozenExportBasename_;
    };

} // namespace Core
