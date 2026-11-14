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
     * Unused cells are not assignable (Zones panel em-dash).
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
}
