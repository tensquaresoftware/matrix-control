#include "RampPortamentoPanel.h"

#include "GUI/Helpers/GrayedControlHelper.h"
#include "GUI/Helpers/StrigUnisonGateHelper.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/ParameterCell.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/Helpers/UnisonKeyboardModePolicy.h"

namespace
{
    constexpr int kMasterOverrideBadgeDesignSize = 12;
    constexpr int kMasterOverrideBadgeGap = 4;
    constexpr const char* kMasterOverrideBadgeLabel = "M";
}

class RampPortamentoPanel::MasterOverrideBadge : public juce::Component
{
public:
    MasterOverrideBadge(TSS::ISkin& skin, std::function<void()> showFooter)
        : skin_(&skin)
        , showFooter_(std::move(showFooter))
    {
        setOpaque(false);
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }

    void setSkin(TSS::ISkin& skin)
    {
        skin_ = &skin;
        repaint();
    }

    void setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;

        uiScale_ = uiScale;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        if (skin_ == nullptr)
            return;

        const auto fill = skin_->getColour(TSS::SkinColourId::kSectionHeaderLineOrange);
        const auto textColour = skin_->getColour(TSS::SkinColourId::kBodyPanelBackground);
        auto font = skin_->getBaseFontBold().withHeight(
            static_cast<float>(kMasterOverrideBadgeDesignSize) * uiScale_ * 0.75f);

        g.setColour(fill);
        g.fillRect(getLocalBounds());

        g.setColour(textColour);
        g.setFont(font);
        g.drawText(kMasterOverrideBadgeLabel, getLocalBounds(), juce::Justification::centred, false);
    }

    void mouseDown(const juce::MouseEvent&) override
    {
        if (showFooter_)
            showFooter_();
    }

    void mouseEnter(const juce::MouseEvent&) override
    {
        if (showFooter_)
            showFooter_();
    }

private:
    TSS::ISkin* skin_ = nullptr;
    std::function<void()> showFooter_;
    float uiScale_ = 1.0f;
};

ModulePanelLayout RampPortamentoPanel::createLayout()
{
    return makePatchEditInitOnlyModuleLayout(
        PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
        PluginIDs::PatchEditSection::RampPortamentoModule::StandaloneWidgets::kInit,
        {
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Rate,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Trigger,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Rate,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Trigger,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoRate,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoModByVelocity,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoMode,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoLegato,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode,
            ""
        });
}

RampPortamentoPanel::RampPortamentoPanel(const Config& config)
    : BaseModulePanel(BaseModulePanel::Config{
          .skin = config.skin,
          .widgetFactory = config.widgetFactory,
          .apvts = config.apvts,
          .layout = createLayout(),
          .width = config.width,
          .height = config.height,
          .moduleHeaderDims = config.moduleHeaderDims,
          .parameterCellDims = config.parameterCellDims})
{
    masterOverrideBadge_ = std::make_unique<MasterOverrideBadge>(
        config.skin,
        [this] { showMasterOverrideFooter(); });
    addChildComponent(*masterOverrideBadge_);

    apvts_.addParameterListener(
        PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable,
        this);
    apvts_.state.addListener(this);

    keyboardModeListener_ = std::make_unique<TSS::StrigUnisonGateHelper::KeyboardModeChangeListener>(
        apvts_,
        [this](bool leftUnison)
        {
            refreshLegatoPortaGraying();
            refreshStrigGates(leftUnison);
            refreshMasterOverrideBadge();
        });

    refreshLegatoPortaGraying();
    refreshStrigGates(false);
    refreshMasterOverrideBadge();
}

RampPortamentoPanel::~RampPortamentoPanel()
{
    keyboardModeListener_.reset();
    apvts_.state.removeListener(this);
    apvts_.removeParameterListener(
        PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable,
        this);
}

void RampPortamentoPanel::resized()
{
    BaseModulePanel::resized();
    layoutMasterOverrideBadge();
}

void RampPortamentoPanel::parameterChanged(const juce::String& parameterID, float)
{
    if (parameterID == PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable)
        refreshMasterOverrideBadge();
}

void RampPortamentoPanel::valueTreePropertyChanged(juce::ValueTree&,
                                                   const juce::Identifier& property)
{
    if (property == juce::Identifier(MatrixDeviceTypes::kApvtsPropertyName))
        refreshMasterOverrideBadge();
}

