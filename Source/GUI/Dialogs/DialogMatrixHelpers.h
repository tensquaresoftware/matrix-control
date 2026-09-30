#pragma once

#include <memory>
#include <vector>

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

    // Shared Matrix modal chrome metrics (design pixels at 100% UI scale).
    inline constexpr int kTitleBarHeight = 28;
    inline constexpr int kBorderThickness = 4;
    inline constexpr int kButtonGap = 12;
    inline constexpr int kButtonBottomMargin = 16;
    inline constexpr int kCheckboxHeight = 22;

    // Body text sits in an invisible block inset by this fraction of the modal width on each side.
    inline constexpr float kBodySideInsetFraction = 0.10f;

    std::unique_ptr<TSS::Button> makeButton(TSS::ISkin& skin,
                                            int designWidth,
                                            const juce::String& text,
                                            int designHeight = kDefaultButtonHeight);

    void applyButtonSkin(TSS::Button& button, TSS::ISkin& skin);
    void applyButtonUiScale(TSS::Button& button, float uiScale);

    juce::Font scaledModalBodyFont(const TSS::ISkin& skin, float uiScale);
    juce::Font scaledTitleFont(const TSS::ISkin& skin, float uiScale);

    /** Pixel width for a Matrix button label, sized from the button font's glyph width. */
    int estimateButtonWidth(TSS::ISkin& skin, const juce::String& text, float uiScale);

    struct ButtonPlacement
    {
        juce::Component* component = nullptr;
        int width = 0;
    };

    /** Packs the buttons left-to-right with the shared gap, then centres the pack in `row`.
        When the pack is wider than the row, gaps shrink first, then widths, so nothing leaves the row. */
    void layoutCentredButtonRow(juce::Rectangle<int> row,
                                float uiScale,
                                const std::vector<ButtonPlacement>& buttons);

    // ---- Body text (left-aligned, no horizontal squeeze, measured so nothing clips) ----

    int contentWidthFor(int designWidth, float uiScale);
    int bodyTextWidthFor(int contentWidth);

    /** Height needed to draw `text` with the shared body settings inside `textWidth` pixels. */
    int measureBodyHeight(const juce::Font& font, const juce::String& text, int textWidth);

    /** Vertical distance between two consecutive text lines of `font`. */
    int measureLineStep(const juce::Font& font, int textWidth);

    void paintBodyText(juce::Graphics& g,
                       const juce::Font& font,
                       const juce::String& text,
                       juce::Rectangle<int> textArea);

    // ---- Shared modal frame geometry ----

    struct ModalGeometryArgs
    {
        juce::Rectangle<int> hostBounds;
        int designWidth = 0;
        float uiScale = 1.0f;
        float bodyEm = 14.0f;
        int bodyHeight = 0;
        /** Pixel height of extra controls (e.g. checkbox, form rows) placed between body and buttons. */
        int extraBandHeight = 0;
    };

    struct ModalGeometry
    {
        juce::Rectangle<int> dialogBounds;
        int border = 0;
        int titleBarHeight = 0;
        juce::Rectangle<int> textArea;
        /** Space between the body text and the button row (extra controls are centred here). */
        juce::Rectangle<int> band;
        juce::Rectangle<int> buttonRow;
    };

    ModalGeometry computeModalGeometry(const ModalGeometryArgs& args);

    /** Geometry plus the scaled body font, for dialogs whose body is a single text block. */
    struct TextModalLayout
    {
        ModalGeometry geometry;
        juce::Font bodyFont { juce::FontOptions{} };
    };

    struct TextModalLayoutArgs
    {
        const TSS::ISkin& skin;
        const juce::String& bodyText;
        juce::Rectangle<int> hostBounds;
        int designWidth = 0;
        float uiScale = 1.0f;
        int extraBandHeight = 0;
    };

    TextModalLayout computeTextModalLayout(const TextModalLayoutArgs& args);

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

    /** Paints overlay dim, border and title bar; the title is always drawn UPPERCASE. */
    void paintMatrixOverlayChrome(const OverlayChromePaintArgs& args);

    /** Toggle look that draws its label with the modal body font (Montserrat). */
    class ModalToggleLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void setLabelFont(juce::Font font);
        void setUiScale(float uiScale);

        void drawToggleButton(juce::Graphics& g,
                              juce::ToggleButton& button,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

        /** Pixel width needed for the toggle tick plus its label. */
        int getPreferredWidth(const juce::String& text, int buttonHeight) const;

    private:
        float getTickWidth(int buttonHeight) const;

        juce::Font labelFont_ { juce::FontOptions{} };
        float uiScale_ = 1.0f;
    };
}
