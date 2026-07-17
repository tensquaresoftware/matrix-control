#include "PatchMutatorPanel.h"

#include <vector>

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/HierarchicalComboBox.h"
#include "GUI/Widgets/Toggle.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Core/Services/PatchMutator/MutationNaming.h"

namespace
{
    namespace MutatorDisplayNames = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;
    namespace MutatorIds = PluginIDs::PatchManagerSection::PatchMutatorModule;
    namespace MutatorState = MutatorIds::StateProperties;
    namespace MutatorWidgets = MutatorIds::StandaloneWidgets;

    struct ActionEnabledBinding
    {
        const char* propertyId;
        TSS::Button* button;
    };

    const PluginDescriptors::IntParameterDescriptor* findMutatorIntDescriptor(const char* parameterId)
    {
        const auto& descriptors = PluginDescriptors::PatchManagerSection::PatchMutatorModule::kIntParameters;

        for (const auto& descriptor : descriptors)
        {
            if (descriptor.parameterId == parameterId)
                return &descriptor;
        }

        return nullptr;
    }

    void hydrateIntSlider(TSS::Slider* slider,
                          const juce::ValueTree& state,
                          const char* propertyId,
                          int defaultValue)
    {
        if (slider == nullptr)
            return;

        slider->setValue(static_cast<double>(static_cast<int>(state.getProperty(propertyId, defaultValue))),
                         juce::dontSendNotification);
    }

    void hydrateToggleBinding(TSS::Toggle* toggle, const juce::ValueTree& state, const char* propertyId)
    {
        if (toggle == nullptr)
            return;

        toggle->setToggleState(static_cast<bool>(state.getProperty(propertyId, false)),
                                juce::dontSendNotification);
    }
}

class PatchMutatorPanel::ActionEnabledPropertyListener : public juce::ValueTree::Listener
{
public:
    ActionEnabledPropertyListener(juce::ValueTree state, std::vector<ActionEnabledBinding> bindings)
        : state_(std::move(state))
        , bindings_(std::move(bindings))
    {
        state_.addListener(this);
        syncFromState();
    }

    ~ActionEnabledPropertyListener() override
    {
        state_.removeListener(this);
    }

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override
    {
        for (const auto& binding : bindings_)
        {
            if (property.toString() != binding.propertyId || binding.button == nullptr)
                continue;

            binding.button->setEnabled(static_cast<bool>(
                treeWhosePropertyHasChanged.getProperty(property, false)));
        }
    }

    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    void valueTreeRedirected(juce::ValueTree&) override
    {
        syncFromState();
    }

private:
    void syncFromState()
    {
        for (const auto& binding : bindings_)
        {
            if (binding.button == nullptr)
                continue;

            binding.button->setEnabled(static_cast<bool>(
                state_.getProperty(binding.propertyId, false)));
        }
    }

    juce::ValueTree state_;
    std::vector<ActionEnabledBinding> bindings_;
};

PatchMutatorPanel::PatchMutatorPanel(TSS::ISkin& skin, const PatchMutatorPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : dims_(dims)
    , skin_(&skin)
    , apvts_(apvts)
{
    setOpaque(false);
    setupModuleHeader(skin, widgetFactory);
    setupAmountLine(skin, widgetFactory);
    setupRandomLine(skin, widgetFactory);
    setupHistoryLine(skin, widgetFactory);

    if (! apvts_.state.hasProperty(MutatorState::kCompareActive))
        apvts_.state.setProperty(MutatorState::kCompareActive, false, nullptr);

    actionEnabledListener_ = std::make_unique<ActionEnabledPropertyListener>(
        apvts_.state,
        std::vector<ActionEnabledBinding>{
            { MutatorState::kMutateEnabled, mutateButton_.get() },
            { MutatorState::kRetryEnabled, retryButton_.get() },
            { MutatorState::kExportEnabled, exportButton_.get() },
            { MutatorState::kDeleteEnabled, deleteButton_.get() },
            { MutatorState::kClearEnabled, clearButton_.get() } });

    apvts_.state.addListener(this);
    refreshRecipeFromApvts();
    refreshHistoryComboBox();
    refreshCompareUiState();

    setSize(dims_.width, dims_.height);
}

PatchMutatorPanel::~PatchMutatorPanel()
{
    stopTimer();
    apvts_.state.removeListener(this);
}

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

void PatchMutatorPanel::setupAmountLine(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    amountLabel_ = std::make_unique<TSS::Label>(
        dims_.labels.patchMutatorWidth,
        dims_.labels.height,
        TSS::labelLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount);
    addAndMakeVisible(*amountLabel_);

    const auto* amountDesc = findMutatorIntDescriptor(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount);

    amountSlider_ = std::make_unique<TSS::Slider>(
        dims_.sliders.patchMutatorWidth,
        dims_.sliders.standardHeight,
        TSS::sliderLookFromSkin(skin),
        TSS::SliderConfig{
            amountDesc != nullptr ? static_cast<double>(amountDesc->minValue) : 1.0,
            amountDesc != nullptr ? static_cast<double>(amountDesc->maxValue) : 100.0,
            amountDesc != nullptr ? static_cast<double>(amountDesc->defaultValue) : 50.0,
            1.0,
            PluginDisplayNames::Units::kPercent,
            {},
            {},
            {}});
    amountSlider_->onValueChange = [this]
    {
        if (recipeHydrating_)
            return;

        apvts_.state.setProperty(MutatorWidgets::kAmount,
                                  static_cast<int>(amountSlider_->getValue()),
                                  nullptr);
    };
    addAndMakeVisible(*amountSlider_);

    mutateButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kMutate,
        skin,
        dims_.buttons.height);
    connectButtonToApvts(mutateButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kMutate);
    addAndMakeVisible(*mutateButton_);

    dco1Toggle_ = std::make_unique<TSS::Toggle>(
        dims_.toggles.patchMutatorWidth,
        dims_.toggles.height,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableDco1);
    connectToggleToApvts(dco1Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableDco1);
    addAndMakeVisible(*dco1Toggle_);

    dco2Toggle_ = std::make_unique<TSS::Toggle>(
        dims_.toggles.patchMutatorWidth,
        dims_.toggles.height,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableDco2);
    connectToggleToApvts(dco2Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableDco2);
    addAndMakeVisible(*dco2Toggle_);

    vcfVcaToggle_ = std::make_unique<TSS::Toggle>(
        dims_.toggles.patchMutatorWidth,
        dims_.toggles.height,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableVcfVca);
    connectToggleToApvts(vcfVcaToggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableVcfVca);
    addAndMakeVisible(*vcfVcaToggle_);

    fmTrackToggle_ = std::make_unique<TSS::Toggle>(
        dims_.toggles.patchMutatorWidth,
        dims_.toggles.height,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableFmTrack);
    connectToggleToApvts(fmTrackToggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableFmTrack);
    addAndMakeVisible(*fmTrackToggle_);

    rampPortamentoToggle_ = std::make_unique<TSS::Toggle>(
        dims_.toggles.patchMutatorWidth,
        dims_.toggles.height,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableRampPortamento);
    connectToggleToApvts(rampPortamentoToggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableRampPortamento);
    addAndMakeVisible(*rampPortamentoToggle_);
}

