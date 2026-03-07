#include "PatchMutatorPanel.h"

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
#include "Shared/Definitions/PluginDimensions.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Core/Factories/ApvtsFactory.h"

PatchMutatorPanel::PatchMutatorPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , apvts_(apvts)
{
    setOpaque(false);
    setupModuleHeader(skin, widgetFactory);
    setupAmountLine(skin, widgetFactory);
    setupRandomLine(skin, widgetFactory);
    setupHistoryLine(skin, widgetFactory);
    setSize(width_, height_);
}

PatchMutatorPanel::~PatchMutatorPanel() = default;

void PatchMutatorPanel::setupModuleHeader(tss::ISkin&, WidgetFactory& widgetFactory)
{
    moduleHeader_ = std::make_unique<tss::ModuleHeader>(
        widgetFactory.getGroupDisplayName(PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId),
        PluginDimensions::Widgets::Widths::ModuleHeader::kPatchManagerModule,
        PluginDimensions::Widgets::Heights::kModuleHeader,
        tss::ModuleHeader::ColourVariant::Blue);
    addAndMakeVisible(*moduleHeader_);
}

void PatchMutatorPanel::setupAmountLine(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    amountLabel_ = std::make_unique<tss::Label>(
        PluginDimensions::Widgets::Widths::Label::kPatchMutator,
        PluginDimensions::Widgets::Heights::kLabel,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount);
    amountLabel_->setLook(tss::labelLookFromSkin(skin));
    addAndMakeVisible(*amountLabel_);

    const auto allIntParams = ApvtsFactory::getAllIntParameters();
    const auto amountIt = std::find_if(allIntParams.begin(), allIntParams.end(),
        [](const PluginDescriptors::IntParameterDescriptor& desc) {
            return desc.parameterId == PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount;
        });
    
    amountSlider_ = std::make_unique<tss::Slider>(PluginDimensions::Widgets::Widths::Slider::kPatchMutator, PluginDimensions::Widgets::Heights::kSlider, 0.0);
    amountSlider_->setLook(tss::sliderLookFromSkin(skin));
    if (amountIt != allIntParams.end())
    {
        amountSlider_->setRange(static_cast<double>(amountIt->minValue), static_cast<double>(amountIt->maxValue), 1.0);
        amountSlider_->setValue(static_cast<double>(amountIt->defaultValue));
    }
    amountSlider_->setUnit(PluginDisplayNames::Units::kPercent);
    amountSlider_->onValueChange = [this]
    {
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount,
            static_cast<int>(amountSlider_->getValue()),
            nullptr);
    };
    addAndMakeVisible(*amountSlider_);

    mutateButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kMutate,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(mutateButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kMutate);
    addAndMakeVisible(*mutateButton_);

    dco1Toggle_ = std::make_unique<tss::Toggle>(
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco1);
    connectToggleToApvts(dco1Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco1);
    addAndMakeVisible(*dco1Toggle_);

    dco2Toggle_ = std::make_unique<tss::Toggle>(
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco2);
    connectToggleToApvts(dco2Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco2);
    addAndMakeVisible(*dco2Toggle_);

    vcfVcaToggle_ = std::make_unique<tss::Toggle>(
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kVcfVca);
    connectToggleToApvts(vcfVcaToggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kVcfVca);
    addAndMakeVisible(*vcfVcaToggle_);

    fmTrackToggle_ = std::make_unique<tss::Toggle>(
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kFmTrack);
    connectToggleToApvts(fmTrackToggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kFmTrack);
    addAndMakeVisible(*fmTrackToggle_);

    rampPortamentoToggle_ = std::make_unique<tss::Toggle>(
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRampPortamento);
    connectToggleToApvts(rampPortamentoToggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRampPortamento);
    addAndMakeVisible(*rampPortamentoToggle_);
}

