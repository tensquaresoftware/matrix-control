#include "GUI/Skins/Dev/DevSkinApplier.h"

#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"
#include "GUI/Skins/SkinZoneOverlay.h"
#include "GUI/Skins/SkinZoneRegistry.h"

namespace TSS::DevSkin
{
    namespace
    {
        bool isCompiledKind(SkinKind kind) noexcept
        {
            return kind == SkinKind::kCompiledBlack
                || kind == SkinKind::kCompiledBlackChroma
                || kind == SkinKind::kCompiledBlackMinimal
                || kind == SkinKind::kCompiledCream
                || kind == SkinKind::kCompiledCreamMinimal
                || kind == SkinKind::kCompiledCreamChroma;
        }

        struct CompiledVariantRule
        {
            SkinKind kind;
            const char* productId;
            Skin::ColourVariant variant;
        };

        constexpr CompiledVariantRule kCompiledVariantRules[] = {
            { SkinKind::kCompiledBlack, kCompiledBlackId, Skin::ColourVariant::Black },
            { SkinKind::kCompiledBlackChroma, kCompiledBlackChromaId, Skin::ColourVariant::BlackChroma },
            { SkinKind::kCompiledBlackMinimal, kCompiledBlackMinimalId, Skin::ColourVariant::BlackMinimal },
            { SkinKind::kCompiledCream, kCompiledCreamId, Skin::ColourVariant::Cream },
            { SkinKind::kCompiledCreamMinimal, kCompiledCreamMinimalId, Skin::ColourVariant::CreamMinimal },
            { SkinKind::kCompiledCreamChroma, kCompiledCreamChromaId, Skin::ColourVariant::CreamChroma },
        };

        bool payloadMatchesRule(const PushPayload& payload, const CompiledVariantRule& rule) noexcept
        {
            return payload.kind == rule.kind
                || payload.compiledAncestorId == rule.productId
                || payload.derivedFrom == rule.productId;
        }

        ApplyResult validateZoneMap(const SkinZoneMap& zoneMap, juce::int64 revision, const juce::String& skinId)
        {
            ApplyResult result;
            result.revision = revision;
            result.appliedSkinId = skinId;
            for (const auto& [zoneKey, colourSet] : zoneMap)
            {
                if (! isZoneUsed(zoneKey.object, zoneKey.zone)
                    || ! supportsColourSet(zoneKey.object, colourSet))
                {
                    result.ok = false;
                    result.code = "invalid_zone_map";
                    result.message = isZoneUsed(zoneKey.object, zoneKey.zone)
                                         ? "Zone map colour set out of range for object"
                                         : "Zone map assigns an unused MCS cell";
                    return result;
                }
            }
            result.ok = true;
            return result;
        }
    }

    Skin::ColourVariant compiledBaseVariant(const PushPayload& payload)
    {
        for (const auto& rule : kCompiledVariantRules)
            if (payloadMatchesRule(payload, rule))
                return rule.variant;

        // Unknown Dev lineage: prefer Cream only when ancestor was not resolved.
        return Skin::ColourVariant::Cream;
    }

    ApplyResult applyPushToSkin(const PushPayload& payload, Skin& targetSkin)
    {
        ApplyResult result;
        result.revision = payload.revision;
        result.appliedSkinId = payload.activeSkinId;

        if (isCompiledKind(payload.kind))
        {
            clearSkinZoneOverlay();
            result.ok = true;
            result.message = "Compiled skin selection";
            return result;
        }

        // Validate every pastille before mutating the live skin (atomic apply).
        for (const auto& entry : payload.mappings)
        {
            if (! argbForPastilleName(payload.palette, entry.second).has_value())
            {
                result.ok = false;
                result.code = "unknown_pastille";
                result.message = "Mapping references unknown pastille: " + entry.second;
                return result;
            }
        }

        const auto zoneCheck = validateZoneMap(payload.zoneMap, payload.revision, payload.activeSkinId);
        if (! zoneCheck.ok)
            return zoneCheck;

        targetSkin.clearHigherColourSets();
        for (const auto& [key, paletteName] : payload.mappings)
        {
            const auto argb = argbForPastilleName(payload.palette, paletteName);
            jassert(argb.has_value());
            targetSkin.setColour(key, juce::Colour(*argb));
        }

        setSkinZoneOverlay(payload.zoneMap);

        result.ok = true;
        result.message = "Applied";
        return result;
    }
}
