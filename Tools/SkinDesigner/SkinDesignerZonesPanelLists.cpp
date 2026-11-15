#include "Tools/SkinDesigner/SkinDesignerZonesPanel.h"

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinDesigner/SkinDesignerChrome.h"

namespace
{
    constexpr int kRowH = SkinDesignerChrome::kColumnHeaderHeight;
    constexpr int kHeaderRowH = SkinDesignerChrome::kColumnHeaderHeight;
    constexpr int kDivider = SkinDesignerChrome::kMappingsColumnDividerThickness;
    constexpr int kPad = SkinDesignerChrome::kMappingsColumnPad;
    constexpr int kUcsCols = 3;
    constexpr int kExceptionObjectColW = 110;
    constexpr int kUsesColW = 72;

    struct TableSlab
    {
        juce::Rectangle<int> area;
        juce::Colour fill;
        bool roundTop = false;
        bool roundBottom = false;
    };

    void fillTableSlab(juce::Graphics& g, TableSlab spec)
    {
        if (spec.area.isEmpty())
            return;
        const float r = SkinDesignerChrome::kControlCorner;
        juce::Path path;
        path.addRoundedRectangle((float) spec.area.getX(), (float) spec.area.getY(),
                                 (float) spec.area.getWidth(), (float) spec.area.getHeight(),
                                 r, r,
                                 spec.roundTop, spec.roundTop,
                                 spec.roundBottom, spec.roundBottom);
        g.setColour(spec.fill);
        g.fillPath(path);
    }

    void paintLocationPath(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& path)
    {
        juce::StringArray parts;
        int start = 0;
        while (start <= path.length())
        {
            const int sep = path.indexOf(start, " > ");
            if (sep < 0)
            {
                parts.add(path.substring(start).trim());
                break;
            }
            parts.add(path.substring(start, sep).trim());
            start = sep + 3;
        }

        int x = area.getX();
        const int arrowW = SkinDesignerChrome::kOperationArrowWidth;
        const int arrowGap = SkinDesignerChrome::kOperationArrowGap;
        g.setFont(SkinDesignerChrome::fontRegular());
        for (int p = 0; p < parts.size(); ++p)
        {
            const int textW = SkinDesignerChrome::measureTextWidth(
                SkinDesignerChrome::fontRegular(), parts[p]);
            g.setColour(SkinDesignerChrome::textTableBody());
            g.drawText(parts[p],
                       juce::Rectangle<int>(x, area.getY(), textW, area.getHeight()),
                       juce::Justification::centredLeft,
                       false);
            x += textW;
            if (p + 1 >= parts.size())
                break;
            x += arrowGap;
            SkinDesignerChrome::drawOperationNextArrow(
                g,
                { x, area.getCentreY() - 6, arrowW, 12 },
                SkinDesignerChrome::textMuted());
            x += arrowW + arrowGap;
        }
    }

    struct UcsGeom
    {
        int rows = 0;
        int colW = 0;
        int bodyH = 0;

        int colLeft(int col) const noexcept { return col * (colW + kDivider); }
    };

    UcsGeom makeUcsGeom(const std::vector<std::unique_ptr<juce::Label>>& labels) noexcept
    {
        UcsGeom g;
        const int n = static_cast<int>(labels.size());
        if (n <= 0)
            return g;
        g.rows = (n + kUcsCols - 1) / kUcsCols;
        int maxTextW = 0;
        for (const auto& label : labels)
            maxTextW = juce::jmax(maxTextW,
                                  SkinDesignerChrome::measureTextWidth(
                                      SkinDesignerChrome::fontRegular(), label->getText()));
        g.colW = juce::jmax(1, maxTextW + 2 * kPad);
        g.bodyH = g.rows * kRowH;
        return g;
    }

    juce::String initialLetter(const juce::String& name)
    {
        if (name.isEmpty())
            return "?";
        return name.substring(0, 1).toUpperCase();
    }

    juce::String ucsColumnRangeHeader(const std::vector<std::unique_ptr<juce::Label>>& labels,
                                      int col,
                                      int rows) noexcept
    {
        const int n = static_cast<int>(labels.size());
        const int first = col * rows;
        if (first >= n)
            return {};
        const int last = juce::jmin(n, (col + 1) * rows) - 1;
        const auto a = initialLetter(labels[static_cast<size_t>(first)]->getText());
        const auto b = initialLetter(labels[static_cast<size_t>(last)]->getText());
        if (a == b)
            return a;
        return a + "-" + b;
    }
}

