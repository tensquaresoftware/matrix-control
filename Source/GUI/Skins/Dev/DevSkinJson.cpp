#include "GUI/Skins/Dev/DevSkinJson.h"

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "GUI/Skins/Dev/DevSkinSsotSeed.h"

namespace TSS::DevSkin
{
    namespace
    {
        juce::String kindToString(SkinKind kind)
        {
            switch (kind)
            {
                case SkinKind::kCompiledBlack: return "compiledBlack";
                case SkinKind::kCompiledCream: return "compiledCream";
                case SkinKind::kDev:           return "dev";
            }
            return "dev";
        }

        bool kindFromString(const juce::String& text, SkinKind& out)
        {
            if (text == "compiledBlack") { out = SkinKind::kCompiledBlack; return true; }
            if (text == "compiledCream") { out = SkinKind::kCompiledCream; return true; }
            if (text == "dev")           { out = SkinKind::kDev; return true; }
            return false;
        }

        juce::var pastilleToVar(const Pastille& pastille)
        {
            auto* obj = new juce::DynamicObject();
            obj->setProperty("name", pastille.name);
            obj->setProperty("argb", juce::String::formatted("0x%08X", static_cast<unsigned>(pastille.argb)));
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
                out.name = obj->getProperty("name").toString();
                const auto argbText = obj->getProperty("argb").toString().trim();
                if (out.name.isEmpty() || argbText.isEmpty())
                {
                    error = "Pastille requires name and argb";
                    return false;
                }
                return parseArgbHex(argbText, out.argb, error);
            }
            error = "Pastille entry must be an object";
            return false;
        }

        juce::var mappingToVar(const SkinColourKey& key, const juce::String& paletteName)
        {
            auto* obj = new juce::DynamicObject();
            obj->setProperty("object", toString(key.object));
            obj->setProperty("property", toString(key.property));
            obj->setProperty("state", toString(key.state));
            obj->setProperty("paletteName", paletteName);
            return juce::var(obj);
        }

        bool mappingFromVar(const juce::var& v, SkinColourKey& key, juce::String& paletteName, juce::String& error)
        {
            auto* obj = v.getDynamicObject();
            if (obj == nullptr)
            {
                error = "Mapping entry must be an object";
                return false;
            }

            SkinObjectType object {};
            SkinColourProperty property {};
            SkinColourState state {};
            if (! parseObjectType(obj->getProperty("object").toString(), object)
                || ! parseProperty(obj->getProperty("property").toString(), property)
                || ! parseState(obj->getProperty("state").toString(), state))
            {
                error = "Mapping has unknown object/property/state";
                return false;
            }

            paletteName = obj->getProperty("paletteName").toString();
            if (paletteName.isEmpty())
            {
                error = "Mapping requires paletteName";
                return false;
            }

            key = skinColourKey(object, property, state);
            return true;
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

            if (auto* mapArr = obj->getProperty("mappings").getArray())
            {
                for (const auto& entry : *mapArr)
                {
                    SkinColourKey key;
                    juce::String paletteName;
                    if (! mappingFromVar(entry, key, paletteName, error))
                        return false;
                    out.mappings[key] = paletteName;
                }
            }
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

        return juce::JSON::toString(juce::var(root), true);
    }

    juce::String exportEditorSkinToJson(const NamedSkin& skin)
    {
        auto* root = new juce::DynamicObject();
        root->setProperty("schemaVersion", kSchemaVersion);
        root->setProperty("activeSkinId", skin.id);
        juce::Array<juce::var> skins;
        skins.add(skinToVar(skin));
        root->setProperty("skins", skins);
        return juce::JSON::toString(juce::var(root), true);
    }

    juce::String exportEditorSkinToJson(const Layer& layer, const juce::String& skinId)
    {
        const auto* skin = layer.findSkin(skinId);
        if (skin == nullptr || skin->kind != SkinKind::kDev)
            return {};
        return exportEditorSkinToJson(*skin);
    }

    namespace
    {
        JsonIoResult parseDevSkinsFromRoot(juce::DynamicObject& root, std::vector<NamedSkin>& outSkins)
        {
            auto* arr = root.getProperty("skins").getArray();
            if (arr == nullptr)
                return JsonIoResult::success();

            for (const auto& entry : *arr)
            {
                NamedSkin skin;
                juce::String error;
                if (! skinFromVar(entry, skin, error))
                    return JsonIoResult::failure(error);
                if (skin.kind != SkinKind::kDev)
                    return JsonIoResult::failure("Imported skins must be kind=dev");
                if (skin.id == kCompiledBlackId || skin.id == kCompiledCreamId)
                    return JsonIoResult::failure("Imported Editor skin cannot reuse compiled ids");
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

            if (static_cast<int>(root->getProperty("schemaVersion")) != kSchemaVersion)
                return JsonIoResult::failure("Unsupported schemaVersion");

            return JsonIoResult::success();
        }
    }

    JsonIoResult importFromJson(Layer& layer, const juce::String& jsonText)
    {
        juce::var parsed;
        juce::DynamicObject* root = nullptr;
        const auto gate = parseRootObject(jsonText, parsed, root);
        if (! gate.ok)
            return gate;

        std::vector<NamedSkin> skins;
        skins.push_back(makeCompiledSkin(SkinKind::kCompiledBlack));
        skins.push_back(makeCompiledSkin(SkinKind::kCompiledCream));

        const auto imported = appendImportedDevSkins(*root, skins);
        if (! imported.ok)
            return imported;

        const auto activeId = resolveActiveSkinId(*root, skins);
        layer.replaceAll(std::move(skins), activeId);
        return JsonIoResult::success();
    }

    JsonIoResult appendEditorSkinsFromJson(Layer& layer, const juce::String& jsonText)
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
            return JsonIoResult::failure("No Editor skins found in JSON");

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

            if (auto* mapArr = obj.getProperty("mappings").getArray())
            {
                for (const auto& entry : *mapArr)
                {
                    SkinColourKey key;
                    juce::String paletteName;
                    if (! mappingFromVar(entry, key, paletteName, error))
                        return false;
                    out.mappings[key] = paletteName;
                }
            }
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
