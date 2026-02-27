#include "ModulationBusPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/HorizontalSeparator.h"
#include "Shared/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulationBusPanel::~ModulationBusPanel() = default;

ModulationBusPanel::ModulationBusPanel(int busNumber,
                                      WidgetFactory& factory,
                                      tss::Skin& skin,
                                      juce::AudioProcessorValueTreeState& apvts,
                                      const juce::String& sourceParamId,
                                      const juce::String& amountParamId,
                                      const juce::String& destinationParamId,
                                      const juce::String& busId)
    : skin_(&skin)
    , apvts_(apvts)
    , busId_(busId)
{
    setOpaque(false);
    createBusNumberLabel(busNumber, skin);
    createSourceComboBox(factory, skin, sourceParamId, apvts);
    createAmountSlider(factory, skin, amountParamId, apvts);
    createDestinationComboBox(busNumber, skin, destinationParamId, apvts);
    createInitButton(skin, busNumber);
    createSeparator(skin);
}

void ModulationBusPanel::createBusNumberLabel(int busNumber, tss::Skin& skin)
{
    busNumberLabel_ = std::make_unique<tss::Label>(
        skin,
        PluginDimensions::Widgets::Widths::Label::kModulationBusNumber,
        PluginDimensions::Widgets::Heights::kLabel,
        juce::String(busNumber));
    addAndMakeVisible(*busNumberLabel_);
}

void ModulationBusPanel::createSourceComboBox(WidgetFactory& factory, tss::Skin& skin, const juce::String& sourceParamId, juce::AudioProcessorValueTreeState& apvts)
{
    sourceComboBox_ = factory.createChoiceParameterComboBox(
        sourceParamId,
        skin,
        PluginDimensions::Widgets::Widths::ComboBox::kMatrixModulationSource,
        PluginDimensions::Widgets::Heights::kComboBox);
    sourceAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts,
        sourceParamId,
        *sourceComboBox_);
    addAndMakeVisible(*sourceComboBox_);
}

void ModulationBusPanel::createAmountSlider(WidgetFactory& factory, tss::Skin& skin, const juce::String& amountParamId, juce::AudioProcessorValueTreeState& apvts)
{
    amountSlider_ = factory.createIntParameterSlider(amountParamId, skin);
    amountAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts,
        amountParamId,
        *amountSlider_);
    addAndMakeVisible(*amountSlider_);
}

void ModulationBusPanel::createDestinationComboBox(int busNumber, tss::Skin& skin, const juce::String& destinationParamId, juce::AudioProcessorValueTreeState& apvts)
{
    const auto busNumberAsSizeT = static_cast<size_t>(busNumber);
    const auto& destinationDesc = PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[busNumberAsSizeT][1];

    destinationComboBox_ = std::make_unique<tss::ComboBox>(
        skin,
        PluginDimensions::Widgets::Widths::ComboBox::kMatrixModulationDestination,
        PluginDimensions::Widgets::Heights::kComboBox);
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

void ModulationBusPanel::createInitButton(tss::Skin& skin, int busNumber)
{
    initButton_ = std::make_unique<tss::Button>(
        skin,
        PluginDimensions::Widgets::Widths::Button::kInit,
        PluginDimensions::Widgets::Heights::kButton,
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

void ModulationBusPanel::createSeparator(tss::Skin& skin)
{
    separator_ = std::make_unique<tss::HorizontalSeparator>(
        skin,
        PluginDimensions::Widgets::Widths::HorizontalSeparator::kMatrixModulationBus,
        PluginDimensions::Widgets::Heights::kHorizontalSeparator);
    addAndMakeVisible(*separator_);
}

void ModulationBusPanel::resized()
{
    const auto widgetRowHeight = PluginDimensions::Widgets::Heights::kLabel;
    int y = 0;

    layoutWidgetRow();
    y += widgetRowHeight;
    layoutSeparator(y);
}

void ModulationBusPanel::layoutWidgetRow()
{
    const auto busNumberLabelWidth = PluginDimensions::Widgets::Widths::Label::kModulationBusNumber;
    const auto busNumberLabelHeight = PluginDimensions::Widgets::Heights::kLabel;
    const auto sourceComboBoxWidth = PluginDimensions::Widgets::Widths::ComboBox::kMatrixModulationSource;
    const auto sourceComboBoxHeight = PluginDimensions::Widgets::Heights::kComboBox;
    const auto amountSliderWidth = PluginDimensions::Widgets::Widths::Slider::kStandard;
    const auto amountSliderHeight = PluginDimensions::Widgets::Heights::kSlider;
    const auto destinationComboBoxWidth = PluginDimensions::Widgets::Widths::ComboBox::kMatrixModulationDestination;
    const auto destinationComboBoxHeight = PluginDimensions::Widgets::Heights::kComboBox;
    const auto initButtonWidth = PluginDimensions::Widgets::Widths::Button::kInit;
    const auto initButtonHeight = PluginDimensions::Widgets::Heights::kButton;
    const int y = 0;

    int x = 0;

    if (auto* label = busNumberLabel_.get())
        label->setBounds(x, y, busNumberLabelWidth, busNumberLabelHeight);
    x += busNumberLabelWidth;

    if (auto* comboBox = sourceComboBox_.get())
        comboBox->setBounds(x, y, sourceComboBoxWidth, sourceComboBoxHeight);
    x += sourceComboBoxWidth + kSpacing_;

    if (auto* slider = amountSlider_.get())
        slider->setBounds(x, y, amountSliderWidth, amountSliderHeight);
    x += amountSliderWidth + kSpacing_;

    if (auto* comboBox = destinationComboBox_.get())
        comboBox->setBounds(x, y, destinationComboBoxWidth, destinationComboBoxHeight);
    x += destinationComboBoxWidth + kSpacing_;

    if (auto* button = initButton_.get())
        button->setBounds(x, y, initButtonWidth, initButtonHeight);
}

void ModulationBusPanel::layoutSeparator(int y)
{
    const auto separatorWidth = PluginDimensions::Widgets::Widths::HorizontalSeparator::kMatrixModulationBus;
    const auto separatorHeight = PluginDimensions::Widgets::Heights::kHorizontalSeparator;

    if (auto* separator = separator_.get())
        separator->setBounds(0, y, separatorWidth, separatorHeight);
}

void ModulationBusPanel::setSkin(tss::Skin& skin)
{
    skin_ = &skin;

    if (auto* label = busNumberLabel_.get())
        label->setSkin(skin);

    if (auto* comboBox = sourceComboBox_.get())
        comboBox->setSkin(skin);

    if (auto* slider = amountSlider_.get())
        slider->setSkin(skin);

    if (auto* comboBox = destinationComboBox_.get())
        comboBox->setSkin(skin);

    if (auto* button = initButton_.get())
        button->setSkin(skin);

    if (auto* separator = separator_.get())
        separator->setSkin(skin);
}

int ModulationBusPanel::getHeight()
{
    return PluginDimensions::Widgets::Heights::kLabel + PluginDimensions::Widgets::Heights::kHorizontalSeparator;
}
