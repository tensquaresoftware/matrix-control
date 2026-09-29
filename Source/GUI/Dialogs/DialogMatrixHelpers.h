#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/Button.h"

namespace TSS
{
    class ISkin;
}

namespace DialogMatrixHelpers
{
    inline constexpr int kDefaultButtonHeight = 24;
    inline constexpr int kDefaultButtonWidth = 88;
    inline constexpr juce::uint32 kDialogBorderColour = 0xff5E5E5E;

    std::unique_ptr<TSS::Button> makeButton(TSS::ISkin& skin,
                                            int designWidth,
                                            const juce::String& text,
                                            int designHeight = kDefaultButtonHeight);

    void applyButtonSkin(TSS::Button& button, TSS::ISkin& skin);
    void applyButtonUiScale(TSS::Button& button, float uiScale);

    juce::Font scaledModalBodyFont(const TSS::ISkin& skin, float uiScale);
    juce::Font scaledTitleFont(const TSS::ISkin& skin, float uiScale);

    struct OverlayChromePaintArgs
    {
        juce::Graphics& g;
        const TSS::ISkin& skin;
        juce::Rectangle<int> dialogBounds;
        int borderThickness = 0;
        int titleBarHeight = 0;
        juce::String title;
        float uiScale = 1.0f;
    };

    void paintMatrixOverlayChrome(const OverlayChromePaintArgs& args);
}
