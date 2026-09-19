// Extracted from PluginEditor.cpp for modular maintenance.
// Skin variant + UI scale application, and JUCE_DEBUG UI test-elements helpers.

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "GUI/About/AboutWindow.h"
#include "GUI/Dialogs/BankTransferProgressDialog.h"
#include "GUI/Dialogs/EpromTypePromptDialog.h"
#include "GUI/Dialogs/MasterInitConfirmDialog.h"
#include "GUI/Dialogs/MutatorHistoryDefragConfirmDialog.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "GUI/Settings/SettingsWindow.h"
#include "Shared/Definitions/PluginIDs.h"

int PluginEditorInternal::matchingScaleIdForUiScale(float uiScale)
{
    const int layoutPercentRounded = juce::roundToInt(uiScale * 100.0f);
    for (int id = PluginIDs::Settings::ScaleLevels::kMin; id <= PluginIDs::Settings::ScaleLevels::kMax; ++id)
    {
        const int presetPercentRounded = juce::roundToInt(
            PluginIDs::Settings::ScaleLevels::getUiScale(id) * 100.0f);
        if (layoutPercentRounded == presetPercentRounded)
            return id;
    }

    return 0;
}

void PluginEditor::updateSkin()
{
    if (auto* widget = mainComponent_.get())
        widget->setSkin(*skin_);

    if (settingsWindow_ != nullptr)
        settingsWindow_->setSkin(*skin_);

    if (aboutWindow_ != nullptr)
        aboutWindow_->setSkin(*skin_);

    if (bankTransferProgressDialog_ != nullptr)
        bankTransferProgressDialog_->setSkin(*skin_);

    if (masterInitConfirmDialog_ != nullptr)
        masterInitConfirmDialog_->setSkin(*skin_);

    if (mutatorHistoryDefragConfirmDialog_ != nullptr)
        mutatorHistoryDefragConfirmDialog_->setSkin(*skin_);

    if (epromTypePromptDialog_ != nullptr)
        epromTypePromptDialog_->setSkin(*skin_);

#if JUCE_DEBUG
    if (testComponent_ != nullptr)
        testComponent_->setSkin(*skin_);
#endif

    repaint();
}

void PluginEditor::applyUiScale(float uiScale)
{
    appliedUiScale_ = uiScale;

    const int targetWidth = juce::roundToInt(static_cast<float>(layoutDimensions_.editor.width) * uiScale);
    const int targetHeight = juce::roundToInt(static_cast<float>(layoutDimensions_.editor.height) * uiScale);

    if (pluginProcessor.isStandalone())
        setResizeLimits(targetWidth, targetHeight, targetWidth, targetHeight);

    setSize(targetWidth, targetHeight);

    if (pluginProcessor.isStandalone())
        syncStandaloneWindowSize();
}

void PluginEditor::syncStandaloneWindowSize()
{
    if (auto* window = findParentComponentOfClass<juce::DocumentWindow>())
        window->setContentComponentSize(getWidth(), getHeight());
}

void PluginEditor::syncUiScaleFromEditor()
{
    const int baseWidth = layoutDimensions_.editor.width;
    if (baseWidth <= 0)
        return;

    const float uiScale = TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth);

    if (auto* comp = mainComponent_.get())
    {
        comp->setUiScale(uiScale);

        const int matchingScaleId = PluginEditorInternal::matchingScaleIdForUiScale(uiScale);
        if (matchingScaleId != 0)
            comp->getHeaderPanel().setCurrentUiScaleId(matchingScaleId);
    }

    updateOverlayLayoutsForUiScale(uiScale);
}

void PluginEditor::updateOverlayLayoutsForUiScale(float uiScale)
{
    const auto layoutIfVisible = [uiScale](const auto& component, auto&& update)
    {
        if (component != nullptr && component->isVisible())
            update(uiScale);
    };

    layoutIfVisible(settingsWindow_, [this](float scale) { updateSettingsWindowLayout(scale); });
    layoutIfVisible(aboutWindow_, [this](float scale) { updateAboutWindowLayout(scale); });
    layoutIfVisible(masterInitConfirmDialog_,
                    [this](float scale) { updateMasterInitConfirmDialogLayout(scale); });
    layoutIfVisible(mutatorHistoryDefragConfirmDialog_,
                    [this](float scale) { updateMutatorHistoryDefragConfirmDialogLayout(scale); });
    layoutIfVisible(epromTypePromptDialog_,
                    [this](float scale) { updateEpromTypePromptDialogLayout(scale); });
    layoutIfVisible(bankTransferProgressDialog_,
                    [this](float scale) { updateBankTransferProgressDialogLayout(scale); });
}

void PluginEditor::applySkinFromItemId(int skinItemId, bool persistToState)
{
    skin_ = (skinItemId == PluginIDs::Settings::SkinVariants::kBlack)
        ? skinBlack_.get()
        : skinCream_.get();
    updateSkin();

    if (persistToState)
        pluginProcessor.setSkinVariantId(skinItemId);

    if (auto* comp = mainComponent_.get())
        comp->getHeaderPanel().setCurrentSkinItemId(skinItemId);
}

void PluginEditor::applyUiScaleFromItemId(int scaleId, bool persistToState)
{
    const float uiScale = PluginIDs::Settings::ScaleLevels::getUiScale(scaleId);
    applyUiScale(uiScale);
    updateSettingsWindowLayout(uiScale);
    updateAboutWindowLayout(uiScale);

    if (persistToState)
        pluginProcessor.setGuiScaleId(scaleId);

    if (auto* comp = mainComponent_.get())
        comp->getHeaderPanel().setCurrentUiScaleId(scaleId);
}

#if JUCE_DEBUG
void PluginEditor::setUiElementsTestVisible(bool visible)
{
    if (uiElementsTestVisible_ == visible)
        return;

    uiElementsTestVisible_ = visible;

    if (auto* main = mainComponent_.get())
        main->setUiElementsTestVisible(visible);

    if (auto* test = testComponent_.get())
    {
        test->setVisible(visible);
        if (visible)
        {
            test->toFront(false);
            test->grabKeyboardFocus();
        }
    }

    layoutUiElementsTestComponent();
    repaint();
}

void PluginEditor::layoutUiElementsTestComponent()
{
    if (!uiElementsTestVisible_ || mainComponent_ == nullptr || testComponent_ == nullptr)
        return;

    testComponent_->setBounds(mainComponent_->getUiElementsTestAreaBounds());
}
#endif
