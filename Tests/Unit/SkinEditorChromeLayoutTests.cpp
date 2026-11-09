#include <juce_core/juce_core.h>

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"
#include "Tools/SkinEditor/SkinEditorChromeTokens.h"
#include "Tools/SkinEditor/SkinEditorColourPicker.h"
#include "Tools/SkinEditor/SkinEditorMappingModePolicy.h"
#include "Tools/SkinEditor/SkinEditorMappingsColumns.h"
#include "Tools/SkinEditor/SkinEditorSkinsPanel.h"

class SkinEditorChromeLayoutTests : public juce::UnitTest
{
public:
    SkinEditorChromeLayoutTests()
        : juce::UnitTest("SkinEditorChromeLayout")
    {
    }

    void runTest() override
    {
        operationWellMatchesSessionHeight();
        titleChromeMinWidthIncludesOperationFloor();
        windowFitsFlexiblePaletteMapped();
        idleWitnessHiddenHoldWitnessVisible();
        mappingsColumnPadAndWidths();
        mappingsChromeBadgeTokens();
        propertyBindHoverInk();
        unboundHoldNeighbourhoodGeometry();
        skinsPanelEditorActionsFitOneRow();
        minContentHeightStacksSkinsAndColor();
        pathElideKeepsLeafName();
        lastPushBadgeAlignsWithPaletteColumn();
    }

private:
    void operationWellMatchesSessionHeight()
    {
        beginTest("OPERATION well matches SESSION height; badge gap shared with BRIDGE");

        expectEquals(SkinEditorChrome::kMappingBandHeight,
                     SkinEditorChrome::kSessionWellHeight);
        expectEquals(SkinEditorChrome::kOperationWitnessPad, 4);
        expectEquals(SkinEditorChrome::kChromeBadgeGap, 8);
    }

    void titleChromeMinWidthIncludesOperationFloor()
    {
        beginTest("Title chrome min width locks SESSION + OPERATION 510");

        const int exact = SkinEditorChrome::kWindowEdgeGutter
                          + SkinEditorChrome::sessionWellPreferredWidth()
                          + SkinEditorChrome::kButtonGap
                          + SkinEditorChrome::kOperationWellMinWidth
                          + SkinEditorChrome::kWindowEdgeGutter;
        const int bodyMin = SkinEditorChrome::windowBodyWidthForPalettePanel(
            SkinEditorChrome::palettePanelMinWidth());

        expectEquals(SkinEditorChrome::kOperationWellMinWidth, 510);
        expectEquals(SkinEditorChrome::windowTitleChromeMinWidth(), exact);
        expectEquals(SkinEditorChrome::windowMinWidth(),
                     juce::jmax(bodyMin, SkinEditorChrome::windowTitleChromeMinWidth()));
    }

    void windowFitsFlexiblePaletteMapped()
    {
        beginTest("Mapped colour tracks Palette width when the window stretches");

        expectEquals(SkinEditorChrome::kWindowBodyMinWidth,
                     SkinEditorChrome::windowBodyWidthForPalettePanel(
                         SkinEditorChrome::kPalettePanelMinWidth));

        const int paletteAtMin = SkinEditorChrome::kPalettePanelMinWidth;
        const int mappedAtMin = SkinEditorChrome::paletteListPaintWidthForPanel(paletteAtMin);
        expectEquals(mappedAtMin, SkinEditorChrome::kMappingsMappedColumnMinWidth);
        expectEquals(SkinEditorChrome::mappingsPanelWidthForPalettePanel(paletteAtMin),
                     SkinEditorChrome::kMappingsPanelMinWidth);

        // Growth: +20 Palette → +20 Mapped paint, +20 Mappings panel, +40 window body.
        const int paletteWide = paletteAtMin + 20;
        expectEquals(SkinEditorChrome::paletteListPaintWidthForPanel(paletteWide),
                     mappedAtMin + 20);
        expectEquals(SkinEditorChrome::mappingsPanelWidthForPalettePanel(paletteWide),
                     SkinEditorChrome::kMappingsPanelMinWidth + 20);
        expectEquals(SkinEditorChrome::windowBodyWidthForPalettePanel(paletteWide),
                     SkinEditorChrome::windowBodyWidthForPalettePanel(paletteAtMin) + 40);

        // Inverse: body remainder after Color + leading Palette gap recovers Palette width.
        const int bodyWide = SkinEditorChrome::windowBodyWidthForPalettePanel(paletteWide);
        const int remainderAfterColorAndLeadingGap =
            bodyWide - SkinEditorChrome::kWindowEdgeGutter - SkinEditorChrome::kColorPanelWidth
            - SkinEditorChrome::kPaletteSideGap - SkinEditorChrome::kWindowEdgeGutter;
        const int paletteFromRemainder =
            SkinEditorChrome::palettePanelWidthFromBodyRemainder(remainderAfterColorAndLeadingGap);
        expectEquals(paletteFromRemainder, paletteWide);
        expectEquals(SkinEditorChrome::windowBodyWidthForPalettePanel(paletteFromRemainder),
                     bodyWide);
    }

