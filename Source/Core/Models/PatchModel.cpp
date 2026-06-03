#include <cstring>

#include "PatchModel.h"

#include "Core/Models/PackedFieldCodec.h"

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

    void PatchModel::loadFrom(const juce::uint8* packedData) noexcept
    {
        jassert(packedData != nullptr);
        std::memcpy(buffer_.data(), packedData, kBufferSize);
    }

    int PatchModel::getValue(const PluginDescriptors::IntParameterDescriptor& descriptor) const
    {
        const size_t offset = PackedFieldCodec::safeOffset(descriptor.sysExOffset, kBufferSize);
        return PackedFieldCodec::decodeField(buffer_[offset], descriptor.minValue, descriptor.maxValue);
    }

    void PatchModel::setValue(const PluginDescriptors::IntParameterDescriptor& descriptor, int value)
    {
        const size_t offset = PackedFieldCodec::safeOffset(descriptor.sysExOffset, kBufferSize);
        buffer_[offset] = PackedFieldCodec::encodeField(value, descriptor.minValue, descriptor.maxValue);
    }

    int PatchModel::getChoiceIndex(const PluginDescriptors::ChoiceParameterDescriptor& descriptor) const
    {
        const int lastIndex = juce::jmax(0, descriptor.choices.size() - 1);
        const size_t offset = PackedFieldCodec::safeOffset(descriptor.sysExOffset, kBufferSize);
        return juce::jlimit(0, lastIndex, static_cast<int>(buffer_[offset]));
    }

    void PatchModel::setChoiceIndex(const PluginDescriptors::ChoiceParameterDescriptor& descriptor, int index)
    {
        const int lastIndex = juce::jmax(0, descriptor.choices.size() - 1);
        const size_t offset = PackedFieldCodec::safeOffset(descriptor.sysExOffset, kBufferSize);
        buffer_[offset] = static_cast<juce::uint8>(juce::jlimit(0, lastIndex, index));
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

    juce::juce_wchar PatchModel::decodeNameChar(juce::uint8 raw) noexcept
    {
        int character = raw & kSixBitMask;
        if (character < kNamePrintableBase)
            character += kNameLetterBase;

        return static_cast<juce::juce_wchar>(character);
    }

}
