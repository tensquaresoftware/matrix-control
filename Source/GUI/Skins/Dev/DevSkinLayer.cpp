#include "GUI/Skins/Dev/DevSkinLayer.h"

#include <algorithm>

#include "GUI/Skins/Dev/DevSkinInventory.h"
#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"
#include "GUI/Skins/Dev/DevSkinSsotSeed.h"

namespace TSS::DevSkin
{
    Layer::Layer()
    {
        resetToCompiledBaseline();
    }

    void Layer::resetToCompiledBaseline()
    {
        skins_.clear();
        skins_.push_back(makeCompiledSkin(SkinKind::kCompiledBlack));
        skins_.push_back(makeCompiledSkin(SkinKind::kCompiledCream));
        activeSkinId_ = kCompiledCreamId;
        bump();
    }

    const NamedSkin* Layer::findSkin(const juce::String& id) const
    {
        for (const auto& skin : skins_)
            if (skin.id == id)
                return &skin;
        return nullptr;
    }

    NamedSkin* Layer::findSkinMutable(const juce::String& id)
    {
        for (auto& skin : skins_)
            if (skin.id == id)
                return &skin;
        return nullptr;
    }

    bool Layer::selectSkin(const juce::String& id)
    {
        if (findSkin(id) == nullptr)
            return false;
        if (activeSkinId_ == id)
            return true;
        activeSkinId_ = id;
        bump();
        return true;
    }

    bool Layer::isActiveReadOnly() const
    {
        return isCompiledId(activeSkinId_);
    }

    juce::Result Layer::duplicateSkin(const juce::String& sourceId, const juce::String& newName)
    {
        const auto trimmed = newName.trim();
        if (trimmed.isEmpty())
            return juce::Result::fail("Dev skin name cannot be empty");
        if (nameTaken(trimmed))
            return juce::Result::fail("A skin with that name already exists");

        const auto* source = findSkin(sourceId);
        if (source == nullptr)
            return juce::Result::fail("Source skin not found");

        NamedSkin copy = *source;
        copy.kind = SkinKind::kDev;
        copy.id = makeUniqueDevId(trimmed);
        copy.name = trimmed;
        copy.derivedFrom = source->id;
        if (source->kind == SkinKind::kDev && source->palette.empty())
            seedFromCompiled(copy, SkinKind::kCompiledCream);

        skins_.push_back(std::move(copy));
        activeSkinId_ = skins_.back().id;
        bump();
        return juce::Result::ok();
    }

    juce::Result Layer::createEmptyDevSkin(const juce::String& newName)
    {
        return duplicateSkin(kCompiledCreamId, newName);
    }

    juce::Result Layer::deleteDevSkin(const juce::String& id)
    {
        if (isCompiledId(id))
            return juce::Result::fail("Compiled Black/Cream cannot be deleted");

        const auto it = std::find_if(skins_.begin(), skins_.end(),
                                     [&](const NamedSkin& skin) { return skin.id == id; });
        if (it == skins_.end())
            return juce::Result::fail("Dev skin not found");

        const bool wasActive = activeSkinId_ == id;
        skins_.erase(it);
        if (wasActive)
            activeSkinId_ = kCompiledCreamId;
        bump();
        return juce::Result::ok();
    }

    juce::Result Layer::renameDevSkin(const juce::String& id, const juce::String& newName)
    {
        if (isCompiledId(id))
            return juce::Result::fail("Compiled Black/Cream cannot be renamed");

        const auto trimmed = newName.trim();
        if (trimmed.isEmpty())
            return juce::Result::fail("Dev skin name cannot be empty");
        if (nameTaken(trimmed, id))
            return juce::Result::fail("A skin with that name already exists");

        auto* skin = findSkinMutable(id);
        if (skin == nullptr)
            return juce::Result::fail("Dev skin not found");

        skin->name = trimmed;
        bump();
        return juce::Result::ok();
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

        for (auto& pastille : skin->palette)
        {
            if (pastille.name == pastilleName)
            {
                pastille.argb = argb;
                bump();
                return juce::Result::ok();
            }
        }
        return juce::Result::fail("Pastille not found");
    }

