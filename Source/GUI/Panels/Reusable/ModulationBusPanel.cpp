#include "ModulationBusPanel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/HorizontalSeparator.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulationBusPanel::~ModulationBusPanel() = default;

ModulationBusPanel::ModulationBusPanel(tss::ISkin& skin,
                                      int width,
                                      int height,
                                      const ModulationBusPanelDimensions& dimensions,
                                      int busNumber,
                                      WidgetFactory& factory,
                                      juce::AudioProcessorValueTreeState& apvts,
                                      const juce::String& sourceParamId,
                                      const juce::String& amountParamId,
                                      const juce::String& destinationParamId,
                                      const juce::String& busId)
    : dimensions_(dimensions)
    , skin_(&skin)
    , apvts_(apvts)
    , busId_(busId)
{
    setOpaque(false);
    setSize(width, height);
    createBusNumberLabel(busNumber, skin);
    createSourceComboBox(factory, skin, sourceParamId, apvts);
    createAmountSlider(factory, skin, amountParamId, apvts);
    createDestinationComboBox(busNumber, skin, destinationParamId, apvts);
    createInitButton(skin, busNumber);
    createSeparator(skin);

    layoutWidgetRow();
    layoutSeparator(dimensions_.busNumberLabelHeight);
}

void ModulationBusPanel::createBusNumberLabel(int busNumber, tss::ISkin& skin)
{
    busNumberLabel_ = std::make_unique<tss::Label>(
        skin,
        dimensions_.busNumberLabelWidth,
        dimensions_.busNumberLabelHeight,
        juce::String(busNumber));
    addAndMakeVisible(*busNumberLabel_);
}

void ModulationBusPanel::createSourceComboBox(WidgetFactory& factory, tss::ISkin& skin, const juce::String& sourceParamId, juce::AudioProcessorValueTreeState& apvts)
{
    sourceComboBox_ = factory.createChoiceParameterComboBox(
        sourceParamId,
        skin,
        dimensions_.sourceComboBoxWidth,
        dimensions_.sourceComboBoxHeight);
    sourceAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts,
        sourceParamId,
        *sourceComboBox_);
    addAndMakeVisible(*sourceComboBox_);
}

void ModulationBusPanel::createAmountSlider(WidgetFactory& factory, tss::ISkin& skin, const juce::String& amountParamId, juce::AudioProcessorValueTreeState& apvts)
{
    amountSlider_ = factory.createIntParameterSlider(
        amountParamId,
        skin,
        dimensions_.amountSliderWidth,
        dimensions_.amountSliderHeight);
    amountAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts,
        amountParamId,
        *amountSlider_);
    addAndMakeVisible(*amountSlider_);
}

void ModulationBusPanel::createDestinationComboBox(int busNumber, tss::ISkin& skin, const juce::String& destinationParamId, juce::AudioProcessorValueTreeState& apvts)
{
    const auto busNumberAsSizeT = static_cast<size_t>(busNumber);
    const auto& destinationDesc = PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[busNumberAsSizeT][1];

    destinationComboBox_ = std::make_unique<tss::ComboBox>(
        skin,
        dimensions_.destinationComboBoxWidth,
        dimensions_.destinationComboBoxHeight);
    for (const auto& choice : destinationDesc.choices)
    {
        destinationComboBox_->addItem(choice, destinationComboBox_->getNumItems() + 1);
    }
    destinationComboBox_->setSelectedItemIndex(destinationDesc.defaultIndex);
    destinationAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts,
        destinationParamId,
        *destinationComboBox_);
    addAndMakeVisible(*destinationComboBox_);
}

void ModulationBusPanel::createInitButton(tss::ISkin& skin, int busNumber)
{
    initButton_ = std::make_unique<tss::Button>(
        skin,
        dimensions_.initButtonWidth,
        dimensions_.initButtonHeight,
        PluginDisplayNames::ShortLabels::kInit);
    
    juce::String initBusId;
    switch (busNumber)
    {
        case 0: initBusId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus0Init; break;
        case 1: initBusId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus1Init; break;
        case 2: initBusId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus2Init; break;
        case 3: initBusId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus3Init; break;
        case 4: initBusId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus4Init; break;
        case 5: initBusId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus5Init; break;
        case 6: initBusId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus6Init; break;
        case 7: initBusId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus7Init; break;
        case 8: initBusId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus8Init; break;
        case 9: initBusId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus9Init; break;
        default: return;
    }
    
    initButton_->onClick = [this, initBusId]
    {
        apvts_.state.setProperty(initBusId, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
    };
    addAndMakeVisible(*initButton_);
}

void ModulationBusPanel::createSeparator(tss::ISkin& skin)
{
    separator_ = std::make_unique<tss::HorizontalSeparator>(
        skin,
        dimensions_.separatorWidth,
        dimensions_.separatorHeight);
    addAndMakeVisible(*separator_);
}

void ModulationBusPanel::resized()
{
    const auto widgetRowHeight = dimensions_.busNumberLabelHeight;
    int y = 0;

    layoutWidgetRow();
    y += widgetRowHeight;
    layoutSeparator(y);
}

void ModulationBusPanel::layoutWidgetRow()
{
    const int y = 0;
    int x = 0;

    if (auto* label = busNumberLabel_.get())
        label->setBounds(x, y, dimensions_.busNumberLabelWidth, dimensions_.busNumberLabelHeight);
    x += dimensions_.busNumberLabelWidth;

    if (auto* comboBox = sourceComboBox_.get())
        comboBox->setBounds(x, y, dimensions_.sourceComboBoxWidth, dimensions_.sourceComboBoxHeight);
    x += dimensions_.sourceComboBoxWidth + kSpacing_;

    if (auto* slider = amountSlider_.get())
        slider->setBounds(x, y, dimensions_.amountSliderWidth, dimensions_.amountSliderHeight);
    x += dimensions_.amountSliderWidth + kSpacing_;

    if (auto* comboBox = destinationComboBox_.get())
        comboBox->setBounds(x, y, dimensions_.destinationComboBoxWidth, dimensions_.destinationComboBoxHeight);
    x += dimensions_.destinationComboBoxWidth + kSpacing_;

    if (auto* button = initButton_.get())
        button->setBounds(x, y, dimensions_.initButtonWidth, dimensions_.initButtonHeight);
}

void ModulationBusPanel::layoutSeparator(int y)
{
    if (auto* separator = separator_.get())
        separator->setBounds(0, y, dimensions_.separatorWidth, dimensions_.separatorHeight);
}

void ModulationBusPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    tss::propagateSkin(skin,
        busNumberLabel_.get(),
        sourceComboBox_.get(),
        amountSlider_.get(),
        destinationComboBox_.get(),
        initButton_.get(),
        separator_.get());
}

