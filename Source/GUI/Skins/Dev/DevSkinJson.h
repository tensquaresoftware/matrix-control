#pragma once

#include "GUI/Skins/Dev/DevSkinLayer.h"

namespace TSS::DevSkin
{
    struct JsonIoResult
    {
        bool ok = false;
        juce::String message;
        juce::StringArray importedSkinIds;

        static JsonIoResult success() { return { true, {}, {} }; }
        static JsonIoResult failure(juce::String msg) { return { false, std::move(msg), {} }; }
    };

    /** Full Dev-layer snapshot (session / legacy multi-skin). */
    juce::String exportToJson(const Layer& layer);

    /** Single Editor skin handoff JSON (Export). */
    juce::String exportEditorSkinToJson(const NamedSkin& skin);
    juce::String exportEditorSkinToJson(const Layer& layer, const juce::String& skinId);

    /** Replace-all import used by session restore and legacy round-trip tests. */
    JsonIoResult importFromJson(Layer& layer, const juce::String& jsonText);

    /** Append Editor skin(s) from single-skin or multi-skin JSON without wiping session. */
    JsonIoResult appendEditorSkinsFromJson(Layer& layer, const juce::String& jsonText);

    juce::var pushPayloadToVar(const PushPayload& payload);
    bool pushPayloadFromVar(const juce::var& root, PushPayload& out, juce::String& error);
}
