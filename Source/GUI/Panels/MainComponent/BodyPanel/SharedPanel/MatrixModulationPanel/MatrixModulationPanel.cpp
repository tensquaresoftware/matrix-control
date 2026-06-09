#include "MatrixModulationPanel.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/SectionHeader.h"
#include "GUI/Widgets/ModulationBusHeader.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ModulationBusCell.h"
#include "GUI/Widgets/ModulationBusCellDimensions.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "GUI/Layout/Design/Design.h"
#include "GUI/Factories/WidgetFactory.h"

namespace
{
    ModulationBusCellDimensions createModulationBusCellDimensions()
    {
        ModulationBusCellDimensions dims;
        dims.panelWidth = TSS::Design::Panels::Body::SharedColumn::kWidth;
        dims.panelHeight = TSS::Design::Recipes::ModulationBusCell::kHeight;
        dims.busNumberLabelWidth = TSS::Design::Atoms::Widths::Label::kModulationBusNumber;
        dims.busNumberLabelHeight = TSS::Design::Atoms::Heights::kLabel;
        dims.sourceComboBoxWidth = TSS::Design::Atoms::Widths::ComboBox::kMatrixModulationSource;
        dims.sourceComboBoxHeight = TSS::Design::Atoms::Heights::kComboBox;
        dims.amountSliderWidth = TSS::Design::Recipes::Slider::kStandard;
        dims.amountSliderHeight = TSS::Design::Atoms::Heights::kSlider;
        dims.destinationComboBoxWidth = TSS::Design::Atoms::Widths::ComboBox::kMatrixModulationDestination;
        dims.destinationComboBoxHeight = TSS::Design::Atoms::Heights::kComboBox;
        dims.initButtonWidth = TSS::Design::Atoms::Widths::Button::kInit;
        dims.initButtonHeight = TSS::Design::Atoms::Heights::kButton;
        dims.separatorWidth = TSS::Design::PanelWidgets::Widths::HorizontalSeparator::kMatrixModulationBus;
        dims.separatorHeight = TSS::Design::Atoms::Heights::kHorizontalSeparator;
        return dims;
    }
}

MatrixModulationPanel::~MatrixModulationPanel() = default;

MatrixModulationPanel::MatrixModulationPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , modulationBusHeight_(TSS::Design::Atoms::Heights::kLabel + TSS::Design::Atoms::Heights::kHorizontalSeparator)
    , skin_(&skin)
    , apvts_(apvts)
    , sectionHeader_(std::make_unique<TSS::SectionHeader>(
        TSS::Design::PanelWidgets::Widths::SectionHeader::kMatrixModulation,
        TSS::Design::Atoms::Heights::kSectionHeader,
        TSS::sectionHeaderLookFromSkin(skin),
        PluginHelpers::getSectionDisplayName(PluginIDs::MatrixModulationSection::kGroupId)))
    , modulationBusHeader_(std::make_unique<TSS::ModulationBusHeader>(
        TSS::Design::PanelWidgets::Widths::ModulationBusHeader::kStandard,
        TSS::Design::Atoms::Heights::kModulationBusHeader,
        TSS::modulationBusHeaderLookFromSkin(skin)))
{
    setOpaque(false);
    addAndMakeVisible(*sectionHeader_);
    addAndMakeVisible(*modulationBusHeader_);

    const auto parameterArrays = createModulationBusParameterArrays();
    const auto modulationBusDimensions = createModulationBusCellDimensions();

    createInitAllBussesButton(skin);

    modulationBuses_.reserve(Matrix1000Limits::kModulationBusCount);
    for (int busNumber = 0; busNumber < Matrix1000Limits::kModulationBusCount; ++busNumber)
    {
        const auto busNumberAsSizeT = static_cast<size_t>(busNumber);
        auto bus = std::make_unique<ModulationBusCell>(
            skin,
            width_,
            modulationBusHeight_,
            modulationBusDimensions,
            busNumber,
            widgetFactory,
            apvts_,
            parameterArrays.sourceParameterIds[busNumberAsSizeT],
            parameterArrays.amountParameterIds[busNumberAsSizeT],
            parameterArrays.destinationParameterIds[busNumberAsSizeT],
            parameterArrays.busIds[busNumberAsSizeT]);
        addAndMakeVisible(*bus);
        bus->setReorderDragCallbacks(
            [this](int sourceBus) { beginBusReorderDrag(sourceBus); },
            [this](juce::Point<int> positionInPanel) { updateBusReorderDrag(positionInPanel); },
            [this](juce::Point<int> positionInPanel) { finishBusReorderDrag(positionInPanel); });
        modulationBuses_.push_back(std::move(bus));
    }

    setSize(width_, height_);
}

