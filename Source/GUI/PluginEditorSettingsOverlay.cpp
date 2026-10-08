// Settings overlay lifecycle (open/close + MIDI / standalone AUDIO page attach).

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "Core/Audio/AudioPassthroughProcessor.h"
#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MidiManager.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/MainComponent.h"
#include "GUI/Panels/MainComponent/FooterPanel/FooterPanel.h"
#include "GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "GUI/Settings/SettingsAudioPage.h"
#include "GUI/Settings/SettingsMidiPage.h"
#include "GUI/Settings/SettingsPanel.h"
#include "GUI/Settings/SettingsShellMetrics.h"
#include "GUI/Settings/SettingsWindow.h"

void PluginEditor::attachStandaloneAudioSettingsPage(SettingsPanel& panel)
{
    auto* deviceManager = Core::StandaloneAudioInputRouter::getAudioDeviceManager();
    if (deviceManager == nullptr)
        return;

    Core::StandaloneAudioInputRouter::enableInputMonitoring();
    panel.attachAudioPage(SettingsAudioPage::Config{
        .skin = skin_,
        .deviceManager = deviceManager,
        .peakLevelProvider = [this]
        {
            return pluginProcessor.getAudioPassthroughProcessor().getPeakLevel();
        }});
    if (auto* audioPage = panel.getAudioPage())
        wireSynthFromComboChange(*audioPage);
}

void PluginEditor::attachMidiSettingsPage(SettingsPanel& panel)
{
    const bool isNewPage = panel.getMidiPage() == nullptr;
    if (isNewPage)
    {
        panel.attachMidiPage(SettingsMidiPage::Config{
            .skin = skin_,
            .isPluginMode = ! pluginProcessor.isStandalone(),
            .activityTrackerProvider = [this]() -> const Core::MidiActivityTracker&
            {
                return pluginProcessor.getMidiActivityTracker();
            },
            .onPortListsRefreshRequested = [this] { refreshMidiPortListsFromOsChange(); }});
    }

    auto* midiPage = panel.getMidiPage();
    if (midiPage == nullptr)
        return;

    if (isNewPage)
        wireMidiPagePortChanges(*midiPage);

    midiPage->populatePortLists(
        pluginProcessor.getMidiManager().getOpenInputDeviceId(),
        pluginProcessor.getMidiManager().getOpenOutputDeviceId(),
        pluginProcessor.getKeyboardFromOpenDeviceId());
    midiPage->selectSynthFromPort(
        pluginProcessor.getApvts().state.getProperty("midiInputPortId", juce::String()).toString());
    midiPage->selectSynthToPort(
        pluginProcessor.getApvts().state.getProperty("midiOutputPortId", juce::String()).toString());
    if (pluginProcessor.isStandalone())
    {
        midiPage->selectKeyboardFromPort(
            pluginProcessor.getApvts().state.getProperty("keyboardFromPortId", juce::String()).toString());
    }
}

void PluginEditor::wireMidiPagePortChanges(SettingsMidiPage& midiPage)
{
    const auto refreshDeviceRow = [this]
    {
        if (auto* panel = getSettingsPanelIfOpen())
            panel->refreshDeviceRow();
    };

    midiPage.getSynthFromCombo().onChange = [this, &midiPage, refreshDeviceRow]
    {
        const auto previousPortId =
            pluginProcessor.getApvts().state.getProperty("midiInputPortId", juce::String()).toString();
        const auto selectedPortId = midiPage.getSelectedSynthFromPortId();

        if (! pluginProcessor.setMidiInputPort(selectedPortId))
        {
            midiPage.selectSynthFromPort(previousPortId);
            if (previousPortId.isNotEmpty())
                pluginProcessor.setMidiInputPort(previousPortId);
        }
        refreshDeviceRow();
    };

    midiPage.getSynthToCombo().onChange = [this, &midiPage, refreshDeviceRow]
    {
        const auto previousPortId =
            pluginProcessor.getApvts().state.getProperty("midiOutputPortId", juce::String()).toString();
        const auto selectedPortId = midiPage.getSelectedSynthToPortId();

        if (! pluginProcessor.setMidiOutputPort(selectedPortId))
        {
            midiPage.selectSynthToPort(previousPortId);
            if (previousPortId.isNotEmpty())
                pluginProcessor.setMidiOutputPort(previousPortId);
        }
        syncPanicFromMidiOutputState();
        refreshDeviceRow();
    };

    auto* keyboardCombo = midiPage.getKeyboardFromCombo();
    if (keyboardCombo == nullptr)
        return;

    keyboardCombo->onChange = [this, &midiPage]
    {
        if (! pluginProcessor.isStandalone())
            return;

        const auto previousPortId =
            pluginProcessor.getApvts().state.getProperty("keyboardFromPortId", juce::String()).toString();
        if (pluginProcessor.setKeyboardFromPort(midiPage.getSelectedKeyboardFromPortId()))
            return;

        midiPage.selectKeyboardFromPort(previousPortId);
        if (previousPortId.isNotEmpty())
            pluginProcessor.setKeyboardFromPort(previousPortId);
    };
}

