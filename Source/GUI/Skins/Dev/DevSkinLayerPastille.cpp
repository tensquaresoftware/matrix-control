#include "GUI/Skins/Dev/DevSkinLayer.h"

#include <algorithm>

namespace TSS::DevSkin
{
    namespace
    {
        juce::Result renamePastilleNameGuards(const Layer& layer,
                                              const juce::String& pastilleName,
                                              const juce::String& trimmedNewName)
        {
            if (layer.isProductPastilleName(pastilleName))
                return juce::Result::fail("Product colours cannot be renamed");
            if (trimmedNewName.isEmpty())
                return juce::Result::fail("Pastille name cannot be empty");
            if (layer.isProductPastilleName(trimmedNewName))
                return juce::Result::fail("Product colour names are reserved");
            return juce::Result::ok();
        }

        void rewriteMappingPastilleName(NamedSkin& skin,
                                        const juce::String& oldName,
                                        const juce::String& newName)
        {
            for (auto& [key, mapped] : skin.mappings)
            {
                juce::ignoreUnused(key);
                if (mapped == oldName)
                    mapped = newName;
            }
        }
    }

    juce::Result Layer::setPastilleArgb(const juce::String& skinId,
                                        const juce::String& pastilleName,
                                        juce::uint32 argb)
    {
        if (isCompiledId(skinId))
            return juce::Result::fail("Compiled Black/Cream are read-only; Duplicate first");

        auto* skin = findSkinMutable(skinId);
        if (skin == nullptr)
            return juce::Result::fail("Dev skin not found");

        Pastille* target = nullptr;
        for (auto& pastille : skin->palette)
        {
            if (pastille.name == pastilleName)
            {
                target = &pastille;
                break;
            }
        }
        if (target == nullptr)
            return juce::Result::fail("Pastille not found");

        if (target->argb == argb)
            return juce::Result::ok();

        for (const auto& pastille : skin->palette)
        {
            if (pastille.name != pastilleName && pastille.argb == argb)
                return juce::Result::fail("A pastille with that colour already exists");
        }

        target->argb = argb;
        target->pending = false;
        bump();
        return juce::Result::ok();
    }

    juce::Result Layer::addPastille(const juce::String& skinId,
                                    const juce::String& pastilleName,
                                    juce::uint32 argb,
                                    bool pending)
    {
        if (isCompiledId(skinId))
            return juce::Result::fail("Compiled Black/Cream are read-only; Duplicate first");

        const auto trimmed = pastilleName.trim();
        if (trimmed.isEmpty())
            return juce::Result::fail("Pastille name cannot be empty");

        auto* skin = findSkinMutable(skinId);
        if (skin == nullptr)
            return juce::Result::fail("Dev skin not found");

        for (const auto& pastille : skin->palette)
        {
            if (pastille.name.equalsIgnoreCase(trimmed))
                return juce::Result::fail("Pastille already exists");
            if (pastille.argb == argb)
                return juce::Result::fail("A pastille with that colour already exists");
        }

        skin->palette.push_back({ trimmed, argb, pending });
        bump();
        return juce::Result::ok();
    }

    juce::Result Layer::renamePastille(const juce::String& skinId,
                                       const juce::String& pastilleName,
                                       const juce::String& newName)
    {
        if (isCompiledId(skinId))
            return juce::Result::fail("Compiled Black/Cream are read-only; Duplicate first");

        const auto trimmed = newName.trim();
        if (const auto guard = renamePastilleNameGuards(*this, pastilleName, trimmed);
            guard.failed())
            return guard;

        auto* skin = findSkinMutable(skinId);
        if (skin == nullptr)
            return juce::Result::fail("Dev skin not found");

        Pastille* target = nullptr;
        for (auto& pastille : skin->palette)
        {
            if (pastille.name == pastilleName)
                target = &pastille;
            else if (pastille.name.equalsIgnoreCase(trimmed))
                return juce::Result::fail("Pastille already exists");
        }
        if (target == nullptr)
            return juce::Result::fail("Pastille not found");
        if (target->name == trimmed)
            return juce::Result::ok();

        const auto oldName = target->name;
        target->name = trimmed;
        rewriteMappingPastilleName(*skin, oldName, trimmed);
        bump();
        return juce::Result::ok();
    }

    juce::Result Layer::deletePastille(const juce::String& skinId,
                                       const juce::String& pastilleName,
                                       bool removeReferencingMappings)
    {
        if (isCompiledId(skinId))
            return juce::Result::fail("Compiled Black/Cream are read-only; Duplicate first");

        if (isProductPastilleName(pastilleName))
            return juce::Result::fail("Product colours cannot be deleted");

        auto* skin = findSkinMutable(skinId);
        if (skin == nullptr)
            return juce::Result::fail("Dev skin not found");

        const auto pastilleIt = std::find_if(skin->palette.begin(), skin->palette.end(),
                                             [&](const Pastille& p) { return p.name == pastilleName; });
        if (pastilleIt == skin->palette.end())
            return juce::Result::fail("Pastille not found");

        const auto refs = countMappingsReferencing(skinId, pastilleName);
        if (refs > 0 && ! removeReferencingMappings)
            return juce::Result::fail("Pastille is referenced by mappings");

        if (removeReferencingMappings)
        {
            for (auto it = skin->mappings.begin(); it != skin->mappings.end();)
            {
                if (it->second == pastilleName)
                    it = skin->mappings.erase(it);
                else
                    ++it;
            }
        }

        skin->palette.erase(pastilleIt);
        bump();
        return juce::Result::ok();
    }
}
