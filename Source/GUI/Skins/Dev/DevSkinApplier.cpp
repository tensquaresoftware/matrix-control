#include "GUI/Skins/Dev/DevSkinApplier.h"

#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"

namespace TSS::DevSkin
{
    namespace
    {
        bool isCompiledKind(SkinKind kind) noexcept
        {
            return kind == SkinKind::kCompiledBlack
                || kind == SkinKind::kCompiledBlackChroma
                || kind == SkinKind::kCompiledCream;
        }
    }

    Skin::ColourVariant compiledBaseVariant(const PushPayload& payload)
    {
        if (payload.kind == SkinKind::kCompiledBlack
            || payload.compiledAncestorId == kCompiledBlackId
            || payload.derivedFrom == kCompiledBlackId)
        {
            return Skin::ColourVariant::Black;
        }

        if (payload.kind == SkinKind::kCompiledBlackChroma
            || payload.compiledAncestorId == kCompiledBlackChromaId
            || payload.derivedFrom == kCompiledBlackChromaId)
        {
            return Skin::ColourVariant::BlackChroma;
        }

        if (payload.kind == SkinKind::kCompiledCream
            || payload.compiledAncestorId == kCompiledCreamId
            || payload.derivedFrom == kCompiledCreamId)
        {
            return Skin::ColourVariant::Cream;
        }

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

        targetSkin.clearHigherColourSets();
        for (const auto& [key, paletteName] : payload.mappings)
        {
            const auto argb = argbForPastilleName(payload.palette, paletteName);
            jassert(argb.has_value());
            targetSkin.setColour(key, juce::Colour(*argb));
        }

        result.ok = true;
        result.message = "Applied";
        return result;
    }
}