void PatchMutatorPanel::setupRandomLine(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    randomLabel_ = std::make_unique<TSS::Label>(
        dims_.labels.patchMutatorWidth,
        dims_.labels.height,
        TSS::labelLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom);
    addAndMakeVisible(*randomLabel_);

    const auto* randomDesc = findMutatorIntDescriptor(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom);

    randomSlider_ = std::make_unique<TSS::Slider>(
        dims_.sliders.patchMutatorWidth,
        dims_.sliders.standardHeight,
        TSS::sliderLookFromSkin(skin),
        TSS::SliderConfig{
            randomDesc != nullptr ? static_cast<double>(randomDesc->minValue) : 1.0,
            randomDesc != nullptr ? static_cast<double>(randomDesc->maxValue) : 100.0,
            randomDesc != nullptr ? static_cast<double>(randomDesc->defaultValue) : 25.0,
            1.0,
            PluginDisplayNames::Units::kPercent,
            {},
            {},
            {}});
    randomSlider_->onValueChange = [this]
    {
        if (recipeHydrating_)
            return;

        apvts_.state.setProperty(MutatorWidgets::kRandom,
                                  static_cast<int>(randomSlider_->getValue()),
                                  nullptr);
    };
    addAndMakeVisible(*randomSlider_);

    retryButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry,
        skin,
        dims_.buttons.height);
    connectButtonToApvts(retryButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry);
    addAndMakeVisible(*retryButton_);

    env1Toggle_ = std::make_unique<TSS::Toggle>(
        dims_.toggles.patchMutatorWidth,
        dims_.toggles.height,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope1);
    connectToggleToApvts(env1Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope1);
    addAndMakeVisible(*env1Toggle_);

    env2Toggle_ = std::make_unique<TSS::Toggle>(
        dims_.toggles.patchMutatorWidth,
        dims_.toggles.height,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope2);
    connectToggleToApvts(env2Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope2);
    addAndMakeVisible(*env2Toggle_);

    env3Toggle_ = std::make_unique<TSS::Toggle>(
        dims_.toggles.patchMutatorWidth,
        dims_.toggles.height,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope3);
    connectToggleToApvts(env3Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope3);
    addAndMakeVisible(*env3Toggle_);

    lfo1Toggle_ = std::make_unique<TSS::Toggle>(
        dims_.toggles.patchMutatorWidth,
        dims_.toggles.height,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableLfo1);
    connectToggleToApvts(lfo1Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableLfo1);
    addAndMakeVisible(*lfo1Toggle_);

    lfo2Toggle_ = std::make_unique<TSS::Toggle>(
        dims_.toggles.patchMutatorWidth,
        dims_.toggles.height,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableLfo2);
    connectToggleToApvts(lfo2Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableLfo2);
    addAndMakeVisible(*lfo2Toggle_);
}

