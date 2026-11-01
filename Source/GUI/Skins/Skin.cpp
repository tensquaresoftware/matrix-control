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

    juce::Colour Skin::getColour(SkinObjectType object,
                                 SkinColourProperty property,
                                 SkinColourState state,
                                 ColourSet colourSet) const
    {
        return getColour(skinColourKey(object, property, state, colourSet));
    }

    juce::Colour Skin::getColour(SkinColourKey key) const
    {
        const auto it = colours_.find(key);
        if (it != colours_.end())
            return it->second;

        if (key.colourSet != ColourSet::k1)
        {
            key.colourSet = ColourSet::k1;
            const auto fallback = colours_.find(key);
            if (fallback != colours_.end())
                return fallback->second;
        }

        jassertfalse;
        DBG("Missing skin colour key - object/property/state/colourSet not in map");
        return juce::Colour(ColourChart::kWhite);
    }

    void Skin::setColour(SkinColourKey key, juce::Colour colour)
    {
        colours_[key] = colour;
    }

    void Skin::clearHigherColourSets()
    {
        for (auto it = colours_.begin(); it != colours_.end();)
        {
            if (it->first.colourSet != ColourSet::k1)
                it = colours_.erase(it);
            else
                ++it;
        }
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

    juce::Font Skin::getModalBodyFont() const
    {
        const auto typeface = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::MontserratRegular_ttf,
            static_cast<size_t>(PluginFontsData::MontserratRegular_ttfSize)
        );
        return juce::Font(juce::FontOptions(typeface).withHeight(kSkinBaseFontHeight_));
    }

    juce::Font Skin::getModalBodyFontBold() const
    {
        const auto typeface = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::MontserratBold_ttf,
            static_cast<size_t>(PluginFontsData::MontserratBold_ttfSize)
        );
        return juce::Font(juce::FontOptions(typeface).withHeight(kSkinBaseFontHeight_));
    }

    void Skin::initializeDefaultColours()
    {
        if (currentVariant_ == ColourVariant::Black)
            initializeVariantColours([](const auto& el) { return el.blackVariant; });
        else
            initializeVariantColours([](const auto& el) { return el.creamVariant; });
    }

    template <typename Accessor>
    void Skin::putColour(Accessor accessColour, const SkinColours::ColourElement& element)
    {
        const auto colour = juce::Colour(accessColour(element));
        colours_[element.key] = colour;
        if (! supportsMultipleColourSets(element.key.object))
            return;

        for (const auto colourSet : kAllColourSets)
        {
            auto key = element.key;
            key.colourSet = colourSet;
            colours_[key] = colour;
        }
    }

    template <typename Accessor>
    void Skin::putColourExact(Accessor accessColour, const SkinColours::ColourElement& element)
    {
        colours_[element.key] = juce::Colour(accessColour(element));
    }

    template <typename Accessor>
    void Skin::overlayFormerButtonLikeAtK4(Accessor accessColour)
    {
        using namespace SkinColours;

        putColourExact(accessColour, Widgets::SliderK4::kTrack);
        putColourExact(accessColour, Widgets::SliderK4::kFocusBorder);
        putColourExact(accessColour, Widgets::SliderK4::kValueBar);
        putColourExact(accessColour, Widgets::SliderK4::kText);

        putColourExact(accessColour, Widgets::ComboBoxK4::kBackground);
        putColourExact(accessColour, Widgets::ComboBoxK4::kBackgroundDisabled);
        putColourExact(accessColour, Widgets::ComboBoxK4::kBorder);
        putColourExact(accessColour, Widgets::ComboBoxK4::kBorderDisabled);
        putColourExact(accessColour, Widgets::ComboBoxK4::kFocusBorder);
        putColourExact(accessColour, Widgets::ComboBoxK4::kText);
        putColourExact(accessColour, Widgets::ComboBoxK4::kTextDisabled);
        putColourExact(accessColour, Widgets::ComboBoxK4::kTriangle);
        putColourExact(accessColour, Widgets::ComboBoxK4::kTriangleDisabled);

        putColourExact(accessColour, Widgets::PopupMenuK4::kBackground);
        putColourExact(accessColour, Widgets::PopupMenuK4::kBorder);
        putColourExact(accessColour, Widgets::PopupMenuK4::kSeparator);
        putColourExact(accessColour, Widgets::PopupMenuK4::kText);
        putColourExact(accessColour, Widgets::PopupMenuK4::kTextDisabled);
        putColourExact(accessColour, Widgets::PopupMenuK4::kBackgroundHover);
        putColourExact(accessColour, Widgets::PopupMenuK4::kTextHover);
        putColourExact(accessColour, Widgets::PopupMenuK4::kScrollbar);
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
        overlayFormerButtonLikeAtK4(accessColour);
    }

    template <typename Accessor>
    void Skin::initializePanelColours(Accessor accessColour)
    {
        using namespace SkinColours;

        putColour(accessColour, Panels::kHeaderPanelBackground);
        putColour(accessColour, Panels::kBodyPanelBackground);
        putColour(accessColour, Panels::kFooterPanelBackground);
        putColour(accessColour, Panels::kHeaderPanelBorder);
        putColour(accessColour, Panels::kFooterPanelBorder);
        putColour(accessColour, Panels::kFooterMessageErrorBackground);
        putColour(accessColour, Panels::kDialogBorder);
        putColour(accessColour, Panels::kDialogTitleBand);
        putColour(accessColour, Panels::kWindowCloseGlyph);
        putColour(accessColour, Widgets::DarkPanel::kText);
        putColour(accessColour, Widgets::DarkPanel::kTextHover);
        putColour(accessColour, Widgets::FooterMessage::kMessageInfo);
        putColour(accessColour, Widgets::FooterMessage::kMessageWarning);
        putColour(accessColour, Widgets::FooterMessage::kMessageError);
    }

    template <typename Accessor>
    void Skin::initializeLayoutColours(Accessor accessColour)
    {
        using namespace SkinColours;

        putColour(accessColour, Widgets::SectionHeader::kText);
        putColour(accessColour, Widgets::SectionHeader::kLineBlue);
        putColour(accessColour, Widgets::SectionHeader::kLineOrange);
        putColour(accessColour, Widgets::ModuleHeader::kText);
        putColour(accessColour, Widgets::ModuleHeader::kLineBlue);
        putColour(accessColour, Widgets::ModuleHeader::kLineOrange);
        putColour(accessColour, Widgets::GroupLabel::kText);
        putColour(accessColour, Widgets::GroupLabel::kLine);
        putColour(accessColour, Widgets::Label::kText);
        putColour(accessColour, Widgets::HorizontalSeparator::kLine);
    }

    template <typename Accessor>
    void Skin::initializeButtonColours(Accessor accessColour)
    {
        using namespace SkinColours;

        putColour(accessColour, Widgets::Button::kBackgroundOff);
        putColour(accessColour, Widgets::Button::kBorderOff);
        putColour(accessColour, Widgets::Button::kTextOff);
        putColour(accessColour, Widgets::Button::kBackgroundOn);
        putColour(accessColour, Widgets::Button::kBorderOn);
        putColour(accessColour, Widgets::Button::kTextOn);
        putColour(accessColour, Widgets::Button::kBackgroundHover);
        putColour(accessColour, Widgets::Button::kTextHover);
        putColour(accessColour, Widgets::Button::kBackgroundClicked);
        putColour(accessColour, Widgets::Button::kTextClicked);
        putColour(accessColour, Widgets::Button::kBackgroundDisabled);
        putColour(accessColour, Widgets::Button::kBorderDisabled);
        putColour(accessColour, Widgets::Button::kTextDisabled);
    }

    template <typename Accessor>
    void Skin::initializeToggleColours(Accessor accessColour)
    {
        using namespace SkinColours;

        putColour(accessColour, Widgets::Toggle::kBorder);
        putColour(accessColour, Widgets::Toggle::kBackgroundOff);
        putColour(accessColour, Widgets::Toggle::kTextOff);
        putColour(accessColour, Widgets::Toggle::kBackgroundOn);
        putColour(accessColour, Widgets::Toggle::kTextOn);
        putColour(accessColour, Widgets::Toggle::kBackgroundDisabled);
        putColour(accessColour, Widgets::Toggle::kBorderDisabled);
        putColour(accessColour, Widgets::Toggle::kTextDisabled);
    }

    template <typename Accessor>
    void Skin::initializeSliderColours(Accessor accessColour)
    {
        using namespace SkinColours;

        putColour(accessColour, Widgets::Slider::kTrack);
        putColour(accessColour, Widgets::Slider::kTrackDisabled);
        putColour(accessColour, Widgets::Slider::kBackgroundDisabled);
        putColour(accessColour, Widgets::Slider::kValueBar);
        putColour(accessColour, Widgets::Slider::kValueBarDisabled);
        putColour(accessColour, Widgets::Slider::kText);
        putColour(accessColour, Widgets::Slider::kTextDisabled);
        putColour(accessColour, Widgets::Slider::kFocusBorder);
    }

    template <typename Accessor>
    void Skin::initializeComboBoxColours(Accessor accessColour)
    {
        using namespace SkinColours;

        putColour(accessColour, Widgets::ComboBox::kBackground);
        putColour(accessColour, Widgets::ComboBox::kBackgroundDisabled);
        putColour(accessColour, Widgets::ComboBox::kBorder);
        putColour(accessColour, Widgets::ComboBox::kBorderDisabled);
        putColour(accessColour, Widgets::ComboBox::kFocusBorder);
        putColour(accessColour, Widgets::ComboBox::kTriangle);
        putColour(accessColour, Widgets::ComboBox::kTriangleDisabled);
        putColour(accessColour, Widgets::ComboBox::kText);
        putColour(accessColour, Widgets::ComboBox::kTextDisabled);
    }

    template <typename Accessor>
    void Skin::initializePopupMenuColours(Accessor accessColour)
    {
        using namespace SkinColours;

        putColour(accessColour, Widgets::PopupMenu::kBackground);
        putColour(accessColour, Widgets::PopupMenu::kBorder);
        putColour(accessColour, Widgets::PopupMenu::kSeparator);
        putColour(accessColour, Widgets::PopupMenu::kText);
        putColour(accessColour, Widgets::PopupMenu::kTextDisabled);
        putColour(accessColour, Widgets::PopupMenu::kBackgroundHover);
        putColour(accessColour, Widgets::PopupMenu::kTextHover);
        putColour(accessColour, Widgets::PopupMenu::kScrollbar);
    }

    template <typename Accessor>
    void Skin::initializeDisplayColours(Accessor accessColour)
    {
        using namespace SkinColours;

        putColour(accessColour, Widgets::NumberBox::kBackground);
        putColour(accessColour, Widgets::NumberBox::kBackgroundDisabled);
        putColour(accessColour, Widgets::NumberBox::kText);
        putColour(accessColour, Widgets::NumberBox::kTextFocus);
        putColour(accessColour, Widgets::NumberBox::kEditorBackground);
        putColour(accessColour, Widgets::NumberBox::kEditorSelectionBackground);
        putColour(accessColour, Widgets::NumberBox::kEditorText);
        putColour(accessColour, Widgets::EnvelopeDisplay::kBackground);
        putColour(accessColour, Widgets::EnvelopeDisplay::kBorder);
        putColour(accessColour, Widgets::EnvelopeDisplay::kEnvelope);
        putColour(accessColour, Widgets::PatchNameDisplay::kBackground);
        putColour(accessColour, Widgets::PatchNameDisplay::kBorder);
        putColour(accessColour, Widgets::PatchNameDisplay::kText);
        putColour(accessColour, Widgets::PatchNameDisplay::kSecondaryText);
        putColour(accessColour, Widgets::PatchNameDisplay::kCaret);
        putColour(accessColour, Widgets::TrackGeneratorDisplay::kBackground);
        putColour(accessColour, Widgets::TrackGeneratorDisplay::kBorder);
        putColour(accessColour, Widgets::TrackGeneratorDisplay::kShaper);
        putColour(accessColour, Widgets::ReadOnlyValueField::kBackground);
        putColour(accessColour, Widgets::ReadOnlyValueField::kText);
        putColour(accessColour, Widgets::ReadOnlyValueField::kTextError);
        putColour(accessColour, Widgets::Led::kBackgroundOff);
        putColour(accessColour, Widgets::Led::kBorderOff);
        putColour(accessColour, Widgets::Led::kMaxOn);
        putColour(accessColour, Widgets::PeakIndicator::kBackgroundOff);
        putColour(accessColour, Widgets::PeakIndicator::kBorderOff);
        putColour(accessColour, Widgets::PeakIndicator::kMaxOn);
    }
}
