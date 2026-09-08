// Extracted from PluginEditor.cpp for modular maintenance.
// Overlay window/dialog lifecycle: Settings, About, Master Init confirm, Bank Transfer progress.

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "GUI/About/AboutWindow.h"
#include "GUI/Dialogs/BankTransferProgressDialog.h"
#include "GUI/Dialogs/MasterInitConfirmDialog.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Settings/SettingsPanel.h"
#include "GUI/Settings/SettingsWindow.h"

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
