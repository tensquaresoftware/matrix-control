#pragma once

#include "ColourChartTones.h"
#include "Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace TSS::SkinCatalog
{
    inline constexpr int kBaseSkinCount = 6;
    inline constexpr int kTonesSkinCount = ColourChartTones::kToneCount;
    inline constexpr int kGlobalSkinCount = kBaseSkinCount + kTonesSkinCount;

    namespace Ids = PluginIDs::Settings::SkinVariants;
    namespace Names = PluginDisplayNames::ChoiceLists::SkinVariants;

    static_assert(Ids::kTonesFuchsia - Ids::kTonesCherry + 1 == ColourChartTones::kToneCount);
    static_assert(kTonesSkinCount == ColourChartTones::kToneCount);
    static_assert(static_cast<int>(Skin::SkinComboBoxItemId::kTonesFuchsia)
                      - static_cast<int>(Skin::SkinComboBoxItemId::kTonesCherry) + 1
                  == ColourChartTones::kToneCount);

    /** Digit / global cycle order: bases 1-6 then TONES CHERRY…FUCHSIA. */
    inline constexpr int kGlobalCycleOrder[kGlobalSkinCount] = {
        Ids::kBlack,
        Ids::kBlackChroma,
        Ids::kBlackMinimal,
        Ids::kCream,
        Ids::kCreamChroma,
        Ids::kCreamMinimal,
        Ids::kTonesCherry,
        Ids::kTonesCoral,
        Ids::kTonesSaffron,
        Ids::kTonesOchre,
        Ids::kTonesGold,
        Ids::kTonesOlive,
        Ids::kTonesLime,
        Ids::kTonesEmerald,
        Ids::kTonesKigee,
        Ids::kTonesPetrol,
        Ids::kTonesAzur,
        Ids::kTonesMarine,
        Ids::kTonesPurple,
        Ids::kTonesIndigo,
        Ids::kTonesFuchsia
    };

    inline constexpr int kBlackFamilyIds[] = {
        Ids::kBlack, Ids::kBlackChroma, Ids::kBlackMinimal
    };

    inline constexpr int kCreamFamilyIds[] = {
        Ids::kCream, Ids::kCreamChroma, Ids::kCreamMinimal
    };

    inline constexpr int kTonesFamilyIds[] = {
        Ids::kTonesCherry, Ids::kTonesCoral, Ids::kTonesSaffron, Ids::kTonesOchre,
        Ids::kTonesGold, Ids::kTonesOlive, Ids::kTonesLime, Ids::kTonesEmerald,
        Ids::kTonesKigee, Ids::kTonesPetrol, Ids::kTonesAzur, Ids::kTonesMarine,
        Ids::kTonesPurple, Ids::kTonesIndigo, Ids::kTonesFuchsia
    };

    static_assert(sizeof(kTonesFamilyIds) / sizeof(kTonesFamilyIds[0]) == kTonesSkinCount);

    inline Skin::SkinFamily familyForItemId(int skinItemId) noexcept
    {
        if (skinItemId == Ids::kCream || skinItemId == Ids::kCreamChroma
            || skinItemId == Ids::kCreamMinimal)
            return Skin::SkinFamily::Cream;
        if (skinItemId >= Ids::kTonesCherry && skinItemId <= Ids::kTonesFuchsia)
            return Skin::SkinFamily::Tones;
        return Skin::SkinFamily::Black;
    }

    /** Parallel to kGlobalCycleOrder. */
    inline constexpr const char* kGlobalDisplayNames[kGlobalSkinCount] = {
        Names::kBlack, Names::kBlackChroma, Names::kBlackMinimal,
        Names::kCream, Names::kCreamChroma, Names::kCreamMinimal,
        Names::kTonesCherry, Names::kTonesCoral, Names::kTonesSaffron, Names::kTonesOchre,
        Names::kTonesGold, Names::kTonesOlive, Names::kTonesLime, Names::kTonesEmerald,
        Names::kTonesKigee, Names::kTonesPetrol, Names::kTonesAzur, Names::kTonesMarine,
        Names::kTonesPurple, Names::kTonesIndigo, Names::kTonesFuchsia
    };

    inline int indexInOrder(const int* ids, int count, int skinItemId) noexcept
    {
        for (int i = 0; i < count; ++i)
            if (ids[i] == skinItemId)
                return i;
        return -1;
    }

    inline const char* displayNameForItemId(int skinItemId) noexcept
    {
        const int index = indexInOrder(kGlobalCycleOrder, kGlobalSkinCount, skinItemId);
        if (index < 0)
            return Names::kBlack;
        return kGlobalDisplayNames[index];
    }

    inline int cycleInOrder(const int* ids, int count, int skinItemId, int direction) noexcept
    {
        if (count <= 0)
            return skinItemId;

        int index = indexInOrder(ids, count, skinItemId);
        if (index < 0)
            index = 0;

        index = (index + direction) % count;
        if (index < 0)
            index += count;
        return ids[index];
    }

    inline int cycleFamily(int skinItemId, int direction) noexcept
    {
        switch (familyForItemId(skinItemId))
        {
            case Skin::SkinFamily::Cream:
                return cycleInOrder(kCreamFamilyIds, 3, skinItemId, direction);
            case Skin::SkinFamily::Tones:
                return cycleInOrder(kTonesFamilyIds, kTonesSkinCount, skinItemId, direction);
            case Skin::SkinFamily::Black:
            default:
                return cycleInOrder(kBlackFamilyIds, 3, skinItemId, direction);
        }
    }

    inline int cycleGlobal(int skinItemId, int direction) noexcept
    {
        return cycleInOrder(kGlobalCycleOrder, kGlobalSkinCount, skinItemId, direction);
    }

    template <typename Combo>
    void populateFlatSkinCombo(Combo& combo)
    {
        for (const int id : kGlobalCycleOrder)
            combo.addItem(displayNameForItemId(id), id);
    }
}
