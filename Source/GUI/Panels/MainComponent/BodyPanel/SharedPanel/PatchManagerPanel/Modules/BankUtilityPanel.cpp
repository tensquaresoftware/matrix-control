#include "BankUtilityPanel.h"

#include "../PatchManagerEqualWidthStrip.h"

#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "GUI/Helpers/ClipboardFeedbackButtonBinding.h"
#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "GUI/Helpers/GrayedControlHelper.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/Button.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "GUI/Factories/WidgetFactory.h"
#include <juce_core/juce_core.h>

namespace
{

    bool isRootSectionLocked(const juce::AudioProcessorValueTreeState& apvts,
                             MatrixDeviceTypes::Type& deviceTypeOut)
    {
        const bool deviceDetected = static_cast<bool>(apvts.state.getProperty("deviceDetected"));
        const bool deviceMidiUnresponsive = static_cast<bool>(
            apvts.state.getProperty(Core::kDeviceMidiUnresponsiveProperty, false));
        deviceTypeOut = Core::DeviceTypeRegistry::fromApvtsProperty(
            apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
        const bool compareActive = static_cast<bool>(apvts.state.getProperty(
            PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties::kCompareActive,
            false));
        return Core::isSectionLocked(
            deviceDetected, deviceTypeOut, compareActive, deviceMidiUnresponsive);
    }

    void setSubtreeKeyboardInteractionEnabled(juce::Component& root, bool enabled)
    {
        root.setWantsKeyboardFocus(enabled);
        root.setMouseClickGrabsKeyboardFocus(enabled);

        for (int i = 0; i < root.getNumChildComponents(); ++i)
        {
            if (auto* child = root.getChildComponent(i))
                setSubtreeKeyboardInteractionEnabled(*child, enabled);
        }
    }

    void applyGrayedToChild(juce::Component* child, bool grayed)
    {
        if (child == nullptr)
            return;

        TSS::GrayedControlHelper::applyGrayedAppearance(*child, grayed);
        child->setInterceptsMouseClicks(! grayed, ! grayed);
        setSubtreeKeyboardInteractionEnabled(*child, ! grayed);
    }

    void setOptionalUiScale(TSS::ModuleHeader* header, float sf)
    {
        if (header != nullptr)
            header->setUiScale(sf);
    }

    void setOptionalUiScale(TSS::Button* button, float sf)
    {
        if (button != nullptr)
            button->setUiScale(sf);
    }

    void setOptionalLook(TSS::Button* button, const TSS::ButtonLook& look)
    {
        if (button != nullptr)
            button->setLook(look);
    }

    struct ScaledButtonPlacement
    {
        TSS::Button* button = nullptr;
        int x = 0;
        int y = 0;
        int designWidth = 0;
        int buttonHeight = 0;
        float uiScale = 1.0f;
    };

    void placeScaledButton(const ScaledButtonPlacement& placement)
    {
        if (placement.button == nullptr)
            return;

        placement.button->setBounds(
            placement.x,
            placement.y,
            TSS::ScaledLayout::scaledInt(static_cast<float>(placement.designWidth), placement.uiScale),
            placement.buttonHeight);
    }
}

BankUtilityPanel::BankUtilityPanel(TSS::ISkin& skin, const BankUtilityPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : dims_(dims)
    , skin_(&skin)
    , apvts_(apvts)
{
    setOpaque(false);
    setupModuleHeader(skin, widgetFactory, PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId);
    setupSelectBankButtons(skin, widgetFactory);
    setupUtilityButtons(skin, widgetFactory);
    setupClipboardFeedbackBindings();

    normalBankLook_ = TSS::buttonLookFromSkin(skin);
    apvts_.state.addListener(this);
    refreshDeviceGating();
    refreshCurrentBankMarker();
    refreshUtilityEnabled();
    registerContextualHelp();

    setSize(dims_.width, dims_.height);
}

void BankUtilityPanel::registerContextualHelp()
{
    namespace Help = PluginDisplayNames::PatchManagerSection::BankUtilityModule::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));

    static constexpr const char* kBankHelps[kBankCount] = {
        Help::kBank0, Help::kBank1, Help::kBank2, Help::kBank3, Help::kBank4,
        Help::kBank5, Help::kBank6, Help::kBank7, Help::kBank8, Help::kBank9,
    };

    for (int i = 0; i < kBankCount; ++i)
        contextualHelpBinder_->bind(selectBankButtons_[static_cast<size_t>(i)].get(), kBankHelps[i]);

    contextualHelpBinder_->bind(copyBankButton_.get(), Help::kCopy);
    contextualHelpBinder_->bind(pasteBankButton_.get(), Help::kPaste);
    contextualHelpBinder_->bind(importBankButton_.get(), Help::kImport);
    contextualHelpBinder_->bind(exportBankButton_.get(), Help::kExport);
}

BankUtilityPanel::~BankUtilityPanel()
{
    apvts_.state.removeListener(this);
}

