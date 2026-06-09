#include "ComputerPatchesPanel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/GroupLabel.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"
#include <juce_core/juce_core.h>


ComputerPatchesPanel::ComputerPatchesPanel(TSS::ISkin& skin, const ComputerPatchesPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : dims_(dims)
    , skin_(&skin)
    , apvts_(apvts)
{
    setOpaque(false);
    setupModuleHeader(skin, widgetFactory, PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId);

    setupBrowserGroupLabel(skin);
    setupLoadPreviousPatchFileButton(skin, widgetFactory);
    setupLoadNextPatchFileButton(skin, widgetFactory);
    setupSelectPatchFileComboBox(skin);

    setupStorageGroupLabel(skin);
    setupOpenPatchFolderButton(skin, widgetFactory);
    setupSavePatchFileAsButton(skin, widgetFactory);
    setupSavePatchFileButton(skin, widgetFactory);

    setSize(dims_.width, dims_.height);
}

ComputerPatchesPanel::~ComputerPatchesPanel() = default;

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

    const int storageGroupX = juce::roundToInt(static_cast<float>(dims_.groupLabels.computerPatchesBrowserWidth + kGroupLabelGap_) * sf);
    if (storageGroupLabel)
        storageGroupLabel->setBounds(storageGroupX, row1Y, storageGroupW, groupLabelH);

    // Row 2 Y: widgets (computed independently)
    const int row2Y = juce::roundToInt(static_cast<float>(dims_.moduleHeader.height + dims_.groupLabels.height) * sf);

    // Browser section: nav buttons + combobox
    const float navStep = static_cast<float>(dims_.buttons.initWidth + kGap_) * sf;
    if (loadPreviousPatchFileButton_)
        loadPreviousPatchFileButton_->setBounds(0, row2Y, navButtonW, buttonH);
    if (loadNextPatchFileButton_)
        loadNextPatchFileButton_->setBounds(juce::roundToInt(navStep), row2Y, navButtonW, buttonH);
    if (selectPatchFileComboBox_)
        selectPatchFileComboBox_->setBounds(juce::roundToInt(navStep * 2.0f), row2Y, comboBoxW, buttonH);

    // Storage section: open + save-as + save
    const float storageOriginX = static_cast<float>(dims_.groupLabels.computerPatchesBrowserWidth + kGroupLabelGap_) * sf;
    const float openStep       = static_cast<float>(dims_.buttons.computerPatchesLoadWidth + kGap_) * sf;
    const float saveAsStep     = static_cast<float>(dims_.buttons.computerPatchesSaveAsWidth + kGap_) * sf;

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
        widgetFactory.getGroupDisplayName(moduleId));
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

    const juce::StringArray patchNames = {
        "TOTOHORN", "1000STRG", "MOOOG_B", "EZYBRASS", "SYNTH",
        "MIBES", "CHUNK", "MINDSEAR", "CASTILLO", "DESTROY+",
        "BIG PIK", "M-CHOIR", "STRINGME", ")LIQUID(", "PNO-ELEC",
        "BED TRAK", "STELLAR", "SYNCAGE", "SHIVERS", "+ ZETA +",
        "STEELDR.", "TAURUS", "POWRSOLO", "INTERSTL", "REZTFUL",
        "WATRLNG", "BEELS", "LIKETHIS", "NTHENEWS", "SOFT MIX",
        "OBXA-A7", "BREATH", "MUTRONO", "SLOWATER", "HAUNTING",
        "FLANGED", "TENSION", "ECHOTRON", "PIRATES!", "EP SWEP",
        "DEJAVUE'", "DRAMA", "VIOLINCE", "BOUNCE", "SAGAN'Z",
        "OB LEAD", "FEEDGIT", "SAMPLE", "TINYPIAN", "GALACTIC"
    };
    
    for (int i = 0; i < patchNames.size(); ++i)
    {
        selectPatchFileComboBox_->addItem(patchNames[i], i + 1);
    }
    
    selectPatchFileComboBox_->setSelectedId(1);
    selectPatchFileComboBox_->setEnabled(true);
    selectPatchFileComboBox_->onChange = [this]
    {
        if (auto* comboBox = selectPatchFileComboBox_.get())
        {
            apvts_.state.setProperty(PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
                                    comboBox->getSelectedId(),
                                    nullptr);
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
