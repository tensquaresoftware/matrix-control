#include "ComputerPatchesPanel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/GroupLabel.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDimensions.h"
#include "GUI/Factories/WidgetFactory.h"
#include <juce_core/juce_core.h>


ComputerPatchesPanel::ComputerPatchesPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
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

    setSize(width_, height_);
}

ComputerPatchesPanel::~ComputerPatchesPanel() = default;

void ComputerPatchesPanel::resized()
{
    using namespace PluginDimensions::Widgets;
    const float sf = scalingFactor_;

    // Dimensions (scaled)
    const int moduleHeaderH    = juce::roundToInt(static_cast<float>(Heights::kModuleHeader) * sf);
    const int moduleHeaderW    = juce::roundToInt(static_cast<float>(Widths::ModuleHeader::kPatchManagerModule) * sf);
    const int groupLabelH      = juce::roundToInt(static_cast<float>(Heights::kGroupLabel) * sf);
    const int browserGroupW    = juce::roundToInt(static_cast<float>(Widths::GroupLabel::kComputerPatchesBrowser) * sf);
    const int storageGroupW    = juce::roundToInt(static_cast<float>(Widths::GroupLabel::kComputerPatchesStorage) * sf);
    const int navButtonW       = juce::roundToInt(static_cast<float>(Widths::Button::kInit) * sf);
    const int comboBoxW        = juce::roundToInt(static_cast<float>(Widths::ComboBox::kPatchManagerComputerPatches) * sf);
    const int storageButtonW   = juce::roundToInt(static_cast<float>(Widths::Button::kComputerPatchesStorage) * sf);
    const int saveAsButtonW    = juce::roundToInt(static_cast<float>(Widths::Button::kComputerPatchesSaveAs) * sf);
    const int buttonH          = juce::roundToInt(static_cast<float>(Heights::kButton) * sf);

    // Module header
    if (moduleHeader_)
        moduleHeader_->setBounds(0, 0, moduleHeaderW, moduleHeaderH);

    // Row 1 Y: group labels (computed independently)
    const int row1Y = juce::roundToInt(static_cast<float>(Heights::kModuleHeader) * sf);

    if (browserGroupLabel)
        browserGroupLabel->setBounds(0, row1Y, browserGroupW, groupLabelH);

    const int storageGroupX = juce::roundToInt(static_cast<float>(Widths::GroupLabel::kComputerPatchesBrowser + kGroupLabelSpacing_) * sf);
    if (storageGroupLabel)
        storageGroupLabel->setBounds(storageGroupX, row1Y, storageGroupW, groupLabelH);

    // Row 2 Y: widgets (computed independently)
    const int row2Y = juce::roundToInt(static_cast<float>(Heights::kModuleHeader + Heights::kGroupLabel) * sf);

    // Browser section: nav buttons + combobox
    const float navStep = static_cast<float>(Widths::Button::kInit + kSpacing_) * sf;
    if (loadPreviousPatchFileButton_)
        loadPreviousPatchFileButton_->setBounds(0, row2Y, navButtonW, buttonH);
    if (loadNextPatchFileButton_)
        loadNextPatchFileButton_->setBounds(juce::roundToInt(navStep), row2Y, navButtonW, buttonH);
    if (selectPatchFileComboBox_)
        selectPatchFileComboBox_->setBounds(juce::roundToInt(navStep * 2.0f), row2Y, comboBoxW, buttonH);

    // Storage section: open + save-as + save
    const float storageOriginX = static_cast<float>(Widths::GroupLabel::kComputerPatchesBrowser + kGroupLabelSpacing_) * sf;
    const float openStep       = static_cast<float>(Widths::Button::kComputerPatchesStorage + kSpacing_) * sf;
    const float saveAsStep     = static_cast<float>(Widths::Button::kComputerPatchesSaveAs + kSpacing_) * sf;

    if (openPatchFolderButton_)
        openPatchFolderButton_->setBounds(juce::roundToInt(storageOriginX), row2Y, storageButtonW, buttonH);
    if (savePatchFileAsButton_)
        savePatchFileAsButton_->setBounds(juce::roundToInt(storageOriginX + openStep), row2Y, saveAsButtonW, buttonH);
    if (savePatchFileButton_)
        savePatchFileButton_->setBounds(juce::roundToInt(storageOriginX + openStep + saveAsStep), row2Y, storageButtonW, buttonH);

    if (moduleHeader_)                  moduleHeader_->setScalingFactor(sf);
    if (browserGroupLabel)              browserGroupLabel->setScalingFactor(sf);
    if (storageGroupLabel)              storageGroupLabel->setScalingFactor(sf);
    if (loadPreviousPatchFileButton_)   loadPreviousPatchFileButton_->setScalingFactor(sf);
    if (loadNextPatchFileButton_)       loadNextPatchFileButton_->setScalingFactor(sf);
    if (selectPatchFileComboBox_)       selectPatchFileComboBox_->setScalingFactor(sf);
    if (openPatchFolderButton_)         openPatchFolderButton_->setScalingFactor(sf);
    if (savePatchFileAsButton_)         savePatchFileAsButton_->setScalingFactor(sf);
    if (savePatchFileButton_)           savePatchFileButton_->setScalingFactor(sf);
}

void ComputerPatchesPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    if (moduleHeader_)
        moduleHeader_->setLook(tss::moduleHeaderLookFromSkin(skin));
    if (browserGroupLabel)
        browserGroupLabel->setLook(tss::groupLabelLookFromSkin(skin));
    if (storageGroupLabel)
        storageGroupLabel->setLook(tss::groupLabelLookFromSkin(skin));

    if (selectPatchFileComboBox_)
    {
        selectPatchFileComboBox_->setLook(tss::comboBoxLookFromSkin(skin));
        selectPatchFileComboBox_->setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    }
    if (loadPreviousPatchFileButton_)
        loadPreviousPatchFileButton_->setLook(tss::buttonLookFromSkin(skin));
    if (loadNextPatchFileButton_)
        loadNextPatchFileButton_->setLook(tss::buttonLookFromSkin(skin));
    if (openPatchFolderButton_)
        openPatchFolderButton_->setLook(tss::buttonLookFromSkin(skin));
    if (savePatchFileAsButton_)
        savePatchFileAsButton_->setLook(tss::buttonLookFromSkin(skin));
    if (savePatchFileButton_)
        savePatchFileButton_->setLook(tss::buttonLookFromSkin(skin));
}

void ComputerPatchesPanel::setScalingFactor(float scalingFactor)
{
    if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
        return;
    
    scalingFactor_ = scalingFactor;
    repaint();
}

void ComputerPatchesPanel::setupModuleHeader(tss::ISkin&, WidgetFactory& widgetFactory, const juce::String& moduleId)
{
    moduleHeader_ = std::make_unique<tss::ModuleHeader>(
        widgetFactory.getGroupDisplayName(moduleId),
        PluginDimensions::Widgets::Widths::ModuleHeader::kPatchManagerModule,
        PluginDimensions::Widgets::Heights::kModuleHeader,
        tss::ModuleHeader::ColourVariant::Blue);
    addAndMakeVisible(*moduleHeader_);
}

void ComputerPatchesPanel::setupBrowserGroupLabel(tss::ISkin&)
{
    browserGroupLabel = std::make_unique<tss::GroupLabel>(
        PluginDimensions::Widgets::Widths::GroupLabel::kComputerPatchesBrowser,
        PluginDimensions::Widgets::Heights::kGroupLabel,
        PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kBrowser);
    addAndMakeVisible(*browserGroupLabel);
}

void ComputerPatchesPanel::setupLoadPreviousPatchFileButton(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    loadPreviousPatchFileButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    loadPreviousPatchFileButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*loadPreviousPatchFileButton_);
}

void ComputerPatchesPanel::setupLoadNextPatchFileButton(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    loadNextPatchFileButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    loadNextPatchFileButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*loadNextPatchFileButton_);
}

void ComputerPatchesPanel::setupSelectPatchFileComboBox(tss::ISkin& skin)
{
    selectPatchFileComboBox_ = std::make_unique<tss::ComboBox>(
        PluginDimensions::Widgets::Widths::ComboBox::kPatchManagerComputerPatches,
        PluginDimensions::Widgets::Heights::kButton,
        tss::ComboBox::Style::ButtonLike);
    selectPatchFileComboBox_->setLook(tss::comboBoxLookFromSkin(skin));
    selectPatchFileComboBox_->setPopupMenuLook(tss::popupMenuLookFromSkin(skin));

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

void ComputerPatchesPanel::setupStorageGroupLabel(tss::ISkin&)
{
    storageGroupLabel = std::make_unique<tss::GroupLabel>(
        PluginDimensions::Widgets::Widths::GroupLabel::kComputerPatchesStorage,
        PluginDimensions::Widgets::Heights::kGroupLabel,
        PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kStorage);
    addAndMakeVisible(*storageGroupLabel);
}

void ComputerPatchesPanel::setupOpenPatchFolderButton(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    openPatchFolderButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kOpenPatchFolder,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    openPatchFolderButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kOpenPatchFolder,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*openPatchFolderButton_);
}

void ComputerPatchesPanel::setupSavePatchFileAsButton(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    savePatchFileAsButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchAs,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    savePatchFileAsButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchAs,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*savePatchFileAsButton_);
}

void ComputerPatchesPanel::setupSavePatchFileButton(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    savePatchFileButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchFile,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    savePatchFileButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchFile,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*savePatchFileButton_);
}
