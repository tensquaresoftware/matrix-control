#include "ModulationBusPanel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
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
        dimensions_.busNumberLabelWidth,
        dimensions_.busNumberLabelHeight,
        juce::String(busNumber));
    busNumberLabel_->setLook(tss::labelLookFromSkin(skin));
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
        dimensions_.destinationComboBoxWidth,
        dimensions_.destinationComboBoxHeight);
    destinationComboBox_->setLook(tss::comboBoxLookFromSkin(skin));
    destinationComboBox_->setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
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
        dimensions_.initButtonWidth,
        dimensions_.initButtonHeight,
        PluginDisplayNames::ShortLabels::kInit);
    initButton_->setLook(tss::buttonLookFromSkin(skin));
    
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
        dimensions_.separatorWidth,
        dimensions_.separatorHeight);
    separator_->setLook(tss::horizontalSeparatorLookFromSkin(skin));
    addAndMakeVisible(*separator_);
}

void ModulationBusPanel::resized()
{
    const int widgetRowHeight = juce::roundToInt(
        static_cast<float>(dimensions_.busNumberLabelHeight) * scalingFactor_);
    int y = 0;

    layoutWidgetRow();
    y += widgetRowHeight;
    layoutSeparator(y);

    if (busNumberLabel_)
        busNumberLabel_->setScalingFactor(scalingFactor_);
    if (sourceComboBox_)
        sourceComboBox_->setScalingFactor(scalingFactor_);
    if (amountSlider_)
        amountSlider_->setScalingFactor(scalingFactor_);
    if (destinationComboBox_)
        destinationComboBox_->setScalingFactor(scalingFactor_);
    if (initButton_)
        initButton_->setScalingFactor(scalingFactor_);
    if (separator_)
        separator_->setScalingFactor(scalingFactor_);
}

void ModulationBusPanel::layoutWidgetRow()
{
    const float sf = scalingFactor_;
    const int y = 0;

    const int labelW  = juce::roundToInt(static_cast<float>(dimensions_.busNumberLabelWidth) * sf);
    const int labelH  = juce::roundToInt(static_cast<float>(dimensions_.busNumberLabelHeight) * sf);
    const int sourceW = juce::roundToInt(static_cast<float>(dimensions_.sourceComboBoxWidth) * sf);
    const int sourceH = juce::roundToInt(static_cast<float>(dimensions_.sourceComboBoxHeight) * sf);
    const int amountW = juce::roundToInt(static_cast<float>(dimensions_.amountSliderWidth) * sf);
    const int amountH = juce::roundToInt(static_cast<float>(dimensions_.amountSliderHeight) * sf);
    const int destW   = juce::roundToInt(static_cast<float>(dimensions_.destinationComboBoxWidth) * sf);
    const int destH   = juce::roundToInt(static_cast<float>(dimensions_.destinationComboBoxHeight) * sf);
    const int initW   = juce::roundToInt(static_cast<float>(dimensions_.initButtonWidth) * sf);
    const int initH   = juce::roundToInt(static_cast<float>(dimensions_.initButtonHeight) * sf);

    // X positions computed independently from float origin to avoid rounding accumulation.
    // Label has no spacing before source (they abut directly).
    const float sourceX = static_cast<float>(dimensions_.busNumberLabelWidth) * sf;
    const float amountX = sourceX + static_cast<float>(dimensions_.sourceComboBoxWidth + kSpacing_) * sf;
    const float destX   = amountX + static_cast<float>(dimensions_.amountSliderWidth + kSpacing_) * sf;
    const float initX   = destX   + static_cast<float>(dimensions_.destinationComboBoxWidth + kSpacing_) * sf;

    if (auto* label   = busNumberLabel_.get())     label->setBounds(0, y, labelW, labelH);
    if (auto* combo   = sourceComboBox_.get())     combo->setBounds(juce::roundToInt(sourceX), y, sourceW, sourceH);
    if (auto* slider  = amountSlider_.get())       slider->setBounds(juce::roundToInt(amountX), y, amountW, amountH);
    if (auto* combo   = destinationComboBox_.get()) combo->setBounds(juce::roundToInt(destX), y, destW, destH);
    if (auto* button  = initButton_.get())         button->setBounds(juce::roundToInt(initX), y, initW, initH);
}

void ModulationBusPanel::layoutSeparator(int y)
{
    const int sepW = juce::roundToInt(static_cast<float>(dimensions_.separatorWidth) * scalingFactor_);
    const int sepH = juce::roundToInt(static_cast<float>(dimensions_.separatorHeight) * scalingFactor_);

    if (auto* separator = separator_.get())
        separator->setBounds(0, y, sepW, sepH);
}

void ModulationBusPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    if (busNumberLabel_)
        busNumberLabel_->setLook(tss::labelLookFromSkin(skin));
    if (sourceComboBox_)
    {
        sourceComboBox_->setLook(tss::comboBoxLookFromSkin(skin));
        sourceComboBox_->setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    }
    if (amountSlider_)
        amountSlider_->setLook(tss::sliderLookFromSkin(skin));
    if (destinationComboBox_)
    {
        destinationComboBox_->setLook(tss::comboBoxLookFromSkin(skin));
        destinationComboBox_->setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    }
    if (initButton_)
        initButton_->setLook(tss::buttonLookFromSkin(skin));
    if (separator_)
        separator_->setLook(tss::horizontalSeparatorLookFromSkin(skin));
}

void ModulationBusPanel::setScalingFactor(float scalingFactor)
{
    if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
        return;
    
    scalingFactor_ = scalingFactor;
    repaint();
}

