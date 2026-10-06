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
#include "Core/Services/DeviceTypeRegistry.h"
#include "Core/Services/EpromTypePolicy.h"
#include "GUI/Dialogs/EpromTypePromptDialog.h"
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
        {
            applyAudioCatalogToSettings(*audioPage, names, ids, sourceIdToRestore);
            return;
        }
    }

    applyAudioCatalogSelectionOnly(ids, sourceIdToRestore);
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
        if (propertyName != "keyboardFromPortId")
            refreshEpromTypePromptDialogPorts();
        return;
    }

    if (propertyName == PluginIDs::Settings::kEpromTypePromptPending)
    {
        handleEpromTypePromptPendingProperty();
        return;
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
    const bool pending = static_cast<bool>(
        pluginProcessor.getApvts().state.getProperty(
            PluginIDs::Settings::kEpromTypePromptPending, false));
    if (! pending)
        return;

    juce::MessageManager::callAsync(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)]
        {
            if (safeThis != nullptr)
                safeThis->openEpromTypePromptDialog();
        });
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
    if (propertyName != Core::kDeviceMidiUnresponsiveProperty)
        refreshEpromTypePromptDialogSuggestion();
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
    // DEVICE SETUP owns EPROM persistence until CONFIRM; do not rewrite APVTS while open.
    if (coerced != stored && ! assistantOpen)
        state.setProperty(PluginIDs::Settings::kEpromType, coerced, nullptr);

    pluginProcessor.getMidiManager().refreshSysExDelayFromSettings();

    if (auto* panel = getSettingsPanelIfOpen())
    {
        panel->setDeviceType(deviceType);
        panel->refreshEpromTypeItems(coerced);
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
        || visible(epromTypePromptDialog_) || visible(bankTransferProgressDialog_);
}

SettingsPanel* PluginEditor::getSettingsPanelIfOpen()
{
    if (settingsWindow_ == nullptr || !settingsWindow_->isVisible())
        return nullptr;

    return &settingsWindow_->getSettingsPanel();
}
