#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/PatchFileNameSanitizer.h"
#include "Core/Services/PatchFileService.h"

// Free helpers shared by PatchManagerActionHandler.cpp and its companion .cpp files.
namespace PatchManagerActionHandlerInternal
{
    inline void setPatchLoadSuppressHooks(Core::ActionExecutionHooks& hooks, bool suppress)
    {
        if (hooks.setSuppressPatchSysEx)
            hooks.setSuppressPatchSysEx(suppress);

        if (hooks.setSuppressMatrixModSysEx)
            hooks.setSuppressMatrixModSysEx(suppress);
    }

    inline void flushDeferredApvtsParameterSync(juce::AudioProcessorValueTreeState& apvts)
    {
        // setValueNotifyingHost queues ValueTree sync on APVTS's internal timer; copyState() forces flush (public API).
        (void) apvts.copyState();
    }

    inline void pushPatchModelToApvtsWithSuppress(juce::AudioProcessorValueTreeState& apvts,
                                                  Core::ActionExecutionHooks& hooks,
                                                  Core::ApvtsPatchMapper& mapper,
                                                  Core::PatchNameSyncer* patchNameSyncer)
    {
        setPatchLoadSuppressHooks(hooks, true);
        mapper.bufferToApvts();

        if (patchNameSyncer != nullptr)
            patchNameSyncer->bufferToApvts();

        flushDeferredApvtsParameterSync(apvts);
        setPatchLoadSuppressHooks(hooks, false);
    }

    inline int indexOfFileNameIgnoreCase(const juce::StringArray& names, const juce::String& fileName)
    {
        for (int i = 0; i < names.size(); ++i)
        {
            if (names[i].equalsIgnoreCase(fileName))
                return i;
        }

        return -1;
    }

    inline juce::String savedSyxFileName(const juce::File& targetFile)
    {
        return targetFile.hasFileExtension(Core::PatchFileService::kSyxExtension)
            ? targetFile.getFileName()
            : targetFile.withFileExtension(Core::PatchFileService::kSyxExtension).getFileName();
    }

    // Strip a matching Bank Utility export prefix ("Pxx. {Name}") before FILE NAMES reconcile.
    // Non-matching stems (artistic "P99 - DJ", slot-only "P76", …) stay unchanged.
    inline juce::String stemForFilenameReconcile(const juce::File& file)
    {
        const auto rawStem = file.getFileNameWithoutExtension();
        const auto fromExport = Core::PatchFileNameSanitizer::nameFromBankExportStem(rawStem);
        return fromExport.isNotEmpty() ? fromExport : rawStem;
    }

    // After a Matrix case-fold write (e.g. Patch 71.syx → PATCH 71.syx), case-sensitive
    // volumes can leave the old-cased twin beside the new file. Remove exact-name mismatches
    // that equalIgnoreCase the kept name. On case-insensitive volumes the directory lists a
    // single entry, so this is a no-op and cannot delete the file just written.
    inline void removeCaseFoldTwinSyxFiles(const juce::File& keptSyxFile)
    {
        const auto parent = keptSyxFile.getParentDirectory();
        if (! parent.isDirectory())
            return;

        const auto keptName = keptSyxFile.getFileName();
        for (const auto& entry : juce::RangedDirectoryIterator(
                 parent, false, "*" + juce::String(Core::PatchFileService::kSyxExtension),
                 juce::File::findFiles))
        {
            const auto candidate = entry.getFile();
            const auto name = candidate.getFileName();
            if (name.equalsIgnoreCase(keptName) && name != keptName)
                candidate.deleteFile();
        }
    }
}
