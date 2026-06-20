#include "PatchMutatorPanel.h"

#include <vector>

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Toggle.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Core/Factories/ApvtsFactory.h"

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

    void hydrateIntSlider(TSS::Slider* slider, const juce::ValueTree& state, const char* propertyId)
    {
        if (slider == nullptr)
            return;

        slider->setValue(static_cast<double>(static_cast<int>(state.getProperty(propertyId, 0))),
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
    refreshHistoryMComboBox();
    refreshHistoryRComboBox();
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

    const auto allIntParams = ApvtsFactory::getAllIntParameters();
    const auto amountIt = std::find_if(allIntParams.begin(), allIntParams.end(),
        [](const PluginDescriptors::IntParameterDescriptor& desc) {
            return desc.parameterId == PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount;
        });
    
    amountSlider_ = std::make_unique<TSS::Slider>(
        dims_.sliders.patchMutatorWidth,
        dims_.sliders.standardHeight,
        TSS::sliderLookFromSkin(skin),
        TSS::SliderConfig{
            amountIt != allIntParams.end() ? static_cast<double>(amountIt->minValue) : 0.0,
            amountIt != allIntParams.end() ? static_cast<double>(amountIt->maxValue) : 100.0,
            amountIt != allIntParams.end() ? static_cast<double>(amountIt->defaultValue) : 0.0,
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

    const auto allIntParams = ApvtsFactory::getAllIntParameters();
    const auto randomIt = std::find_if(allIntParams.begin(), allIntParams.end(),
        [](const PluginDescriptors::IntParameterDescriptor& desc) {
            return desc.parameterId == PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom;
        });
    
    randomSlider_ = std::make_unique<TSS::Slider>(
        dims_.sliders.patchMutatorWidth,
        dims_.sliders.standardHeight,
        TSS::sliderLookFromSkin(skin),
        TSS::SliderConfig{
            randomIt != allIntParams.end() ? static_cast<double>(randomIt->minValue) : 0.0,
            randomIt != allIntParams.end() ? static_cast<double>(randomIt->maxValue) : 100.0,
            randomIt != allIntParams.end() ? static_cast<double>(randomIt->defaultValue) : 0.0,
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

    historyMComboBox_ = std::make_unique<TSS::ComboBox>(
        dims_.comboBoxes.patchMutatorHistoryMWidth,
        dims_.comboBoxes.standardHeight,
        TSS::comboBoxLookFromSkin(skin),
        TSS::ComboBox::Style::Standard);
    historyMComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    historyMComboBox_->setTextWhenNothingSelected(MutatorDisplayNames::kEmptyHistorySentinel);
    historyMComboBox_->onChange = [this]
    {
        if (historyMComboBox_ == nullptr)
            return;

        const int selectedId = historyMComboBox_->getSelectedId();
        if (selectedId <= 0 || selectedId > historyMRootIndices_.size())
            return;

        const int rootIndex = historyMRootIndices_[selectedId - 1];
        apvts_.state.setProperty(MutatorState::kSelectedM, rootIndex, nullptr);
        apvts_.state.setProperty(MutatorState::kSelectedR,
                                 MutatorState::kSelectedRRootOnly,
                                 nullptr);
    };
    addAndMakeVisible(*historyMComboBox_);

    historyRComboBox_ = std::make_unique<TSS::ComboBox>(
        dims_.comboBoxes.patchMutatorHistoryRWidth,
        dims_.comboBoxes.standardHeight,
        TSS::comboBoxLookFromSkin(skin),
        TSS::ComboBox::Style::Standard);
    historyRComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    historyRComboBox_->onChange = [this]
    {
        if (historyRComboBox_ == nullptr)
            return;

        const int selectedId = historyRComboBox_->getSelectedId();
        if (selectedId <= 0 || selectedId > historyRRetryIndices_.size())
            return;

        apvts_.state.setProperty(MutatorState::kSelectedR,
                                 historyRRetryIndices_[selectedId - 1],
                                 nullptr);
    };
    addAndMakeVisible(*historyRComboBox_);

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
}

void PatchMutatorPanel::valueTreePropertyChanged(juce::ValueTree&,
                                               const juce::Identifier& property)
{
    const auto name = property.toString();

    if (isRecipeProperty(name))
        refreshRecipeFromApvts();

    if (name == MutatorState::kHistoryMList || name == MutatorState::kSelectedM)
        refreshHistoryMComboBox();
    if (name == MutatorState::kHistoryRList || name == MutatorState::kSelectedM
        || name == MutatorState::kSelectedR)
    {
        refreshHistoryRComboBox();
    }
    if (name == MutatorState::kCompareActive || name == MutatorState::kHistoryMList
        || name == MutatorState::kSelectedM)
    {
        refreshCompareUiState();
    }
}

void PatchMutatorPanel::valueTreeRedirected(juce::ValueTree&)
{
    refreshRecipeFromApvts();
    refreshHistoryMComboBox();
    refreshHistoryRComboBox();
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
           || propertyName == MutatorWidgets::kEnableLfo2;
}

void PatchMutatorPanel::refreshRecipeFromApvts()
{
    const auto& state = apvts_.state;

    recipeHydrating_ = true;
    hydrateIntSlider(amountSlider_.get(), state, MutatorWidgets::kAmount);
    hydrateIntSlider(randomSlider_.get(), state, MutatorWidgets::kRandom);
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
    const auto mList = parsePipeSeparatedList(apvts_.state.getProperty(MutatorState::kHistoryMList).toString());
    const int selectedM = static_cast<int>(apvts_.state.getProperty(MutatorState::kSelectedM, -1));
    const bool historyEmpty = mList.isEmpty() || selectedM < 0;

    if (compareButton_ != nullptr)
        compareButton_->setEnabled(compareActive || ! historyEmpty);

    if (historyMComboBox_ != nullptr)
        historyMComboBox_->setEnabled(! compareActive && ! historyEmpty);

    if (historyRComboBox_ != nullptr)
        historyRComboBox_->setEnabled(! compareActive && ! historyEmpty);

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

juce::StringArray PatchMutatorPanel::parsePipeSeparatedList(const juce::String& encodedList)
{
    if (encodedList.isEmpty())
        return {};

    return juce::StringArray::fromTokens(encodedList, "|", "");
}

void PatchMutatorPanel::refreshHistoryMComboBox()
{
    if (historyMComboBox_ == nullptr)
        return;

    const auto mList = parsePipeSeparatedList(apvts_.state.getProperty(MutatorState::kHistoryMList).toString());
    historyMComboBox_->clear(juce::dontSendNotification);
    historyMRootIndices_.clear();

    if (mList.isEmpty())
    {
        historyMComboBox_->setTextWhenNothingSelected(MutatorDisplayNames::kEmptyHistorySentinel);
        historyMComboBox_->setSelectedId(0, juce::dontSendNotification);
        if (historyRComboBox_ != nullptr)
        {
            historyRComboBox_->clear(juce::dontSendNotification);
            historyRComboBox_->setSelectedId(0, juce::dontSendNotification);
        }
        refreshCompareUiState();
        return;
    }

    const auto selectedM = static_cast<int>(apvts_.state.getProperty(MutatorState::kSelectedM, -1));
    for (int i = 0; i < mList.size(); ++i)
    {
        const auto label = mList[i];
        const int rootIndex = label.substring(1, 3).getIntValue();
        historyMRootIndices_.add(rootIndex);
        historyMComboBox_->addItem(label, i + 1);
    }

    historyMComboBox_->setEnabled(true);
    syncHistoryMSelectionFromApvts();
    refreshCompareUiState();
}

void PatchMutatorPanel::refreshHistoryRComboBox()
{
    if (historyRComboBox_ == nullptr || historyMComboBox_ == nullptr)
        return;

    const auto mList = parsePipeSeparatedList(apvts_.state.getProperty(MutatorState::kHistoryMList).toString());
    if (mList.isEmpty())
    {
        historyRComboBox_->clear(juce::dontSendNotification);
        historyRComboBox_->setSelectedId(0, juce::dontSendNotification);
        refreshCompareUiState();
        return;
    }

    const auto rList = parsePipeSeparatedList(apvts_.state.getProperty(MutatorState::kHistoryRList).toString());
    historyRComboBox_->clear(juce::dontSendNotification);
    historyRRetryIndices_.clear();

    for (int i = 0; i < rList.size(); ++i)
    {
        const int retryIndex = i == 0
                                   ? MutatorState::kSelectedRRootOnly
                                   : rList[i].substring(1, 3).getIntValue();
        historyRRetryIndices_.add(retryIndex);
        historyRComboBox_->addItem(rList[i], i + 1);
    }

    historyRComboBox_->setEnabled(true);
    syncHistoryRSelectionFromApvts();
    refreshCompareUiState();
}

void PatchMutatorPanel::syncHistoryMSelectionFromApvts()
{
    if (historyMComboBox_ == nullptr)
        return;

    const int selectedM = static_cast<int>(apvts_.state.getProperty(MutatorState::kSelectedM, -1));
    if (selectedM < 0)
    {
        historyMComboBox_->setSelectedId(0, juce::dontSendNotification);
        return;
    }

    for (int i = 0; i < historyMRootIndices_.size(); ++i)
    {
        if (historyMRootIndices_[i] == selectedM)
        {
            historyMComboBox_->setSelectedId(i + 1, juce::dontSendNotification);
            return;
        }
    }

    historyMComboBox_->setSelectedId(0, juce::dontSendNotification);
}

void PatchMutatorPanel::syncHistoryRSelectionFromApvts()
{
    if (historyRComboBox_ == nullptr)
        return;

    const int selectedR = static_cast<int>(apvts_.state.getProperty(MutatorState::kSelectedR,
                                                                    MutatorState::kSelectedRRootOnly));
    for (int i = 0; i < historyRRetryIndices_.size(); ++i)
    {
        if (historyRRetryIndices_[i] == selectedR)
        {
            historyRComboBox_->setSelectedId(i + 1, juce::dontSendNotification);
            return;
        }
    }

    historyRComboBox_->setSelectedId(0, juce::dontSendNotification);
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
    if (historyMComboBox_)        historyMComboBox_->setUiScale(sf);
    if (historyRComboBox_)        historyRComboBox_->setUiScale(sf);
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
    const int comboBoxMW = juce::roundToInt(static_cast<float>(dims_.comboBoxes.patchMutatorHistoryMWidth) * sf);
    const int comboBoxRW = juce::roundToInt(static_cast<float>(dims_.comboBoxes.patchMutatorHistoryRWidth) * sf);
    const int comboBoxH   = juce::roundToInt(static_cast<float>(dims_.comboBoxes.standardHeight) * sf);
    const int rowH          = juce::roundToInt(static_cast<float>(dims_.layout.contentRowHeight) * sf);
    const int labelY        = y + (rowH - labelH) / 2;
    const int comboBoxY     = y + (rowH - comboBoxH) / 2;
    const int buttonH       = juce::roundToInt(static_cast<float>(dims_.buttons.height) * sf);
    const int compareW    = juce::roundToInt(static_cast<float>(dims_.buttons.patchMutatorCompareWidth) * sf);
    const int deleteW     = juce::roundToInt(static_cast<float>(dims_.buttons.patchMutatorDeleteWidth) * sf);
    const int clearW      = juce::roundToInt(static_cast<float>(dims_.buttons.patchMutatorClearWidth) * sf);
    const int exportW     = juce::roundToInt(static_cast<float>(dims_.buttons.patchMutatorExportWidth) * sf);
    const int controlGap  = dims_.layout.interControlGap;

    const float originX     = static_cast<float>(x);
    const float labelStep   = static_cast<float>(dims_.labels.patchMutatorWidth + controlGap) * sf;
    const float comboMStep  = static_cast<float>(dims_.comboBoxes.patchMutatorHistoryMWidth + controlGap) * sf;
    const float comboRStep  = static_cast<float>(dims_.comboBoxes.patchMutatorHistoryRWidth + controlGap) * sf;
    const float compareStep = static_cast<float>(dims_.buttons.patchMutatorCompareWidth + controlGap) * sf;
    const float deleteStep  = static_cast<float>(dims_.buttons.patchMutatorDeleteWidth + controlGap) * sf;
    const float clearStep   = static_cast<float>(dims_.buttons.patchMutatorClearWidth + controlGap) * sf;

    if (auto* label = historyLabel_.get())
        label->setBounds(x, labelY, labelW, labelH);
    if (auto* comboBox = historyMComboBox_.get())
        comboBox->setBounds(juce::roundToInt(originX + labelStep), comboBoxY, comboBoxMW, comboBoxH);
    if (auto* comboBox = historyRComboBox_.get())
        comboBox->setBounds(juce::roundToInt(originX + labelStep + comboMStep), comboBoxY, comboBoxRW, comboBoxH);
    if (auto* button = compareButton_.get())
        button->setBounds(juce::roundToInt(originX + labelStep + comboMStep + comboRStep), y, compareW, buttonH);
    if (auto* button = deleteButton_.get())
        button->setBounds(juce::roundToInt(originX + labelStep + comboMStep + comboRStep + compareStep), y, deleteW, buttonH);
    if (auto* button = clearButton_.get())
        button->setBounds(juce::roundToInt(originX + labelStep + comboMStep + comboRStep + compareStep + deleteStep), y, clearW, buttonH);
    if (auto* button = exportButton_.get())
        button->setBounds(juce::roundToInt(originX + labelStep + comboMStep + comboRStep + compareStep + deleteStep + clearStep), y, exportW, buttonH);
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
    if (historyMComboBox_)
    {
        historyMComboBox_->setLook(TSS::comboBoxLookFromSkin(skin));
        historyMComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    }
    if (historyRComboBox_)
    {
        historyRComboBox_->setLook(TSS::comboBoxLookFromSkin(skin));
        historyRComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    }
    if (mutateButton_)
        mutateButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (retryButton_)
        retryButton_->setLook(TSS::buttonLookFromSkin(skin));
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
}
