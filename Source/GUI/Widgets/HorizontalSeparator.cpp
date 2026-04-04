#include "HorizontalSeparator.h"

namespace tss
{
    HorizontalSeparator::HorizontalSeparator(int width, int height)
        : width_(width)
        , height_(height)
    {
        setOpaque(false);
        setSize(width_, height_);
    }

    void HorizontalSeparator::setLook(const HorizontalSeparatorLook& look)
    {
        look_ = look;
        repaint();
    }

    void HorizontalSeparator::setScalingFactor(float scalingFactor)
    {
        if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
            return;
        
        scalingFactor_ = scalingFactor;
        repaint();
    }

    void HorizontalSeparator::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        const float h = juce::jmax(1.0f, bounds.getHeight());
        const float lineY = bounds.getY() + h * 0.5f;
        const float lineThickness = std::max(1.0f, static_cast<float>(kLineThickness_) * scalingFactor_);
        
        g.setColour(look_.line);
        g.drawLine(bounds.getX(), lineY, bounds.getRight(), lineY, lineThickness);
    }
}
