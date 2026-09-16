#include "ComputerPatchesPanel.h"

#include "Core/Services/PatchFileService.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/GroupLabel.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"
#include <juce_core/juce_core.h>

namespace
{
    namespace ComputerPatchesDisplayNames = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule;
    namespace ComputerPatchesIds = PluginIDs::PatchManagerSection::ComputerPatchesModule;

    juce::String fileNameWithoutSyxExtension(const juce::String& fileName)
    {
        return juce::File::createFileWithoutCheckingPath(fileName).getFileNameWithoutExtension();
    }

    void placeOrSkipLeft(juce::Rectangle<int>& row, juce::Component* component, int width)
    {
        if (component != nullptr)
            component->setBounds(row.removeFromLeft(width));
        else
            row.removeFromLeft(width);
    }

    struct LayoutMetrics
    {
        int moduleHeaderH = 0;
        int moduleHeaderW = 0;
        int groupLabelH = 0;
        int browserGroupW = 0;
        int storageGroupW = 0;
        int buttonH = 0;
        int columnGap = 0;

        static LayoutMetrics make(const ComputerPatchesPanelDimensions& dims, float sf)
        {
            LayoutMetrics m;
            m.moduleHeaderH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims.moduleHeader.height), sf);
            m.moduleHeaderW = TSS::ScaledLayout::scaledInt(
                static_cast<float>(dims.moduleHeader.patchManagerTitleBandWidth), sf);
            m.groupLabelH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims.groupLabels.height), sf);
            m.browserGroupW = TSS::ScaledLayout::scaledInt(
                static_cast<float>(dims.groupLabels.computerPatchesBrowserWidth), sf);
            m.storageGroupW = TSS::ScaledLayout::scaledInt(
                static_cast<float>(dims.groupLabels.computerPatchesStorageWidth), sf);
            m.buttonH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims.buttons.height), sf);
            m.columnGap = TSS::ScaledLayout::scaledInt(static_cast<float>(dims.layout.columnGap), sf);
            return m;
        }
    };

    template <typename ComponentT>
    void setOptionalUiScale(ComponentT* component, float sf)
    {
        if (component != nullptr)
            component->setUiScale(sf);
    }

    void dispatchTimestampAction(juce::AudioProcessorValueTreeState& apvts, const juce::Identifier& propertyId)
    {
        apvts.state.setProperty(propertyId, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
    }
}

ComputerPatchesPanel::ComputerPatchesPanel(const Config& config)
    : dims_(config.dims)
    , skin_(&config.skin)
    , apvts_(config.apvts)
    , patchFileService_(config.patchFileService)
{
    setOpaque(false);
    setupModuleHeader(config.skin, config.widgetFactory, ComputerPatchesIds::kGroupId);

    setupBrowserGroupLabel(config.skin);
    setupLoadPreviousPatchFileButton(config.skin, config.widgetFactory);
    setupLoadNextPatchFileButton(config.skin, config.widgetFactory);
    setupSelectPatchFileComboBox(config.skin);

    setupStorageGroupLabel(config.skin);
    setupOpenPatchFolderButton(config.skin, config.widgetFactory);
    setupSavePatchFileAsButton(config.skin, config.widgetFactory);
    setupSavePatchFileButton(config.skin, config.widgetFactory);

    apvts_.state.addListener(this);
    refreshPatchFileComboBox();
    refreshNavigationFocusHighlight();
    registerContextualHelp();

    setSize(dims_.width, dims_.height);
}

ComputerPatchesPanel::~ComputerPatchesPanel()
{
    apvts_.state.removeListener(this);
}

void ComputerPatchesPanel::valueTreePropertyChanged(juce::ValueTree&,
                                                    const juce::Identifier& property)
{
    const auto name = property.toString();
    if (name == ComputerPatchesIds::StateProperties::kScanRevision)
    {
        refreshPatchFileComboBox();
        refreshNavigationFocusHighlight();
    }
    else if (name == ComputerPatchesIds::StandaloneWidgets::kSelectPatchFile)
    {
        syncSelectionFromApvts();
    }
    else if (name == PluginIDs::PatchManagerSection::StateProperties::kNavigationFocus)
    {
        refreshNavigationFocusHighlight();
    }
}

