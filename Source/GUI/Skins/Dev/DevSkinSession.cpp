#include "GUI/Skins/Dev/DevSkinSession.h"

#include "Shared/ProjectPaths.h"

namespace TSS::DevSkin
{
    namespace
    {
        juce::String readExistingDirectory(const juce::DynamicObject& root)
        {
            const auto path = root.getProperty("lastSkinJsonDirectory").toString();
            if (path.isNotEmpty() && ! juce::File(path).isDirectory())
                return {};
            return path;
        }
    }

    void clearProductSelectionAtLaunch(DesignerSessionState& state)
    {
        state.productSkinId = {};
    }

    juce::String exportSessionToJson(const DesignerSessionState& state)
    {
        auto* root = new juce::DynamicObject();
        root->setProperty("document", kSessionDocumentId);
        root->setProperty("lastSkinJsonDirectory", state.lastSkinJsonDirectory);
        root->setProperty("jsonSyncMode", jsonSyncModeToString(state.jsonSyncMode));
        return juce::JSON::toString(juce::var(root), true);
    }

    JsonIoResult importSessionFromJson(DesignerSessionState& state, const juce::String& jsonText)
    {
        const auto parsed = juce::JSON::parse(jsonText);
        auto* root = parsed.getDynamicObject();
        if (root == nullptr)
            return JsonIoResult::failure("JSON root must be an object");

        if (root->getProperty("document").toString() != kSessionDocumentId)
            return JsonIoResult::failure("Not a Skin Designer session document");

        // Everything except preferences stays at its blank-launch default.
        state = {};
        state.lastSkinJsonDirectory = readExistingDirectory(*root);
        state.jsonSyncMode = jsonSyncModeFromString(root->getProperty("jsonSyncMode").toString());
        return JsonIoResult::success();
    }

    juce::File skinDesignerSessionFile()
    {
        return ProjectPaths::getApplicationDataDirectory().getChildFile("skin-designer-session.json");
    }

    bool saveSessionFile(const DesignerSessionState& state, juce::String& error)
    {
        const auto file = skinDesignerSessionFile();
        const auto parent = file.getParentDirectory();
        if (! parent.isDirectory() && ! parent.createDirectory())
        {
            error = "Could not create session directory";
            return false;
        }
        if (! file.replaceWithText(exportSessionToJson(state)))
        {
            error = "Could not write session file";
            return false;
        }
        return true;
    }

    JsonIoResult loadSessionFile(DesignerSessionState& state)
    {
        const auto file = skinDesignerSessionFile();
        if (! file.existsAsFile())
            return JsonIoResult::failure("No session file");

        return importSessionFromJson(state, file.loadFileAsString());
    }
}
