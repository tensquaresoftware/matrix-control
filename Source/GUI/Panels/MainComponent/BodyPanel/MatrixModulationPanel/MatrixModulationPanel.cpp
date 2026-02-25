#include "MatrixModulationPanel.h"

#include "GUI/Themes/Skin.h"
#include "GUI/Widgets/SectionHeader.h"
#include "GUI/Widgets/ModulationBusHeader.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Panels/Reusable/ModulationBusPanel.h"
#include "Shared/PluginDescriptors.h"
#include "Shared/PluginDimensions.h"
#include "GUI/Factories/WidgetFactory.h"


MatrixModulationPanel::~MatrixModulationPanel() = default;

MatrixModulationPanel::MatrixModulationPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
    , apvts_(apvts)
    , sectionHeader_(std::make_unique<tss::SectionHeader>(
        skin,
        PluginDimensions::Widgets::Widths::SectionHeader::kMatrixModulation,
        PluginDimensions::Widgets::Heights::kSectionHeader,
        PluginDescriptors::getSectionDisplayName(PluginIDs::MatrixModulationSection::kGroupId)))
    , modulationBusHeader_(std::make_unique<tss::ModulationBusHeader>(
        skin,
        PluginDimensions::Widgets::Widths::ModulationBusHeader::kStandard,
        PluginDimensions::Widgets::Heights::kModulationBusHeader))
{
    setOpaque(false);
    addAndMakeVisible(*sectionHeader_);
    addAndMakeVisible(*modulationBusHeader_);

    const auto parameterArrays = createModulationBusParameterArrays();

    createInitAllBussesButton(skin);

    modulationBuses_.reserve(PluginIDs::MatrixModulationSection::kModulationBusCount);
    for (int busNumber = 0; busNumber < PluginIDs::MatrixModulationSection::kModulationBusCount; ++busNumber)
    {
        const auto busNumberAsSizeT = static_cast<size_t>(busNumber);
        auto bus = std::make_unique<ModulationBusPanel>(
            busNumber,
            widgetFactory,
            skin,
            apvts_,
            parameterArrays.sourceParameterIds[busNumberAsSizeT],
            parameterArrays.amountParameterIds[busNumberAsSizeT],
            parameterArrays.destinationParameterIds[busNumberAsSizeT],
            parameterArrays.busIds[busNumberAsSizeT]);
        addAndMakeVisible(*bus);
        modulationBuses_.push_back(std::move(bus));
    }

    setSize(getWidth(), getHeight());
}

std::array<const char*, PluginIDs::MatrixModulationSection::kModulationBusCount> MatrixModulationPanel::createBusIds() const
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

std::array<const char*, PluginIDs::MatrixModulationSection::kModulationBusCount> MatrixModulationPanel::createSourceParameterIds() const
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

std::array<const char*, PluginIDs::MatrixModulationSection::kModulationBusCount> MatrixModulationPanel::createAmountParameterIds() const
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

std::array<const char*, PluginIDs::MatrixModulationSection::kModulationBusCount> MatrixModulationPanel::createDestinationParameterIds() const
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

void MatrixModulationPanel::createInitAllBussesButton(tss::Skin& skin)
{
    initAllBussesButton_ = std::make_unique<tss::Button>(
        skin,
        PluginDimensions::Widgets::Widths::Button::kInit,
        PluginDimensions::Widgets::Heights::kButton,
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

    if (auto* header = sectionHeader_.get())
        header->setBounds(bounds.removeFromTop(header->getHeight()));

    if (auto* busHeader = modulationBusHeader_.get())
        busHeader->setBounds(bounds.removeFromTop(busHeader->getHeight()));

    if (auto* initButton = initAllBussesButton_.get())
    {
        const auto initAllButtonWidth = PluginDimensions::Widgets::Widths::Button::kInit;
        const auto initAllButtonHeight = PluginDimensions::Widgets::Heights::kButton;
        const auto initAllButtonX = getWidth() - initAllButtonWidth;
        initButton->setBounds(initAllButtonX, sectionHeader_->getHeight(), initAllButtonWidth, initAllButtonHeight);
    }

    for (auto& bus : modulationBuses_)
    {
        if (bus != nullptr)
            bus->setBounds(bounds.removeFromTop(bus->getHeight()));
    }
}

void MatrixModulationPanel::setSkin(tss::Skin& skin)
{
    skin_ = &skin;

    if (auto* header = sectionHeader_.get())
        header->setSkin(skin);

    if (auto* busHeader = modulationBusHeader_.get())
        busHeader->setSkin(skin);

    if (auto* button = initAllBussesButton_.get())
        button->setSkin(skin);

    for (auto& bus : modulationBuses_)
    {
        if (bus != nullptr)
            bus->setSkin(skin);
    }
}