void PluginEditor::syncPanicFromMidiOutputState()
{
    if (mainComponent_ == nullptr)
        return;

    const auto outputId =
        pluginProcessor.getApvts().state.getProperty("midiOutputPortId", juce::String()).toString();
    mainComponent_->getHeaderPanel().setPanicMidiOutputAvailable(outputId.isNotEmpty());
}

void PluginEditor::openSettingsWindow()
{
    closeGettingStartedWizard();
    closeAboutWindow();
    closeMutatorHistoryDefragConfirmDialog();

    const bool isPluginMode = !pluginProcessor.isStandalone();
    if (settingsWindow_ == nullptr)
    {
        settingsWindow_ = std::make_unique<SettingsWindow>(
            *skin_,
            isPluginMode,
            [this, isPluginMode](SettingsPanel& panel)
            {
                attachMidiSettingsPage(panel);
                if (! isPluginMode)
                    attachStandaloneAudioSettingsPage(panel);
                panel.registerContextualHelp([this]() -> FooterPanel*
                {
                    return mainComponent_ != nullptr ? &mainComponent_->getFooterPanel()
                                                     : nullptr;
                });
                panel.setOnSearchingWindowStarted([this]
                {
                    pluginProcessor.getMidiManager().refreshDeviceInquiryAfterPortSync();
                });
                wireSettingsPanel(panel);
            },
            [this] { closeSettingsWindow(); });
        settingsWindow_->setOnTabChanged([this, isPluginMode](int tabId)
        {
            SettingsShellMetrics::writeLastTab(pluginProcessor.getApvts().state, tabId, isPluginMode);
        });
        addChildComponent(*settingsWindow_);
    }
    else
    {
        settingsWindow_->setSkin(*skin_);
    }

    auto& panel = settingsWindow_->getSettingsPanel();
    attachMidiSettingsPage(panel);
    if (! isPluginMode && panel.getAudioPage() == nullptr)
        attachStandaloneAudioSettingsPage(panel);

    const int baseWidth = layoutDimensions_.editor.width;
    const float uiScale = (baseWidth > 0)
        ? TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth)
        : 1.0f;
    updateSettingsWindowLayout(uiScale);
    showReadySettingsWindow(panel, isPluginMode);
}

void PluginEditor::showReadySettingsWindow(SettingsPanel& panel, bool isPluginMode)
{
    settingsWindow_->setVisible(true);
    restoreSettingsPanelFromState(panel);
    settingsWindow_->setActiveTab(SettingsShellMetrics::readAndCoerceLastTab(
        pluginProcessor.getApvts().state, isPluginMode));
    refreshSettingsLiveDeviceStatus();
    refreshAudioFromCombo();
    settingsWindow_->toFront(true);
    settingsWindow_->grabKeyboardFocus();
}

void PluginEditor::closeSettingsWindow()
{
    closeMasterM1kmLoadChoiceDialog();

    if (settingsWindow_ != nullptr)
    {
        auto& panel = settingsWindow_->getSettingsPanel();
        panel.stopLiveTimers();
        if (auto* midiPage = panel.getMidiPage())
            midiPage->setMonitoringActive(false);
        if (auto* audioPage = panel.getAudioPage())
            audioPage->setMonitoringActive(false);
        settingsWindow_->setVisible(false);
    }

    requestEditorKeyboardFocusIfNeeded();
}
