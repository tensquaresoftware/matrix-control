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

    void VerticalSeparator::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto lineX = bounds.getCentreX() - kLineWidth_ * 0.5f;

        auto line = bounds;
        line.removeFromTop(kTopPadding_);
        line.removeFromBottom(kBottomPadding_);
        line.setX(lineX);
        line.setWidth(kLineWidth_);

        g.setColour(look_.line);
        g.fillRect(line);
    }

    void VerticalSeparator::resized()
    {
        repaint();
    }
}