void ComputerPatchesPanel::valueTreeRedirected(juce::ValueTree&)
{
    refreshPatchFileComboBox();
    refreshNavigationFocusHighlight();
}

void ComputerPatchesPanel::refreshNavigationFocusHighlight()
{
    if (selectPatchFileComboBox_ == nullptr || skin_ == nullptr)
        return;

    const int navigationFocus = static_cast<int>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::StateProperties::kNavigationFocus,
        PluginIDs::PatchManagerSection::NavigationFocus::kDefault));

    auto look = TSS::comboBoxLookFromSkin(*skin_);

    if (navigationFocus == PluginIDs::PatchManagerSection::NavigationFocus::kComputer)
    {
        const auto focusColour = skin_->getColour(TSS::SkinColourId::kNumberBoxTextFocus);
        look.buttonLikeText = focusColour;
        look.textEnabled = focusColour;
    }

    selectPatchFileComboBox_->setLook(look);
}

void ComputerPatchesPanel::refreshPatchFileComboBox()
{
    if (selectPatchFileComboBox_ == nullptr)
        return;

    const auto& scan = patchFileService_.getLastScanResult();
    selectPatchFileComboBox_->clear(juce::dontSendNotification);

    if (! scan.folderUsable || scan.validCount == 0)
    {
        applyEmptySentinel();
        return;
    }

    applySelectSentinel(scan.sortedValidFileNames);
    syncSelectionFromApvts();
}

void ComputerPatchesPanel::syncSelectionFromApvts()
{
    if (selectPatchFileComboBox_ == nullptr)
        return;

    const int selectedId = static_cast<int>(apvts_.state.getProperty(
        ComputerPatchesIds::StandaloneWidgets::kSelectPatchFile,
        0));

    if (selectedId >= 1 && selectedId <= selectPatchFileComboBox_->getNumItems())
    {
        selectPatchFileComboBox_->setSelectedId(selectedId, juce::dontSendNotification);
        setNavigationButtonsEnabled(true);
        return;
    }

    selectPatchFileComboBox_->setSelectedId(0, juce::dontSendNotification);
    clearPatchFileSelectionProperty();
    setNavigationButtonsEnabled(false);
}

void ComputerPatchesPanel::applyEmptySentinel()
{
    selectPatchFileComboBox_->setTextWhenNothingSelected(ComputerPatchesDisplayNames::kEmptySentinel);
    selectPatchFileComboBox_->setSelectedId(0, juce::dontSendNotification);
    selectPatchFileComboBox_->setEnabled(false);
    setNavigationButtonsEnabled(false);
    clearPatchFileSelectionProperty();
}

void ComputerPatchesPanel::applySelectSentinel(const juce::StringArray& sortedValidFileNames)
{
    for (int i = 0; i < sortedValidFileNames.size(); ++i)
        selectPatchFileComboBox_->addItem(fileNameWithoutSyxExtension(sortedValidFileNames[i]), i + 1);

    selectPatchFileComboBox_->setTextWhenNothingSelected(ComputerPatchesDisplayNames::kSelectSentinel);
    selectPatchFileComboBox_->setSelectedId(0, juce::dontSendNotification);
    selectPatchFileComboBox_->setEnabled(true);
    setNavigationButtonsEnabled(false);
}

void ComputerPatchesPanel::setNavigationButtonsEnabled(bool enabled)
{
    if (loadPreviousPatchFileButton_ != nullptr)
        loadPreviousPatchFileButton_->setEnabled(enabled);
    if (loadNextPatchFileButton_ != nullptr)
        loadNextPatchFileButton_->setEnabled(enabled);
}

void ComputerPatchesPanel::clearPatchFileSelectionProperty()
{
    apvts_.state.setProperty(ComputerPatchesIds::StandaloneWidgets::kSelectPatchFile, 0, nullptr);
}

void ComputerPatchesPanel::handleSelectPatchFileChanged()
{
    auto* comboBox = selectPatchFileComboBox_.get();
    if (comboBox == nullptr)
        return;

    const int selectedId = comboBox->getSelectedId();
    if (selectedId >= 1)
    {
        using namespace PluginIDs::PatchManagerSection::ComputerPatchesModule;

        const int previousId = static_cast<int>(apvts_.state.getProperty(
            StandaloneWidgets::kSelectPatchFile, 0));
        if (previousId >= 1 && previousId != selectedId)
        {
            apvts_.state.setProperty(
                StateProperties::kSelectPatchCancelBaseline,
                previousId,
                nullptr);
        }

        apvts_.state.setProperty(StandaloneWidgets::kSelectPatchFile, selectedId, nullptr);
    }

    setNavigationButtonsEnabled(selectedId >= 1);
}

