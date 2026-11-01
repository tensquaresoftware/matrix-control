#include "LookBuilders.h"

#include "GUI/Layout/Design/DesignPanels.h"
#include "GUI/Looks/TypographyStyles.h"
#include "GUI/Skins/ISkin.h"

namespace TSS
{
    namespace
    {
        juce::Font resolvedTypographyFont(const ISkin& skin, TypographyStyleId styleId)
        {
            const auto style = getTypographyStyle(styleId);
            const auto baseFont = style.useBoldFont ? skin.getBaseFontBold() : skin.getBaseFont();
            return baseFont.withHeight(style.fontHeight);
        }
    }

    LabelLook labelLookFromSkin(const ISkin& skin)
    {
        LabelLook look;
        look.text = skin.getColour(SkinObjectType::kLabel, SkinColourProperty::kText);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    LabelLook darkPanelLabelLookFromSkin(const ISkin& skin)
    {
        LabelLook look;
        look.text = skin.getColour(SkinObjectType::kDarkPanel, SkinColourProperty::kText);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    LabelLook brandLabelLookFromSkin(const ISkin& skin)
    {
        LabelLook look;
        look.text = skin.getColour(SkinObjectType::kDarkPanel, SkinColourProperty::kText);
        look.font = skin.getBrandFontBold().withHeight(static_cast<float>(Design::Panels::Header::kLogoFontHeight));
        return look;
    }

    HorizontalSeparatorLook horizontalSeparatorLookFromSkin(const ISkin& skin)
    {
        HorizontalSeparatorLook look;
        look.line = skin.getColour(SkinObjectType::kHorizontalSeparator, SkinColourProperty::kLine);
        return look;
    }

    EnvelopeDisplayLook envelopeDisplayLookFromSkin(const ISkin& skin)
    {
        EnvelopeDisplayLook look;
        look.background = skin.getColour(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kBackground);
        look.border = skin.getColour(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kBorder);
        look.envelope = skin.getColour(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kEnvelope);
        return look;
    }

    TrackGeneratorDisplayLook trackGeneratorDisplayLookFromSkin(const ISkin& skin)
    {
        TrackGeneratorDisplayLook look;
        look.background = skin.getColour(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kBackground);
        look.border = skin.getColour(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kBorder);
        look.curve = skin.getColour(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kShaper);
        return look;
    }

    PatchNameDisplayLook patchNameDisplayLookFromSkin(const ISkin& skin)
    {
        // Secondary Mutator/Compare label uses a solid map entry (no withAlpha shortcut).
        // Primary idle/hover alphas (80% / 100%) stay paint-owned for the editability affordance.
        constexpr float kSecondaryFontHeightRatio = 0.7f;

        PatchNameDisplayLook look;
        look.background = skin.getColour(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kBackground);
        look.border = skin.getColour(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kBorder);
        look.text = skin.getColour(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kText);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kPatchName);
        look.secondaryText = skin.getColour(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kSecondaryText);
        look.secondaryFont = look.font.withHeight(look.font.getHeight() * kSecondaryFontHeightRatio);
        look.caretInk = skin.getColour(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kCaret);
        return look;
    }

    GroupLabelLook groupLabelLookFromSkin(const ISkin& skin)
    {
        GroupLabelLook look;
        look.text = skin.getColour(SkinObjectType::kGroupLabel, SkinColourProperty::kText);
        look.line = skin.getColour(SkinObjectType::kGroupLabel, SkinColourProperty::kLine);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    SectionHeaderLook sectionHeaderLookFromSkin(const ISkin& skin)
    {
        SectionHeaderLook look;
        look.text = skin.getColour(SkinObjectType::kSectionHeader, SkinColourProperty::kText);
        look.lineBlue = skin.getColour(SkinObjectType::kSectionHeader, SkinColourProperty::kLineBlue);
        look.lineOrange = skin.getColour(SkinObjectType::kSectionHeader, SkinColourProperty::kLineOrange);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kSectionHeader);
        return look;
    }

    ModuleHeaderLook moduleHeaderLookFromSkin(const ISkin& skin)
    {
        ModuleHeaderLook look;
        look.text = skin.getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kText);
        look.textFocus = skin.getColour(SkinObjectType::kNumberBox,
                                        SkinColourProperty::kText,
                                        SkinColourState::kFocus,
                                        ColourSet::k4);
        look.lineBlue = skin.getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kLineBlue);
        look.lineOrange = skin.getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kLineOrange);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kModuleHeader);
        return look;
    }

    ModulationBusHeaderLook modulationBusHeaderLookFromSkin(const ISkin& skin)
    {
        ModulationBusHeaderLook look;
        look.text = skin.getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kText);
        look.lineBlue = skin.getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kLineBlue);
        look.lineOrange = skin.getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kLineOrange);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kModuleHeader);
        return look;
    }
}
