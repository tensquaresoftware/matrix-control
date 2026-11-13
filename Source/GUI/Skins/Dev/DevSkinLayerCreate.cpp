#include "GUI/Skins/Dev/DevSkinLayer.h"

namespace TSS::DevSkin
{
    juce::Result Layer::createDesignerSkinWithUnboundMappings(
        const juce::String& sourceId,
        const juce::String& newName,
        const juce::String& mergeCustomPastillesFromId)
    {
        const auto created = duplicateSkin(sourceId, newName, false);
        if (created.failed())
            return created;

        if (mergeCustomPastillesFromId.isEmpty()
            || mergeCustomPastillesFromId == activeSkinId_)
            return juce::Result::ok();

        const auto* donor = findSkin(mergeCustomPastillesFromId);
        auto* dest = findSkinMutable(activeSkinId_);
        if (donor == nullptr || dest == nullptr)
            return juce::Result::ok();

        appendCustomPastillesFrom(*dest, *donor);
        bump();
        return juce::Result::ok();
    }

    void Layer::appendCustomPastillesFrom(NamedSkin& dest, const NamedSkin& source) const
    {
        for (const auto& pastille : source.palette)
        {
            if (isProductPastilleName(pastille.name))
                continue;

            bool exists = false;
            for (const auto& existing : dest.palette)
            {
                if (existing.name.equalsIgnoreCase(pastille.name))
                {
                    exists = true;
                    break;
                }
            }
            if (! exists)
                dest.palette.push_back(pastille);
        }
    }

    juce::String Layer::resolveCompiledAncestorId(const NamedSkin& skin) const
    {
        static constexpr std::pair<SkinKind, const char*> kKindToId[] = {
            { SkinKind::kCompiledBlack, kCompiledBlackId },
            { SkinKind::kCompiledBlackChroma, kCompiledBlackChromaId },
            { SkinKind::kCompiledBlackMinimal, kCompiledBlackMinimalId },
            { SkinKind::kCompiledCream, kCompiledCreamId },
            { SkinKind::kCompiledCreamMinimal, kCompiledCreamMinimalId },
            { SkinKind::kCompiledCreamChroma, kCompiledCreamChromaId },
        };

        const auto idForKind = [](SkinKind kind) -> juce::String
        {
            for (const auto& entry : kKindToId)
                if (entry.first == kind)
                    return entry.second;
            return {};
        };
        const auto isCompiledProductId = [](const juce::String& id) noexcept
        {
            for (const auto& entry : kKindToId)
                if (id == entry.second)
                    return true;
            return false;
        };

        if (auto id = idForKind(skin.kind); id.isNotEmpty())
            return id;

        juce::String cursor = skin.derivedFrom;
        for (int guard = 0; guard < 64; ++guard)
        {
            if (isCompiledProductId(cursor))
                return cursor;
            const auto* parent = findSkin(cursor);
            if (parent == nullptr)
                break;
            if (auto id = idForKind(parent->kind); id.isNotEmpty())
                return id;
            cursor = parent->derivedFrom;
        }
        return kCompiledCreamId;
    }
}
