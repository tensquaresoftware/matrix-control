#include "ComputerPatchesPanel.h"

#include "GUI/Themes/Skin.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/GroupLabel.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "Shared/PluginDescriptors.h"
#include "Shared/PluginDimensions.h"
#include "GUI/Factories/WidgetFactory.h"
#include <juce_core/juce_core.h>


ComputerPatchesPanel::ComputerPatchesPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
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

    setSize(getWidth(), getHeight());
}

ComputerPatchesPanel::~ComputerPatchesPanel() = default;

void ComputerPatchesPanel::resized()
{
    int x = 0;
    int y = 0;

    layoutModuleHeader(x, y);
    y += PluginDimensions::Widgets::Heights::kModuleHeader;

    const auto browserGroupWidth = PluginDimensions::Widgets::Widths::GroupLabel::kComputerPatchesBrowser;
    const auto groupLabelHeight = PluginDimensions::Widgets::Heights::kGroupLabel;

    layoutBrowserGroupLabel(x, y);
    x = browserGroupWidth + kGroupLabelSpacing_;

    layoutStorageGroupLabel(x, y);
    y += groupLabelHeight;

    const auto navigationButtonWidth = PluginDimensions::Widgets::Widths::Button::kInit;
    x = 0;

    layoutLoadPreviousPatchFileButton(x, y);
    x += navigationButtonWidth + kSpacing_;

    layoutLoadNextPatchFileButton(x, y);
    x += navigationButtonWidth + kSpacing_;

    layoutSelectPatchFileComboBox(x, y);

    const auto storageButtonWidth = PluginDimensions::Widgets::Widths::Button::kComputerPatchesStorage;
    const auto saveAsButtonWidth = PluginDimensions::Widgets::Widths::Button::kComputerPatchesSaveAs;
    x = browserGroupWidth + kGroupLabelSpacing_;

    layoutOpenPatchFolderButton(x, y);
    x += storageButtonWidth + kSpacing_;

    layoutSavePatchFileAsButton(x, y);
    x += saveAsButtonWidth + kSpacing_;

    layoutSavePatchFileButton(x, y);
}

void ComputerPatchesPanel::setSkin(tss::Skin& skin)
{
    skin_ = &skin;

    if (auto* header = moduleHeader_.get())
        header->setSkin(skin);

    if (auto* browserLabel = browserGroupLabel.get())
        browserLabel->setSkin(skin);

    if (auto* storageLabel = storageGroupLabel.get())
        storageLabel->setSkin(skin);

    if (auto* comboBox = selectPatchFileComboBox_.get())
        comboBox->setSkin(skin);
}

void ComputerPatchesPanel::setupModuleHeader(tss::Skin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId)
{
    moduleHeader_ = std::make_unique<tss::ModuleHeader>(
        skin, 
        widgetFactory.getGroupDisplayName(moduleId),
        PluginDimensions::Widgets::Widths::ModuleHeader::kPatchManagerModule,
        PluginDimensions::Widgets::Heights::kModuleHeader,
        tss::ModuleHeader::ColourVariant::Blue);
    addAndMakeVisible(*moduleHeader_);
}

void ComputerPatchesPanel::setupBrowserGroupLabel(tss::Skin& skin)
{
    browserGroupLabel = std::make_unique<tss::GroupLabel>(
        skin,
        PluginDimensions::Widgets::Widths::GroupLabel::kComputerPatchesBrowser,
        PluginDimensions::Widgets::Heights::kGroupLabel,
        PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kBrowser);
    addAndMakeVisible(*browserGroupLabel);
}

void ComputerPatchesPanel::setupLoadPreviousPatchFileButton(tss::Skin& skin, WidgetFactory& widgetFactory)
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

void ComputerPatchesPanel::setupLoadNextPatchFileButton(tss::Skin& skin, WidgetFactory& widgetFactory)
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

