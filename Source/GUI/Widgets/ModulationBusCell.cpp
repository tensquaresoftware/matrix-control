#include "ModulationBusCell.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/HorizontalSeparator.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "GUI/Factories/WidgetFactory.h"

namespace
{
constexpr float kReorderDragSourcePlaceholderAlpha = 0.12f;
constexpr float kReorderDropTargetRedAlpha         = 0.25f;
}

ModulationBusCell::~ModulationBusCell() = default;

ModulationBusCell::ModulationBusCell(TSS::ISkin& skin,
                                      int width,
                                      int height,
                                      const ModulationBusCellDimensions& dimensions,
                                      int busNumber,
                                      WidgetFactory& factory,
                                      juce::AudioProcessorValueTreeState& apvts,
                                      const juce::String& sourceParamId,
                                      const juce::String& amountParamId,
                                      const juce::String& destinationParamId,
                                      const juce::String& busId)
    : busNumber_(busNumber)
    , dimensions_(dimensions)
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

    resized();
}

void ModulationBusCell::createBusNumberLabel(int busNumber, TSS::ISkin& skin)
{
    busNumberLabel_ = std::make_unique<TSS::Label>(
        dimensions_.busNumberLabelWidth,
        dimensions_.busNumberLabelHeight,
        TSS::labelLookFromSkin(skin),
        juce::String(busNumber));
    busNumberLabel_->setInterceptsMouseClicks(false, false);
    addAndMakeVisible(*busNumberLabel_);
}

void ModulationBusCell::createSourceComboBox(WidgetFactory& factory, TSS::ISkin& skin, const juce::String& sourceParamId, juce::AudioProcessorValueTreeState& apvts)
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

void ModulationBusCell::createAmountSlider(WidgetFactory& factory, TSS::ISkin& skin, const juce::String& amountParamId, juce::AudioProcessorValueTreeState& apvts)
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

void ModulationBusCell::createDestinationComboBox(int busNumber, TSS::ISkin& skin, const juce::String& destinationParamId, juce::AudioProcessorValueTreeState& apvts)
{
    const auto busNumberAsSizeT = static_cast<size_t>(busNumber);
    const auto& destinationDesc = PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[busNumberAsSizeT][1];

    destinationComboBox_ = std::make_unique<TSS::ComboBox>(
        dimensions_.destinationComboBoxWidth,
        dimensions_.destinationComboBoxHeight,
        TSS::comboBoxLookFromSkin(skin));
    destinationComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
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

void ModulationBusCell::createInitButton(TSS::ISkin& skin, int busNumber)
{
    initButton_ = std::make_unique<TSS::Button>(
        dimensions_.initButtonWidth,
        dimensions_.initButtonHeight,
        TSS::buttonLookFromSkin(skin),
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

void ModulationBusCell::createSeparator(TSS::ISkin& skin)
{
    separator_ = std::make_unique<TSS::HorizontalSeparator>(
        dimensions_.separatorWidth,
        dimensions_.separatorHeight,
        TSS::horizontalSeparatorLookFromSkin(skin));
    addAndMakeVisible(*separator_);
}

void ModulationBusCell::setReorderDragCallbacks(ReorderDragBeginFn onBegin,
                                                ReorderDragMoveFn onMove,
                                                ReorderDragEndFn onEnd)
{
    onReorderDragBegin_ = std::move(onBegin);
    onReorderDragMove_ = std::move(onMove);
    onReorderDragEnd_ = std::move(onEnd);
}

void ModulationBusCell::setDropTargetHighlighted(bool highlighted)
{
    if (dropTargetHighlighted_ == highlighted)
        return;

    dropTargetHighlighted_ = highlighted;
    repaint();
}

void ModulationBusCell::setDragSourceHighlighted(bool highlighted)
{
    if (dragSourceHighlighted_ == highlighted)
        return;

    dragSourceHighlighted_ = highlighted;
    repaint();
}

void ModulationBusCell::paint(juce::Graphics& g)
{
    if (!dropTargetHighlighted_ && !dragSourceHighlighted_)
        return;

    const auto bounds = getLocalBounds().toFloat();
    const juce::Colour highlightColour = dragSourceHighlighted_
        ? juce::Colours::white.withAlpha(kReorderDragSourcePlaceholderAlpha)
        : juce::Colour(ColourChart::kRed).withAlpha(kReorderDropTargetRedAlpha);
    g.setColour(highlightColour);
    g.fillRect(bounds);
}

void ModulationBusCell::mouseDown(const juce::MouseEvent& e)
{
    if (!isBusNumberLabelHit(e.getPosition()))
        return;

    reorderDragPending_ = true;
    reorderDragActive_ = false;
    dragStartPosition_ = e.getPosition();
}

void ModulationBusCell::mouseDrag(const juce::MouseEvent& e)
{
    if (!reorderDragPending_ && !reorderDragActive_)
        return;

    if (!reorderDragActive_
        && e.getPosition().toFloat().getDistanceFrom(dragStartPosition_.toFloat())
            < static_cast<float>(dimensions_.reorderDragThreshold) * uiScale_)
    {
        return;
    }

    if (!reorderDragActive_)
    {
        reorderDragActive_ = true;
        if (onReorderDragBegin_)
            onReorderDragBegin_(busNumber_);
    }

    if (onReorderDragMove_)
        onReorderDragMove_(toPanelPosition(e.getPosition()));
}

void ModulationBusCell::mouseUp(const juce::MouseEvent& e)
{
    finishReorderDrag(e.getPosition());
}

void ModulationBusCell::finishReorderDrag(juce::Point<int> localPosition)
{
    if (reorderDragActive_)
    {
        if (onReorderDragEnd_)
            onReorderDragEnd_(toPanelPosition(localPosition));
    }

    reorderDragPending_ = false;
    reorderDragActive_ = false;
}

bool ModulationBusCell::isBusNumberLabelHit(juce::Point<int> localPosition) const
{
    if (busNumberLabel_ == nullptr)
        return false;

    return busNumberLabel_->getBounds().contains(localPosition);
}

juce::Point<int> ModulationBusCell::toPanelPosition(juce::Point<int> localPosition) const
{
    if (auto* panel = getParentComponent())
        return panel->getLocalPoint(this, localPosition);

    return localPosition;
}

void ModulationBusCell::resized()
{
    const float sf = uiScale_;
    const int h = getHeight();
    const int labelH = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.busNumberLabelHeight), sf);
    const int sourceH = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.sourceComboBoxHeight), sf);
    const int amountH = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.amountSliderHeight), sf);
    const int destH = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.destinationComboBoxHeight), sf);
    const int initH = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.initButtonHeight), sf);
    const int rowH = juce::jmax(labelH, juce::jmax(sourceH, juce::jmax(amountH, juce::jmax(destH, initH))));
    const int sepH = juce::jmax(1, TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.separatorHeight), sf));
    const int ySep = juce::jmin(rowH, juce::jmax(0, h - sepH));

    layoutWidgetRow();
    layoutSeparator(ySep, sepH);

    if (busNumberLabel_)
        busNumberLabel_->setUiScale(uiScale_);
    if (sourceComboBox_)
        sourceComboBox_->setUiScale(uiScale_);
    if (amountSlider_)
        amountSlider_->setUiScale(uiScale_);
    if (destinationComboBox_)
        destinationComboBox_->setUiScale(uiScale_);
    if (initButton_)
        initButton_->setUiScale(uiScale_);
    if (separator_)
        separator_->setUiScale(uiScale_);
}

