#include "Tools/SkinDesigner/SkinDesignerZonesPanel.h"

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinDesigner/SkinDesignerChrome.h"

namespace
{
    constexpr int kDivider = SkinDesignerChrome::kMappingsColumnDividerThickness;
    constexpr int kPad = SkinDesignerChrome::kMappingsColumnPad;
    constexpr int kZoneContentW = 64;
    constexpr int kZoneColW = kZoneContentW + kDivider;
    constexpr int kComboH = 18;
    /** Same air above first combo, between combos, and below last combo (and L/R). */
    constexpr int kGap = kPad;
    constexpr int kHeaderRowH = SkinDesignerChrome::kColumnHeaderHeight;
    constexpr int kZonesBarH = SkinDesignerChrome::kColumnHeaderHeight;
    constexpr int kRowGap = kDivider;
    constexpr int kTableCols = 1 + static_cast<int>(TSS::kAllSkinZones.size());
    constexpr int kMinObjectContentW = 110;

    struct HeaderYs
    {
        int skin = 0;
        int zones = 0;
        int columns = 0;
        int body = 0;
    };

    constexpr int kSkinRowH = SkinDesignerChrome::kButtonHeight;
    /** One content-line of air under Skin + Reset before the table. */
    constexpr int kBelowSkinGap = kHeaderRowH;

    HeaderYs headerYs() noexcept
    {
        constexpr int skin = 0;
        const int zones = skin + kSkinRowH + kBelowSkinGap;
        const int columns = zones + kZonesBarH + kRowGap;
        return { skin, zones, columns, columns + kHeaderRowH };
    }

    int bodyHeight() noexcept
    {
        const int n = static_cast<int>(TSS::kMultiSetSkinObjects.size());
        return n * kComboH + (n + 1) * kGap;
    }

    int rowComboY(int bodyY, int row) noexcept
    {
        return bodyY + kGap + row * (kComboH + kGap);
    }

    int objectColumnWidth() noexcept
    {
        const auto& bold = SkinDesignerChrome::fontBold();
        const auto& body = SkinDesignerChrome::fontRegular();
        const int objectsW = SkinDesignerChrome::measureTextWidth(bold, "Objects");
        int rowW = 0;
        for (const auto object : TSS::kMultiSetSkinObjects)
            rowW = juce::jmax(rowW,
                              SkinDesignerChrome::measureTextWidth(
                                  body, TSS::DevSkin::toString(object)));
        const int content = juce::jmax(kMinObjectContentW,
                                       juce::jmax(objectsW, rowW) + 2 * kPad);
        return content + kDivider;
    }

    int tableWidth(int objectColW) noexcept
    {
        return objectColW + static_cast<int>(TSS::kAllSkinZones.size()) * kZoneColW - kDivider;
    }

    int columnLeft(int objectColW, int col) noexcept
    {
        if (col <= 0)
            return 0;
        return objectColW + (col - 1) * kZoneColW;
    }

    int columnContentWidth(int objectColW, int col) noexcept
    {
        const int full = (col == 0) ? objectColW : kZoneColW;
        return juce::jmax(0, full - kDivider);
    }

    juce::Rectangle<int> columnSlab(int objectColW, int col, int y, int h) noexcept
    {
        return { columnLeft(objectColW, col), y, columnContentWidth(objectColW, col), h };
    }

    juce::Rectangle<int> zonesSpanSlab(int objectColW, int y, int h) noexcept
    {
        const int left = columnLeft(objectColW, 1);
        return { left, y, tableWidth(objectColW) - left, h };
    }

    struct SlabFill
    {
        juce::Rectangle<float> area;
        juce::Colour fill;
        bool roundTop = false;
        bool roundBottom = false;
    };

    void fillRoundedSlab(juce::Graphics& g, SlabFill spec)
    {
        if (spec.area.getWidth() <= 0.0f || spec.area.getHeight() <= 0.0f)
            return;
        const float r = SkinDesignerChrome::kControlCorner;
        juce::Path path;
        path.addRoundedRectangle(spec.area.getX(),
                                 spec.area.getY(),
                                 spec.area.getWidth(),
                                 spec.area.getHeight(),
                                 r,
                                 r,
                                 spec.roundTop,
                                 spec.roundTop,
                                 spec.roundBottom,
                                 spec.roundBottom);
        g.setColour(spec.fill);
        g.fillPath(path);
    }