void PatchMutatorPanel::setupHistoryLine(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    historyLabel_ = std::make_unique<TSS::Label>(
        dims_.labels.patchMutatorWidth,
        dims_.labels.height,
        TSS::labelLookFromSkin(skin),
        MutatorDisplayNames::kHistory);
    addAndMakeVisible(*historyLabel_);

    historyComboBox_ = std::make_unique<TSS::HierarchicalComboBox>(
        dims_.comboBoxes.patchMutatorHistoryWidth,
        dims_.comboBoxes.standardHeight,
        TSS::comboBoxLookFromSkin(skin));
    historyComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    historyComboBox_->setTextWhenNothingSelected(MutatorDisplayNames::kEmptyHistorySentinel);
    historyComboBox_->onBeforeShowPopup = [this]
    {
        refreshHistoryComboBox();
    };
    historyComboBox_->onChange = [this]
    {
        if (historySelectionHydrating_ || historyComboBox_ == nullptr)
            return;

        const int primaryId = historyComboBox_->getSelectedPrimaryId();
        if (primaryId <= 0 || primaryId > mutateRootIndices_.size())
            return;

        const int newRootIndex = mutateRootIndices_[primaryId - 1];
        const int currentRootIndex = static_cast<int>(apvts_.state.getProperty(MutatorState::kSelectedMutateRootIndex, -1));

        const int childId = historyComboBox_->getSelectedChildId();

        if (newRootIndex != currentRootIndex)
        {
            // Hierarchical UX: honour the submenu child clicked while changing M;
            // fall back to root-only when no child is selected.
            int newRetryIndex = MutatorState::kSelectedRetryRootOnly;
            const auto cacheIt = retryLabelsByRootIndex_.find(newRootIndex);
            if (childId > 0 && cacheIt != retryLabelsByRootIndex_.end())
            {
                const auto display = Core::MutationNaming::buildHistorySubmenuDisplay(
                    newRootIndex, cacheIt->second);
                if (childId <= display.retryIndices.size())
                    newRetryIndex = display.retryIndices[childId - 1];
            }

            deferHistoryComboRefresh_ = true;
            apvts_.state.setProperty(MutatorState::kSelectedRetryIndex, newRetryIndex, nullptr);
            apvts_.state.setProperty(MutatorState::kSelectedMutateRootIndex, newRootIndex, nullptr);
            deferHistoryComboRefresh_ = false;
            refreshHistoryComboBox();
            return;
        }

        if (childId <= 0)
        {
            apvts_.state.setProperty(MutatorState::kSelectedRetryIndex,
                                     MutatorState::kSelectedRetryRootOnly,
                                     nullptr);
            return;
        }

        if (childId > retryIndices_.size())
            return;

        apvts_.state.setProperty(MutatorState::kSelectedRetryIndex,
                                 retryIndices_[childId - 1],
                                 nullptr);
    };
    addAndMakeVisible(*historyComboBox_);

    historyPreviousButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistoryPrevious,
        skin,
        dims_.buttons.height);
    connectButtonToApvts(historyPreviousButton_.get(),
                         PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistoryPrevious);
    addAndMakeVisible(*historyPreviousButton_);

    historyNextButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistoryNext,
        skin,
        dims_.buttons.height);
    connectButtonToApvts(historyNextButton_.get(),
                         PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistoryNext);
    addAndMakeVisible(*historyNextButton_);

    compareButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare,
        skin,
        dims_.buttons.height);
    connectButtonToApvts(compareButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare);
    addAndMakeVisible(*compareButton_);

    deleteButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete,
        skin,
        dims_.buttons.height);
    connectButtonToApvts(deleteButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete);
    addAndMakeVisible(*deleteButton_);

    clearButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear,
        skin,
        dims_.buttons.height);
    connectButtonToApvts(clearButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear);
    addAndMakeVisible(*clearButton_);

    exportButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport,
        skin,
        dims_.buttons.height);
    connectButtonToApvts(exportButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport);
    addAndMakeVisible(*exportButton_);

    enableMatrixModToggle_ = std::make_unique<TSS::Toggle>(
        dims_.toggles.patchMutatorWidth,
        dims_.toggles.height,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableMatrixMod);
    connectToggleToApvts(enableMatrixModToggle_.get(),
                         PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableMatrixMod);
    addAndMakeVisible(*enableMatrixModToggle_);
}

void PatchMutatorPanel::valueTreePropertyChanged(juce::ValueTree&,
                                               const juce::Identifier& property)
{
    if (recipeHydrating_)
        return;

    const auto name = property.toString();

    if (isRecipeProperty(name))
        refreshRecipeFromApvts();

    if (name == MutatorState::kHistoryMutateList || name == MutatorState::kHistoryRetryList
        || name == MutatorState::kHistoryRetryListsByRoot
        || name == MutatorState::kSelectedMutateRootIndex || name == MutatorState::kSelectedRetryIndex)
    {
        scheduleHistoryComboBoxRefresh();
    }

    if (name == MutatorState::kCompareActive || name == MutatorState::kHistoryMutateList
        || name == MutatorState::kHistoryRetryListsByRoot
        || name == MutatorState::kSelectedMutateRootIndex)
    {
        refreshCompareUiState();
    }
}

void PatchMutatorPanel::valueTreeRedirected(juce::ValueTree&)
{
    refreshRecipeFromApvts();
    refreshHistoryComboBox();
    refreshCompareUiState();
}

bool PatchMutatorPanel::isRecipeProperty(const juce::String& propertyName)
{
    return propertyName == MutatorWidgets::kAmount
           || propertyName == MutatorWidgets::kRandom
           || propertyName == MutatorWidgets::kEnableDco1
           || propertyName == MutatorWidgets::kEnableDco2
           || propertyName == MutatorWidgets::kEnableVcfVca
           || propertyName == MutatorWidgets::kEnableFmTrack
           || propertyName == MutatorWidgets::kEnableRampPortamento
           || propertyName == MutatorWidgets::kEnableEnvelope1
           || propertyName == MutatorWidgets::kEnableEnvelope2
           || propertyName == MutatorWidgets::kEnableEnvelope3
           || propertyName == MutatorWidgets::kEnableLfo1
           || propertyName == MutatorWidgets::kEnableLfo2
           || propertyName == MutatorWidgets::kEnableMatrixMod;
}

