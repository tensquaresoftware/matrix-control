// Settings INIT TEMPLATE / Master UTILITY actions (message-thread).

#include "PluginProcessor.h"

#include "Core/Init/InitTemplateFooter.h"
#include "Core/Init/InitTemplateWriter.h"
#include "Core/Init/MasterModuleInitService.h"
#include "Core/MIDI/MasterParameterSysExDispatcher.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/PatchFileService.h"
#include "MIDI/MidiManager.h"
#include "Shared/Definitions/PluginDisplayNames.h"

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

    const auto result = masterModuleInitService_->initAllModules();
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

    const auto result = Core::InitTemplateWriter::loadMasterFromUserFile(
        *masterModel_, file, *initTemplateLoader_);

    if (! result.success || result.source != Core::InitTemplateSource::kUserFile)
    {
        publishSettingsFooter(apvts,
                              PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed,
                              true);
        return;
    }

    apvtsMasterMapper_->bufferToApvts();
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
