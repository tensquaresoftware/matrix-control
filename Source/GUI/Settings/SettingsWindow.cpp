#include "SettingsWindow.h"

#include "SettingsPanel.h"
#include "SettingsShellMetrics.h"
#include "SettingsTabRail.h"
#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinValues.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    constexpr juce::uint32 kCloseCrossColour = 0xff9A131D;

    juce::Path makeCloseCrossShape()
    {
        juce::Path shape;
        constexpr float crossThickness = 0.15f;
        shape.addLineSegment({ 0.0f, 0.0f, 1.0f, 1.0f }, crossThickness);
        shape.addLineSegment({ 1.0f, 0.0f, 0.0f, 1.0f }, crossThickness);
        return shape;
    }
}

SettingsCloseButton::SettingsCloseButton()
    : juce::Button("close")
    , crossShape_(makeCloseCrossShape())
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void SettingsCloseButton::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    repaint();
}

void SettingsCloseButton::setUiScale(float uiScale)
{
    uiScale_ = uiScale;
    repaint();
}

void SettingsCloseButton::paintButton(juce::Graphics& g,
                                      bool shouldDrawButtonAsHighlighted,
                                      bool shouldDrawButtonAsDown)
{
    const auto titleBandColour = juce::Colour(DialogMatrixHelpers::kModalTitleBandColour);

    const auto crossColour = juce::Colour(kCloseCrossColour);
    g.setColour((! isEnabled() || shouldDrawButtonAsDown) ? crossColour.withAlpha(0.6f) : crossColour);

    if (shouldDrawButtonAsHighlighted)
    {
        g.fillAll(crossColour);
        g.setColour(titleBandColour);
    }

    const auto reducedRect = juce::Justification(juce::Justification::centred)
                                 .appliedToRectangle(juce::Rectangle<int>(getHeight(), getHeight()), getLocalBounds())
                                 .toFloat()
                                 .reduced(static_cast<float>(getHeight()) * 0.3f);

    g.fillPath(crossShape_, crossShape_.getTransformToScaleToFit(reducedRect, true));
}

SettingsWindow::SettingsWindow(TSS::ISkin& skin,
                               bool isPluginMode,
                               std::function<void(SettingsPanel&)> onPanelReady,
                               std::function<void()> onCloseRequested)
    : onCloseRequested_(std::move(onCloseRequested))
    , skin_(&skin)
    , isPluginMode_(isPluginMode)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    closeButton_.onClick = [this]
    {
        if (onCloseRequested_)
            onCloseRequested_();
    };
    closeButton_.setSkin(skin);
    addAndMakeVisible(closeButton_);

    tabRail_ = std::make_unique<SettingsTabRail>(skin, isPluginMode_, [this](int tabId)
    {
        settingsPanel_->setActiveTab(tabId);

        if (onTabChanged_)
            onTabChanged_(tabId);
    });
    addAndMakeVisible(*tabRail_);

    settingsPanel_ = std::make_unique<SettingsPanel>(skin, isPluginMode_);
    addAndMakeVisible(*settingsPanel_);

    if (onPanelReady)
        onPanelReady(*settingsPanel_);
}

SettingsWindow::~SettingsWindow() = default;

void SettingsWindow::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    closeButton_.setSkin(skin);
    tabRail_->setSkin(skin);
    settingsPanel_->setSkin(skin);
    repaint();
}

void SettingsWindow::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    closeButton_.setUiScale(uiScale);
    tabRail_->setUiScale(uiScale);
    settingsPanel_->setUiScale(uiScale);
    resized();
    repaint();
}

void SettingsWindow::setOnTabChanged(std::function<void(int)> onTabChanged)
{
    onTabChanged_ = std::move(onTabChanged);
}

void SettingsWindow::setActiveTab(int tabId)
{
    const int normalized = PluginIDs::Settings::LastTab::normalize(tabId, isPluginMode_);
    tabRail_->setSelectedTab(normalized);
    settingsPanel_->setActiveTab(normalized);
}

