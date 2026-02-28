#pragma once

#include <map>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

#include "ISkin.h"
#include "SkinValues.h"

namespace tss
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

        float getValue(SkinValueId valueId) const override;
        juce::Colour getColour(SkinColourId colourId) const override;

        void setValue(SkinValueId valueId, float value) override;
        void setColour(SkinColourId colourId, juce::Colour colour) override;

        juce::Font getBaseFont() const override;
        juce::Font getBaseFontBold() const override;

        // Theme-compatible API for migration
        juce::Colour getHeaderPanelBackgroundColour() const;
        juce::Colour getHeaderPanelLabelTextColour() const;
        juce::Colour getBodyPanelBackgroundColour() const;
        juce::Colour getFooterPanelBackgroundColour() const;
        juce::Colour getSectionHeaderTextColour() const;
        juce::Colour getSectionHeaderLineColourBlue() const;
        juce::Colour getSectionHeaderLineColourOrange() const;
        juce::Colour getModuleHeaderTextColour() const;
        juce::Colour getModuleHeaderLineColourBlue() const;
        juce::Colour getModuleHeaderLineColourOrange() const;
        juce::Colour getGroupLabelTextColour() const;
        juce::Colour getGroupLabelLineColour() const;
        juce::Colour getLabelTextColour() const;
        juce::Colour getVerticalSeparatorLineColour() const;
        juce::Colour getHorizontalSeparatorLineColour() const;
        juce::Colour getButtonBackgroundColourOff() const;
        juce::Colour getButtonBorderColourOff() const;
        juce::Colour getButtonTextColourOff() const;
        juce::Colour getButtonBackgroundColourOn() const;
        juce::Colour getButtonBorderColourOn() const;
        juce::Colour getButtonTextColourOn() const;
        juce::Colour getButtonBackgroundColourHoover() const;
        juce::Colour getButtonTextColourHoover() const;
        juce::Colour getButtonBackgroundColourClicked() const;
        juce::Colour getButtonTextColourClicked() const;
        juce::Colour getToggleBorderColour() const;
        juce::Colour getToggleBackgroundColour(bool isOn) const override;
        juce::Colour getToggleTextColour(bool isOn) const override;
        juce::Colour getSliderTrackColour(bool isEnabled = true) const override;
        juce::Colour getSliderValueBarColour(bool isEnabled = true) const override;
        juce::Colour getSliderTextColour(bool isEnabled = true) const override;
        juce::Colour getSliderFocusBorderColour() const;
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
        juce::Colour getNumberBoxTextColour() const;
        juce::Colour getNumberBoxDotColour() const;
        juce::Colour getNumberBoxEditorBackgroundColour() const;
        juce::Colour getNumberBoxEditorSelectionBackgroundColour() const;
        juce::Colour getNumberBoxEditorTextColour() const;
        juce::Colour getEnvelopeDisplayBackgroundColour() const;
        juce::Colour getEnvelopeDisplayBorderColour() const;
        juce::Colour getEnvelopeDisplayEnvelopeColour() const;
        juce::Colour getPatchNameDisplayBackgroundColour() const;
        juce::Colour getPatchNameDisplayBorderColour() const;
        juce::Colour getPatchNameDisplayTextColour() const;
        juce::Colour getTrackGeneratorDisplayBackgroundColour() const;
        juce::Colour getTrackGeneratorDisplayBorderColour() const;
        juce::Colour getTrackGeneratorDisplayShaperColour() const;

    private:
        void initializeDefaultValues();
        void initializeDefaultColours();
        
        template <typename Accessor>
        void initializeVariantColours(Accessor accessColour);

        ColourVariant currentVariant_;
        std::map<SkinValueId, float> values_;
        std::map<SkinColourId, juce::Colour> colours_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Skin)
    };
}
