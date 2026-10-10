#include "ModulationBusCell.h"
#include "ModulationBusRowLayout.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Helpers/ApvtsUndoableParameterAttachments.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/HorizontalSeparator.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "GUI/Factories/WidgetFactory.h"

ModulationBusCell::~ModulationBusCell()
{
    if (busNumberLabel_ != nullptr)
        busNumberLabel_->removeMouseListener(this);
}

ModulationBusCell::ModulationBusCell(const Config& config)
    : busNumber_(config.busNumber)
    , dimensions_(config.dimensions)
    , skin_(&config.skin)
    , apvts_(config.apvts)
    , busId_(config.busId)
{
    setOpaque(false);
    setSize(config.width, config.height);
    createBusNumberLabel();
    createSourceComboBox(config.factory, config.sourceParamId);
    createAmountSlider(config.factory, config.amountParamId);
    createDestinationComboBox(config.destinationParamId);
    createInitButton();
    createSeparator();

    resized();
}

void ModulationBusCell::createBusNumberLabel()
{
    jassert(skin_ != nullptr);

    busNumberLabel_ = std::make_unique<TSS::Label>(
        dimensions_.busNumberLabelWidth,
        dimensions_.busNumberLabelHeight,
        TSS::labelLookFromSkin(*skin_),
        juce::String(busNumber_));
    // Intercept so ContextualHelpBinder can see hover; forward reorder via mouse listener.
    busNumberLabel_->setInterceptsMouseClicks(true, false);
    busNumberLabel_->addMouseListener(this, false);
    addAndMakeVisible(*busNumberLabel_);
}

void ModulationBusCell::createSourceComboBox(WidgetFactory& factory, const juce::String& sourceParamId)
{
    jassert(skin_ != nullptr);

    sourceComboBox_ = factory.createChoiceParameterComboBox(
        sourceParamId,
        *skin_,
        dimensions_.sourceComboBoxWidth,
        dimensions_.sourceComboBoxHeight);
    sourceAttachment_ = std::make_unique<TSS::ApvtsUndoableComboBoxAttachment>(
        apvts_,
        sourceParamId,
        *sourceComboBox_);
    addAndMakeVisible(*sourceComboBox_);
}

void ModulationBusCell::createAmountSlider(WidgetFactory& factory, const juce::String& amountParamId)
{
    jassert(skin_ != nullptr);

    amountSlider_ = factory.createIntParameterSlider(
        amountParamId,
        *skin_,
        dimensions_.amountSliderWidth,
        dimensions_.amountSliderHeight);
    amountAttachment_ = std::make_unique<TSS::ApvtsUndoableSliderAttachment>(
        apvts_,
        amountParamId,
        *amountSlider_);

    addAndMakeVisible(*amountSlider_);
}

void ModulationBusCell::createDestinationComboBox(const juce::String& destinationParamId)
{
    jassert(skin_ != nullptr);

    const auto busNumberAsSizeT = static_cast<size_t>(busNumber_);
    const auto& destinationDesc = PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[busNumberAsSizeT][1];

    destinationComboBox_ = std::make_unique<TSS::ComboBox>(
        dimensions_.destinationComboBoxWidth,
        dimensions_.destinationComboBoxHeight,
        TSS::comboBoxLookFromSkin(*skin_));
    destinationComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(*skin_));
    for (const auto& choice : destinationDesc.choices)
    {
        destinationComboBox_->addItem(choice, destinationComboBox_->getNumItems() + 1);
    }
    destinationComboBox_->setSelectedItemIndex(destinationDesc.defaultIndex);
    destinationAttachment_ = std::make_unique<TSS::ApvtsUndoableComboBoxAttachment>(
        apvts_,
        destinationParamId,
        *destinationComboBox_);
    addAndMakeVisible(*destinationComboBox_);
}

