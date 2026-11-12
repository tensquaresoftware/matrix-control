#include "GUI/Skins/Dev/DevSkinJson.h"

#include <map>
#include <set>

#include "GUI/Skins/Dev/DevSkinInventory.h"
#include "GUI/Skins/Dev/DevSkinJsonMapping.h"
#include "GUI/Skins/Dev/DevSkinKindCodec.h"
#include "GUI/Skins/Dev/DevSkinSsotSeed.h"

namespace TSS::DevSkin
{
    namespace
    {
        // Colour sets beyond the object's inventory (older trial files) are dropped, not rejected.
        bool mappingsFromVar(const juce::var& mappingsVar,
                             std::map<SkinColourKey, juce::String>& out,
                             juce::String& error)
        {
            auto* mapArr = mappingsVar.getArray();
            if (mapArr == nullptr)
                return true;

            for (const auto& entry : *mapArr)
            {
                SkinColourKey key;
                juce::String paletteName;
                if (! mappingFromVar(entry, key, paletteName, error))
                    return false;
                if (supportsColourSet(key.object, key.colourSet))
                    out[key] = paletteName;
            }
            return true;
        }

        juce::var pastilleToVar(const Pastille& pastille)
        {
            auto* obj = new juce::DynamicObject();
            obj->setProperty("name", pastille.name);
            obj->setProperty("argb", juce::String::formatted("0x%08X", static_cast<unsigned>(pastille.argb)));
            obj->setProperty("pending", pastille.pending);
            return juce::var(obj);
        }

        bool parseArgbHex(const juce::String& text, juce::uint32& outArgb, juce::String& error)
        {
            auto cleaned = text.trim().trimCharactersAtStart("#");
            if (cleaned.startsWithIgnoreCase("0x"))
                cleaned = cleaned.substring(2);
            if (cleaned.length() == 6)
                cleaned = "FF" + cleaned;
            if (cleaned.length() != 8)
            {
                error = "ARGB hex must be 6 or 8 hex digits";
                return false;
            }
            for (int i = 0; i < cleaned.length(); ++i)
            {
                if (juce::CharacterFunctions::getHexDigitValue(cleaned[i]) < 0)
                {
                    error = "ARGB hex contains non-hex digits";
                    return false;
                }
            }
            outArgb = static_cast<juce::uint32>(cleaned.getHexValue64());
            return true;
        }

        bool pastilleFromVar(const juce::var& v, Pastille& out, juce::String& error)
        {
            if (auto* obj = v.getDynamicObject())
            {
                out = {};
                out.name = obj->getProperty("name").toString();
                const auto argbText = obj->getProperty("argb").toString().trim();
                if (out.name.isEmpty() || argbText.isEmpty())
                {
                    error = "Pastille requires name and argb";
                    return false;
                }
                // Missing pending → false (legacy JSON = calibrated filled swatch).
                out.pending = static_cast<bool>(obj->getProperty("pending"));
                return parseArgbHex(argbText, out.argb, error);
            }
            error = "Pastille entry must be an object";
            return false;
        }

        juce::var skinToVar(const NamedSkin& skin)
        {
            auto* obj = new juce::DynamicObject();
            obj->setProperty("id", skin.id);
            obj->setProperty("name", skin.name);
            obj->setProperty("kind", kindToString(skin.kind));
            if (skin.derivedFrom.isNotEmpty())
                obj->setProperty("derivedFrom", skin.derivedFrom);

            juce::Array<juce::var> palette;
            for (const auto& pastille : skin.palette)
                palette.add(pastilleToVar(pastille));
            obj->setProperty("palette", palette);

            juce::Array<juce::var> mappings;
            for (const auto& [key, paletteName] : skin.mappings)
                mappings.add(mappingToVar(key, paletteName));
            obj->setProperty("mappings", mappings);

            // Designer-only metadata: omit when empty; never written into PushPayload.
            const auto labels = colourSetLabelsToVar(skin);
            if (! labels.isVoid())
                obj->setProperty("colourSetLabels", labels);
            const auto linkGroups = mappingLinkGroupsToVar(skin);
            if (! linkGroups.isVoid())
                obj->setProperty("mappingLinkGroups", linkGroups);
            return juce::var(obj);
        }

