#include "InternalPatchesPanel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/GroupLabel.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/NumberBox.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDimensions.h"
#include "GUI/Factories/WidgetFactory.h"
#include <juce_core/juce_core.h>


InternalPatchesPanel::InternalPatchesPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , apvts_(apvts)
{
    setOpaque(false);
    setupModuleHeader(skin, widgetFactory, PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId);

    setupBrowserGroupLabel(skin);
    setupLoadPreviousPatchButton(skin, widgetFactory);
    setupLoadNextPatchButton(skin, widgetFactory);
    setupCurrentBankNumberBox(skin);
    setupCurrentPatchNumberBox(skin);

    setupMemoryGroupLabel(skin);
    setupInitPatchButton(skin, widgetFactory);
    setupCopyPatchButton(skin, widgetFactory);
    setupPastePatchButton(skin, widgetFactory);
    setupStorePatchButton(skin, widgetFactory);

    apvts_.state.addListener(this);
    
    setSize(width_, height_);
}

InternalPatchesPanel::~InternalPatchesPanel()
{
    apvts_.state.removeListener(this);
}

void InternalPatchesPanel::valueTreePropertyChanged(
    juce::ValueTree& treeWhosePropertyHasChanged,
    const juce::Identifier& property)
{
    const auto propertyName = property.toString();
    
    // Mise à jour du NumberBox depuis le Core (via Property)
    if (propertyName == PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber)
    {
        const int bankNumber = treeWhosePropertyHasChanged.getProperty(property);
        if (auto* numberBox = currentBankNumber.get())
            numberBox->setValue(bankNumber);
    }
    
    if (propertyName == PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber)
    {
        const int patchNumber = treeWhosePropertyHasChanged.getProperty(property);
        if (auto* numberBox = currentPatchNumber.get())
            numberBox->setValue(patchNumber);
    }
}

void InternalPatchesPanel::resized()
{
    int x = 0;
    int y = 0;

    layoutModuleHeader(x, y);
    y += PluginDimensions::Widgets::Heights::kModuleHeader;

    const auto browserGroupWidth = PluginDimensions::Widgets::Widths::GroupLabel::kInternalPatchesBrowser;
    const auto groupLabelHeight = PluginDimensions::Widgets::Heights::kGroupLabel;

    layoutBrowserGroupLabel(x, y);
    x = browserGroupWidth + kGroupLabelSpacing_;

    layoutMemoryGroupLabel(x, y);
    y += groupLabelHeight;

    const auto navigationButtonWidth = PluginDimensions::Widgets::Widths::Button::kInit;
    x = 0;

    layoutLoadPreviousPatchButton(x, y);
    x += navigationButtonWidth + kSpacing_;

    layoutLoadNextPatchButton(x, y);
    x += navigationButtonWidth + kSpacing_;

    layoutCurrentBankNumberBox(x, y);
    x += PluginDimensions::Widgets::Widths::NumberBox::kPatchManagerBankNumber + kSpacing_;

    layoutCurrentPatchNumberBox(x, y);

    const auto memoryButtonWidth = PluginDimensions::Widgets::Widths::Button::kInternalPatchesMemory;
    x = browserGroupWidth + kGroupLabelSpacing_;

    layoutInitPatchButton(x, y);
    x += memoryButtonWidth + kSpacing_;

    layoutCopyPatchButton(x, y);
    x += memoryButtonWidth + kSpacing_;

    layoutPastePatchButton(x, y);
    x += memoryButtonWidth + kSpacing_;

    layoutStorePatchButton(x, y);
}

void InternalPatchesPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    tss::propagateSkin(skin,
        moduleHeader.get(),
        browserGroupLabel.get(),
        memoryGroupLabel.get(),
        currentBankNumber.get(),
        currentPatchNumber.get());
}

