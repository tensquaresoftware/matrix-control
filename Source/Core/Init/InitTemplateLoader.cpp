#include "Core/Init/InitTemplateLoader.h"

#include "Core/Init/InitDefaults.h"
#include "Core/Loggers/MidiLogger.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/PatchModel.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"

namespace Core
{
    namespace
    {
        bool isUsableTemplatesFolder(const juce::File& templatesFolder)
        {
            return templatesFolder.getFullPathName().isNotEmpty() && templatesFolder.isDirectory();
        }

        InitTemplateLoadResult makeUserFileResult()
        {
            InitTemplateLoadResult result;
            result.source = InitTemplateSource::kUserFile;
            result.fallbackReason = InitTemplateFallbackReason::kNone;
            result.success = true;
            return result;
        }

        bool loadSysExBytes(const juce::File& file, juce::MemoryBlock& sysEx)
        {
            if (file.loadFileAsData(sysEx))
                return true;

            MidiLogger::getInstance().logError("Failed to read " + file.getFullPathName());
            return false;
        }
    }

    InitTemplateLoader::InitTemplateLoader(SysExDecoder& decoder) noexcept
        : decoder_(decoder)
    {
    }

    juce::File InitTemplateLoader::resolvePatchInitFile(const juce::File& templatesFolder)
    {
        return templatesFolder.getChildFile(kPatchInitFileName);
    }

    juce::File InitTemplateLoader::resolveMasterInitFile(const juce::File& templatesFolder)
    {
        return templatesFolder.getChildFile(kMasterInitFileName);
    }

    juce::String InitTemplateLoader::formatFallbackMessage(InitTemplateKind kind,
                                                           InitTemplateFallbackReason reason)
    {
        const auto fileName = kind == InitTemplateKind::kPatch ? kPatchInitFileName : kMasterInitFileName;

        if (reason == InitTemplateFallbackReason::kFileMissing)
            return juce::String(fileName) + " not found — using built-in defaults.";

        return juce::String(fileName) + " is invalid — using built-in defaults.";
    }

    InitTemplateLoadResult InitTemplateLoader::loadPatch(PatchModel& model,
                                                         const juce::File& templatesFolder) const
    {
        if (! isUsableTemplatesFolder(templatesFolder))
        {
            return makeFallbackResult(InitTemplateKind::kPatch,
                                      InitTemplateFallbackReason::kFileMissing,
                                      InitDefaults::patchData(),
                                      model);
        }

        const auto file = resolvePatchInitFile(templatesFolder);

        if (! file.existsAsFile())
        {
            return makeFallbackResult(InitTemplateKind::kPatch,
                                      InitTemplateFallbackReason::kFileMissing,
                                      InitDefaults::patchData(),
                                      model);
        }

        return loadPatchFromFile(model, file);
    }

    InitTemplateLoadResult InitTemplateLoader::loadMaster(MasterModel& model,
                                                          const juce::File& templatesFolder) const
    {
        if (! isUsableTemplatesFolder(templatesFolder))
        {
            return makeFallbackResult(InitTemplateKind::kMaster,
                                      InitTemplateFallbackReason::kFileMissing,
                                      InitDefaults::masterData(),
                                      model);
        }

        const auto file = resolveMasterInitFile(templatesFolder);

        if (! file.existsAsFile())
        {
            return makeFallbackResult(InitTemplateKind::kMaster,
                                      InitTemplateFallbackReason::kFileMissing,
                                      InitDefaults::masterData(),
                                      model);
        }

        return loadMasterFromFile(model, file);
    }

    InitTemplateLoadResult InitTemplateLoader::loadPatchFromFile(PatchModel& model,
                                                                 const juce::File& file) const
    {
        juce::MemoryBlock sysEx;
        if (! loadSysExBytes(file, sysEx))
        {
            return makeFallbackResult(InitTemplateKind::kPatch,
                                      InitTemplateFallbackReason::kFileInvalid,
                                      InitDefaults::patchData(),
                                      model);
        }

        return decodePatchIntoModel(model, file, sysEx);
    }

    InitTemplateLoadResult InitTemplateLoader::decodePatchIntoModel(PatchModel& model,
                                                                    const juce::File& file,
                                                                    const juce::MemoryBlock& sysEx) const
    {
        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        if (! decoder_.decodePatchSysEx(sysEx, packed))
        {
            MidiLogger::getInstance().logError("Invalid patch init SysEx: " + file.getFullPathName());
            return makeFallbackResult(InitTemplateKind::kPatch,
                                      InitTemplateFallbackReason::kFileInvalid,
                                      InitDefaults::patchData(),
                                      model);
        }

        model.loadFrom(packed);
        return makeUserFileResult();
    }

    InitTemplateLoadResult InitTemplateLoader::loadMasterFromFile(MasterModel& model,
                                                                  const juce::File& file) const
    {
        juce::MemoryBlock sysEx;
        if (! loadSysExBytes(file, sysEx))
        {
            return makeFallbackResult(InitTemplateKind::kMaster,
                                      InitTemplateFallbackReason::kFileInvalid,
                                      InitDefaults::masterData(),
                                      model);
        }

        return decodeMasterIntoModel(model, file, sysEx);
    }

    InitTemplateLoadResult InitTemplateLoader::decodeMasterIntoModel(MasterModel& model,
                                                                     const juce::File& file,
                                                                     const juce::MemoryBlock& sysEx) const
    {
        juce::uint8 packed[SysExConstants::kMasterPackedDataSize] = {};
        if (! decoder_.decodeMasterSysEx(sysEx, packed))
        {
            MidiLogger::getInstance().logError("Invalid master init SysEx: " + file.getFullPathName());
            return makeFallbackResult(InitTemplateKind::kMaster,
                                      InitTemplateFallbackReason::kFileInvalid,
                                      InitDefaults::masterData(),
                                      model);
        }

        model.loadFrom(packed);
        return makeUserFileResult();
    }

    InitTemplateLoadResult InitTemplateLoader::makeFallbackResult(InitTemplateKind kind,
                                                                  InitTemplateFallbackReason reason,
                                                                  const juce::uint8* defaults,
                                                                  PatchModel& model)
    {
        model.loadFrom(defaults);

        InitTemplateLoadResult result;
        result.source = InitTemplateSource::kHardcodedFallback;
        result.fallbackReason = reason;
        result.success = true;
        result.infoMessage = formatFallbackMessage(kind, reason);
        return result;
    }

    InitTemplateLoadResult InitTemplateLoader::makeFallbackResult(InitTemplateKind kind,
                                                                  InitTemplateFallbackReason reason,
                                                                  const juce::uint8* defaults,
                                                                  MasterModel& model)
    {
        model.loadFrom(defaults);

        InitTemplateLoadResult result;
        result.source = InitTemplateSource::kHardcodedFallback;
        result.fallbackReason = reason;
        result.success = true;
        result.infoMessage = formatFallbackMessage(kind, reason);
        return result;
    }

} // namespace Core