        bool skinFromVar(const juce::var& v, NamedSkin& out, juce::String& error)
        {
            auto* obj = v.getDynamicObject();
            if (obj == nullptr)
            {
                error = "Skin entry must be an object";
                return false;
            }

            out = {};
            out.id = obj->getProperty("id").toString();
            out.name = obj->getProperty("name").toString();
            out.derivedFrom = obj->getProperty("derivedFrom").toString();
            if (out.id.isEmpty() || out.name.isEmpty())
            {
                error = "Skin requires id and name";
                return false;
            }
            if (! kindFromString(obj->getProperty("kind").toString(), out.kind))
            {
                error = "Skin has unknown kind";
                return false;
            }

            if (auto* paletteArr = obj->getProperty("palette").getArray())
            {
                for (const auto& entry : *paletteArr)
                {
                    Pastille pastille;
                    if (! pastilleFromVar(entry, pastille, error))
                        return false;
                    out.palette.push_back(pastille);
                }
            }

            if (! mappingsFromVar(obj->getProperty("mappings"), out.mappings, error))
                return false;

            colourSetLabelsFromVar(obj->getProperty("colourSetLabels"), out);

            const auto inventory = allStructuredKeys();
            const std::set<SkinColourKey> validKeys(inventory.begin(), inventory.end());
            mappingLinkGroupsFromVar(obj->getProperty("mappingLinkGroups"), out, validKeys);
            return true;
        }
    }

    juce::String exportToJson(const Layer& layer)
    {
        auto* root = new juce::DynamicObject();
        root->setProperty("schemaVersion", kSchemaVersion);
        root->setProperty("activeSkinId", layer.activeSkinId());

        juce::Array<juce::var> skins;
        for (const auto& skin : layer.skins())
        {
            if (skin.kind == SkinKind::kDev)
                skins.add(skinToVar(skin));
        }
        root->setProperty("skins", skins);

        // Human-readable indent — Import parses either form.
        return juce::JSON::toString(juce::var(root), false);
    }

    juce::String exportDesignerSkinToJson(const NamedSkin& skin)
    {
        auto* root = new juce::DynamicObject();
        root->setProperty("schemaVersion", kSchemaVersion);
        root->setProperty("activeSkinId", skin.id);
        juce::Array<juce::var> skins;
        skins.add(skinToVar(skin));
        root->setProperty("skins", skins);
        // Human-readable indent — Import parses either form.
        return juce::JSON::toString(juce::var(root), false);
    }

    juce::String exportDesignerSkinToJson(const Layer& layer, const juce::String& skinId)
    {
        const auto* skin = layer.findSkin(skinId);
        if (skin == nullptr || skin->kind != SkinKind::kDev)
            return {};
        return exportDesignerSkinToJson(*skin);
    }

    namespace
    {
        JsonIoResult parseDevSkinsFromRoot(juce::DynamicObject& root, std::vector<NamedSkin>& outSkins)
        {
            const int schemaVersion = static_cast<int>(root.getProperty("schemaVersion"));
            auto* arr = root.getProperty("skins").getArray();
            if (arr == nullptr)
                return JsonIoResult::success();

            for (const auto& entry : *arr)
            {
                NamedSkin skin;
                juce::String error;
                if (! skinFromVar(entry, skin, error))
                    return JsonIoResult::failure(error);
                if (schemaVersion == kLegacySchemaVersion)
                    expandLegacyColourSets(skin);
                if (skin.kind != SkinKind::kDev)
                    return JsonIoResult::failure("Imported skins must be kind=dev");
                if (isReservedCompiledSkinId(skin.id))
                    return JsonIoResult::failure("Imported Designer skin cannot reuse compiled ids");
                outSkins.push_back(std::move(skin));
            }
            return JsonIoResult::success();
        }

