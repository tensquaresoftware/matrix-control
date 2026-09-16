#include "InternalPatchesPanel.h"

#include "../PatchManagerEqualWidthStrip.h"

#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "GUI/Helpers/ClipboardFeedbackButtonBinding.h"
#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/GroupLabel.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/NumberBox.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"
#include <juce_core/juce_core.h>


namespace
{
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
        int memoryGroupW = 0;
        int buttonH = 0;
        int columnGap = 0;

        static LayoutMetrics make(const InternalPatchesPanelDimensions& dims, float sf)
        {
            LayoutMetrics m;
            m.moduleHeaderH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims.moduleHeader.height), sf);
            m.moduleHeaderW = TSS::ScaledLayout::scaledInt(
                static_cast<float>(dims.moduleHeader.patchManagerTitleBandWidth), sf);
            m.groupLabelH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims.groupLabels.height), sf);
            m.browserGroupW = TSS::ScaledLayout::scaledInt(
                static_cast<float>(dims.groupLabels.internalPatchesBrowserWidth), sf);
            m.memoryGroupW = TSS::ScaledLayout::scaledInt(
                static_cast<float>(dims.groupLabels.internalPatchesMemoryWidth), sf);
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
}

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

    if (copyPatchButton_ != nullptr)
    {
        copyFeedbackBinding_ = std::make_unique<TSS::ClipboardFeedbackButtonBinding>(
            apvts_,
            *copyPatchButton_,
            PluginIDs::ClipboardFeedback::kInternalPatchesCopy,
            true);
    }
    if (pastePatchButton_ != nullptr)
    {
        pasteFeedbackBinding_ = std::make_unique<TSS::ClipboardFeedbackButtonBinding>(
            apvts_,
            *pastePatchButton_,
            PluginIDs::ClipboardFeedback::kInternalPatchesPaste,
            false);
    }

    apvts_.state.addListener(this);
    clipboardPasteEnabled_ = static_cast<bool>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatchEnabled,
        false));
    refreshDeviceLimits();
    syncNumberBoxesFromApvts();

    if (initPatchButton_)
        initPatchButton_->addMouseListener(this, false);
    if (pastePatchButton_)
        pastePatchButton_->addMouseListener(this, false);
    if (storePatchButton_)
        storePatchButton_->addMouseListener(this, false);

    registerContextualHelp();
    setSize(dims_.width, dims_.height);
}

void InternalPatchesPanel::registerContextualHelp()
{
    namespace Help = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));

    contextualHelpBinder_->bind(moduleHeader.get(), Help::kTitle);
    contextualHelpBinder_->bind(browserGroupLabel.get(), Help::kBrowser);
    contextualHelpBinder_->bind(memoryGroupLabel.get(), Help::kMemory);
    contextualHelpBinder_->bind(loadPreviousPatchButton_.get(), Help::kPrevious);
    contextualHelpBinder_->bind(loadNextPatchButton_.get(), Help::kNext);
    contextualHelpBinder_->bind(currentBankNumber.get(), Help::kCurrentBank);
    contextualHelpBinder_->bind(currentPatchNumber.get(), Help::kCurrentPatch);
    contextualHelpBinder_->bind(initPatchButton_.get(), Help::kInit);
    contextualHelpBinder_->bind(copyPatchButton_.get(), Help::kCopy);
    contextualHelpBinder_->bind(pastePatchButton_.get(), Help::kPaste);
    contextualHelpBinder_->bind(storePatchButton_.get(), Help::kStore);
}

InternalPatchesPanel::~InternalPatchesPanel()
{
    if (initPatchButton_)
        initPatchButton_->removeMouseListener(this);
    if (pastePatchButton_)
        pastePatchButton_->removeMouseListener(this);
    if (storePatchButton_)
        storePatchButton_->removeMouseListener(this);

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

    if (propertyName == PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatchEnabled)
    {
        clipboardPasteEnabled_ = static_cast<bool>(treeWhosePropertyHasChanged.getProperty(property, false));
        refreshDeviceLimits();
    }

    if (propertyName == PluginIDs::PatchManagerSection::StateProperties::kPatchCoordinatesEstablished
        || propertyName == PluginIDs::PatchManagerSection::StateProperties::kNavigationFocus)
    {
        refreshCoordinateDisplayStates();
    }

    if (propertyName == PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties::kCompareActive)
        refreshDeviceLimits();
}

