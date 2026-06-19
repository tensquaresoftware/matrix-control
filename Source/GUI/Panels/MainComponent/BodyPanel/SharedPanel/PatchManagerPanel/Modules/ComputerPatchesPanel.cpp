#include "ComputerPatchesPanel.h"

#include "Core/Services/PatchFileService.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
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
}

ComputerPatchesPanel::ComputerPatchesPanel(TSS::ISkin& skin,
                                           const ComputerPatchesPanelDimensions& dims,
                                           WidgetFactory& widgetFactory,
                                           juce::AudioProcessorValueTreeState& apvts,
                                           const Core::PatchFileService& patchFileService)
    : dims_(dims)
    , skin_(&skin)
    , apvts_(apvts)
    , patchFileService_(patchFileService)
{
    setOpaque(false);
    setupModuleHeader(skin, widgetFactory, ComputerPatchesIds::kGroupId);

    setupBrowserGroupLabel(skin);
    setupLoadPreviousPatchFileButton(skin, widgetFactory);
    setupLoadNextPatchFileButton(skin, widgetFactory);
    setupSelectPatchFileComboBox(skin);

    setupStorageGroupLabel(skin);
    setupOpenPatchFolderButton(skin, widgetFactory);
    setupSavePatchFileAsButton(skin, widgetFactory);
    setupSavePatchFileButton(skin, widgetFactory);

    apvts_.state.addListener(this);
    refreshPatchFileComboBox();

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
        refreshPatchFileComboBox();
    else if (name == ComputerPatchesIds::StandaloneWidgets::kSelectPatchFile)
        syncSelectionFromApvts();
}

void ComputerPatchesPanel::valueTreeRedirected(juce::ValueTree&)
{
    refreshPatchFileComboBox();
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

void ComputerPatchesPanel::resized()
{
    const float sf = uiScale_;

    // Dimensions (scaled)
    const int moduleHeaderH    = juce::roundToInt(static_cast<float>(dims_.moduleHeader.height) * sf);
    const int moduleHeaderW    = juce::roundToInt(static_cast<float>(dims_.moduleHeader.patchManagerTitleBandWidth) * sf);
    const int groupLabelH      = juce::roundToInt(static_cast<float>(dims_.groupLabels.height) * sf);
    const int browserGroupW    = juce::roundToInt(static_cast<float>(dims_.groupLabels.computerPatchesBrowserWidth) * sf);
    const int storageGroupW    = juce::roundToInt(static_cast<float>(dims_.groupLabels.computerPatchesStorageWidth) * sf);
    const int navButtonW       = juce::roundToInt(static_cast<float>(dims_.buttons.initWidth) * sf);
    const int comboBoxW        = juce::roundToInt(static_cast<float>(dims_.comboBoxes.patchManagerComputerPatchesWidth) * sf);
    const int loadButtonW      = juce::roundToInt(static_cast<float>(dims_.buttons.computerPatchesLoadWidth) * sf);
    const int saveButtonW      = juce::roundToInt(static_cast<float>(dims_.buttons.computerPatchesSaveWidth) * sf);
    const int saveAsButtonW    = juce::roundToInt(static_cast<float>(dims_.buttons.computerPatchesSaveAsWidth) * sf);
    const int buttonH          = juce::roundToInt(static_cast<float>(dims_.buttons.height) * sf);

    // Module header
    if (moduleHeader_)
        moduleHeader_->setBounds(0, 0, moduleHeaderW, moduleHeaderH);

    // Row 1 Y: group labels (computed independently)
    const int row1Y = juce::roundToInt(static_cast<float>(dims_.moduleHeader.height) * sf);

    if (browserGroupLabel)
        browserGroupLabel->setBounds(0, row1Y, browserGroupW, groupLabelH);

    const int storageGroupX = juce::roundToInt(
        static_cast<float>(dims_.groupLabels.computerPatchesBrowserWidth + dims_.layout.columnGap) * sf);
    if (storageGroupLabel)
        storageGroupLabel->setBounds(storageGroupX, row1Y, storageGroupW, groupLabelH);

    // Row 2 Y: widgets (computed independently)
    const int row2Y = juce::roundToInt(static_cast<float>(dims_.moduleHeader.height + dims_.groupLabels.height) * sf);

    // Browser section: nav buttons + combobox
    const float navStep = static_cast<float>(dims_.buttons.initWidth + dims_.layout.interControlGap) * sf;
    if (loadPreviousPatchFileButton_)
        loadPreviousPatchFileButton_->setBounds(0, row2Y, navButtonW, buttonH);
    if (loadNextPatchFileButton_)
        loadNextPatchFileButton_->setBounds(juce::roundToInt(navStep), row2Y, navButtonW, buttonH);
    if (selectPatchFileComboBox_)
        selectPatchFileComboBox_->setBounds(juce::roundToInt(navStep * 2.0f), row2Y, comboBoxW, buttonH);

    // Storage section: open + save-as + save
    const float storageOriginX = static_cast<float>(dims_.groupLabels.computerPatchesBrowserWidth + dims_.layout.columnGap) * sf;
    const float openStep       = static_cast<float>(dims_.buttons.computerPatchesLoadWidth + dims_.layout.interControlGap) * sf;
    const float saveAsStep     = static_cast<float>(dims_.buttons.computerPatchesSaveAsWidth + dims_.layout.interControlGap) * sf;

    if (openPatchFolderButton_)
        openPatchFolderButton_->setBounds(juce::roundToInt(storageOriginX), row2Y, loadButtonW, buttonH);
    if (savePatchFileAsButton_)
        savePatchFileAsButton_->setBounds(juce::roundToInt(storageOriginX + openStep), row2Y, saveAsButtonW, buttonH);
    if (savePatchFileButton_)
        savePatchFileButton_->setBounds(juce::roundToInt(storageOriginX + openStep + saveAsStep), row2Y, saveButtonW, buttonH);

    if (moduleHeader_)                  moduleHeader_->setUiScale(sf);
    if (browserGroupLabel)              browserGroupLabel->setUiScale(sf);
    if (storageGroupLabel)              storageGroupLabel->setUiScale(sf);
    if (loadPreviousPatchFileButton_)   loadPreviousPatchFileButton_->setUiScale(sf);
    if (loadNextPatchFileButton_)       loadNextPatchFileButton_->setUiScale(sf);
    if (selectPatchFileComboBox_)       selectPatchFileComboBox_->setUiScale(sf);
    if (openPatchFolderButton_)         openPatchFolderButton_->setUiScale(sf);
    if (savePatchFileAsButton_)         savePatchFileAsButton_->setUiScale(sf);
    if (savePatchFileButton_)           savePatchFileButton_->setUiScale(sf);
}

void ComputerPatchesPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    if (moduleHeader_)
        moduleHeader_->setLook(TSS::moduleHeaderLookFromSkin(skin));
    if (browserGroupLabel)
        browserGroupLabel->setLook(TSS::groupLabelLookFromSkin(skin));
    if (storageGroupLabel)
        storageGroupLabel->setLook(TSS::groupLabelLookFromSkin(skin));

    if (selectPatchFileComboBox_)
    {
        selectPatchFileComboBox_->setLook(TSS::comboBoxLookFromSkin(skin));
        selectPatchFileComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
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

void ComputerPatchesPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    repaint();
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
    loadPreviousPatchFileButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile,
        skin,
        dims_.buttons.height);
    loadPreviousPatchFileButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*loadPreviousPatchFileButton_);
}

