// Settings INIT TEMPLATE / Master UTILITY actions (message-thread).

#include "PluginProcessor.h"
#include "PluginProcessorInternal.h"

#include "Core/Init/InitTemplateFooter.h"
#include "Core/Init/InitTemplateWriter.h"
#include "Core/Init/InitDefaults.h"
#include "Core/Init/MasterModuleInitService.h"
#include "Core/MIDI/MasterParameterSysExDispatcher.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/MasterM1kmCodec.h"
#include "Core/Services/MasterM1kmLoadPolicy.h"
#include "Core/Services/PatchFileService.h"
#include "MIDI/MidiManager.h"
#include "Shared/Definitions/PluginDisplayNames.h"

#include <cstring>

namespace
{
    void publishSettingsFooter(juce::AudioProcessorValueTreeState& apvts,
                               const juce::String& message,
                               bool warning)
    {
        if (message.isEmpty())
            return;

        apvts.state.setProperty("uiMessageText", message, nullptr);
        apvts.state.setProperty("uiMessageSeverity",
                               warning ? juce::String("warning") : juce::String("info"),
                               nullptr);
    }
}

void PluginProcessor::savePatchAsInitTemplate()
{
    if (patchModel_ == nullptr || apvtsPatchMapper_ == nullptr || patchNameSyncer_ == nullptr
        || patchFileService_ == nullptr || midiManager == nullptr)
    {
        publishSettingsFooter(apvts,
                              PluginDisplayNames::Settings::FooterMessages::kInitTemplateWriteFailed,
                              true);
        return;
    }

    apvtsPatchMapper_->apvtsToBuffer();
    patchNameSyncer_->apvtsToBuffer();

    const auto result = Core::InitTemplateWriter::writePatchInit(
        *patchModel_, *patchFileService_, midiManager->getSysExEncoder());
    publishSettingsFooter(apvts, result.infoMessage, ! result.success);
}

void PluginProcessor::saveMasterAsInitTemplate()
{
    if (masterModel_ == nullptr || apvtsMasterMapper_ == nullptr || midiManager == nullptr)
    {
        publishSettingsFooter(apvts,
                              PluginDisplayNames::Settings::FooterMessages::kInitTemplateWriteFailed,
                              true);
        return;
    }

    apvtsMasterMapper_->apvtsToBuffer();

    const auto result = Core::InitTemplateWriter::writeMasterInit(
        *masterModel_, midiManager->getSysExEncoder());
    publishSettingsFooter(apvts, result.infoMessage, ! result.success);
}

void PluginProcessor::deletePatchInitTemplate()
{
    const auto result = Core::InitTemplateWriter::deletePatchInit();
    publishSettingsFooter(apvts, result.infoMessage, ! result.success);
}

void PluginProcessor::deleteMasterInitTemplate()
{
    const auto result = Core::InitTemplateWriter::deleteMasterInit();
    publishSettingsFooter(apvts, result.infoMessage, ! result.success);
}

bool PluginProcessor::patchInitTemplateExists() const
{
    return Core::InitTemplateWriter::patchInitExists();
}

bool PluginProcessor::masterInitTemplateExists() const
{
    return Core::InitTemplateWriter::masterInitExists();
}

void PluginProcessor::initAllMasterModulesFromTemplate()
{
    if (masterModuleInitService_ == nullptr)
    {
        publishSettingsFooter(apvts,
                              PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed,
                              true);
        return;
    }

    cancelMasterEditSysExDebounce();
    const auto result = masterModuleInitService_->initAllModules();
    // bufferToApvts may arm via deferred APVTS→ValueTree sync; flush then cancel so only
    // the intentional dispatchFull inside initAllModules remains.
    PluginProcessorInternal::flushDeferredApvtsParameterSync(apvts);
    cancelMasterEditSysExDebounce();
    Core::InitTemplateFooter::propagateMessage(apvts, result);
}

