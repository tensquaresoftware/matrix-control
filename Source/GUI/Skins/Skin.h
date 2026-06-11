#pragma once

#include <map>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

#include "ISkin.h"

namespace TSS
{
    class Skin : public ISkin
    {
    public:
        enum class ColourVariant
        {
            Black,
            Cream
        };

        enum class SkinComboBoxItemId : int
        {
            kBlack = 1,
            kCream = 2
        };

        explicit Skin(ColourVariant variant);
        ~Skin() override = default;

        static std::unique_ptr<Skin> create(ColourVariant variant);

        ColourVariant getColourVariant() const { return currentVariant_; }

        juce::Colour getColour(SkinColourId colourId) const override;

        void setColour(SkinColourId colourId, juce::Colour colour) override;

        juce::Font getBaseFont() const override;
        juce::Font getBaseFontBold() const override;
        juce::Font getBrandFontBold() const override;

        juce::Colour getToggleBackgroundColour(bool isOn) const override;
        juce::Colour getToggleTextColour(bool isOn) const override;
        juce::Colour getSliderTrackColour(bool isEnabled = true) const override;
        juce::Colour getSliderValueBarColour(bool isEnabled = true) const override;
        juce::Colour getSliderTextColour(bool isEnabled = true) const override;
        juce::Colour getComboBoxBackgroundColour(bool isEnabled = true, bool isButtonLike = false) const override;
        juce::Colour getComboBoxBorderColour(bool isEnabled = true, bool isButtonLike = false) const override;
        juce::Colour getComboBoxFocusBorderColour(bool isButtonLike = false) const override;
        juce::Colour getComboBoxTriangleColour(bool isEnabled = true, bool isButtonLike = false) const override;
        juce::Colour getComboBoxTextColour(bool isEnabled = true, bool isButtonLike = false) const override;
        juce::Colour getPopupMenuBackgroundColour(bool isButtonLike = false) const override;
        juce::Colour getPopupMenuBorderColour(bool isButtonLike = false) const override;
        juce::Colour getPopupMenuSeparatorColour(bool isButtonLike = false) const override;
        juce::Colour getPopupMenuTextColour(bool isButtonLike = false) const override;
        juce::Colour getPopupMenuBackgroundHooverColour(bool isButtonLike = false) const override;
        juce::Colour getPopupMenuTextHooverColour(bool isButtonLike = false) const override;
        juce::Colour getPopupMenuScrollbarColour(bool isButtonLike = false) const override;

    private:
        void initializeDefaultColours();
        
        template <typename Accessor>
        void initializeVariantColours(Accessor accessColour);
        
        template <typename Accessor>
        void initializePanelColours(Accessor accessColour);
        
        template <typename Accessor>
        void initializeLayoutColours(Accessor accessColour);
        
        template <typename Accessor>
        void initializeButtonColours(Accessor accessColour);
        
        template <typename Accessor>
        void initializeToggleColours(Accessor accessColour);
        
        template <typename Accessor>
        void initializeSliderColours(Accessor accessColour);
        
        template <typename Accessor>
        void initializeComboBoxColours(Accessor accessColour);
        
        template <typename Accessor>
        void initializePopupMenuColours(Accessor accessColour);
        
        template <typename Accessor>
        void initializeDisplayColours(Accessor accessColour);

        ColourVariant currentVariant_;
        std::map<SkinColourId, juce::Colour> colours_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Skin)
    };
}
