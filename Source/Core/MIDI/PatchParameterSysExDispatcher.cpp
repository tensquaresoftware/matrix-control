#include "PatchParameterSysExDispatcher.h"

#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PackedFieldCodec.h"

namespace Core
{

PatchParameterSysExDispatcher::PatchParameterSysExDispatcher(const PatchModel& patchModel,
                                                             EnqueueRemoteEditFn enqueueRemoteEdit)
    : patchModel_(patchModel)
    , enqueueRemoteEdit_(std::move(enqueueRemoteEdit))
    , intByParameterId_(buildIntMap())
    , choiceByParameterId_(buildChoiceMap())
{
    jassert(enqueueRemoteEdit_ != nullptr);
}

void PatchParameterSysExDispatcher::dispatchModule(const juce::String& moduleGroupId) const
{
    for (const auto& d : ApvtsPatchMapper::buildIntDescriptors())
    {
        if (d.parentGroupId == moduleGroupId && d.sysExId != PluginDescriptors::kNoSysExId)
            dispatch(d.parameterId);
    }

    for (const auto& d : ApvtsPatchMapper::buildChoiceDescriptors())
    {
        if (d.parentGroupId == moduleGroupId && d.sysExId != PluginDescriptors::kNoSysExId)
            dispatch(d.parameterId);
    }
}

void PatchParameterSysExDispatcher::dispatch(const juce::String& parameterId) const
{
    if (enqueueRemoteEdit_ == nullptr)
        return;

    if (const auto intIt = intByParameterId_.find(parameterId); intIt != intByParameterId_.end())
    {
        const auto& d = intIt->second;
        if (d.sysExId == PluginDescriptors::kNoSysExId)
            return;

        enqueueRemoteEdit_(d.sysExId, readPackedByte(d.sysExOffset));
        return;
    }

    if (const auto choiceIt = choiceByParameterId_.find(parameterId); choiceIt != choiceByParameterId_.end())
    {
        const auto& d = choiceIt->second;
        if (d.sysExId == PluginDescriptors::kNoSysExId)
            return;

        enqueueRemoteEdit_(d.sysExId, readPackedByte(d.sysExOffset));
    }
}

juce::uint8 PatchParameterSysExDispatcher::readPackedByte(int sysExOffset) const noexcept
{
    const size_t offset = PackedFieldCodec::safeOffset(sysExOffset, PatchModel::kBufferSize);
    return patchModel_.data()[offset];
}

std::unordered_map<juce::String, PluginDescriptors::IntParameterDescriptor>
PatchParameterSysExDispatcher::buildIntMap()
{
    std::unordered_map<juce::String, PluginDescriptors::IntParameterDescriptor> map;
    for (const auto& d : ApvtsPatchMapper::buildIntDescriptors())
        map.emplace(d.parameterId, d);

    return map;
}

std::unordered_map<juce::String, PluginDescriptors::ChoiceParameterDescriptor>
PatchParameterSysExDispatcher::buildChoiceMap()
{
    std::unordered_map<juce::String, PluginDescriptors::ChoiceParameterDescriptor> map;
    for (const auto& d : ApvtsPatchMapper::buildChoiceDescriptors())
        map.emplace(d.parameterId, d);

    return map;
}

} // namespace Core
