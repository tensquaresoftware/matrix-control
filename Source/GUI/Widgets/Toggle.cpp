#include "Toggle.h"

#include "Shared/PluginDimensions.h"
#include "GUI/Skins/Skin.h"

namespace tss
{
    Toggle::Toggle(Skin& skin, int width, const juce::String& text)
        : juce::ToggleButton(text)
        , skin_(&skin)
        , width_(width)
    {
        setOpaque(true);
        setSize(width_, PluginDimensions::Widgets::Heights::kToggle);
    }

    void Toggle::setSkin(Skin& skin)
    {
        skin_ = &skin;
        repaint();
    }

    void Toggle::paintButton(juce::Graphics& g, bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
    {
        if (skin_ == nullptr)
            return;

        const auto bounds = getLocalBounds().toFloat();
        const bool isOn = getToggleState();

        g.setColour(skin_->getToggleBackgroundColour(isOn));
        g.fillRect(bounds);

        g.setColour(skin_->getToggleBorderColour());
        g.drawRect(bounds, static_cast<float>(kBorderThickness_));

        const auto buttonText = getButtonText();
        if (!buttonText.isEmpty())
        {
            g.setColour(skin_->getToggleTextColour(isOn));
            g.setFont(skin_->getBaseFont());
            g.drawText(buttonText, bounds, juce::Justification::centred, false);
        }
    }
}
