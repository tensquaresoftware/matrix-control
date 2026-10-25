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

        mappingBandMatchesStatusAndSharedBadgeGap();
        windowMinCoversBodyAndMeasuredTitleSession();
        idleWitnessHiddenHoldWitnessVisible();
        bodyColumnsLeaveMappingsFloorWithoutUnbound();
        skinsPanelWrapUsesProductionButtonWidths();
    }

private:
    void mappingBandMatchesStatusAndSharedBadgeGap()
    {
        beginTest("Mapping band height matches status; badge gap shared with CONNECTION");

        expectEquals(SkinEditorChrome::kMappingBandHeight, SkinEditorChrome::kStatusBarHeight);
        expectEquals(SkinEditorChrome::kChromeBadgeGap, 8);
    }

    void windowMinCoversBodyAndMeasuredTitleSession()
    {
        beginTest("Window min is max(body, measured title+SESSION); used by MainWindow floor");

        const int titleSession = SkinEditorChrome::windowTitleSessionMinWidth();
        const int measuredFloor = SkinEditorChrome::windowMinWidth();
        expectEquals(titleSession,
                     SkinEditorChrome::kPanelPad + SkinEditorChrome::appTitlePreferredWidth()
                         + SkinEditorChrome::kButtonGap
                         + SkinEditorChrome::sessionWellPreferredWidth()
                         + SkinEditorChrome::kPanelPad);
        expectEquals(measuredFloor,
                     juce::jmax(SkinEditorChrome::kWindowBodyMinWidth, titleSession));
        expect(measuredFloor < SkinEditorChrome::kWindowMaxWidth);
        expectEquals(SkinEditorChrome::kWindowBodyMinWidth,
                     SkinEditorChrome::kGutter + SkinEditorChrome::kColorPanelWidth
                         + SkinEditorChrome::kGutter + SkinEditorChrome::kPalettePanelWidth
                         + SkinEditorChrome::kGutter + SkinEditorChrome::kMappingsMinWidth
                         + SkinEditorChrome::kGutter);
        // Title + SESSION pad budget must be covered by the same helper MainWindow uses.
        expect(measuredFloor >= titleSession);
    }

    void idleWitnessHiddenHoldWitnessVisible()
    {
        beginTest("Idle mapping band empty; Hold modes show witness");

        using namespace SkinEditorMappingPolicy;
        expect(! mappingWitnessVisible(Mode::kIdle));
        expect(mappingWitnessVisible(Mode::kHeldColour));
        expect(mappingWitnessVisible(Mode::kArmedRow));
    }

    void bodyColumnsLeaveMappingsFloorWithoutUnbound()
    {
        beginTest("Body is Color|Palette|Mappings (no Unbound half-split)");

        expect(SkinEditorChrome::kMappingsMinWidth >= 320);
        expect(SkinEditorChrome::kWindowBodyMinWidth < SkinEditorChrome::kWindowMaxWidth);
    }

    void skinsPanelWrapUsesProductionButtonWidths()
    {
        beginTest("SKINS wrap decision uses measureButtonWidth on Dup/New/Rename/Delete");

        const int contentW = SkinEditorChrome::kColorPanelWidth
                             - 2 * SkinEditorChrome::kPanelContentPad;
        const int fourButtons =
            SkinEditorChrome::measureButtonWidth(juce::TextButton("Duplicate"))
            + SkinEditorChrome::kButtonGap
            + SkinEditorChrome::measureButtonWidth(juce::TextButton("New"))
            + SkinEditorChrome::kButtonGap
            + SkinEditorChrome::measureButtonWidth(juce::TextButton("Rename"))
            + SkinEditorChrome::kButtonGap
            + SkinEditorChrome::measureButtonWidth(juce::TextButton("Delete"));
        expectEquals(contentW, 240);
        expect(fourButtons > contentW);
    }
};

static SkinEditorChromeLayoutTests skinEditorChromeLayoutTests;