void MatrixModulationPanel::setBusReorderHandler(BusReorderHandler handler)
{
    busReorderHandler_ = std::move(handler);
}

void MatrixModulationPanel::beginBusReorderDrag(int sourceBus)
{
    dragSourceBus_ = sourceBus;
    dropTargetBus_.reset();
    setDropTargetBus(std::nullopt);

    if (sourceBus >= 0 && sourceBus < static_cast<int>(modulationBuses_.size()))
        modulationBuses_[static_cast<size_t>(sourceBus)]->setDragSourceHighlighted(true);

    setMouseCursor(juce::MouseCursor::DraggingHandCursor);
}

void MatrixModulationPanel::updateBusReorderDrag(juce::Point<int> positionInPanel)
{
    const int hoverBus = findBusIndexAtPanelPosition(positionInPanel);
    setDropTargetBus(hoverBus >= 0 ? std::optional<int>(hoverBus) : std::nullopt);
}

void MatrixModulationPanel::finishBusReorderDrag(juce::Point<int> positionInPanel)
{
    const int targetBus = findBusIndexAtPanelPosition(positionInPanel);

    if (dragSourceBus_.has_value()
        && targetBus >= 0
        && targetBus != dragSourceBus_.value()
        && busReorderHandler_)
    {
        busReorderHandler_(dragSourceBus_.value(), targetBus);
    }

    clearBusReorderDragState();
}

void MatrixModulationPanel::clearBusReorderDragState()
{
    for (auto& bus : modulationBuses_)
    {
        if (bus != nullptr)
        {
            bus->setDragSourceHighlighted(false);
            bus->setDropTargetHighlighted(false);
        }
    }

    dragSourceBus_.reset();
    dropTargetBus_.reset();
    setMouseCursor(juce::MouseCursor::NormalCursor);
}

int MatrixModulationPanel::findBusIndexAtPanelPosition(juce::Point<int> positionInPanel) const
{
    for (int busIndex = 0; busIndex < static_cast<int>(modulationBuses_.size()); ++busIndex)
    {
        if (auto* bus = modulationBuses_[static_cast<size_t>(busIndex)].get())
        {
            if (bus->getBounds().contains(positionInPanel))
                return busIndex;
        }
    }

    return -1;
}

void MatrixModulationPanel::setDropTargetBus(std::optional<int> busIndex)
{
    if (dropTargetBus_ == busIndex)
        return;

    if (dropTargetBus_.has_value())
    {
        const auto previousIndex = static_cast<size_t>(*dropTargetBus_);
        if (previousIndex < modulationBuses_.size() && modulationBuses_[previousIndex] != nullptr)
            modulationBuses_[previousIndex]->setDropTargetHighlighted(false);
    }

    dropTargetBus_ = busIndex;

    if (dropTargetBus_.has_value()
        && dropTargetBus_.value() != dragSourceBus_
        && dropTargetBus_.value() >= 0
        && dropTargetBus_.value() < static_cast<int>(modulationBuses_.size()))
    {
        modulationBuses_[static_cast<size_t>(*dropTargetBus_)]->setDropTargetHighlighted(true);
    }
}

std::array<const char*, Matrix1000Limits::kModulationBusCount> MatrixModulationPanel::createBusIds() const
{
    return {
        PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus9
    };
}

std::array<const char*, Matrix1000Limits::kModulationBusCount> MatrixModulationPanel::createSourceParameterIds() const
{
    return {
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Source,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Source,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Source,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Source,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Source,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Source,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Source,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Source,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Source,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Source
    };
}

std::array<const char*, Matrix1000Limits::kModulationBusCount> MatrixModulationPanel::createAmountParameterIds() const
{
    return {
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Amount,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Amount,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Amount,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Amount,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Amount,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Amount,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Amount,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Amount,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Amount,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Amount
    };
}

