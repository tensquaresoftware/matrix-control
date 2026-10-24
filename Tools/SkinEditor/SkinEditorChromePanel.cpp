#include "Tools/SkinEditor/SkinEditorChromePanel.h"

namespace SkinEditorChrome
{
    PanelChrome layoutPanel(juce::Rectangle<int> bounds, int tabWidth)
    {
        bounds.removeFromBottom(kGutter);
        bounds.removeFromTop(kGutter);
        auto titleRow = bounds.removeFromTop(kPanelTitleHeight);
        bounds.removeFromTop(kPanelContentPad);
        bounds.removeFromBottom(kPanelContentPad);
        const int w = juce::jlimit(1, titleRow.getWidth(), tabWidth);
        auto tab = titleRow.removeFromLeft(w); // flush left — no side offset
        bounds.removeFromLeft(kPanelContentPad);
        bounds.removeFromRight(kPanelContentPad);
        return { tab, bounds };
    }

    void paintPanelFrame(juce::Graphics& g,
                         juce::Rectangle<int> panelBounds,
                         juce::Rectangle<int> tab)
    {
        const float r = kControlCorner;
        const float left = (float) panelBounds.getX();
        const float right = (float) panelBounds.getRight();
        const float top = (float) (panelBounds.getY() + kGutter);
        const float bottom = (float) (panelBounds.getBottom() - kGutter);
        // Same rhythm as table section tabs: rail top (= body shoulder) lines up with
        // the title baseline, not the full title-band bottom.
        const float titleBottom = top + (float) kPanelTitleHeight
                                  - (float) kTableSectionRailHeight;
        const float drop = titleBottom - top; // 45° run equals drop
        const float tabRight = juce::jmax(left + r * 2.0f,
                                          (float) tab.getRight() - kTabBevelStartEarly);
        const float bevelEndX = tabRight + drop;

        juce::Path silhouette;
        silhouette.startNewSubPath(left + r, top);
        silhouette.lineTo(tabRight - r, top);
        silhouette.quadraticTo(tabRight, top, tabRight + r, top + r);             // into 45°
        if (bevelEndX + r < right)
        {
            silhouette.lineTo(bevelEndX - r, titleBottom - r);                    // 45° mid-run
            silhouette.quadraticTo(bevelEndX, titleBottom, bevelEndX + r, titleBottom);
        }
        else
        {
            silhouette.lineTo(tabRight, titleBottom);
        }
        silhouette.lineTo(right - r, titleBottom);
        silhouette.quadraticTo(right, titleBottom, right, titleBottom + r);       // body top-right
        silhouette.lineTo(right, bottom - r);
        silhouette.quadraticTo(right, bottom, right - r, bottom);
        silhouette.lineTo(left + r, bottom);
        silhouette.quadraticTo(left, bottom, left, bottom - r);
        silhouette.lineTo(left, top + r);
        silhouette.quadraticTo(left, top, left + r, top);                         // tab top-left
        silhouette.closeSubPath();

        g.setColour(surfacePanel());
        g.fillPath(silhouette);
    }

    void paintRoundedWell(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour fill)
    {
        g.setColour(fill);
        g.fillRoundedRectangle(area.toFloat(), kControlCorner);
    }
}