void InternalPatchesPanel::setupModuleHeader(tss::ISkin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId)
{
    moduleHeader = std::make_unique<tss::ModuleHeader>(
        skin, 
        widgetFactory.getGroupDisplayName(moduleId),
        PluginDimensions::Widgets::Widths::ModuleHeader::kPatchManagerModule,
        PluginDimensions::Widgets::Heights::kModuleHeader,
        tss::ModuleHeader::ColourVariant::Blue);
    addAndMakeVisible(*moduleHeader);
}

void InternalPatchesPanel::setupBrowserGroupLabel(tss::ISkin& skin)
{
    browserGroupLabel = std::make_unique<tss::GroupLabel>(
        skin,
        PluginDimensions::Widgets::Widths::GroupLabel::kInternalPatchesBrowser,
        PluginDimensions::Widgets::Heights::kGroupLabel,
        PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kBrowser);
    addAndMakeVisible(*browserGroupLabel);
}

void InternalPatchesPanel::setupLoadPreviousPatchButton(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    loadPreviousPatchButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    loadPreviousPatchButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*loadPreviousPatchButton_);
}

void InternalPatchesPanel::setupLoadNextPatchButton(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    loadNextPatchButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadNextPatch,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    loadNextPatchButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadNextPatch,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*loadNextPatchButton_);
}

void InternalPatchesPanel::setupCurrentBankNumberBox(tss::ISkin& skin)
{
    currentBankNumber = std::make_unique<tss::NumberBox>(
        skin,
        PluginDimensions::Widgets::Widths::NumberBox::kPatchManagerBankNumber,
        false,
        Matrix1000Limits::kMinBankNumber,
        Matrix1000Limits::kMaxBankNumber);
    currentBankNumber->setShowDot(true);
    addAndMakeVisible(*currentBankNumber);
}

void InternalPatchesPanel::setupCurrentPatchNumberBox(tss::ISkin& skin)
{
    currentPatchNumber = std::make_unique<tss::NumberBox>(
        skin,
        PluginDimensions::Widgets::Widths::NumberBox::kPatchManagerPatchNumber,
        true,
        Matrix1000Limits::kMinPatchNumber,
        Matrix1000Limits::kMaxPatchNumber);
    
    // Callback pour envoyer la valeur via Property (comme les boutons)
    currentPatchNumber->setOnValueChanged([this](int newValue)
    {
        // Éviter la boucle : ne pas setProperty si la valeur vient déjà du ValueTree
        const auto currentPropertyValue = apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber, -1);
        
        if (static_cast<int>(currentPropertyValue) != newValue)
        {
            apvts_.state.setProperty(PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber,
                                    newValue,
                                    nullptr);
        }
    });
    
    addAndMakeVisible(*currentPatchNumber);
}

void InternalPatchesPanel::setupMemoryGroupLabel(tss::ISkin& skin)
{
    memoryGroupLabel = std::make_unique<tss::GroupLabel>(
        skin,
        PluginDimensions::Widgets::Widths::GroupLabel::kInternalPatchesMemory,
        PluginDimensions::Widgets::Heights::kGroupLabel,
        PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kMemory);
    addAndMakeVisible(*memoryGroupLabel);
}

void InternalPatchesPanel::setupInitPatchButton(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    initPatchButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kInitPatch,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    initPatchButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kInitPatch,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*initPatchButton_);
}

void InternalPatchesPanel::setupCopyPatchButton(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    copyPatchButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCopyPatch,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    copyPatchButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCopyPatch,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*copyPatchButton_);
}

void InternalPatchesPanel::setupPastePatchButton(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    pastePatchButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    pastePatchButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*pastePatchButton_);
}

void InternalPatchesPanel::setupStorePatchButton(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    storePatchButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    storePatchButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*storePatchButton_);
}

void InternalPatchesPanel::layoutModuleHeader(int x, int y)
{
    const auto moduleHeaderHeight = PluginDimensions::Widgets::Heights::kModuleHeader;
    const auto moduleHeaderWidth = PluginDimensions::Widgets::Widths::ModuleHeader::kPatchManagerModule;

    if (auto* header = moduleHeader.get())
        header->setBounds(x, y, moduleHeaderWidth, moduleHeaderHeight);
}

