#include <cstring>

#include "MasterModel.h"

#include "Core/Models/PackedFieldCodec.h"

namespace Core
{

    void MasterModel::loadFrom(const juce::uint8* packedData) noexcept
    {
        jassert(packedData != nullptr);
        std::memcpy(buffer_.data(), packedData, kBufferSize);
    }

    int MasterModel::getValue(const PluginDescriptors::IntParameterDescriptor& descriptor) const
    {
        const size_t offset = PackedFieldCodec::safeOffset(descriptor.sysExOffset, kBufferSize);
        return PackedFieldCodec::decodeField(buffer_[offset], descriptor.minValue, descriptor.maxValue);
    }

    void MasterModel::setValue(const PluginDescriptors::IntParameterDescriptor& descriptor, int value)
    {
        const size_t offset = PackedFieldCodec::safeOffset(descriptor.sysExOffset, kBufferSize);
        buffer_[offset] = PackedFieldCodec::encodeField(value, descriptor.minValue, descriptor.maxValue);
    }

    int MasterModel::getChoiceIndex(const PluginDescriptors::ChoiceParameterDescriptor& descriptor) const
    {
        const int lastIndex = juce::jmax(0, descriptor.choices.size() - 1);
        const size_t offset = PackedFieldCodec::safeOffset(descriptor.sysExOffset, kBufferSize);
        return juce::jlimit(0, lastIndex, static_cast<int>(buffer_[offset]));
    }

    void MasterModel::setChoiceIndex(const PluginDescriptors::ChoiceParameterDescriptor& descriptor, int index)
    {
        const int lastIndex = juce::jmax(0, descriptor.choices.size() - 1);
        const size_t offset = PackedFieldCodec::safeOffset(descriptor.sysExOffset, kBufferSize);
        buffer_[offset] = static_cast<juce::uint8>(juce::jlimit(0, lastIndex, index));
    }

}