std::array<const char*, Matrix1000Limits::kModulationBusCount> MatrixModulationPanel::createDestinationParameterIds() const
{
    return {
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Destination,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Destination,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Destination,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Destination,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Destination,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Destination,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Destination,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Destination,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Destination,
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Destination
    };
}

MatrixModulationPanel::ModulationBusParameterArrays MatrixModulationPanel::createModulationBusParameterArrays() const
{
    ModulationBusParameterArrays arrays;
    arrays.busIds = createBusIds();
    arrays.sourceParameterIds = createSourceParameterIds();
    arrays.amountParameterIds = createAmountParameterIds();
    arrays.destinationParameterIds = createDestinationParameterIds();
    return arrays;
}

void MatrixModulationPanel::createInitAllBussesButton(TSS::ISkin& skin)
{
    initAllBussesButton_ = std::make_unique<TSS::Button>(
        TSS::Design::Atoms::Widths::Button::kInit,
        TSS::Design::Atoms::Heights::kButton,
        TSS::buttonLookFromSkin(skin),
        PluginDisplayNames::ShortLabels::kInit);
    initAllBussesButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationInit,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*initAllBussesButton_);
}

void MatrixModulationPanel::resized()
{
    auto bounds = getLocalBounds();

    const float sf = uiScale_;

    if (auto* header = sectionHeader_.get())
    {
        const int headerHeight = TSS::ScaledLayout::scaledInt(
            static_cast<float>(TSS::Design::Atoms::Heights::kSectionHeader), sf);
        header->setBounds(bounds.removeFromTop(headerHeight));
    }

    if (auto* busHeader = modulationBusHeader_.get())
    {
        const int busHeaderHeight = TSS::ScaledLayout::scaledInt(
            static_cast<float>(TSS::Design::Atoms::Heights::kModulationBusHeader), sf);
        busHeader->setBounds(bounds.removeFromTop(busHeaderHeight));
    }

    if (auto* initButton = initAllBussesButton_.get())
    {
        const int initAllButtonWidth = TSS::ScaledLayout::scaledInt(
            static_cast<float>(TSS::Design::Atoms::Widths::Button::kInit), sf);
        const int initAllButtonHeight = TSS::ScaledLayout::scaledInt(
            static_cast<float>(TSS::Design::Atoms::Heights::kButton), sf);
        const int sectionHeaderHeight = TSS::ScaledLayout::scaledInt(
            static_cast<float>(TSS::Design::Atoms::Heights::kSectionHeader), sf);
        const int scaledPanelWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(width_), sf);
        const int initAllButtonX = scaledPanelWidth - initAllButtonWidth;
        initButton->setBounds(initAllButtonX, sectionHeaderHeight, initAllButtonWidth, initAllButtonHeight);
    }

    const size_t busCount = modulationBuses_.size();
    if (busCount == 0)
        return;

    const auto busHeights = TSS::ScaledLayout::distributeFixedDesignRowsWithRemainderOnLast(
        bounds.getHeight(), busCount, modulationBusHeight_, uiScale_);

    for (size_t i = 0; i < busCount; ++i)
    {
        if (auto* bus = modulationBuses_[i].get())
            bus->setBounds(bounds.removeFromTop(busHeights[i]));
    }
}

void MatrixModulationPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    sectionHeader_->setLook(TSS::sectionHeaderLookFromSkin(skin));
    modulationBusHeader_->setLook(TSS::modulationBusHeaderLookFromSkin(skin));

    if (initAllBussesButton_)
        initAllBussesButton_->setLook(TSS::buttonLookFromSkin(skin));

    for (auto& bus : modulationBuses_)
        TSS::propagateSkin(skin, bus.get());
}

void MatrixModulationPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    
    if (sectionHeader_)
        sectionHeader_->setUiScale(uiScale_);
    if (modulationBusHeader_)
        modulationBusHeader_->setUiScale(uiScale_);
    if (initAllBussesButton_)
        initAllBussesButton_->setUiScale(uiScale_);
    
    for (auto& bus : modulationBuses_)
    {
        if (bus)
            bus->setUiScale(uiScale_);
    }
    
    resized();
    repaint();
}

