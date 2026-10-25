#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"
#include "Tools/SkinEditor/SkinEditorChromeTokens.h"
#include "Tools/SkinEditor/SkinEditorMappingModePolicy.h"

/**
 * Locks Skin Editor chrome layout tokens for SESSION / SKINS / mapping-band
 * (no Unbound column). Witness Idle/Hold visibility uses MappingModePolicy SSOT.
 * Export unboundCount == 0 gate: DevSkinLayerJsonTests.
 * Bind unbound keys: DevSkinLayerJsonTests / DevSkinMatrixTests.
 */
class SkinEditorChromeLayoutTests : public juce::UnitTest
{
public:
    SkinEditorChromeLayoutTests()
        : juce::UnitTest("SkinEditorChromeLayout")
    {
    }

    void runTest() override
    {
        SkinEditorChrome::warmFonts();

        operationWellMatchesSessionAndSharedBadgeGap();
        windowFitsFixedMappingsNotStretch();
        idleWitnessHiddenHoldWitnessVisible();
        mappingsKeepsLegacyHalfShareWidth();
        skinsPanelEditorActionsFitOneRow();
    }

private:
    void operationWellMatchesSessionAndSharedBadgeGap()
    {
        beginTest("OPERATION well matches SESSION height; badge gap shared with CONNECTION");

        expectEquals(SkinEditorChrome::kMappingBandHeight, SkinEditorChrome::kSessionWellHeight);
        expectEquals(SkinEditorChrome::kTitleBarHeight,
                     SkinEditorChrome::kChromeStackGap + SkinEditorChrome::kSessionWellHeight
                         + SkinEditorChrome::kGutter);
        expectEquals(SkinEditorChrome::kStatusBarHeight, SkinEditorChrome::kSessionWellHeight);
        expectEquals(SkinEditorChrome::kChromeStackGap, 12);
        expectEquals(SkinEditorChrome::kWindowEdgeGutter, 12);
        expectEquals(SkinEditorChrome::kPaletteSideGap, 12);
        expectEquals(SkinEditorChrome::kOperationWitnessPad, 4);
        expectEquals(SkinEditorChrome::kChromeBadgeGap, 8);
    }

    void windowFitsFixedMappingsNotStretch()
    {
        beginTest("Window content width fits fixed columns; does not stretch Mappings");

        const int titleSession = SkinEditorChrome::windowTitleSessionMinWidth();
        const int contentW = SkinEditorChrome::windowContentWidth();
        expectEquals(contentW,
                     juce::jmax(SkinEditorChrome::kWindowBodyWidth, titleSession));
        expectEquals(SkinEditorChrome::windowMinWidth(), contentW);
        expectEquals(SkinEditorChrome::windowMaxWidth(), contentW);
        expectEquals(SkinEditorChrome::kWindowBodyWidth,
                     SkinEditorChrome::kWindowEdgeGutter + SkinEditorChrome::kColorPanelWidth
                         + SkinEditorChrome::kPaletteSideGap + SkinEditorChrome::kPalettePanelWidth
                         + SkinEditorChrome::kPaletteSideGap + SkinEditorChrome::kMappingsPanelWidth
                         + SkinEditorChrome::kWindowEdgeGutter);
        expect(contentW < SkinEditorChrome::kLegacyWindowMaxWidth);
    }

    void idleWitnessHiddenHoldWitnessVisible()
    {
        beginTest("Idle mapping band empty; Hold modes show witness");

        using namespace SkinEditorMappingPolicy;
        expect(! mappingWitnessVisible(Mode::kIdle));
        expect(mappingWitnessVisible(Mode::kHeldColour));
        expect(mappingWitnessVisible(Mode::kArmedRow));
    }

    void mappingsKeepsLegacyHalfShareWidth()
    {
        beginTest("Mappings width equals former Unbound half-split at legacy 1210");

        expectEquals(SkinEditorChrome::kMappingsPanelWidth, 385);
        expect(SkinEditorChrome::kMappingsPanelWidth
               < SkinEditorChrome::kLegacyWindowMaxWidth / 2);
    }

    void skinsPanelEditorActionsFitOneRow()
    {
        beginTest("SKINS Delete matches Duplicate; New/Rename/Delete fit combo-to-Duplicate span");

        const int contentW = SkinEditorChrome::kColorPanelWidth
                             - 2 * SkinEditorChrome::kPanelContentPad;
        const int labelColW = SkinEditorChrome::measureLabelWidth(
            "Product:", SkinEditorChrome::fontRegular());
        // Prefer Editor: if wider — labelColumnWidth takes max of both.
        const int editorLabelW = SkinEditorChrome::measureLabelWidth(
            "Editor:", SkinEditorChrome::fontRegular());
        const int labelsW = juce::jmax(labelColW, editorLabelW);
        const int dupW = SkinEditorChrome::measureButtonWidth(juce::TextButton("Duplicate"));
        const int renameW = SkinEditorChrome::measureButtonWidth(juce::TextButton("Rename"));
        const int gaps = 2 * SkinEditorChrome::kButtonGap;
        const int comboW = juce::jmax(72, contentW - labelsW - SkinEditorChrome::kButtonGap - dupW);
        const int spanW = comboW + SkinEditorChrome::kButtonGap + dupW;
        const int newW = spanW - gaps - renameW - dupW;
        expectEquals(contentW, 240);
        expect(newW >= 1);
        expectEquals(newW + gaps + renameW + dupW, spanW);
    }
};

static SkinEditorChromeLayoutTests skinEditorChromeLayoutTests;