void ComputerPatchesPanel::layoutBrowserRow(float sf, int row2Y, int buttonH)
{
    const int navButtonW = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.buttons.initWidth), sf);
    const int comboBoxW = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.comboBoxes.patchManagerComputerPatchesWidth), sf);
    const int interGap = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.layout.interControlGap), sf);

    auto browserRow = juce::Rectangle<int>(0, row2Y, getWidth(), buttonH);

    placeOrSkipLeft(browserRow, loadPreviousPatchFileButton_.get(), navButtonW);
    browserRow.removeFromLeft(interGap);

    placeOrSkipLeft(browserRow, loadNextPatchFileButton_.get(), navButtonW);
    browserRow.removeFromLeft(interGap);

    if (selectPatchFileComboBox_)
        selectPatchFileComboBox_->setBounds(browserRow.removeFromLeft(comboBoxW));
}

void ComputerPatchesPanel::layoutStorageRow(float sf, int row2Y, int buttonH, int storageGroupX)
{
    const int loadButtonW = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.buttons.computerPatchesLoadWidth), sf);
    const int saveButtonW = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.buttons.computerPatchesSaveWidth), sf);
    const int saveAsButtonW = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.buttons.computerPatchesSaveAsWidth), sf);
    const int interGap = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.layout.interControlGap), sf);

    auto storageRow = juce::Rectangle<int>(storageGroupX, row2Y, getWidth() - storageGroupX, buttonH);

    placeOrSkipLeft(storageRow, openPatchFolderButton_.get(), loadButtonW);
    storageRow.removeFromLeft(interGap);

    placeOrSkipLeft(storageRow, savePatchFileAsButton_.get(), saveAsButtonW);
    storageRow.removeFromLeft(interGap);

    placeOrSkipLeft(storageRow, savePatchFileButton_.get(), saveButtonW);
}

void ComputerPatchesPanel::layoutContentRows(float sf)
{
    const auto m = LayoutMetrics::make(dims_, sf);

    if (moduleHeader_)
        moduleHeader_->setBounds(0, 0, m.moduleHeaderW, m.moduleHeaderH);

    const int row1Y = m.moduleHeaderH;
    if (browserGroupLabel)
        browserGroupLabel->setBounds(0, row1Y, m.browserGroupW, m.groupLabelH);

    const int storageGroupX = m.browserGroupW + m.columnGap;
    if (storageGroupLabel)
        storageGroupLabel->setBounds(storageGroupX, row1Y, m.storageGroupW, m.groupLabelH);

    // Row 2 — successive integer strips (fixed widths; no remainder absorption)
    const int row2Y = row1Y + m.groupLabelH;
    layoutBrowserRow(sf, row2Y, m.buttonH);
    layoutStorageRow(sf, row2Y, m.buttonH, storageGroupX);
}

void ComputerPatchesPanel::applyChildUiScales(float sf)
{
    setOptionalUiScale(moduleHeader_.get(), sf);
    setOptionalUiScale(browserGroupLabel.get(), sf);
    setOptionalUiScale(storageGroupLabel.get(), sf);
    setOptionalUiScale(loadPreviousPatchFileButton_.get(), sf);
    setOptionalUiScale(loadNextPatchFileButton_.get(), sf);
    setOptionalUiScale(selectPatchFileComboBox_.get(), sf);
    setOptionalUiScale(openPatchFolderButton_.get(), sf);
    setOptionalUiScale(savePatchFileAsButton_.get(), sf);
    setOptionalUiScale(savePatchFileButton_.get(), sf);
}

void ComputerPatchesPanel::resized()
{
    layoutContentRows(uiScale_);
    applyChildUiScales(uiScale_);
}

