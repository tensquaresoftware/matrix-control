#include "Button.h"

#include "GUI/Layout/ScaledDrawing.h"

namespace TSS
{
    Button::Button(int width, int height, const ButtonLook& look, const juce::String& text)
        : juce::Button(text)
        , look_(look)
        , width_(width)
        , height_(height)
    {
        setOpaque(true);
        setSize(width_, height_);
    }

    void Button::setLook(const ButtonLook& look)
    {
        look_ = look;
        repaint();
    }

    void Button::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;
        
        uiScale_ = uiScale;
        repaint();
    }

    void Button::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto buttonText = getButtonText();
        const bool enabled = isEnabled();
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            static_cast<float>(kBorderThickness_),
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kFloor);

        g.setColour(getBackgroundColour(enabled, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown));
        g.fillRect(bounds);

        g.setColour(getBorderColour(enabled));
        g.drawRect(bounds, borderThickness);

        if (! buttonText.isEmpty())
        {
            g.setColour(getTextColour(enabled, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown));
            g.setFont(look_.font.withHeight(look_.font.getHeight() * uiScale_));
            g.drawText(buttonText, bounds, juce::Justification::centred, false);
        }
    }

    juce::Colour Button::getBackgroundColour(bool enabled, bool isHighlighted, bool isDown) const
    {
        if (! enabled)
            return look_.backgroundOff;
        
        if (isDown)
            return look_.backgroundClicked;
        
        if (isHighlighted)
            return look_.backgroundHover;
        
        return getToggleState() ? look_.backgroundOn : look_.backgroundOff;
    }

    juce::Colour Button::getBorderColour(bool /* enabled */) const
    {
        return getToggleState() ? look_.borderOn : look_.borderOff;
    }

    juce::Colour Button::getTextColour(bool enabled, bool isHighlighted, bool isDown) const
    {
        if (! enabled)
            return look_.textOff;
        
        if (isDown)
            return look_.textClicked;
        
        if (isHighlighted)
            return look_.textHover;
        
        return getToggleState() ? look_.textOn : look_.textOff;
    }
}
