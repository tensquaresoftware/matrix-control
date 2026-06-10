#include "ActivityLed.h"

#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/Skin.h"

namespace TSS
{
    ActivityLed::ActivityLed(int width, int height)
        : width_(width)
        , height_(height)
    {
        setOpaque(false);
        setSize(width_, height_);
    }

    void ActivityLed::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr)
            return;

        const auto bounds = getLocalBounds().toFloat();
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            kBorderThicknessDesign_,
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kFloor);
        const float maxBorder = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.49f;
        const float safeBorder = juce::jmin(borderThickness, maxBorder);
        const auto innerBounds = bounds.reduced(safeBorder);

        g.setColour(look_.valueBarEnabled);
        g.fillRect(innerBounds);

        const float clampedLevel = juce::jlimit(0.0f, 1.0f, level_);

        if (clampedLevel > 0.0f)
        {
            const auto activeColour = look_.textEnabled.withAlpha(clampedLevel);
            g.setColour(activeColour);
            g.fillRect(innerBounds);
        }

        g.setColour(look_.trackEnabled);
        g.drawRect(bounds, borderThickness);
    }

    void ActivityLed::setSkin(ISkin& skin)
    {
        skin_ = &skin;
        look_ = sliderLookFromSkin(skin);
        repaint();
    }

    void ActivityLed::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;

        uiScale_ = uiScale;
        repaint();
    }

    void ActivityLed::setLevel(float normalisedLevel)
    {
        const float clamped = juce::jlimit(0.0f, 1.0f, normalisedLevel);

        if (juce::approximatelyEqual(level_, clamped))
            return;

        level_ = clamped;
        repaint();
    }
}