void PatchMutatorPanel::setupRandomLine(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    randomLabel_ = std::make_unique<tss::Label>(
        PluginDimensions::Widgets::Widths::Label::kPatchMutator,
        PluginDimensions::Widgets::Heights::kLabel,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom);
    randomLabel_->setLook(tss::labelLookFromSkin(skin));
    addAndMakeVisible(*randomLabel_);

    const auto allIntParams = ApvtsFactory::getAllIntParameters();
    const auto randomIt = std::find_if(allIntParams.begin(), allIntParams.end(),
        [](const PluginDescriptors::IntParameterDescriptor& desc) {
            return desc.parameterId == PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom;
        });
    
    randomSlider_ = std::make_unique<tss::Slider>(PluginDimensions::Widgets::Widths::Slider::kPatchMutator, PluginDimensions::Widgets::Heights::kSlider, 0.0);
    randomSlider_->setLook(tss::sliderLookFromSkin(skin));
    if (randomIt != allIntParams.end())
    {
        randomSlider_->setRange(static_cast<double>(randomIt->minValue), static_cast<double>(randomIt->maxValue), 1.0);
        randomSlider_->setValue(static_cast<double>(randomIt->defaultValue));
    }
    randomSlider_->setUnit(PluginDisplayNames::Units::kPercent);
    randomSlider_->onValueChange = [this]
    {
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom,
            static_cast<int>(randomSlider_->getValue()),
            nullptr);
    };
    addAndMakeVisible(*randomSlider_);

    retryButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(retryButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry);
    addAndMakeVisible(*retryButton_);

    env1Toggle_ = std::make_unique<tss::Toggle>(
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope1);
    connectToggleToApvts(env1Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope1);
    addAndMakeVisible(*env1Toggle_);

    env2Toggle_ = std::make_unique<tss::Toggle>(
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope2);
    connectToggleToApvts(env2Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope2);
    addAndMakeVisible(*env2Toggle_);

    env3Toggle_ = std::make_unique<tss::Toggle>(
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope3);
    connectToggleToApvts(env3Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope3);
    addAndMakeVisible(*env3Toggle_);

    lfo1Toggle_ = std::make_unique<tss::Toggle>(
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo1);
    connectToggleToApvts(lfo1Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo1);
    addAndMakeVisible(*lfo1Toggle_);

    lfo2Toggle_ = std::make_unique<tss::Toggle>(
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo2);
    connectToggleToApvts(lfo2Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo2);
    addAndMakeVisible(*lfo2Toggle_);
}

void PatchMutatorPanel::setupHistoryLine(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    historyLabel_ = std::make_unique<tss::Label>(
        PluginDimensions::Widgets::Widths::Label::kPatchMutator,
        PluginDimensions::Widgets::Heights::kLabel,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistory);
    historyLabel_->setLook(tss::labelLookFromSkin(skin));
    addAndMakeVisible(*historyLabel_);

    historyComboBox_ = std::make_unique<tss::ComboBox>(
        PluginDimensions::Widgets::Widths::ComboBox::kPatchMutatorHistory,
        PluginDimensions::Widgets::Heights::kComboBox,
        tss::ComboBox::Style::Standard);
    historyComboBox_->setLook(tss::comboBoxLookFromSkin(skin));
    historyComboBox_->setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    historyComboBox_->addItem(PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEmptyHistory, 1);
    historyComboBox_->setSelectedId(1);
    historyComboBox_->onChange = [this]
    {
        if (auto* comboBox = historyComboBox_.get())
        {
            apvts_.state.setProperty(
                PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistory,
                comboBox->getSelectedId(),
                nullptr);
        }
    };
    addAndMakeVisible(*historyComboBox_);

    compareButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(compareButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare);
    addAndMakeVisible(*compareButton_);

    deleteButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(deleteButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete);
    addAndMakeVisible(*deleteButton_);

    clearButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(clearButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear);
    addAndMakeVisible(*clearButton_);

    exportButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(exportButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport);
    addAndMakeVisible(*exportButton_);
}

