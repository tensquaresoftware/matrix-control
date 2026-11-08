#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace TSS
{
    struct ComboBoxControlMetrics
    {
        static constexpr int kDefaultWidth = 100;
        static constexpr int kDefaultHeight = 20;
        static constexpr int kLeftPadding = 4;
        static constexpr int kRightPadding = 4;
        static constexpr int kBorderThickness = 1;
        static constexpr int kBorderThicknessScrollable = 2;
        static constexpr int kTriangleBaseSize = 7;
        static constexpr float kTriangleHeightFactor = 0.8660254f;
    };

    /** Closed-face chrome: MultiColumn uses a focus ring; Scrollable always draws a thick border. */
    enum class ComboBoxClosedChrome
    {
        FocusRing,
        AlwaysThickBorder
    };

    class ComboBoxControlPainter
    {
    public:
        struct ClosedStateArgs
        {
            const juce::Component& component;
            juce::Rectangle<float> bounds;
            ComboBoxClosedChrome closedChrome = ComboBoxClosedChrome::FocusRing;
            const ComboBoxLook& look;
            float uiScale = 1.0f;
            juce::String text;
            bool enabled = true;
            bool hasFocus = false;
        };

        static void paintClosedState(juce::Graphics& g, const ClosedStateArgs& args);
    };
}
