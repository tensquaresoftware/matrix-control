#include "VerticalSeparator.h"

namespace tss
{
    VerticalSeparator::VerticalSeparator(int width, int height)
        : width_(width)
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

    void VerticalSeparator::setDisplayScale(float displayScale)
    {
        if (juce::approximatelyEqual(displayScale_, displayScale))
            return;
        
        displayScale_ = displayScale;
        repaint();
    }

    void VerticalSeparator::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        const float lineWidth = std::max(1.0f, kLineWidth_ * displayScale_);
        const auto lineX = bounds.getCentreX() - lineWidth * 0.5f;

        auto line = bounds;
        line.removeFromTop(kTopPadding_ * displayScale_);
        line.removeFromBottom(kBottomPadding_ * displayScale_);
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