void BankUtilityPanel::valueTreePropertyChanged(juce::ValueTree&,
                                                const juce::Identifier& property)
{
    const auto propertyName = property.toString();
    if (propertyName == MatrixDeviceTypes::kApvtsPropertyName
        || propertyName == "deviceDetected"
        || propertyName == Core::kDeviceMidiUnresponsiveProperty
        || propertyName == PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties::kCompareActive)
    {
        refreshDeviceGating();
    }
    else if (propertyName == PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kSelectedBank
             || propertyName == PluginIDs::PatchManagerSection::StateProperties::kPatchCoordinatesEstablished
             || propertyName == PluginIDs::PatchManagerSection::StateProperties::kNavigationFocus)
    {
        refreshCurrentBankMarker();
        if (propertyName != PluginIDs::PatchManagerSection::StateProperties::kNavigationFocus)
            refreshUtilityEnabled();
    }
    else if (propertyName == PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kPasteBankEnabled)
    {
        refreshUtilityEnabled();
    }
}

void BankUtilityPanel::valueTreeRedirected(juce::ValueTree&)
{
    refreshDeviceGating();
    refreshCurrentBankMarker();
    refreshUtilityEnabled();
}

void BankUtilityPanel::refreshDeviceGating()
{
    MatrixDeviceTypes::Type deviceType = MatrixDeviceTypes::Type::kUnknown;
    const bool rootLocked = isRootSectionLocked(apvts_, deviceType);
    const bool deviceDetected = static_cast<bool>(apvts_.state.getProperty("deviceDetected"));
    const auto limits = Core::DeviceMemoryLimits::resolve(deviceType);

    setBankUtilityGrayed(! rootLocked && deviceDetected && ! limits.hasBankConcept());
    refreshUtilityEnabled();
}

void BankUtilityPanel::setBankUtilityGrayed(bool grayed)
{
    bankUtilityGrayed_ = grayed;

    applyGrayedToChild(bankUtilityModuleHeader_.get(), grayed);

    for (auto& button : selectBankButtons_)
        applyGrayedToChild(button.get(), grayed);

    applyGrayedToChild(copyBankButton_.get(), grayed);
    applyGrayedToChild(pasteBankButton_.get(), grayed);

    if (grayed)
        giveAwayKeyboardFocus();

    refreshCurrentBankMarker();
    repaint();
}

void BankUtilityPanel::refreshImportBankEnabled(bool rootLocked, const Core::DeviceMemoryLimits& limits)
{
    if (importBankButton_ == nullptr)
        return;

    if (rootLocked)
    {
        importBankButton_->setEnabled(false);
        return;
    }

    if (! limits.hasBankConcept())
    {
        importBankButton_->setEnabled(true);
        return;
    }

    const int selectedBank = static_cast<int>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kSelectedBank,
        0));
    importBankButton_->setEnabled(! limits.isRomBank(selectedBank));
}

void BankUtilityPanel::refreshPasteBankEnabled(bool rootLocked)
{
    if (pasteBankButton_ == nullptr)
        return;

    if (rootLocked || bankUtilityGrayed_)
    {
        pasteBankButton_->setEnabled(false);
        return;
    }

    const bool pasteEnabled = static_cast<bool>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kPasteBankEnabled,
        false));
    pasteBankButton_->setEnabled(pasteEnabled);
}

void BankUtilityPanel::refreshClipboardBlinkBindings()
{
    if (copyFeedbackBinding_ != nullptr)
        copyFeedbackBinding_->refresh();
    if (pasteFeedbackBinding_ != nullptr)
        pasteFeedbackBinding_->refresh();
}

void BankUtilityPanel::refreshUtilityEnabled()
{
    MatrixDeviceTypes::Type deviceType = MatrixDeviceTypes::Type::kUnknown;
    const bool rootLocked = isRootSectionLocked(apvts_, deviceType);
    const auto limits = Core::DeviceMemoryLimits::resolve(deviceType);

    if (exportBankButton_)
        exportBankButton_->setEnabled(! rootLocked);

    if (copyBankButton_)
        copyBankButton_->setEnabled(! rootLocked && ! bankUtilityGrayed_);

    refreshImportBankEnabled(rootLocked, limits);
    refreshPasteBankEnabled(rootLocked);
    refreshClipboardBlinkBindings();
}

void BankUtilityPanel::refreshCurrentBankMarker()
{
    const int currentBank = static_cast<int>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kSelectedBank,
        0));
    const bool coordinatesEstablished = static_cast<bool>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::StateProperties::kPatchCoordinatesEstablished,
        false));
    const int navigationFocus = static_cast<int>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::StateProperties::kNavigationFocus,
        PluginIDs::PatchManagerSection::NavigationFocus::kDefault));
    const bool internalHasFocus =
        navigationFocus == PluginIDs::PatchManagerSection::NavigationFocus::kInternal;

    for (auto& button : selectBankButtons_)
        setOptionalLook(button.get(), normalBankLook_);

    // Mark only while Internal owns navigation focus and a bank of work exists.
    // When Computer (or none) owns focus, CurrentBankNumber still shows the bank.
    if (bankUtilityGrayed_ || ! coordinatesEstablished || ! internalHasFocus)
        return;

    if (currentBank < 0 || currentBank >= kBankCount)
        return;

    if (auto* button = selectBankButtons_[static_cast<size_t>(currentBank)].get())
        button->setLook(makeCurrentBankMarkerLook());
}

