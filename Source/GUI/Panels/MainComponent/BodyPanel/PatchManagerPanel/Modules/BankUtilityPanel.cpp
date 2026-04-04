#include "BankUtilityPanel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Button.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDimensions.h"
#include "GUI/Factories/WidgetFactory.h"
#include <juce_core/juce_core.h>


BankUtilityPanel::BankUtilityPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , apvts_(apvts)
{
    setOpaque(false);
    setupModuleHeader(skin, widgetFactory, PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId);
    setupBankSelectorLabel(skin);
    setupSelectBankButtons(skin, widgetFactory);

    setSize(width_, height_);
}

BankUtilityPanel::~BankUtilityPanel() = default;

void BankUtilityPanel::resized()
{
    using namespace PluginDimensions::Widgets;
    const float sf = displayScale_;

    const int moduleHeaderHeight  = juce::roundToInt(static_cast<float>(Heights::kModuleHeader) * sf);
    const int moduleHeaderWidth   = juce::roundToInt(static_cast<float>(Widths::ModuleHeader::kPatchManagerModule) * sf);
    const int labelWidth          = juce::roundToInt(static_cast<float>(Widths::Label::kPatchManagerBankSelector) * sf);
    const int labelHeight         = juce::roundToInt(static_cast<float>(Heights::kLabel) * sf);
    const int buttonWidth         = juce::roundToInt(static_cast<float>(Widths::Button::kPatchManagerBankSelect) * sf);
    const int buttonHeight        = juce::roundToInt(static_cast<float>(Heights::kButton) * sf);
    const int unlockButtonWidth   = juce::roundToInt(static_cast<float>(Widths::Button::kPatchManagerUnlockBank) * sf);

    if (auto* header = bankUtilityModuleHeader_.get())
        header->setBounds(0, 0, moduleHeaderWidth, moduleHeaderHeight);

    // Row 1 Y (computed from float to avoid accumulation with row2 Y)
    const int row1Y = juce::roundToInt(static_cast<float>(Heights::kModuleHeader + kTopPadding_) * sf);

    if (auto* label = bankSelectorLabel_.get())
        label->setBounds(0, row1Y, labelWidth, labelHeight);

    // Row 1 X positions: each computed independently from float origin
    const float row1OriginX = static_cast<float>(Widths::Label::kPatchManagerBankSelector + kSpacing_) * sf;
    const float bankButtonStep = static_cast<float>(Widths::Button::kPatchManagerBankSelect + kSpacing_) * sf;

    auto setBankButtonBounds = [&](tss::Button* btn, int index, int y)
    {
        if (btn)
        {
            const int x = juce::roundToInt(row1OriginX + static_cast<float>(index) * bankButtonStep);
            btn->setBounds(x, y, buttonWidth, buttonHeight);
        }
    };

    setBankButtonBounds(selectBank0Button_.get(), 0, row1Y);
    setBankButtonBounds(selectBank1Button_.get(), 1, row1Y);
    setBankButtonBounds(selectBank2Button_.get(), 2, row1Y);
    setBankButtonBounds(selectBank3Button_.get(), 3, row1Y);
    setBankButtonBounds(selectBank4Button_.get(), 4, row1Y);

    // Row 2 Y (computed independently from float origin)
    const int row2Y = juce::roundToInt(static_cast<float>(Heights::kModuleHeader + kTopPadding_ + Heights::kButton + kSpacing_) * sf);

    if (auto* button = unlockBankButton_.get())
        button->setBounds(0, row2Y, unlockButtonWidth, buttonHeight);

    // Row 2 X positions: banks 5-9, after unlock button
    const float row2OriginX = static_cast<float>(Widths::Button::kPatchManagerUnlockBank + kSpacing_) * sf;

    auto setBankButtonBoundsRow2 = [&](tss::Button* btn, int index, int y)
    {
        if (btn)
        {
            const int x = juce::roundToInt(row2OriginX + static_cast<float>(index) * bankButtonStep);
            btn->setBounds(x, y, buttonWidth, buttonHeight);
        }
    };

    setBankButtonBoundsRow2(selectBank5Button_.get(), 0, row2Y);
    setBankButtonBoundsRow2(selectBank6Button_.get(), 1, row2Y);
    setBankButtonBoundsRow2(selectBank7Button_.get(), 2, row2Y);
    setBankButtonBoundsRow2(selectBank8Button_.get(), 3, row2Y);
    setBankButtonBoundsRow2(selectBank9Button_.get(), 4, row2Y);

    if (bankSelectorLabel_)      bankSelectorLabel_->setDisplayScale(sf);
    if (bankUtilityModuleHeader_) bankUtilityModuleHeader_->setDisplayScale(sf);
    if (unlockBankButton_)       unlockBankButton_->setDisplayScale(sf);
    if (selectBank0Button_)      selectBank0Button_->setDisplayScale(sf);
    if (selectBank1Button_)      selectBank1Button_->setDisplayScale(sf);
    if (selectBank2Button_)      selectBank2Button_->setDisplayScale(sf);
    if (selectBank3Button_)      selectBank3Button_->setDisplayScale(sf);
    if (selectBank4Button_)      selectBank4Button_->setDisplayScale(sf);
    if (selectBank5Button_)      selectBank5Button_->setDisplayScale(sf);
    if (selectBank6Button_)      selectBank6Button_->setDisplayScale(sf);
    if (selectBank7Button_)      selectBank7Button_->setDisplayScale(sf);
    if (selectBank8Button_)      selectBank8Button_->setDisplayScale(sf);
    if (selectBank9Button_)      selectBank9Button_->setDisplayScale(sf);
}

void BankUtilityPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    if (bankUtilityModuleHeader_)
        bankUtilityModuleHeader_->setLook(tss::moduleHeaderLookFromSkin(skin));

    if (bankSelectorLabel_)
        bankSelectorLabel_->setLook(tss::labelLookFromSkin(skin));
    if (selectBank0Button_)
        selectBank0Button_->setLook(tss::buttonLookFromSkin(skin));
    if (selectBank1Button_)
        selectBank1Button_->setLook(tss::buttonLookFromSkin(skin));
    if (selectBank2Button_)
        selectBank2Button_->setLook(tss::buttonLookFromSkin(skin));
    if (selectBank3Button_)
        selectBank3Button_->setLook(tss::buttonLookFromSkin(skin));
    if (selectBank4Button_)
        selectBank4Button_->setLook(tss::buttonLookFromSkin(skin));
    if (unlockBankButton_)
        unlockBankButton_->setLook(tss::buttonLookFromSkin(skin));
    if (selectBank5Button_)
        selectBank5Button_->setLook(tss::buttonLookFromSkin(skin));
    if (selectBank6Button_)
        selectBank6Button_->setLook(tss::buttonLookFromSkin(skin));
    if (selectBank7Button_)
        selectBank7Button_->setLook(tss::buttonLookFromSkin(skin));
    if (selectBank8Button_)
        selectBank8Button_->setLook(tss::buttonLookFromSkin(skin));
    if (selectBank9Button_)
        selectBank9Button_->setLook(tss::buttonLookFromSkin(skin));
}

void BankUtilityPanel::setDisplayScale(float displayScale)
{
    if (juce::approximatelyEqual(displayScale_, displayScale))
        return;
    
    displayScale_ = displayScale;
    repaint();
}

void BankUtilityPanel::setupModuleHeader(tss::ISkin&, WidgetFactory& widgetFactory, const juce::String& moduleId)
{
    bankUtilityModuleHeader_ = std::make_unique<tss::ModuleHeader>(
        widgetFactory.getGroupDisplayName(moduleId),
        PluginDimensions::Widgets::Widths::ModuleHeader::kPatchManagerModule,
        PluginDimensions::Widgets::Heights::kModuleHeader,
        tss::ModuleHeader::ColourVariant::Blue);
    addAndMakeVisible(*bankUtilityModuleHeader_);
}

