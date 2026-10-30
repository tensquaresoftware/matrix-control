#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace SkinEditorChrome
{
    /** Neutral greys + combo-matched corner radius for buttons and combos. */
    class LookAndFeel final : public juce::LookAndFeel_V4
    {
    public:
        LookAndFeel();

        void drawScrollbar(juce::Graphics& g,
                           juce::ScrollBar& scrollbar,
                           int x,
                           int y,
                           int width,
                           int height,
                           bool isScrollbarVertical,
                           int thumbStartPosition,
                           int thumbSize,
                           bool isMouseOver,
                           bool isMouseDown) override;

        juce::Font getTextButtonFont(juce::TextButton&, int) override;
        juce::Font getComboBoxFont(juce::ComboBox&) override;
        juce::Font getLabelFont(juce::Label& label) override;
        juce::Font getPopupMenuFont() override;

        void drawButtonBackground(juce::Graphics& g,
                                  juce::Button& button,
                                  const juce::Colour& backgroundColour,
                                  bool shouldDrawButtonAsHighlighted,
                                  bool shouldDrawButtonAsDown) override;

        void drawToggleButton(juce::Graphics& g,
                              juce::ToggleButton& button,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

        void drawComboBox(juce::Graphics& g,
                          int width,
                          int height,
                          bool isButtonDown,
                          int buttonX,
                          int buttonY,
                          int buttonW,
                          int buttonH,
                          juce::ComboBox& box) override;

        void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override;

        void fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& ed) override;
        void drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& ed) override;

        juce::Rectangle<int> getTooltipBounds(const juce::String& tipText,
                                              juce::Point<int> screenPos,
                                              juce::Rectangle<int> parentArea) override;
        void drawTooltip(juce::Graphics& g, const juce::String& text, int width, int height) override;
    };
}