void InternalPatchesPanel::layoutBrowserGroupLabel(int x, int y)
{
    const auto browserGroupWidth = PluginDimensions::Widgets::Widths::GroupLabel::kInternalPatchesBrowser;
    const auto groupLabelHeight = PluginDimensions::Widgets::Heights::kGroupLabel;

    if (auto* browserLabel = browserGroupLabel.get())
        browserLabel->setBounds(x, y, browserGroupWidth, groupLabelHeight);
}

void InternalPatchesPanel::layoutLoadPreviousPatchButton(int x, int y)
{
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto navigationButtonWidth = PluginDimensions::Widgets::Widths::Button::kInit;

    if (auto* prevButton = loadPreviousPatchButton_.get())
        prevButton->setBounds(x, y, navigationButtonWidth, buttonHeight);
}

void InternalPatchesPanel::layoutLoadNextPatchButton(int x, int y)
{
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto navigationButtonWidth = PluginDimensions::Widgets::Widths::Button::kInit;

    if (auto* nextButton = loadNextPatchButton_.get())
        nextButton->setBounds(x, y, navigationButtonWidth, buttonHeight);
}

void InternalPatchesPanel::layoutCurrentBankNumberBox(int x, int y)
{
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto bankNumberWidth = PluginDimensions::Widgets::Widths::NumberBox::kPatchManagerBankNumber;

    if (auto* bankNumber = currentBankNumber.get())
        bankNumber->setBounds(x, y, bankNumberWidth, buttonHeight);
}

void InternalPatchesPanel::layoutCurrentPatchNumberBox(int x, int y)
{
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto patchNumberWidth = PluginDimensions::Widgets::Widths::NumberBox::kPatchManagerPatchNumber;

    if (auto* patchNumber = currentPatchNumber.get())
        patchNumber->setBounds(x, y, patchNumberWidth, buttonHeight);
}

void InternalPatchesPanel::layoutMemoryGroupLabel(int x, int y)
{
    const auto memoryGroupWidth = PluginDimensions::Widgets::Widths::GroupLabel::kInternalPatchesMemory;
    const auto groupLabelHeight = PluginDimensions::Widgets::Heights::kGroupLabel;

    if (auto* memoryLabel = memoryGroupLabel.get())
        memoryLabel->setBounds(x, y, memoryGroupWidth, groupLabelHeight);
}

void InternalPatchesPanel::layoutInitPatchButton(int x, int y)
{
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto memoryButtonWidth = PluginDimensions::Widgets::Widths::Button::kInternalPatchesMemory;

    if (auto* initButton = initPatchButton_.get())
        initButton->setBounds(x, y, memoryButtonWidth, buttonHeight);
}

void InternalPatchesPanel::layoutCopyPatchButton(int x, int y)
{
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto memoryButtonWidth = PluginDimensions::Widgets::Widths::Button::kInternalPatchesMemory;

    if (auto* copyButton = copyPatchButton_.get())
        copyButton->setBounds(x, y, memoryButtonWidth, buttonHeight);
}

void InternalPatchesPanel::layoutPastePatchButton(int x, int y)
{
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto memoryButtonWidth = PluginDimensions::Widgets::Widths::Button::kInternalPatchesMemory;

    if (auto* pasteButton = pastePatchButton_.get())
        pasteButton->setBounds(x, y, memoryButtonWidth, buttonHeight);
}

void InternalPatchesPanel::layoutStorePatchButton(int x, int y)
{
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto memoryButtonWidth = PluginDimensions::Widgets::Widths::Button::kInternalPatchesMemory;

    if (auto* storeButton = storePatchButton_.get())
        storeButton->setBounds(x, y, memoryButtonWidth, buttonHeight);
}
