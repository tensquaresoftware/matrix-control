#include "Core/Init/InitTemplateWriter.h"

#include <cstring>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchFileService.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/ProjectPaths.h"

namespace Core
{

    namespace
    {
        constexpr juce::uint8 kMasterSysExVersion = 0x03;
        constexpr juce::uint8 kBlankNamePad = 0x20;
    }

    juce::File InitTemplateWriter::resolveWritableInitDirectory()
    {
        return ProjectPaths::getInitTemplatesDirectory();
    }

    InitTemplateWriteResult InitTemplateWriter::makeFailure(const juce::String& message)
    {
        return { false, message };
    }

    InitTemplateWriteResult InitTemplateWriter::makeSuccess(const juce::String& message)
    {
        return { true, message };
    }

    void InitTemplateWriter::blankPatchNameBytes(juce::uint8* packedData) noexcept
    {
        if (packedData == nullptr)
            return;

        std::memset(packedData, kBlankNamePad, static_cast<size_t>(PatchModel::kNameLength));
    }

    InitTemplateWriteResult InitTemplateWriter::writePatchInit(const PatchModel& patchModel,
                                                              PatchFileService& patchFileService,
                                                              SysExEncoder& encoder)
    {
        return writePatchInit(patchModel, patchFileService, encoder, resolveWritableInitDirectory());
    }

    InitTemplateWriteResult InitTemplateWriter::writePatchInit(const PatchModel& patchModel,
                                                              PatchFileService& patchFileService,
                                                              SysExEncoder& encoder,
                                                              const juce::File& initDirectory)
    {
        if (! initDirectory.isDirectory())
        {
            return makeFailure(PluginDisplayNames::Settings::FooterMessages::kInitTemplateWriteFailed);
        }

        juce::uint8 packed[PatchModel::kBufferSize] = {};
        std::memcpy(packed, patchModel.data(), PatchModel::kBufferSize);
        blankPatchNameBytes(packed);

        const auto target = InitTemplateLoader::resolvePatchInitFile(initDirectory);
        const auto save = patchFileService.savePatchSysExFile(target, packed, encoder);

        if (! save.success)
        {
            return makeFailure(save.errorMessage.isNotEmpty()
                                   ? save.errorMessage
                                   : juce::String(PluginDisplayNames::Settings::FooterMessages::kInitTemplateWriteFailed));
        }

        return makeSuccess(PluginDisplayNames::Settings::FooterMessages::kPatchInitTemplateSaved);
    }

    InitTemplateWriteResult InitTemplateWriter::writeMasterToFile(const MasterModel& masterModel,
                                                                 const juce::File& targetFile,
                                                                 SysExEncoder& encoder)
    {
        if (targetFile.getFullPathName().isEmpty())
            return makeFailure(PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed);

        const auto encoded = encoder.encodeMasterSysEx(kMasterSysExVersion, masterModel.data());
        if (encoded.getSize() == 0)
            return makeFailure(PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed);

        const auto target = targetFile.withFileExtension("syx");
        target.getParentDirectory().createDirectory();

        if (! target.replaceWithData(encoded.getData(), encoded.getSize()))
            return makeFailure(PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed);

        return makeSuccess(PluginDisplayNames::Settings::FooterMessages::kMasterSaved);
    }

    InitTemplateWriteResult InitTemplateWriter::writeMasterInit(const MasterModel& masterModel,
                                                               SysExEncoder& encoder)
    {
        return writeMasterInit(masterModel, encoder, resolveWritableInitDirectory());
    }

    InitTemplateWriteResult InitTemplateWriter::writeMasterInit(const MasterModel& masterModel,
                                                               SysExEncoder& encoder,
                                                               const juce::File& initDirectory)
    {
        if (! initDirectory.isDirectory())
        {
            return makeFailure(PluginDisplayNames::Settings::FooterMessages::kInitTemplateWriteFailed);
        }

        const auto target = InitTemplateLoader::resolveMasterInitFile(initDirectory);
        auto result = writeMasterToFile(masterModel, target, encoder);

        if (result.success)
            result.infoMessage = PluginDisplayNames::Settings::FooterMessages::kMasterInitTemplateSaved;
        else
            result.infoMessage = PluginDisplayNames::Settings::FooterMessages::kInitTemplateWriteFailed;

        return result;
    }

    InitTemplateLoadResult InitTemplateWriter::loadMasterFromUserFile(MasterModel& masterModel,
                                                                     const juce::File& file,
                                                                     InitTemplateLoader& loader)
    {
        MasterModel scratch;
        const auto result = loader.loadMasterFile(scratch, file);

        if (result.source != InitTemplateSource::kUserFile)
        {
            InitTemplateLoadResult failed;
            failed.success = false;
            failed.source = InitTemplateSource::kHardcodedFallback;
            failed.fallbackReason = result.fallbackReason;
            failed.infoMessage = PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed;
            return failed;
        }

        masterModel.loadFrom(scratch.data());
        return result;
    }

    InitTemplateWriteResult InitTemplateWriter::deleteInitFile(const juce::File& target,
                                                              const juce::String& successMessage)
    {
        if (! target.existsAsFile())
        {
            return makeFailure(PluginDisplayNames::Settings::FooterMessages::kInitTemplateDeleteFailed);
        }

        if (! target.deleteFile())
        {
            return makeFailure(PluginDisplayNames::Settings::FooterMessages::kInitTemplateDeleteFailed);
        }

        return makeSuccess(successMessage);
    }

    InitTemplateWriteResult InitTemplateWriter::deletePatchInit()
    {
        return deletePatchInit(resolveWritableInitDirectory());
    }

    InitTemplateWriteResult InitTemplateWriter::deletePatchInit(const juce::File& initDirectory)
    {
        if (! initDirectory.isDirectory())
        {
            return makeFailure(PluginDisplayNames::Settings::FooterMessages::kInitTemplateDeleteFailed);
        }

        return deleteInitFile(InitTemplateLoader::resolvePatchInitFile(initDirectory),
                              PluginDisplayNames::Settings::FooterMessages::kPatchInitTemplateDeleted);
    }

    InitTemplateWriteResult InitTemplateWriter::deleteMasterInit()
    {
        return deleteMasterInit(resolveWritableInitDirectory());
    }

    InitTemplateWriteResult InitTemplateWriter::deleteMasterInit(const juce::File& initDirectory)
    {
        if (! initDirectory.isDirectory())
        {
            return makeFailure(PluginDisplayNames::Settings::FooterMessages::kInitTemplateDeleteFailed);
        }

        return deleteInitFile(InitTemplateLoader::resolveMasterInitFile(initDirectory),
                              PluginDisplayNames::Settings::FooterMessages::kMasterInitTemplateDeleted);
    }

    bool InitTemplateWriter::patchInitExists()
    {
        return patchInitExists(resolveWritableInitDirectory());
    }

    bool InitTemplateWriter::patchInitExists(const juce::File& initDirectory)
    {
        if (! initDirectory.isDirectory())
            return false;

        return InitTemplateLoader::resolvePatchInitFile(initDirectory).existsAsFile();
    }

    bool InitTemplateWriter::masterInitExists()
    {
        return masterInitExists(resolveWritableInitDirectory());
    }

    bool InitTemplateWriter::masterInitExists(const juce::File& initDirectory)
    {
        if (! initDirectory.isDirectory())
            return false;

        return InitTemplateLoader::resolveMasterInitFile(initDirectory).existsAsFile();
    }

} // namespace Core
