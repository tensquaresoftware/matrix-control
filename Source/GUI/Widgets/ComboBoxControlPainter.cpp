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

        juce::Colour textColour(const ComboBoxLook& look, bool enabled)
        {
            return enabled ? look.textEnabled : look.textDisabled;
        }

        juce::Colour triangleColour(const ComboBoxLook& look, bool enabled)
        {
            return enabled ? look.triangleEnabled : look.triangleDisabled;
        }

        juce::Colour backgroundColour(const ComboBoxLook& look, bool enabled)
        {
            return enabled ? look.backgroundEnabled : look.backgroundDisabled;
        }

        juce::Colour borderColour(const ComboBoxLook& look,
                                  bool enabled,
                                  ComboBoxClosedChrome closedChrome)
        {
            if (closedChrome == ComboBoxClosedChrome::AlwaysThickBorder)
            {
                juce::ignoreUnused(enabled);
                // Disabled border matches the enabled Scrollable closed-face border.
                return look.borderEnabled;
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

    void ComboBoxControlPainter::paintClosedState(juce::Graphics& g, const ClosedStateArgs& args)
    {
        const auto backgroundBounds = args.bounds;
        g.setColour(backgroundColour(args.look, args.enabled));
        g.fillRect(backgroundBounds);

        const auto textBounds = calculateTextBounds(args.bounds, args.uiScale);
        g.setColour(textColour(args.look, args.enabled));
        g.setFont(args.look.font.withHeight(args.look.font.getHeight() * args.uiScale));
        g.drawText(args.text, textBounds, juce::Justification::centredLeft, false);

        g.setColour(triangleColour(args.look, args.enabled));
        const float triangleBaseSize = static_cast<float>(ComboBoxControlMetrics::kTriangleBaseSize) * args.uiScale;
        const float triangleHeight = triangleBaseSize * ComboBoxControlMetrics::kTriangleHeightFactor;
        const float rightPad = static_cast<float>(ComboBoxControlMetrics::kRightPadding) * args.uiScale;
        const float triangleX = args.bounds.getRight() - triangleBaseSize - rightPad;
        const float triangleY = args.bounds.getCentreY() - triangleHeight * 0.5f;
        g.fillPath(createTrianglePath(triangleX, triangleY, triangleBaseSize));

        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(args.component);

        if (args.closedChrome == ComboBoxClosedChrome::AlwaysThickBorder)
        {
            const float thickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
                static_cast<float>(ComboBoxControlMetrics::kBorderThicknessScrollable),
                args.uiScale,
                systemDisplayScale,
                ScaledDrawing::StrokeSnapPolicy::kRound);
            g.setColour(borderColour(args.look, args.enabled, args.closedChrome));
            g.drawRect(args.bounds, thickness);
            return;
        }

        if (args.hasFocus)
        {
            const float thickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
                static_cast<float>(ComboBoxControlMetrics::kBorderThickness),
                args.uiScale,
                systemDisplayScale,
                ScaledDrawing::StrokeSnapPolicy::kRound);
            g.setColour(args.look.focusBorder);
            g.drawRect(backgroundBounds, thickness);
        }
    }
}
