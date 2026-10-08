// Extracted from PluginEditor.cpp for modular maintenance.
// Standalone SYNTH FROM refresh, audio device change listener, and
// APVTS property/redirect sync for MIDI ports and audio-from source id.

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "Core/Audio/AudioPassthroughProcessor.h"
#include "Core/Audio/SceneAudioSafety.h"
#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/Services/DeviceConnectionMachineDefaults.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Core/Services/EpromTypePolicy.h"
#include "GUI/Dialogs/EpromTypePromptDialog.h"
#include "GUI/Dialogs/GettingStartedWizardDialog.h"
#include "GUI/Dialogs/MasterInitConfirmDialog.h"
#include "GUI/Dialogs/MasterM1kmLoadChoiceDialog.h"
#include "GUI/Dialogs/MutatorHistoryDefragConfirmDialog.h"
#include "GUI/Dialogs/BankTransferProgressDialog.h"
#include "GUI/About/AboutWindow.h"
#include "GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "GUI/Settings/SettingsAudioPage.h"
#include "GUI/Settings/SettingsMidiPage.h"
#include "GUI/Settings/SettingsPanel.h"
#include "GUI/Settings/SettingsWindow.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginIDs.h"

void PluginEditor::refreshAudioFromCombo()
{
    if (!pluginProcessor.isStandalone())
        return;

    const auto names = pluginProcessor.getAudioInputSourceNames();
    const auto ids = pluginProcessor.getAudioInputSourceIds();
    const auto sourceIdToRestore = pluginProcessor.getApvts().state.getProperty(
        "audioFromSourceId", juce::String()).toString();

    if (auto* panel = getSettingsPanelIfOpen())
    {
        if (auto* audioPage = panel->getAudioPage())
            applyAudioCatalogToSettings(*audioPage, names, ids, sourceIdToRestore);
        else
            applyAudioCatalogSelectionOnly(ids, sourceIdToRestore);
    }
    else
    {
        applyAudioCatalogSelectionOnly(ids, sourceIdToRestore);
    }

    refreshGettingStartedWizardSynthFrom();
}

void PluginEditor::applyAudioCatalogToSettings(SettingsAudioPage& audioPage,
                                               const juce::StringArray& names,
                                               const juce::StringArray& ids,
                                               juce::String sourceIdToRestore)
{
    const auto currentIdentity = Core::StandaloneAudioInputRouter::getCurrentInputDeviceName();
    const auto boundIdentity = pluginProcessor.getApvts().state.getProperty(
        Core::kAudioFromBoundInputDeviceNameProperty, juce::String()).toString();
    const auto decision = Core::decideAudioFromSelectionSync(
        sourceIdToRestore, boundIdentity, currentIdentity, ids);

    audioPage.populateSynthFromCombo(names, ids);
    audioPage.selectSynthFromSourceId(decision.sourceIdToApply);

    if (decision.shouldDefer)
        return;

    if (decision.selectionKept)
    {
        pluginProcessor.setAudioFromSourceId(decision.sourceIdToApply);
        pluginProcessor.bindAudioFromInputDeviceIdentity(currentIdentity);
        return;
    }

    pluginProcessor.setAudioFromSourceId({});
}

void PluginEditor::applyAudioCatalogSelectionOnly(const juce::StringArray& ids,
                                                  juce::String sourceIdToRestore)
{
    const auto currentIdentity = Core::StandaloneAudioInputRouter::getCurrentInputDeviceName();
    const auto boundIdentity = pluginProcessor.getApvts().state.getProperty(
        Core::kAudioFromBoundInputDeviceNameProperty, juce::String()).toString();
    const auto decision = Core::decideAudioFromSelectionSync(
        sourceIdToRestore, boundIdentity, currentIdentity, ids);

    if (decision.shouldDefer)
        return;

    if (decision.selectionKept)
    {
        pluginProcessor.setAudioFromSourceId(decision.sourceIdToApply);
        pluginProcessor.bindAudioFromInputDeviceIdentity(currentIdentity);
        return;
    }

    pluginProcessor.setAudioFromSourceId({});
}

void PluginEditor::attachStandaloneAudioDeviceListener()
{
    if (!pluginProcessor.isStandalone())
        return;

    Core::StandaloneAudioInputRouter::addAudioDeviceChangeListener(*this);
}

void PluginEditor::detachStandaloneAudioDeviceListener()
{
    Core::StandaloneAudioInputRouter::removeAudioDeviceChangeListener(*this);
}