void PatchMutatorPanel::connectButtonToApvts(tss::Button* button, const char* widgetId)
{
    if (button != nullptr)
    {
        button->onClick = [this, widgetId]
        {
            apvts_.state.setProperty(widgetId, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
        };
    }
}

void PatchMutatorPanel::connectToggleToApvts(tss::Toggle* toggle, const char* widgetId)
{
    if (toggle != nullptr)
    {
        toggle->onStateChange = [this, toggle, widgetId]
        {
            apvts_.state.setProperty(widgetId, toggle->getToggleState(), nullptr);
        };
    }
}

void PatchMutatorPanel::resized()
{
    int x = 0;
    int y = 0;

    layoutModuleHeader(x, y);
    y += PluginDimensions::Widgets::Heights::kModuleHeader + kSpacing_;

    layoutAmountLine(x, y);
    layoutRandomLine(x, y);
    layoutHistoryLine(x, y);
    
    if (amountLabel_)
        amountLabel_->setScalingFactor(scalingFactor_);
    if (amountSlider_)
        amountSlider_->setScalingFactor(scalingFactor_);
    if (mutateButton_)
        mutateButton_->setScalingFactor(scalingFactor_);
    if (randomLabel_)
        randomLabel_->setScalingFactor(scalingFactor_);
    if (randomSlider_)
        randomSlider_->setScalingFactor(scalingFactor_);
    if (retryButton_)
        retryButton_->setScalingFactor(scalingFactor_);
    if (historyLabel_)
        historyLabel_->setScalingFactor(scalingFactor_);
    if (historyComboBox_)
        historyComboBox_->setScalingFactor(scalingFactor_);
    if (compareButton_)
        compareButton_->setScalingFactor(scalingFactor_);
    if (deleteButton_)
        deleteButton_->setScalingFactor(scalingFactor_);
    if (clearButton_)
        clearButton_->setScalingFactor(scalingFactor_);
    if (exportButton_)
        exportButton_->setScalingFactor(scalingFactor_);
}

void PatchMutatorPanel::layoutModuleHeader(int x, int y)
{
    const auto width = PluginDimensions::Widgets::Widths::ModuleHeader::kPatchManagerModule;
    const auto height = PluginDimensions::Widgets::Heights::kModuleHeader;

    if (auto* header = moduleHeader_.get())
        header->setBounds(x, y, width, height);
}

void PatchMutatorPanel::layoutSliderLine(int x, int& y, tss::Label* label, tss::Slider* slider, tss::Button* button, const std::vector<tss::Toggle*>& toggles)
{
    const int widgetY = y;

    if (label != nullptr)
        label->setBounds(x, widgetY, PluginDimensions::Widgets::Widths::Label::kPatchMutator, PluginDimensions::Widgets::Heights::kLabel);
    x += PluginDimensions::Widgets::Widths::Label::kPatchMutator + kSpacing_;

    if (slider != nullptr)
        slider->setBounds(x, widgetY, PluginDimensions::Widgets::Widths::Slider::kPatchMutator, PluginDimensions::Widgets::Heights::kSlider);
    x += PluginDimensions::Widgets::Widths::Slider::kPatchMutator + kSpacing_;

    if (button != nullptr)
        button->setBounds(x, widgetY, button->getWidth(), PluginDimensions::Widgets::Heights::kLabel);
    x += button->getWidth() + kSpacing_;

    for (auto* toggle : toggles)
    {
        if (toggle != nullptr)
            toggle->setBounds(x, widgetY, PluginDimensions::Widgets::Widths::Toggle::kPatchMutator, PluginDimensions::Widgets::Heights::kToggle);
        x += PluginDimensions::Widgets::Widths::Toggle::kPatchMutator + kSpacing_;
    }

    y += PluginDimensions::Widgets::Heights::kLabel + kSpacing_;
}

void PatchMutatorPanel::layoutAmountLine(int x, int& y)
{
    layoutSliderLine(x, y, amountLabel_.get(), amountSlider_.get(), mutateButton_.get(),
        {dco1Toggle_.get(), dco2Toggle_.get(), vcfVcaToggle_.get(), fmTrackToggle_.get(), rampPortamentoToggle_.get()});
}

void PatchMutatorPanel::layoutRandomLine(int x, int& y)
{
    layoutSliderLine(x, y, randomLabel_.get(), randomSlider_.get(), retryButton_.get(),
        {env1Toggle_.get(), env2Toggle_.get(), env3Toggle_.get(), lfo1Toggle_.get(), lfo2Toggle_.get()});
}

void PatchMutatorPanel::layoutHistoryLine(int x, int& y)
{
    const int widgetY = y;

    if (auto* label = historyLabel_.get())
        label->setBounds(x, widgetY, PluginDimensions::Widgets::Widths::Label::kPatchMutator, PluginDimensions::Widgets::Heights::kLabel);
    x += PluginDimensions::Widgets::Widths::Label::kPatchMutator + kSpacing_;

    if (auto* comboBox = historyComboBox_.get())
        comboBox->setBounds(x, widgetY, PluginDimensions::Widgets::Widths::ComboBox::kPatchMutatorHistory, PluginDimensions::Widgets::Heights::kComboBox);
    x += PluginDimensions::Widgets::Widths::ComboBox::kPatchMutatorHistory + kSpacing_;

    if (auto* button = compareButton_.get())
        button->setBounds(x, widgetY, button->getWidth(), PluginDimensions::Widgets::Heights::kLabel);
    x += compareButton_->getWidth() + kSpacing_;

    if (auto* button = deleteButton_.get())
        button->setBounds(x, widgetY, button->getWidth(), PluginDimensions::Widgets::Heights::kLabel);
    x += deleteButton_->getWidth() + kSpacing_;

    if (auto* button = clearButton_.get())
        button->setBounds(x, widgetY, button->getWidth(), PluginDimensions::Widgets::Heights::kLabel);
    x += clearButton_->getWidth() + kSpacing_;

    if (auto* button = exportButton_.get())
        button->setBounds(x, widgetY, button->getWidth(), PluginDimensions::Widgets::Heights::kLabel);

    y += PluginDimensions::Widgets::Heights::kLabel;
}

void PatchMutatorPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    propagateSkinsToControlWidgets(skin);
    propagateSkinsToToggleWidgets(skin);
}

