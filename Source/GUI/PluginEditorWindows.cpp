// Extracted from PluginEditor.cpp for modular maintenance.
// Overlay window/dialog lifecycle: Settings, About, Master Init confirm, Bank Transfer progress.

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "GUI/About/AboutWindow.h"
#include "GUI/About/AboutPanel.h"
#include "GUI/Dialogs/BankTransferProgressDialog.h"
#include "GUI/Dialogs/EpromTypePromptDialog.h"
#include "GUI/Dialogs/MasterInitConfirmDialog.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/MainComponent.h"
#include "GUI/Panels/MainComponent/FooterPanel/FooterPanel.h"
#include "GUI/Settings/SettingsPanel.h"
#include "GUI/Settings/SettingsWindow.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Core/Services/EpromTypePolicy.h"
#include "Core/MIDI/MidiManager.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginIDs.h"

void PluginEditor::updateSettingsWindowLayout(float uiScale)
{
    if (settingsWindow_ == nullptr)
        return;

    settingsWindow_->setUiScale(uiScale);
    settingsWindow_->setBounds(getLocalBounds());
}

void PluginEditor::updateAboutWindowLayout(float uiScale)
{
    if (aboutWindow_ == nullptr)
        return;

    aboutWindow_->setUiScale(uiScale);
    aboutWindow_->setBounds(getLocalBounds());
}

void PluginEditor::updateMasterInitConfirmDialogLayout(float uiScale)
{
    if (masterInitConfirmDialog_ == nullptr)
        return;

    masterInitConfirmDialog_->setUiScale(uiScale);
    masterInitConfirmDialog_->setBounds(getLocalBounds());
}

void PluginEditor::updateEpromTypePromptDialogLayout(float uiScale)
{
    if (epromTypePromptDialog_ == nullptr)
        return;

    epromTypePromptDialog_->setUiScale(uiScale);
    epromTypePromptDialog_->setBounds(getLocalBounds());
}

void PluginEditor::updateBankTransferProgressDialogLayout(float uiScale)
{
    if (bankTransferProgressDialog_ == nullptr)
        return;

    bankTransferProgressDialog_->setUiScale(uiScale);
    bankTransferProgressDialog_->setBounds(getLocalBounds());
}

bool PluginEditor::isEscapeBlockedByOverlay() const
{
    if (settingsWindow_ != nullptr && settingsWindow_->isVisible())
        return true;
    if (aboutWindow_ != nullptr && aboutWindow_->isVisible())
        return true;
    if (masterInitConfirmDialog_ != nullptr && masterInitConfirmDialog_->isVisible())
        return true;
    if (epromTypePromptDialog_ != nullptr && epromTypePromptDialog_->isVisible())
        return true;
    if (bankTransferProgressDialog_ != nullptr && bankTransferProgressDialog_->isVisible())
        return true;

    return false;
}

SettingsPanel* PluginEditor::getSettingsPanelIfOpen()
{
    if (settingsWindow_ == nullptr || !settingsWindow_->isVisible())
        return nullptr;

    return &settingsWindow_->getSettingsPanel();
}

void PluginEditor::openSettingsWindow()
{
    closeAboutWindow();

    if (settingsWindow_ == nullptr)
    {
        const bool isPluginMode = !pluginProcessor.isStandalone();
        settingsWindow_ = std::make_unique<SettingsWindow>(
            *skin_,
            isPluginMode,
            [this](SettingsPanel& panel)
            {
                panel.registerContextualHelp([this]() -> FooterPanel*
                {
                    return mainComponent_ != nullptr ? &mainComponent_->getFooterPanel()
                                                     : nullptr;
                });
                wireSettingsPanel(panel);
            },
            [this] { closeSettingsWindow(); });
        addChildComponent(*settingsWindow_);
    }
    else
    {
        settingsWindow_->setSkin(*skin_);
    }

    const int baseWidth = layoutDimensions_.editor.width;
    const float uiScale = (baseWidth > 0)
        ? TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth)
        : 1.0f;
    updateSettingsWindowLayout(uiScale);

    settingsWindow_->setVisible(true);
    restoreSettingsPanelFromState(settingsWindow_->getSettingsPanel());
    settingsWindow_->toFront(true);
    settingsWindow_->grabKeyboardFocus();
}

