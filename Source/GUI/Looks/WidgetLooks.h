#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    struct ButtonLook
    {
        juce::Colour backgroundOff{};
        juce::Colour backgroundOn{};
        juce::Colour backgroundHover{};
        juce::Colour backgroundClicked{};
        juce::Colour borderOff{};
        juce::Colour borderOn{};
        juce::Colour textOff{};
        juce::Colour textOn{};
        juce::Colour textHover{};
        juce::Colour textClicked{};
        juce::Colour backgroundDisabled{};
        juce::Colour borderDisabled{};
        juce::Colour textDisabled{};
        /** Called-out label among peers (Button Text / Emphasis). */
        juce::Colour textEmphasis{};
        juce::Font font{juce::FontOptions{}};
    };

    struct SliderLook
    {
        juce::Colour trackEnabled{};
        juce::Colour trackDisabled{};
        juce::Colour backgroundDisabled{};
        juce::Colour valueBarEnabled{};
        juce::Colour valueBarDisabled{};
        juce::Colour textEnabled{};
        juce::Colour textDisabled{};
        juce::Colour focusBorder{};
        juce::Colour editorText{};
        juce::Colour editorCaret{};
        juce::Font font{juce::FontOptions{}};
    };

    struct LabelLook
    {
        juce::Colour text{};
        juce::Font font{juce::FontOptions{}};
    };

    struct HorizontalSeparatorLook
    {
        juce::Colour line{};
    };

    struct ComboBoxLook
    {
        juce::Colour backgroundEnabled{};
        juce::Colour backgroundDisabled{};
        juce::Colour borderEnabled{};
        juce::Colour borderDisabled{};
        juce::Colour focusBorder{};
        juce::Colour triangleEnabled{};
        juce::Colour triangleDisabled{};
        juce::Colour textEnabled{};
        juce::Colour textDisabled{};
        /** Navigation-focus label (ComboBox Text / Focus). */
        juce::Colour textFocus{};
        juce::Font font{juce::FontOptions{}};
    };

    struct PopupMenuLook
    {
        juce::Colour background{};
        juce::Colour border{};
        juce::Colour separator{};
        juce::Colour text{};
        juce::Colour textDisabled{};
        juce::Colour backgroundHover{};
        juce::Colour textHover{};
        juce::Colour scrollbar{};
        juce::Font font{juce::FontOptions{}};
    };

    struct EnvelopeDisplayLook
    {
        juce::Colour background{};
        juce::Colour border{};
        juce::Colour envelope{};
    };

    struct TrackGeneratorDisplayLook
    {
        juce::Colour background{};
        juce::Colour border{};
        juce::Colour curve{};
    };

    struct PatchNameDisplayLook
    {
        juce::Colour background{};
        juce::Colour border{};
        juce::Colour text{};
        juce::Font font{juce::FontOptions{}};
        // Mutator / Compare secondary line (Mxx, Mxx-Ryy, INITIAL) — solid mapped colour + smaller
        // than the primary user patch name so the eye keeps the name as the dominant line.
        juce::Colour secondaryText{};
        juce::Font secondaryFont{juce::FontOptions{}};
        // Glyph ink drawn over the caret plate while editing.
        juce::Colour caretInk{};
    };

    struct ReadOnlyValueFieldLook
    {
        juce::Colour background{};
        juce::Colour text{};
    };

    struct LedLook
    {
        juce::Colour backgroundOff{};
        juce::Colour borderOff{};
        juce::Colour maxOn{};
    };

    struct GroupLabelLook
    {
        juce::Colour text{};
        juce::Colour line{};
        juce::Font font{juce::FontOptions{}};
    };

    struct ToggleLook
    {
        juce::Colour border{};
        juce::Colour backgroundOff{};
        juce::Colour backgroundOn{};
        juce::Colour textOff{};
        juce::Colour textOn{};
        juce::Colour backgroundDisabled{};
        juce::Colour borderDisabled{};
        juce::Colour textDisabled{};
        juce::Font font{juce::FontOptions{}};
    };

    struct NumberBoxLook
    {
        juce::Colour background{};
        juce::Colour backgroundDisabled{};
        juce::Colour borderOff{};
        juce::Colour borderOn{};
        juce::Colour text{};
        // Used instead of `text` while this box owns patch-navigation focus.
        juce::Colour textFocus{};
        juce::Colour editorBackground{};
        juce::Colour editorText{};
        juce::Colour editorCaret{};
        juce::Colour editorSelectionBackground{};
        juce::Font font{juce::FontOptions{}};
    };

    struct SectionHeaderLook
    {
        juce::Colour text{};
        juce::Colour lineBlue{};
        juce::Colour lineOrange{};
        juce::Font font{juce::FontOptions{}};
    };

    struct ModuleHeaderLook
    {
        juce::Colour text{};
        // Opt-in title-hover colour (NumberBox / Computer combo focus-red token).
        juce::Colour textFocus{};
        juce::Colour lineBlue{};
        juce::Colour lineOrange{};
        juce::Font font{juce::FontOptions{}};
    };

    struct ModulationBusHeaderLook
    {
        juce::Colour text{};
        juce::Colour lineBlue{};
        juce::Colour lineOrange{};
        juce::Font font{juce::FontOptions{}};
    };
}