        JsonIoResult appendImportedDevSkins(juce::DynamicObject& root, std::vector<NamedSkin>& skins)
        {
            std::vector<NamedSkin> imported;
            const auto parsed = parseDevSkinsFromRoot(root, imported);
            if (! parsed.ok)
                return parsed;

            for (auto& skin : imported)
            {
                for (const auto& existing : skins)
                {
                    if (existing.id == skin.id)
                        return JsonIoResult::failure("Duplicate skin id: " + skin.id);
                }
                skins.push_back(std::move(skin));
            }
            return JsonIoResult::success();
        }

        juce::String resolveActiveSkinId(juce::DynamicObject& root, const std::vector<NamedSkin>& skins)
        {
            auto activeId = root.getProperty("activeSkinId").toString();
            if (activeId.isEmpty())
                activeId = kCompiledCreamId;

            for (const auto& skin : skins)
                if (skin.id == activeId)
                    return activeId;
            return kCompiledCreamId;
        }

        JsonIoResult parseRootObject(const juce::String& jsonText, juce::var& parsed, juce::DynamicObject*& root)
        {
            parsed = juce::JSON::parse(jsonText);
            if (parsed.isVoid())
                return JsonIoResult::failure("Malformed JSON");

            root = parsed.getDynamicObject();
            if (root == nullptr)
                return JsonIoResult::failure("JSON root must be an object");

            const int schemaVersion = static_cast<int>(root->getProperty("schemaVersion"));
            if (schemaVersion != kLegacySchemaVersion && schemaVersion != kSchemaVersion)
                return JsonIoResult::failure("Unsupported schemaVersion");

            return JsonIoResult::success();
        }

        JsonIoResult importReplacingAll(Layer& layer, const juce::String& jsonText, bool requireDesignerSkins)
        {
            juce::var parsed;
            juce::DynamicObject* root = nullptr;
            const auto gate = parseRootObject(jsonText, parsed, root);
            if (! gate.ok)
                return gate;

            std::vector<NamedSkin> skins;
            skins.push_back(makeCompiledSkin(SkinKind::kCompiledBlack));
            skins.push_back(makeCompiledSkin(SkinKind::kCompiledBlackChroma));
            skins.push_back(makeCompiledSkin(SkinKind::kCompiledBlackMinimal));
            skins.push_back(makeCompiledSkin(SkinKind::kCompiledCream));
            skins.push_back(makeCompiledSkin(SkinKind::kCompiledCreamMinimal));

            const auto compiledCount = skins.size();
            const auto imported = appendImportedDevSkins(*root, skins);
            if (! imported.ok)
                return imported;
            if (requireDesignerSkins && skins.size() == compiledCount)
                return JsonIoResult::failure("No Designer skins found in JSON");

            auto result = JsonIoResult::success();
            for (size_t i = compiledCount; i < skins.size(); ++i)
                result.importedSkinIds.add(skins[i].id);

            const auto activeId = resolveActiveSkinId(*root, skins);
            layer.replaceAll(std::move(skins), activeId);
            return result;
        }
    }

    JsonIoResult importFromJson(Layer& layer, const juce::String& jsonText)
    {
        return importReplacingAll(layer, jsonText, false);
    }

    JsonIoResult replaceDesignerSkinsFromJson(Layer& layer, const juce::String& jsonText)
    {
        return importReplacingAll(layer, jsonText, true);
    }

    JsonIoResult appendDesignerSkinsFromJson(Layer& layer, const juce::String& jsonText)
    {
        juce::var parsed;
        juce::DynamicObject* root = nullptr;
        const auto gate = parseRootObject(jsonText, parsed, root);
        if (! gate.ok)
            return gate;

        std::vector<NamedSkin> imported;
        const auto parsedSkins = parseDevSkinsFromRoot(*root, imported);
        if (! parsedSkins.ok)
            return parsedSkins;
        if (imported.empty())
            return JsonIoResult::failure("No Designer skins found in JSON");

        JsonIoResult result = JsonIoResult::success();
        for (auto& skin : imported)
        {
            juce::String newId;
            const auto appended = layer.appendDevSkin(std::move(skin), &newId);
            if (appended.failed())
            {
                for (const auto& id : result.importedSkinIds)
                    layer.deleteDevSkin(id);
                return JsonIoResult::failure(appended.getErrorMessage());
            }
            result.importedSkinIds.add(newId);
        }
        return result;
    }

