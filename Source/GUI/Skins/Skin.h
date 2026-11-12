#pragma once

#include <map>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

#include "ISkin.h"
#include "SkinColoursCommon.h"

namespace TSS
{
    class Skin : public ISkin
    {
    public:
        enum class ColourVariant
        {
            Black,
            BlackChroma,
            BlackMinimal,
            Cream,
            CreamMinimal
        };

        /** Getting Started Matrix monochrome chrome always uses BLACK/GREEN,
            never BLACK/CHROMA or CREAM/GREEN (dark plate vs cream body text). */
        static constexpr ColourVariant kGettingStartedMatrixChromeVariant = ColourVariant::Black;

        enum class SkinComboBoxItemId : int
        {
            kBlack = 1,
            kCream = 2,
            kBlackChroma = 3,
            kBlackMinimal = 4,
            kCreamMinimal = 5
        };

        explicit Skin(ColourVariant variant);
        ~Skin() override = default;

        static std::unique_ptr<Skin> create(ColourVariant variant);

        ColourVariant getColourVariant() const { return currentVariant_; }
        size_t getMappedColourCount() const noexcept { return colours_.size(); }

        juce::Colour getColour(SkinObjectType object,
                               SkinColourProperty property,
                               SkinColourState state = SkinColourState::kDefault,
                               ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getColour(SkinColourKey key) const override;
        void setColour(SkinColourKey key, juce::Colour colour) override;
        void clearHigherColourSets();

        juce::Font getBaseFont() const override;
        juce::Font getBaseFontBold() const override;
        juce::Font getBrandFontBold() const override;
        juce::Font getModalBodyFont() const override;
        juce::Font getModalBodyFontBold() const override;

        juce::Colour getToggleBackgroundColour(bool isOn,
                                               ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getToggleTextColour(bool isOn,
                                         ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getSliderTrackColour(bool isEnabled = true,
                                          ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getSliderValueBarColour(bool isEnabled = true,
                                             ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getSliderTextColour(bool isEnabled = true,
                                         ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getComboBoxBackgroundColour(bool isEnabled = true,
                                                 ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getComboBoxBorderColour(bool isEnabled = true,
                                             ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getComboBoxFocusBorderColour(ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getComboBoxTriangleColour(bool isEnabled = true,
                                               ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getComboBoxTextColour(bool isEnabled = true,
                                           ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getPopupMenuBackgroundColour(ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getPopupMenuBorderColour(ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getPopupMenuSeparatorColour(ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getPopupMenuTextColour(ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getPopupMenuBackgroundHooverColour(ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getPopupMenuTextHooverColour(ColourSet colourSet = ColourSet::k1) const override;
        juce::Colour getPopupMenuScrollbarColour(ColourSet colourSet = ColourSet::k1) const override;

    private:
        void initializeDefaultColours();

        ColourVariant currentVariant_;
        std::map<SkinColourKey, juce::Colour> colours_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Skin)
    };
}