void ComputerPatchesPanel::applyChildLooks(TSS::ISkin& skin)
{
    if (moduleHeader_)
        moduleHeader_->setLook(TSS::moduleHeaderLookFromSkin(skin));
    if (browserGroupLabel)
        browserGroupLabel->setLook(TSS::groupLabelLookFromSkin(skin));
    if (storageGroupLabel)
        storageGroupLabel->setLook(TSS::groupLabelLookFromSkin(skin));

    if (selectPatchFileComboBox_)
    {
        selectPatchFileComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
        refreshNavigationFocusHighlight();
    }

    if (loadPreviousPatchFileButton_)
        loadPreviousPatchFileButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (loadNextPatchFileButton_)
        loadNextPatchFileButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (openPatchFolderButton_)
        openPatchFolderButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (savePatchFileAsButton_)
        savePatchFileAsButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (savePatchFileButton_)
        savePatchFileButton_->setLook(TSS::buttonLookFromSkin(skin));
}

void ComputerPatchesPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    applyChildLooks(skin);
}

void ComputerPatchesPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    repaint();
}

std::unique_ptr<TSS::Button> ComputerPatchesPanel::makeTimestampActionButton(
    TSS::ISkin& skin,
    WidgetFactory& widgetFactory,
    const juce::Identifier& actionId)
{
    auto button = widgetFactory.createStandaloneButton(actionId.toString(), skin, dims_.buttons.height);
    button->onClick = [this, actionId]
    {
        dispatchTimestampAction(apvts_, actionId);
    };
    addAndMakeVisible(*button);
    return button;
}

void ComputerPatchesPanel::setupModuleHeader(TSS::ISkin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId)
{
    moduleHeader_ = std::make_unique<TSS::ModuleHeader>(
        dims_.moduleHeader.patchManagerTitleBandWidth,
        dims_.moduleHeader.height,
        TSS::moduleHeaderLookFromSkin(skin),
        TSS::ModuleHeader::ColourVariant::Blue,
        widgetFactory.getGroupDisplayName(moduleId),
        dims_.moduleHeader);
    moduleHeader_->setTitleClickHandler([this]
    {
        dispatchTimestampAction(apvts_, ComputerPatchesIds::StandaloneWidgets::kHeaderClick);
    });
    addAndMakeVisible(*moduleHeader_);
}

void ComputerPatchesPanel::setupBrowserGroupLabel(TSS::ISkin& skin)
{
    browserGroupLabel = std::make_unique<TSS::GroupLabel>(
        dims_.groupLabels.computerPatchesBrowserWidth,
        dims_.groupLabels.height,
        TSS::groupLabelLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kBrowser);
    addAndMakeVisible(*browserGroupLabel);
}

void ComputerPatchesPanel::setupLoadPreviousPatchFileButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    loadPreviousPatchFileButton_ = makeTimestampActionButton(
        skin,
        widgetFactory,
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile);
}

void ComputerPatchesPanel::setupLoadNextPatchFileButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    loadNextPatchFileButton_ = makeTimestampActionButton(
        skin,
        widgetFactory,
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile);
}

void ComputerPatchesPanel::setupSelectPatchFileComboBox(TSS::ISkin& skin)
{
    selectPatchFileComboBox_ = std::make_unique<TSS::ComboBox>(
        dims_.comboBoxes.patchManagerComputerPatchesWidth,
        dims_.buttons.height,
        TSS::comboBoxLookFromSkin(skin),
        TSS::ComboBox::Style::ButtonLike);
    selectPatchFileComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    selectPatchFileComboBox_->onChange = [this] { handleSelectPatchFileChanged(); };
    addAndMakeVisible(*selectPatchFileComboBox_);
}

void ComputerPatchesPanel::setupStorageGroupLabel(TSS::ISkin& skin)
{
    storageGroupLabel = std::make_unique<TSS::GroupLabel>(
        dims_.groupLabels.computerPatchesStorageWidth,
        dims_.groupLabels.height,
        TSS::groupLabelLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kStorage);
    addAndMakeVisible(*storageGroupLabel);
}

void ComputerPatchesPanel::setupOpenPatchFolderButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    openPatchFolderButton_ = makeTimestampActionButton(
        skin,
        widgetFactory,
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kOpenPatchFolder);
}

void ComputerPatchesPanel::setupSavePatchFileAsButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    savePatchFileAsButton_ = makeTimestampActionButton(
        skin,
        widgetFactory,
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchAs);
}

void ComputerPatchesPanel::setupSavePatchFileButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    savePatchFileButton_ = makeTimestampActionButton(
        skin,
        widgetFactory,
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchFile);
}
