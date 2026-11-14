#pragma once

#include "GUI/Skins/SkinZoneRegistry.h"

namespace TSS
{
    /**
     * Dev-session zone overlay consulted by Look-for-zone helpers.
     * Empty / cleared => registry defaults. Message-thread / UI-thread only.
     */
    void setSkinZoneOverlay(SkinZoneMap map);
    void clearSkinZoneOverlay() noexcept;
    bool hasSkinZoneOverlay() noexcept;
    const SkinZoneMap& skinZoneOverlayMap() noexcept;

    /**
     * Overlay wins for used MCS cells present in the map; otherwise registry defaults.
     * Unused / UCS pairs always use zoneColourSet (product rules unchanged).
     */
    ColourSet resolveZoneColourSet(SkinObjectType object, SkinZone zone) noexcept;
}
