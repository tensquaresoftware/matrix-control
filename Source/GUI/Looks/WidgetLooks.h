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
        juce::Font font{juce::FontOptions{}};
    };

    struct SliderLook
    {
        juce::Colour trackEnabled{};
        juce::Colour trackDisabled{};
        juce::Colour valueBarEnabled{};
        juce::Colour valueBarDisabled{};
        juce::Colour textEnabled{};
        juce::Colour textDisabled{};
        juce::Colour focusBorder{};
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
        
        juce::Colour buttonLikeBackground{};
        juce::Colour buttonLikeBackgroundDisabled{};
        juce::Colour buttonLikeBorder{};
        juce::Colour buttonLikeBorderDisabled{};
        juce::Colour buttonLikeText{};
        juce::Colour buttonLikeTextDisabled{};
        juce::Colour buttonLikeTriangle{};
        juce::Colour buttonLikeTriangleDisabled{};
        
        juce::Font font{juce::FontOptions{}};
    };

    struct PopupMenuLook
    {
        juce::Colour background{};
        juce::Colour border{};
        juce::Colour separator{};
        juce::Colour text{};
        juce::Colour backgroundHover{};
        juce::Colour textHover{};
        juce::Colour scrollbar{};
        
        juce::Colour backgroundButtonLike{};
        juce::Colour borderButtonLike{};
        juce::Colour separatorButtonLike{};
        juce::Colour textButtonLike{};
        juce::Colour backgroundHoverButtonLike{};
        juce::Colour textHoverButtonLike{};
        juce::Colour scrollbarButtonLike{};
        
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
    };

    struct GroupLabelLook
    {
        juce::Colour text{};
        juce::Colour line{};
        juce::Font font{juce::FontOptions{}};
    };

    struct VerticalSeparatorLook
    {
        juce::Colour line{};
    };

    struct ToggleLook
    {
        juce::Colour border{};
        juce::Colour backgroundOff{};
        juce::Colour backgroundOn{};
        juce::Colour textOff{};
        juce::Colour textOn{};
        juce::Font font{juce::FontOptions{}};
    };

    struct NumberBoxLook
    {
        juce::Colour background{};
        juce::Colour borderOff{};
        juce::Colour borderOn{};
        juce::Colour text{};
        juce::Colour dot{};
        juce::Colour editorBackground{};
        juce::Colour editorText{};
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
