#include "Toggle.h"

#include "GUI/Layout/ScaledDrawing.h"

namespace TSS
{
    Toggle::Toggle(int width, int height, const ToggleLook& look, const juce::String& text)
        : juce::ToggleButton(text)
        , look_(look)
        , width_(width)
        , height_(height)
    {
        setOpaque(true);
        setSize(width_, height_);
    }

    void Toggle::setLook(const ToggleLook& look)
    {
        look_ = look;
        repaint();
    }

    void Toggle::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;

        uiScale_ = uiScale;
        repaint();
    }

    void Toggle::paintButton(juce::Graphics& g, bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
    {
        const auto bounds = getLocalBounds().toFloat();
        const bool isOn = getToggleState();
        const bool enabled = isEnabled();
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            static_cast<float>(kBorderThickness_),
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);

        if (! enabled)
        {
            g.setColour(look_.backgroundDisabled);
            g.fillRect(bounds);

            g.setColour(look_.border);
            g.drawRect(bounds, borderThickness);

            const auto buttonText = getButtonText();
            if (! buttonText.isEmpty())
            {
                g.setColour(look_.textDisabled);
                g.setFont(look_.font.withHeight(look_.font.getHeight() * uiScale_));
                g.drawText(buttonText, bounds, juce::Justification::centred, false);
            }

            return;
        }

        g.setColour(isOn ? look_.backgroundOn : look_.backgroundOff);
        g.fillRect(bounds);

        g.setColour(look_.border);
        g.drawRect(bounds, borderThickness);

        const auto buttonText = getButtonText();
        if (!buttonText.isEmpty())
        {
            g.setColour(isOn ? look_.textOn : look_.textOff);
            g.setFont(look_.font.withHeight(look_.font.getHeight() * uiScale_));
            g.drawText(buttonText, bounds, juce::Justification::centred, false);
        }
    }
}