void PluginProcessor::loadMasterFromUserFile(const juce::File& file)
{
    if (masterModel_ == nullptr || apvtsMasterMapper_ == nullptr || initTemplateLoader_ == nullptr
        || masterParameterSysExDispatcher_ == nullptr)
    {
        publishSettingsFooter(apvts,
                              PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed,
                              true);
        return;
    }

    // .m1km requires an explicit Groups/cascade choice before commit (editor modal).
    if (Core::MasterM1kmCodec::hasExtension(file))
    {
        publishSettingsFooter(apvts,
                              PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed,
                              true);
        return;
    }

    const auto result = Core::InitTemplateWriter::loadMasterFromUserFile(
        *masterModel_, file, *initTemplateLoader_);

    if (! result.success || result.source != Core::InitTemplateSource::kUserFile)
    {
        publishSettingsFooter(apvts,
                              PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed,
                              true);
        return;
    }

    commitMasterUserLoadToApvtsAndSynth();
}

bool PluginProcessor::tryDecodeMasterM1kmUserFile(const juce::File& file, juce::uint8* packedOut172)
{
    if (packedOut172 == nullptr || initTemplateLoader_ == nullptr
        || ! Core::MasterM1kmCodec::hasExtension(file))
    {
        publishSettingsFooter(apvts,
                              PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed,
                              true);
        return false;
    }

    Core::MasterModel scratch;
    const auto result = Core::InitTemplateWriter::decodeMasterUserFile(
        scratch, file, *initTemplateLoader_);

    if (! result.success || result.source != Core::InitTemplateSource::kUserFile)
    {
        publishSettingsFooter(apvts,
                              PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed,
                              true);
        return false;
    }

    std::memcpy(packedOut172, scratch.data(), Core::MasterModel::kBufferSize);
    return true;
}

void PluginProcessor::commitMasterM1kmUserLoad(const juce::uint8* packed172,
                                               Core::MasterM1kmGroupsPolicy policy)
{
    if (packed172 == nullptr || masterModel_ == nullptr || apvtsMasterMapper_ == nullptr
        || masterParameterSysExDispatcher_ == nullptr)
    {
        publishSettingsFooter(apvts,
                              PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed,
                              true);
        return;
    }

    Core::MasterM1kmLoadPolicy::loadPackedIntoModel(
        *masterModel_, packed172, policy, Core::InitDefaults::masterData());
    commitMasterUserLoadToApvtsAndSynth();
}

void PluginProcessor::commitMasterUserLoadToApvtsAndSynth()
{
    cancelMasterEditSysExDebounce();
    apvtsMasterMapper_->bufferToApvts();
    PluginProcessorInternal::flushDeferredApvtsParameterSync(apvts);
    cancelMasterEditSysExDebounce();
    masterParameterSysExDispatcher_->dispatchFull();
    publishSettingsFooter(apvts, PluginDisplayNames::Settings::FooterMessages::kMasterLoaded, false);
}

void PluginProcessor::saveMasterToUserFile(const juce::File& file)
{
    if (masterModel_ == nullptr || apvtsMasterMapper_ == nullptr || midiManager == nullptr)
    {
        publishSettingsFooter(apvts,
                              PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed,
                              true);
        return;
    }

    apvtsMasterMapper_->apvtsToBuffer();

    const auto result = Core::InitTemplateWriter::writeMasterToFile(
        *masterModel_, file, midiManager->getSysExEncoder());
    publishSettingsFooter(apvts, result.infoMessage, ! result.success);
}

bool PluginProcessor::hasMutationHistory() const
{
    return patchMutatorEngine_ != nullptr && patchMutatorEngine_->rootCount() > 0;
}

void PluginProcessor::defragMutationHistory()
{
    if (patchMutatorEngine_ == nullptr)
        return;

    const auto result = patchMutatorEngine_->defragHistory();
    if (result.footerMessage.isNotEmpty())
        publishSettingsFooter(apvts, result.footerMessage, result.footerSeverity == "warning");
}