void PatchMutatorPanel::refreshRecipeFromApvts()
{
    auto& state = apvts_.state;

    recipeHydrating_ = true;

    const auto clampRecipePercentProperty = [&state](const char* propertyId, int defaultValue)
    {
        const int raw = state.hasProperty(propertyId)
                            ? static_cast<int>(state.getProperty(propertyId))
                            : defaultValue;
        const int clamped = juce::jlimit(1, 100, raw);

        if (! state.hasProperty(propertyId) || static_cast<int>(state.getProperty(propertyId)) != clamped)
            state.setProperty(propertyId, clamped, nullptr);
    };

    clampRecipePercentProperty(MutatorWidgets::kAmount, 50);
    clampRecipePercentProperty(MutatorWidgets::kRandom, 25);

    hydrateIntSlider(amountSlider_.get(), state, MutatorWidgets::kAmount, 50);
    hydrateIntSlider(randomSlider_.get(), state, MutatorWidgets::kRandom, 25);
    hydrateRecipeTogglesFromApvts(state);
    recipeHydrating_ = false;
}

void PatchMutatorPanel::hydrateRecipeTogglesFromApvts(const juce::ValueTree& state)
{
    hydrateToggleBinding(dco1Toggle_.get(), state, MutatorWidgets::kEnableDco1);
    hydrateToggleBinding(dco2Toggle_.get(), state, MutatorWidgets::kEnableDco2);
    hydrateToggleBinding(vcfVcaToggle_.get(), state, MutatorWidgets::kEnableVcfVca);
    hydrateToggleBinding(fmTrackToggle_.get(), state, MutatorWidgets::kEnableFmTrack);
    hydrateToggleBinding(rampPortamentoToggle_.get(), state, MutatorWidgets::kEnableRampPortamento);
    hydrateToggleBinding(env1Toggle_.get(), state, MutatorWidgets::kEnableEnvelope1);
    hydrateToggleBinding(env2Toggle_.get(), state, MutatorWidgets::kEnableEnvelope2);
    hydrateToggleBinding(env3Toggle_.get(), state, MutatorWidgets::kEnableEnvelope3);
    hydrateToggleBinding(lfo1Toggle_.get(), state, MutatorWidgets::kEnableLfo1);
    hydrateToggleBinding(lfo2Toggle_.get(), state, MutatorWidgets::kEnableLfo2);
    hydrateToggleBinding(enableMatrixModToggle_.get(), state, MutatorWidgets::kEnableMatrixMod);
}

void PatchMutatorPanel::timerCallback()
{
    compareBlinkVisible_ = ! compareBlinkVisible_;
    if (compareButton_ != nullptr)
        compareButton_->setAlpha(compareBlinkVisible_ ? 1.0f : 0.35f);
}

void PatchMutatorPanel::refreshCompareUiState()
{
    const bool compareActive = static_cast<bool>(apvts_.state.getProperty(MutatorState::kCompareActive,
                                                                         false));
    const auto mutateLabelList = parsePipeSeparatedList(apvts_.state.getProperty(MutatorState::kHistoryMutateList).toString());
    const int selectedMutateRootIndex = static_cast<int>(apvts_.state.getProperty(MutatorState::kSelectedMutateRootIndex, -1));
    const bool historyEmpty = mutateLabelList.isEmpty() || selectedMutateRootIndex < 0;
    const int flatHistoryEntryCount = countFlatHistoryEntries(apvts_.state);

    applyCompareControlLock(compareActive);

    if (compareButton_ != nullptr)
    {
        compareButton_->setEnabled(compareActive || ! historyEmpty);
        compareButton_->setToggleState(compareActive, juce::dontSendNotification);
    }

    if (historyComboBox_ != nullptr)
        historyComboBox_->setEnabled(! compareActive && ! historyEmpty);

    // Nav is useful only when circular step can change selection (≥2 flat entries).
    const bool historyNavEnabled = ! compareActive && flatHistoryEntryCount >= 2;
    if (historyPreviousButton_ != nullptr)
        historyPreviousButton_->setEnabled(historyNavEnabled);
    if (historyNextButton_ != nullptr)
        historyNextButton_->setEnabled(historyNavEnabled);

    if (compareActive)
    {
        compareBlinkVisible_ = true;
        startTimerHz(2);
    }
    else
    {
        stopTimer();
        if (compareButton_ != nullptr)
            compareButton_->setAlpha(1.0f);
    }
}

void PatchMutatorPanel::applyCompareControlLock(bool compareActive)
{
    const auto lockControl = [compareActive](juce::Component* control)
    {
        if (control == nullptr)
            return;

        control->setEnabled(! compareActive);
    };

    lockControl(amountSlider_.get());
    lockControl(randomSlider_.get());
    lockControl(dco1Toggle_.get());
    lockControl(dco2Toggle_.get());
    lockControl(vcfVcaToggle_.get());
    lockControl(fmTrackToggle_.get());
    lockControl(rampPortamentoToggle_.get());
    lockControl(env1Toggle_.get());
    lockControl(env2Toggle_.get());
    lockControl(env3Toggle_.get());
    lockControl(lfo1Toggle_.get());
    lockControl(lfo2Toggle_.get());
    lockControl(enableMatrixModToggle_.get());

    // Action buttons: locked -> disabled skin paint; unlocked -> restore from their enabled mirror.
    const auto lockActionButton = [this, compareActive](TSS::Button* button, const char* mirrorId)
    {
        if (button == nullptr)
            return;

        const bool enabled = compareActive
                                 ? false
                                 : static_cast<bool>(apvts_.state.getProperty(mirrorId, false));
        button->setEnabled(enabled);
    };

    lockActionButton(mutateButton_.get(), MutatorState::kMutateEnabled);
    lockActionButton(retryButton_.get(), MutatorState::kRetryEnabled);
    lockActionButton(deleteButton_.get(), MutatorState::kDeleteEnabled);
    lockActionButton(clearButton_.get(), MutatorState::kClearEnabled);
    lockActionButton(exportButton_.get(), MutatorState::kExportEnabled);

    if (historyComboBox_ != nullptr)
        historyComboBox_->setEnabled(! compareActive);

    // History prev/next enablement is owned by refreshCompareUiState (needs flat entry count).
}

