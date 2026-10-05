// Settings overlay lifecycle (open/close + standalone AUDIO page attach).

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "Core/Audio/AudioPassthroughProcessor.h"
#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/MainComponent.h"
#include "GUI/Panels/MainComponent/FooterPanel/FooterPanel.h"
#include "GUI/Settings/SettingsAudioPage.h"
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

void PluginEditor::openSettingsWindow()
{
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
                if (! isPluginMode)
                    attachStandaloneAudioSettingsPage(panel);
                panel.registerContextualHelp([this]() -> FooterPanel*
                {
                    return mainComponent_ != nullptr ? &mainComponent_->getFooterPanel()
                                                     : nullptr;
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
    if (auto* audioPage = panel.getAudioPage())
        audioPage->setMonitoringActive(true);
    refreshAudioFromCombo();
    settingsWindow_->toFront(true);
    settingsWindow_->grabKeyboardFocus();
}

void PluginEditor::closeSettingsWindow()
{
    closeMasterM1kmLoadChoiceDialog();

    if (settingsWindow_ != nullptr)
    {
        if (auto* audioPage = settingsWindow_->getSettingsPanel().getAudioPage())
            audioPage->setMonitoringActive(false);
        settingsWindow_->setVisible(false);
    }
}

