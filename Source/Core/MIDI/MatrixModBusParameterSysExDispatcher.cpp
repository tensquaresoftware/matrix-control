#include "MatrixModBusParameterSysExDispatcher.h"

#include "Core/Models/PackedFieldCodec.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/PluginDescriptors.h"

namespace Core
{

MatrixModBusParameterSysExDispatcher::MatrixModBusParameterSysExDispatcher(
    const PatchModel& patchModel,
    EnqueueMatrixModBusEditFn enqueueMatrixModBusEdit)
    : patchModel_(patchModel)
    , enqueueMatrixModBusEdit_(std::move(enqueueMatrixModBusEdit))
    , parameterIdToBus_(buildParameterIdToBusMap())
{
    jassert(enqueueMatrixModBusEdit_ != nullptr);
}

void MatrixModBusParameterSysExDispatcher::dispatch(const juce::String& parameterId) const
{
    if (enqueueMatrixModBusEdit_ == nullptr)
        return;

    const auto busIt = parameterIdToBus_.find(parameterId);
    if (busIt == parameterIdToBus_.end())
        return;

    const int busIndex = busIt->second;
    jassert(busIndex >= 0 && busIndex < Matrix1000Limits::kModulationBusCount);

    using namespace PluginDescriptors::MatrixModulationSection;

    const auto& intParams = kModulationBusIntParameters[static_cast<size_t>(busIndex)];
    const auto& choiceParams = kModulationBusChoiceParameters[static_cast<size_t>(busIndex)];

    const juce::uint8 source = readPackedByte(choiceParams[0].sysExOffset);
    const juce::uint8 amount = readPackedByte(intParams[0].sysExOffset);
    const juce::uint8 destination = readPackedByte(choiceParams[1].sysExOffset);

    enqueueMatrixModBusEdit_(static_cast<juce::uint8>(busIndex), source, amount, destination);
}

juce::uint8 MatrixModBusParameterSysExDispatcher::readPackedByte(int sysExOffset) const noexcept
{
    const size_t offset = PackedFieldCodec::safeOffset(sysExOffset, PatchModel::kBufferSize);
    return patchModel_.data()[offset];
}

std::unordered_map<juce::String, int> MatrixModBusParameterSysExDispatcher::buildParameterIdToBusMap()
{
    using namespace PluginDescriptors::MatrixModulationSection;

    std::unordered_map<juce::String, int> map;

    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
    {
        for (const auto& d : kModulationBusIntParameters[static_cast<size_t>(bus)])
            map.emplace(d.parameterId, bus);

        for (const auto& d : kModulationBusChoiceParameters[static_cast<size_t>(bus)])
            map.emplace(d.parameterId, bus);
    }

    return map;
}

} // namespace Core
