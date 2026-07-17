#include "AboutWindow.h"

#include "AboutPanel.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

namespace
{
    constexpr juce::uint32 kCloseCrossColour = 0xff9A131D;
    constexpr juce::uint32 kDialogBorderColour = 0xff5E5E5E;

    juce::Path makeCloseCrossShape()
    {
        juce::Path shape;
        constexpr float crossThickness = 0.15f;
        shape.addLineSegment({ 0.0f, 0.0f, 1.0f, 1.0f }, crossThickness);
        shape.addLineSegment({ 1.0f, 0.0f, 0.0f, 1.0f }, crossThickness);
        return shape;
    }
}

AboutCloseButton::AboutCloseButton()
    : juce::Button("close")
    , crossShape_(makeCloseCrossShape())
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void AboutCloseButton::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    repaint();
}

void AboutCloseButton::setUiScale(float uiScale)
{
    uiScale_ = uiScale;
    repaint();
}

void AboutCloseButton::paintButton(juce::Graphics& g,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown)
{
    const auto titleBarBackground = skin_ != nullptr
        ? skin_->getColour(SkinColourId::kHeaderPanelBackground)
        : juce::Colours::darkgrey;

    const auto crossColour = juce::Colour(kCloseCrossColour);
    g.setColour((! isEnabled() || shouldDrawButtonAsDown) ? crossColour.withAlpha(0.6f) : crossColour);

    if (shouldDrawButtonAsHighlighted)
    {
        g.fillAll(crossColour);
        g.setColour(titleBarBackground);
    }

    const auto reducedRect = juce::Justification(juce::Justification::centred)
                                 .appliedToRectangle(juce::Rectangle<int>(getHeight(), getHeight()), getLocalBounds())
                                 .toFloat()
                                 .reduced(static_cast<float>(getHeight()) * 0.3f);

    g.fillPath(crossShape_, crossShape_.getTransformToScaleToFit(reducedRect, true));
}

AboutWindow::AboutWindow(TSS::ISkin& skin, std::function<void()> onCloseRequested)
    : onCloseRequested_(std::move(onCloseRequested))
    , skin_(&skin)
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

    aboutPanel_ = std::make_unique<AboutPanel>(skin);
    aboutPanel_->setOnEscapePressed([this]
    {
        if (onCloseRequested_)
            onCloseRequested_();
    });
    addAndMakeVisible(*aboutPanel_);
}

AboutWindow::~AboutWindow() = default;

void AboutWindow::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    closeButton_.setSkin(skin);
    aboutPanel_->setSkin(skin);
    repaint();
}

void AboutWindow::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    closeButton_.setUiScale(uiScale);
    aboutPanel_->setUiScale(uiScale);
    resized();
    repaint();
}

int AboutWindow::getBorderThickness() const
{
    return juce::roundToInt(static_cast<float>(kBorderThickness_) * uiScale_);
}

juce::Rectangle<int> AboutWindow::getDialogBounds() const
{
    const int border = getBorderThickness();
    const int dialogWidth = juce::roundToInt(static_cast<float>(AboutPanel::kDesignWidth) * uiScale_) + border * 2;
    const int dialogHeight = juce::roundToInt(static_cast<float>(AboutPanel::kDesignHeight) * uiScale_)
                             + juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_)
                             + border * 2;

    return getLocalBounds().withSizeKeepingCentre(dialogWidth, dialogHeight);
}

void AboutWindow::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kBodyPanelBackground).withAlpha(0.85f));

    const auto dialogBounds = getDialogBounds();
    const int border = getBorderThickness();

    g.setColour(juce::Colour(kDialogBorderColour));
    g.fillRect(dialogBounds);

    auto inner = dialogBounds.reduced(border);
    const int titleBarHeight = juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_);
    auto titleBar = inner.removeFromTop(titleBarHeight);

    g.setColour(skin_->getColour(SkinColourId::kHeaderPanelBackground));
    g.fillRect(titleBar);
    g.fillRect(inner);

    g.setColour(skin_->getColour(SkinColourId::kDarkPanelText));
    g.setFont(skin_->getBaseFontBold().withHeight(skin_->getBaseFontBold().getHeight() * uiScale_));
    g.drawText(PluginDisplayNames::About::kWindowTitle,
               titleBar,
               juce::Justification::centred,
               false);
}

void AboutWindow::resized()
{
    auto inner = getDialogBounds().reduced(getBorderThickness());
    const int titleBarHeight = juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_);
    const int closeButtonWidth = juce::roundToInt(static_cast<float>(titleBarHeight) * 1.2f);

    auto titleBar = inner.removeFromTop(titleBarHeight);
    closeButton_.setBounds(titleBar.removeFromRight(closeButtonWidth));
    aboutPanel_->setBounds(inner);
}

void AboutWindow::mouseDown(const juce::MouseEvent& e)
{
    if (! getDialogBounds().contains(e.getPosition()) && onCloseRequested_)
        onCloseRequested_();
}

bool AboutWindow::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        if (onCloseRequested_)
            onCloseRequested_();
        return true;
    }

    return Component::keyPressed(key);
}