void ComputerPatchesPanel::setupSelectPatchFileComboBox(tss::Skin& skin)
{
    selectPatchFileComboBox_ = std::make_unique<tss::ComboBox>(
        skin,
        PluginDimensions::Widgets::Widths::ComboBox::kPatchManagerComputerPatches,
        PluginDimensions::Widgets::Heights::kButton,
        tss::ComboBox::Style::ButtonLike);
    
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

void ComputerPatchesPanel::setupStorageGroupLabel(tss::Skin& skin)
{
    storageGroupLabel = std::make_unique<tss::GroupLabel>(
        skin,
        PluginDimensions::Widgets::Widths::GroupLabel::kComputerPatchesStorage,
        PluginDimensions::Widgets::Heights::kGroupLabel,
        PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kStorage);
    addAndMakeVisible(*storageGroupLabel);
}

void ComputerPatchesPanel::setupOpenPatchFolderButton(tss::Skin& skin, WidgetFactory& widgetFactory)
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

void ComputerPatchesPanel::setupSavePatchFileAsButton(tss::Skin& skin, WidgetFactory& widgetFactory)
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

void ComputerPatchesPanel::setupSavePatchFileButton(tss::Skin& skin, WidgetFactory& widgetFactory)
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

void ComputerPatchesPanel::layoutModuleHeader(int x, int y)
{
    const auto moduleHeaderHeight = PluginDimensions::Widgets::Heights::kModuleHeader;
    const auto moduleHeaderWidth = PluginDimensions::Widgets::Widths::ModuleHeader::kPatchManagerModule;

    if (auto* header = moduleHeader_.get())
        header->setBounds(x, y, moduleHeaderWidth, moduleHeaderHeight);
}

void ComputerPatchesPanel::layoutBrowserGroupLabel(int x, int y)
{
    const auto browserGroupWidth = PluginDimensions::Widgets::Widths::GroupLabel::kComputerPatchesBrowser;
    const auto groupLabelHeight = PluginDimensions::Widgets::Heights::kGroupLabel;

    if (auto* browserLabel = browserGroupLabel.get())
        browserLabel->setBounds(x, y, browserGroupWidth, groupLabelHeight);
}

void ComputerPatchesPanel::layoutLoadPreviousPatchFileButton(int x, int y)
{
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto navigationButtonWidth = PluginDimensions::Widgets::Widths::Button::kInit;

    if (auto* prevButton = loadPreviousPatchFileButton_.get())
        prevButton->setBounds(x, y, navigationButtonWidth, buttonHeight);
}

void ComputerPatchesPanel::layoutLoadNextPatchFileButton(int x, int y)
{
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto navigationButtonWidth = PluginDimensions::Widgets::Widths::Button::kInit;

    if (auto* nextButton = loadNextPatchFileButton_.get())
        nextButton->setBounds(x, y, navigationButtonWidth, buttonHeight);
}

void ComputerPatchesPanel::layoutSelectPatchFileComboBox(int x, int y)
{
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto comboBoxWidth = PluginDimensions::Widgets::Widths::ComboBox::kPatchManagerComputerPatches;

    if (auto* comboBox = selectPatchFileComboBox_.get())
        comboBox->setBounds(x, y, comboBoxWidth, buttonHeight);
}

void ComputerPatchesPanel::layoutStorageGroupLabel(int x, int y)
{
    const auto storageGroupWidth = PluginDimensions::Widgets::Widths::GroupLabel::kComputerPatchesStorage;
    const auto groupLabelHeight = PluginDimensions::Widgets::Heights::kGroupLabel;

    if (auto* storageLabel = storageGroupLabel.get())
        storageLabel->setBounds(x, y, storageGroupWidth, groupLabelHeight);
}

void ComputerPatchesPanel::layoutOpenPatchFolderButton(int x, int y)
{
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto storageButtonWidth = PluginDimensions::Widgets::Widths::Button::kComputerPatchesStorage;

    if (auto* openButton = openPatchFolderButton_.get())
        openButton->setBounds(x, y, storageButtonWidth, buttonHeight);
}

void ComputerPatchesPanel::layoutSavePatchFileAsButton(int x, int y)
{
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto saveAsButtonWidth = PluginDimensions::Widgets::Widths::Button::kComputerPatchesSaveAs;

    if (auto* saveAsButton = savePatchFileAsButton_.get())
        saveAsButton->setBounds(x, y, saveAsButtonWidth, buttonHeight);
}

void ComputerPatchesPanel::layoutSavePatchFileButton(int x, int y)
{
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto storageButtonWidth = PluginDimensions::Widgets::Widths::Button::kComputerPatchesStorage;

    if (auto* saveButton = savePatchFileButton_.get())
        saveButton->setBounds(x, y, storageButtonWidth, buttonHeight);
}
