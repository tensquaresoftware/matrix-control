#pragma once

#include <array>
#include <map>
#include <optional>
#include <vector>

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include "GUI/Skins/SkinValues.h"

namespace TSS::DevSkin
{
    inline constexpr int kLegacySchemaVersion = 1;
    inline constexpr int kSchemaVersion = 2;
    /** Designer-only Colour set help label hard cap (ASCII). */
    inline constexpr int kColourSetLabelMaxChars = 16;

    /** Printable ASCII 32..126 — same charset Layer keeps for colourSetLabels. */
    inline const juce::String& colourSetLabelAllowedChars()
    {
        static const juce::String chars = []
        {
            juce::String s;
            s.preallocateBytes(95);
            for (int c = 32; c < 127; ++c)
                s += static_cast<juce::juce_wchar>(c);
            return s;
        }();
        return chars;
    }

    /** Trim, keep allowed ASCII, then hard-cap length (strip before truncate). */
    inline juce::String sanitizeColourSetLabel(const juce::String& raw)
    {
        const auto trimmed = raw.trim();
        const auto& allowed = colourSetLabelAllowedChars();
        juce::String asciiOnly;
        asciiOnly.preallocateBytes(static_cast<size_t>(trimmed.length()));
        for (int i = 0; i < trimmed.length(); ++i)
        {
            const auto ch = trimmed[i];
            if (allowed.containsChar(ch))
                asciiOnly += ch;
        }
        if (asciiOnly.length() > kColourSetLabelMaxChars)
            return asciiOnly.substring(0, kColourSetLabelMaxChars);
        return asciiOnly;
    }
    inline constexpr const char* kCompiledBlackId = "compiled:black";
    inline constexpr const char* kCompiledBlackDuoId = "compiled:black-duo";
    inline constexpr const char* kCompiledCreamId = "compiled:cream";

    enum class SkinKind
    {
        kCompiledBlack,
        kCompiledBlackDuo,
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
        /**
         * Designer-only help labels for ColourSet cs1..cs4 (document-global).
         * Omitted from PushPayload; optional in skin JSON as colourSetLabels.
         */
        std::array<juce::String, kAllColourSets.size()> colourSetLabels {};
        /**
         * Designer-only bidirectional mapping link groups (full SkinColourKey members).
         * Omitted from PushPayload; optional in skin JSON as mappingLinkGroups.
         * Each inner vector has >= 2 keys; a key belongs to at most one group.
         */
        std::vector<std::vector<SkinColourKey>> mappingLinkGroups {};
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