void SkinDesignerZonesPanel::paintSharedChrome(juce::Graphics& g) const
{
    const auto geom = makeUcsGeom(ucsLabels_);
    if (geom.rows <= 0 || geom.colW <= 0)
        return;

    const auto headerFill = SkinDesignerChrome::surfaceTableHeader();
    const auto bodyFill = SkinDesignerChrome::surfaceTable();
    for (int col = 0; col < kUcsCols; ++col)
    {
        const int x = geom.colLeft(col);
        fillTableSlab(g, { { x, 0, geom.colW, kHeaderRowH }, headerFill, true, false });
        fillTableSlab(g, { { x, kHeaderRowH, geom.colW, geom.bodyH }, bodyFill, false, true });
    }
}

void SkinDesignerZonesPanel::paintExceptionsChrome(juce::Graphics& g) const
{
    const int objectW = exceptionObjectHeader_.getWidth();
    const int locationW = locationHeader_.getWidth();
    const int usesW = usesHeader_.getWidth();
    if (objectW <= 0 || locationW <= 0 || usesW <= 0)
        return;

    const int bodyH = static_cast<int>(exceptionObjectLabels_.size()) * kRowH;
    const int objectX = 0;
    const int locationX = objectW + kDivider;
    const int usesX = locationX + locationW + kDivider;
    const auto headerFill = SkinDesignerChrome::surfaceTableHeader();
    const auto bodyFill = SkinDesignerChrome::surfaceTable();

    fillTableSlab(g, { { objectX, 0, objectW, kHeaderRowH }, headerFill, true, false });
    fillTableSlab(g, { { locationX, 0, locationW, kHeaderRowH }, headerFill, true, false });
    fillTableSlab(g, { { usesX, 0, usesW, kHeaderRowH }, headerFill, true, false });
    fillTableSlab(g, { { objectX, kHeaderRowH, objectW, bodyH }, bodyFill, false, true });
    fillTableSlab(g, { { locationX, kHeaderRowH, locationW, bodyH }, bodyFill, false, true });
    fillTableSlab(g, { { usesX, kHeaderRowH, usesW, bodyH }, bodyFill, false, true });

    for (size_t i = 0; i < exceptionLocationLabels_.size() && i < TSS::kZoneExceptionRows.size(); ++i)
        paintLocationPath(g,
                          exceptionLocationLabels_[i]->getBounds().reduced(kPad, 0),
                          TSS::kZoneExceptionRows[i].where);
}

int SkinDesignerZonesPanel::layoutSharedPage(int /*contentW*/)
{
    const auto geom = makeUcsGeom(ucsLabels_);
    if (geom.rows <= 0)
        return 0;

    const juce::BorderSize<int> cellPad (0, kPad, 0, kPad);
    const int n = static_cast<int>(ucsLabels_.size());
    for (int col = 0; col < kUcsCols; ++col)
    {
        ucsColumnHeaders_[static_cast<size_t>(col)].setText(
            ucsColumnRangeHeader(ucsLabels_, col, geom.rows), juce::dontSendNotification);
        ucsColumnHeaders_[static_cast<size_t>(col)].setBounds(
            geom.colLeft(col), 0, geom.colW, kHeaderRowH);
    }

    for (int i = 0; i < n; ++i)
    {
        const int col = i / geom.rows;
        const int row = i % geom.rows;
        auto& label = *ucsLabels_[static_cast<size_t>(i)];
        label.setBounds(geom.colLeft(col), kHeaderRowH + row * kRowH, geom.colW, kRowH);
        label.setBorderSize(cellPad);
    }
    return kHeaderRowH + geom.bodyH;
}

int SkinDesignerZonesPanel::layoutExceptionsPage(int contentW)
{
    const int objectW = kExceptionObjectColW;
    const int usesW = kUsesColW;
    const int locationW = juce::jmax(120, contentW - objectW - usesW - 2 * kDivider);
    const juce::BorderSize<int> cellPad (0, kPad, 0, kPad);

    exceptionObjectHeader_.setBounds(0, 0, objectW, kHeaderRowH);
    locationHeader_.setBounds(objectW + kDivider, 0, locationW, kHeaderRowH);
    usesHeader_.setBounds(objectW + kDivider + locationW + kDivider, 0, usesW, kHeaderRowH);

    int y = kHeaderRowH;
    for (size_t i = 0; i < exceptionObjectLabels_.size(); ++i)
    {
        exceptionObjectLabels_[i]->setBounds(0, y, objectW, kRowH);
        exceptionObjectLabels_[i]->setBorderSize(cellPad);
        exceptionLocationLabels_[i]->setBounds(objectW + kDivider, y, locationW, kRowH);
        exceptionUsesLabels_[i]->setBounds(objectW + kDivider + locationW + kDivider, y, usesW, kRowH);
        exceptionUsesLabels_[i]->setBorderSize(cellPad);
        y += kRowH;
    }
    return y;
}
