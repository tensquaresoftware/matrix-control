#include "HorizontalSeparator.h"

namespace tss
{
    HorizontalSeparator::HorizontalSeparator(int width, int height, const HorizontalSeparatorLook& look)
        : look_(look)
        , width_(width)
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

    void HorizontalSeparator::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;
        
        uiScale_ = uiScale;
        repaint();
    }

    void HorizontalSeparator::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        const float h = juce::jmax(1.0f, bounds.getHeight());
        const float lineY = bounds.getY() + h * 0.5f;
        const float lineThickness = std::max(1.0f, static_cast<float>(kLineThickness_) * uiScale_);
        
        g.setColour(look_.line);
        g.drawLine(bounds.getX(), lineY, bounds.getRight(), lineY, lineThickness);
    }
}