void ModulationBusCell::createInitButton()
{
    jassert(skin_ != nullptr);

    initButton_ = std::make_unique<TSS::Button>(
        dimensions_.initButtonWidth,
        dimensions_.initButtonHeight,
        TSS::buttonLookFromSkin(*skin_),
        PluginDisplayNames::ShortLabels::kInit);

    juce::String initBusId;
    switch (busNumber_)
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

void ModulationBusCell::createSeparator()
{
    jassert(skin_ != nullptr);

    separator_ = std::make_unique<TSS::HorizontalSeparator>(
        dimensions_.separatorWidth,
        dimensions_.separatorHeight,
        TSS::horizontalSeparatorLookFromSkin(*skin_));
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
    updateBusNumberLabelWeight();
    repaint();
}

void ModulationBusCell::setDragSourceHighlighted(bool highlighted)
{
    if (dragSourceHighlighted_ == highlighted)
        return;

    dragSourceHighlighted_ = highlighted;
    updateBusNumberLabelWeight();
    repaint();
}

void ModulationBusCell::updateBusNumberLabelWeight()
{
    if (busNumberLabel_ == nullptr || skin_ == nullptr)
        return;

    auto look = TSS::labelLookFromSkin(*skin_);
    if (dragSourceHighlighted_ || dropTargetHighlighted_)
        look.font = skin_->getBaseFontBold().withHeight(look.font.getHeight());

    busNumberLabel_->setLook(look);
}

void ModulationBusCell::paint(juce::Graphics& g)
{
    if (!dropTargetHighlighted_ && !dragSourceHighlighted_)
        return;

    if (busNumberLabel_ == nullptr)
        return;

    const auto bounds = busNumberLabel_->getBounds().toFloat();
    const juce::Colour highlightColour = dragSourceHighlighted_
        ? juce::Colour(ColourChart::kBlue).withAlpha(0.75f)
        : juce::Colour(ColourChart::kRed).withAlpha(0.75f);
    g.setColour(highlightColour);
    g.fillRect(bounds);
}

void ModulationBusCell::mouseDown(const juce::MouseEvent& e)
{
    const auto pos = e.getEventRelativeTo(this).getPosition();
    if (!isBusNumberLabelHit(pos))
        return;

    reorderDragPending_ = true;
    reorderDragActive_ = false;
    dragStartPosition_ = pos;
}

void ModulationBusCell::mouseDrag(const juce::MouseEvent& e)
{
    if (!reorderDragPending_ && !reorderDragActive_)
        return;

    const auto pos = e.getEventRelativeTo(this).getPosition();

    if (!reorderDragActive_
        && pos.toFloat().getDistanceFrom(dragStartPosition_.toFloat())
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
        onReorderDragMove_(toPanelPosition(pos));
}

void ModulationBusCell::mouseUp(const juce::MouseEvent& e)
{
    finishReorderDrag(e.getEventRelativeTo(this).getPosition());
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

    const int labelH = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.busNumberLabelHeight), sf);
    const int sourceH = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.sourceComboBoxHeight), sf);
    const int amountH = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.amountSliderHeight), sf);
    const int destH = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.destinationComboBoxHeight), sf);
    const int initH = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions_.initButtonHeight), sf);

    // Fixed scaled widths (no remainder absorption) so Init stays on design X 248 @ 100 %.
    const auto cols = TSS::computeModulationBusColumnStrip(
        sf,
        {
            dimensions_.busNumberLabelWidth,
            dimensions_.sourceComboBoxWidth,
            dimensions_.amountSliderWidth,
            dimensions_.destinationComboBoxWidth,
            dimensions_.initButtonWidth,
            dimensions_.interControlGap,
        });

    if (auto* label = busNumberLabel_.get())
        label->setBounds(cols.busNumberX, y, cols.busNumberW, labelH);
    if (auto* combo = sourceComboBox_.get())
        combo->setBounds(cols.sourceX, y, cols.sourceW, sourceH);
    if (auto* slider = amountSlider_.get())
        slider->setBounds(cols.amountX, y, cols.amountW, amountH);
    if (auto* combo = destinationComboBox_.get())
        combo->setBounds(cols.destinationX, y, cols.destinationW, destH);
    if (auto* button = initButton_.get())
        button->setBounds(cols.initX, y, cols.initW, initH);
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
    updateBusNumberLabelWeight();
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
