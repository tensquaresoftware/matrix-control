#pragma once

#include "GUI/Skins/Dev/DevSkinJson.h"

namespace TSS::DevSkin
{
    struct EditorSessionState
    {
        juce::String productSkinId = kCompiledCreamId;
        juce::String editorSkinId;
        bool compareOn = false;
        /** Absolute path of last Export/Import parent folder (empty = Documents). */
        juce::String lastSkinJsonDirectory;
    };

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