void InternalPatchesPanel::valueTreeRedirected(juce::ValueTree&)
{
    clipboardPasteEnabled_ = static_cast<bool>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatchEnabled,
        false));
    refreshDeviceLimits();
    syncNumberBoxesFromApvts();
}

void InternalPatchesPanel::layoutBrowserRow(float sf, int row2Y, int buttonH)
{
    const int navButtonW = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.buttons.initWidth), sf);
    const int bankNumberW = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.numberBoxes.bankNumberWidth), sf);
    const int patchNumberW = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.numberBoxes.patchNumberWidth), sf);
    const int interGap = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.layout.interControlGap), sf);

    auto browserRow = juce::Rectangle<int>(0, row2Y, getWidth(), buttonH);

    placeOrSkipLeft(browserRow, loadPreviousPatchButton_.get(), navButtonW);
    browserRow.removeFromLeft(interGap);

    placeOrSkipLeft(browserRow, loadNextPatchButton_.get(), navButtonW);
    browserRow.removeFromLeft(interGap);

    placeOrSkipLeft(browserRow, currentBankNumber.get(), bankNumberW);
    browserRow.removeFromLeft(interGap);

    if (currentPatchNumber)
        currentPatchNumber->setBounds(browserRow.removeFromLeft(patchNumberW));
}

void InternalPatchesPanel::layoutContentRows(float sf)
{
    const auto m = LayoutMetrics::make(dims_, sf);

    if (moduleHeader)
        moduleHeader->setBounds(0, 0, m.moduleHeaderW, m.moduleHeaderH);

    const int row1Y = m.moduleHeaderH;
    if (browserGroupLabel)
        browserGroupLabel->setBounds(0, row1Y, m.browserGroupW, m.groupLabelH);

    const int memoryGroupX = m.browserGroupW + m.columnGap;
    if (memoryGroupLabel)
        memoryGroupLabel->setBounds(memoryGroupX, row1Y, m.memoryGroupW, m.groupLabelH);

    // Row 2 — successive integer strips (fixed widths; bank NumberBox always laid out)
    const int row2Y = row1Y + m.groupLabelH;
    layoutBrowserRow(sf, row2Y, m.buttonH);

    juce::Component* memButtons[] = {
        initPatchButton_.get(), copyPatchButton_.get(),
        pastePatchButton_.get(), storePatchButton_.get()
    };
    TSS::placeEqualWidthStrip({
        .startX = memoryGroupX,
        .y = row2Y,
        .uiScale = sf,
        .designWidth = dims_.buttons.internalPatchesInitWidth,
        .designHeight = dims_.buttons.height,
        .designGap = dims_.layout.interControlGap,
        .controls = memButtons,
        .count = 4,
    });
}

void InternalPatchesPanel::applyChildUiScales(float sf)
{
    setOptionalUiScale(moduleHeader.get(), sf);
    setOptionalUiScale(browserGroupLabel.get(), sf);
    setOptionalUiScale(memoryGroupLabel.get(), sf);
    setOptionalUiScale(currentBankNumber.get(), sf);
    setOptionalUiScale(currentPatchNumber.get(), sf);
    setOptionalUiScale(loadPreviousPatchButton_.get(), sf);
    setOptionalUiScale(loadNextPatchButton_.get(), sf);
    setOptionalUiScale(initPatchButton_.get(), sf);
    setOptionalUiScale(copyPatchButton_.get(), sf);
    setOptionalUiScale(pastePatchButton_.get(), sf);
    setOptionalUiScale(storePatchButton_.get(), sf);
}

void InternalPatchesPanel::resized()
{
    layoutContentRows(uiScale_);
    applyChildUiScales(uiScale_);
}

