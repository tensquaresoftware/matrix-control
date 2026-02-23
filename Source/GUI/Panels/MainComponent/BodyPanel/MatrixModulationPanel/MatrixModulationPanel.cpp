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
        PluginDescriptors::getSectionDisplayName(PluginDescriptors::SectionIds::kMatrixModulation)))
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

    modulationBuses_.reserve(PluginDescriptors::kModulationBusCount);
    for (int busNumber = 0; busNumber < PluginDescriptors::kModulationBusCount; ++busNumber)
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

std::array<const char*, PluginDescriptors::kModulationBusCount> MatrixModulationPanel::createBusIds() const
{
    return {
        PluginDescriptors::ModulationBusIds::kModulationBus0,
        PluginDescriptors::ModulationBusIds::kModulationBus1,
        PluginDescriptors::ModulationBusIds::kModulationBus2,
        PluginDescriptors::ModulationBusIds::kModulationBus3,
        PluginDescriptors::ModulationBusIds::kModulationBus4,
        PluginDescriptors::ModulationBusIds::kModulationBus5,
        PluginDescriptors::ModulationBusIds::kModulationBus6,
        PluginDescriptors::ModulationBusIds::kModulationBus7,
        PluginDescriptors::ModulationBusIds::kModulationBus8,
        PluginDescriptors::ModulationBusIds::kModulationBus9
    };
}

std::array<const char*, PluginDescriptors::kModulationBusCount> MatrixModulationPanel::createSourceParameterIds() const
{
    return {
        PluginDescriptors::ParameterIds::kModulationBus0Source,
        PluginDescriptors::ParameterIds::kModulationBus1Source,
        PluginDescriptors::ParameterIds::kModulationBus2Source,
        PluginDescriptors::ParameterIds::kModulationBus3Source,
        PluginDescriptors::ParameterIds::kModulationBus4Source,
        PluginDescriptors::ParameterIds::kModulationBus5Source,
        PluginDescriptors::ParameterIds::kModulationBus6Source,
        PluginDescriptors::ParameterIds::kModulationBus7Source,
        PluginDescriptors::ParameterIds::kModulationBus8Source,
        PluginDescriptors::ParameterIds::kModulationBus9Source
    };
}

std::array<const char*, PluginDescriptors::kModulationBusCount> MatrixModulationPanel::createAmountParameterIds() const
{
    return {
        PluginDescriptors::ParameterIds::kModulationBus0Amount,
        PluginDescriptors::ParameterIds::kModulationBus1Amount,
        PluginDescriptors::ParameterIds::kModulationBus2Amount,
        PluginDescriptors::ParameterIds::kModulationBus3Amount,
        PluginDescriptors::ParameterIds::kModulationBus4Amount,
        PluginDescriptors::ParameterIds::kModulationBus5Amount,
        PluginDescriptors::ParameterIds::kModulationBus6Amount,
        PluginDescriptors::ParameterIds::kModulationBus7Amount,
        PluginDescriptors::ParameterIds::kModulationBus8Amount,
        PluginDescriptors::ParameterIds::kModulationBus9Amount
    };
}

std::array<const char*, PluginDescriptors::kModulationBusCount> MatrixModulationPanel::createDestinationParameterIds() const
{
    return {
        PluginDescriptors::ParameterIds::kModulationBus0Destination,
        PluginDescriptors::ParameterIds::kModulationBus1Destination,
        PluginDescriptors::ParameterIds::kModulationBus2Destination,
        PluginDescriptors::ParameterIds::kModulationBus3Destination,
        PluginDescriptors::ParameterIds::kModulationBus4Destination,
        PluginDescriptors::ParameterIds::kModulationBus5Destination,
        PluginDescriptors::ParameterIds::kModulationBus6Destination,
        PluginDescriptors::ParameterIds::kModulationBus7Destination,
        PluginDescriptors::ParameterIds::kModulationBus8Destination,
        PluginDescriptors::ParameterIds::kModulationBus9Destination
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
        PluginDescriptors::DisplayNames::ShortLabels::kInit);
    initAllBussesButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginDescriptors::StandaloneWidgetIds::kMatrixModulationInit,
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