juce::StringArray PatchMutatorPanel::parsePipeSeparatedList(const juce::String& encodedList)
{
    if (encodedList.isEmpty())
        return {};

    return juce::StringArray::fromTokens(encodedList, "|", "");
}

void PatchMutatorPanel::scheduleHistoryComboBoxRefresh()
{
    if (deferHistoryComboRefresh_ || historyComboRefreshScheduled_)
        return;

    historyComboRefreshScheduled_ = true;

    juce::Component::SafePointer<PatchMutatorPanel> safePanel(this);
    juce::MessageManager::callAsync([safePanel]
    {
        if (safePanel == nullptr)
            return;

        safePanel->historyComboRefreshScheduled_ = false;
        safePanel->refreshHistoryComboBox();
    });
}

std::map<int, juce::StringArray> PatchMutatorPanel::parseRetryListsByRoot(const juce::String& encoded)
{
    std::map<int, juce::StringArray> result;
    if (encoded.isEmpty())
        return result;

    for (const auto& part : juce::StringArray::fromTokens(encoded, ";", ""))
    {
        const int eq = part.indexOfChar('=');
        if (eq <= 0)
            continue;

        const int rootIndex = part.substring(0, eq).getIntValue();
        result[rootIndex] = parsePipeSeparatedList(part.substring(eq + 1));
    }

    return result;
}

int PatchMutatorPanel::countFlatHistoryEntries(const juce::ValueTree& state)
{
    const auto byRoot = parseRetryListsByRoot(state.getProperty(MutatorState::kHistoryRetryListsByRoot).toString());
    if (! byRoot.empty())
    {
        int count = 0;
        for (const auto& entry : byRoot)
            count += entry.second.size();
        return count;
    }

    return parsePipeSeparatedList(state.getProperty(MutatorState::kHistoryMutateList).toString()).size();
}

void PatchMutatorPanel::rebuildRetryLabelsCacheFromApvts()
{
    retryLabelsByRootIndex_ = parseRetryListsByRoot(
        apvts_.state.getProperty(MutatorState::kHistoryRetryListsByRoot).toString());

    // Fallback for older in-session state that only has the selected-root mirror.
    if (retryLabelsByRootIndex_.empty())
    {
        const int selectedMutateRootIndex = static_cast<int>(
            apvts_.state.getProperty(MutatorState::kSelectedMutateRootIndex, -1));
        const auto retryLabelList = parsePipeSeparatedList(
            apvts_.state.getProperty(MutatorState::kHistoryRetryList).toString());
        if (selectedMutateRootIndex >= 0 && ! retryLabelList.isEmpty())
            retryLabelsByRootIndex_[selectedMutateRootIndex] = retryLabelList;
    }
}

void PatchMutatorPanel::pruneRetryLabelsCache()
{
    for (auto it = retryLabelsByRootIndex_.begin(); it != retryLabelsByRootIndex_.end();)
    {
        if (! mutateRootIndices_.contains(it->first))
            it = retryLabelsByRootIndex_.erase(it);
        else
            ++it;
    }
}

void PatchMutatorPanel::addRetryChildrenForPrimary(int primaryId, const juce::StringArray& retryLabelList)
{
    if (primaryId <= 0 || primaryId > mutateRootIndices_.size())
        return;

    const int rootIndex = mutateRootIndices_[primaryId - 1];
    const auto display = Core::MutationNaming::buildHistorySubmenuDisplay(rootIndex, retryLabelList);
    if (display.labels.isEmpty())
        return;

    jassert(display.labels.size() == display.retryIndices.size());

    const bool trackSelectionIndices = (rootIndex
                                        == static_cast<int>(apvts_.state.getProperty(
                                            MutatorState::kSelectedMutateRootIndex, -1)));

    if (trackSelectionIndices)
        retryIndices_.clear();

    for (int i = 0; i < display.labels.size(); ++i)
    {
        const int childId = i + 1;
        if (trackSelectionIndices)
            retryIndices_.add(display.retryIndices[i]);
        historyComboBox_->addChildItem(primaryId, childId, display.labels[i]);
    }
}

