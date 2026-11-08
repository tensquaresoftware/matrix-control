#include <juce_core/juce_core.h>

#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Widgets/PopupMenuCustomScrollBar.h"

class PopupMenuCustomScrollBarTests : public juce::UnitTest
{
public:
    PopupMenuCustomScrollBarTests()
        : juce::UnitTest("PopupMenuCustomScrollBar")
    {
    }

    void runTest() override
    {
        beginTest("thumbInsetLogicalPixels_usesUiScaleOnly_andStaysBelowHalfTrack");

        // Design inset 2 at uiScale 1 → 2 logical px (fits in an 8 px track).
        expectWithinAbsoluteError(TSS::PopupMenuCustomScrollBar::thumbInsetLogicalPixels(2.0f, 1.0f),
                                  2.0f,
                                  0.01f);

        // Retina: old ScaledDrawing path multiplied by displayScale=2 → inset 4 on an 8 px
        // track → 0 px thumb. uiScale-only keeps inset at 2 so the thumb stays visible.
        constexpr float kDesignInset = 2.0f;
        constexpr float kUiScale = 1.0f;
        constexpr float kRetinaDisplayScale = 2.0f;
        constexpr float kTrackWidth = 8.0f;
        const auto logicalOnly =
            TSS::PopupMenuCustomScrollBar::thumbInsetLogicalPixels(kDesignInset, kUiScale);
        const auto oldHiDpi = static_cast<float>(TSS::ScaledDrawing::logicalInsetPixelsFromDesign(
            kDesignInset, kUiScale, kRetinaDisplayScale));
        expectWithinAbsoluteError(logicalOnly, 2.0f, 0.01f);
        expect(logicalOnly < oldHiDpi);
        expect(logicalOnly * 2.0f < kTrackWidth);
        expect(oldHiDpi * 2.0f >= kTrackWidth);

        expectWithinAbsoluteError(TSS::PopupMenuCustomScrollBar::thumbInsetLogicalPixels(2.0f, 1.5f),
                                  3.0f,
                                  0.01f);
        expect(TSS::PopupMenuCustomScrollBar::thumbInsetLogicalPixels(2.0f, 0.0f) >= 1.0f);

        beginTest("clampThumbInsetForAxis_keepsAtLeastOnePixelOnAxis");

        // Oversized inset on an 8 px thumb would collapse without clamp.
        expectWithinAbsoluteError(
            TSS::PopupMenuCustomScrollBar::clampThumbInsetForAxis(10.0f, 8.0f),
            3.5f,
            0.01f);
        expectWithinAbsoluteError(
            TSS::PopupMenuCustomScrollBar::clampThumbInsetForAxis(2.0f, 8.0f),
            2.0f,
            0.01f);
        expectWithinAbsoluteError(
            TSS::PopupMenuCustomScrollBar::clampThumbInsetForAxis(1.0f, 1.0f),
            0.0f,
            0.01f);
    }
};

static PopupMenuCustomScrollBarTests popupMenuCustomScrollBarTests;