void ComputerPatchesPanel::setupLoadNextPatchFileButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    loadNextPatchFileButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile,
        skin,
        dims_.buttons.height);
    loadNextPatchFileButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*loadNextPatchFileButton_);
}

void ComputerPatchesPanel::setupSelectPatchFileComboBox(TSS::ISkin& skin)
{
    selectPatchFileComboBox_ = std::make_unique<TSS::ComboBox>(
        dims_.comboBoxes.patchManagerComputerPatchesWidth,
        dims_.buttons.height,
        TSS::comboBoxLookFromSkin(skin),
        TSS::ComboBox::Style::ButtonLike);
    selectPatchFileComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    selectPatchFileComboBox_->onChange = [this]
    {
        if (auto* comboBox = selectPatchFileComboBox_.get())
        {
            const int selectedId = comboBox->getSelectedId();
            if (selectedId >= 1)
            {
                apvts_.state.setProperty(ComputerPatchesIds::StandaloneWidgets::kSelectPatchFile,
                                        selectedId,
                                        nullptr);
            }
            setNavigationButtonsEnabled(selectedId >= 1);
        }
    };
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
    openPatchFolderButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kOpenPatchFolder,
        skin,
        dims_.buttons.height);
    openPatchFolderButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kOpenPatchFolder,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*openPatchFolderButton_);
}

void ComputerPatchesPanel::setupSavePatchFileAsButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    savePatchFileAsButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchAs,
        skin,
        dims_.buttons.height);
    savePatchFileAsButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchAs,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*savePatchFileAsButton_);
}

void ComputerPatchesPanel::setupSavePatchFileButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    savePatchFileButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchFile,
        skin,
        dims_.buttons.height);
    savePatchFileButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchFile,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*savePatchFileButton_);
}