void PatchMutatorPanel::setScalingFactor(float scalingFactor)
{
    if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
        return;
    
    scalingFactor_ = scalingFactor;
    resized();
    repaint();
}

void PatchMutatorPanel::propagateSkinsToControlWidgets(tss::ISkin& skin)
{
    if (moduleHeader_)
        moduleHeader_->setLook(tss::moduleHeaderLookFromSkin(skin));

    if (amountLabel_)
        amountLabel_->setLook(tss::labelLookFromSkin(skin));
    if (amountSlider_)
        amountSlider_->setLook(tss::sliderLookFromSkin(skin));
    if (randomLabel_)
        randomLabel_->setLook(tss::labelLookFromSkin(skin));
    if (randomSlider_)
        randomSlider_->setLook(tss::sliderLookFromSkin(skin));
    if (historyLabel_)
        historyLabel_->setLook(tss::labelLookFromSkin(skin));
    if (historyComboBox_)
        historyComboBox_->setLook(tss::comboBoxLookFromSkin(skin));
        historyComboBox_->setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    if (mutateButton_)
        mutateButton_->setLook(tss::buttonLookFromSkin(skin));
    if (retryButton_)
        retryButton_->setLook(tss::buttonLookFromSkin(skin));
    if (compareButton_)
        compareButton_->setLook(tss::buttonLookFromSkin(skin));
    if (deleteButton_)
        deleteButton_->setLook(tss::buttonLookFromSkin(skin));
    if (clearButton_)
        clearButton_->setLook(tss::buttonLookFromSkin(skin));
    if (exportButton_)
        exportButton_->setLook(tss::buttonLookFromSkin(skin));
}

void PatchMutatorPanel::propagateSkinsToToggleWidgets(tss::ISkin& skin)
{
    const auto toggleLook = tss::toggleLookFromSkin(skin);
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