TSS::ButtonLook BankUtilityPanel::makeCurrentBankMarkerLook() const
{
    // Red text matches Internal NumberBoxes / PATCH NAME navigation focus.
    auto markerLook = normalBankLook_;

    if (skin_ == nullptr)
        return markerLook;

    const auto focusColour = skin_->getColour(TSS::SkinColourId::kNumberBoxTextFocus);

    markerLook.textOff = focusColour;
    markerLook.textOn = focusColour;
    markerLook.textHover = focusColour;
    markerLook.textClicked = focusColour;
    markerLook.textDisabled = focusColour;

    return markerLook;
}

void BankUtilityPanel::layoutContentRows(float sf)
{
    const int moduleHeaderHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.moduleHeader.height), sf);
    const int moduleHeaderWidth = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.moduleHeader.patchManagerTitleBandWidth), sf);
    const int buttonHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.buttons.height), sf);
    const int copyWidth = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.buttons.patchManagerCopyBankWidth), sf);
    const int rowGap = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.layout.interControlGap), sf);
    const int rowH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.layout.contentRowHeight), sf);
    const int selectorToUtilityGap = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.selectorToUtilityGap), sf);

    if (auto* header = bankUtilityModuleHeader_.get())
        header->setBounds(0, 0, moduleHeaderWidth, moduleHeaderHeight);

    const int row1Y = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.moduleHeader.height), sf);
    const int row2Y = row1Y + rowH + rowGap;

    juce::Component* row1Banks[5] = {};
    juce::Component* row2Banks[5] = {};
    for (int i = 0; i < 5; ++i)
    {
        row1Banks[i] = selectBankButtons_[static_cast<size_t>(i)].get();
        row2Banks[i] = selectBankButtons_[static_cast<size_t>(i + 5)].get();
    }

    const int selectorEndX = TSS::placeEqualWidthStrip({
        .startX = 0, .y = row1Y, .uiScale = sf,
        .designWidth = dims_.buttons.patchManagerBankSelectWidth,
        .designHeight = dims_.buttons.height,
        .designGap = dims_.layout.interControlGap,
        .controls = row1Banks, .count = 5,
    });
    TSS::placeEqualWidthStrip({
        .startX = 0, .y = row2Y, .uiScale = sf,
        .designWidth = dims_.buttons.patchManagerBankSelectWidth,
        .designHeight = dims_.buttons.height,
        .designGap = dims_.layout.interControlGap,
        .controls = row2Banks, .count = 5,
    });

    const int utilityX = selectorEndX + selectorToUtilityGap;
    const int utilitySubcol2X = utilityX + copyWidth + rowGap;
    placeScaledButton({ copyBankButton_.get(), utilityX, row1Y, dims_.buttons.patchManagerCopyBankWidth, buttonHeight, sf });
    placeScaledButton({ pasteBankButton_.get(), utilityX, row2Y, dims_.buttons.patchManagerPasteBankWidth, buttonHeight, sf });
    placeScaledButton({ exportBankButton_.get(), utilitySubcol2X, row1Y, dims_.buttons.patchManagerExportBankWidth, buttonHeight, sf });
    placeScaledButton({ importBankButton_.get(), utilitySubcol2X, row2Y, dims_.buttons.patchManagerImportBankWidth, buttonHeight, sf });
}

void BankUtilityPanel::applyChildUiScales(float sf)
{
    setOptionalUiScale(bankUtilityModuleHeader_.get(), sf);
    setOptionalUiScale(copyBankButton_.get(), sf);
    setOptionalUiScale(pasteBankButton_.get(), sf);
    setOptionalUiScale(importBankButton_.get(), sf);
    setOptionalUiScale(exportBankButton_.get(), sf);

    for (auto& button : selectBankButtons_)
        setOptionalUiScale(button.get(), sf);
}

void BankUtilityPanel::resized()
{
    layoutContentRows(uiScale_);
    applyChildUiScales(uiScale_);
}

void BankUtilityPanel::applyNormalLookToActionButtons()
{
    setOptionalLook(copyBankButton_.get(), normalBankLook_);
    setOptionalLook(pasteBankButton_.get(), normalBankLook_);
    setOptionalLook(importBankButton_.get(), normalBankLook_);
    setOptionalLook(exportBankButton_.get(), normalBankLook_);

    for (auto& button : selectBankButtons_)
        setOptionalLook(button.get(), normalBankLook_);
}

void BankUtilityPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    normalBankLook_ = TSS::buttonLookFromSkin(skin);

    if (bankUtilityModuleHeader_)
        bankUtilityModuleHeader_->setLook(TSS::moduleHeaderLookFromSkin(skin));

    applyNormalLookToActionButtons();
    refreshCurrentBankMarker();

    if (copyFeedbackBinding_)
        copyFeedbackBinding_->refresh();
    if (pasteFeedbackBinding_)
        pasteFeedbackBinding_->refresh();
}

void BankUtilityPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    repaint();
}
