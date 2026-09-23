// Extracted from PatchMutatorPanel.cpp for modular maintenance.
// Widget construction for mode / pitch / history rows.

#include "PatchMutatorPanel.h"
#include "PatchMutatorPanelInternal.h"

#include "Core/Services/PatchMutator/MutationPolicy.h"
#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/HierarchicalComboBox.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/Toggle.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

using namespace PatchMutatorPanelInternal;

void PatchMutatorPanel::setupModuleHeader(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    moduleHeader_ = std::make_unique<TSS::ModuleHeader>(
        dims_.moduleHeader.patchManagerTitleBandWidth,
        dims_.moduleHeader.height,
        TSS::moduleHeaderLookFromSkin(skin),
        TSS::ModuleHeader::ColourVariant::Blue,
        widgetFactory.getGroupDisplayName(PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId),
        dims_.moduleHeader);
    addAndMakeVisible(*moduleHeader_);
}

std::unique_ptr<TSS::Toggle> PatchMutatorPanel::makeRecipeToggle(TSS::ISkin& skin,
                                                                const char* displayName,
                                                                const char* widgetId)
{
    auto toggle = std::make_unique<TSS::Toggle>(
        dims_.toggles.patchMutatorWidth,
        dims_.toggles.height,
        TSS::toggleLookFromSkin(skin),
        displayName);
    connectToggleToApvts(toggle.get(), widgetId);
    addAndMakeVisible(*toggle);
    return toggle;
}

void PatchMutatorPanel::setupModeLine(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    modeLabel_ = std::make_unique<TSS::Label>(
        dims_.labels.patchMutatorWidth,
        dims_.labels.height,
        TSS::labelLookFromSkin(skin),
        MutatorDisplayNames::kMode);
    addAndMakeVisible(*modeLabel_);

    modeComboBox_ = std::make_unique<TSS::ComboBox>(
        dims_.comboBoxes.patchMutatorHistoryWidth,
        dims_.comboBoxes.standardHeight,
        TSS::comboBoxLookFromSkin(skin));
    modeComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    modeComboBox_->setPopupVerticalPlacement(TSS::PopupVerticalPlacement::Above);

    for (int modeIndex = 0; modeIndex < Core::kMutationModeCount; ++modeIndex)
        modeComboBox_->addItem(MutatorChoiceLists::MutationMode::kAll[modeIndex], modeIndex + 1);

    modeComboBox_->onChange = [this]
    {
        handleModeComboSelectionChange();
    };
    addAndMakeVisible(*modeComboBox_);

    mutateButton_ = widgetFactory.createStandaloneButton(
        MutatorWidgets::kMutate, skin, dims_.buttons.height);
    connectMutateOrRetryButton(mutateButton_.get(),
                               MutatorWidgets::kMutate,
                               MutatorState::kMutateDefragRecovery);
    addAndMakeVisible(*mutateButton_);

    dco1Toggle_ = makeRecipeToggle(skin, MutatorDisplayNames::kEnableDco1, MutatorWidgets::kEnableDco1);
    dco2Toggle_ = makeRecipeToggle(skin, MutatorDisplayNames::kEnableDco2, MutatorWidgets::kEnableDco2);
    vcfVcaToggle_ = makeRecipeToggle(skin, MutatorDisplayNames::kEnableVcfVca, MutatorWidgets::kEnableVcfVca);
    fmTrackToggle_ = makeRecipeToggle(skin, MutatorDisplayNames::kEnableFmTrack, MutatorWidgets::kEnableFmTrack);
    rampPortamentoToggle_ = makeRecipeToggle(skin,
                                             MutatorDisplayNames::kEnableRampPortamento,
                                             MutatorWidgets::kEnableRampPortamento);
}

void PatchMutatorPanel::setupPitchLine(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    pitchLabel_ = std::make_unique<TSS::Label>(
        dims_.labels.patchMutatorWidth,
        dims_.labels.height,
        TSS::labelLookFromSkin(skin),
        MutatorDisplayNames::kPitch);
    addAndMakeVisible(*pitchLabel_);

    pitchComboBox_ = std::make_unique<TSS::HierarchicalComboBox>(
        dims_.comboBoxes.patchMutatorHistoryWidth,
        dims_.comboBoxes.standardHeight,
        TSS::comboBoxLookFromSkin(skin));
    pitchComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    pitchComboBox_->setPopupVerticalPlacement(TSS::PopupVerticalPlacement::Above);

    for (int pitchIndex = 0; pitchIndex < Core::kMutationPitchModeCount; ++pitchIndex)
    {
        const int primaryId = pitchPrimaryIdForMode(pitchIndex);
        pitchComboBox_->addPrimaryItem(primaryId,
                                       MutatorChoiceLists::MutationPitch::kAll[pitchIndex],
                                       false,
                                       pitchClosedPrimaryLabel(pitchIndex));

        if (! Core::pitchModeUsesOctaveWindow(Core::mutationPitchModeFromIndex(pitchIndex)))
            continue;

        for (int octaves = Core::MutationCalibration::kMinPitchOctaves;
             octaves <= Core::MutationCalibration::kMaxPitchOctaves;
             ++octaves)
        {
            pitchComboBox_->addChildItem(primaryId,
                                         pitchChildIdFor(primaryId, octaves),
                                         MutatorChoiceLists::MutationPitch::formatOctaveWindow(octaves),
                                         MutatorChoiceLists::MutationPitch::formatOctaveWindowClosed(octaves));
        }
    }

    pitchComboBox_->onChange = [this]
    {
        handlePitchComboSelectionChange();
    };
    addAndMakeVisible(*pitchComboBox_);

    retryButton_ = widgetFactory.createStandaloneButton(
        MutatorWidgets::kRetry, skin, dims_.buttons.height);
    connectMutateOrRetryButton(retryButton_.get(),
                               MutatorWidgets::kRetry,
                               MutatorState::kRetryDefragRecovery);
    addAndMakeVisible(*retryButton_);

    env1Toggle_ = makeRecipeToggle(skin, MutatorDisplayNames::kEnableEnvelope1, MutatorWidgets::kEnableEnvelope1);
    env2Toggle_ = makeRecipeToggle(skin, MutatorDisplayNames::kEnableEnvelope2, MutatorWidgets::kEnableEnvelope2);
    env3Toggle_ = makeRecipeToggle(skin, MutatorDisplayNames::kEnableEnvelope3, MutatorWidgets::kEnableEnvelope3);
    lfo1Toggle_ = makeRecipeToggle(skin, MutatorDisplayNames::kEnableLfo1, MutatorWidgets::kEnableLfo1);
    lfo2Toggle_ = makeRecipeToggle(skin, MutatorDisplayNames::kEnableLfo2, MutatorWidgets::kEnableLfo2);
}