void PatchMutatorPanel::refreshHistoryComboBox()
{
    if (historyComboBox_ == nullptr)
        return;

    const auto mutateLabelList = parsePipeSeparatedList(apvts_.state.getProperty(MutatorState::kHistoryMutateList).toString());
    historySelectionHydrating_ = true;
    historyComboBox_->clear();
    mutateRootIndices_.clear();
    retryIndices_.clear();

    if (mutateLabelList.isEmpty())
    {
        retryLabelsByRootIndex_.clear();
        historyComboBox_->setTextWhenNothingSelected(MutatorDisplayNames::kEmptyHistorySentinel);
        historyComboBox_->setSelectedIds(0, 0, juce::dontSendNotification);
        historySelectionHydrating_ = false;
        refreshCompareUiState();
        return;
    }

    rebuildRetryLabelsCacheFromApvts();

    for (int i = 0; i < mutateLabelList.size(); ++i)
    {
        const auto label = mutateLabelList[i];
        const int primaryId = i + 1;
        const int rootIndex = label.substring(1, 3).getIntValue();
        mutateRootIndices_.add(rootIndex);
        historyComboBox_->addPrimaryItem(primaryId, label);
    }

    pruneRetryLabelsCache();

    for (int i = 0; i < mutateRootIndices_.size(); ++i)
    {
        const int rootIndex = mutateRootIndices_[i];
        const auto cacheIt = retryLabelsByRootIndex_.find(rootIndex);
        if (cacheIt == retryLabelsByRootIndex_.end() || cacheIt->second.isEmpty())
            continue;

        addRetryChildrenForPrimary(i + 1, cacheIt->second);
    }

    historyComboBox_->setEnabled(true);
    syncHistorySelectionFromApvts();
    historySelectionHydrating_ = false;
    refreshCompareUiState();
}

void PatchMutatorPanel::syncHistorySelectionFromApvts()
{
    if (historyComboBox_ == nullptr)
        return;

    const int selectedMutateRootIndex = static_cast<int>(apvts_.state.getProperty(MutatorState::kSelectedMutateRootIndex, -1));
    if (selectedMutateRootIndex < 0)
    {
        historyComboBox_->setSelectedIds(0, 0, juce::dontSendNotification);
        return;
    }

    int primaryId = 0;
    for (int i = 0; i < mutateRootIndices_.size(); ++i)
    {
        if (mutateRootIndices_[i] == selectedMutateRootIndex)
        {
            primaryId = i + 1;
            break;
        }
    }

    if (primaryId <= 0)
    {
        historyComboBox_->setSelectedIds(0, 0, juce::dontSendNotification);
        return;
    }

    const int selectedRetryIndex = static_cast<int>(apvts_.state.getProperty(MutatorState::kSelectedRetryIndex,
                                                                             MutatorState::kSelectedRetryRootOnly));
    int childId = 0;

    for (int i = 0; i < retryIndices_.size(); ++i)
    {
        if (retryIndices_[i] == selectedRetryIndex)
        {
            childId = i + 1;
            break;
        }
    }

    if (childId == 0 && ! retryIndices_.isEmpty())
    {
        // Root-only, or orphan retry not present in this root's list — select N2 root recall.
        childId = 1;
    }

    historyComboBox_->setSelectedIds(primaryId, childId, juce::dontSendNotification);
}