void PluginEditor::closeSettingsWindow()
{
    if (settingsWindow_ != nullptr)
        settingsWindow_->setVisible(false);
}

void PluginEditor::openAboutWindow()
{
    closeSettingsWindow();

    if (aboutWindow_ == nullptr)
    {
        aboutWindow_ = std::make_unique<AboutWindow>(
            *skin_,
            [this] { closeAboutWindow(); });
        aboutWindow_->getAboutPanel().registerContextualHelp([this]() -> FooterPanel*
        {
            return mainComponent_ != nullptr ? &mainComponent_->getFooterPanel() : nullptr;
        });
        addChildComponent(*aboutWindow_);
    }
    else
    {
        aboutWindow_->setSkin(*skin_);
    }

    const int baseWidth = layoutDimensions_.editor.width;
    const float uiScale = (baseWidth > 0)
        ? TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth)
        : 1.0f;
    updateAboutWindowLayout(uiScale);

    aboutWindow_->setVisible(true);
    aboutWindow_->toFront(true);
    aboutWindow_->grabKeyboardFocus();
}

void PluginEditor::closeAboutWindow()
{
    if (aboutWindow_ != nullptr)
        aboutWindow_->setVisible(false);
}

void PluginEditor::openMasterInitConfirmDialog(const juce::String& moduleDisplayName,
                                               std::function<void()> onConfirm)
{
    closeSettingsWindow();
    closeAboutWindow();

    if (masterInitConfirmDialog_ == nullptr)
    {
        masterInitConfirmDialog_ = std::make_unique<MasterInitConfirmDialog>(
            *skin_,
            [this] { closeMasterInitConfirmDialog(); });
        addChildComponent(*masterInitConfirmDialog_);
    }
    else
    {
        masterInitConfirmDialog_->setSkin(*skin_);
    }

    masterInitConfirmDialog_->prepareForShow(moduleDisplayName, std::move(onConfirm));

    const int baseWidth = layoutDimensions_.editor.width;
    const float uiScale = (baseWidth > 0)
        ? TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth)
        : 1.0f;
    updateMasterInitConfirmDialogLayout(uiScale);

    masterInitConfirmDialog_->setVisible(true);
    masterInitConfirmDialog_->toFront(true);
    masterInitConfirmDialog_->grabKeyboardFocus();
}

void PluginEditor::openMasterGlobalInitConfirmDialog(std::function<void()> onConfirm)
{
    closeSettingsWindow();
    closeAboutWindow();

    if (masterInitConfirmDialog_ == nullptr)
    {
        masterInitConfirmDialog_ = std::make_unique<MasterInitConfirmDialog>(
            *skin_,
            [this] { closeMasterInitConfirmDialog(); });
        addChildComponent(*masterInitConfirmDialog_);
    }
    else
    {
        masterInitConfirmDialog_->setSkin(*skin_);
    }

    masterInitConfirmDialog_->prepareForGlobalShow(std::move(onConfirm));

    const int baseWidth = layoutDimensions_.editor.width;
    const float uiScale = (baseWidth > 0)
        ? TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth)
        : 1.0f;
    updateMasterInitConfirmDialogLayout(uiScale);

    masterInitConfirmDialog_->setVisible(true);
    masterInitConfirmDialog_->toFront(true);
    masterInitConfirmDialog_->grabKeyboardFocus();
}

void PluginEditor::closeMasterInitConfirmDialog()
{
    if (masterInitConfirmDialog_ != nullptr)
        masterInitConfirmDialog_->setVisible(false);
}

namespace
{
    int preferredEpromTypeForPrompt(juce::ValueTree& state, MatrixDeviceTypes::Type deviceType)
    {
        const auto family = Core::EpromTypePolicy::deviceFamilyFromType(deviceType);
        const juce::String version = state.getProperty("deviceVersion", juce::String()).toString();
        const int suggested = Core::EpromTypePolicy::suggestFromInquiryVersion(version, family);
        const int stored = Core::EpromTypePolicy::normalize(static_cast<int>(
            state.getProperty(PluginIDs::Settings::kEpromType,
                              PluginIDs::Settings::EpromType::kDefault)));

        return Core::EpromTypePolicy::preferredForPrompt(suggested, stored);
    }

    void markEpromTypePromptFinished(juce::ValueTree& state)
    {
        state.setProperty(PluginIDs::Settings::kEpromTypePromptDone, true, nullptr);
        state.setProperty(PluginIDs::Settings::kEpromTypePromptPending, false, nullptr);
    }
}

