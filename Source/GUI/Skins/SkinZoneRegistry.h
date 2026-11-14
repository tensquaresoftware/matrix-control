#pragma once

#include <array>
#include <map>

#include "GUI/Skins/SkinValues.h"

namespace TSS
{
    inline constexpr std::array<SkinZone, 4> kAllSkinZones {
        SkinZone::kPatch,
        SkinZone::kMaster,
        SkinZone::kHeader,
        SkinZone::kModales
    };

    /**
     * MCS occupancy: true only when the product places that object in that geography.
     * Unused cells are not assignable (Zones panel shows ASCII `-`).
     */
    constexpr bool isZoneUsed(SkinObjectType object, SkinZone zone) noexcept
    {
        if (object == SkinObjectType::kSlider)
            return true;
        if (isMenuObject(object))
            return zone != SkinZone::kHeader;
        if (isLevelObject(object))
            return zone == SkinZone::kHeader || zone == SkinZone::kModales;
        return false;
    }

    /** Product default drawer for a cell — alias of zoneColourSet (used cells + UCS fallback). */
    constexpr ColourSet defaultColourSet(SkinObjectType object, SkinZone zone) noexcept
    {
        return zoneColourSet(object, zone);
    }

    enum class ZoneExceptionUsesKind
    {
        kColourSet,
        kUcs
    };

    /** Read-only V1 exception row for Designer Exceptions block / tests. */
    struct ZoneExceptionRow
    {
        const char* where = nullptr;
        SkinObjectType object = SkinObjectType::kButton;
        ZoneExceptionUsesKind usesKind = ZoneExceptionUsesKind::kUcs;
        ColourSet colourSet = ColourSet::k1;
    };

    inline constexpr std::array<ZoneExceptionRow, 4> kZoneExceptionRows {
        ZoneExceptionRow {
            "Patch Manager - Computer Patches file browser",
            SkinObjectType::kComboBox,
            ZoneExceptionUsesKind::kColourSet,
            ColourSet::k3
        },
        ZoneExceptionRow {
            "Patch Manager - Computer Patches file browser",
            SkinObjectType::kPopupMenu,
            ZoneExceptionUsesKind::kColourSet,
            ColourSet::k3
        },
        ZoneExceptionRow {
            "Header - logo menu",
            SkinObjectType::kPopupMenu,
            ZoneExceptionUsesKind::kColourSet,
            ColourSet::k3
        },
        ZoneExceptionRow {
            "Header - Undo / Redo / Panic",
            SkinObjectType::kButton,
            ZoneExceptionUsesKind::kUcs,
            ColourSet::k1
        }
    };

    /** Per-skin / push map key: used MCS (object, zone) cells only. */
    struct SkinZoneMapKey
    {
        SkinObjectType object = SkinObjectType::kSlider;
        SkinZone zone = SkinZone::kPatch;

        constexpr bool operator==(const SkinZoneMapKey& other) const noexcept
        {
            return object == other.object && zone == other.zone;
        }

        constexpr bool operator<(const SkinZoneMapKey& other) const noexcept
        {
            if (object != other.object)
                return object < other.object;
            return zone < other.zone;
        }
    };

    using SkinZoneMap = std::map<SkinZoneMapKey, ColourSet>;

    /**
     * UCS inventory (colourSetCount == 1), product object order.
     * Zones Shared block reads this — do not duplicate the list in Designer.
     */
    inline constexpr std::array<SkinObjectType, 20> kSingleSetSkinObjects {
        SkinObjectType::kWindowSurface,
        SkinObjectType::kHeaderPanel,
        SkinObjectType::kBodyPanel,
        SkinObjectType::kFooterPanel,
        SkinObjectType::kChromeText,
        SkinObjectType::kFooterMessage,
        SkinObjectType::kSectionHeader,
        SkinObjectType::kModuleHeader,
        SkinObjectType::kGroupLabel,
        SkinObjectType::kLabel,
        SkinObjectType::kHorizontalSeparator,
        SkinObjectType::kButton,
        SkinObjectType::kToggle,
        SkinObjectType::kProgressBar,
        SkinObjectType::kNumberBox,
        SkinObjectType::kEnvelopeDisplay,
        SkinObjectType::kTrackGeneratorDisplay,
        SkinObjectType::kPatchNameDisplay,
        SkinObjectType::kDialog,
        SkinObjectType::kReadOnlyValueField
    };

    constexpr bool singleSetListExcludesMultiSet() noexcept
    {
        for (const auto single : kSingleSetSkinObjects)
            for (const auto multi : kMultiSetSkinObjects)
                if (single == multi)
                    return false;
        return true;
    }

    constexpr bool singleSetListHasUniqueEntries() noexcept
    {
        for (size_t i = 0; i < kSingleSetSkinObjects.size(); ++i)
            for (size_t j = i + 1; j < kSingleSetSkinObjects.size(); ++j)
                if (kSingleSetSkinObjects[i] == kSingleSetSkinObjects[j])
                    return false;
        return true;
    }

    static_assert(singleSetListExcludesMultiSet(),
                  "kSingleSetSkinObjects must not include MCS objects");
    static_assert(singleSetListHasUniqueEntries(),
                  "kSingleSetSkinObjects must not contain duplicates");
    static_assert(kSingleSetSkinObjects.size() + kMultiSetSkinObjects.size() == 25,
                  "UCS + MCS must cover the SkinObjectType inventory");
}