void PatchMutatorPanel::wireHistoryComboBox(TSS::ISkin& skin)
{
    historyComboBox_ = std::make_unique<TSS::HierarchicalComboBox>(
        dims_.comboBoxes.patchMutatorHistoryWidth,
        dims_.comboBoxes.standardHeight,
        TSS::comboBoxLookFromSkin(skin));
    historyComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    historyComboBox_->setPopupVerticalPlacement(TSS::PopupVerticalPlacement::Above);
    historyComboBox_->setTextWhenNothingSelected(MutatorDisplayNames::kEmptyHistorySentinel);
    historyComboBox_->onBeforeShowPopup = [this]
    {
        refreshHistoryComboBox();
    };
    historyComboBox_->onChange = [this]
    {
        handleHistoryComboSelectionChange();
    };
    addAndMakeVisible(*historyComboBox_);
}

void PatchMutatorPanel::setupHistoryActionButtons(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    const auto makeActionButton = [this, &skin, &widgetFactory](const char* widgetId)
    {
        auto button = widgetFactory.createStandaloneButton(widgetId, skin, dims_.buttons.height);
        connectButtonToApvts(button.get(), widgetId);
        addAndMakeVisible(*button);
        return button;
    };

    historyPreviousButton_ = makeActionButton(MutatorWidgets::kHistoryPrevious);
    historyNextButton_ = makeActionButton(MutatorWidgets::kHistoryNext);
    compareButton_ = makeActionButton(MutatorWidgets::kCompare);
    deleteButton_ = makeActionButton(MutatorWidgets::kDelete);
    clearButton_ = makeActionButton(MutatorWidgets::kClear);
    exportButton_ = makeActionButton(MutatorWidgets::kExport);

    enableMatrixModToggle_ = makeRecipeToggle(skin,
                                              MutatorDisplayNames::kEnableMatrixMod,
                                              MutatorWidgets::kEnableMatrixMod);
}

void PatchMutatorPanel::setupHistoryLine(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    historyLabel_ = std::make_unique<TSS::Label>(
        dims_.labels.patchMutatorWidth,
        dims_.labels.height,
        TSS::labelLookFromSkin(skin),
        MutatorDisplayNames::kHistory);
    addAndMakeVisible(*historyLabel_);

    wireHistoryComboBox(skin);
    setupHistoryActionButtons(skin, widgetFactory);
}

void PatchMutatorPanel::setDefragRecoveryRequestHandler(std::function<void()> handler)
{
    onDefragRecoveryRequested_ = std::move(handler);
}

void PatchMutatorPanel::connectButtonToApvts(TSS::Button* button, const char* widgetId)
{
    if (button == nullptr)
        return;

    button->onClick = [this, widgetId]
    {
        apvts_.state.setProperty(widgetId, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
    };
}

void PatchMutatorPanel::connectMutateOrRetryButton(TSS::Button* button,
                                                   const char* widgetId,
                                                   const char* recoveryPropertyId)
{
    if (button == nullptr)
        return;

    button->onClick = [this, widgetId, recoveryPropertyId]
    {
        if (isDefragRecoveryActive(recoveryPropertyId))
        {
            jassert(onDefragRecoveryRequested_);
            if (onDefragRecoveryRequested_)
                onDefragRecoveryRequested_();

            return;
        }

        apvts_.state.setProperty(widgetId, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
    };
}

void PatchMutatorPanel::connectToggleToApvts(TSS::Toggle* toggle, const char* widgetId)
{
    if (toggle == nullptr)
        return;

    toggle->onStateChange = [this, toggle, widgetId]
    {
        if (recipeHydrating_)
            return;

        apvts_.state.setProperty(widgetId, toggle->getToggleState(), nullptr);
    };
}
