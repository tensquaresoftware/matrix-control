#include "InternalPatchesPanel.h"

#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/GroupLabel.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/NumberBox.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"
#include <juce_core/juce_core.h>


InternalPatchesPanel::InternalPatchesPanel(TSS::ISkin& skin, const InternalPatchesPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : dims_(dims)
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
    refreshDeviceLimits();
    
    setSize(dims_.width, dims_.height);
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
    
    // Sync NumberBox from Core state (via APVTS property)
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

    if (propertyName == MatrixDeviceTypes::kApvtsPropertyName
        || propertyName == "deviceDetected"
        || propertyName == PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber)
    {
        refreshDeviceLimits();
    }
}

void InternalPatchesPanel::resized()
{
    const float sf = uiScale_;

    // Dimensions (scaled)
    const int moduleHeaderH   = juce::roundToInt(static_cast<float>(dims_.moduleHeader.height) * sf);
    const int moduleHeaderW   = juce::roundToInt(static_cast<float>(dims_.moduleHeader.patchManagerTitleBandWidth) * sf);
    const int groupLabelH     = juce::roundToInt(static_cast<float>(dims_.groupLabels.height) * sf);
    const int browserGroupW   = juce::roundToInt(static_cast<float>(dims_.groupLabels.internalPatchesBrowserWidth) * sf);
    const int memoryGroupW    = juce::roundToInt(static_cast<float>(dims_.groupLabels.internalPatchesMemoryWidth) * sf);
    const int navButtonW      = juce::roundToInt(static_cast<float>(dims_.buttons.initWidth) * sf);
    const int bankNumberW     = juce::roundToInt(static_cast<float>(dims_.numberBoxes.bankNumberWidth) * sf);
    const int patchNumberW    = juce::roundToInt(static_cast<float>(dims_.numberBoxes.patchNumberWidth) * sf);
    const int memButtonW      = juce::roundToInt(static_cast<float>(dims_.buttons.internalPatchesInitWidth) * sf);
    const int buttonH         = juce::roundToInt(static_cast<float>(dims_.buttons.height) * sf);

    // Module header
    if (moduleHeader)
        moduleHeader->setBounds(0, 0, moduleHeaderW, moduleHeaderH);

    // Row 1 Y: group labels
    const int row1Y = juce::roundToInt(static_cast<float>(dims_.moduleHeader.height) * sf);

    // Browser group label at X=0
    if (browserGroupLabel)
        browserGroupLabel->setBounds(0, row1Y, browserGroupW, groupLabelH);

    // Memory group label at X = browserGroupWidth + kGroupLabelGap
    const int memoryGroupX = juce::roundToInt(
        static_cast<float>(dims_.groupLabels.internalPatchesBrowserWidth + dims_.layout.columnGap) * sf);
    if (memoryGroupLabel)
        memoryGroupLabel->setBounds(memoryGroupX, row1Y, memoryGroupW, groupLabelH);

    // Row 2 Y: buttons/numberboxes
    const int row2Y = juce::roundToInt(static_cast<float>(dims_.moduleHeader.height + dims_.groupLabels.height) * sf);

    // Browser section: nav buttons + number boxes, each X computed independently
    const float navStep    = static_cast<float>(dims_.buttons.initWidth + dims_.layout.interControlGap) * sf;
    const float bankNumX   = navStep * 2.0f;
    const float patchNumX  = bankNumberVisible_
        ? bankNumX + static_cast<float>(dims_.numberBoxes.bankNumberWidth + dims_.layout.interControlGap) * sf
        : bankNumX;

    if (loadPreviousPatchButton_)
        loadPreviousPatchButton_->setBounds(0, row2Y, navButtonW, buttonH);
    if (loadNextPatchButton_)
        loadNextPatchButton_->setBounds(juce::roundToInt(navStep), row2Y, navButtonW, buttonH);
    if (currentBankNumber && bankNumberVisible_)
        currentBankNumber->setBounds(juce::roundToInt(bankNumX), row2Y, bankNumberW, buttonH);
    if (currentPatchNumber)
        currentPatchNumber->setBounds(juce::roundToInt(patchNumX), row2Y, patchNumberW, buttonH);

    // Memory section: 4 buttons, each X computed independently from memory origin
    const float memOriginX = static_cast<float>(dims_.groupLabels.internalPatchesBrowserWidth + dims_.layout.columnGap) * sf;
    const float memStep    = static_cast<float>(dims_.buttons.internalPatchesInitWidth + dims_.layout.interControlGap) * sf;

    if (initPatchButton_)
        initPatchButton_->setBounds(juce::roundToInt(memOriginX), row2Y, memButtonW, buttonH);
    if (copyPatchButton_)
        copyPatchButton_->setBounds(juce::roundToInt(memOriginX + 1.0f * memStep), row2Y, memButtonW, buttonH);
    if (pastePatchButton_)
        pastePatchButton_->setBounds(juce::roundToInt(memOriginX + 2.0f * memStep), row2Y, memButtonW, buttonH);
    if (storePatchButton_)
        storePatchButton_->setBounds(juce::roundToInt(memOriginX + 3.0f * memStep), row2Y, memButtonW, buttonH);

    if (moduleHeader)             moduleHeader->setUiScale(sf);
    if (browserGroupLabel)        browserGroupLabel->setUiScale(sf);
    if (memoryGroupLabel)         memoryGroupLabel->setUiScale(sf);
    if (currentBankNumber)        currentBankNumber->setUiScale(sf);
    if (currentPatchNumber)       currentPatchNumber->setUiScale(sf);
    if (loadPreviousPatchButton_) loadPreviousPatchButton_->setUiScale(sf);
    if (loadNextPatchButton_)     loadNextPatchButton_->setUiScale(sf);
    if (initPatchButton_)         initPatchButton_->setUiScale(sf);
    if (copyPatchButton_)         copyPatchButton_->setUiScale(sf);
    if (pastePatchButton_)        pastePatchButton_->setUiScale(sf);
    if (storePatchButton_)        storePatchButton_->setUiScale(sf);
}

void InternalPatchesPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    if (moduleHeader)
        moduleHeader->setLook(TSS::moduleHeaderLookFromSkin(skin));
    if (browserGroupLabel)
        browserGroupLabel->setLook(TSS::groupLabelLookFromSkin(skin));
    if (memoryGroupLabel)
        memoryGroupLabel->setLook(TSS::groupLabelLookFromSkin(skin));
    if (currentBankNumber)
        currentBankNumber->setLook(TSS::numberBoxLookFromSkin(skin));
    if (currentPatchNumber)
        currentPatchNumber->setLook(TSS::numberBoxLookFromSkin(skin));

    if (loadPreviousPatchButton_)
        loadPreviousPatchButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (loadNextPatchButton_)
        loadNextPatchButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (initPatchButton_)
        initPatchButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (copyPatchButton_)
        copyPatchButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (pastePatchButton_)
        pastePatchButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (storePatchButton_)
        storePatchButton_->setLook(TSS::buttonLookFromSkin(skin));
}

void InternalPatchesPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    repaint();
}

void InternalPatchesPanel::refreshDeviceLimits()
{
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts_.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    const auto limits = Core::DeviceMemoryLimits::resolve(deviceType);

    bankNumberVisible_ = limits.hasBankConcept();
    if (currentBankNumber)
        currentBankNumber->setVisible(bankNumberVisible_);

    applyPatchNumberRange(limits);

    const int currentBank = static_cast<int>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber,
        Matrix1000Limits::kMinBankNumber));
    updatePasteStoreEnabled(limits, currentBank);

    resized();
}

void InternalPatchesPanel::applyPatchNumberRange(const Core::DeviceMemoryLimits& limits)
{
    if (currentPatchNumber)
        currentPatchNumber->setRange(limits.minPatchNumber(), limits.maxPatchNumber());
}