void PluginEditor::applyEpromTypePromptSelection(int selectedId)
{
    auto& apvtsState = pluginProcessor.getApvts().state;
    const int normalized = Core::EpromTypePolicy::normalize(selectedId);
    apvtsState.setProperty(PluginIDs::Settings::kEpromType, normalized, nullptr);
    markEpromTypePromptFinished(apvtsState);
    pluginProcessor.getMidiManager().refreshSysExDelayFromSettings();

    if (auto* panel = getSettingsPanelIfOpen())
    {
        panel->setDeviceType(Core::DeviceTypeRegistry::fromApvtsProperty(
            apvtsState.getProperty(MatrixDeviceTypes::kApvtsPropertyName)));
        panel->refreshEpromTypeItems(normalized);
    }
}

void PluginEditor::ensureEpromTypePromptDialog()
{
    if (epromTypePromptDialog_ == nullptr)
    {
        epromTypePromptDialog_ = std::make_unique<EpromTypePromptDialog>(
            *skin_,
            [this] { closeEpromTypePromptDialog(); });
        addChildComponent(*epromTypePromptDialog_);
        return;
    }

    epromTypePromptDialog_->setSkin(*skin_);
}

void PluginEditor::openEpromTypePromptDialog()
{
    auto& state = pluginProcessor.getApvts().state;
    if (static_cast<bool>(state.getProperty(PluginIDs::Settings::kEpromTypePromptDone, false)))
        return;
    if (epromTypePromptDialog_ != nullptr && epromTypePromptDialog_->isVisible())
        return;

    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    const int preferred = preferredEpromTypeForPrompt(state, deviceType);
    const juce::String deviceVersion = state.getProperty("deviceVersion", juce::String()).toString().trim();
    const bool includeFirmwareSuggestionHint = deviceVersion.isNotEmpty()
        && static_cast<bool>(state.getProperty("deviceDetected", false));

    ensureEpromTypePromptDialog();
    epromTypePromptDialog_->prepareForShow({
        .deviceType = deviceType,
        .preferredSelectedId = preferred,
        .includeFirmwareSuggestionHint = includeFirmwareSuggestionHint,
        .onConfirm = [this](int selectedId) { applyEpromTypePromptSelection(selectedId); },
        .onLater =
            [this]
            {
                markEpromTypePromptFinished(pluginProcessor.getApvts().state);
            },
    });

    const int baseWidth = layoutDimensions_.editor.width;
    const float uiScale = (baseWidth > 0)
        ? TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth)
        : 1.0f;
    updateEpromTypePromptDialogLayout(uiScale);

    epromTypePromptDialog_->setVisible(true);
    epromTypePromptDialog_->toFront(true);
    epromTypePromptDialog_->grabKeyboardFocus();
}

void PluginEditor::closeEpromTypePromptDialog()
{
    if (epromTypePromptDialog_ != nullptr)
        epromTypePromptDialog_->setVisible(false);
}

void PluginEditor::showBankTransferProgressDialog(const BankTransferProgressShowRequest& request)
{
    closeSettingsWindow();
    closeAboutWindow();

    if (bankTransferProgressDialog_ == nullptr)
    {
        bankTransferProgressDialog_ = std::make_unique<BankTransferProgressDialog>(*skin_);
        addChildComponent(*bankTransferProgressDialog_);
    }
    else
    {
        bankTransferProgressDialog_->setSkin(*skin_);
    }

    bankTransferProgressDialog_->prepareForShow({
        .title = request.title,
        .message = request.message,
        .detail = request.detail,
        .totalSteps = request.totalSteps,
        .onCancelRequested = request.onCancelRequested,
        .layout = request.layout,
    });

    const int baseWidth = layoutDimensions_.editor.width;
    const float uiScale = (baseWidth > 0)
        ? TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth)
        : 1.0f;
    updateBankTransferProgressDialogLayout(uiScale);

    bankTransferProgressDialog_->setVisible(true);
    bankTransferProgressDialog_->toFront(true);
    bankTransferProgressDialog_->grabKeyboardFocus();
}

void PluginEditor::hideBankTransferProgressDialog()
{
    if (bankTransferProgressDialog_ != nullptr)
        bankTransferProgressDialog_->setVisible(false);
}
