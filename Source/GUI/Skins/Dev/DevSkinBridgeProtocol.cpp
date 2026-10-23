#include "GUI/Skins/Dev/DevSkinBridgeProtocol.h"

#include <cstring>

namespace TSS::DevSkin
{
    juce::MemoryBlock encodeMessage(const juce::String& utf8Json)
    {
        const auto utf8 = utf8Json.toRawUTF8();
        const auto byteCount = static_cast<juce::uint32>(strlen(utf8));

        juce::MemoryBlock block;
        block.append(kBridgeMagic, 4);

        juce::uint32 length = juce::ByteOrder::swapIfBigEndian(byteCount);
        block.append(&length, sizeof(length));
        block.append(utf8, byteCount);
        return block;
    }

    bool decodeMessage(const juce::MemoryBlock& block, juce::String& utf8Json)
    {
        if (block.getSize() < 8)
            return false;

        const auto* data = static_cast<const char*>(block.getData());
        if (std::memcmp(data, kBridgeMagic, 4) != 0)
            return false;

        juce::uint32 length = 0;
        std::memcpy(&length, data + 4, sizeof(length));
        length = juce::ByteOrder::swapIfBigEndian(length);

        if (length > kBridgeMaxPayloadBytes)
            return false;
        if (block.getSize() < 8 + static_cast<size_t>(length))
            return false;

        utf8Json = juce::String::fromUTF8(data + 8, static_cast<int>(length));
        return true;
    }
}