void RampPortamentoPanel::refreshLegatoPortaGraying()
{
    const auto* keyboardModeParam = apvts_.getParameter(
        PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode);
    const auto* choiceParam = dynamic_cast<const juce::AudioParameterChoice*>(keyboardModeParam);
    const int keyboardModeIndex = choiceParam != nullptr ? choiceParam->getIndex() : -1;
    legatoPortaGrayed_ = ! TSS::UnisonKeyboardModePolicy::isLegatoPortaEnabled(keyboardModeIndex);

    if (auto* legatoCell = getParameterCellAt(static_cast<size_t>(kLegatoPortaCellIndex)))
    {
        // Label and separator stay fully opaque — only the ComboBox uses disabled skin paint.
        legatoCell->setAlpha(1.0f);

        if (auto* combo = legatoCell->getComboBox())
            combo->setEnabled(! legatoPortaGrayed_);

        if (legatoPortaGrayed_)
        {
            TSS::GrayedControlHelper::setGrayedClickHandler(*legatoCell, true, [this]
            {
                TSS::GrayedControlHelper::setFooterInfoMessage(
                    apvts_,
                    PluginDisplayNames::PatchEditSection::RampPortamentoModule::kLegatoPortaUnisonBlockedFooter);
            });
        }
        else
        {
            TSS::GrayedControlHelper::clearGrayedClickHandler(*legatoCell);
        }
    }
}

void RampPortamentoPanel::refreshStrigGates(bool clearIfCurrentStrig)
{
    if (auto* ramp1Cell = getParameterCellAt(static_cast<size_t>(kRamp1TriggerCellIndex)))
    {
        TSS::StrigUnisonGateHelper::refreshTriggerParameter({
            .apvts = apvts_,
            .combo = ramp1Cell->getComboBox(),
            .triggerParameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Trigger,
            .strigChoiceIndex = kRampTriggerStrigIndex,
            .clearIfCurrentStrig = clearIfCurrentStrig});
    }

    if (auto* ramp2Cell = getParameterCellAt(static_cast<size_t>(kRamp2TriggerCellIndex)))
    {
        TSS::StrigUnisonGateHelper::refreshTriggerParameter({
            .apvts = apvts_,
            .combo = ramp2Cell->getComboBox(),
            .triggerParameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Trigger,
            .strigChoiceIndex = kRampTriggerStrigIndex,
            .clearIfCurrentStrig = clearIfCurrentStrig});
    }
}

void RampPortamentoPanel::showMasterOverrideFooter()
{
    TSS::GrayedControlHelper::setFooterInfoMessage(
        apvts_,
        PluginDisplayNames::PatchEditSection::RampPortamentoModule::kMasterUnisonOverrideFooter);
}

void RampPortamentoPanel::refreshMasterOverrideBadge()
{
    const auto deviceType = MatrixDeviceTypes::fromApvtsString(
        apvts_.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName).toString());

    const auto* masterUnisonParam = apvts_.getParameter(
        PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable);
    const auto* masterChoice = dynamic_cast<const juce::AudioParameterChoice*>(masterUnisonParam);
    const bool masterUnisonOn = masterChoice != nullptr && masterChoice->getIndex() == 1;

    const auto* keyboardModeParam = apvts_.getParameter(
        PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode);
    const auto* keyboardChoice = dynamic_cast<const juce::AudioParameterChoice*>(keyboardModeParam);
    const int keyboardModeIndex = keyboardChoice != nullptr ? keyboardChoice->getIndex() : -1;

    const bool showBadge = TSS::UnisonKeyboardModePolicy::shouldShowMasterOverrideBadge(
        deviceType, masterUnisonOn, keyboardModeIndex);

    if (masterOverrideBadge_ != nullptr)
    {
        masterOverrideBadge_->setVisible(showBadge);
        if (showBadge)
            layoutMasterOverrideBadge();
    }
}

void RampPortamentoPanel::layoutMasterOverrideBadge()
{
    if (masterOverrideBadge_ == nullptr || ! masterOverrideBadge_->isVisible())
        return;

    auto* keyboardCell = getParameterCellAt(static_cast<size_t>(kKeyboardModeCellIndex));
    if (keyboardCell == nullptr)
        return;

    if (skin_ == nullptr)
        return;

    masterOverrideBadge_->setUiScale(uiScale_);
    masterOverrideBadge_->setSkin(*skin_);

    const auto labelBoundsInCell = keyboardCell->getLabelBounds();
    const auto labelText = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode;

    auto labelFont = TSS::labelLookFromSkin(*skin_).font;
    labelFont = labelFont.withHeight(labelFont.getHeight() * uiScale_);
    const int textWidth = juce::roundToInt(juce::GlyphArrangement::getStringWidth(labelFont, labelText));

    const int badgeSize = TSS::ScaledLayout::scaledInt(
        static_cast<float>(kMasterOverrideBadgeDesignSize), uiScale_);
    const int gap = TSS::ScaledLayout::scaledInt(
        static_cast<float>(kMasterOverrideBadgeGap), uiScale_);

    const auto cellBounds = keyboardCell->getBounds();
    const int badgeX = cellBounds.getX() + labelBoundsInCell.getX() + textWidth + gap;
    const int badgeY = cellBounds.getY()
        + labelBoundsInCell.getY()
        + (labelBoundsInCell.getHeight() - badgeSize) / 2;

    masterOverrideBadge_->setBounds(badgeX, badgeY, badgeSize, badgeSize);
    masterOverrideBadge_->toFront(false);
}
