#pragma once

#include <juce_core/juce_core.h>

class SysExDecoder;

namespace Core
{

    class MasterModel;
    class PatchModel;

    enum class InitTemplateKind { kPatch, kMaster };

    enum class InitTemplateSource { kUserFile, kHardcodedFallback };

    enum class InitTemplateFallbackReason { kNone, kFileMissing, kFileInvalid };

    struct InitTemplateLoadResult
    {
        InitTemplateSource source = InitTemplateSource::kHardcodedFallback;
        InitTemplateFallbackReason fallbackReason = InitTemplateFallbackReason::kNone;
        bool success = false;
        juce::String infoMessage;
    };

    class InitTemplateLoader
    {
    public:
        static constexpr const char* kPatchInitFileName = "PatchInit.syx";
        static constexpr const char* kMasterInitFileName = "MasterInit.syx";

        explicit InitTemplateLoader(SysExDecoder& decoder) noexcept;

        InitTemplateLoadResult loadPatch(PatchModel& model, const juce::File& templatesFolder) const;
        InitTemplateLoadResult loadMaster(MasterModel& model, const juce::File& templatesFolder) const;

        static juce::File resolvePatchInitFile(const juce::File& templatesFolder);
        static juce::File resolveMasterInitFile(const juce::File& templatesFolder);
        static juce::String formatFallbackMessage(InitTemplateKind kind, InitTemplateFallbackReason reason);

    private:
        InitTemplateLoadResult loadPatchFromFile(PatchModel& model, const juce::File& file) const;
        InitTemplateLoadResult loadMasterFromFile(MasterModel& model, const juce::File& file) const;
        InitTemplateLoadResult decodePatchIntoModel(PatchModel& model, const juce::File& file,
                                                    const juce::MemoryBlock& sysEx) const;
        InitTemplateLoadResult decodeMasterIntoModel(MasterModel& model, const juce::File& file,
                                                     const juce::MemoryBlock& sysEx) const;

        static InitTemplateLoadResult makeFallbackResult(InitTemplateKind kind,
                                                         InitTemplateFallbackReason reason,
                                                         const juce::uint8* defaults,
                                                         PatchModel& model);
        static InitTemplateLoadResult makeFallbackResult(InitTemplateKind kind,
                                                         InitTemplateFallbackReason reason,
                                                         const juce::uint8* defaults,
                                                         MasterModel& model);

        SysExDecoder& decoder_;
    };

} // namespace Core
