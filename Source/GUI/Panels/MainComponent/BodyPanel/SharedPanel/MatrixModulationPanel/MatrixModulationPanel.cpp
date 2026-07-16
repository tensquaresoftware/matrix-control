#include "MatrixModulationPanel.h"

#include "GUI/Helpers/GrayedControlHelper.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/SectionHeader.h"
#include "GUI/Widgets/ModulationBusHeader.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ModulationBusCell.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"

class MatrixModulationPanel::PasteEnabledPropertyListener : public juce::ValueTree::Listener
{
public:
    PasteEnabledPropertyListener(juce::AudioProcessorValueTreeState& apvts,
                                 juce::ValueTree state,
                                 const juce::String& propertyId,
                                 TSS::Button& pasteButton)
        : apvts_(apvts)
        , state_(std::move(state))
        , propertyId_(propertyId)
        , pasteButton_(pasteButton)
    {
        state_.addListener(this);
        syncFromState();
    }

    ~PasteEnabledPropertyListener() override
    {
        state_.removeListener(this);
    }

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override
    {
        if (property.toString() != propertyId_)
            return;

        pasteEnabled_ = static_cast<bool>(treeWhosePropertyHasChanged.getProperty(property, false));
        syncFromState();
    }

    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    void valueTreeRedirected(juce::ValueTree&) override
    {
        syncFromState();
    }

private:
    void syncFromState()
    {
        pasteEnabled_ = static_cast<bool>(state_.getProperty(propertyId_, false));
        pasteButton_.setEnabled(true);
        pasteButton_.setInactiveAppearance(! pasteEnabled_);

        if (pasteEnabled_)
        {
            TSS::GrayedControlHelper::clearGrayedClickHandler(pasteButton_);
            pasteButton_.onClick = [this]
            {
                apvts_.state.setProperty(
                    PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationPaste,
                    juce::Time::getCurrentTime().toMilliseconds(),
                    nullptr);
            };
        }
        else
        {
            pasteButton_.onClick = nullptr;
            TSS::GrayedControlHelper::setGrayedClickHandler(pasteButton_, true, [this]
            {
                TSS::GrayedControlHelper::setFooterInfoMessage(
                    apvts_,
                    PluginDisplayNames::MatrixModulationSection::Header::kIncompatiblePasteFooter);
            });
        }
    }

    juce::AudioProcessorValueTreeState& apvts_;
    juce::ValueTree state_;
    juce::String propertyId_;
    TSS::Button& pasteButton_;
    bool pasteEnabled_ = false;
};

MatrixModulationPanel::~MatrixModulationPanel() = default;

