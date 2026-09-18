// Extracted from PluginEditor.cpp for modular maintenance.
// Standalone audio-from-source combo refresh, audio device change listener, and
// APVTS property/redirect sync for MIDI ports and audio-from source id.

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Core/Services/EpromTypePolicy.h"
#include "GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "GUI/Settings/SettingsPanel.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginIDs.h"

void PluginEditor::refreshAudioFromCombo(HeaderPanel* headerOverride)
{
    if (!pluginProcessor.isStandalone())
        return;

    const auto names = pluginProcessor.getAudioInputSourceNames();
    const auto ids = pluginProcessor.getAudioInputSourceIds();

    auto* header = headerOverride;

    if (header == nullptr && mainComponent_ != nullptr)
        header = &mainComponent_->getHeaderPanel();

    const auto sourceIdToRestore = pluginProcessor.getApvts().state.getProperty(
        "audioFromSourceId", juce::String()).toString();

    if (header != nullptr)
    {
        applyAudioCatalogToHeader(*header, names, ids, sourceIdToRestore);
        return;
    }

    applyAudioCatalogWithoutHeader(ids, sourceIdToRestore);
}

void PluginEditor::applyAudioCatalogToHeader(HeaderPanel& header,
                                             const juce::StringArray& names,
                                             const juce::StringArray& ids,
                                             juce::String sourceIdToRestore)
{
    if (sourceIdToRestore.isEmpty())
        sourceIdToRestore = header.getSelectedAudioFromSourceId();

    header.populateAudioFromCombo(names, ids);

    if (sourceIdToRestore.isNotEmpty() && ids.contains(sourceIdToRestore))
    {
        header.selectAudioFromSourceId(sourceIdToRestore);
        pluginProcessor.setAudioFromSourceId(sourceIdToRestore);
        return;
    }

    if (sourceIdToRestore.isEmpty())
    {
        const auto effectiveSourceId = header.getSelectedAudioFromSourceId();

        if (effectiveSourceId.isNotEmpty())
        {
            header.selectAudioFromSourceId(effectiveSourceId);
            pluginProcessor.setAudioFromSourceId(effectiveSourceId);
        }
        else if (! ids.isEmpty())
        {
            header.selectAudioFromSourceId(ids[0]);
            pluginProcessor.setAudioFromSourceId(ids[0]);
        }
        else
        {
            header.selectAudioFromSourceId({});
        }
    }
    else
    {
        // Stale id not in catalog: keep APVTS preference; avoid writing combo fallback of the wrong kind.
        header.selectAudioFromSourceId(sourceIdToRestore);
    }
}

void PluginEditor::applyAudioCatalogWithoutHeader(const juce::StringArray& ids, juce::String sourceIdToRestore)
{
    if (sourceIdToRestore.isEmpty() && ! ids.isEmpty())
        sourceIdToRestore = ids[0];

    if (sourceIdToRestore.isNotEmpty() && (ids.isEmpty() || ids.contains(sourceIdToRestore)))
        pluginProcessor.setAudioFromSourceId(sourceIdToRestore);
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
    if (coerced != stored)
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
            if (safeThis == nullptr || safeThis->mainComponent_ == nullptr)
                return;

            auto& header = safeThis->mainComponent_->getHeaderPanel();
            auto& state = safeThis->pluginProcessor.getApvts().state;

            if (propertyName == "midiInputPortId")
                header.selectMidiFromPort(state.getProperty("midiInputPortId", juce::String()).toString());
            else if (propertyName == "midiOutputPortId")
                header.selectMidiToPort(state.getProperty("midiOutputPortId", juce::String()).toString());
            else if (safeThis->pluginProcessor.isStandalone())
                header.selectKeyboardFromPort(
                    state.getProperty("keyboardFromPortId", juce::String()).toString());
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
