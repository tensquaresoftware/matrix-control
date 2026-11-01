#pragma once

#include <array>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    /** Generic UI kind addressed by the skin colour map (no patch or master object types). */
    enum class SkinObjectType
    {
        kHeaderPanel,
        kBodyPanel,
        kFooterPanel,
        kDarkPanel,
        kFooterMessage,
        kSectionHeader,
        kModuleHeader,
        kGroupLabel,
        kLabel,
        kHorizontalSeparator,
        kButton,
        kToggle,
        kSlider,
        kSliderButtonLike,
        kComboBox,
        kComboBoxButtonLike,
        kPopupMenu,
        kPopupMenuButtonLike,
        kNumberBox,
        kEnvelopeDisplay,
        kTrackGeneratorDisplay,
        kPatchNameDisplay,
        kDialog,
        kWindowClose,
        kReadOnlyValueField,
        kLed,
        kPeakIndicator
    };

    enum class ColourSet
    {
        k1,
        k2,
        k3,
        k4
    };

    inline constexpr std::array<ColourSet, 4> kAllColourSets {
        ColourSet::k1,
        ColourSet::k2,
        ColourSet::k3,
        ColourSet::k4
    };

    inline constexpr std::array<SkinObjectType, 12> kMultiSetSkinObjects {
        SkinObjectType::kSlider,
        SkinObjectType::kSliderButtonLike,
        SkinObjectType::kComboBox,
        SkinObjectType::kComboBoxButtonLike,
        SkinObjectType::kPopupMenu,
        SkinObjectType::kPopupMenuButtonLike,
        SkinObjectType::kNumberBox,
        SkinObjectType::kButton,
        SkinObjectType::kToggle,
        SkinObjectType::kLed,
        SkinObjectType::kPeakIndicator,
        SkinObjectType::kReadOnlyValueField
    };

    constexpr bool supportsMultipleColourSets(SkinObjectType object) noexcept
    {
        for (const auto multiSetObject : kMultiSetSkinObjects)
            if (multiSetObject == object)
                return true;
        return false;
    }

    /** What is painted for an object (intent WHAT, not HOW). */
    enum class SkinColourProperty
    {
        kBackground,
        kBorder,
        kText,
        kSecondaryText,
        kLine,
        kLineBlue,
        kLineOrange,
        kTrack,
        kValueBar,
        kFocusBorder,
        kTriangle,
        kSeparator,
        kScrollbar,
        kEnvelope,
        kShaper,
        kEditorBackground,
        kEditorSelectionBackground,
        kEditorText,
        kTitleBand,
        kGlyph,
        kCaret
    };

    /** Named visual variant for a (object, property) pair. */
    enum class SkinColourState
    {
        kDefault,
        kOff,
        kOn,
        kHover,
        kClicked,
        kDisabled,
        kFocus,
        kInfo,
        kWarning,
        kError
    };

    /** SSOT mapping address: (ObjectType, Property, State, ColourSet) -> palette entry. */
    struct SkinColourKey
    {
        SkinObjectType object = SkinObjectType::kButton;
        SkinColourProperty property = SkinColourProperty::kBackground;
        SkinColourState state = SkinColourState::kDefault;
        ColourSet colourSet = ColourSet::k1;

        constexpr bool operator==(const SkinColourKey& other) const noexcept
        {
            return object == other.object
                && property == other.property
                && state == other.state
                && colourSet == other.colourSet;
        }

        constexpr bool operator<(const SkinColourKey& other) const noexcept
        {
            if (object != other.object)
                return object < other.object;
            if (property != other.property)
                return property < other.property;
            if (state != other.state)
                return state < other.state;
            return colourSet < other.colourSet;
        }
    };

    constexpr SkinColourKey skinColourKey(SkinObjectType object,
                                          SkinColourProperty property,
                                          SkinColourState state = SkinColourState::kDefault,
                                          ColourSet colourSet = ColourSet::k1) noexcept
    {
        return SkinColourKey { object, property, state, colourSet };
    }
}