    juce::var pushPayloadToVar(const PushPayload& payload)
    {
        auto* obj = new juce::DynamicObject();
        obj->setProperty("type", "push");
        obj->setProperty("revision", static_cast<int>(payload.revision));
        obj->setProperty("activeSkinId", payload.activeSkinId);
        obj->setProperty("kind", kindToString(payload.kind));
        if (payload.derivedFrom.isNotEmpty())
            obj->setProperty("derivedFrom", payload.derivedFrom);
        if (payload.compiledAncestorId.isNotEmpty())
            obj->setProperty("compiledAncestorId", payload.compiledAncestorId);

        juce::Array<juce::var> palette;
        for (const auto& pastille : payload.palette)
            palette.add(pastilleToVar(pastille));
        obj->setProperty("palette", palette);

        juce::Array<juce::var> mappings;
        for (const auto& [key, paletteName] : payload.mappings)
            mappings.add(mappingToVar(key, paletteName));
        obj->setProperty("mappings", mappings);
        return juce::var(obj);
    }

    namespace
    {
        void fillDefaultCompiledAncestor(PushPayload& out)
        {
            if (out.compiledAncestorId.isNotEmpty())
                return;
            if (out.kind == SkinKind::kCompiledBlack || out.derivedFrom == kCompiledBlackId)
                out.compiledAncestorId = kCompiledBlackId;
            else if (out.kind == SkinKind::kCompiledBlackChroma || out.derivedFrom == kCompiledBlackChromaId)
                out.compiledAncestorId = kCompiledBlackChromaId;
            else if (out.kind == SkinKind::kCompiledBlackMinimal || out.derivedFrom == kCompiledBlackMinimalId)
                out.compiledAncestorId = kCompiledBlackMinimalId;
            else if (out.kind == SkinKind::kCompiledCreamMinimal || out.derivedFrom == kCompiledCreamMinimalId)
                out.compiledAncestorId = kCompiledCreamMinimalId;
            else
                out.compiledAncestorId = kCompiledCreamId;
        }

        bool readPushPaletteAndMappings(juce::DynamicObject& obj, PushPayload& out, juce::String& error)
        {
            if (auto* paletteArr = obj.getProperty("palette").getArray())
            {
                for (const auto& entry : *paletteArr)
                {
                    Pastille pastille;
                    if (! pastilleFromVar(entry, pastille, error))
                        return false;
                    out.palette.push_back(pastille);
                }
            }

            if (! mappingsFromVar(obj.getProperty("mappings"), out.mappings, error))
                return false;
            return true;
        }
    }

    bool pushPayloadFromVar(const juce::var& root, PushPayload& out, juce::String& error)
    {
        auto* obj = root.getDynamicObject();
        if (obj == nullptr)
        {
            error = "Push payload must be an object";
            return false;
        }
        if (obj->getProperty("type").toString() != "push")
        {
            error = "Expected type=push";
            return false;
        }

        out = {};
        out.revision = static_cast<juce::int64>(static_cast<int>(obj->getProperty("revision")));
        out.activeSkinId = obj->getProperty("activeSkinId").toString();
        out.derivedFrom = obj->getProperty("derivedFrom").toString();
        out.compiledAncestorId = obj->getProperty("compiledAncestorId").toString();
        if (! kindFromString(obj->getProperty("kind").toString(), out.kind))
        {
            error = "Unknown skin kind";
            return false;
        }
        fillDefaultCompiledAncestor(out);
        return readPushPaletteAndMappings(*obj, out, error);
    }
}
