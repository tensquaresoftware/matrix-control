#include "Tools/SkinEditor/SkinEditorPalettePanel.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"

namespace
{
    void paintElidedLeft(juce::Graphics& g,
                         const juce::Font& font,
                         const juce::String& text,
                         juce::Rectangle<int> bounds)
    {
        g.setFont(font);
        g.drawText(SkinEditorChrome::elideToWidth(font, text, bounds.getWidth()),
                   bounds, juce::Justification::centredLeft);
    }
}

void SkinEditorPalettePanel::attachScrollBarListener()
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

void SkinEditorPalettePanel::layoutStickyHeadersOverlay()
{
    stickyHeaders_.setBounds(list_.getX(), list_.getY(), paintWidth(), list_.getHeight());
    stickyHeaders_.toFront(false);
}

void SkinEditorPalettePanel::scrollBarMoved(juce::ScrollBar*, double)
{
    clearMappingCountHover();
    stickyHeaders_.repaint();
}

int SkinEditorPalettePanel::stickyHeaderYInView(int sectionIndex, int scrollY) const noexcept
{
    if (sectionIndex < 0 || sectionIndex >= static_cast<int>(stickySections_.size()))
        return 0;
    const int naturalY = stickySections_[static_cast<size_t>(sectionIndex)].upperListRow
                         * list_.getRowHeight();
    // Keep the inter-group list gap (kTableGapHeight under the last Product colour)
    // between stacked sticky titles so Editor does not butt against Product.
    const int assignedTop = sectionIndex
                            * (SkinEditorChrome::kColumnHeaderHeight
                               + SkinEditorChrome::kTableGapHeight);
    return juce::jmax(assignedTop, naturalY - scrollY);
}

void SkinEditorPalettePanel::paintStickySectionHeaders(juce::Graphics& g)
{
    if (stickySections_.empty())
        return;
    auto* vp = list_.getViewport();
    if (vp == nullptr)
        return;

    const int headerH = SkinEditorChrome::kColumnHeaderHeight;
    const int gapH = SkinEditorChrome::kTableGapHeight;
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
            g.setColour(SkinEditorChrome::surfacePanel());
            g.fillRect(0, assignedTop - gapH, paintW, gapH);
        }

        const auto rowArea = juce::Rectangle<float>(0.0f, (float) yInView,
                                                    (float) paintW, (float) headerH);
        SkinEditorChrome::fillTableRow(g, { rowArea, SkinEditorChrome::surfaceSection(),
                                            true, true });
        g.setColour(SkinEditorChrome::textTableHeader());
        paintElidedLeft(g, SkinEditorChrome::fontBold(),
                        stickySections_[static_cast<size_t>(i)].title,
                        { 6, yInView, juce::jmax(0, paintW - 12), headerH });
    }
}

bool SkinEditorPalettePanel::stickyHeaderHitTest(int x, int y) const noexcept
{
    if (x < 0 || x >= paintWidth() || stickySections_.empty())
        return false;
    auto* vp = list_.getViewport();
    if (vp == nullptr)
        return false;

    const int headerH = SkinEditorChrome::kColumnHeaderHeight;
    const int gapH = SkinEditorChrome::kTableGapHeight;
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