void InternalPatchesPanel::applyChildLooks(TSS::ISkin& skin)
{
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

void InternalPatchesPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    applyChildLooks(skin);
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

    bankNumberAvailable_ = limits.hasBankConcept();

    applyPatchNumberRange(limits);

    const int currentBank = static_cast<int>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber,
        Matrix1000Limits::kMinBankNumber));
    updatePasteStoreEnabled(limits, currentBank);
    refreshCoordinateDisplayStates();

    resized();
}

void InternalPatchesPanel::refreshCoordinateDisplayStates()
{
    using DisplayState = TSS::NumberBox::DisplayState;

    const bool coordinatesEstablished = static_cast<bool>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::StateProperties::kPatchCoordinatesEstablished,
        false));
    const int navigationFocus = static_cast<int>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::StateProperties::kNavigationFocus,
        PluginIDs::PatchManagerSection::NavigationFocus::kDefault));
    const bool internalHasFocus =
        navigationFocus == PluginIDs::PatchManagerSection::NavigationFocus::kInternal;

    const auto coordinateState = coordinatesEstablished ? DisplayState::kValue : DisplayState::kUndefined;

    if (currentBankNumber)
    {
        // Matrix-6/6R has no banks: empty disabled slot (disabled fill from NumberBox skin).
        currentBankNumber->setDisplayState(bankNumberAvailable_ ? coordinateState
                                                               : DisplayState::kUnavailable);
        currentBankNumber->setEnabled(bankNumberAvailable_);
        currentBankNumber->setAlpha(1.0f); // solid skin fill — never stack alpha gray
        currentBankNumber->setFocusHighlight(bankNumberAvailable_ && internalHasFocus);
    }

    if (currentPatchNumber)
    {
        currentPatchNumber->setDisplayState(coordinateState);
        currentPatchNumber->setFocusHighlight(internalHasFocus);
    }
}

void InternalPatchesPanel::syncNumberBoxesFromApvts()
{
    const int bank = static_cast<int>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber,
        Matrix1000Limits::kMinBankNumber));
    const int patch = static_cast<int>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber,
        Matrix1000Limits::kMinPatchNumber));

    if (currentBankNumber)
        currentBankNumber->setValue(bank);
    if (currentPatchNumber)
        currentPatchNumber->setValue(patch);
}

void InternalPatchesPanel::applyPatchNumberRange(const Core::DeviceMemoryLimits& limits)
{
    if (currentPatchNumber)
        currentPatchNumber->setRange(limits.minPatchNumber(), limits.maxPatchNumber());
}

void InternalPatchesPanel::updatePasteStoreEnabled(const Core::DeviceMemoryLimits& limits, int currentBank)
{
    romPasteStoreBlocked_ = ! limits.isPasteStoreAllowed(currentBank);
    const bool compareActive = static_cast<bool>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties::kCompareActive,
        false));

    wirePasteStoreButton(
        initPatchButton_.get(),
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kInitPatch,
        ! compareActive);
    wirePasteStoreButton(
        pastePatchButton_.get(),
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch,
        clipboardPasteEnabled_);
    wirePasteStoreButton(
        storePatchButton_.get(),
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch,
        ! compareActive);

    if (pasteFeedbackBinding_ != nullptr)
        pasteFeedbackBinding_->refresh();
}

void InternalPatchesPanel::wirePasteStoreButton(TSS::Button* button,
                                                const juce::Identifier& actionPropertyId,
                                                bool functionallyEnabled)
{
    if (button == nullptr)
        return;

    if (romPasteStoreBlocked_)
    {
        // Grayed state alone signals unavailability — no warning footer on click/hover.
        button->setInactiveAppearance(true);
        button->setEnabled(false);
        button->setAlpha(1.0f);
        button->onClick = nullptr;
        return;
    }

    button->setInactiveAppearance(false);
    button->setAlpha(1.0f);
    button->setEnabled(functionallyEnabled);
    button->onClick = [this, actionPropertyId]
    {
        apvts_.state.setProperty(actionPropertyId, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
    };
}
