#pragma once

#include "GUI/Skins/Dev/DevSkinLayer.h"

namespace TSS::DevSkin
{
    struct JsonIoResult
    {
        bool ok = false;
        juce::String message;
    };

    juce::String exportToJson(const Layer& layer);
    JsonIoResult importFromJson(Layer& layer, const juce::String& jsonText);

    juce::var pushPayloadToVar(const PushPayload& payload);
    bool pushPayloadFromVar(const juce::var& root, PushPayload& out, juce::String& error);
}
