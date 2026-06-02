#include <cstring>

#include "PatchModel.h"

namespace Core
{

    namespace
    {
        // Matrix name charset: a byte's low 6 bits hold the character. Codes below
        // kNamePrintableBase map to the uppercase/symbol block at kNameLetterBase.
        constexpr juce::uint8 kSixBitMask = 0x3F;
        constexpr int kNamePrintableBase = 0x20;
        constexpr int kNameLetterBase = 0x40;
        constexpr juce::uint8 kSevenBitMask = 0x7F;
        constexpr juce::juce_wchar kNamePadChar = ' ';
    }

    size_t PatchModel::safeOffset(int sysExOffset) noexcept
    {
        jassert(sysExOffset >= 0 && static_cast<size_t>(sysExOffset) < kBufferSize);
        return static_cast<size_t>(sysExOffset);
    }

    void PatchModel::loadFrom(const juce::uint8* packedData) noexcept
    {
        jassert(packedData != nullptr);
        std::memcpy(buffer_.data(), packedData, kBufferSize);
    }

    int PatchModel::getValue(const PluginDescriptors::IntParameterDescriptor& descriptor) const
    {
        return decodeField(buffer_[safeOffset(descriptor.sysExOffset)], descriptor.minValue, descriptor.maxValue);
    }

    void PatchModel::setValue(const PluginDescriptors::IntParameterDescriptor& descriptor, int value)
    {
        buffer_[safeOffset(descriptor.sysExOffset)] = encodeField(value, descriptor.minValue, descriptor.maxValue);
    }

    int PatchModel::getChoiceIndex(const PluginDescriptors::ChoiceParameterDescriptor& descriptor) const
    {
        const int lastIndex = juce::jmax(0, descriptor.choices.size() - 1);
        return juce::jlimit(0, lastIndex, static_cast<int>(buffer_[safeOffset(descriptor.sysExOffset)]));
    }

    void PatchModel::setChoiceIndex(const PluginDescriptors::ChoiceParameterDescriptor& descriptor, int index)
    {
        const int lastIndex = juce::jmax(0, descriptor.choices.size() - 1);
        buffer_[safeOffset(descriptor.sysExOffset)] = static_cast<juce::uint8>(juce::jlimit(0, lastIndex, index));
    }

    juce::String PatchModel::getName() const
    {
        juce::String name;
        for (int i = 0; i < kNameLength; ++i)
            name += juce::String::charToString(decodeNameChar(buffer_[static_cast<size_t>(i)]));

        return name.trimEnd();
    }

    void PatchModel::setName(const juce::String& name)
    {
        const juce::String upper = name.toUpperCase();
        for (int i = 0; i < kNameLength; ++i)
        {
            const juce::juce_wchar character = i < upper.length() ? upper[i] : kNamePadChar;
            buffer_[static_cast<size_t>(i)] = static_cast<juce::uint8>(character) & kSevenBitMask;
        }
    }

    int PatchModel::signBitPosition(int maxValue) noexcept
    {
        int position = 0;
        while ((1 << (position + 1)) <= maxValue)
            ++position;

        return position + 1;
    }

    int PatchModel::decodeField(juce::uint8 raw, int minValue, int maxValue) noexcept
    {
        if (! isSignedField(minValue))
            return juce::jlimit(minValue, maxValue, static_cast<int>(raw));

        const int signBit = signBitPosition(maxValue);
        const int fieldMask = (1 << (signBit + 1)) - 1;
        int value = raw & fieldMask;
        if ((value & (1 << signBit)) != 0)
            value -= (1 << (signBit + 1));

        return juce::jlimit(minValue, maxValue, value);
    }

    juce::uint8 PatchModel::encodeField(int value, int minValue, int maxValue) noexcept
    {
        // Signed fields are stored as 8-bit two's complement (the synth sign-extends
        // bit 6 into bit 7), so the plain cast reproduces the synth's canonical byte.
        return static_cast<juce::uint8>(juce::jlimit(minValue, maxValue, value));
    }

    juce::juce_wchar PatchModel::decodeNameChar(juce::uint8 raw) noexcept
    {
        int character = raw & kSixBitMask;
        if (character < kNamePrintableBase)
            character += kNameLetterBase;

        return static_cast<juce::juce_wchar>(character);
    }

}
