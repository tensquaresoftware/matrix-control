#pragma once

#include <map>
#include <optional>
#include <vector>

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include "GUI/Skins/SkinValues.h"

namespace TSS::DevSkin
{
    inline constexpr int kSchemaVersion = 1;
    inline constexpr const char* kCompiledBlackId = "compiled:black";
    inline constexpr const char* kCompiledCreamId = "compiled:cream";

    enum class SkinKind
    {
        kCompiledBlack,
        kCompiledCream,
        kDev
    };

    struct Pastille
    {
        juce::String name;
        juce::uint32 argb = 0xffffffff;
        /** True until the author first changes ARGB via the colour editor. */
        bool pending = false;
    };

    struct Mapping
    {
        SkinColourKey key;
        juce::String paletteName;
    };

    struct NamedSkin
    {
        juce::String id;
        juce::String name;
        SkinKind kind = SkinKind::kDev;
        juce::String derivedFrom;
        std::vector<Pastille> palette;
        std::map<SkinColourKey, juce::String> mappings;
    };

    struct PushPayload
    {
        juce::int64 revision = 0;
        juce::String activeSkinId;
        SkinKind kind = SkinKind::kCompiledCream;
        juce::String derivedFrom;
        /** Ultimate compiled Black/Cream ancestor (walked through Dev-from-Dev). */
        juce::String compiledAncestorId;
        std::vector<Pastille> palette;
        std::map<SkinColourKey, juce::String> mappings;
    };

    struct ApplyResult
    {
        bool ok = false;
        juce::String code;
        juce::String message;
        juce::String appliedSkinId;
        juce::int64 revision = 0;
    };
}