    void idleWitnessHiddenHoldWitnessVisible()
    {
        beginTest("Idle mapping band empty; Hold modes show witness");

        using namespace SkinEditorMappingPolicy;
        expect(! mappingWitnessVisible(Mode::kIdle));
        expect(mappingWitnessVisible(Mode::kHeldColour));
        expect(mappingWitnessVisible(Mode::kArmedRow));
    }

    void mappingsColumnPadAndWidths()
    {
        beginTest("Mappings columns use 6px cell pad; Mapped absorbs table remainder");

        expectEquals(SkinEditorChrome::kPalettePanelMinWidth, 160);
        expectEquals(SkinEditorChrome::kPaletteListPaintMinWidth, 132);
        expect(SkinEditorChrome::palettePanelMinWidth()
               >= SkinEditorChrome::palettePanelWidthForActionButtons());
        expectEquals(SkinEditorChrome::kMappingsColumnPad, 6);
        expectEquals(SkinEditorChrome::kMappingsPropertyColumnWidth, 160);
        expectEquals(SkinEditorChrome::kMappingsStateColumnWidth, 85);
        expectEquals(SkinEditorChrome::kMappingsColourSetColumnWidth, 140);
        expectEquals(SkinEditorChrome::kMappingsLinkColumnWidth, 55);
        expectEquals(SkinEditorChrome::kLinkCountBadgeWidth, 22);
        expectEquals(SkinEditorChrome::kMappingsFixedColumnsWidth, 440);
        expectEquals(SkinEditorChrome::kMappingsMappedColumnMinWidth, 132);
        expectEquals(SkinEditorChrome::kMappingsTablePaintMinWidth, 572);
        expectEquals(SkinEditorChrome::kMappingsPanelMinWidth, 602);
        expect(SkinEditorChrome::kMappingsPropertyColumnWidth % 5 == 0);
        expectEquals(SkinEditorChrome::kObjectComboWidth, 149);
        expectEquals(SkinEditorChrome::mappingsHoldLeft(100, 200), 200);
        expectEquals(SkinEditorChrome::mappingsHoldLeft(200, 100),
                     200 + SkinEditorChrome::kButtonGap);

        const int tableW = SkinEditorChrome::kMappingsTablePaintMinWidth + 40;
        const auto cols = SkinEditorMappingsColumns::forWidth(tableW);
        expectEquals(cols.mappedX, SkinEditorChrome::kMappingsFixedColumnsWidth);
        expectEquals(cols.mappedW, tableW - SkinEditorChrome::kMappingsFixedColumnsWidth);
        expect(SkinEditorChrome::kMappingsPanelMinWidth
               < SkinEditorChrome::kWindowBodyMinWidth);
    }

    void mappingsChromeBadgeTokens()
    {
        beginTest("Mappings chrome badge / chip tokens");

        expectEquals(juce::String(SkinEditorChrome::kAllObjectsComboText),
                     juce::String("- All objects -"));
        expectEquals(SkinEditorChrome::kClearBadgeSize, 12);
        expectEquals(SkinEditorChrome::kSortBadgeSize, SkinEditorChrome::kClearBadgeSize);
        expectEquals(SkinEditorChrome::kLinkChainBadgeSize, SkinEditorChrome::kSortBadgeSize);
        expectEquals(SkinEditorChrome::kLegendChipGap, 4);
        expectEquals(SkinEditorChrome::kCountBadgeDiameter, 12);
    }

    void propertyBindHoverInk()
    {
        beginTest("Property bind hover picks dark ink on light fills");

        expect(SkinEditorChrome::propertyBindHoverNeedsDarkText(juce::Colours::white));
        expect(SkinEditorChrome::propertyBindHoverNeedsDarkText(juce::Colour(0xfff5f0e6)));
        expect(! SkinEditorChrome::propertyBindHoverNeedsDarkText(juce::Colours::black));
        expect(! SkinEditorChrome::propertyBindHoverNeedsDarkText(juce::Colour(0xff1a1a1a)));
    }

