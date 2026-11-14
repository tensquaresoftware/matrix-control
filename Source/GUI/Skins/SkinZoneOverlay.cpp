#include "GUI/Skins/SkinZoneOverlay.h"

namespace TSS
{
    namespace
    {
        struct ZoneOverlayState
        {
            bool active = false;
            SkinZoneMap map;
        };

        ZoneOverlayState& state() noexcept
        {
            static ZoneOverlayState instance;
            return instance;
        }
    }

    void setSkinZoneOverlay(SkinZoneMap map)
    {
        auto& s = state();
        s.map = std::move(map);
        s.active = true;
    }

    void clearSkinZoneOverlay() noexcept
    {
        auto& s = state();
        s.map.clear();
        s.active = false;
    }

    bool hasSkinZoneOverlay() noexcept
    {
        return state().active;
    }

    const SkinZoneMap& skinZoneOverlayMap() noexcept
    {
        return state().map;
    }

    ColourSet resolveZoneColourSet(SkinObjectType object, SkinZone zone) noexcept
    {
        const auto& s = state();
        if (s.active && isZoneUsed(object, zone))
        {
            const auto it = s.map.find(SkinZoneMapKey { object, zone });
            if (it != s.map.end())
                return it->second;
        }
        return zoneColourSet(object, zone);
    }
}