int SettingsWindow::getActiveTab() const
{
    return settingsPanel_->getActiveTab();
}

int SettingsWindow::getBorderThickness() const
{
    return TSS::ScaledLayout::scaledInt(static_cast<float>(DialogMatrixHelpers::kBorderThickness), uiScale_);
}

juce::Rectangle<int> SettingsWindow::getDialogBounds() const
{
    const int border = getBorderThickness();
    const int titleBarHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(DialogMatrixHelpers::kTitleBarHeight), uiScale_);
    const int dialogWidth = SettingsShellMetrics::scaledBodyWidth(uiScale_) + border * 2;
    const int dialogHeight = SettingsShellMetrics::scaledBodyHeight(uiScale_, isPluginMode_)
                             + titleBarHeight + border * 2;

    return SettingsShellMetrics::centredClampedDialog(getLocalBounds(), dialogWidth, dialogHeight);
}

void SettingsWindow::paint(juce::Graphics& g)
{
    DialogMatrixHelpers::paintMatrixOverlayChrome({
        .g = g,
        .skin = *skin_,
        .dialogBounds = getDialogBounds(),
        .borderThickness = getBorderThickness(),
        .titleBarHeight = TSS::ScaledLayout::scaledInt(
            static_cast<float>(DialogMatrixHelpers::kTitleBarHeight), uiScale_),
        .title = PluginDisplayNames::Settings::kWindowTitle,
        .uiScale = uiScale_ });

    auto body = getDialogBounds().reduced(getBorderThickness());
    const int titleBarHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(DialogMatrixHelpers::kTitleBarHeight), uiScale_);
    body.removeFromTop(titleBarHeight);

    g.setColour(skin_->getColour(TSS::SkinColourId::kBodyPanelBackground));
    g.fillRect(body);

    const int thickness = SettingsShellMetrics::ruleStrokeThickness(uiScale_);
    const int railWidth = juce::jmin(SettingsShellMetrics::scaledRailWidth(uiScale_), body.getWidth());
    const int gutter = juce::jmin(SettingsShellMetrics::scaledRuleGutter(uiScale_),
                                  juce::jmax(0, body.getWidth() - railWidth));
    const int ruleX = SettingsShellMetrics::ruleFillX(body.getX(), railWidth, gutter, uiScale_);

    if (thickness > 0 && ruleX >= body.getX() && ruleX + thickness <= body.getRight())
    {
        g.setColour(juce::Colour(DialogMatrixHelpers::kDialogBorderColour));
        g.fillRect(ruleX, body.getY(), thickness, body.getHeight());
    }
}

void SettingsWindow::resized()
{
    auto inner = getDialogBounds().reduced(getBorderThickness());
    const int titleBarHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(DialogMatrixHelpers::kTitleBarHeight), uiScale_);
    const int closeButtonWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(titleBarHeight) * 1.2f, 1.0f);

    auto titleBar = inner.removeFromTop(titleBarHeight);
    closeButton_.setBounds(titleBar.removeFromRight(closeButtonWidth));

    auto body = inner;
    const int railWidth = juce::jmin(SettingsShellMetrics::scaledRailWidth(uiScale_), body.getWidth());
    const int gutter = juce::jmin(SettingsShellMetrics::scaledRuleGutter(uiScale_),
                                  juce::jmax(0, body.getWidth() - railWidth));
    tabRail_->setBounds(body.removeFromLeft(railWidth));
    body.removeFromLeft(gutter);
    settingsPanel_->setBounds(body);
}

void SettingsWindow::mouseDown(const juce::MouseEvent& e)
{
    if (! getDialogBounds().contains(e.getPosition()) && onCloseRequested_)
        onCloseRequested_();
}

bool SettingsWindow::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        if (onCloseRequested_)
            onCloseRequested_();
        return true;
    }

    return Component::keyPressed(key);
}
