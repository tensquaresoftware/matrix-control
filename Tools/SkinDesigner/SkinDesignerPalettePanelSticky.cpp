#include "Tools/SkinDesigner/SkinDesignerPalettePanel.h"

#include "Tools/SkinDesigner/SkinDesignerChrome.h"

namespace
{
    void paintElidedLeft(juce::Graphics& g,
                         const juce::Font& font,
                         const juce::String& text,
                         juce::Rectangle<int> bounds)
    {
        g.setFont(font);
        g.drawText(SkinDesignerChrome::elideToWidth(font, text, bounds.getWidth()),
                   bounds, juce::Justification::centredLeft);
    }
}

void SkinDesignerPalettePanel::attachScrollBarListener()
{
    auto* vp = list_.getViewport();
    auto* bar = (vp != nullptr) ? &vp->getVerticalScrollBar() : nullptr;
    if (bar == listenedScrollBar_)
        return;
    if (listenedScrollBar_ != nullptr)
        listenedScrollBar_->removeListener(this);
    listenedScrollBar_ = bar;
    if (listenedScrollBar_ != nullptr)
        listenedScrollBar_->addListener(this);
}

void SkinDesignerPalettePanel::layoutStickyHeadersOverlay()
{
    stickyHeaders_.setBounds(list_.getX(), list_.getY(), paintWidth(), list_.getHeight());
    stickyHeaders_.toFront(false);
}

void SkinDesignerPalettePanel::scrollBarMoved(juce::ScrollBar*, double)
{
    refreshMappingCountHoverFromPointer();
    stickyHeaders_.repaint();
}

int SkinDesignerPalettePanel::stickyHeaderYInView(int sectionIndex, int scrollY) const noexcept
{
    if (sectionIndex < 0 || sectionIndex >= static_cast<int>(stickySections_.size()))
        return 0;
    const int naturalY = stickySections_[static_cast<size_t>(sectionIndex)].upperListRow
                         * list_.getRowHeight();
    // Keep inter-group list gaps between stacked sticky titles when several exist.
    const int assignedTop = sectionIndex
                            * (SkinDesignerChrome::kColumnHeaderHeight
                               + SkinDesignerChrome::kTableGapHeight);
    return juce::jmax(assignedTop, naturalY - scrollY);
}

void SkinDesignerPalettePanel::paintStickySectionHeaders(juce::Graphics& g)
{
    if (stickySections_.empty())
        return;
    auto* vp = list_.getViewport();
    if (vp == nullptr)
        return;

    const int headerH = SkinDesignerChrome::kColumnHeaderHeight;
    const int gapH = SkinDesignerChrome::kTableGapHeight;
    const int stackStride = headerH + gapH;
    const int paintW = paintWidth();
    const int scrollY = vp->getViewPositionY();
    const int viewH = stickyHeaders_.getHeight();

    for (int i = 0; i < static_cast<int>(stickySections_.size()); ++i)
    {
        const int yInView = stickyHeaderYInView(i, scrollY);
        if (yInView >= viewH)
            break;
        if (yInView + headerH <= 0)
            continue;

        // Opaque spacer when this title is stacked sticky — hides rows scrolling underneath.
        const int assignedTop = i * stackStride;
        if (i > 0 && yInView <= assignedTop)
        {
            g.setColour(SkinDesignerChrome::surfacePanel());
            g.fillRect(0, assignedTop - gapH, paintW, gapH);
        }

        const auto rowArea = juce::Rectangle<float>(0.0f, (float) yInView,
                                                    (float) paintW, (float) headerH);
        SkinDesignerChrome::fillTableRow(g, { rowArea, SkinDesignerChrome::surfaceSection(),
                                            true, true });
        g.setColour(SkinDesignerChrome::textTableHeader());
        paintElidedLeft(g, SkinDesignerChrome::fontBold(),
                        stickySections_[static_cast<size_t>(i)].title,
                        { 6, yInView, juce::jmax(0, paintW - 12), headerH });
    }
}

bool SkinDesignerPalettePanel::stickyHeaderHitTest(int x, int y) const noexcept
{
    if (x < 0 || x >= paintWidth() || stickySections_.empty())
        return false;
    auto* vp = list_.getViewport();
    if (vp == nullptr)
        return false;

    const int headerH = SkinDesignerChrome::kColumnHeaderHeight;
    const int gapH = SkinDesignerChrome::kTableGapHeight;
    const int stackStride = headerH + gapH;
    const int scrollY = vp->getViewPositionY();
    for (int i = 0; i < static_cast<int>(stickySections_.size()); ++i)
    {
        const int yInView = stickyHeaderYInView(i, scrollY);
        if (y >= yInView && y < yInView + headerH)
            return true;

        const int assignedTop = i * stackStride;
        if (i > 0 && yInView <= assignedTop
            && y >= assignedTop - gapH && y < assignedTop)
            return true;
    }
    return false;
}
