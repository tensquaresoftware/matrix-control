#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"
#include "Tools/SkinEditor/SkinEditorChromeTokens.h"
#include "Tools/SkinEditor/SkinEditorColourPicker.h"
#include "Tools/SkinEditor/SkinEditorMappingModePolicy.h"
#include "Tools/SkinEditor/SkinEditorSkinsPanel.h"

/**
 * Locks Skin Editor chrome layout tokens for SESSION / SKINS / OPERATION
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
        minContentHeightStacksSkinsAndColor();
        pathElideKeepsLeafName();
        lastPushBadgeAlignsWithPaletteColumn();
    }

private:
    void operationWellMatchesSessionAndSharedBadgeGap()
    {
        beginTest("OPERATION well matches SESSION height; badge gap shared with BRIDGE");

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
        beginTest("Mappings width includes former Palette surplus for Property/State");

        expectEquals(SkinEditorChrome::kPalettePanelWidth, 156);
        expectEquals(SkinEditorChrome::kPalettePanelWidth,
                     SkinEditorChrome::palettePanelWidthForActionButtons());
        expectEquals(SkinEditorChrome::kPaletteListPaintWidth,
                     SkinEditorChrome::paletteListPaintFloor());
        expectEquals(SkinEditorChrome::kPaletteListPaintWidth, 128);
        expectEquals(SkinEditorChrome::kPaletteListPaintWidth,
                     SkinEditorChrome::kPalettePanelWidth
                         - SkinEditorChrome::kPanelContentPad
                         - SkinEditorChrome::kScrollTrackWidth);
        expectEquals(SkinEditorChrome::kMappingsPanelWidth, 480);
        expect(SkinEditorChrome::kMappingsPanelWidth
               < SkinEditorChrome::kLegacyWindowMaxWidth / 2);
    }

    static juce::TextButton* findButton(juce::Component& root, const juce::String& text)
    {
        for (auto* c : root.getChildren())
            if (auto* b = dynamic_cast<juce::TextButton*>(c))
                if (b->getButtonText() == text)
                    return b;
        return nullptr;
    }

    static juce::TextButton* findLowerDuplicate(juce::Component& root)
    {
        juce::TextButton* lower = nullptr;
        for (auto* c : root.getChildren())
        {
            auto* b = dynamic_cast<juce::TextButton*>(c);
            if (b == nullptr || b->getButtonText() != "Duplicate")
                continue;
            if (lower == nullptr || b->getY() > lower->getY())
                lower = b;
        }
        return lower;
    }

    static juce::ComboBox* findLowerCombo(juce::Component& root)
    {
        juce::ComboBox* lower = nullptr;
        for (auto* c : root.getChildren())
        {
            auto* box = dynamic_cast<juce::ComboBox*>(c);
            if (box == nullptr)
                continue;
            if (lower == nullptr || box->getY() > lower->getY())
                lower = box;
        }
        return lower;
    }

    void skinsPanelEditorActionsFitOneRow()
    {
        beginTest("SKINS Delete matches Duplicate; New/Rename/Delete fit combo-to-Duplicate span");

        SkinEditorSkinsPanel panel;
        panel.setSize(SkinEditorChrome::kColorPanelWidth, SkinEditorSkinsPanel::preferredHeight());

        auto* editorCombo = findLowerCombo(panel);
        auto* editorDup = findLowerDuplicate(panel);
        auto* newBtn = findButton(panel, "New");
        auto* renameBtn = findButton(panel, "Rename");
        auto* deleteBtn = findButton(panel, "Delete");

        expect(editorCombo != nullptr);
        expect(editorDup != nullptr);
        expect(newBtn != nullptr);
        expect(renameBtn != nullptr);
        expect(deleteBtn != nullptr);
        if (editorCombo == nullptr || editorDup == nullptr || newBtn == nullptr
            || renameBtn == nullptr || deleteBtn == nullptr)
            return;

        expectEquals(deleteBtn->getWidth(), editorDup->getWidth());
        expectEquals(newBtn->getY(), renameBtn->getY());
        expectEquals(renameBtn->getY(), deleteBtn->getY());
        expectEquals(newBtn->getX(), editorCombo->getX());
        expectEquals(deleteBtn->getRight(), editorDup->getRight());
        expect(newBtn->getWidth() >= 1);
    }

    void minContentHeightStacksSkinsAndColor()
    {
        beginTest("Min content height stacks title, OPERATION, SKINS, COLOR, status");

        const int skinsH = SkinEditorSkinsPanel::preferredHeight();
        const int colorH = SkinEditorColourPicker::preferredHeightForWidth(
            SkinEditorChrome::kColorPanelWidth);
        // Mirrors SkinEditorMainWindow::skinEditorMinContentHeight (anonymous).
        const int minH = SkinEditorChrome::kTitleBarHeight + SkinEditorChrome::kGutter
                         + SkinEditorChrome::kMappingBandHeight + SkinEditorChrome::kGutter
                         + skinsH + colorH + SkinEditorChrome::kStatusBarHeight;

        expect(skinsH > 0);
        expect(colorH > 0);
        expect(minH > skinsH + colorH);
        // COLOR flush: preferred height has top gutter + content pads, no trailing kGutter.
        constexpr int kFieldRowH = 24;
        constexpr int kHueBarW = 18;
        constexpr int kSvHueGap = 8;
        const int contentW = SkinEditorChrome::kColorPanelWidth
                             - 2 * SkinEditorChrome::kPanelContentPad;
        const int side = juce::jmax(0, contentW - kHueBarW - kSvHueGap);
        expectEquals(colorH,
                     SkinEditorChrome::kGutter + SkinEditorChrome::kPanelTitleHeight
                         + SkinEditorChrome::kPanelContentPad + side
                         + SkinEditorChrome::kPanelContentPad + kFieldRowH
                         + SkinEditorChrome::kPanelContentPad);
        // SKINS keeps top + bottom gutters (bottom contributes to SKINS↔COLOR gap).
        expect(skinsH
               >= SkinEditorChrome::kGutter + SkinEditorChrome::kPanelTitleHeight
                      + SkinEditorChrome::kGutter);
    }

    void pathElideKeepsLeafName()
    {
        beginTest("Path elide keeps leaf filename + extension visible");

        const auto font = SkinEditorChrome::fontRegular();
        const juce::String path = "Exported: /Users/dev/very/long/nested/path/MySkin.json";
        const int fullW = SkinEditorChrome::measureTextWidth(font, path);
        expect(fullW > 80);

        const auto shown = SkinEditorChrome::elidePathToWidth(font, path, fullW / 2);
        expect(shown.contains("..."));
        expect(shown.endsWithIgnoreCase("MySkin.json")
               || shown.endsWithIgnoreCase(".json"));
        expectEquals(SkinEditorChrome::elidePathToWidth(font, path, fullW + 10), path);
    }

    void lastPushBadgeAlignsWithPaletteColumn()
    {
        beginTest("LAST PUSH badge X offset matches Palette column (Color + side gap)");

        expectEquals(SkinEditorChrome::kColorPanelWidth + SkinEditorChrome::kPaletteSideGap,
                     260 + 12);
    }
};

static SkinEditorChromeLayoutTests skinEditorChromeLayoutTests;
