#pragma once

#include "Tools/SkinDesigner/SkinDesignerChromeTokens.h"

#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace SkinDesignerChrome
{
    struct PanelChrome
    {
        juce::Rectangle<int> tab;
        juce::Rectangle<int> content;
    };

    /**
     * Outer gutters above/below the silhouette (window bg). Créneau flush left.
     * Body content inset with kPanelContentPad on top (under title), left, right, bottom.
     * bottomGutter: kGutter for stacked panels (SKINS); 0 when flush to the status bar.
     */
    PanelChrome layoutPanel(juce::Rectangle<int> bounds,
                            int tabWidth,
                            int bottomGutter = kGutter);

    /**
     * Panel silhouette: folder-tab title (45° shoulder, same language as table section
     * headers), then body corners. Top/bottom gutters stay window bg (parent fill).
     */
    void paintPanelFrame(juce::Graphics& g,
                         juce::Rectangle<int> panelBounds,
                         juce::Rectangle<int> tab,
                         int bottomGutter = kGutter);

    /** Fixed-order dual tab notches for the Mappings | Zones work surface. */
    struct WorkSurfaceFrame
    {
        juce::Rectangle<int> panelBounds;
        juce::Rectangle<int> mappingsTab;
        juce::Rectangle<int> zonesTab;
        bool zonesActive = false;
        int bottomGutter = kGutter;
    };

    /**
     * Active tab merges into the panel body; inactive tab is a quieter ledge
     * on the shoulder (always COLOUR MAPPINGS left, ZONE MAPPINGS right).
     */
    void paintWorkSurfacePanelFrame(juce::Graphics& g, const WorkSurfaceFrame& frame);

    void paintRoundedWell(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour fill);
}
