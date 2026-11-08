#pragma once

#include "Tools/SkinEditor/SkinEditorChromeTokens.h"

#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace SkinEditorChrome
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

    void paintRoundedWell(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour fill);
}