void BankUtilityPanel::setupBankSelectorLabel(tss::ISkin& skin)
{
    bankSelectorLabel_ = std::make_unique<tss::Label>(
        PluginDimensions::Widgets::Widths::Label::kPatchManagerBankSelector,
        PluginDimensions::Widgets::Heights::kLabel,
        PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kBankSelector);
    bankSelectorLabel_->setLook(tss::labelLookFromSkin(skin));
    addAndMakeVisible(*bankSelectorLabel_);
}

void BankUtilityPanel::setupSelectBankButtons(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    selectBank0Button_     = std::make_unique<tss::Button>(
        PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect,
        PluginDimensions::Widgets::Heights::kButton,
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank0).value_or(""));
    selectBank0Button_->setLook(tss::buttonLookFromSkin(skin));
    selectBank0Button_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank0,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*selectBank0Button_);

    selectBank1Button_     = std::make_unique<tss::Button>(
        PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect,
        PluginDimensions::Widgets::Heights::kButton,
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank1).value_or(""));
    selectBank1Button_->setLook(tss::buttonLookFromSkin(skin));
    selectBank1Button_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank1,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*selectBank1Button_);

    selectBank2Button_     = std::make_unique<tss::Button>(
        PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect,
        PluginDimensions::Widgets::Heights::kButton,
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank2).value_or(""));
    selectBank2Button_->setLook(tss::buttonLookFromSkin(skin));
    selectBank2Button_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank2,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*selectBank2Button_);

    selectBank3Button_     = std::make_unique<tss::Button>(
        PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect,
        PluginDimensions::Widgets::Heights::kButton,
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank3).value_or(""));
    selectBank3Button_->setLook(tss::buttonLookFromSkin(skin));
    selectBank3Button_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank3,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*selectBank3Button_);

    selectBank4Button_     = std::make_unique<tss::Button>(
        PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect,
        PluginDimensions::Widgets::Heights::kButton,
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank4).value_or(""));
    selectBank4Button_->setLook(tss::buttonLookFromSkin(skin));
    selectBank4Button_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank4,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*selectBank4Button_);

    unlockBankButton_ = std::make_unique<tss::Button>(
        PluginDimensions::Widgets::Widths::Button::kPatchManagerUnlockBank,
        PluginDimensions::Widgets::Heights::kButton,
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kUnlockBank).value_or(""));
    unlockBankButton_->setLook(tss::buttonLookFromSkin(skin));
    unlockBankButton_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kUnlockBank,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*unlockBankButton_);

    selectBank5Button_     = std::make_unique<tss::Button>(
        PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect,
        PluginDimensions::Widgets::Heights::kButton,
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank5).value_or(""));
    selectBank5Button_->setLook(tss::buttonLookFromSkin(skin));
    selectBank5Button_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank5,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*selectBank5Button_);

    selectBank6Button_     = std::make_unique<tss::Button>(
        PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect,
        PluginDimensions::Widgets::Heights::kButton,
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank6).value_or(""));
    selectBank6Button_->setLook(tss::buttonLookFromSkin(skin));
    selectBank6Button_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank6,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*selectBank6Button_);

    selectBank7Button_     = std::make_unique<tss::Button>(
        PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect,
        PluginDimensions::Widgets::Heights::kButton,
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank7).value_or(""));
    selectBank7Button_->setLook(tss::buttonLookFromSkin(skin));
    selectBank7Button_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank7,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*selectBank7Button_);

    selectBank8Button_     = std::make_unique<tss::Button>(
        PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect,
        PluginDimensions::Widgets::Heights::kButton,
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank8).value_or(""));
    selectBank8Button_->setLook(tss::buttonLookFromSkin(skin));
    selectBank8Button_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank8,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*selectBank8Button_);

    selectBank9Button_     = std::make_unique<tss::Button>(
        PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect,
        PluginDimensions::Widgets::Heights::kButton,
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank9).value_or(""));
    selectBank9Button_->setLook(tss::buttonLookFromSkin(skin));
    selectBank9Button_->onClick = [this]
    {
        apvts_.state.setProperty(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank9,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*selectBank9Button_);
}
