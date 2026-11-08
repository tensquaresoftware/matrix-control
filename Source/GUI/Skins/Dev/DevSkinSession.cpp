#include "GUI/Skins/Dev/DevSkinSession.h"

#include "Shared/ProjectPaths.h"

namespace TSS::DevSkin
{
    namespace
    {
        juce::String readExistingDirectoryProperty(const juce::DynamicObject& root,
                                                   const juce::Identifier& primary,
                                                   const juce::Identifier& legacy)
        {
            auto path = root.getProperty(primary).toString();
            if (path.isEmpty())
                path = root.getProperty(legacy).toString();
            if (path.isNotEmpty() && ! juce::File(path).isDirectory())
                return {};
            return path;
        }

        juce::String resolveEditorSkinIdFromSession(const Layer& layer, juce::String editorSkinId)
        {
            if (const auto* skin = layer.findSkin(editorSkinId);
                skin != nullptr && skin->kind == SkinKind::kDev)
            {
                return editorSkinId;
            }

            for (const auto& skin : layer.skins())
                if (skin.kind == SkinKind::kDev)
                    return skin.id;
            return {};
        }

        void readLastImportedJsonWatch(const juce::DynamicObject& root, EditorSessionState& state)
        {
            const auto importedPath = root.getProperty("lastImportedJsonPath").toString();
            state.lastImportedJsonPath =
                (importedPath.isNotEmpty() && juce::File(importedPath).existsAsFile())
                    ? importedPath
                    : juce::String {};
            state.lastImportedJsonFingerprint =
                state.lastImportedJsonPath.isNotEmpty()
                    ? root.getProperty("lastImportedJsonFingerprint").toString()
                    : juce::String {};
        }
    }

    void forceProductToFirstAvailable(EditorSessionState& state)
    {
        state.productSkinId = kCompiledBlackId;
    }

    juce::String exportSessionToJson(const Layer& layer, const EditorSessionState& state)
    {
        const auto parsed = juce::JSON::parse(exportToJson(layer));
        auto* root = parsed.getDynamicObject();
        if (root == nullptr)
            return {};

        root->setProperty("document", "skinEditorSession");
        root->setProperty("productSkinId", state.productSkinId);
        root->setProperty("editorSkinId", state.editorSkinId);
        root->setProperty("lastSkinJsonDirectory", state.lastSkinJsonDirectory);
        root->setProperty("lastImportedJsonPath", state.lastImportedJsonPath);
        root->setProperty("lastImportedJsonFingerprint", state.lastImportedJsonFingerprint);
        return juce::JSON::toString(parsed, true);
    }

    JsonIoResult importSessionFromJson(Layer& layer,
                                       EditorSessionState& state,
                                       const juce::String& jsonText)
    {
        const auto parsed = juce::JSON::parse(jsonText);
        auto* root = parsed.getDynamicObject();
        if (root == nullptr)
            return JsonIoResult::failure("JSON root must be an object");

        if (root->getProperty("document").toString() != "skinEditorSession")
            return JsonIoResult::failure("Not a Skin Editor session document");

        const auto imported = importFromJson(layer, jsonText);
        if (! imported.ok)
            return imported;

        state.productSkinId = root->getProperty("productSkinId").toString();
        if (state.productSkinId != kCompiledBlackId
            && state.productSkinId != kCompiledBlackDuoId
            && state.productSkinId != kCompiledCreamId)
        {
            state.productSkinId = kCompiledBlackId;
        }

        state.editorSkinId =
            resolveEditorSkinIdFromSession(layer, root->getProperty("editorSkinId").toString());

        // Ignore legacy "compareOn" if present — Compare always starts Off.
        state.compareOn = false;
        state.lastSkinJsonDirectory = readExistingDirectoryProperty(*root,
                                                                    "lastSkinJsonDirectory",
                                                                    "lastExportDirectory");
        readLastImportedJsonWatch(*root, state);
        return JsonIoResult::success();
    }

    JsonIoResult importSessionFromJsonOrReset(Layer& layer,
                                              EditorSessionState& state,
                                              const juce::String& jsonText)
    {
        const auto result = importSessionFromJson(layer, state, jsonText);
        if (result.ok)
            return result;

        layer.resetToCompiledBaseline();
        state = {};
        return JsonIoResult::failure("Corrupt session - restored baseline. " + result.message);
    }

    juce::File skinEditorSessionFile()
    {
        return ProjectPaths::getApplicationDataDirectory().getChildFile("skin-editor-session.json");
    }

    bool saveSessionFile(const Layer& layer, const EditorSessionState& state, juce::String& error)
    {
        const auto file = skinEditorSessionFile();
        const auto parent = file.getParentDirectory();
        if (! parent.isDirectory() && ! parent.createDirectory())
        {
            error = "Could not create session directory";
            return false;
        }
        const auto json = exportSessionToJson(layer, state);
        if (json.isEmpty())
        {
            error = "Session export produced empty content";
            return false;
        }
        if (! file.replaceWithText(json))
        {
            error = "Could not write session file";
            return false;
        }
        return true;
    }

    JsonIoResult loadSessionFile(Layer& layer, EditorSessionState& state)
    {
        const auto file = skinEditorSessionFile();
        if (! file.existsAsFile())
            return JsonIoResult::failure("No session file");

        return importSessionFromJsonOrReset(layer, state, file.loadFileAsString());
    }
}
