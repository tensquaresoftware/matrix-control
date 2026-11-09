#pragma once

#include "GUI/Skins/Dev/DevSkinJson.h"

namespace TSS::DevSkin
{
    /** How Skin Editor reacts when the last Import JSON changes on disk. */
    enum class JsonSyncMode
    {
        kOff,  // ignore external edits
        kAsk,  // prompt Reload / Discard (default)
        kAuto  // reload without prompt when editor is clean
    };

    inline constexpr const char* jsonSyncModeToString(JsonSyncMode mode) noexcept
    {
        switch (mode)
        {
            case JsonSyncMode::kOff:  return "off";
            case JsonSyncMode::kAuto: return "auto";
            case JsonSyncMode::kAsk:
            default:                  return "ask";
        }
    }

    inline JsonSyncMode jsonSyncModeFromString(const juce::String& text) noexcept
    {
        if (text.equalsIgnoreCase("off"))
            return JsonSyncMode::kOff;
        if (text.equalsIgnoreCase("auto"))
            return JsonSyncMode::kAuto;
        return JsonSyncMode::kAsk;
    }

    struct EditorSessionState
    {
        juce::String productSkinId = kCompiledBlackId;
        juce::String editorSkinId;
        /** Runtime only — never persisted; always Off after session load. */
        bool compareOn = false;
        /** Absolute path of last Export/Import parent folder (empty = Documents). */
        juce::String lastSkinJsonDirectory;
        /** Absolute path of last successful Import JSON (empty = no external-change watch). */
        juce::String lastImportedJsonPath;
        /** Last accepted disk fingerprint for lastImportedJsonPath (empty = legacy / unset). */
        juce::String lastImportedJsonFingerprint;
        /** Persisted JSON Sync combo; missing/legacy session values → Ask. */
        JsonSyncMode jsonSyncMode = JsonSyncMode::kAsk;
    };

    /** Sets Product to first combo item (BLACK today). Call after session load to ignore restored Product. */
    void forceProductToFirstAvailable(EditorSessionState& state);

    juce::String exportSessionToJson(const Layer& layer, const EditorSessionState& state);
    JsonIoResult importSessionFromJson(Layer& layer,
                                       EditorSessionState& state,
                                       const juce::String& jsonText);

    /** Import session JSON; on failure reset layer to compiled Black/Cream baseline. */
    JsonIoResult importSessionFromJsonOrReset(Layer& layer,
                                              EditorSessionState& state,
                                              const juce::String& jsonText);

    juce::File skinEditorSessionFile();
    bool saveSessionFile(const Layer& layer, const EditorSessionState& state, juce::String& error);
    JsonIoResult loadSessionFile(Layer& layer, EditorSessionState& state);
}
