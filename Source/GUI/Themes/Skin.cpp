#include "Skin.h"

#include "SkinColours.h"
#include "SkinValues.h"

#include <PluginFontsData.h>

namespace tss
{
    Skin::Skin(ColourVariant variant)
        : currentVariant_(variant)
    {
        initializeDefaultValues();
        initializeDefaultColours();
    }

    std::unique_ptr<Skin> Skin::create(ColourVariant variant)
    {
        return std::make_unique<Skin>(variant);
    }

    float Skin::getValue(SkinValueId valueId) const
    {
        const auto it = values_.find(valueId);
        if (it != values_.end())
            return it->second;

        const int index = static_cast<int>(valueId);
        if (index >= 0 && index < static_cast<int>(SkinValueId::kNumValues))
            return kDefaultValues[index];

        return 0.0f;
    }

    juce::Colour Skin::getColour(SkinColourId colourId) const
    {
        const auto it = colours_.find(colourId);
        if (it != colours_.end())
            return it->second;

        return juce::Colours::white;
    }

    void Skin::setValue(SkinValueId valueId, float value)
    {
        values_[valueId] = value;
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
        return juce::Font(juce::FontOptions().withTypeface(typeface).withHeight(kBaseFontHeight));
    }

    juce::Font Skin::getBaseFontBold() const
    {
        const auto typeface = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::PTSansNarrowBold_ttf,
            static_cast<size_t>(PluginFontsData::PTSansNarrowBold_ttfSize)
        );
        return juce::Font(juce::FontOptions().withTypeface(typeface).withHeight(kBaseFontHeight));
    }

    juce::Colour Skin::getHeaderPanelBackgroundColour() const
    {
        return getColour(SkinColourId::kHeaderPanelBackground);
    }

    juce::Colour Skin::getBodyPanelBackgroundColour() const
    {
        return getColour(SkinColourId::kBodyPanelBackground);
    }

    juce::Colour Skin::getFooterPanelBackgroundColour() const
    {
        return getColour(SkinColourId::kFooterPanelBackground);
    }

    juce::Colour Skin::getSectionHeaderTextColour() const
    {
        return getColour(SkinColourId::kSectionHeaderText);
    }

    juce::Colour Skin::getSectionHeaderLineColourBlue() const
    {
        return getColour(SkinColourId::kSectionHeaderLineBlue);
    }

    juce::Colour Skin::getSectionHeaderLineColourOrange() const
    {
        return getColour(SkinColourId::kSectionHeaderLineOrange);
    }

    juce::Colour Skin::getModuleHeaderTextColour() const
    {
        return getColour(SkinColourId::kModuleHeaderText);
    }

    juce::Colour Skin::getModuleHeaderLineColourBlue() const
    {
        return getColour(SkinColourId::kModuleHeaderLineBlue);
    }

    juce::Colour Skin::getModuleHeaderLineColourOrange() const
    {
        return getColour(SkinColourId::kModuleHeaderLineOrange);
    }

    juce::Colour Skin::getGroupLabelTextColour() const
    {
        return getColour(SkinColourId::kGroupLabelText);
    }

    juce::Colour Skin::getGroupLabelLineColour() const
    {
        return getColour(SkinColourId::kGroupLabelLine);
    }

    juce::Colour Skin::getLabelTextColour() const
    {
        return getColour(SkinColourId::kLabelText);
    }

    juce::Colour Skin::getVerticalSeparatorLineColour() const
    {
        return getColour(SkinColourId::kVerticalSeparatorLine);
    }

    juce::Colour Skin::getHorizontalSeparatorLineColour() const
    {
        return getColour(SkinColourId::kHorizontalSeparatorLine);
    }

    juce::Colour Skin::getButtonBackgroundColourOff() const
    {
        return getColour(SkinColourId::kButtonBackgroundOff);
    }

    juce::Colour Skin::getButtonBorderColourOff() const
    {
        return getColour(SkinColourId::kButtonBorderOff);
    }

    juce::Colour Skin::getButtonTextColourOff() const
    {
        return getColour(SkinColourId::kButtonTextOff);
    }

    juce::Colour Skin::getButtonBackgroundColourOn() const
    {
        return getColour(SkinColourId::kButtonBackgroundOn);
    }

    juce::Colour Skin::getButtonBorderColourOn() const
    {
        return getColour(SkinColourId::kButtonBorderOn);
    }

    juce::Colour Skin::getButtonTextColourOn() const
    {
        return getColour(SkinColourId::kButtonTextOn);
    }

    juce::Colour Skin::getButtonBackgroundColourHoover() const
    {
        return getColour(SkinColourId::kButtonBackgroundHover);
    }

    juce::Colour Skin::getButtonTextColourHoover() const
    {
        return getColour(SkinColourId::kButtonTextHover);
    }

    juce::Colour Skin::getButtonBackgroundColourClicked() const
    {
        return getColour(SkinColourId::kButtonBackgroundClicked);
    }

    juce::Colour Skin::getButtonTextColourClicked() const
    {
        return getColour(SkinColourId::kButtonTextClicked);
    }

    juce::Colour Skin::getToggleBorderColour() const
    {
        return getColour(SkinColourId::kToggleBorder);
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

    juce::Colour Skin::getSliderFocusBorderColour() const
    {
        return getColour(SkinColourId::kSliderFocusBorder);
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

    juce::Colour Skin::getNumberBoxTextColour() const
    {
        return getColour(SkinColourId::kNumberBoxText);
    }

    juce::Colour Skin::getNumberBoxDotColour() const
    {
        return getColour(SkinColourId::kNumberBoxDot);
    }

    juce::Colour Skin::getNumberBoxEditorBackgroundColour() const
    {
        return getColour(SkinColourId::kNumberBoxEditorBackground);
    }

    juce::Colour Skin::getNumberBoxEditorSelectionBackgroundColour() const
    {
        return getColour(SkinColourId::kNumberBoxEditorSelectionBackground);
    }

    juce::Colour Skin::getNumberBoxEditorTextColour() const
    {
        return getColour(SkinColourId::kNumberBoxEditorText);
    }

    juce::Colour Skin::getEnvelopeDisplayBackgroundColour() const
    {
        return getColour(SkinColourId::kEnvelopeDisplayBackground);
    }

    juce::Colour Skin::getEnvelopeDisplayBorderColour() const
    {
        return getColour(SkinColourId::kEnvelopeDisplayBorder);
    }

    juce::Colour Skin::getEnvelopeDisplayEnvelopeColour() const
    {
        return getColour(SkinColourId::kEnvelopeDisplayEnvelope);
    }

    juce::Colour Skin::getPatchNameDisplayBackgroundColour() const
    {
        return getColour(SkinColourId::kPatchNameDisplayBackground);
    }

    juce::Colour Skin::getPatchNameDisplayBorderColour() const
    {
        return getColour(SkinColourId::kPatchNameDisplayBorder);
    }

    juce::Colour Skin::getPatchNameDisplayTextColour() const
    {
        return getColour(SkinColourId::kPatchNameDisplayText);
    }

    juce::Colour Skin::getTrackGeneratorDisplayBackgroundColour() const
    {
        return getColour(SkinColourId::kTrackGeneratorDisplayBackground);
    }

    juce::Colour Skin::getTrackGeneratorDisplayBorderColour() const
    {
        return getColour(SkinColourId::kTrackGeneratorDisplayBorder);
    }

    juce::Colour Skin::getTrackGeneratorDisplayShaperColour() const
    {
        return getColour(SkinColourId::kTrackGeneratorDisplayShaper);
    }

    void Skin::initializeDefaultValues()
    {
        for (int i = 0; i < static_cast<int>(SkinValueId::kNumValues); ++i)
        {
            values_[static_cast<SkinValueId>(i)] = kDefaultValues[i];
        }
    }

    void Skin::initializeDefaultColours()
    {
        if (currentVariant_ == ColourVariant::Black)
            initializeBlackVariantColours();
        else
            initializeCreamVariantColours();
    }

    void Skin::initializeBlackVariantColours()
    {
        using namespace SkinColours;

        colours_[SkinColourId::kHeaderPanelBackground] = juce::Colour(Panels::kHeaderPanelBackground.blackVariant);
        colours_[SkinColourId::kBodyPanelBackground] = juce::Colour(Panels::kBodyPanelBackground.blackVariant);
        colours_[SkinColourId::kFooterPanelBackground] = juce::Colour(Panels::kFooterPanelBackground.blackVariant);

        colours_[SkinColourId::kSectionHeaderText] = juce::Colour(Widgets::SectionHeader::kText.blackVariant);
        colours_[SkinColourId::kSectionHeaderLineBlue] = juce::Colour(Widgets::SectionHeader::kLineBlue.blackVariant);
        colours_[SkinColourId::kSectionHeaderLineOrange] = juce::Colour(Widgets::SectionHeader::kLineOrange.blackVariant);

        colours_[SkinColourId::kModuleHeaderText] = juce::Colour(Widgets::ModuleHeader::kText.blackVariant);
        colours_[SkinColourId::kModuleHeaderLineBlue] = juce::Colour(Widgets::ModuleHeader::kLineBlue.blackVariant);
        colours_[SkinColourId::kModuleHeaderLineOrange] = juce::Colour(Widgets::ModuleHeader::kLineOrange.blackVariant);

        colours_[SkinColourId::kGroupLabelText] = juce::Colour(Widgets::GroupLabel::kText.blackVariant);
        colours_[SkinColourId::kGroupLabelLine] = juce::Colour(Widgets::GroupLabel::kLine.blackVariant);

        colours_[SkinColourId::kLabelText] = juce::Colour(Widgets::Label::kText.blackVariant);

        colours_[SkinColourId::kVerticalSeparatorLine] = juce::Colour(Widgets::VerticalSeparator::kLine.blackVariant);
        colours_[SkinColourId::kHorizontalSeparatorLine] = juce::Colour(Widgets::HorizontalSeparator::kLine.blackVariant);

        colours_[SkinColourId::kButtonBackgroundOff] = juce::Colour(Widgets::Button::kBackground.blackVariant);
        colours_[SkinColourId::kButtonBorderOff] = juce::Colour(Widgets::Button::kBorder.blackVariant);
        colours_[SkinColourId::kButtonTextOff] = juce::Colour(Widgets::Button::kText.blackVariant);
        colours_[SkinColourId::kButtonBackgroundOn] = juce::Colour(Widgets::Button::kBackground.blackVariant);
        colours_[SkinColourId::kButtonBorderOn] = juce::Colour(Widgets::Button::kBorder.blackVariant);
        colours_[SkinColourId::kButtonTextOn] = juce::Colour(Widgets::Button::kText.blackVariant);
        colours_[SkinColourId::kButtonBackgroundHover] = juce::Colour(Widgets::Button::kBackgroundHoover.blackVariant);
        colours_[SkinColourId::kButtonTextHover] = juce::Colour(Widgets::Button::kTextHoover.blackVariant);
        colours_[SkinColourId::kButtonBackgroundClicked] = juce::Colour(Widgets::Button::kBackgroundClicked.blackVariant);
        colours_[SkinColourId::kButtonTextClicked] = juce::Colour(Widgets::Button::kTextClicked.blackVariant);

        colours_[SkinColourId::kToggleBorder] = juce::Colour(Widgets::Toggle::kBorder.blackVariant);
        colours_[SkinColourId::kToggleBackgroundOff] = juce::Colour(Widgets::Toggle::kBackgroundOff.blackVariant);
        colours_[SkinColourId::kToggleTextOff] = juce::Colour(Widgets::Toggle::kTextOff.blackVariant);
        colours_[SkinColourId::kToggleBackgroundOn] = juce::Colour(Widgets::Toggle::kBackgroundOn.blackVariant);
        colours_[SkinColourId::kToggleTextOn] = juce::Colour(Widgets::Toggle::kTextOn.blackVariant);

        colours_[SkinColourId::kSliderTrackEnabled] = juce::Colour(Widgets::Slider::kTrack.blackVariant);
        colours_[SkinColourId::kSliderTrackDisabled] = juce::Colour(Widgets::Slider::kTrackDisabled.blackVariant);
        colours_[SkinColourId::kSliderValueBarEnabled] = juce::Colour(Widgets::Slider::kValueBar.blackVariant);
        colours_[SkinColourId::kSliderValueBarDisabled] = juce::Colour(Widgets::Slider::kValueBarDisabled.blackVariant);
        colours_[SkinColourId::kSliderTextEnabled] = juce::Colour(Widgets::Slider::kText.blackVariant);
        colours_[SkinColourId::kSliderTextDisabled] = juce::Colour(Widgets::Slider::kTextDisabled.blackVariant);
        colours_[SkinColourId::kSliderFocusBorder] = juce::Colour(Widgets::Slider::kFocusBorder.blackVariant);

        colours_[SkinColourId::kComboBoxBackgroundEnabled] = juce::Colour(Widgets::ComboBox::Standard::kBackground.blackVariant);
        colours_[SkinColourId::kComboBoxBackgroundDisabled] = juce::Colour(Widgets::ComboBox::Standard::kBackgroundDisabled.blackVariant);
        colours_[SkinColourId::kComboBoxBorderEnabled] = juce::Colour(Widgets::ComboBox::ButtonLike::kBorder.blackVariant);
        colours_[SkinColourId::kComboBoxBorderDisabled] = juce::Colour(Widgets::ComboBox::ButtonLike::kBorderDisabled.blackVariant);
        colours_[SkinColourId::kComboBoxFocusBorder] = juce::Colour(Widgets::ComboBox::Standard::kFocusBorder.blackVariant);
        colours_[SkinColourId::kComboBoxTriangleEnabled] = juce::Colour(Widgets::ComboBox::Standard::kTriangle.blackVariant);
        colours_[SkinColourId::kComboBoxTriangleDisabled] = juce::Colour(Widgets::ComboBox::Standard::kTriangleDisabled.blackVariant);
        colours_[SkinColourId::kComboBoxTextEnabled] = juce::Colour(Widgets::ComboBox::Standard::kText.blackVariant);
        colours_[SkinColourId::kComboBoxTextDisabled] = juce::Colour(Widgets::ComboBox::Standard::kTextDisabled.blackVariant);
        colours_[SkinColourId::kComboBoxButtonLikeBackground] = juce::Colour(Widgets::ComboBox::ButtonLike::kBackground.blackVariant);
        colours_[SkinColourId::kComboBoxButtonLikeBackgroundDisabled] = juce::Colour(Widgets::ComboBox::ButtonLike::kBackgroundDisabled.blackVariant);
        colours_[SkinColourId::kComboBoxButtonLikeBorder] = juce::Colour(Widgets::ComboBox::ButtonLike::kBorder.blackVariant);
        colours_[SkinColourId::kComboBoxButtonLikeBorderDisabled] = juce::Colour(Widgets::ComboBox::ButtonLike::kBorderDisabled.blackVariant);
        colours_[SkinColourId::kComboBoxButtonLikeText] = juce::Colour(Widgets::ComboBox::ButtonLike::kText.blackVariant);
        colours_[SkinColourId::kComboBoxButtonLikeTextDisabled] = juce::Colour(Widgets::ComboBox::ButtonLike::kTextDisabled.blackVariant);
        colours_[SkinColourId::kComboBoxButtonLikeTriangle] = juce::Colour(Widgets::ComboBox::ButtonLike::kTriangle.blackVariant);
        colours_[SkinColourId::kComboBoxButtonLikeTriangleDisabled] = juce::Colour(Widgets::ComboBox::ButtonLike::kTriangleDisabled.blackVariant);

        colours_[SkinColourId::kPopupMenuBackground] = juce::Colour(Widgets::PopupMenu::Standard::kBackground.blackVariant);
        colours_[SkinColourId::kPopupMenuBorder] = juce::Colour(Widgets::PopupMenu::Standard::kBorder.blackVariant);
        colours_[SkinColourId::kPopupMenuSeparator] = juce::Colour(Widgets::PopupMenu::Standard::kSeparator.blackVariant);
        colours_[SkinColourId::kPopupMenuText] = juce::Colour(Widgets::PopupMenu::Standard::kText.blackVariant);
        colours_[SkinColourId::kPopupMenuBackgroundHover] = juce::Colour(Widgets::PopupMenu::Standard::kBackgroundHoover.blackVariant);
        colours_[SkinColourId::kPopupMenuTextHover] = juce::Colour(Widgets::PopupMenu::Standard::kTextHoover.blackVariant);
        colours_[SkinColourId::kPopupMenuScrollbar] = juce::Colour(Widgets::PopupMenu::ButtonLike::kScrollbar.blackVariant);
        colours_[SkinColourId::kPopupMenuBackgroundButtonLike] = juce::Colour(Widgets::PopupMenu::ButtonLike::kBackground.blackVariant);
        colours_[SkinColourId::kPopupMenuBorderButtonLike] = juce::Colour(Widgets::PopupMenu::ButtonLike::kBorder.blackVariant);
        colours_[SkinColourId::kPopupMenuSeparatorButtonLike] = juce::Colour(Widgets::PopupMenu::ButtonLike::kSeparator.blackVariant);
        colours_[SkinColourId::kPopupMenuTextButtonLike] = juce::Colour(Widgets::PopupMenu::ButtonLike::kText.blackVariant);
        colours_[SkinColourId::kPopupMenuBackgroundHoverButtonLike] = juce::Colour(Widgets::PopupMenu::ButtonLike::kBackgroundHoover.blackVariant);
        colours_[SkinColourId::kPopupMenuTextHoverButtonLike] = juce::Colour(Widgets::PopupMenu::ButtonLike::kTextHoover.blackVariant);
        colours_[SkinColourId::kPopupMenuScrollbarButtonLike] = juce::Colour(Widgets::PopupMenu::ButtonLike::kScrollbar.blackVariant);

        colours_[SkinColourId::kNumberBoxText] = juce::Colour(Widgets::NumberBox::kText.blackVariant);
        colours_[SkinColourId::kNumberBoxDot] = juce::Colour(Widgets::NumberBox::kDot.blackVariant);
        colours_[SkinColourId::kNumberBoxEditorBackground] = juce::Colour(Widgets::NumberBox::kEditorBackground.blackVariant);
        colours_[SkinColourId::kNumberBoxEditorSelectionBackground] = juce::Colour(Widgets::NumberBox::kEditorSelectionBackground.blackVariant);
        colours_[SkinColourId::kNumberBoxEditorText] = juce::Colour(Widgets::NumberBox::kEditorText.blackVariant);

        colours_[SkinColourId::kEnvelopeDisplayBackground] = juce::Colour(Widgets::EnvelopeDisplay::kBackground.blackVariant);
        colours_[SkinColourId::kEnvelopeDisplayBorder] = juce::Colour(Widgets::EnvelopeDisplay::kBorder.blackVariant);
        colours_[SkinColourId::kEnvelopeDisplayEnvelope] = juce::Colour(Widgets::EnvelopeDisplay::kEnvelope.blackVariant);

        colours_[SkinColourId::kPatchNameDisplayBackground] = juce::Colour(Widgets::PatchNameDisplay::kBackground.blackVariant);
        colours_[SkinColourId::kPatchNameDisplayBorder] = juce::Colour(Widgets::PatchNameDisplay::kBorder.blackVariant);
        colours_[SkinColourId::kPatchNameDisplayText] = juce::Colour(Widgets::PatchNameDisplay::kText.blackVariant);

        colours_[SkinColourId::kTrackGeneratorDisplayBackground] = juce::Colour(Widgets::TrackGeneratorDisplay::kBackground.blackVariant);
        colours_[SkinColourId::kTrackGeneratorDisplayBorder] = juce::Colour(Widgets::TrackGeneratorDisplay::kBorder.blackVariant);
        colours_[SkinColourId::kTrackGeneratorDisplayShaper] = juce::Colour(Widgets::TrackGeneratorDisplay::kShaper.blackVariant);
    }

    void Skin::initializeCreamVariantColours()
    {
        using namespace SkinColours;

        colours_[SkinColourId::kHeaderPanelBackground] = juce::Colour(Panels::kHeaderPanelBackground.creamVariant);
        colours_[SkinColourId::kBodyPanelBackground] = juce::Colour(Panels::kBodyPanelBackground.creamVariant);
        colours_[SkinColourId::kFooterPanelBackground] = juce::Colour(Panels::kFooterPanelBackground.creamVariant);

        colours_[SkinColourId::kSectionHeaderText] = juce::Colour(Widgets::SectionHeader::kText.creamVariant);
        colours_[SkinColourId::kSectionHeaderLineBlue] = juce::Colour(Widgets::SectionHeader::kLineBlue.creamVariant);
        colours_[SkinColourId::kSectionHeaderLineOrange] = juce::Colour(Widgets::SectionHeader::kLineOrange.creamVariant);

        colours_[SkinColourId::kModuleHeaderText] = juce::Colour(Widgets::ModuleHeader::kText.creamVariant);
        colours_[SkinColourId::kModuleHeaderLineBlue] = juce::Colour(Widgets::ModuleHeader::kLineBlue.creamVariant);
        colours_[SkinColourId::kModuleHeaderLineOrange] = juce::Colour(Widgets::ModuleHeader::kLineOrange.creamVariant);

        colours_[SkinColourId::kGroupLabelText] = juce::Colour(Widgets::GroupLabel::kText.creamVariant);
        colours_[SkinColourId::kGroupLabelLine] = juce::Colour(Widgets::GroupLabel::kLine.creamVariant);

        colours_[SkinColourId::kLabelText] = juce::Colour(Widgets::Label::kText.creamVariant);

        colours_[SkinColourId::kVerticalSeparatorLine] = juce::Colour(Widgets::VerticalSeparator::kLine.creamVariant);
        colours_[SkinColourId::kHorizontalSeparatorLine] = juce::Colour(Widgets::HorizontalSeparator::kLine.creamVariant);

        colours_[SkinColourId::kButtonBackgroundOff] = juce::Colour(Widgets::Button::kBackground.creamVariant);
        colours_[SkinColourId::kButtonBorderOff] = juce::Colour(Widgets::Button::kBorder.creamVariant);
        colours_[SkinColourId::kButtonTextOff] = juce::Colour(Widgets::Button::kText.creamVariant);
        colours_[SkinColourId::kButtonBackgroundOn] = juce::Colour(Widgets::Button::kBackground.creamVariant);
        colours_[SkinColourId::kButtonBorderOn] = juce::Colour(Widgets::Button::kBorder.creamVariant);
        colours_[SkinColourId::kButtonTextOn] = juce::Colour(Widgets::Button::kText.creamVariant);
        colours_[SkinColourId::kButtonBackgroundHover] = juce::Colour(Widgets::Button::kBackgroundHoover.creamVariant);
        colours_[SkinColourId::kButtonTextHover] = juce::Colour(Widgets::Button::kTextHoover.creamVariant);
        colours_[SkinColourId::kButtonBackgroundClicked] = juce::Colour(Widgets::Button::kBackgroundClicked.creamVariant);
        colours_[SkinColourId::kButtonTextClicked] = juce::Colour(Widgets::Button::kTextClicked.creamVariant);

        colours_[SkinColourId::kToggleBorder] = juce::Colour(Widgets::Toggle::kBorder.creamVariant);
        colours_[SkinColourId::kToggleBackgroundOff] = juce::Colour(Widgets::Toggle::kBackgroundOff.creamVariant);
        colours_[SkinColourId::kToggleTextOff] = juce::Colour(Widgets::Toggle::kTextOff.creamVariant);
        colours_[SkinColourId::kToggleBackgroundOn] = juce::Colour(Widgets::Toggle::kBackgroundOn.creamVariant);
        colours_[SkinColourId::kToggleTextOn] = juce::Colour(Widgets::Toggle::kTextOn.creamVariant);

        colours_[SkinColourId::kSliderTrackEnabled] = juce::Colour(Widgets::Slider::kTrack.creamVariant);
        colours_[SkinColourId::kSliderTrackDisabled] = juce::Colour(Widgets::Slider::kTrackDisabled.creamVariant);
        colours_[SkinColourId::kSliderValueBarEnabled] = juce::Colour(Widgets::Slider::kValueBar.creamVariant);
        colours_[SkinColourId::kSliderValueBarDisabled] = juce::Colour(Widgets::Slider::kValueBarDisabled.creamVariant);
        colours_[SkinColourId::kSliderTextEnabled] = juce::Colour(Widgets::Slider::kText.creamVariant);
        colours_[SkinColourId::kSliderTextDisabled] = juce::Colour(Widgets::Slider::kTextDisabled.creamVariant);
        colours_[SkinColourId::kSliderFocusBorder] = juce::Colour(Widgets::Slider::kFocusBorder.creamVariant);

        colours_[SkinColourId::kComboBoxBackgroundEnabled] = juce::Colour(Widgets::ComboBox::Standard::kBackground.creamVariant);
        colours_[SkinColourId::kComboBoxBackgroundDisabled] = juce::Colour(Widgets::ComboBox::Standard::kBackgroundDisabled.creamVariant);
        colours_[SkinColourId::kComboBoxBorderEnabled] = juce::Colour(Widgets::ComboBox::ButtonLike::kBorder.creamVariant);
        colours_[SkinColourId::kComboBoxBorderDisabled] = juce::Colour(Widgets::ComboBox::ButtonLike::kBorderDisabled.creamVariant);
        colours_[SkinColourId::kComboBoxFocusBorder] = juce::Colour(Widgets::ComboBox::Standard::kFocusBorder.creamVariant);
        colours_[SkinColourId::kComboBoxTriangleEnabled] = juce::Colour(Widgets::ComboBox::Standard::kTriangle.creamVariant);
        colours_[SkinColourId::kComboBoxTriangleDisabled] = juce::Colour(Widgets::ComboBox::Standard::kTriangleDisabled.creamVariant);
        colours_[SkinColourId::kComboBoxTextEnabled] = juce::Colour(Widgets::ComboBox::Standard::kText.creamVariant);
        colours_[SkinColourId::kComboBoxTextDisabled] = juce::Colour(Widgets::ComboBox::Standard::kTextDisabled.creamVariant);
        colours_[SkinColourId::kComboBoxButtonLikeBackground] = juce::Colour(Widgets::ComboBox::ButtonLike::kBackground.creamVariant);
        colours_[SkinColourId::kComboBoxButtonLikeBackgroundDisabled] = juce::Colour(Widgets::ComboBox::ButtonLike::kBackgroundDisabled.creamVariant);
        colours_[SkinColourId::kComboBoxButtonLikeBorder] = juce::Colour(Widgets::ComboBox::ButtonLike::kBorder.creamVariant);
        colours_[SkinColourId::kComboBoxButtonLikeBorderDisabled] = juce::Colour(Widgets::ComboBox::ButtonLike::kBorderDisabled.creamVariant);
        colours_[SkinColourId::kComboBoxButtonLikeText] = juce::Colour(Widgets::ComboBox::ButtonLike::kText.creamVariant);
        colours_[SkinColourId::kComboBoxButtonLikeTextDisabled] = juce::Colour(Widgets::ComboBox::ButtonLike::kTextDisabled.creamVariant);
        colours_[SkinColourId::kComboBoxButtonLikeTriangle] = juce::Colour(Widgets::ComboBox::ButtonLike::kTriangle.creamVariant);
        colours_[SkinColourId::kComboBoxButtonLikeTriangleDisabled] = juce::Colour(Widgets::ComboBox::ButtonLike::kTriangleDisabled.creamVariant);

        colours_[SkinColourId::kPopupMenuBackground] = juce::Colour(Widgets::PopupMenu::Standard::kBackground.creamVariant);
        colours_[SkinColourId::kPopupMenuBorder] = juce::Colour(Widgets::PopupMenu::Standard::kBorder.creamVariant);
        colours_[SkinColourId::kPopupMenuSeparator] = juce::Colour(Widgets::PopupMenu::Standard::kSeparator.creamVariant);
        colours_[SkinColourId::kPopupMenuText] = juce::Colour(Widgets::PopupMenu::Standard::kText.creamVariant);
        colours_[SkinColourId::kPopupMenuBackgroundHover] = juce::Colour(Widgets::PopupMenu::Standard::kBackgroundHoover.creamVariant);
        colours_[SkinColourId::kPopupMenuTextHover] = juce::Colour(Widgets::PopupMenu::Standard::kTextHoover.creamVariant);
        colours_[SkinColourId::kPopupMenuScrollbar] = juce::Colour(Widgets::PopupMenu::ButtonLike::kScrollbar.creamVariant);
        colours_[SkinColourId::kPopupMenuBackgroundButtonLike] = juce::Colour(Widgets::PopupMenu::ButtonLike::kBackground.creamVariant);
        colours_[SkinColourId::kPopupMenuBorderButtonLike] = juce::Colour(Widgets::PopupMenu::ButtonLike::kBorder.creamVariant);
        colours_[SkinColourId::kPopupMenuSeparatorButtonLike] = juce::Colour(Widgets::PopupMenu::ButtonLike::kSeparator.creamVariant);
        colours_[SkinColourId::kPopupMenuTextButtonLike] = juce::Colour(Widgets::PopupMenu::ButtonLike::kText.creamVariant);
        colours_[SkinColourId::kPopupMenuBackgroundHoverButtonLike] = juce::Colour(Widgets::PopupMenu::ButtonLike::kBackgroundHoover.creamVariant);
        colours_[SkinColourId::kPopupMenuTextHoverButtonLike] = juce::Colour(Widgets::PopupMenu::ButtonLike::kTextHoover.creamVariant);
        colours_[SkinColourId::kPopupMenuScrollbarButtonLike] = juce::Colour(Widgets::PopupMenu::ButtonLike::kScrollbar.creamVariant);

        colours_[SkinColourId::kNumberBoxText] = juce::Colour(Widgets::NumberBox::kText.creamVariant);
        colours_[SkinColourId::kNumberBoxDot] = juce::Colour(Widgets::NumberBox::kDot.creamVariant);
        colours_[SkinColourId::kNumberBoxEditorBackground] = juce::Colour(Widgets::NumberBox::kEditorBackground.creamVariant);
        colours_[SkinColourId::kNumberBoxEditorSelectionBackground] = juce::Colour(Widgets::NumberBox::kEditorSelectionBackground.creamVariant);
        colours_[SkinColourId::kNumberBoxEditorText] = juce::Colour(Widgets::NumberBox::kEditorText.creamVariant);

        colours_[SkinColourId::kEnvelopeDisplayBackground] = juce::Colour(Widgets::EnvelopeDisplay::kBackground.creamVariant);
        colours_[SkinColourId::kEnvelopeDisplayBorder] = juce::Colour(Widgets::EnvelopeDisplay::kBorder.creamVariant);
        colours_[SkinColourId::kEnvelopeDisplayEnvelope] = juce::Colour(Widgets::EnvelopeDisplay::kEnvelope.creamVariant);

        colours_[SkinColourId::kPatchNameDisplayBackground] = juce::Colour(Widgets::PatchNameDisplay::kBackground.creamVariant);
        colours_[SkinColourId::kPatchNameDisplayBorder] = juce::Colour(Widgets::PatchNameDisplay::kBorder.creamVariant);
        colours_[SkinColourId::kPatchNameDisplayText] = juce::Colour(Widgets::PatchNameDisplay::kText.creamVariant);

        colours_[SkinColourId::kTrackGeneratorDisplayBackground] = juce::Colour(Widgets::TrackGeneratorDisplay::kBackground.creamVariant);
        colours_[SkinColourId::kTrackGeneratorDisplayBorder] = juce::Colour(Widgets::TrackGeneratorDisplay::kBorder.creamVariant);
        colours_[SkinColourId::kTrackGeneratorDisplayShaper] = juce::Colour(Widgets::TrackGeneratorDisplay::kShaper.creamVariant);
    }
}
