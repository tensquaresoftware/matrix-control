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

    void Toggle::paintButton(juce::Graphics& g, bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
    {
        const auto bounds = getLocalBounds().toFloat();
        const bool isOn = getToggleState();

        g.setColour(isOn ? look_.backgroundOn : look_.backgroundOff);
        g.fillRect(bounds);

        g.setColour(look_.border);
        g.drawRect(bounds, static_cast<float>(kBorderThickness_));

        const auto buttonText = getButtonText();
        if (!buttonText.isEmpty())
        {
            g.setColour(isOn ? look_.textOn : look_.textOff);
            g.setFont(look_.font);
            g.drawText(buttonText, bounds, juce::Justification::centred, false);
        }
    }
}
