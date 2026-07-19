#pragma once

#include "Core/Models/PatchModel.h"

namespace Core
{
    class ApvtsPatchMapper;
    class PatchNameSyncer;

    // Tracks whether the live PATCH packed buffer diverged from the last
    // loaded or synced snapshot (FR-51 foundation). Message-thread access only —
    // same contract as ClipboardService.
    class DirtyPatchTracker
    {
    public:
        DirtyPatchTracker() = default;

        void captureSnapshot(const PatchModel& model);
        void clearSnapshot() noexcept;
        bool hasSnapshot() const noexcept;

        // Full 134-byte compare including name bytes 0–7.
        // Caller must pass a PatchModel already synced from APVTS (mapper +
        // PatchNameSyncer), or use syncApvtsAndIsDirty. Raw isDirty on a stale
        // model can report false-clean after APVTS-only edits.
        bool isDirty(const PatchModel& current) const;

        // Message thread only. Overwrites `model` from APVTS via mapper + name
        // syncer, then compares against the stored snapshot. Model-only edits
        // that were never pushed to APVTS are discarded by the sync.
        bool syncApvtsAndIsDirty(ApvtsPatchMapper& mapper,
                                 PatchNameSyncer& nameSyncer,
                                 PatchModel& model);

    private:
        PatchModel snapshot_ {};
        bool hasSnapshot_ = false;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DirtyPatchTracker)
    };

} // namespace Core
