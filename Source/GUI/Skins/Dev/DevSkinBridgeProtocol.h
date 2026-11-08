#pragma once

#include <juce_core/juce_core.h>

namespace TSS::DevSkin
{
    /** Localhost-only Skin Editor → Standalone Dev bridge port. */
    inline constexpr int kBridgePort = 39217;

    inline constexpr const char* kBridgeMagic = "MCSK";

    /** Reject framed payloads larger than this (bytes of UTF-8 body). */
    inline constexpr juce::uint32 kBridgeMaxPayloadBytes = 1u * 1024u * 1024u;

    juce::MemoryBlock encodeMessage(const juce::String& utf8Json);
    bool decodeMessage(const juce::MemoryBlock& block, juce::String& utf8Json);
}
