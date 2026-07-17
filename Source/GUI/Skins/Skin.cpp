#include "Skin.h"

#include "SkinColours.h"

#include <PluginFontsData.h>

namespace TSS
{
    namespace
    {
        constexpr float kSkinBaseFontHeight_ = 14.0f;
        constexpr float kBrandFontHeight_ = 20.0f;
    }

    Skin::Skin(ColourVariant variant)
        : currentVariant_(variant)
    {
        initializeDefaultColours();
    }

    std::unique_ptr<Skin> Skin::create(ColourVariant variant)
    {
        return std::make_unique<Skin>(variant);
    }

    juce::Colour Skin::getColour(SkinColourId colourId) const
    {
        const auto it = colours_.find(colourId);
        if (it != colours_.end())
            return it->second;

        return juce::Colour(ColourChart::kWhite);
    }

    void Skin::setColour(SkinColourId colourId, juce::Colour colour)
    {
        colours_[colourId] = colour;
    }

    juce::Font Skin::getBaseFont() const
    {
        const auto typeface = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::PTSansNarrowRegular_ttf,
            static_cast<size_t>(PluginFontsData::PTSansNarrowRegular_ttfSize)
        );
        return juce::Font(juce::FontOptions(typeface).withHeight(kSkinBaseFontHeight_));
    }

    juce::Font Skin::getBaseFontBold() const
    {
        const auto typeface = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::PTSansNarrowBold_ttf,
            static_cast<size_t>(PluginFontsData::PTSansNarrowBold_ttfSize)
        );
        return juce::Font(juce::FontOptions(typeface).withHeight(kSkinBaseFontHeight_));
    }

    juce::Font Skin::getBrandFontBold() const
    {
        const auto typeface = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::OrbitronBold_ttf,
            static_cast<size_t>(PluginFontsData::OrbitronBold_ttfSize)
        );
        return juce::Font(juce::FontOptions(typeface).withHeight(kBrandFontHeight_));
    }

    juce::Colour Skin::getToggleBackgroundColour(bool isOn) const
    {
        return getColour(isOn ? SkinColourId::kToggleBackgroundOn : SkinColourId::kToggleBackgroundOff);
    }

    juce::Colour Skin::getToggleTextColour(bool isOn) const
    {
        return getColour(isOn ? SkinColourId::kToggleTextOn : SkinColourId::kToggleTextOff);
    }

    juce::Colour Skin::getSliderTrackColour(bool isEnabled) const
    {
        return getColour(isEnabled ? SkinColourId::kSliderTrackEnabled : SkinColourId::kSliderTrackDisabled);
    }

    juce::Colour Skin::getSliderValueBarColour(bool isEnabled) const
    {
        return getColour(isEnabled ? SkinColourId::kSliderValueBarEnabled : SkinColourId::kSliderValueBarDisabled);
    }

    juce::Colour Skin::getSliderTextColour(bool isEnabled) const
    {
        return getColour(isEnabled ? SkinColourId::kSliderTextEnabled : SkinColourId::kSliderTextDisabled);
    }

    juce::Colour Skin::getComboBoxBackgroundColour(bool isEnabled, bool isButtonLike) const
    {
        if (isButtonLike)
            return getColour(isEnabled ? SkinColourId::kComboBoxButtonLikeBackground : SkinColourId::kComboBoxButtonLikeBackgroundDisabled);
        return getColour(isEnabled ? SkinColourId::kComboBoxBackgroundEnabled : SkinColourId::kComboBoxBackgroundDisabled);
    }

    juce::Colour Skin::getComboBoxBorderColour(bool isEnabled, bool isButtonLike) const
    {
        if (isButtonLike)
            return getColour(isEnabled ? SkinColourId::kComboBoxButtonLikeBorder : SkinColourId::kComboBoxButtonLikeBorderDisabled);
        return getColour(isEnabled ? SkinColourId::kComboBoxBorderEnabled : SkinColourId::kComboBoxBorderDisabled);
    }

    juce::Colour Skin::getComboBoxFocusBorderColour(bool isButtonLike) const
    {
        if (isButtonLike)
            return getColour(SkinColourId::kComboBoxButtonLikeBorder);
        return getColour(SkinColourId::kComboBoxFocusBorder);
    }

    juce::Colour Skin::getComboBoxTriangleColour(bool isEnabled, bool isButtonLike) const
    {
        if (isButtonLike)
            return getColour(isEnabled ? SkinColourId::kComboBoxButtonLikeTriangle : SkinColourId::kComboBoxButtonLikeTriangleDisabled);
        return getColour(isEnabled ? SkinColourId::kComboBoxTriangleEnabled : SkinColourId::kComboBoxTriangleDisabled);
    }

    juce::Colour Skin::getComboBoxTextColour(bool isEnabled, bool isButtonLike) const
    {
        if (isButtonLike)
            return getColour(isEnabled ? SkinColourId::kComboBoxButtonLikeText : SkinColourId::kComboBoxButtonLikeTextDisabled);
        return getColour(isEnabled ? SkinColourId::kComboBoxTextEnabled : SkinColourId::kComboBoxTextDisabled);
    }

    juce::Colour Skin::getPopupMenuBackgroundColour(bool isButtonLike) const
    {
        return getColour(isButtonLike ? SkinColourId::kPopupMenuBackgroundButtonLike : SkinColourId::kPopupMenuBackground);
    }

    juce::Colour Skin::getPopupMenuBorderColour(bool isButtonLike) const
    {
        return getColour(isButtonLike ? SkinColourId::kPopupMenuBorderButtonLike : SkinColourId::kPopupMenuBorder);
    }

    juce::Colour Skin::getPopupMenuSeparatorColour(bool isButtonLike) const
    {
        return getColour(isButtonLike ? SkinColourId::kPopupMenuSeparatorButtonLike : SkinColourId::kPopupMenuSeparator);
    }

    juce::Colour Skin::getPopupMenuTextColour(bool isButtonLike) const
    {
        return getColour(isButtonLike ? SkinColourId::kPopupMenuTextButtonLike : SkinColourId::kPopupMenuText);
    }

    juce::Colour Skin::getPopupMenuBackgroundHooverColour(bool isButtonLike) const
    {
        return getColour(isButtonLike ? SkinColourId::kPopupMenuBackgroundHoverButtonLike : SkinColourId::kPopupMenuBackgroundHover);
    }

    juce::Colour Skin::getPopupMenuTextHooverColour(bool isButtonLike) const
    {
        return getColour(isButtonLike ? SkinColourId::kPopupMenuTextHoverButtonLike : SkinColourId::kPopupMenuTextHover);
    }

    juce::Colour Skin::getPopupMenuScrollbarColour(bool isButtonLike) const
    {
        return getColour(isButtonLike ? SkinColourId::kPopupMenuScrollbarButtonLike : SkinColourId::kPopupMenuScrollbar);
    }

    void Skin::initializeDefaultColours()
    {
        if (currentVariant_ == ColourVariant::Black)
            initializeVariantColours([](const auto& el) { return el.blackVariant; });
        else
            initializeVariantColours([](const auto& el) { return el.creamVariant; });
    }

    template <typename Accessor>
    void Skin::initializeVariantColours(Accessor accessColour)
    {
        initializePanelColours(accessColour);
        initializeLayoutColours(accessColour);
        initializeButtonColours(accessColour);
        initializeToggleColours(accessColour);
        initializeSliderColours(accessColour);
        initializeComboBoxColours(accessColour);
        initializePopupMenuColours(accessColour);
        initializeDisplayColours(accessColour);
    }
    
    template <typename Accessor>
    void Skin::initializePanelColours(Accessor accessColour)
    {
        using namespace SkinColours;
        
        colours_[SkinColourId::kHeaderPanelBackground] = juce::Colour(accessColour(Panels::kHeaderPanelBackground));
        colours_[SkinColourId::kBodyPanelBackground] = juce::Colour(accessColour(Panels::kBodyPanelBackground));
        colours_[SkinColourId::kFooterPanelBackground] = juce::Colour(accessColour(Panels::kFooterPanelBackground));
        colours_[SkinColourId::kDarkPanelText] = juce::Colour(accessColour(Widgets::DarkPanel::kText));
        colours_[SkinColourId::kFooterMessageInfo] = juce::Colour(accessColour(Widgets::FooterPanel::kMessageInfo));
        colours_[SkinColourId::kFooterMessageSuccess] = juce::Colour(accessColour(Widgets::FooterPanel::kMessageSuccess));
        colours_[SkinColourId::kFooterMessageWarning] = juce::Colour(accessColour(Widgets::FooterPanel::kMessageWarning));
        colours_[SkinColourId::kFooterMessageError] = juce::Colour(accessColour(Widgets::FooterPanel::kMessageError));
    }
    
    template <typename Accessor>
    void Skin::initializeLayoutColours(Accessor accessColour)
    {
        using namespace SkinColours;
        
        colours_[SkinColourId::kSectionHeaderText] = juce::Colour(accessColour(Widgets::SectionHeader::kText));
        colours_[SkinColourId::kSectionHeaderLineBlue] = juce::Colour(accessColour(Widgets::SectionHeader::kLineBlue));
        colours_[SkinColourId::kSectionHeaderLineOrange] = juce::Colour(accessColour(Widgets::SectionHeader::kLineOrange));

        colours_[SkinColourId::kModuleHeaderText] = juce::Colour(accessColour(Widgets::ModuleHeader::kText));
        colours_[SkinColourId::kModuleHeaderLineBlue] = juce::Colour(accessColour(Widgets::ModuleHeader::kLineBlue));
        colours_[SkinColourId::kModuleHeaderLineOrange] = juce::Colour(accessColour(Widgets::ModuleHeader::kLineOrange));

        colours_[SkinColourId::kGroupLabelText] = juce::Colour(accessColour(Widgets::GroupLabel::kText));
        colours_[SkinColourId::kGroupLabelLine] = juce::Colour(accessColour(Widgets::GroupLabel::kLine));

        colours_[SkinColourId::kLabelText] = juce::Colour(accessColour(Widgets::Label::kText));

        colours_[SkinColourId::kVerticalSeparatorLine] = juce::Colour(accessColour(Widgets::VerticalSeparator::kLine));
        colours_[SkinColourId::kHorizontalSeparatorLine] = juce::Colour(accessColour(Widgets::HorizontalSeparator::kLine));
    }
    
    template <typename Accessor>
    void Skin::initializeButtonColours(Accessor accessColour)
    {
        using namespace SkinColours;
        
        colours_[SkinColourId::kButtonBackgroundOff] = juce::Colour(accessColour(Widgets::Button::kBackground));
        colours_[SkinColourId::kButtonBorderOff] = juce::Colour(accessColour(Widgets::Button::kBorder));
        colours_[SkinColourId::kButtonTextOff] = juce::Colour(accessColour(Widgets::Button::kText));
        colours_[SkinColourId::kButtonBackgroundOn] = juce::Colour(accessColour(Widgets::Button::kBackground));
        colours_[SkinColourId::kButtonBorderOn] = juce::Colour(accessColour(Widgets::Button::kBorder));
        colours_[SkinColourId::kButtonTextOn] = juce::Colour(accessColour(Widgets::Button::kTextOn));
        colours_[SkinColourId::kButtonBackgroundHover] = juce::Colour(accessColour(Widgets::Button::kBackgroundHoover));
        colours_[SkinColourId::kButtonTextHover] = juce::Colour(accessColour(Widgets::Button::kTextHoover));
        colours_[SkinColourId::kButtonBackgroundClicked] = juce::Colour(accessColour(Widgets::Button::kBackgroundClicked));
        colours_[SkinColourId::kButtonTextClicked] = juce::Colour(accessColour(Widgets::Button::kTextClicked));
        colours_[SkinColourId::kButtonBackgroundDisabled] = juce::Colour(accessColour(Widgets::Button::kBackgroundDisabled));
        colours_[SkinColourId::kButtonBorderDisabled] = juce::Colour(accessColour(Widgets::Button::kBorderDisabled));
        colours_[SkinColourId::kButtonTextDisabled] = juce::Colour(accessColour(Widgets::Button::kTextDisabled));
    }
    
    template <typename Accessor>
    void Skin::initializeToggleColours(Accessor accessColour)
    {
        using namespace SkinColours;
        
        colours_[SkinColourId::kToggleBorder] = juce::Colour(accessColour(Widgets::Toggle::kBorder));
        colours_[SkinColourId::kToggleBackgroundOff] = juce::Colour(accessColour(Widgets::Toggle::kBackgroundOff));
        colours_[SkinColourId::kToggleTextOff] = juce::Colour(accessColour(Widgets::Toggle::kTextOff));
        colours_[SkinColourId::kToggleBackgroundOn] = juce::Colour(accessColour(Widgets::Toggle::kBackgroundOn));
        colours_[SkinColourId::kToggleTextOn] = juce::Colour(accessColour(Widgets::Toggle::kTextOn));
        colours_[SkinColourId::kToggleBackgroundDisabled] = juce::Colour(accessColour(Widgets::Toggle::kBackgroundDisabled));
        colours_[SkinColourId::kToggleBorderDisabled] = juce::Colour(accessColour(Widgets::Toggle::kBorderDisabled));
        colours_[SkinColourId::kToggleTextDisabled] = juce::Colour(accessColour(Widgets::Toggle::kTextDisabled));
    }
    
    template <typename Accessor>
    void Skin::initializeSliderColours(Accessor accessColour)
    {
        using namespace SkinColours;
        
        colours_[SkinColourId::kSliderTrackEnabled] = juce::Colour(accessColour(Widgets::Slider::kTrack));
        colours_[SkinColourId::kSliderTrackDisabled] = juce::Colour(accessColour(Widgets::Slider::kTrackDisabled));
        colours_[SkinColourId::kSliderBackgroundDisabled] = juce::Colour(accessColour(Widgets::Slider::kBackgroundDisabled));
        colours_[SkinColourId::kSliderValueBarEnabled] = juce::Colour(accessColour(Widgets::Slider::kValueBar));
        colours_[SkinColourId::kSliderValueBarDisabled] = juce::Colour(accessColour(Widgets::Slider::kValueBarDisabled));
        colours_[SkinColourId::kSliderTextEnabled] = juce::Colour(accessColour(Widgets::Slider::kText));
        colours_[SkinColourId::kSliderTextDisabled] = juce::Colour(accessColour(Widgets::Slider::kTextDisabled));
        colours_[SkinColourId::kSliderFocusBorder] = juce::Colour(accessColour(Widgets::Slider::kFocusBorder));
    }
    
    template <typename Accessor>
    void Skin::initializeComboBoxColours(Accessor accessColour)
    {
        using namespace SkinColours;
        
        colours_[SkinColourId::kComboBoxBackgroundEnabled] = juce::Colour(accessColour(Widgets::ComboBox::Standard::kBackground));
        colours_[SkinColourId::kComboBoxBackgroundDisabled] = juce::Colour(accessColour(Widgets::ComboBox::Standard::kBackgroundDisabled));
        colours_[SkinColourId::kComboBoxBorderEnabled] = juce::Colour(accessColour(Widgets::ComboBox::ButtonLike::kBorder));
        colours_[SkinColourId::kComboBoxBorderDisabled] = juce::Colour(accessColour(Widgets::ComboBox::ButtonLike::kBorderDisabled));
        colours_[SkinColourId::kComboBoxFocusBorder] = juce::Colour(accessColour(Widgets::ComboBox::Standard::kFocusBorder));
        colours_[SkinColourId::kComboBoxTriangleEnabled] = juce::Colour(accessColour(Widgets::ComboBox::Standard::kTriangle));
        colours_[SkinColourId::kComboBoxTriangleDisabled] = juce::Colour(accessColour(Widgets::ComboBox::Standard::kTriangleDisabled));
        colours_[SkinColourId::kComboBoxTextEnabled] = juce::Colour(accessColour(Widgets::ComboBox::Standard::kText));
        colours_[SkinColourId::kComboBoxTextDisabled] = juce::Colour(accessColour(Widgets::ComboBox::Standard::kTextDisabled));
        colours_[SkinColourId::kComboBoxButtonLikeBackground] = juce::Colour(accessColour(Widgets::ComboBox::ButtonLike::kBackground));
        colours_[SkinColourId::kComboBoxButtonLikeBackgroundDisabled] = juce::Colour(accessColour(Widgets::ComboBox::ButtonLike::kBackgroundDisabled));
        colours_[SkinColourId::kComboBoxButtonLikeBorder] = juce::Colour(accessColour(Widgets::ComboBox::ButtonLike::kBorder));
        colours_[SkinColourId::kComboBoxButtonLikeBorderDisabled] = juce::Colour(accessColour(Widgets::ComboBox::ButtonLike::kBorderDisabled));
        colours_[SkinColourId::kComboBoxButtonLikeText] = juce::Colour(accessColour(Widgets::ComboBox::ButtonLike::kText));
        colours_[SkinColourId::kComboBoxButtonLikeTextDisabled] = juce::Colour(accessColour(Widgets::ComboBox::ButtonLike::kTextDisabled));
        colours_[SkinColourId::kComboBoxButtonLikeTriangle] = juce::Colour(accessColour(Widgets::ComboBox::ButtonLike::kTriangle));
        colours_[SkinColourId::kComboBoxButtonLikeTriangleDisabled] = juce::Colour(accessColour(Widgets::ComboBox::ButtonLike::kTriangleDisabled));
    }
    
    template <typename Accessor>
    void Skin::initializePopupMenuColours(Accessor accessColour)
    {
        using namespace SkinColours;
        
        colours_[SkinColourId::kPopupMenuBackground] = juce::Colour(accessColour(Widgets::PopupMenu::Standard::kBackground));
        colours_[SkinColourId::kPopupMenuBorder] = juce::Colour(accessColour(Widgets::PopupMenu::Standard::kBorder));
        colours_[SkinColourId::kPopupMenuSeparator] = juce::Colour(accessColour(Widgets::PopupMenu::Standard::kSeparator));
        colours_[SkinColourId::kPopupMenuText] = juce::Colour(accessColour(Widgets::PopupMenu::Standard::kText));
        colours_[SkinColourId::kPopupMenuBackgroundHover] = juce::Colour(accessColour(Widgets::PopupMenu::Standard::kBackgroundHoover));
        colours_[SkinColourId::kPopupMenuTextHover] = juce::Colour(accessColour(Widgets::PopupMenu::Standard::kTextHoover));
        colours_[SkinColourId::kPopupMenuScrollbar] = juce::Colour(accessColour(Widgets::PopupMenu::ButtonLike::kScrollbar));
        colours_[SkinColourId::kPopupMenuBackgroundButtonLike] = juce::Colour(accessColour(Widgets::PopupMenu::ButtonLike::kBackground));
        colours_[SkinColourId::kPopupMenuBorderButtonLike] = juce::Colour(accessColour(Widgets::PopupMenu::ButtonLike::kBorder));
        colours_[SkinColourId::kPopupMenuSeparatorButtonLike] = juce::Colour(accessColour(Widgets::PopupMenu::ButtonLike::kSeparator));
        colours_[SkinColourId::kPopupMenuTextButtonLike] = juce::Colour(accessColour(Widgets::PopupMenu::ButtonLike::kText));
        colours_[SkinColourId::kPopupMenuBackgroundHoverButtonLike] = juce::Colour(accessColour(Widgets::PopupMenu::ButtonLike::kBackgroundHoover));
        colours_[SkinColourId::kPopupMenuTextHoverButtonLike] = juce::Colour(accessColour(Widgets::PopupMenu::ButtonLike::kTextHoover));
        colours_[SkinColourId::kPopupMenuScrollbarButtonLike] = juce::Colour(accessColour(Widgets::PopupMenu::ButtonLike::kScrollbar));
    }
    
    template <typename Accessor>
    void Skin::initializeDisplayColours(Accessor accessColour)
    {
        using namespace SkinColours;
        
        colours_[SkinColourId::kNumberBoxText] = juce::Colour(accessColour(Widgets::NumberBox::kText));
        colours_[SkinColourId::kNumberBoxDot] = juce::Colour(accessColour(Widgets::NumberBox::kDot));
        colours_[SkinColourId::kNumberBoxEditorBackground] = juce::Colour(accessColour(Widgets::NumberBox::kEditorBackground));
        colours_[SkinColourId::kNumberBoxEditorSelectionBackground] = juce::Colour(accessColour(Widgets::NumberBox::kEditorSelectionBackground));
        colours_[SkinColourId::kNumberBoxEditorText] = juce::Colour(accessColour(Widgets::NumberBox::kEditorText));

        colours_[SkinColourId::kEnvelopeDisplayBackground] = juce::Colour(accessColour(Widgets::EnvelopeDisplay::kBackground));
        colours_[SkinColourId::kEnvelopeDisplayBorder] = juce::Colour(accessColour(Widgets::EnvelopeDisplay::kBorder));
        colours_[SkinColourId::kEnvelopeDisplayEnvelope] = juce::Colour(accessColour(Widgets::EnvelopeDisplay::kEnvelope));

        colours_[SkinColourId::kPatchNameDisplayBackground] = juce::Colour(accessColour(Widgets::PatchNameDisplay::kBackground));
        colours_[SkinColourId::kPatchNameDisplayBorder] = juce::Colour(accessColour(Widgets::PatchNameDisplay::kBorder));
        colours_[SkinColourId::kPatchNameDisplayText] = juce::Colour(accessColour(Widgets::PatchNameDisplay::kText));

        colours_[SkinColourId::kTrackGeneratorDisplayBackground] = juce::Colour(accessColour(Widgets::TrackGeneratorDisplay::kBackground));
        colours_[SkinColourId::kTrackGeneratorDisplayBorder] = juce::Colour(accessColour(Widgets::TrackGeneratorDisplay::kBorder));
        colours_[SkinColourId::kTrackGeneratorDisplayShaper] = juce::Colour(accessColour(Widgets::TrackGeneratorDisplay::kShaper));
    }
}
