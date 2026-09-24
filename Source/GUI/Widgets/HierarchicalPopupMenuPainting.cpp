// Extracted from HierarchicalPopupMenu.cpp for modular maintenance.
// Panel borders and column painting for the hierarchical popup.

#include "HierarchicalPopupMenu.h"

#include "HierarchicalComboBox.h"
#include "GUI/Layout/ScaledDrawing.h"

namespace TSS
{
    void HierarchicalPopupMenu::drawPanelBorderEdges(juce::Graphics& g,
                                                     juce::Rectangle<float> panel,
                                                     float thickness,
                                                     PanelBorderEdgeFlags edges) const
    {
        if (edges.top)
            g.fillRect(panel.getX(), panel.getY(), panel.getWidth(), thickness);

        if (edges.bottom)
            g.fillRect(panel.getX(), panel.getBottom() - thickness, panel.getWidth(), thickness);

        if (edges.left)
            g.fillRect(panel.getX(), panel.getY(), thickness, panel.getHeight());

        if (edges.right)
            g.fillRect(panel.getRight() - thickness, panel.getY(), thickness, panel.getHeight());
    }

    void HierarchicalPopupMenu::drawAlignedPanelBorders(juce::Graphics& g,
                                                        juce::Rectangle<float> primaryPanel,
                                                        juce::Rectangle<float> secondaryPanel,
                                                        float thickness) const
    {
        // Shared edge: omit primary right / secondary left, then stroke the join once.
        drawPanelBorderEdges(g, primaryPanel, thickness, { true, true, false, true });
        drawPanelBorderEdges(g, secondaryPanel, thickness, { false, true, true, true });

        const float sharedX = primaryPanel.getRight() - thickness;
        const float joinTop = juce::jmax(primaryPanel.getY(), secondaryPanel.getY());
        const float joinBottom = juce::jmin(primaryPanel.getBottom(), secondaryPanel.getBottom());
        if (joinBottom > joinTop)
            g.fillRect(sharedX, joinTop, thickness, joinBottom - joinTop);

        // Cap any protruding vertical segment at the shared edge outside the secondary.
        if (secondaryPanel.getY() > primaryPanel.getY())
            g.fillRect(sharedX, primaryPanel.getY(), thickness, secondaryPanel.getY() - primaryPanel.getY());

        if (secondaryPanel.getBottom() < primaryPanel.getBottom())
            g.fillRect(sharedX,
                       secondaryPanel.getBottom(),
                       thickness,
                       primaryPanel.getBottom() - secondaryPanel.getBottom());

        if (primaryPanel.getY() > secondaryPanel.getY())
            g.fillRect(sharedX, secondaryPanel.getY(), thickness, primaryPanel.getY() - secondaryPanel.getY());

        if (primaryPanel.getBottom() < secondaryPanel.getBottom())
            g.fillRect(sharedX,
                       primaryPanel.getBottom(),
                       thickness,
                       secondaryPanel.getBottom() - primaryPanel.getBottom());

        // Close secondary left edge above/below shared join (already covered by fillRect join when aligned).
        // Re-draw secondary left only where it does not overlap primary height.
        if (secondaryPanel.getY() < primaryPanel.getY())
            g.fillRect(secondaryPanel.getX(),
                       secondaryPanel.getY(),
                       thickness,
                       primaryPanel.getY() - secondaryPanel.getY());

        if (secondaryPanel.getBottom() > primaryPanel.getBottom())
            g.fillRect(secondaryPanel.getX(),
                       primaryPanel.getBottom(),
                       thickness,
                       secondaryPanel.getBottom() - primaryPanel.getBottom());
    }

    void HierarchicalPopupMenu::paintPrimaryColumn(juce::Graphics& g,
                                                   juce::Rectangle<float> primaryPanel,
                                                   juce::Rectangle<float> primaryContent) const
    {
        renderer_.drawBackground(g, primaryPanel);

        juce::Graphics::ScopedSaveState clipState(g);
        g.reduceClipRegion(primaryContent.toNearestInt());

        for (int primaryIndex = 0; primaryIndex < getOpenablePrimaryCount(); ++primaryIndex)
        {
            const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, primaryIndex);
            if (storageIndex < 0)
                continue;

            const auto& primary = owner_.getPrimaryItem(storageIndex);
            const auto itemBounds = getPrimaryItemBounds(primaryIndex);
            if (! itemBounds.intersects(primaryContent))
                continue;

            const bool isHighlighted = primaryIndex == highlightedPrimaryIndex_;
            renderer_.drawLabelItem(g, {
                .text = primary.label,
                .itemBounds = itemBounds,
                .isHighlighted = isHighlighted,
                .isEnabled = true,
                .font = &cachedFont_,
                .systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this)});

            if (! primary.children.empty())
                renderer_.drawSubMenuChevron(g, itemBounds, cachedFont_);
        }
    }

    void HierarchicalPopupMenu::paintSecondaryColumn(juce::Graphics& g,
                                                     juce::Rectangle<float> primaryPanel,
                                                     juce::Rectangle<float> secondaryPanel) const
    {
        const auto secondaryContent = getSecondaryContentBounds();
        const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, highlightedPrimaryIndex_);
        const auto& primary = owner_.getPrimaryItem(storageIndex);
        const auto popupLook = owner_.getPopupMenuLook();

        renderer_.drawBackground(g, secondaryPanel);

        {
            juce::Graphics::ScopedSaveState clipState(g);
            g.reduceClipRegion(secondaryContent.toNearestInt());

            for (size_t childIndex = 0; childIndex < primary.children.size(); ++childIndex)
            {
                const auto itemBounds = getSecondaryItemBounds(static_cast<int>(childIndex));
                if (! itemBounds.intersects(secondaryContent))
                    continue;

                const bool isHighlighted = highlightedChildIndex_ == static_cast<int>(childIndex);
                renderer_.drawLabelItem(g, {
                    .text = primary.children[childIndex].label,
                    .itemBounds = itemBounds,
                    .isHighlighted = isHighlighted,
                    .isEnabled = true,
                    .font = &cachedFont_,
                    .systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this)});
            }
        }

        g.setColour(popupLook.border);
        drawAlignedPanelBorders(g, primaryPanel, secondaryPanel, getLayoutBorderThickness());
    }

    void HierarchicalPopupMenu::paint(juce::Graphics& g)
    {
        const auto primaryPanel = getPrimaryPanelBounds();
        const auto primaryContent = getPrimaryContentBounds();

        paintPrimaryColumn(g, primaryPanel, primaryContent);

        if (hasSecondaryColumn())
        {
            paintSecondaryColumn(g, primaryPanel, getSecondaryPanelBounds());
            return;
        }

        g.setColour(owner_.getPopupMenuLook().border);
        drawPanelBorderEdges(g, primaryPanel, getLayoutBorderThickness(), {});
    }
}