void ModulationBusCell::layoutWidgetRow()
{
    const float sf = uiScale_;
    const int y = 0;

    const int labelW  = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.busNumberLabelWidth), sf);
    const int labelH  = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.busNumberLabelHeight), sf);
    const int sourceW = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.sourceComboBoxWidth), sf);
    const int sourceH = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.sourceComboBoxHeight), sf);
    const int amountW = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.amountSliderWidth), sf);
    const int amountH = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.amountSliderHeight), sf);
    const int destW   = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.destinationComboBoxWidth), sf);
    const int destH   = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.destinationComboBoxHeight), sf);
    const int initW   = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.initButtonWidth), sf);
    const int initH   = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.initButtonHeight), sf);

    const float gap = static_cast<float>(dimensions_.interControlGap) * sf;
    const float sourceX = (static_cast<float>(dimensions_.busNumberLabelWidth)
        + static_cast<float>(dimensions_.interControlGap)) * sf;
    const float amountX = sourceX + static_cast<float>(dimensions_.sourceComboBoxWidth) * sf + gap;
    const float destX   = amountX + static_cast<float>(dimensions_.amountSliderWidth) * sf + gap;
    const float initX   = destX   + static_cast<float>(dimensions_.destinationComboBoxWidth) * sf + gap;

    if (auto* label   = busNumberLabel_.get())     label->setBounds(0, y, labelW, labelH);
    if (auto* combo   = sourceComboBox_.get())     combo->setBounds(juce::roundToInt(sourceX), y, sourceW, sourceH);
    if (auto* slider  = amountSlider_.get())       slider->setBounds(juce::roundToInt(amountX), y, amountW, amountH);
    if (auto* combo   = destinationComboBox_.get()) combo->setBounds(juce::roundToInt(destX), y, destW, destH);
    if (auto* button  = initButton_.get())         button->setBounds(juce::roundToInt(initX), y, initW, initH);
}

void ModulationBusCell::layoutSeparator(int yTop, int separatorHeight)
{
    const int sepW = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.separatorWidth), uiScale_);

    if (auto* separator = separator_.get())
        separator->setBounds(0, yTop, sepW, separatorHeight);
}

void ModulationBusCell::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    if (busNumberLabel_)
        busNumberLabel_->setLook(TSS::labelLookFromSkin(skin));
    if (sourceComboBox_)
    {
        sourceComboBox_->setLook(TSS::comboBoxLookFromSkin(skin));
        sourceComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    }
    if (amountSlider_)
        amountSlider_->setLook(TSS::sliderLookFromSkin(skin));
    if (destinationComboBox_)
    {
        destinationComboBox_->setLook(TSS::comboBoxLookFromSkin(skin));
        destinationComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    }
    if (initButton_)
        initButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (separator_)
        separator_->setLook(TSS::horizontalSeparatorLookFromSkin(skin));
}

void ModulationBusCell::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    resized();
    repaint();
}
