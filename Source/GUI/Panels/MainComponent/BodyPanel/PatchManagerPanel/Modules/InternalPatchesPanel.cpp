#include "InternalPatchesPanel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
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
    setupModuleHeader(widgetFactory, PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId);

    setupBrowserGroupLabel();
    setupLoadPreviousPatchButton(skin, widgetFactory);
    setupLoadNextPatchButton(skin, widgetFactory);
    setupCurrentBankNumberBox();
    setupCurrentPatchNumberBox();

    setupMemoryGroupLabel();
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
    using namespace PluginDimensions::Widgets;
    const float sf = displayScale_;

    // Dimensions (scaled)
    const int moduleHeaderH   = juce::roundToInt(static_cast<float>(Heights::kModuleHeader) * sf);
    const int moduleHeaderW   = juce::roundToInt(static_cast<float>(Widths::ModuleHeader::kPatchManagerModule) * sf);
    const int groupLabelH     = juce::roundToInt(static_cast<float>(Heights::kGroupLabel) * sf);
    const int browserGroupW   = juce::roundToInt(static_cast<float>(Widths::GroupLabel::kInternalPatchesBrowser) * sf);
    const int memoryGroupW    = juce::roundToInt(static_cast<float>(Widths::GroupLabel::kInternalPatchesMemory) * sf);
    const int navButtonW      = juce::roundToInt(static_cast<float>(Widths::Button::kInit) * sf);
    const int bankNumberW     = juce::roundToInt(static_cast<float>(Widths::NumberBox::kPatchManagerBankNumber) * sf);
    const int patchNumberW    = juce::roundToInt(static_cast<float>(Widths::NumberBox::kPatchManagerPatchNumber) * sf);
    const int memButtonW      = juce::roundToInt(static_cast<float>(Widths::Button::kInternalPatchesMemory) * sf);
    const int buttonH         = juce::roundToInt(static_cast<float>(Heights::kButton) * sf);

    // Module header
    if (moduleHeader)
        moduleHeader->setBounds(0, 0, moduleHeaderW, moduleHeaderH);

    // Row 1 Y: group labels
    const int row1Y = juce::roundToInt(static_cast<float>(Heights::kModuleHeader) * sf);

    // Browser group label at X=0
    if (browserGroupLabel)
        browserGroupLabel->setBounds(0, row1Y, browserGroupW, groupLabelH);

    // Memory group label at X = browserGroupWidth + kGroupLabelSpacing
    const int memoryGroupX = juce::roundToInt(static_cast<float>(Widths::GroupLabel::kInternalPatchesBrowser + kGroupLabelSpacing_) * sf);
    if (memoryGroupLabel)
        memoryGroupLabel->setBounds(memoryGroupX, row1Y, memoryGroupW, groupLabelH);

    // Row 2 Y: buttons/numberboxes
    const int row2Y = juce::roundToInt(static_cast<float>(Heights::kModuleHeader + Heights::kGroupLabel) * sf);

    // Browser section: nav buttons + number boxes, each X computed independently
    const float navStep    = static_cast<float>(Widths::Button::kInit + kSpacing_) * sf;
    const float bankNumX   = navStep * 2.0f;
    const float patchNumX  = bankNumX + static_cast<float>(Widths::NumberBox::kPatchManagerBankNumber + kSpacing_) * sf;

    if (loadPreviousPatchButton_)
        loadPreviousPatchButton_->setBounds(0, row2Y, navButtonW, buttonH);
    if (loadNextPatchButton_)
        loadNextPatchButton_->setBounds(juce::roundToInt(navStep), row2Y, navButtonW, buttonH);
    if (currentBankNumber)
        currentBankNumber->setBounds(juce::roundToInt(bankNumX), row2Y, bankNumberW, buttonH);
    if (currentPatchNumber)
        currentPatchNumber->setBounds(juce::roundToInt(patchNumX), row2Y, patchNumberW, buttonH);

    // Memory section: 4 buttons, each X computed independently from memory origin
    const float memOriginX = static_cast<float>(Widths::GroupLabel::kInternalPatchesBrowser + kGroupLabelSpacing_) * sf;
    const float memStep    = static_cast<float>(Widths::Button::kInternalPatchesMemory + kSpacing_) * sf;

    if (initPatchButton_)
        initPatchButton_->setBounds(juce::roundToInt(memOriginX), row2Y, memButtonW, buttonH);
    if (copyPatchButton_)
        copyPatchButton_->setBounds(juce::roundToInt(memOriginX + 1.0f * memStep), row2Y, memButtonW, buttonH);
    if (pastePatchButton_)
        pastePatchButton_->setBounds(juce::roundToInt(memOriginX + 2.0f * memStep), row2Y, memButtonW, buttonH);
    if (storePatchButton_)
        storePatchButton_->setBounds(juce::roundToInt(memOriginX + 3.0f * memStep), row2Y, memButtonW, buttonH);

    if (moduleHeader)             moduleHeader->setDisplayScale(sf);
    if (browserGroupLabel)        browserGroupLabel->setDisplayScale(sf);
    if (memoryGroupLabel)         memoryGroupLabel->setDisplayScale(sf);
    if (currentBankNumber)        currentBankNumber->setDisplayScale(sf);
    if (currentPatchNumber)       currentPatchNumber->setDisplayScale(sf);
    if (loadPreviousPatchButton_) loadPreviousPatchButton_->setDisplayScale(sf);
    if (loadNextPatchButton_)     loadNextPatchButton_->setDisplayScale(sf);
    if (initPatchButton_)         initPatchButton_->setDisplayScale(sf);
    if (copyPatchButton_)         copyPatchButton_->setDisplayScale(sf);
    if (pastePatchButton_)        pastePatchButton_->setDisplayScale(sf);
    if (storePatchButton_)        storePatchButton_->setDisplayScale(sf);
}

void InternalPatchesPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    if (moduleHeader)
        moduleHeader->setLook(tss::moduleHeaderLookFromSkin(skin));
    if (browserGroupLabel)
        browserGroupLabel->setLook(tss::groupLabelLookFromSkin(skin));
    if (memoryGroupLabel)
        memoryGroupLabel->setLook(tss::groupLabelLookFromSkin(skin));
    if (currentBankNumber)
        currentBankNumber->setLook(tss::numberBoxLookFromSkin(skin));
    if (currentPatchNumber)
        currentPatchNumber->setLook(tss::numberBoxLookFromSkin(skin));

    if (loadPreviousPatchButton_)
        loadPreviousPatchButton_->setLook(tss::buttonLookFromSkin(skin));
    if (loadNextPatchButton_)
        loadNextPatchButton_->setLook(tss::buttonLookFromSkin(skin));
    if (initPatchButton_)
        initPatchButton_->setLook(tss::buttonLookFromSkin(skin));
    if (copyPatchButton_)
        copyPatchButton_->setLook(tss::buttonLookFromSkin(skin));
    if (pastePatchButton_)
        pastePatchButton_->setLook(tss::buttonLookFromSkin(skin));
    if (storePatchButton_)
        storePatchButton_->setLook(tss::buttonLookFromSkin(skin));
}

void InternalPatchesPanel::setDisplayScale(float displayScale)
{
    if (juce::approximatelyEqual(displayScale_, displayScale))
        return;
    
    displayScale_ = displayScale;
    repaint();
}

void InternalPatchesPanel::setupModuleHeader(WidgetFactory& widgetFactory, const juce::String& moduleId)
{
    moduleHeader = std::make_unique<tss::ModuleHeader>(
        widgetFactory.getGroupDisplayName(moduleId),
        PluginDimensions::Widgets::Widths::ModuleHeader::kPatchManagerModule,
        PluginDimensions::Widgets::Heights::kModuleHeader,
        tss::ModuleHeader::ColourVariant::Blue);
    addAndMakeVisible(*moduleHeader);
}

void InternalPatchesPanel::setupBrowserGroupLabel()
{
    browserGroupLabel = std::make_unique<tss::GroupLabel>(
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

void InternalPatchesPanel::setupCurrentBankNumberBox()
{
    currentBankNumber = std::make_unique<tss::NumberBox>(
        PluginDimensions::Widgets::Widths::NumberBox::kPatchManagerBankNumber,
        false,
        Matrix1000Limits::kMinBankNumber,
        Matrix1000Limits::kMaxBankNumber);
    currentBankNumber->setShowDot(true);
    addAndMakeVisible(*currentBankNumber);
}

void InternalPatchesPanel::setupCurrentPatchNumberBox()
{
    currentPatchNumber = std::make_unique<tss::NumberBox>(
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

void InternalPatchesPanel::setupMemoryGroupLabel()
{
    memoryGroupLabel = std::make_unique<tss::GroupLabel>(
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