void InternalPatchesPanel::updatePasteStoreEnabled(const Core::DeviceMemoryLimits& limits, int currentBank)
{
    const bool allowPasteStore = limits.isPasteStoreAllowed(currentBank);

    if (pastePatchButton_)
        pastePatchButton_->setEnabled(allowPasteStore);

    if (storePatchButton_)
        storePatchButton_->setEnabled(allowPasteStore);
}

void InternalPatchesPanel::setupModuleHeader(TSS::ISkin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId)
{
    moduleHeader = std::make_unique<TSS::ModuleHeader>(
        dims_.moduleHeader.patchManagerTitleBandWidth,
        dims_.moduleHeader.height,
        TSS::moduleHeaderLookFromSkin(skin),
        TSS::ModuleHeader::ColourVariant::Blue,
        widgetFactory.getGroupDisplayName(moduleId),
        dims_.moduleHeader);
    addAndMakeVisible(*moduleHeader);
}

void InternalPatchesPanel::setupBrowserGroupLabel(TSS::ISkin& skin)
{
    browserGroupLabel = std::make_unique<TSS::GroupLabel>(
        dims_.groupLabels.internalPatchesBrowserWidth,
        dims_.groupLabels.height,
        TSS::groupLabelLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kBrowser);
    addAndMakeVisible(*browserGroupLabel);
}

void InternalPatchesPanel::setupLoadPreviousPatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    loadPreviousPatchButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch,
        skin,
        dims_.buttons.height);
    loadPreviousPatchButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*loadPreviousPatchButton_);
}

void InternalPatchesPanel::setupLoadNextPatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    loadNextPatchButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadNextPatch,
        skin,
        dims_.buttons.height);
    loadNextPatchButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadNextPatch,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*loadNextPatchButton_);
}

void InternalPatchesPanel::setupCurrentBankNumberBox(TSS::ISkin& skin)
{
    currentBankNumber = std::make_unique<TSS::NumberBox>(
        dims_.numberBoxes.bankNumberWidth,
        dims_.numberBoxes.height,
        TSS::numberBoxLookFromSkin(skin),
        false,
        Matrix1000Limits::kMinBankNumber,
        Matrix1000Limits::kMaxBankNumber);
    currentBankNumber->setShowDot(true);
    addAndMakeVisible(*currentBankNumber);
}

void InternalPatchesPanel::setupCurrentPatchNumberBox(TSS::ISkin& skin)
{
    currentPatchNumber = std::make_unique<TSS::NumberBox>(
        dims_.numberBoxes.patchNumberWidth,
        dims_.numberBoxes.height,
        TSS::numberBoxLookFromSkin(skin),
        true,
        Matrix1000Limits::kMinPatchNumber,
        Matrix1000Limits::kMaxPatchNumber);
    
    // Push value to APVTS via property (same pattern as buttons)
    currentPatchNumber->setOnValueChanged([this](int newValue)
    {
        // Avoid feedback loop when the value already came from the ValueTree
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

void InternalPatchesPanel::setupMemoryGroupLabel(TSS::ISkin& skin)
{
    memoryGroupLabel = std::make_unique<TSS::GroupLabel>(
        dims_.groupLabels.internalPatchesMemoryWidth,
        dims_.groupLabels.height,
        TSS::groupLabelLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kMemory);
    addAndMakeVisible(*memoryGroupLabel);
}

void InternalPatchesPanel::setupInitPatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    initPatchButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kInitPatch,
        skin,
        dims_.buttons.height);
    initPatchButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kInitPatch,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*initPatchButton_);
}

void InternalPatchesPanel::setupCopyPatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    copyPatchButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCopyPatch,
        skin,
        dims_.buttons.height);
    copyPatchButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCopyPatch,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*copyPatchButton_);
}

void InternalPatchesPanel::setupPastePatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    pastePatchButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch,
        skin,
        dims_.buttons.height);
    pastePatchButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*pastePatchButton_);
}

void InternalPatchesPanel::setupStorePatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    storePatchButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch,
        skin,
        dims_.buttons.height);
    storePatchButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*storePatchButton_);
}
