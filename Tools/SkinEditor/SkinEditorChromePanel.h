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
     * Outer kGutter above/below the silhouette (window bg). Créneau flush left.
     * Body content inset with kPanelContentPad on top (under title), left, right, bottom.
     */
    PanelChrome layoutPanel(juce::Rectangle<int> bounds, int tabWidth);

    /**
     * Panel silhouette: folder-tab title (45° shoulder, same language as table section
     * headers), then body corners. Top/bottom gutters stay window bg (parent fill).
     */
    void paintPanelFrame(juce::Graphics& g,
                         juce::Rectangle<int> panelBounds,
                         juce::Rectangle<int> tab);

    void paintRoundedWell(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour fill);
}
