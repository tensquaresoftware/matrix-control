#include "Toggle.h"

#include "Shared/Definitions/PluginDimensions.h"

namespace tss
{
    Toggle::Toggle(int width, const juce::String& text)
        : juce::ToggleButton(text)
        , width_(width)
    {
        setOpaque(true);
        setSize(width_, PluginDimensions::Widgets::Heights::kToggle);
    }

    void Toggle::setLook(const ToggleLook& look)
    {
        look_ = look;
        repaint();
    }

    void Toggle::setScalingFactor(float scalingFactor)
    {
        if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
            return;

        scalingFactor_ = scalingFactor;
        repaint();
    }

    void Toggle::paintButton(juce::Graphics& g, bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
    {
        const auto bounds = getLocalBounds().toFloat();
        const bool isOn = getToggleState();
        const float borderThickness = std::max(1.0f, static_cast<float>(kBorderThickness_) * scalingFactor_);

        g.setColour(isOn ? look_.backgroundOn : look_.backgroundOff);
        g.fillRect(bounds);

        g.setColour(look_.border);
        g.drawRect(bounds, borderThickness);

        const auto buttonText = getButtonText();
        if (!buttonText.isEmpty())
        {
            g.setColour(isOn ? look_.textOn : look_.textOff);
            g.setFont(look_.font.withHeight(look_.font.getHeight() * scalingFactor_));
            g.drawText(buttonText, bounds, juce::Justification::centred, false);
        }
    }
}
