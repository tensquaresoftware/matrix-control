#include "BankUtilityPanel.h"

#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Button.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "GUI/Factories/WidgetFactory.h"
#include <juce_core/juce_core.h>


namespace
{
    void setFooterInfoMessage(juce::AudioProcessorValueTreeState& apvts, const juce::String& message)
    {
        apvts.state.setProperty("uiMessageText", message, nullptr);
        apvts.state.setProperty("uiMessageSeverity", "info", nullptr);
    }
}

BankUtilityPanel::BankUtilityPanel(TSS::ISkin& skin, const BankUtilityPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : dims_(dims)
    , skin_(&skin)
    , apvts_(apvts)
{
    setOpaque(false);
    setupModuleHeader(skin, widgetFactory, PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId);
    setupBankSelectorLabel(skin);
    setupSelectBankButtons(skin, widgetFactory);

    normalBankLook_ = TSS::buttonLookFromSkin(skin);
    apvts_.state.addListener(this);
    refreshDeviceGating();
    refreshSelectedBankHighlight();

    setSize(dims_.width, dims_.height);
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
        || propertyName == "deviceDetected")
    {
        refreshDeviceGating();
    }
    else if (propertyName == PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kSelectedBank)
    {
        refreshSelectedBankHighlight();
    }
}

void BankUtilityPanel::refreshDeviceGating()
{
    const bool deviceDetected = static_cast<bool>(apvts_.state.getProperty("deviceDetected"));
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts_.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    const auto limits = Core::DeviceMemoryLimits::resolve(deviceType);

    const bool shouldGray = deviceDetected && !limits.hasBankConcept();
    setBankUtilityGrayed(shouldGray);
}

void BankUtilityPanel::setBankUtilityGrayed(bool grayed)
{
    bankUtilityGrayed_ = grayed;
    const float alpha = grayed ? 0.5f : 1.0f;

    styleBankButton(unlockBankButton_.get(), grayed);
    styleBankButton(selectBank0Button_.get(), grayed);
    styleBankButton(selectBank1Button_.get(), grayed);
    styleBankButton(selectBank2Button_.get(), grayed);
    styleBankButton(selectBank3Button_.get(), grayed);
    styleBankButton(selectBank4Button_.get(), grayed);
    styleBankButton(selectBank5Button_.get(), grayed);
    styleBankButton(selectBank6Button_.get(), grayed);
    styleBankButton(selectBank7Button_.get(), grayed);
    styleBankButton(selectBank8Button_.get(), grayed);
    styleBankButton(selectBank9Button_.get(), grayed);

    if (bankSelectorLabel_)
    {
        bankSelectorLabel_->setAlpha(alpha);
        bankSelectorLabel_->setInterceptsMouseClicks(!grayed, !grayed);
    }

    if (bankUtilityModuleHeader_)
    {
        bankUtilityModuleHeader_->setAlpha(alpha);
        bankUtilityModuleHeader_->setInterceptsMouseClicks(!grayed, !grayed);
    }

    refreshSelectedBankHighlight();
    repaint();
}

void BankUtilityPanel::refreshSelectedBankHighlight()
{
    const int selected = static_cast<int>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kSelectedBank,
        0));

    TSS::Button* const buttons[] = {
        selectBank0Button_.get(),
        selectBank1Button_.get(),
        selectBank2Button_.get(),
        selectBank3Button_.get(),
        selectBank4Button_.get(),
        selectBank5Button_.get(),
        selectBank6Button_.get(),
        selectBank7Button_.get(),
        selectBank8Button_.get(),
        selectBank9Button_.get(),
    };

    for (int index = 0; index < 10; ++index)
    {
        if (auto* button = buttons[index])
            button->setLook(normalBankLook_);
    }

    if (!bankUtilityGrayed_ && selected >= 0 && selected < 10)
    {
        if (auto* button = buttons[selected])
        {
            auto accentLook = normalBankLook_;
            accentLook.textOff = juce::Colour(ColourChart::kRed);
            button->setLook(accentLook);
        }
    }
}