    void fillZoneHeaderSlabs(juce::Graphics& g, int objectColW, int y, int h)
    {
        for (int col = 1; col < kTableCols; ++col)
            fillRoundedSlab(g, { columnSlab(objectColW, col, y, h).toFloat(),
                                 SkinDesignerChrome::surfaceTableHeader(), true, false });
    }

    void fillBodySlabs(juce::Graphics& g, int objectColW, int y, int h)
    {
        for (int col = 0; col < kTableCols; ++col)
            fillRoundedSlab(g, { columnSlab(objectColW, col, y, h).toFloat(),
                                 SkinDesignerChrome::surfaceTable(), false, true });
    }

    void styleHeaderLabel(juce::Label& label, const juce::String& text, bool leftAlign)
    {
        label.setText(text, juce::dontSendNotification);
        label.setFont(SkinDesignerChrome::fontBold());
        label.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        label.setColour(juce::Label::textColourId, SkinDesignerChrome::textTableHeader());
        label.setJustificationType(leftAlign ? juce::Justification::centredLeft
                                             : juce::Justification::centred);
        SkinDesignerChrome::preventTextSquish(label);
    }

    juce::Rectangle<int> comboBoundsInCell(juce::Rectangle<int> cell) noexcept
    {
        cell.setX(cell.getX() + kPad);
        cell.setWidth(juce::jmax(0, cell.getWidth() - 2 * kPad));
        return cell;
    }

    struct SkinRowLayout
    {
        juce::Rectangle<int> prefix;
        juce::Rectangle<int> badge;
        juce::Rectangle<int> reset;
    };

    SkinRowLayout layoutSkinRow(int tableW, int y, int resetW, const juce::String& skinName) noexcept
    {
        const int resetX = juce::jmax(0, tableW - resetW);
        const int prefixW = SkinDesignerChrome::measureTextWidth(
                                SkinDesignerChrome::fontRegular(), "Skin:")
                          + kPad;
        // Same height and side pad as matrix combos.
        const int badgeH = kComboH;
        const int badgeWWanted = SkinDesignerChrome::measureTextWidth(
                                     SkinDesignerChrome::fontBold(), skinName)
                               + 2 * kPad;
        // Keep Reset hittable when the skin display name is very long.
        const int badgeW = juce::jmin(badgeWWanted, juce::jmax(0, resetX - prefixW));
        return { { 0, y, prefixW, kSkinRowH },
                 { prefixW, y + (kSkinRowH - badgeH) / 2, badgeW, badgeH },
                 { resetX, y, resetW, kSkinRowH } };
    }
}

void SkinDesignerZonesPanel::Content::paint(juce::Graphics& g)
{
    switch (owner_.subPage_)
    {
        case SubPage::kMatrix:     owner_.paintMatrixChrome(g); break;
        case SubPage::kShared:     owner_.paintSharedChrome(g); break;
        case SubPage::kExceptions: owner_.paintExceptionsChrome(g); break;
    }
}

void SkinDesignerZonesPanel::paintMatrixChrome(juce::Graphics& g) const
{
    const int objectColW = objectColumnWidth();
    const auto ys = headerYs();

    if (! skinNameBadgeBounds_.isEmpty() && skinNameBadgeText_.isNotEmpty())
    {
        g.setColour(SkinDesignerChrome::textMuted());
        g.fillRoundedRectangle(skinNameBadgeBounds_.toFloat(),
                               SkinDesignerChrome::kControlCorner);
        g.setColour(SkinDesignerChrome::surfacePanel());
        g.setFont(SkinDesignerChrome::fontBold());
        g.drawText(skinNameBadgeText_,
                   skinNameBadgeBounds_,
                   juce::Justification::centred,
                   false);
    }

    // Zones: Patch…Modales only (#151515), same height as Colour Mappings Property/State row.
    fillRoundedSlab(g, { zonesSpanSlab(objectColW, ys.zones, kZonesBarH).toFloat(),
                         SkinDesignerChrome::surfaceSection(), true, true });

    // Column headers: Objects folder-tab + zone name slabs (#424242).
    SkinDesignerChrome::paintTableSectionHeader(
        g,
        columnSlab(objectColW, 0, ys.columns, kHeaderRowH).toFloat(),
        "Objects");
    fillZoneHeaderSlabs(g, objectColW, ys.columns, kHeaderRowH);

    fillBodySlabs(g, objectColW, ys.body, bodyHeight());
}

