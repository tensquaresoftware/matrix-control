#include "MatrixModInitService.h"

#include "Core/MIDI/MatrixModBusParameterSysExDispatcher.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/PluginDescriptors.h"

namespace Core
{

MatrixModInitService::MatrixModInitService(
    PatchModel& patchModel,
    ApvtsPatchMapper& apvtsPatchMapper,
    const MatrixModBusParameterSysExDispatcher& sysExDispatcher)
    : patchModel_(patchModel)
    , apvtsPatchMapper_(apvtsPatchMapper)
    , sysExDispatcher_(sysExDispatcher)
{
}

void MatrixModInitService::initBus(int busIndex)
{
    if (!isValidBusIndex(busIndex))
        return;

    resetBusInModel(busIndex);
    apvtsPatchMapper_.pushBusToApvts(busIndex);
    sysExDispatcher_.dispatchBus(busIndex);
}

void MatrixModInitService::initAllBuses()
{
    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        resetBusInModel(bus);

    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        apvtsPatchMapper_.pushBusToApvts(bus);

    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        sysExDispatcher_.dispatchBus(bus);
}

void MatrixModInitService::resetBusInModel(int busIndex)
{
    using namespace PluginDescriptors::MatrixModulationSection;

    const auto& intParams = kModulationBusIntParameters[static_cast<size_t>(busIndex)];
    const auto& choiceParams = kModulationBusChoiceParameters[static_cast<size_t>(busIndex)];

    patchModel_.setChoiceIndex(choiceParams[0], kDefaultSourceIndex);
    patchModel_.setValue(intParams[0], kDefaultAmount);
    patchModel_.setChoiceIndex(choiceParams[1], kDefaultDestinationIndex);
}

bool MatrixModInitService::isValidBusIndex(int busIndex) noexcept
{
    return busIndex >= 0 && busIndex < Matrix1000Limits::kModulationBusCount;
}

} // namespace Core