void BankUtilityPanel::styleBankButton(TSS::Button* button, bool grayed)
{
    if (button == nullptr)
        return;

    button->setEnabled(true);
    button->setAlpha(grayed ? 0.5f : 1.0f);
}

void BankUtilityPanel::showMatrix1000OnlyFooterMessage()
{
    setFooterInfoMessage(
        apvts_,
        PluginDisplayNames::PatchManagerSection::BankUtilityModule::kMatrix1000OnlyFooterMessage);
}

void BankUtilityPanel::mouseDown(const juce::MouseEvent& event)
{
    if (!bankUtilityGrayed_)
    {
        juce::Component::mouseDown(event);
        return;
    }

    showMatrix1000OnlyFooterMessage();
}

void BankUtilityPanel::resized()
{
    const float sf = uiScale_;
    const float rowGapDesign = static_cast<float>(dims_.layout.interControlGap);

    const int moduleHeaderHeight  = juce::roundToInt(static_cast<float>(dims_.moduleHeader.height) * sf);
    const int moduleHeaderWidth   = juce::roundToInt(static_cast<float>(dims_.moduleHeader.patchManagerTitleBandWidth) * sf);
    const int labelWidth          = juce::roundToInt(static_cast<float>(dims_.bankSelectorLabel.patchManagerSelectBankWidth) * sf);
    const int labelHeight         = juce::roundToInt(static_cast<float>(dims_.bankSelectorLabel.height) * sf);
    const int buttonWidth         = juce::roundToInt(static_cast<float>(dims_.buttons.patchManagerBankSelectWidth) * sf);
    const int buttonHeight        = juce::roundToInt(static_cast<float>(dims_.buttons.height) * sf);
    const int lockButtonWidth   = juce::roundToInt(static_cast<float>(dims_.buttons.patchManagerUnlockBankWidth) * sf);

    if (auto* header = bankUtilityModuleHeader_.get())
        header->setBounds(0, 0, moduleHeaderWidth, moduleHeaderHeight);

    // Row 1 Y — directly under ModuleHeader (Recipes::BankUtilityModule::kHeight = 76 @ 100 %)
    const int row1Y = juce::roundToInt(static_cast<float>(dims_.moduleHeader.height) * sf);

    const int rowH = juce::roundToInt(static_cast<float>(dims_.layout.contentRowHeight) * sf);
    const int shortControlY = row1Y + (rowH - labelHeight) / 2;

    if (auto* label = bankSelectorLabel_.get())
        label->setBounds(0, shortControlY, labelWidth, labelHeight);

    // Row 1 X positions: each computed independently from float origin
    const float row1OriginX = static_cast<float>(dims_.bankSelectorLabel.patchManagerSelectBankWidth) * sf
        + rowGapDesign * sf;
    const float bankButtonStep = static_cast<float>(dims_.buttons.patchManagerBankSelectWidth) * sf
        + rowGapDesign * sf;

    auto setBankButtonBounds = [&](TSS::Button* btn, int index, int y)
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
    const int row2Y = juce::roundToInt(
        (static_cast<float>(dims_.moduleHeader.height + dims_.buttons.height) + rowGapDesign) * sf);

    if (auto* button = unlockBankButton_.get())
        button->setBounds(0, row2Y, lockButtonWidth, buttonHeight);

    // Row 2 X positions: banks 5-9, after unlock button
    const float row2OriginX = static_cast<float>(dims_.buttons.patchManagerUnlockBankWidth) * sf
        + rowGapDesign * sf;

    auto setBankButtonBoundsRow2 = [&](TSS::Button* btn, int index, int y)
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

    if (bankSelectorLabel_)      bankSelectorLabel_->setUiScale(sf);
    if (bankUtilityModuleHeader_) bankUtilityModuleHeader_->setUiScale(sf);
    if (unlockBankButton_)       unlockBankButton_->setUiScale(sf);
    if (selectBank0Button_)      selectBank0Button_->setUiScale(sf);
    if (selectBank1Button_)      selectBank1Button_->setUiScale(sf);
    if (selectBank2Button_)      selectBank2Button_->setUiScale(sf);
    if (selectBank3Button_)      selectBank3Button_->setUiScale(sf);
    if (selectBank4Button_)      selectBank4Button_->setUiScale(sf);
    if (selectBank5Button_)      selectBank5Button_->setUiScale(sf);
    if (selectBank6Button_)      selectBank6Button_->setUiScale(sf);
    if (selectBank7Button_)      selectBank7Button_->setUiScale(sf);
    if (selectBank8Button_)      selectBank8Button_->setUiScale(sf);
    if (selectBank9Button_)      selectBank9Button_->setUiScale(sf);
}

void BankUtilityPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    normalBankLook_ = TSS::buttonLookFromSkin(skin);

    if (bankUtilityModuleHeader_)
        bankUtilityModuleHeader_->setLook(TSS::moduleHeaderLookFromSkin(skin));

    if (bankSelectorLabel_)
        bankSelectorLabel_->setLook(TSS::labelLookFromSkin(skin));
    if (selectBank0Button_)
        selectBank0Button_->setLook(normalBankLook_);
    if (selectBank1Button_)
        selectBank1Button_->setLook(normalBankLook_);
    if (selectBank2Button_)
        selectBank2Button_->setLook(normalBankLook_);
    if (selectBank3Button_)
        selectBank3Button_->setLook(normalBankLook_);
    if (selectBank4Button_)
        selectBank4Button_->setLook(normalBankLook_);
    if (unlockBankButton_)
        unlockBankButton_->setLook(normalBankLook_);
    if (selectBank5Button_)
        selectBank5Button_->setLook(normalBankLook_);
    if (selectBank6Button_)
        selectBank6Button_->setLook(normalBankLook_);
    if (selectBank7Button_)
        selectBank7Button_->setLook(normalBankLook_);
    if (selectBank8Button_)
        selectBank8Button_->setLook(normalBankLook_);
    if (selectBank9Button_)
        selectBank9Button_->setLook(normalBankLook_);

    refreshSelectedBankHighlight();
}

void BankUtilityPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    repaint();
}

void BankUtilityPanel::setupModuleHeader(TSS::ISkin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId)
{
    bankUtilityModuleHeader_ = std::make_unique<TSS::ModuleHeader>(
        dims_.moduleHeader.patchManagerTitleBandWidth,
        dims_.moduleHeader.height,
        TSS::moduleHeaderLookFromSkin(skin),
        TSS::ModuleHeader::ColourVariant::Blue,
        widgetFactory.getGroupDisplayName(moduleId),
        dims_.moduleHeader);
    addAndMakeVisible(*bankUtilityModuleHeader_);
}

void BankUtilityPanel::setupBankSelectorLabel(TSS::ISkin& skin)
{
    bankSelectorLabel_ = std::make_unique<TSS::Label>(
        dims_.bankSelectorLabel.patchManagerSelectBankWidth,
        dims_.bankSelectorLabel.height,
        TSS::labelLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kBankSelector,
        TSS::LabelStyle::Centered);
    addAndMakeVisible(*bankSelectorLabel_);
}

