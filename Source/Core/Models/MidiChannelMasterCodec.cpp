#include "MidiChannelMasterCodec.h"

#include "Core/Models/PackedFieldCodec.h"

namespace Core
{

    namespace
    {
        constexpr juce::uint8 kOmniOff = 0;
        constexpr juce::uint8 kOmniOn  = 1;
        constexpr juce::uint8 kMonoOff = 0;
        constexpr juce::uint8 kMonoOn  = 1;

        int clampComboIndex(int comboIndex) noexcept
        {
            return juce::jlimit(MidiChannelMasterCodec::kOmniComboIndex,
                                MidiChannelMasterCodec::kLastMonoIndex,
                                comboIndex);
        }

        juce::uint8 basicChannelWire(int humanChannel) noexcept
        {
            const int clamped = juce::jlimit(1, MidiChannelMasterCodec::kMaxBasicChannel, humanChannel);
            return static_cast<juce::uint8>(clamped - 1);
        }

        int humanChannelFromWire(juce::uint8 wire, int maxHuman) noexcept
        {
            return juce::jlimit(1, maxHuman, static_cast<int>(wire) + 1);
        }
    }

    void MidiChannelMasterCodec::applyComboIndex(juce::uint8* buffer,
                                                 size_t bufferSize,
                                                 int comboIndex) noexcept
    {
        jassert(buffer != nullptr);
        const size_t basicOffset = PackedFieldCodec::safeOffset(kBasicChannelOffset, bufferSize);
        const size_t omniOffset  = PackedFieldCodec::safeOffset(kOmniOffset, bufferSize);
        const size_t monoOffset  = PackedFieldCodec::safeOffset(kMonoOffset, bufferSize);

        const int index = clampComboIndex(comboIndex);

        if (index == kOmniComboIndex)
        {
            buffer[basicOffset] = basicChannelWire(1);
            buffer[omniOffset]  = kOmniOn;
            buffer[monoOffset]  = kMonoOff;
            return;
        }

        if (index <= kLastChannelIndex)
        {
            buffer[basicOffset] = basicChannelWire(index);
            buffer[omniOffset]  = kOmniOff;
            buffer[monoOffset]  = kMonoOff;
            return;
        }

        const int monoGroup = juce::jlimit(1, kMaxMonoGroup, index - kLastChannelIndex);
        buffer[basicOffset] = basicChannelWire(monoGroup);
        buffer[omniOffset]  = kOmniOff;
        buffer[monoOffset]  = kMonoOn;
    }

    int MidiChannelMasterCodec::readComboIndex(const juce::uint8* buffer, size_t bufferSize) noexcept
    {
        jassert(buffer != nullptr);
        const size_t basicOffset = PackedFieldCodec::safeOffset(kBasicChannelOffset, bufferSize);
        const size_t omniOffset  = PackedFieldCodec::safeOffset(kOmniOffset, bufferSize);
        const size_t monoOffset  = PackedFieldCodec::safeOffset(kMonoOffset, bufferSize);

        const juce::uint8 basicWire = buffer[basicOffset];
        const bool omniOn = buffer[omniOffset] != kOmniOff;
        const bool monoOn = buffer[monoOffset] != kMonoOff;

        if (monoOn)
        {
            const int monoGroup = humanChannelFromWire(basicWire, kMaxMonoGroup);
            return kLastChannelIndex + monoGroup;
        }

        if (omniOn)
            return kOmniComboIndex;

        return humanChannelFromWire(basicWire, kMaxBasicChannel);
    }

} // namespace Core
