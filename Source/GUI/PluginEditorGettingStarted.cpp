// GETTING STARTED wizard overlay lifecycle (open / close / layout), mirroring Settings and Device Setup.

#include "PluginEditor.h"

#include "GUI/Dialogs/GettingStartedWizardDialog.h"
#include "GUI/Layout/ScaledLayout.h"

void PluginEditor::updateGettingStartedWizardLayout(float uiScale)
{
    if (gettingStartedWizardDialog_ == nullptr)
        return;

    gettingStartedWizardDialog_->setUiScale(uiScale);
    gettingStartedWizardDialog_->setBounds(getLocalBounds());
}

void PluginEditor::openGettingStartedWizard(GettingStartedWizard::Step startStep)
{
    closeSettingsWindow();
    closeAboutWindow();
    closeMasterInitConfirmDialog();
    closeMasterM1kmLoadChoiceDialog();
    closeMutatorHistoryDefragConfirmDialog();
    closeEpromTypePromptDialog();
    hideBankTransferProgressDialog();

    if (gettingStartedWizardDialog_ == nullptr)
    {
        gettingStartedWizardDialog_ = std::make_unique<GettingStartedWizardDialog>(
            *skin_,
            ! pluginProcessor.isStandalone(),
            [this] { closeGettingStartedWizard(); });
        addChildComponent(*gettingStartedWizardDialog_);
    }
    else
    {
        gettingStartedWizardDialog_->setSkin(*skin_);
    }

    gettingStartedWizardDialog_->prepareForShow(startStep);

    const int baseWidth = layoutDimensions_.editor.width;
    const float uiScale = (baseWidth > 0)
        ? TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth)
        : 1.0f;
    updateGettingStartedWizardLayout(uiScale);

    gettingStartedWizardDialog_->setVisible(true);
    gettingStartedWizardDialog_->toFront(true);
    gettingStartedWizardDialog_->grabKeyboardFocus();
}

void PluginEditor::closeGettingStartedWizard()
{
    if (gettingStartedWizardDialog_ != nullptr)
        gettingStartedWizardDialog_->setVisible(false);

    requestEditorKeyboardFocusIfNeeded();
}