void SkinDesignerZonesPanel::buildMatrixHeaders()
{
    skinHeader_.setText("Skin:", juce::dontSendNotification);
    skinHeader_.setFont(SkinDesignerChrome::fontRegular());
    skinHeader_.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    skinHeader_.setColour(juce::Label::textColourId, SkinDesignerChrome::textTableHeader());
    skinHeader_.setJustificationType(juce::Justification::centredLeft);
    SkinDesignerChrome::preventTextSquish(skinHeader_);
    styleHeaderLabel(zonesGroupHeader_, "Zones", false);
    content_.addAndMakeVisible(skinHeader_);
    content_.addAndMakeVisible(zonesGroupHeader_);
    for (size_t i = 0; i < TSS::kAllSkinZones.size(); ++i)
    {
        styleHeaderLabel(zoneHeaders_[i], TSS::DevSkin::toString(TSS::kAllSkinZones[i]), false);
        content_.addAndMakeVisible(zoneHeaders_[i]);
    }
    for (size_t i = 0; i < TSS::kMultiSetSkinObjects.size(); ++i)
    {
        rowLabels_[i].setText(TSS::DevSkin::toString(TSS::kMultiSetSkinObjects[i]),
                              juce::dontSendNotification);
        rowLabels_[i].setFont(SkinDesignerChrome::fontRegular());
        rowLabels_[i].setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        rowLabels_[i].setColour(juce::Label::textColourId, SkinDesignerChrome::textTableBody());
        rowLabels_[i].setJustificationType(juce::Justification::centredLeft);
        SkinDesignerChrome::preventTextSquish(rowLabels_[i]);
        rowLabels_[i].addMouseListener(this, false);
        content_.addAndMakeVisible(rowLabels_[i]);
    }
}

int SkinDesignerZonesPanel::layoutMatrixPage(int /*contentW*/)
{
    skinNameBadgeText_ = activeSkinDisplayName();
    const int objectColW = objectColumnWidth();
    const int tableW = tableWidth(objectColW);
    const auto ys = headerYs();

    const auto skinRow = layoutSkinRow(tableW,
                                       ys.skin,
                                       SkinDesignerChrome::measureButtonWidth(resetButton_),
                                       skinNameBadgeText_);
    skinHeader_.setBounds(skinRow.prefix);
    skinNameBadgeBounds_ = skinRow.badge;
    resetButton_.setBounds(skinRow.reset);
    zonesGroupHeader_.setBounds(zonesSpanSlab(objectColW, ys.zones, kZonesBarH));
    for (size_t i = 0; i < TSS::kAllSkinZones.size(); ++i)
    {
        auto cell = columnSlab(objectColW, static_cast<int>(i) + 1, ys.columns, kHeaderRowH);
        zoneHeaders_[i].setBounds(cell);
    }

    size_t dashIndex = 0;
    size_t usedIndex = 0;
    for (size_t row = 0; row < TSS::kMultiSetSkinObjects.size(); ++row)
    {
        const auto object = TSS::kMultiSetSkinObjects[row];
        const int comboY = rowComboY(ys.body, static_cast<int>(row));
        auto objectCell = columnSlab(objectColW, 0, comboY, kComboH);
        rowLabels_[row].setBounds(objectCell.reduced(kPad, 0));

        for (size_t col = 0; col < TSS::kAllSkinZones.size(); ++col)
        {
            const auto zone = TSS::kAllSkinZones[col];
            auto cell = columnSlab(objectColW, static_cast<int>(col) + 1, comboY, kComboH);
            if (! TSS::isZoneUsed(object, zone))
            {
                if (dashIndex < dashLabels_.size())
                {
                    dashLabels_[dashIndex]->setJustificationType(juce::Justification::centred);
                    dashLabels_[dashIndex++]->setBounds(cell);
                }
                continue;
            }
            if (usedIndex < usedCells_.size() && usedCells_[usedIndex].combo != nullptr)
                usedCells_[usedIndex++].combo->setBounds(comboBoundsInCell(cell));
        }
    }

    return ys.body + bodyHeight();
}