MatrixModulationPanel::MatrixModulationPanel(TSS::ISkin& skin, const MatrixModulationPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : widgetFactory_(widgetFactory)
    , dims_(dims)
    , skin_(&skin)
    , apvts_(apvts)
    , sectionHeader_(std::make_unique<TSS::SectionHeader>(
        dims_.sectionHeaderWidth,
        dims_.sectionHeaderHeight,
        TSS::sectionHeaderLookFromSkin(skin),
        PluginHelpers::getSectionDisplayName(PluginIDs::MatrixModulationSection::kGroupId)))
    , modulationBusHeader_(std::make_unique<TSS::ModulationBusHeader>(
        dims_.busHeaderWidth,
        dims_.busHeaderHeight,
        dims_.busHeader,
        TSS::modulationBusHeaderLookFromSkin(skin)))
{
    setOpaque(false);
    addAndMakeVisible(*sectionHeader_);
    addAndMakeVisible(*modulationBusHeader_);

    const auto parameterArrays = createModulationBusParameterArrays();

    createSectionActionButtons(skin);

    modulationBuses_.reserve(Matrix1000Limits::kModulationBusCount);
    for (int busNumber = 0; busNumber < Matrix1000Limits::kModulationBusCount; ++busNumber)
    {
        const auto busNumberAsSizeT = static_cast<size_t>(busNumber);
        auto bus = std::make_unique<ModulationBusCell>(
            skin,
            dims_.width,
            dims_.modulationBusRowHeight,
            dims_.busCell,
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

    setSize(dims_.width, dims_.height);
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

void MatrixModulationPanel::createSectionActionButtons(TSS::ISkin& skin)
{
    initButton_ = widgetFactory_.createStandaloneButton(
        PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationInit,
        skin,
        dims_.buttonHeight);
    initButton_->onClick = [this]
    {
        apvts_.state.setProperty(
            PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationInit,
            juce::Time::getCurrentTime().toMilliseconds(),
            nullptr);
    };
    addAndMakeVisible(*initButton_);

    copyButton_ = widgetFactory_.createStandaloneButton(
        PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationCopy,
        skin,
        dims_.buttonHeight);
    copyButton_->onClick = [this]
    {
        apvts_.state.setProperty(
            PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationCopy,
            juce::Time::getCurrentTime().toMilliseconds(),
            nullptr);
    };
    addAndMakeVisible(*copyButton_);

    pasteButton_ = widgetFactory_.createStandaloneButton(
        PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationPaste,
        skin,
        dims_.buttonHeight);
    addAndMakeVisible(*pasteButton_);

    attachPasteEnabledListener();
}

void MatrixModulationPanel::attachPasteEnabledListener()
{
    if (pasteButton_ == nullptr)
        return;

    pasteEnabledListener_ = std::make_unique<PasteEnabledPropertyListener>(
        apvts_,
        apvts_.state,
        PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationPasteEnabled,
        *pasteButton_);
}

void MatrixModulationPanel::layoutSectionActionButtons()
{
    const float sf = uiScale_;
    const int buttonHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.buttonHeight), sf);
    const int scaledPanelWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.width), sf);
    const int sectionHeaderHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.sectionHeaderHeight), sf);

    const int pasteButtonWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.pasteWidth), sf);
    const int copyButtonWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.copyWidth), sf);
    const int initButtonWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.initWidth), sf);

    const int pasteX = scaledPanelWidth - pasteButtonWidth;
    const int copyX = scaledPanelWidth - TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.pasteWidth + dims_.copyWidth), sf);
    const int initX = scaledPanelWidth - TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.pasteWidth + dims_.copyWidth + dims_.initWidth), sf);

    if (auto* button = pasteButton_.get())
        button->setBounds(pasteX, sectionHeaderHeight, pasteButtonWidth, buttonHeight);
    if (auto* button = copyButton_.get())
        button->setBounds(copyX, sectionHeaderHeight, copyButtonWidth, buttonHeight);
    if (auto* button = initButton_.get())
        button->setBounds(initX, sectionHeaderHeight, initButtonWidth, buttonHeight);
}

void MatrixModulationPanel::resized()
{
    auto bounds = getLocalBounds();

    const float sf = uiScale_;

    if (auto* header = sectionHeader_.get())
    {
        const int headerHeight = TSS::ScaledLayout::scaledInt(
            static_cast<float>(dims_.sectionHeaderHeight), sf);
        header->setBounds(bounds.removeFromTop(headerHeight));
    }

    if (auto* busHeader = modulationBusHeader_.get())
    {
        const int busHeaderHeight = TSS::ScaledLayout::scaledInt(
            static_cast<float>(dims_.busHeaderHeight), sf);
        busHeader->setBounds(bounds.removeFromTop(busHeaderHeight));
    }

    layoutSectionActionButtons();

    const int busRowHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.modulationBusRowHeight), sf);

    for (auto& bus : modulationBuses_)
    {
        if (bus != nullptr)
            bus->setBounds(bounds.removeFromTop(busRowHeight));
    }
}

void MatrixModulationPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    sectionHeader_->setLook(TSS::sectionHeaderLookFromSkin(skin));
    modulationBusHeader_->setLook(TSS::modulationBusHeaderLookFromSkin(skin));

    const auto buttonLook = TSS::buttonLookFromSkin(skin);
    if (initButton_)
        initButton_->setLook(buttonLook);
    if (copyButton_)
        copyButton_->setLook(buttonLook);
    if (pasteButton_)
        pasteButton_->setLook(buttonLook);

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
    if (initButton_)
        initButton_->setUiScale(uiScale_);
    if (copyButton_)
        copyButton_->setUiScale(uiScale_);
    if (pasteButton_)
        pasteButton_->setUiScale(uiScale_);
    
    for (auto& bus : modulationBuses_)
    {
        if (bus)
            bus->setUiScale(uiScale_);
    }
    
    resized();
    repaint();
}
