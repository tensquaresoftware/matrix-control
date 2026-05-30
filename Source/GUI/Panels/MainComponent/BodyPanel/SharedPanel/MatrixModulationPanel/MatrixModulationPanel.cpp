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
#include "Shared/Definitions/PluginDesignDimensions.h"
#include "GUI/Factories/WidgetFactory.h"

namespace
{
    ModulationBusCellDimensions createModulationBusCellDimensions()
    {
        ModulationBusCellDimensions dims;
        dims.panelWidth = PluginDesignDimensions::Panels::Body::SharedColumn::kWidth;
        dims.panelHeight = PluginDesignDimensions::Widgets::Heights::kLabel + PluginDesignDimensions::Widgets::Heights::kHorizontalSeparator;
        dims.busNumberLabelWidth = PluginDesignDimensions::Widgets::Widths::Label::kModulationBusNumber;
        dims.busNumberLabelHeight = PluginDesignDimensions::Widgets::Heights::kLabel;
        dims.sourceComboBoxWidth = PluginDesignDimensions::Widgets::Widths::ComboBox::kMatrixModulationSource;
        dims.sourceComboBoxHeight = PluginDesignDimensions::Widgets::Heights::kComboBox;
        dims.amountSliderWidth = PluginDesignDimensions::Widgets::Widths::Slider::kStandard;
        dims.amountSliderHeight = PluginDesignDimensions::Widgets::Heights::kSlider;
        dims.destinationComboBoxWidth = PluginDesignDimensions::Widgets::Widths::ComboBox::kMatrixModulationDestination;
        dims.destinationComboBoxHeight = PluginDesignDimensions::Widgets::Heights::kComboBox;
        dims.initButtonWidth = PluginDesignDimensions::Widgets::Widths::Button::kInit;
        dims.initButtonHeight = PluginDesignDimensions::Widgets::Heights::kButton;
        dims.separatorWidth = PluginDesignDimensions::Widgets::Widths::HorizontalSeparator::kMatrixModulationBus;
        dims.separatorHeight = PluginDesignDimensions::Widgets::Heights::kHorizontalSeparator;
        return dims;
    }
}

MatrixModulationPanel::~MatrixModulationPanel() = default;

MatrixModulationPanel::MatrixModulationPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , modulationBusHeight_(PluginDesignDimensions::Widgets::Heights::kLabel + PluginDesignDimensions::Widgets::Heights::kHorizontalSeparator)
    , skin_(&skin)
    , apvts_(apvts)
    , sectionHeader_(std::make_unique<tss::SectionHeader>(
        PluginDesignDimensions::Widgets::Widths::SectionHeader::kMatrixModulation,
        PluginDesignDimensions::Widgets::Heights::kSectionHeader,
        tss::sectionHeaderLookFromSkin(skin),
        PluginHelpers::getSectionDisplayName(PluginIDs::MatrixModulationSection::kGroupId)))
    , modulationBusHeader_(std::make_unique<tss::ModulationBusHeader>(
        PluginDesignDimensions::Widgets::Widths::ModulationBusHeader::kStandard,
        PluginDesignDimensions::Widgets::Heights::kModulationBusHeader,
        tss::modulationBusHeaderLookFromSkin(skin)))
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
        modulationBuses_.push_back(std::move(bus));
    }

    setSize(width_, height_);
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

void MatrixModulationPanel::createInitAllBussesButton(tss::ISkin& skin)
{
    initAllBussesButton_ = std::make_unique<tss::Button>(
        PluginDesignDimensions::Widgets::Widths::Button::kInit,
        PluginDesignDimensions::Widgets::Heights::kButton,
        tss::buttonLookFromSkin(skin),
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
        const int headerHeight = tss::ScaledLayout::scaledInt(
            static_cast<float>(PluginDesignDimensions::Widgets::Heights::kSectionHeader), sf);
        header->setBounds(bounds.removeFromTop(headerHeight));
    }

    if (auto* busHeader = modulationBusHeader_.get())
    {
        const int busHeaderHeight = tss::ScaledLayout::scaledInt(
            static_cast<float>(PluginDesignDimensions::Widgets::Heights::kModulationBusHeader), sf);
        busHeader->setBounds(bounds.removeFromTop(busHeaderHeight));
    }

    if (auto* initButton = initAllBussesButton_.get())
    {
        const int initAllButtonWidth = tss::ScaledLayout::scaledInt(
            static_cast<float>(PluginDesignDimensions::Widgets::Widths::Button::kInit), sf);
        const int initAllButtonHeight = tss::ScaledLayout::scaledInt(
            static_cast<float>(PluginDesignDimensions::Widgets::Heights::kButton), sf);
        const int sectionHeaderHeight = tss::ScaledLayout::scaledInt(
            static_cast<float>(PluginDesignDimensions::Widgets::Heights::kSectionHeader), sf);
        const int scaledPanelWidth = tss::ScaledLayout::scaledInt(static_cast<float>(width_), sf);
        const int initAllButtonX = scaledPanelWidth - initAllButtonWidth;
        initButton->setBounds(initAllButtonX, sectionHeaderHeight, initAllButtonWidth, initAllButtonHeight);
    }

    const size_t busCount = modulationBuses_.size();
    if (busCount == 0)
        return;

    const auto busHeights = tss::ScaledLayout::distributeFixedDesignRowsWithRemainderOnLast(
        bounds.getHeight(), busCount, modulationBusHeight_, uiScale_);

    for (size_t i = 0; i < busCount; ++i)
    {
        if (auto* bus = modulationBuses_[i].get())
            bus->setBounds(bounds.removeFromTop(busHeights[i]));
    }
}

void MatrixModulationPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    sectionHeader_->setLook(tss::sectionHeaderLookFromSkin(skin));
    modulationBusHeader_->setLook(tss::modulationBusHeaderLookFromSkin(skin));

    if (initAllBussesButton_)
        initAllBussesButton_->setLook(tss::buttonLookFromSkin(skin));

    for (auto& bus : modulationBuses_)
        tss::propagateSkin(skin, bus.get());
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

