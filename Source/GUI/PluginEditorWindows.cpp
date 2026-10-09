// Extracted from PluginEditor.cpp for modular maintenance.
// Overlay window/dialog lifecycle: Settings, About, Master Init confirm, Bank Transfer progress.

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "GUI/About/AboutWindow.h"
#include "GUI/About/AboutPanel.h"
#include "GUI/Dialogs/BankTransferProgressDialog.h"
#include "GUI/Dialogs/EpromTypePromptDialog.h"
#include "GUI/Dialogs/GettingStartedWizardDialog.h"
#include "GUI/Dialogs/MasterInitConfirmDialog.h"
#include "GUI/Dialogs/MutatorHistoryDefragConfirmDialog.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/MainComponent.h"
#include "GUI/Panels/MainComponent/FooterPanel/FooterPanel.h"
#include "GUI/Settings/SettingsPanel.h"
#include "GUI/Settings/SettingsWindow.h"
#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/Services/DeviceConnectionMachineDefaults.h"
#include "Core/Services/DeviceSetupDeviceRow.h"
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

void PluginEditor::openAboutWindow()
{
    closeGettingStartedWizard();
    closeSettingsWindow();
    closeMutatorHistoryDefragConfirmDialog();

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
    closeMasterM1kmLoadChoiceDialog();

    if (aboutWindow_ != nullptr)
        aboutWindow_->setVisible(false);

    requestEditorKeyboardFocusIfNeeded();
}

void PluginEditor::openMasterInitConfirmDialog(const juce::String& moduleDisplayName,
                                               std::function<void()> onConfirm)
{
    closeGettingStartedWizard();
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
    closeGettingStartedWizard();
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

    requestEditorKeyboardFocusIfNeeded();
}

namespace
{
    EpromTypePromptDialog::LiveDeviceStatus makeEpromTypePromptLiveStatus(
        const juce::ValueTree& state)
    {
        return {
            .deviceDetected = static_cast<bool>(state.getProperty("deviceDetected", false)),
            .deviceMidiUnresponsive = static_cast<bool>(
                state.getProperty(Core::kDeviceMidiUnresponsiveProperty, false)),
            .deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
                state.getProperty(MatrixDeviceTypes::kApvtsPropertyName)),
            .deviceVersion = state.getProperty("deviceVersion", juce::String()).toString(),
        };
    }

    int preferredEpromTypeForPrompt(juce::ValueTree& state, MatrixDeviceTypes::Type deviceType)
    {
        const auto family = Core::EpromTypePolicy::deviceFamilyFromType(deviceType);
        return Core::EpromTypePolicy::preferredForPrompt(
            Core::EpromTypePolicy::suggestFromInquiryVersion(
                state.getProperty("deviceVersion", juce::String()).toString(), family),
            Core::EpromTypePolicy::normalize(static_cast<int>(
                state.getProperty(PluginIDs::Settings::kEpromType,
                                  PluginIDs::Settings::EpromType::kDefault))));
    }
}

