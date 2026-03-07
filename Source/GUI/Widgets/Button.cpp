#include "Button.h"

namespace tss
{
    Button::Button(int width, int height, const juce::String& text)
        : juce::Button(text)
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

    void Button::setScalingFactor(float scalingFactor)
    {
        if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
            return;
        
        scalingFactor_ = scalingFactor;
        repaint();
    }

    void Button::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto buttonText = getButtonText();
        const bool enabled = isEnabled();
        const float borderThickness = std::max(1.0f, static_cast<float>(kBorderThickness_) * scalingFactor_);

        g.setColour(getBackgroundColour(enabled, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown));
        g.fillRect(bounds);

        g.setColour(getBorderColour(enabled));
        g.drawRect(bounds, borderThickness);

        if (! buttonText.isEmpty())
        {
            g.setColour(getTextColour(enabled, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown));
            g.setFont(look_.font.withHeight(kFontSize_ * scalingFactor_));
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
