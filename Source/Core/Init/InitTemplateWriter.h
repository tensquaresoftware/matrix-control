#pragma once

#include <juce_core/juce_core.h>

#include "Core/Init/InitTemplateLoader.h"

class SysExEncoder;

namespace Core
{

    class MasterModel;
    class PatchFileService;
    class PatchModel;

    struct InitTemplateWriteResult
    {
        bool success = false;
        juce::String infoMessage;
    };

    /// Writes PatchInit.syx / MasterInit.syx under an Init templates folder.
    class InitTemplateWriter
    {
    public:
        static InitTemplateWriteResult writePatchInit(const PatchModel& patchModel,
                                                      PatchFileService& patchFileService,
                                                      SysExEncoder& encoder);
        static InitTemplateWriteResult writePatchInit(const PatchModel& patchModel,
                                                      PatchFileService& patchFileService,
                                                      SysExEncoder& encoder,
                                                      const juce::File& initDirectory);

        static InitTemplateWriteResult writeMasterInit(const MasterModel& masterModel,
                                                       SysExEncoder& encoder);
        static InitTemplateWriteResult writeMasterInit(const MasterModel& masterModel,
                                                       SysExEncoder& encoder,
                                                       const juce::File& initDirectory);

        static InitTemplateWriteResult writeMasterToFile(const MasterModel& masterModel,
                                                        const juce::File& targetFile,
                                                        SysExEncoder& encoder);
        static InitTemplateLoadResult loadMasterFromUserFile(MasterModel& masterModel,
                                                             const juce::File& file,
                                                             InitTemplateLoader& loader);

        static InitTemplateWriteResult deletePatchInit();
        static InitTemplateWriteResult deletePatchInit(const juce::File& initDirectory);
        static InitTemplateWriteResult deleteMasterInit();
        static InitTemplateWriteResult deleteMasterInit(const juce::File& initDirectory);

        static bool patchInitExists();
        static bool patchInitExists(const juce::File& initDirectory);
        static bool masterInitExists();
        static bool masterInitExists(const juce::File& initDirectory);

    private:
        static juce::File resolveWritableInitDirectory();
        static InitTemplateWriteResult makeFailure(const juce::String& message);
        static InitTemplateWriteResult makeSuccess(const juce::String& message);
        static void blankPatchNameBytes(juce::uint8* packedData) noexcept;
        static InitTemplateWriteResult deleteInitFile(const juce::File& target,
                                                      const juce::String& successMessage);
    };

} // namespace Core
