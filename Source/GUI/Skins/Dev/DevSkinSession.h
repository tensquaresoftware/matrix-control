#pragma once

#include "GUI/Skins/Dev/DevSkinJson.h"

namespace TSS::DevSkin
{
    /** How Skin Designer reacts when the last loaded JSON changes on disk. */
    enum class JsonSyncMode
    {
        kOff,  // ignore external edits
        kAsk,  // prompt Reload / Discard (default)
        kAuto  // reload without prompt (disk wins, even if Designer is dirty)
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

    /**
     * Skin Designer launch preferences. Designer skins are never restored from disk:
     * the workspace starts blank and is filled with Load.
     */
    struct DesignerSessionState
    {
        /** Runtime only: selected Product skin (empty = none / "Select a skin"). */
        juce::String productSkinId;
        /** Runtime only: active Designer skin of the open workspace (empty = none). */
        juce::String designerSkinId;
        /** Runtime only: Compare always starts Off. */
        bool compareOn = false;
        /** Persisted: parent folder of the last Load / Save as draft / Export (empty = Documents). */
        juce::String lastSkinJsonDirectory;
        /** Persisted JSON Sync combo; missing values fall back to Ask. */
        JsonSyncMode jsonSyncMode = JsonSyncMode::kAsk;
    };

    /** Document id written into (and required from) the session file. */
    inline constexpr const char* kSessionDocumentId = "skinDesignerSession";

    /**
     * Blank launch: clear Product selection (never persisted). Call after session load so a
     * stale in-memory id cannot survive preferences restore.
     */
    void clearProductSelectionAtLaunch(DesignerSessionState& state);

    /** Preferences-only session document (no skins, no watch path). */
    juce::String exportSessionToJson(const DesignerSessionState& state);

    /** Reads preferences only; refuses any document that is not a Skin Designer session. */
    JsonIoResult importSessionFromJson(DesignerSessionState& state, const juce::String& jsonText);

    juce::File skinDesignerSessionFile();
    bool saveSessionFile(const DesignerSessionState& state, juce::String& error);
    /** Loads preferences; missing / old / corrupt files fail and leave state unchanged. */
    JsonIoResult loadSessionFile(DesignerSessionState& state);
}
