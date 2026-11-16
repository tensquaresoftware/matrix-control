#include "Tools/SkinDesigner/SkinDesignerChromePanel.h"

namespace SkinDesignerChrome
{
    namespace
    {
        struct PanelBand
        {
            float left = 0.0f;
            float right = 0.0f;
            float top = 0.0f;
            float bottom = 0.0f;
            float titleBottom = 0.0f;
            float r = 0.0f;
            float drop = 0.0f;
        };

        PanelBand makeBand(juce::Rectangle<int> panelBounds, int bottomGutter) noexcept
        {
            PanelBand band;
            band.r = kControlCorner;
            band.left = (float) panelBounds.getX();
            band.right = (float) panelBounds.getRight();
            band.top = (float) (panelBounds.getY() + kGutter);
            band.bottom = (float) (panelBounds.getBottom() - juce::jmax(0, bottomGutter));
            // Same rhythm as table section tabs: rail top (= body shoulder) lines up with
            // the title baseline, not the full title-band bottom.
            band.titleBottom = band.top + (float) kPanelTitleHeight
                               - (float) kTableSectionRailHeight;
            band.drop = band.titleBottom - band.top; // 45° run equals drop
            return band;
        }

        float tabBevelRight(const juce::Rectangle<int>& tab, float floorLeft, float r) noexcept
        {
            return juce::jmax(floorLeft + r * 2.0f, (float) tab.getRight() - kTabBevelStartEarly);
        }

        void appendBevelAndBody(juce::Path& silhouette,
                                const PanelBand& band,
                                float tabRight) noexcept
        {
            const float bevelEndX = tabRight + band.drop;
            silhouette.lineTo(tabRight - band.r, band.top);
            silhouette.quadraticTo(tabRight, band.top, tabRight + band.r, band.top + band.r);
            if (bevelEndX + band.r < band.right)
            {
                silhouette.lineTo(bevelEndX - band.r, band.titleBottom - band.r);
                silhouette.quadraticTo(bevelEndX, band.titleBottom, bevelEndX + band.r,
                                       band.titleBottom);
            }
            else
            {
                silhouette.lineTo(tabRight, band.titleBottom);
            }

            silhouette.lineTo(band.right - band.r, band.titleBottom);
            silhouette.quadraticTo(band.right, band.titleBottom, band.right,
                                   band.titleBottom + band.r);
            silhouette.lineTo(band.right, band.bottom - band.r);
            silhouette.quadraticTo(band.right, band.bottom, band.right - band.r, band.bottom);
            silhouette.lineTo(band.left + band.r, band.bottom);
            silhouette.quadraticTo(band.left, band.bottom, band.left, band.bottom - band.r);
        }

        void paintInactiveTabLedge(juce::Graphics& g,
                                   const PanelBand& band,
                                   juce::Rectangle<int> tab)
        {
            const float tabLeft = (float) tab.getX();
            const float tabRight = tabBevelRight(tab, tabLeft, band.r);
            const float bevelEndX = tabRight + band.drop;

            juce::Path ledge;
            ledge.startNewSubPath(tabLeft + band.r, band.top);
            ledge.lineTo(tabRight - band.r, band.top);
            // Same 45° shoulder as the active tab notch (smooth landing to the right).
            ledge.quadraticTo(tabRight, band.top, tabRight + band.r, band.top + band.r);
            if (bevelEndX + band.r < band.right)
            {
                ledge.lineTo(bevelEndX - band.r, band.titleBottom - band.r);
                ledge.quadraticTo(bevelEndX, band.titleBottom, bevelEndX + band.r,
                                  band.titleBottom);
            }
            else
            {
                ledge.lineTo(tabRight, band.titleBottom);
            }
            // Sharp bottom-left: keep the shoulder as a continuous horizontal line.
            ledge.lineTo(tabLeft, band.titleBottom);
            ledge.lineTo(tabLeft, band.top + band.r);
            ledge.quadraticTo(tabLeft, band.top, tabLeft + band.r, band.top);
            ledge.closeSubPath();

            g.setColour(surfacePanel());
            g.fillPath(ledge);
        }
    }

    PanelChrome layoutPanel(juce::Rectangle<int> bounds, int tabWidth, int bottomGutter)
    {
        bounds.removeFromBottom(juce::jmax(0, bottomGutter));
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
                         juce::Rectangle<int> tab,
                         int bottomGutter)
    {
        const auto band = makeBand(panelBounds, bottomGutter);
        const float tabRight = tabBevelRight(tab, band.left, band.r);

        juce::Path silhouette;
        silhouette.startNewSubPath(band.left + band.r, band.top);
        appendBevelAndBody(silhouette, band, tabRight);
        silhouette.lineTo(band.left, band.top + band.r);
        silhouette.quadraticTo(band.left, band.top, band.left + band.r, band.top);
        silhouette.closeSubPath();

        g.setColour(surfacePanel());
        g.fillPath(silhouette);
    }

    void paintWorkSurfacePanelFrame(juce::Graphics& g, const WorkSurfaceFrame& frame)
    {
        const auto band = makeBand(frame.panelBounds, frame.bottomGutter);
        const auto& activeTab = frame.zonesActive ? frame.zonesTab : frame.mappingsTab;
        const auto& inactiveTab = frame.zonesActive ? frame.mappingsTab : frame.zonesTab;
        const float activeRight = tabBevelRight(
            activeTab,
            frame.zonesActive ? (float) activeTab.getX() : band.left,
            band.r);

        juce::Path silhouette;
        if (! frame.zonesActive)
        {
            silhouette.startNewSubPath(band.left + band.r, band.top);
            appendBevelAndBody(silhouette, band, activeRight);
            silhouette.lineTo(band.left, band.top + band.r);
            silhouette.quadraticTo(band.left, band.top, band.left + band.r, band.top);
        }
        else
        {
            const float tabLeft = (float) activeTab.getX();
            silhouette.startNewSubPath(band.left + band.r, band.titleBottom);
            // Sharp rise into ZONE MAPPINGS: do not round the shoulder into the tab.
            silhouette.lineTo(tabLeft, band.titleBottom);
            silhouette.lineTo(tabLeft, band.top + band.r);
            silhouette.quadraticTo(tabLeft, band.top, tabLeft + band.r, band.top);
            appendBevelAndBody(silhouette, band, activeRight);
            silhouette.lineTo(band.left, band.titleBottom);
        }
        silhouette.closeSubPath();

        g.setColour(surfacePanel());
        g.fillPath(silhouette);
        paintInactiveTabLedge(g, band, inactiveTab);
    }

    void paintRoundedWell(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour fill)
    {
        g.setColour(fill);
        g.fillRoundedRectangle(area.toFloat(), kControlCorner);
    }
}
