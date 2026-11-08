#pragma once

#include <array>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    /** Generic UI kind addressed by the skin colour map (no patch or master object types). */
    enum class SkinObjectType
    {
        kWindowSurface,
        kHeaderPanel,
        kBodyPanel,
        kFooterPanel,
        kChromeText,
        kFooterMessage,
        kSectionHeader,
        kModuleHeader,
        kGroupLabel,
        kLabel,
        kHorizontalSeparator,
        kButton,
        kToggle,
        kSlider,
        kProgressBar,
        kComboBox,
        kPopupMenu,
        kNumberBox,
        kEnvelopeDisplay,
        kTrackGeneratorDisplay,
        kPatchNameDisplay,
        kDialog,
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

    /** MCS objects (several colour sets, chosen by zone). Every other object is UCS (k1 only). */
    inline constexpr std::array<SkinObjectType, 5> kMultiSetSkinObjects {
        SkinObjectType::kSlider,
        SkinObjectType::kComboBox,
        SkinObjectType::kPopupMenu,
        SkinObjectType::kLed,
        SkinObjectType::kPeakIndicator
    };

    constexpr bool isMenuObject(SkinObjectType object) noexcept
    {
        return object == SkinObjectType::kComboBox || object == SkinObjectType::kPopupMenu;
    }

    constexpr bool isLevelObject(SkinObjectType object) noexcept
    {
        return object == SkinObjectType::kLed || object == SkinObjectType::kPeakIndicator;
    }

    /** Slider 4, ComboBox / PopupMenu 3, Led / PeakIndicator 2; every UCS object returns 1. */
    constexpr size_t colourSetCount(SkinObjectType object) noexcept
    {
        if (object == SkinObjectType::kSlider)
            return 4;
        if (isMenuObject(object))
            return 3;
        return isLevelObject(object) ? 2 : 1;
    }

    constexpr bool supportsColourSet(SkinObjectType object, ColourSet colourSet) noexcept
    {
        return static_cast<size_t>(colourSet) < colourSetCount(object);
    }

    constexpr bool supportsMultipleColourSets(SkinObjectType object) noexcept
    {
        return colourSetCount(object) > 1;
    }

    constexpr bool multiSetListMatchesColourSetCounts() noexcept
    {
        for (const auto object : kMultiSetSkinObjects)
            if (colourSetCount(object) <= 1)
                return false;
        return true;
    }

    static_assert(multiSetListMatchesColourSetCounts(),
                  "Every kMultiSetSkinObjects entry must expose more than one colour set");
    static_assert(colourSetCount(SkinObjectType::kButton) == 1, "UCS objects expose exactly k1");

    /** Product zones that pick an MCS colour set. UCS objects ignore zones. */
    enum class SkinZone
    {
        kPatch,
        kMaster,
        kHeader,
        kModales
    };

    static_assert(static_cast<int>(SkinZone::kPatch) == static_cast<int>(ColourSet::k1)
                      && static_cast<int>(SkinZone::kMaster) == static_cast<int>(ColourSet::k2)
                      && static_cast<int>(SkinZone::kHeader) == static_cast<int>(ColourSet::k3)
                      && static_cast<int>(SkinZone::kModales) == static_cast<int>(ColourSet::k4),
                  "SkinZone order must match Slider colour sets (zoneColourSet casts zone to ColourSet)");

    /** Hard exception: Computer Patches combo/popup and Header logo menu borrow the Modales mono set. */
    inline constexpr SkinZone kMonoMenuExceptionZone = SkinZone::kModales;

    /**
     * Frozen zone -> colour set matrix. Header has no ComboBox / PopupMenu and only Header /
     * Modales carry Led / PeakIndicator; unsupported pairs and UCS objects resolve to k1.
     */
    constexpr ColourSet zoneColourSet(SkinObjectType object, SkinZone zone) noexcept
    {
        if (object == SkinObjectType::kSlider)
            return static_cast<ColourSet>(zone);
        if (isMenuObject(object) && zone == SkinZone::kMaster)
            return ColourSet::k2;
        if (isMenuObject(object) && zone == SkinZone::kModales)
            return ColourSet::k3;
        if (isLevelObject(object) && zone == SkinZone::kModales)
            return ColourSet::k2;
        return ColourSet::k1;
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
