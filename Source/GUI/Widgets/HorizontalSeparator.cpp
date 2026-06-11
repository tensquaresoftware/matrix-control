#include "HorizontalSeparator.h"

#include "GUI/Layout/ScaledDrawing.h"

namespace TSS
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
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float lineThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            static_cast<float>(kLineThickness_),
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);

        auto line = bounds;
        line.setHeight(lineThickness);
        line.setY(bounds.getCentreY() - lineThickness * 0.5f);

        g.setColour(look_.line);
        g.fillRect(line);
    }
}