void PluginEditor::changeListenerCallback(juce::ChangeBroadcaster*)
{
    if (pluginProcessor.isStandalone()
        && Core::StandaloneAudioInputRouter::applyMissingAudioDeviceNonePolicy())
    {
        pluginProcessor.setAudioFromSourceId({});
    }

    refreshAudioFromCombo();

    if (! pluginProcessor.isStandalone())
        return;

    const auto sourceId = pluginProcessor.getApvts().state.getProperty("audioFromSourceId", juce::String()).toString();
    if (sourceId.isNotEmpty())
        Core::StandaloneAudioInputRouter::enableInputMonitoring();
}

void PluginEditor::valueTreePropertyChanged(juce::ValueTree&,
                                            const juce::Identifier& property)
{
    const auto propertyName = property.toString();

    if (propertyName == "midiInputPortId" || propertyName == "midiOutputPortId"
        || propertyName == "keyboardFromPortId")
    {
        syncMidiPortSelectionFromState(propertyName);
        if (propertyName == "keyboardFromPortId")
            refreshGettingStartedWizardKeyboardFrom();
        else
        {
            refreshEpromTypePromptDialogPorts();
            refreshGettingStartedWizardPorts();
        }
        return;
    }

    if (propertyName == PluginIDs::Settings::kEpromTypePromptPending)
    {
        handleEpromTypePromptPendingProperty();
        return;
    }

    if (propertyName == PluginIDs::Settings::kEpromType)
    {
        const int epromType = Core::EpromTypePolicy::normalize(static_cast<int>(
            pluginProcessor.getApvts().state.getProperty(
                PluginIDs::Settings::kEpromType,
                PluginIDs::Settings::EpromType::kDefault)));
        Core::DeviceConnectionMachineDefaults::writeEpromType(epromType);
        pluginProcessor.getMidiManager().refreshSysExDelayFromSettings();
    }

    handleDeviceSetupAssistantProperty(propertyName);

    if (propertyName == MatrixDeviceTypes::kApvtsPropertyName)
        coerceEpromTypeForCurrentDeviceFamily();

    if (! pluginProcessor.isStandalone())
        return;

    scheduleAudioFromRefreshIfNeeded(propertyName);
}

void PluginEditor::handleEpromTypePromptPendingProperty()
{
    // GS-3 absorb: inquiry must not reopen Device Setup. Clear the legacy pending flag.
    const bool pending = static_cast<bool>(
        pluginProcessor.getApvts().state.getProperty(
            PluginIDs::Settings::kEpromTypePromptPending, false));
    if (! pending)
        return;

    pluginProcessor.getApvts().state.setProperty(
        PluginIDs::Settings::kEpromTypePromptPending, false, nullptr);
}

void PluginEditor::handleDeviceSetupAssistantProperty(const juce::String& propertyName)
{
    const bool isDeviceStatus = propertyName == "deviceDetected"
        || propertyName == "deviceVersion"
        || propertyName == MatrixDeviceTypes::kApvtsPropertyName
        || propertyName == Core::kDeviceMidiUnresponsiveProperty;
    if (! isDeviceStatus)
        return;

    refreshEpromTypePromptDialogLiveState();
    refreshGettingStartedWizardLiveState();
    if (propertyName != Core::kDeviceMidiUnresponsiveProperty)
    {
        refreshEpromTypePromptDialogSuggestion();
        refreshGettingStartedWizardSuggestion();
    }

    refreshSettingsLiveDeviceStatus();

    if (auto* panel = getSettingsPanelIfOpen())
    {
        auto& state = pluginProcessor.getApvts().state;
        const int epromType = Core::EpromTypePolicy::normalize(static_cast<int>(
            state.getProperty(PluginIDs::Settings::kEpromType,
                              PluginIDs::Settings::EpromType::kDefault)));
        panel->refreshEpromTypeItems(
            epromType, PluginEditorInternal::epromInquiryPopupMarkerFromState(state));
    }
}

