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

    /** Full Dev-layer snapshot: all Designer skins + activeSkinId (Save as draft workspace pack). */
    juce::String exportToJson(const Layer& layer);

    /** Single Designer skin handoff JSON (Export). */
    juce::String exportDesignerSkinToJson(const NamedSkin& skin);
    juce::String exportDesignerSkinToJson(const Layer& layer, const juce::String& skinId);

    /** Replace-all import (compiled Product skins rebuilt, Designer skins taken from the JSON). */
    JsonIoResult importFromJson(Layer& layer, const juce::String& jsonText);

    /**
     * Load: replace every Designer skin with the file contents (draft pack or single-skin
     * Export file). Refuses a file without Designer skins; the layer is untouched on failure.
     */
    JsonIoResult replaceDesignerSkinsFromJson(Layer& layer, const juce::String& jsonText);

    /** Append Designer skin(s) from single-skin or multi-skin JSON without wiping session. */
    JsonIoResult appendDesignerSkinsFromJson(Layer& layer, const juce::String& jsonText);

    juce::var pushPayloadToVar(const PushPayload& payload);
    bool pushPayloadFromVar(const juce::var& root, PushPayload& out, juce::String& error);
}