    juce::Result Layer::addPastille(const juce::String& skinId,
                                    const juce::String& pastilleName,
                                    juce::uint32 argb)
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
            if (pastille.name == trimmed)
                return juce::Result::fail("Pastille already exists");

        skin->palette.push_back({ trimmed, argb });
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

    juce::Result Layer::setMapping(const juce::String& skinId,
                                   SkinColourKey key,
                                   const juce::String& pastilleName)
    {
        if (isCompiledId(skinId))
            return juce::Result::fail("Compiled Black/Cream are read-only; Duplicate first");

        auto* skin = findSkinMutable(skinId);
        if (skin == nullptr)
            return juce::Result::fail("Dev skin not found");

        if (pastilleName.isNotEmpty()
            && ! argbForPastilleName(skin->palette, pastilleName).has_value())
        {
            return juce::Result::fail("Unknown pastille for this skin");
        }

        if (pastilleName.isEmpty())
            skin->mappings.erase(key);
        else
            skin->mappings[key] = pastilleName;

        bump();
        return juce::Result::ok();
    }

    juce::Result Layer::bindUnbound(const juce::String& skinId,
                                    SkinColourKey key,
                                    const juce::String& pastilleName)
    {
        if (pastilleName.trim().isEmpty())
            return juce::Result::fail("Pastille name required to bind Unbound key");

        auto* skin = findSkinMutable(skinId);
        if (skin == nullptr)
            return juce::Result::fail("Dev skin not found");

        bool pastilleOk = false;
        for (const auto& pastille : skin->palette)
        {
            if (pastille.name == pastilleName)
            {
                pastilleOk = true;
                break;
            }
        }
        if (! pastilleOk)
            return juce::Result::fail("Invalid pastille: not in this skin's palette");

        return setMapping(skinId, key, pastilleName);
    }

    std::vector<SkinColourKey> Layer::unboundKeys(const juce::String& skinId) const
    {
        std::vector<SkinColourKey> unbound;
        const auto* skin = findSkin(skinId);
        if (skin == nullptr)
            return unbound;

        for (const auto& key : allStructuredKeys())
        {
            if (skin->mappings.find(key) == skin->mappings.end())
                unbound.push_back(key);
        }
        return unbound;
    }

    int Layer::unboundCount(const juce::String& skinId) const
    {
        return static_cast<int>(unboundKeys(skinId).size());
    }

    bool Layer::isEditorSkinExportAllowed(const juce::String& skinId) const
    {
        const auto* skin = findSkin(skinId);
        if (skin == nullptr || skin->kind != SkinKind::kDev)
            return false;
        return unboundCount(skinId) == 0;
    }

    bool Layer::isProductPastilleName(const juce::String& pastilleName) const
    {
        for (const auto& pastille : colourChartPastilles())
            if (pastille.name == pastilleName)
                return true;
        return false;
    }

    int Layer::countMappingsReferencing(const juce::String& skinId,
                                        const juce::String& pastilleName) const
    {
        const auto* skin = findSkin(skinId);
        if (skin == nullptr)
            return 0;

        int count = 0;
        for (const auto& [key, name] : skin->mappings)
        {
            juce::ignoreUnused(key);
            if (name == pastilleName)
                ++count;
        }
        return count;
    }