void BankUtilityPanel::setupSelectBankButtons(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    const auto makeBankAction = [this](const char* propertyId)
    {
        return [this, propertyId]
        {
            if (bankUtilityGrayed_)
            {
                showMatrix1000OnlyFooterMessage();
                return;
            }

            apvts_.state.setProperty(propertyId,
                                    juce::Time::getCurrentTime().toMilliseconds(),
                                    nullptr);
        };
    };

    selectBank0Button_     = std::make_unique<TSS::Button>(
        dims_.buttons.patchManagerBankSelectWidth,
        dims_.buttons.height,
        TSS::buttonLookFromSkin(skin),
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank0).value_or(""));
    selectBank0Button_->onClick = makeBankAction(
        PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank0);
    addAndMakeVisible(*selectBank0Button_);

    selectBank1Button_     = std::make_unique<TSS::Button>(
        dims_.buttons.patchManagerBankSelectWidth,
        dims_.buttons.height,
        TSS::buttonLookFromSkin(skin),
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank1).value_or(""));
    selectBank1Button_->onClick = makeBankAction(
        PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank1);
    addAndMakeVisible(*selectBank1Button_);

    selectBank2Button_     = std::make_unique<TSS::Button>(
        dims_.buttons.patchManagerBankSelectWidth,
        dims_.buttons.height,
        TSS::buttonLookFromSkin(skin),
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank2).value_or(""));
    selectBank2Button_->onClick = makeBankAction(
        PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank2);
    addAndMakeVisible(*selectBank2Button_);

    selectBank3Button_     = std::make_unique<TSS::Button>(
        dims_.buttons.patchManagerBankSelectWidth,
        dims_.buttons.height,
        TSS::buttonLookFromSkin(skin),
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank3).value_or(""));
    selectBank3Button_->onClick = makeBankAction(
        PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank3);
    addAndMakeVisible(*selectBank3Button_);

    selectBank4Button_     = std::make_unique<TSS::Button>(
        dims_.buttons.patchManagerBankSelectWidth,
        dims_.buttons.height,
        TSS::buttonLookFromSkin(skin),
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank4).value_or(""));
    selectBank4Button_->onClick = makeBankAction(
        PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank4);
    addAndMakeVisible(*selectBank4Button_);

    unlockBankButton_ = std::make_unique<TSS::Button>(
        dims_.buttons.patchManagerUnlockBankWidth,
        dims_.buttons.height,
        TSS::buttonLookFromSkin(skin),
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kUnlockBank).value_or(""));
    unlockBankButton_->onClick = [this]
    {
        if (bankUtilityGrayed_)
        {
            showMatrix1000OnlyFooterMessage();
            return;
        }

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kUnlockBank,
            juce::Time::getCurrentTime().toMilliseconds(),
            nullptr);
        setFooterInfoMessage(
            apvts_,
            PluginDisplayNames::PatchManagerSection::BankUtilityModule::kUnlockBankFooterMessage);
    };
    addAndMakeVisible(*unlockBankButton_);

    selectBank5Button_     = std::make_unique<TSS::Button>(
        dims_.buttons.patchManagerBankSelectWidth,
        dims_.buttons.height,
        TSS::buttonLookFromSkin(skin),
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank5).value_or(""));
    selectBank5Button_->onClick = makeBankAction(
        PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank5);
    addAndMakeVisible(*selectBank5Button_);

    selectBank6Button_     = std::make_unique<TSS::Button>(
        dims_.buttons.patchManagerBankSelectWidth,
        dims_.buttons.height,
        TSS::buttonLookFromSkin(skin),
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank6).value_or(""));
    selectBank6Button_->onClick = makeBankAction(
        PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank6);
    addAndMakeVisible(*selectBank6Button_);

    selectBank7Button_     = std::make_unique<TSS::Button>(
        dims_.buttons.patchManagerBankSelectWidth,
        dims_.buttons.height,
        TSS::buttonLookFromSkin(skin),
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank7).value_or(""));
    selectBank7Button_->onClick = makeBankAction(
        PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank7);
    addAndMakeVisible(*selectBank7Button_);

    selectBank8Button_     = std::make_unique<TSS::Button>(
        dims_.buttons.patchManagerBankSelectWidth,
        dims_.buttons.height,
        TSS::buttonLookFromSkin(skin),
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank8).value_or(""));
    selectBank8Button_->onClick = makeBankAction(
        PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank8);
    addAndMakeVisible(*selectBank8Button_);

    selectBank9Button_     = std::make_unique<TSS::Button>(
        dims_.buttons.patchManagerBankSelectWidth,
        dims_.buttons.height,
        TSS::buttonLookFromSkin(skin),
        widgetFactory.getStandaloneWidgetDisplayName(PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank9).value_or(""));
    selectBank9Button_->onClick = makeBankAction(
        PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank9);
    addAndMakeVisible(*selectBank9Button_);
}
