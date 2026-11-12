#include <juce_core/juce_core.h>

#include "Tools/SkinDesigner/SkinDesignerChromeFonts.h"
#include "Tools/SkinDesigner/SkinDesignerChromeTokens.h"
#include "Tools/SkinDesigner/SkinDesignerColourPicker.h"
#include "Tools/SkinDesigner/SkinDesignerMappingModePolicy.h"
#include "Tools/SkinDesigner/SkinDesignerMappingsColumns.h"
#include "Tools/SkinDesigner/SkinDesignerSkinsPanel.h"
#include "Tools/SkinDesigner/SkinDesignerStepper.h"

class SkinDesignerChromeLayoutTests : public juce::UnitTest
{
public:
    SkinDesignerChromeLayoutTests()
        : juce::UnitTest("SkinDesignerChromeLayout")
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
        skinsPanelDesignerActionsFitOneRow();
        skinsPanelProductDuplicateCompareFitOneRow();
        minContentHeightStacksSkinsAndColor();
        pathElideKeepsLeafName();
        lastPushBadgeAlignsWithPaletteColumn();
    }

private:
    void operationWellMatchesSessionHeight()
    {
        beginTest("OPERATION well matches SESSION height; badge gap shared with BRIDGE");

        expectEquals(SkinDesignerChrome::kMappingBandHeight,
                     SkinDesignerChrome::kSessionWellHeight);
        expectEquals(SkinDesignerChrome::kOperationWitnessPad, 4);
        expectEquals(SkinDesignerChrome::kChromeBadgeGap, 8);
        expectEquals(SkinDesignerChrome::kJsonSyncNoticeCloseGap, 2);
    }

    void titleChromeMinWidthIncludesOperationFloor()
    {
        beginTest("Window min width locks at 1100 via derived OPERATION floor");

        const int exact = SkinDesignerChrome::kWindowEdgeGutter
                          + SkinDesignerChrome::sessionWellPreferredWidth()
                          + SkinDesignerChrome::kSessionOperationGap
                          + SkinDesignerChrome::operationWellMinWidth()
                          + SkinDesignerChrome::kWindowEdgeGutter;
        const int bodyMin = SkinDesignerChrome::windowBodyWidthForPalettePanel(
            SkinDesignerChrome::kPalettePanelMinWidth);

        expectEquals(SkinDesignerChrome::kWindowContentMinWidth, 1100);
        expectEquals(SkinDesignerChrome::windowTitleChromeMinWidth(), exact);
        expectEquals(SkinDesignerChrome::windowTitleChromeMinWidth(),
                     SkinDesignerChrome::kWindowContentMinWidth);
        expectEquals(SkinDesignerChrome::windowMinWidth(), 1100);
        expect(bodyMin <= SkinDesignerChrome::kWindowContentMinWidth);
    }

    void windowFitsFlexiblePaletteMapped()
    {
        beginTest("Mapped colour tracks Palette width when the window stretches");

        expectEquals(SkinDesignerChrome::kWindowBodyMinWidth,
                     SkinDesignerChrome::windowBodyWidthForPalettePanel(
                         SkinDesignerChrome::kPalettePanelMinWidth));

        const int paletteAtMin = SkinDesignerChrome::kPalettePanelMinWidth;
        const int mappedAtMin = SkinDesignerChrome::paletteListPaintWidthForPanel(paletteAtMin);
        expectEquals(mappedAtMin, SkinDesignerChrome::kMappingsMappedColumnMinWidth);
        expectEquals(SkinDesignerChrome::mappingsPanelWidthForPalettePanel(paletteAtMin),
                     SkinDesignerChrome::kMappingsPanelMinWidth);

        // Growth: +20 Palette → +20 Mapped paint, +20 Mappings panel, +40 window body.
        const int paletteWide = paletteAtMin + 20;
        expectEquals(SkinDesignerChrome::paletteListPaintWidthForPanel(paletteWide),
                     mappedAtMin + 20);
        expectEquals(SkinDesignerChrome::mappingsPanelWidthForPalettePanel(paletteWide),
                     SkinDesignerChrome::kMappingsPanelMinWidth + 20);
        expectEquals(SkinDesignerChrome::windowBodyWidthForPalettePanel(paletteWide),
                     SkinDesignerChrome::windowBodyWidthForPalettePanel(paletteAtMin) + 40);

        // Inverse: body remainder after Color + leading Palette gap recovers Palette width.
        const int bodyWide = SkinDesignerChrome::windowBodyWidthForPalettePanel(paletteWide);
        const int remainderAfterColorAndLeadingGap =
            bodyWide - SkinDesignerChrome::kWindowEdgeGutter - SkinDesignerChrome::kColorPanelWidth
            - SkinDesignerChrome::kPaletteSideGap - SkinDesignerChrome::kWindowEdgeGutter;
        const int paletteFromRemainder =
            SkinDesignerChrome::palettePanelWidthFromBodyRemainder(remainderAfterColorAndLeadingGap);
        expectEquals(paletteFromRemainder, paletteWide);
        expectEquals(SkinDesignerChrome::windowBodyWidthForPalettePanel(paletteFromRemainder),
                     bodyWide);
    }

    void idleWitnessHiddenHoldWitnessVisible()
    {
        beginTest("Idle mapping band empty; Hold modes show witness");

        using namespace SkinDesignerMappingPolicy;
        expect(! mappingWitnessVisible(Mode::kIdle));
        expect(mappingWitnessVisible(Mode::kHeldColour));
        expect(mappingWitnessVisible(Mode::kArmedRow));
    }

    void mappingsColumnPadAndWidths()
    {
        beginTest("Mappings columns use 6px cell pad; Mapped absorbs table remainder");

        expectEquals(SkinDesignerChrome::kPalettePanelMinWidth, 125);
        expectEquals(SkinDesignerChrome::kPaletteListPaintMinWidth, 97);
        expect(SkinDesignerChrome::palettePanelMinWidth()
               >= SkinDesignerChrome::palettePanelWidthForActionButtons());
        expectEquals(SkinDesignerChrome::kMappingsColumnPad, 6);
        expectEquals(SkinDesignerChrome::kMappingsPropertyColumnWidth, 160);
        expectEquals(SkinDesignerChrome::kMappingsStateColumnWidth, 85);
        expectEquals(SkinDesignerChrome::kMappingsColourSetColumnWidth, 140);
        expectEquals(SkinDesignerChrome::kMappingsLinkColumnWidth, 55);
        expectEquals(SkinDesignerChrome::kLinkCountBadgeWidth, 22);
        expectEquals(SkinDesignerChrome::kMappingsFixedColumnsWidth, 440);
        expectEquals(SkinDesignerChrome::kMappingsMappedColumnMinWidth, 97);
        expectEquals(SkinDesignerChrome::kMappingsTablePaintMinWidth, 537);
        expectEquals(SkinDesignerChrome::kMappingsPanelMinWidth, 567);
        expect(SkinDesignerChrome::kMappingsPropertyColumnWidth % 5 == 0);
        expectEquals(SkinDesignerChrome::kObjectComboWidth, 149);
        expectEquals(SkinDesignerChrome::mappingsHoldLeft(100, 200), 200);
        expectEquals(SkinDesignerChrome::mappingsHoldLeft(200, 100),
                     200 + SkinDesignerChrome::kButtonGap);

        const int tableW = SkinDesignerChrome::kMappingsTablePaintMinWidth + 40;
        const auto cols = SkinDesignerMappingsColumns::forWidth(tableW);
        expectEquals(cols.mappedX, SkinDesignerChrome::kMappingsFixedColumnsWidth);
        expectEquals(cols.mappedW, tableW - SkinDesignerChrome::kMappingsFixedColumnsWidth);
        expect(SkinDesignerChrome::kMappingsPanelMinWidth
               < SkinDesignerChrome::kWindowBodyMinWidth);
    }

    void mappingsChromeBadgeTokens()
    {
        beginTest("Mappings chrome badge / chip tokens");

        expectEquals(juce::String(SkinDesignerChrome::kAllObjectsComboText),
                     juce::String("- All objects -"));
        expectEquals(SkinDesignerChrome::kClearBadgeSize, 12);
        expectEquals(SkinDesignerChrome::kSortBadgeSize, SkinDesignerChrome::kClearBadgeSize);
        expectEquals(SkinDesignerChrome::kLinkChainBadgeSize, SkinDesignerChrome::kSortBadgeSize);
        expectEquals(SkinDesignerChrome::kLegendChipGap, 4);
        expectEquals(SkinDesignerChrome::kCountBadgeDiameter, 12);
    }

    void propertyBindHoverInk()
    {
        beginTest("Property bind hover picks dark ink on light fills");

        expect(SkinDesignerChrome::propertyBindHoverNeedsDarkText(juce::Colours::white));
        expect(SkinDesignerChrome::propertyBindHoverNeedsDarkText(juce::Colour(0xfff5f0e6)));
        expect(! SkinDesignerChrome::propertyBindHoverNeedsDarkText(juce::Colours::black));
        expect(! SkinDesignerChrome::propertyBindHoverNeedsDarkText(juce::Colour(0xff1a1a1a)));
    }

    void unboundHoldNeighbourhoodGeometry()
    {
        beginTest("Unbound origin and Hold align follow Mapped / Colour set contentLeft");

        const auto cols = SkinDesignerMappingsColumns::forWidth(
            SkinDesignerChrome::kMappingsTablePaintMinWidth);
        expectEquals(cols.contentLeft(cols.mappedX),
                     cols.mappedX + SkinDesignerChrome::kMappingsColumnPad);
        expectEquals(cols.contentLeft(cols.colourSetLeft()) - 6,
                     cols.colourSetLeft() + SkinDesignerChrome::kMappingsColumnPad - 6);
        expectEquals(SkinDesignerChrome::kLinkCountBadgeWidth, 22);
        const int holdAlignX = cols.contentLeft(cols.colourSetLeft()) - 6;
        expectEquals(SkinDesignerChrome::mappingsHoldLeft(100, holdAlignX), holdAlignX);
    }

    static juce::TextButton* findButton(juce::Component& root, const juce::String& text)
    {
        for (auto* c : root.getChildren())
            if (auto* b = dynamic_cast<juce::TextButton*>(c))
                if (b->getButtonText() == text)
                    return b;
        return nullptr;
    }

    static SkinDesignerStepper* findLowerStepper(juce::Component& root)
    {
        SkinDesignerStepper* lower = nullptr;
        for (auto* c : root.getChildren())
        {
            auto* stepper = dynamic_cast<SkinDesignerStepper*>(c);
            if (stepper == nullptr)
                continue;
            if (lower == nullptr || stepper->getY() > lower->getY())
                lower = stepper;
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

    static juce::ComboBox* findUpperCombo(juce::Component& root)
    {
        juce::ComboBox* upper = nullptr;
        for (auto* c : root.getChildren())
        {
            auto* box = dynamic_cast<juce::ComboBox*>(c);
            if (box == nullptr)
                continue;
            if (upper == nullptr || box->getY() < upper->getY())
                upper = box;
        }
        return upper;
    }

    void skinsPanelDesignerActionsFitOneRow()
    {
        beginTest("SKINS Dup/New/Ren match combo; Del matches stepper");

        SkinDesignerSkinsPanel panel;
        panel.setSize(SkinDesignerChrome::kColorPanelWidth, SkinDesignerSkinsPanel::preferredHeight());

        auto* designerCombo = findLowerCombo(panel);
        auto* designerStepper = findLowerStepper(panel);
        auto* dupBtn = findButton(panel, "Dup");
        auto* newBtn = findButton(panel, "New");
        auto* renameBtn = findButton(panel, "Ren");
        auto* deleteBtn = findButton(panel, "Del");

        expect(designerCombo != nullptr);
        expect(designerStepper != nullptr);
        expect(dupBtn != nullptr);
        expect(newBtn != nullptr);
        expect(renameBtn != nullptr);
        expect(deleteBtn != nullptr);
        if (designerCombo == nullptr || designerStepper == nullptr || dupBtn == nullptr
            || newBtn == nullptr || renameBtn == nullptr || deleteBtn == nullptr)
            return;

        expectEquals(dupBtn->getY(), newBtn->getY());
        expectEquals(newBtn->getY(), renameBtn->getY());
        expectEquals(renameBtn->getY(), deleteBtn->getY());
        expectEquals(dupBtn->getX(), designerCombo->getX());
        expectEquals(renameBtn->getRight(), designerCombo->getRight());
        expectEquals(deleteBtn->getX(), designerStepper->getX());
        expectEquals(deleteBtn->getRight(), designerStepper->getRight());
        expectEquals(deleteBtn->getWidth(), SkinDesignerStepper::kPreferredWidth);
        expect(dupBtn->getY() > designerCombo->getBottom());
    }

    void skinsPanelProductDuplicateCompareFitOneRow()
    {
        beginTest("SKINS Duplicate + Compare match Product combo width");

        SkinDesignerSkinsPanel panel;
        panel.setSize(SkinDesignerChrome::kColorPanelWidth, SkinDesignerSkinsPanel::preferredHeight());

        auto* productCombo = findUpperCombo(panel);
        auto* duplicateBtn = findButton(panel, "Duplicate");
        auto* compareBtn = findButton(panel, "Compare");

        expect(productCombo != nullptr);
        expect(duplicateBtn != nullptr);
        expect(compareBtn != nullptr);
        if (productCombo == nullptr || duplicateBtn == nullptr || compareBtn == nullptr)
            return;

        expectEquals(duplicateBtn->getY(), compareBtn->getY());
        expectEquals(duplicateBtn->getX(), productCombo->getX());
        expectEquals(compareBtn->getRight(), productCombo->getRight());
        expect(compareBtn->getX() > duplicateBtn->getRight());
        expect(duplicateBtn->getHeight() > 0);
        expect(compareBtn->getHeight() > 0);
    }

    void minContentHeightStacksSkinsAndColor()
    {
        beginTest("Min content height stacks title, SKINS, COLOR, status");

        const int skinsH = SkinDesignerSkinsPanel::preferredHeight();
        const int colorH = SkinDesignerColourPicker::preferredHeightForWidth(
            SkinDesignerChrome::kColorPanelWidth);
        const int minH = SkinDesignerChrome::kTitleBarHeight
                         + skinsH + colorH + SkinDesignerChrome::kStatusBarHeight;

        expect(skinsH > 0);
        expect(colorH > 0);
        expect(minH > skinsH + colorH);
        constexpr int kFieldRowH = 24;
        constexpr int kHueBarW = 18;
        constexpr int kSvHueGap = 8;
        constexpr int kTightPad = SkinDesignerChrome::kPanelContentPad - 5;
        const int contentW = SkinDesignerChrome::kColorPanelWidth
                             - 2 * SkinDesignerChrome::kPanelContentPad;
        const int side = juce::jmax(0, contentW - kHueBarW - kSvHueGap);
        expectEquals(colorH,
                     SkinDesignerChrome::kGutter + SkinDesignerChrome::kPanelTitleHeight
                         + SkinDesignerChrome::kPanelContentPad + side
                         + SkinDesignerChrome::kPanelContentPad + kFieldRowH
                         + kTightPad + 2 * kFieldRowH
                         + kTightPad);
        expect(skinsH
               >= SkinDesignerChrome::kGutter + SkinDesignerChrome::kPanelTitleHeight
                      + SkinDesignerChrome::kGutter);
    }

    void pathElideKeepsLeafName()
    {
        beginTest("Path elide keeps leaf filename + extension visible");

        const auto font = SkinDesignerChrome::fontRegular();
        const juce::String path = "Exported: /Users/dev/very/long/nested/path/MySkin.json";
        const int fullW = SkinDesignerChrome::measureTextWidth(font, path);
        expect(fullW > 80);

        const auto shown = SkinDesignerChrome::elidePathToWidth(font, path, fullW / 2);
        expect(shown.contains("..."));
        expect(shown.endsWithIgnoreCase("MySkin.json")
               || shown.endsWithIgnoreCase(".json"));
        expectEquals(SkinDesignerChrome::elidePathToWidth(font, path, fullW + 10), path);
    }

    void lastPushBadgeAlignsWithPaletteColumn()
    {
        beginTest("LAST PUSH badge X offset matches Palette column (Color + side gap)");

        expectEquals(SkinDesignerChrome::kColorPanelWidth + SkinDesignerChrome::kPaletteSideGap,
                     260 + 12);
    }
};

static SkinDesignerChromeLayoutTests skinDesignerChromeLayoutTests;