    juce::Result Layer::appendDevSkin(NamedSkin skin, juce::String* outId)
    {
        if (skin.kind != SkinKind::kDev)
            return juce::Result::fail("Only Editor skins can be appended");

        auto trimmedName = skin.name.trim();
        if (trimmedName.isEmpty())
            return juce::Result::fail("Editor skin name cannot be empty");

        if (nameTaken(trimmedName) || isCompiledId(skin.id) || findSkin(skin.id) != nullptr)
        {
            if (nameTaken(trimmedName))
            {
                int suffix = 2;
                auto candidate = trimmedName + " " + juce::String(suffix);
                while (nameTaken(candidate))
                {
                    ++suffix;
                    candidate = trimmedName + " " + juce::String(suffix);
                }
                trimmedName = candidate;
            }
            skin.id = makeUniqueDevId(trimmedName);
            skin.name = trimmedName;
        }
        else if (skin.id.isEmpty())
        {
            skin.id = makeUniqueDevId(trimmedName);
            skin.name = trimmedName;
        }
        else
        {
            skin.name = trimmedName;
        }

        skin.kind = SkinKind::kDev;
        skins_.push_back(std::move(skin));
        activeSkinId_ = skins_.back().id;
        if (outId != nullptr)
            *outId = activeSkinId_;
        bump();
        return juce::Result::ok();
    }

    juce::String Layer::resolveCompiledAncestorId(const NamedSkin& skin) const
    {
        if (skin.kind == SkinKind::kCompiledBlack)
            return kCompiledBlackId;
        if (skin.kind == SkinKind::kCompiledCream)
            return kCompiledCreamId;

        juce::String cursor = skin.derivedFrom;
        for (int guard = 0; guard < 64; ++guard)
        {
            if (cursor == kCompiledBlackId)
                return kCompiledBlackId;
            if (cursor == kCompiledCreamId)
                return kCompiledCreamId;

            const auto* parent = findSkin(cursor);
            if (parent == nullptr)
                break;
            if (parent->kind == SkinKind::kCompiledBlack)
                return kCompiledBlackId;
            if (parent->kind == SkinKind::kCompiledCream)
                return kCompiledCreamId;
            cursor = parent->derivedFrom;
        }
        return kCompiledCreamId;
    }

    PushPayload Layer::makePushPayload() const
    {
        return makePushPayloadForSkin(activeSkinId_);
    }

    PushPayload Layer::makePushPayloadForSkin(const juce::String& skinId) const
    {
        PushPayload payload;
        payload.revision = revision_;
        payload.activeSkinId = skinId;

        const auto* skin = findSkin(skinId);
        if (skin == nullptr)
        {
            payload.kind = SkinKind::kCompiledCream;
            payload.compiledAncestorId = kCompiledCreamId;
            return payload;
        }

        payload.kind = skin->kind;
        payload.derivedFrom = skin->derivedFrom;
        payload.compiledAncestorId = resolveCompiledAncestorId(*skin);
        payload.palette = skin->palette;
        payload.mappings = skin->mappings;
        return payload;
    }

    void Layer::replaceAll(std::vector<NamedSkin> skins, juce::String activeId)
    {
        skins_ = std::move(skins);
        if (findSkin(activeId) != nullptr)
            activeSkinId_ = std::move(activeId);
        else if (! skins_.empty())
            activeSkinId_ = skins_.front().id;
        else
            activeSkinId_ = kCompiledCreamId;
        bump();
    }

    void Layer::bump()
    {
        ++revision_;
        if (onChanged)
            onChanged();
    }

    juce::String Layer::makeUniqueDevId(const juce::String& name) const
    {
        auto base = "dev:" + name.toLowerCase().retainCharacters(
            "abcdefghijklmnopqrstuvwxyz0123456789-_");
        if (base == "dev:")
            base = "dev:skin";

        auto candidate = base;
        int suffix = 2;
        while (findSkin(candidate) != nullptr)
        {
            candidate = base + "-" + juce::String(suffix);
            ++suffix;
        }
        return candidate;
    }

    bool Layer::isCompiledId(const juce::String& id) const
    {
        return id == kCompiledBlackId || id == kCompiledCreamId;
    }

    bool Layer::nameTaken(const juce::String& name, const juce::String& exceptId) const
    {
        for (const auto& skin : skins_)
            if (skin.name.equalsIgnoreCase(name) && skin.id != exceptId)
                return true;
        return false;
    }
}