void PluginEditor::applyEpromTypePromptSelection(int selectedId)
{
    auto& apvtsState = pluginProcessor.getApvts().state;
    const auto result = Core::deviceSetupConfirmResult(selectedId);
    apvtsState.setProperty(PluginIDs::Settings::kEpromType, result.epromTypeId, nullptr);
    apvtsState.setProperty(PluginIDs::Settings::kEpromTypePromptDone, result.promptDone, nullptr);
    apvtsState.setProperty(PluginIDs::Settings::kEpromTypePromptPending, result.promptPending, nullptr);
    Core::DeviceConnectionMachineDefaults::writeAfterConfirmFromState(apvtsState, result.epromTypeId);
    pluginProcessor.getMidiManager().refreshSysExDelayFromSettings();

    if (auto* panel = getSettingsPanelIfOpen())
    {
        panel->setDeviceType(Core::DeviceTypeRegistry::fromApvtsProperty(
            apvtsState.getProperty(MatrixDeviceTypes::kApvtsPropertyName)));
        panel->refreshEpromTypeItems(
            result.epromTypeId,
            PluginEditorInternal::epromInquiryPopupMarkerFromState(apvtsState));
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

void PluginEditor::refreshEpromTypePromptDialogLiveState()
{
    if (epromTypePromptDialog_ == nullptr || ! epromTypePromptDialog_->isVisible())
        return;

    epromTypePromptDialog_->updateLiveDeviceStatus(
        makeEpromTypePromptLiveStatus(pluginProcessor.getApvts().state));
}

void PluginEditor::refreshEpromTypePromptDialogPorts()
{
    if (epromTypePromptDialog_ == nullptr || ! epromTypePromptDialog_->isVisible())
        return;

    auto& state = pluginProcessor.getApvts().state;
    epromTypePromptDialog_->syncPortsFromHost(
        state.getProperty("midiInputPortId", juce::String()).toString(),
        state.getProperty("midiOutputPortId", juce::String()).toString(),
        true);
}

void PluginEditor::refreshEpromTypePromptDialogSuggestion()
{
    if (epromTypePromptDialog_ == nullptr || ! epromTypePromptDialog_->isVisible())
        return;

    auto& state = pluginProcessor.getApvts().state;
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    epromTypePromptDialog_->refreshEpromSuggestion(
        deviceType, preferredEpromTypeForPrompt(state, deviceType));
}

void PluginEditor::applyEpromTypePromptMidiPortChange(bool isInput, const juce::String& portId)
{
    auto& state = pluginProcessor.getApvts().state;
    const juce::String key = isInput ? "midiInputPortId" : "midiOutputPortId";
    const auto previousPortId = state.getProperty(key, juce::String()).toString();
    const bool opened = isInput ? pluginProcessor.setMidiInputPort(portId)
                                : pluginProcessor.setMidiOutputPort(portId);
    if (opened)
        return;

    const auto fromId = isInput ? previousPortId
                                : state.getProperty("midiInputPortId", juce::String()).toString();
    const auto toId = isInput ? state.getProperty("midiOutputPortId", juce::String()).toString()
                              : previousPortId;
    if (epromTypePromptDialog_ != nullptr)
        epromTypePromptDialog_->syncPortsFromHost(fromId, toId, false);
    if (gettingStartedWizardDialog_ != nullptr && gettingStartedWizardDialog_->isVisible())
        gettingStartedWizardDialog_->syncPortsFromHost(fromId, toId, false);
    if (previousPortId.isEmpty())
        return;

    if (isInput)
        pluginProcessor.setMidiInputPort(previousPortId);
    else
        pluginProcessor.setMidiOutputPort(previousPortId);
}

void PluginEditor::applyEpromTypePromptSpecifyLater()
{
    auto& state = pluginProcessor.getApvts().state;
    const auto specifyLaterFlags = Core::deviceSetupSpecifyLaterFlags();
    state.setProperty(PluginIDs::Settings::kEpromTypePromptDone, specifyLaterFlags.promptDone, nullptr);
    state.setProperty(PluginIDs::Settings::kEpromTypePromptPending, specifyLaterFlags.promptPending, nullptr);
    Core::DeviceConnectionMachineDefaults::writeAfterSpecifyLaterFromState(state);
}

void PluginEditor::openEpromTypePromptDialog()
{
    // GS-3: Device Setup onboarding is absorbed into GETTING STARTED STEP 2.
    // Keep dialog sources/helpers; do not show Device Setup as a first-run vehicle.
}

void PluginEditor::closeEpromTypePromptDialog()
{
    if (epromTypePromptDialog_ != nullptr)
        epromTypePromptDialog_->setVisible(false);

    requestEditorKeyboardFocusIfNeeded();
}

void PluginEditor::showBankTransferProgressDialog(const BankTransferProgressShowRequest& request)
{
    closeGettingStartedWizard();
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

    requestEditorKeyboardFocusIfNeeded();
}
