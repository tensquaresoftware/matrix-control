#include "VerticalSeparator.h"

namespace TSS
{
    VerticalSeparator::VerticalSeparator(int width, int height, const VerticalSeparatorLook& look)
        : look_(look)
        , width_(width)
        , height_(height)
    {
        setOpaque(false);
        setSize(width_, height_);
    }

    void VerticalSeparator::setLook(const VerticalSeparatorLook& look)
    {
        look_ = look;
        repaint();
    }

    void VerticalSeparator::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;
        
        uiScale_ = uiScale;
        repaint();
    }

    void VerticalSeparator::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        const float lineWidth = std::max(1.0f, kLineWidth_ * uiScale_);
        const auto lineX = bounds.getCentreX() - lineWidth * 0.5f;

        auto line = bounds;
        line.removeFromTop(kTopPadding_ * uiScale_);
        line.removeFromBottom(kBottomPadding_ * uiScale_);
        line.setX(lineX);
        line.setWidth(lineWidth);

        g.setColour(look_.line);
        g.fillRect(line);
    }

    void VerticalSeparator::resized()
    {
        repaint();
    }
}