void PluginEditor::coerceEpromTypeForCurrentDeviceFamily()
{
    auto& state = pluginProcessor.getApvts().state;
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    const auto family = Core::EpromTypePolicy::deviceFamilyFromType(deviceType);
    const int stored = Core::EpromTypePolicy::normalize(static_cast<int>(
        state.getProperty(PluginIDs::Settings::kEpromType,
                          PluginIDs::Settings::EpromType::kDefault)));
    const int coerced = Core::EpromTypePolicy::coerceForDeviceFamily(stored, family);
    const bool assistantOpen = epromTypePromptDialog_ != nullptr
        && epromTypePromptDialog_->isVisible();
    const bool wizardOpen = gettingStartedWizardDialog_ != nullptr
        && gettingStartedWizardDialog_->isVisible();
    const bool wizardSynthOpen = wizardOpen
        && gettingStartedWizardDialog_->getCurrentStep()
            == GettingStartedWizard::Step::kSynthCommunication;
    // Live STEP 2 / Device Setup owns EPROM until the user moves on; do not rewrite while open.
    if (coerced != stored && ! assistantOpen && ! wizardSynthOpen)
    {
        state.setProperty(PluginIDs::Settings::kEpromType, coerced, nullptr);
        // Keep the (possibly hidden) STEP 2 combo aligned when family changes on another step.
        if (wizardOpen)
            refreshGettingStartedWizardSuggestion();
    }

    pluginProcessor.getMidiManager().refreshSysExDelayFromSettings();

    if (auto* panel = getSettingsPanelIfOpen())
    {
        panel->setDeviceType(deviceType);
        panel->refreshEpromTypeItems(
            coerced, PluginEditorInternal::epromInquiryPopupMarkerFromState(state));
    }
}

void PluginEditor::syncMidiPortSelectionFromState(const juce::String& propertyName)
{
    juce::MessageManager::callAsync(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this), propertyName]
        {
            if (safeThis == nullptr)
                return;

            auto& state = safeThis->pluginProcessor.getApvts().state;
            if (auto* panel = safeThis->getSettingsPanelIfOpen())
            {
                if (auto* midiPage = panel->getMidiPage())
                {
                    if (propertyName == "midiInputPortId")
                    {
                        midiPage->selectSynthFromPort(
                            state.getProperty("midiInputPortId", juce::String()).toString());
                    }
                    else if (propertyName == "midiOutputPortId")
                    {
                        midiPage->selectSynthToPort(
                            state.getProperty("midiOutputPortId", juce::String()).toString());
                    }
                    else if (safeThis->pluginProcessor.isStandalone())
                    {
                        midiPage->selectKeyboardFromPort(
                            state.getProperty("keyboardFromPortId", juce::String()).toString());
                    }
                }

                if (propertyName == "midiInputPortId" || propertyName == "midiOutputPortId")
                    panel->refreshDeviceRow();
            }

            if (propertyName == "midiOutputPortId")
                safeThis->syncPanicFromMidiOutputState();
        });
}

void PluginEditor::scheduleAudioFromRefreshIfNeeded(const juce::String& propertyName)
{
    if (propertyName != MatrixDeviceTypes::kApvtsPropertyName && propertyName != "deviceDetected")
        return;

    juce::MessageManager::callAsync(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)]
        {
            if (safeThis != nullptr)
                safeThis->refreshAudioFromCombo();
        });
}

void PluginEditor::valueTreeRedirected(juce::ValueTree&)
{
    if (! pluginProcessor.isStandalone())
        return;

    juce::MessageManager::callAsync(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)]
        {
            if (safeThis != nullptr)
                safeThis->refreshAudioFromCombo();
        });
}

bool PluginEditor::isEscapeBlockedByOverlay() const
{
    const auto visible = [](const auto& c) { return c != nullptr && c->isVisible(); };
    return visible(settingsWindow_) || visible(aboutWindow_)
        || visible(masterInitConfirmDialog_) || isMasterM1kmLoadChoiceDialogVisible()
        || visible(mutatorHistoryDefragConfirmDialog_)
        || visible(epromTypePromptDialog_) || visible(gettingStartedWizardDialog_)
        || visible(bankTransferProgressDialog_);
}

SettingsPanel* PluginEditor::getSettingsPanelIfOpen()
{
    if (settingsWindow_ == nullptr || !settingsWindow_->isVisible())
        return nullptr;

    return &settingsWindow_->getSettingsPanel();
}

void PluginEditor::refreshSettingsLiveDeviceStatus()
{
    auto* panel = getSettingsPanelIfOpen();
    if (panel == nullptr)
        return;

    panel->updateLiveDeviceStatus(PluginEditorInternal::settingsLiveDeviceStatusFromState(
        pluginProcessor.getApvts().state));
}