    void unboundHoldNeighbourhoodGeometry()
    {
        beginTest("Unbound origin and Hold align follow Mapped / Colour set contentLeft");

        const auto cols = SkinEditorMappingsColumns::forWidth(
            SkinEditorChrome::kMappingsTablePaintMinWidth);
        expectEquals(cols.contentLeft(cols.mappedX),
                     cols.mappedX + SkinEditorChrome::kMappingsColumnPad);
        expectEquals(cols.contentLeft(cols.colourSetLeft()) - 6,
                     cols.colourSetLeft() + SkinEditorChrome::kMappingsColumnPad - 6);
        expectEquals(SkinEditorChrome::kLinkCountBadgeWidth, 22);
        const int holdAlignX = cols.contentLeft(cols.colourSetLeft()) - 6;
        expectEquals(SkinEditorChrome::mappingsHoldLeft(100, holdAlignX), holdAlignX);
    }

    static juce::TextButton* findButton(juce::Component& root, const juce::String& text)
    {
        for (auto* c : root.getChildren())
            if (auto* b = dynamic_cast<juce::TextButton*>(c))
                if (b->getButtonText() == text)
                    return b;
        return nullptr;
    }

    static juce::TextButton* findLowerNextNav(juce::Component& root)
    {
        juce::TextButton* lower = nullptr;
        for (auto* c : root.getChildren())
        {
            auto* b = dynamic_cast<juce::TextButton*>(c);
            if (b == nullptr || b->getButtonText() != ">")
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
        beginTest("SKINS Dup/New/Ren/Del fit combo-to-Next span under Editor");

        SkinEditorSkinsPanel panel;
        panel.setSize(SkinEditorChrome::kColorPanelWidth, SkinEditorSkinsPanel::preferredHeight());

        auto* editorCombo = findLowerCombo(panel);
        auto* editorNext = findLowerNextNav(panel);
        auto* dupBtn = findButton(panel, "Dup");
        auto* newBtn = findButton(panel, "New");
        auto* renameBtn = findButton(panel, "Ren");
        auto* deleteBtn = findButton(panel, "Del");

        expect(editorCombo != nullptr);
        expect(editorNext != nullptr);
        expect(dupBtn != nullptr);
        expect(newBtn != nullptr);
        expect(renameBtn != nullptr);
        expect(deleteBtn != nullptr);
        if (editorCombo == nullptr || editorNext == nullptr || dupBtn == nullptr
            || newBtn == nullptr || renameBtn == nullptr || deleteBtn == nullptr)
            return;

        expectEquals(dupBtn->getY(), newBtn->getY());
        expectEquals(newBtn->getY(), renameBtn->getY());
        expectEquals(renameBtn->getY(), deleteBtn->getY());
        expectEquals(dupBtn->getX(), editorCombo->getX());
        expectEquals(deleteBtn->getRight(), editorNext->getRight());
        expect(dupBtn->getY() > editorCombo->getBottom());
    }

    void minContentHeightStacksSkinsAndColor()
    {
        beginTest("Min content height stacks title, SKINS, COLOR, status");

        const int skinsH = SkinEditorSkinsPanel::preferredHeight();
        const int colorH = SkinEditorColourPicker::preferredHeightForWidth(
            SkinEditorChrome::kColorPanelWidth);
        const int minH = SkinEditorChrome::kTitleBarHeight + SkinEditorChrome::kGutter
                         + skinsH + colorH + SkinEditorChrome::kStatusBarHeight;

        expect(skinsH > 0);
        expect(colorH > 0);
        expect(minH > skinsH + colorH);
        constexpr int kFieldRowH = 24;
        constexpr int kHueBarW = 18;
        constexpr int kSvHueGap = 8;
        constexpr int kTightPad = SkinEditorChrome::kPanelContentPad - 5;
        const int contentW = SkinEditorChrome::kColorPanelWidth
                             - 2 * SkinEditorChrome::kPanelContentPad;
        const int side = juce::jmax(0, contentW - kHueBarW - kSvHueGap);
        expectEquals(colorH,
                     SkinEditorChrome::kGutter + SkinEditorChrome::kPanelTitleHeight
                         + SkinEditorChrome::kPanelContentPad + side
                         + SkinEditorChrome::kPanelContentPad + kFieldRowH
                         + kTightPad + 2 * kFieldRowH
                         + kTightPad);
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