void PatchMutatorPanel::connectButtonToApvts(TSS::Button* button, const char* widgetId)
{
    if (button != nullptr)
    {
        button->onClick = [this, widgetId]
        {
            apvts_.state.setProperty(widgetId, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
        };
    }
}

void PatchMutatorPanel::connectToggleToApvts(TSS::Toggle* toggle, const char* widgetId)
{
    if (toggle != nullptr)
    {
        toggle->onStateChange = [this, toggle, widgetId]
        {
            if (recipeHydrating_)
                return;

            apvts_.state.setProperty(widgetId, toggle->getToggleState(), nullptr);
        };
    }
}

void PatchMutatorPanel::resized()
{
    const float sf = uiScale_;

    const int moduleHeaderW = juce::roundToInt(static_cast<float>(dims_.moduleHeader.patchManagerTitleBandWidth) * sf);
    const int moduleHeaderH = juce::roundToInt(static_cast<float>(dims_.moduleHeader.height) * sf);

    if (auto* header = moduleHeader_.get())
        header->setBounds(0, 0, moduleHeaderW, moduleHeaderH);

    const int contentRowH = juce::roundToInt(static_cast<float>(dims_.layout.contentRowHeight) * sf);
    const int rowGap = juce::roundToInt(static_cast<float>(dims_.layout.interControlGap) * sf);
    const int rowStep = contentRowH + rowGap;

    const int row0Y = juce::roundToInt(static_cast<float>(dims_.moduleHeader.height) * sf);
    const int row1Y = row0Y + rowStep;
    const int row2Y = row1Y + rowStep;

    layoutSliderLine(0, row0Y, amountLabel_.get(), amountSlider_.get(), mutateButton_.get(),
        {dco1Toggle_.get(), dco2Toggle_.get(), vcfVcaToggle_.get(), fmTrackToggle_.get(), rampPortamentoToggle_.get()},
        dims_.buttons.patchMutatorMutateWidth);
    layoutSliderLine(0, row1Y, randomLabel_.get(), randomSlider_.get(), retryButton_.get(),
        {env1Toggle_.get(), env2Toggle_.get(), env3Toggle_.get(), lfo1Toggle_.get(), lfo2Toggle_.get()},
        dims_.buttons.patchMutatorRetryWidth);
    layoutHistoryLine(0, row2Y);

    if (moduleHeader_)            moduleHeader_->setUiScale(sf);
    if (amountLabel_)             amountLabel_->setUiScale(sf);
    if (amountSlider_)            amountSlider_->setUiScale(sf);
    if (mutateButton_)            mutateButton_->setUiScale(sf);
    if (randomLabel_)             randomLabel_->setUiScale(sf);
    if (randomSlider_)            randomSlider_->setUiScale(sf);
    if (retryButton_)             retryButton_->setUiScale(sf);
    if (historyLabel_)            historyLabel_->setUiScale(sf);
    if (historyComboBox_)        historyComboBox_->setUiScale(sf);
    if (historyPreviousButton_)   historyPreviousButton_->setUiScale(sf);
    if (historyNextButton_)       historyNextButton_->setUiScale(sf);
    if (compareButton_)           compareButton_->setUiScale(sf);
    if (deleteButton_)            deleteButton_->setUiScale(sf);
    if (clearButton_)             clearButton_->setUiScale(sf);
    if (exportButton_)            exportButton_->setUiScale(sf);
    if (dco1Toggle_)              dco1Toggle_->setUiScale(sf);
    if (dco2Toggle_)              dco2Toggle_->setUiScale(sf);
    if (vcfVcaToggle_)            vcfVcaToggle_->setUiScale(sf);
    if (fmTrackToggle_)           fmTrackToggle_->setUiScale(sf);
    if (rampPortamentoToggle_)    rampPortamentoToggle_->setUiScale(sf);
    if (env1Toggle_)              env1Toggle_->setUiScale(sf);
    if (env2Toggle_)              env2Toggle_->setUiScale(sf);
    if (env3Toggle_)              env3Toggle_->setUiScale(sf);
    if (lfo1Toggle_)              lfo1Toggle_->setUiScale(sf);
    if (lfo2Toggle_)              lfo2Toggle_->setUiScale(sf);
    if (enableMatrixModToggle_)   enableMatrixModToggle_->setUiScale(sf);
}

void PatchMutatorPanel::layoutSliderLine(int x, int y, TSS::Label* label, TSS::Slider* slider, TSS::Button* button,
                                         const std::vector<TSS::Toggle*>& toggles, int actionButtonWidth)
{
    const float sf = uiScale_;

    const int labelW      = juce::roundToInt(static_cast<float>(dims_.labels.patchMutatorWidth) * sf);
    const int labelH      = juce::roundToInt(static_cast<float>(dims_.labels.height) * sf);
    const int sliderW     = juce::roundToInt(static_cast<float>(dims_.sliders.patchMutatorWidth) * sf);
    const int sliderH     = juce::roundToInt(static_cast<float>(dims_.sliders.standardHeight) * sf);
    const int buttonW     = juce::roundToInt(static_cast<float>(actionButtonWidth) * sf);
    const int buttonH     = juce::roundToInt(static_cast<float>(dims_.buttons.height) * sf);
    const int toggleW     = juce::roundToInt(static_cast<float>(dims_.toggles.patchMutatorWidth) * sf);
    const int toggleH     = juce::roundToInt(static_cast<float>(dims_.toggles.height) * sf);
    const int controlGap  = dims_.layout.interControlGap;

    const float labelStep  = static_cast<float>(dims_.labels.patchMutatorWidth + controlGap) * sf;
    const float sliderStep = static_cast<float>(dims_.sliders.patchMutatorWidth + controlGap) * sf;
    const float buttonStep = static_cast<float>(actionButtonWidth + controlGap) * sf;
    const float toggleStep = static_cast<float>(dims_.toggles.patchMutatorWidth + controlGap) * sf;

    const int rowH          = juce::roundToInt(static_cast<float>(dims_.layout.contentRowHeight) * sf);
    const int labelY        = y + (rowH - labelH) / 2;
    const int sliderY       = y + (rowH - sliderH) / 2;

    const float originX = static_cast<float>(x);

    if (label != nullptr)
        label->setBounds(x, labelY, labelW, labelH);

    if (slider != nullptr)
        slider->setBounds(juce::roundToInt(originX + labelStep), sliderY, sliderW, sliderH);

    if (button != nullptr)
        button->setBounds(juce::roundToInt(originX + labelStep + sliderStep), y, buttonW, buttonH);

    const float toggleOriginX = originX + labelStep + sliderStep + buttonStep;
    for (int i = 0; i < static_cast<int>(toggles.size()); ++i)
    {
        if (toggles[static_cast<size_t>(i)] != nullptr)
            toggles[static_cast<size_t>(i)]->setBounds(juce::roundToInt(toggleOriginX + static_cast<float>(i) * toggleStep), y, toggleW, toggleH);
    }
}

void PatchMutatorPanel::layoutHistoryLine(int x, int y)
{
    const float sf = uiScale_;

    const int labelW      = juce::roundToInt(static_cast<float>(dims_.labels.patchMutatorWidth) * sf);
    const int labelH      = juce::roundToInt(static_cast<float>(dims_.labels.height) * sf);
    const int comboBoxW   = juce::roundToInt(static_cast<float>(dims_.comboBoxes.patchMutatorHistoryWidth) * sf);
    const int comboBoxH   = juce::roundToInt(static_cast<float>(dims_.comboBoxes.standardHeight) * sf);
    const int rowH          = juce::roundToInt(static_cast<float>(dims_.layout.contentRowHeight) * sf);
    const int labelY        = y + (rowH - labelH) / 2;
    const int comboBoxY     = y + (rowH - comboBoxH) / 2;
    const int buttonH       = juce::roundToInt(static_cast<float>(dims_.buttons.height) * sf);
    const int navW        = juce::roundToInt(static_cast<float>(dims_.buttons.patchMutatorHistoryNavWidth) * sf);
    const int compareW    = juce::roundToInt(static_cast<float>(dims_.buttons.patchMutatorCompareWidth) * sf);
    const int deleteW     = juce::roundToInt(static_cast<float>(dims_.buttons.patchMutatorDeleteWidth) * sf);
    const int clearW      = juce::roundToInt(static_cast<float>(dims_.buttons.patchMutatorClearWidth) * sf);
    const int exportW     = juce::roundToInt(static_cast<float>(dims_.buttons.patchMutatorExportWidth) * sf);
    const int toggleW     = juce::roundToInt(static_cast<float>(dims_.toggles.patchMutatorWidth) * sf);
    const int toggleH     = juce::roundToInt(static_cast<float>(dims_.toggles.height) * sf);
    const int controlGap  = dims_.layout.interControlGap;

    const float originX     = static_cast<float>(x);
    const float labelStep   = static_cast<float>(dims_.labels.patchMutatorWidth + controlGap) * sf;
    const float comboStep   = static_cast<float>(dims_.comboBoxes.patchMutatorHistoryWidth + controlGap) * sf;
    const float navStep     = static_cast<float>(dims_.buttons.patchMutatorHistoryNavWidth + controlGap) * sf;
    const float compareStep = static_cast<float>(dims_.buttons.patchMutatorCompareWidth + controlGap) * sf;
    const float deleteStep  = static_cast<float>(dims_.buttons.patchMutatorDeleteWidth + controlGap) * sf;
    const float clearStep   = static_cast<float>(dims_.buttons.patchMutatorClearWidth + controlGap) * sf;
    const float exportStep  = static_cast<float>(dims_.buttons.patchMutatorExportWidth + controlGap) * sf;

    float cursorX = originX;

    if (auto* label = historyLabel_.get())
        label->setBounds(x, labelY, labelW, labelH);
    cursorX += labelStep;

    if (auto* comboBox = historyComboBox_.get())
        comboBox->setBounds(juce::roundToInt(cursorX), comboBoxY, comboBoxW, comboBoxH);
    cursorX += comboStep;

    if (auto* button = historyPreviousButton_.get())
        button->setBounds(juce::roundToInt(cursorX), y, navW, buttonH);
    cursorX += navStep;

    if (auto* button = historyNextButton_.get())
        button->setBounds(juce::roundToInt(cursorX), y, navW, buttonH);
    cursorX += navStep;

    if (auto* button = compareButton_.get())
        button->setBounds(juce::roundToInt(cursorX), y, compareW, buttonH);
    cursorX += compareStep;

    if (auto* button = deleteButton_.get())
        button->setBounds(juce::roundToInt(cursorX), y, deleteW, buttonH);
    cursorX += deleteStep;

    if (auto* button = clearButton_.get())
        button->setBounds(juce::roundToInt(cursorX), y, clearW, buttonH);
    cursorX += clearStep;

    if (auto* button = exportButton_.get())
        button->setBounds(juce::roundToInt(cursorX), y, exportW, buttonH);
    cursorX += exportStep;

    if (auto* toggle = enableMatrixModToggle_.get())
        toggle->setBounds(juce::roundToInt(cursorX), y, toggleW, toggleH);
}

void PatchMutatorPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    propagateSkinsToControlWidgets(skin);
    propagateSkinsToToggleWidgets(skin);
}

void PatchMutatorPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    repaint();
}

void PatchMutatorPanel::propagateSkinsToControlWidgets(TSS::ISkin& skin)
{
    if (moduleHeader_)
        moduleHeader_->setLook(TSS::moduleHeaderLookFromSkin(skin));

    if (amountLabel_)
        amountLabel_->setLook(TSS::labelLookFromSkin(skin));
    if (amountSlider_)
        amountSlider_->setLook(TSS::sliderLookFromSkin(skin));
    if (randomLabel_)
        randomLabel_->setLook(TSS::labelLookFromSkin(skin));
    if (randomSlider_)
        randomSlider_->setLook(TSS::sliderLookFromSkin(skin));
    if (historyLabel_)
        historyLabel_->setLook(TSS::labelLookFromSkin(skin));
    if (historyComboBox_)
    {
        historyComboBox_->setLook(TSS::comboBoxLookFromSkin(skin));
        historyComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    }
    if (mutateButton_)
        mutateButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (retryButton_)
        retryButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (historyPreviousButton_)
        historyPreviousButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (historyNextButton_)
        historyNextButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (compareButton_)
        compareButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (deleteButton_)
        deleteButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (clearButton_)
        clearButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (exportButton_)
        exportButton_->setLook(TSS::buttonLookFromSkin(skin));
}

void PatchMutatorPanel::propagateSkinsToToggleWidgets(TSS::ISkin& skin)
{
    const auto toggleLook = TSS::toggleLookFromSkin(skin);
    if (dco1Toggle_)
        dco1Toggle_->setLook(toggleLook);
    if (dco2Toggle_)
        dco2Toggle_->setLook(toggleLook);
    if (vcfVcaToggle_)
        vcfVcaToggle_->setLook(toggleLook);
    if (fmTrackToggle_)
        fmTrackToggle_->setLook(toggleLook);
    if (rampPortamentoToggle_)
        rampPortamentoToggle_->setLook(toggleLook);
    if (env1Toggle_)
        env1Toggle_->setLook(toggleLook);
    if (env2Toggle_)
        env2Toggle_->setLook(toggleLook);
    if (env3Toggle_)
        env3Toggle_->setLook(toggleLook);
    if (lfo1Toggle_)
        lfo1Toggle_->setLook(toggleLook);
    if (lfo2Toggle_)
        lfo2Toggle_->setLook(toggleLook);
    if (enableMatrixModToggle_)
        enableMatrixModToggle_->setLook(toggleLook);
}
