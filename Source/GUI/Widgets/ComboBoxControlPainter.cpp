#include "ComboBoxControlPainter.h"

#include "GUI/Layout/ScaledDrawing.h"

namespace TSS
{
    namespace
    {
        juce::Path createTrianglePath(float x, float y, float baseSize)
        {
            juce::Path path;
            const auto height = baseSize * ComboBoxControlMetrics::kTriangleHeightFactor;

            path.startNewSubPath(x, y);
            path.lineTo(x + baseSize, y);
            path.lineTo(x + baseSize * 0.5f, y + height);
            path.closeSubPath();

            return path;
        }

        juce::Colour textColourForStyle(ComboBoxControlStyle style, const ComboBoxLook& look, bool enabled)
        {
            if (style == ComboBoxControlStyle::ButtonLike)
                return enabled ? look.buttonLikeText : look.buttonLikeTextDisabled;

            return enabled ? look.textEnabled : look.textDisabled;
        }

        juce::Colour triangleColourForStyle(ComboBoxControlStyle style, const ComboBoxLook& look, bool enabled)
        {
            if (style == ComboBoxControlStyle::ButtonLike)
                return enabled ? look.buttonLikeTriangle : look.buttonLikeTriangleDisabled;

            return enabled ? look.triangleEnabled : look.triangleDisabled;
        }

        juce::Colour backgroundColourForStyle(ComboBoxControlStyle style, const ComboBoxLook& look, bool enabled)
        {
            if (style == ComboBoxControlStyle::ButtonLike)
                return enabled ? look.buttonLikeBackground : look.buttonLikeBackgroundDisabled;

            return enabled ? look.backgroundEnabled : look.backgroundDisabled;
        }

        juce::Colour borderColourForStyle(ComboBoxControlStyle style, const ComboBoxLook& look, bool enabled)
        {
            if (style == ComboBoxControlStyle::ButtonLike)
            {
                juce::ignoreUnused(enabled);
                // Disabled border matches the enabled ButtonLike border.
                return look.buttonLikeBorder;
            }

            return enabled ? look.borderEnabled : look.borderDisabled;
        }

        juce::Rectangle<float> calculateTextBounds(juce::Rectangle<float> bounds, float uiScale)
        {
            const float leftPad = static_cast<float>(ComboBoxControlMetrics::kLeftPadding) * uiScale;
            const float triangleSpace = static_cast<float>(ComboBoxControlMetrics::kTriangleBaseSize) * uiScale;
            const float rightPad = static_cast<float>(ComboBoxControlMetrics::kRightPadding) * uiScale;

            bounds.removeFromLeft(leftPad);
            bounds.removeFromRight(triangleSpace);
            bounds.removeFromRight(rightPad);
            return bounds;
        }
    }

    void ComboBoxControlPainter::paintClosedState(juce::Graphics& g,
                                                  const juce::Component& component,
                                                  juce::Rectangle<float> bounds,
                                                  ComboBoxControlStyle style,
                                                  const ComboBoxLook& look,
                                                  float uiScale,
                                                  const juce::String& text,
                                                  bool enabled,
                                                  bool hasFocus)
    {
        const auto backgroundBounds = bounds;
        g.setColour(backgroundColourForStyle(style, look, enabled));
        g.fillRect(backgroundBounds);

        const auto textBounds = calculateTextBounds(bounds, uiScale);
        g.setColour(textColourForStyle(style, look, enabled));
        g.setFont(look.font.withHeight(look.font.getHeight() * uiScale));
        g.drawText(text, textBounds, juce::Justification::centredLeft, false);

        g.setColour(triangleColourForStyle(style, look, enabled));
        const float triangleBaseSize = static_cast<float>(ComboBoxControlMetrics::kTriangleBaseSize) * uiScale;
        const float triangleHeight = triangleBaseSize * ComboBoxControlMetrics::kTriangleHeightFactor;
        const float rightPad = static_cast<float>(ComboBoxControlMetrics::kRightPadding) * uiScale;
        const float triangleX = bounds.getRight() - triangleBaseSize - rightPad;
        const float triangleY = bounds.getCentreY() - triangleHeight * 0.5f;
        g.fillPath(createTrianglePath(triangleX, triangleY, triangleBaseSize));

        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(component);

        if (style == ComboBoxControlStyle::ButtonLike)
        {
            const float thickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
                static_cast<float>(ComboBoxControlMetrics::kBorderThicknessButtonLike),
                uiScale,
                systemDisplayScale,
                ScaledDrawing::StrokeSnapPolicy::kRound);
            g.setColour(borderColourForStyle(style, look, enabled));
            g.drawRect(bounds, thickness);
            return;
        }

        if (hasFocus)
        {
            const float thickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
                static_cast<float>(ComboBoxControlMetrics::kBorderThickness),
                uiScale,
                systemDisplayScale,
                ScaledDrawing::StrokeSnapPolicy::kRound);
            g.setColour(look.focusBorder);
            g.drawRect(backgroundBounds, thickness);
        }
    }
}
