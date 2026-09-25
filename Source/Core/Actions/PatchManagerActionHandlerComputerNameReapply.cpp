#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/Actions/PatchManagerActionHandlerInternal.h"

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/DirtyPatchTracker.h"
#include "Core/Services/PatchFileNameReconciler.h"
#include "Core/Services/PatchFileService.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

    namespace
    {
        bool allowsComputerNameReapply(bool forceFilename, int settingsPolicy) noexcept
        {
            using namespace PluginIDs::Settings::ComputerPatchesNamesPolicy;
            return forceFilename
                || settingsPolicy == kDisplaySysexNames
                || settingsPolicy == kDisplayFileNames;
        }

        int effectiveComputerNamePolicy(bool forceFilename, int settingsPolicy) noexcept
        {
            using namespace PluginIDs::Settings::ComputerPatchesNamesPolicy;
            return forceFilename ? kDisplayFileNames : settingsPolicy;
        }

        void captureSnapshotIfWasClean(DirtyPatchTracker* tracker,
                                       PatchModel& model,
                                       bool wasDirty)
        {
            if (tracker != nullptr && ! wasDirty)
                tracker->captureSnapshot(model);
        }
    }

    void PatchManagerActionHandler::reapplyComputerPatchDisplayedName()
    {
        if (! editorPatchFromComputerFile_ || ! canExecutePatchLoad() || ! hasUsableKnownSyxPath())
            return;

        using namespace PluginIDs::Settings::ComputerPatchesNamesPolicy;
        const juce::File file(knownSyxFullPath_);
        const bool forceFilename = PatchFileService::hasM1kpExtension(file);
        const int settingsPolicy = static_cast<int>(apvts_.state.getProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy, kDefault));

        // ASK ONCE PER LOAD is load-time only — never prompt from a Settings change.
        // .m1kp keeps filename even when Settings flips back to SysEx / Ask Once.
        if (! allowsComputerNameReapply(forceFilename, settingsPolicy))
            return;

        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        if (! patchFileService_->loadPatchSysExFile(file, packed).success)
            return;

        const bool wasDirty = dirtyPatchTracker_ != nullptr
            && dirtyPatchTracker_->isDirty(*patchModel_);

        PatchFileNameReconciler::reseedFromPackedAndReconcile(
            *patchModel_,
            packed,
            forceFilename ? file.getFileNameWithoutExtension()
                          : PatchManagerActionHandlerInternal::stemForFilenameReconcile(file),
            effectiveComputerNamePolicy(forceFilename, settingsPolicy));

        using namespace PatchManagerActionHandlerInternal;
        pushPatchModelToApvtsWithSuppress(apvts_, hooks_, *apvtsPatchMapper_, patchNameSyncer_);
        captureSnapshotIfWasClean(dirtyPatchTracker_, *patchModel_, wasDirty);
    }

} // namespace Core
