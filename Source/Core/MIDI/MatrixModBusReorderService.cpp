#include "MatrixModBusReorderService.h"

#include "Core/MIDI/MatrixModBusParameterSysExDispatcher.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/PluginDescriptors.h"

namespace Core
{

MatrixModBusReorderService::MatrixModBusReorderService(
    PatchModel& patchModel,
    ApvtsPatchMapper& apvtsPatchMapper,
    const MatrixModBusParameterSysExDispatcher& sysExDispatcher)
    : patchModel_(patchModel)
    , apvtsPatchMapper_(apvtsPatchMapper)
    , sysExDispatcher_(sysExDispatcher)
{
}

void MatrixModBusReorderService::swapBusContents(int busA, int busB)
{
    if (busA == busB)
        return;

    if (!isValidBusIndex(busA) || !isValidBusIndex(busB))
        return;

    using namespace PluginDescriptors::MatrixModulationSection;

    const auto readTriplet = [this](int bus, int& source, int& amount, int& destination)
    {
        const auto& intParams = kModulationBusIntParameters[static_cast<size_t>(bus)];
        const auto& choiceParams = kModulationBusChoiceParameters[static_cast<size_t>(bus)];

        source = patchModel_.getChoiceIndex(choiceParams[0]);
        amount = patchModel_.getValue(intParams[0]);
        destination = patchModel_.getChoiceIndex(choiceParams[1]);
    };

    const auto writeTriplet = [this](int bus, int source, int amount, int destination)
    {
        const auto& intParams = kModulationBusIntParameters[static_cast<size_t>(bus)];
        const auto& choiceParams = kModulationBusChoiceParameters[static_cast<size_t>(bus)];

        patchModel_.setChoiceIndex(choiceParams[0], source);
        patchModel_.setValue(intParams[0], amount);
        patchModel_.setChoiceIndex(choiceParams[1], destination);
    };

    int sourceA = 0;
    int amountA = 0;
    int destinationA = 0;
    int sourceB = 0;
    int amountB = 0;
    int destinationB = 0;

    readTriplet(busA, sourceA, amountA, destinationA);
    readTriplet(busB, sourceB, amountB, destinationB);

    writeTriplet(busA, sourceB, amountB, destinationB);
    writeTriplet(busB, sourceA, amountA, destinationA);

    apvtsPatchMapper_.pushBusToApvts(busA);
    apvtsPatchMapper_.pushBusToApvts(busB);

    const int firstBus = juce::jmin(busA, busB);
    const int secondBus = juce::jmax(busA, busB);

    sysExDispatcher_.dispatchBus(firstBus);
    sysExDispatcher_.dispatchBus(secondBus);
}

bool MatrixModBusReorderService::isValidBusIndex(int busIndex) noexcept
{
    return busIndex >= 0 && busIndex < Matrix1000Limits::kModulationBusCount;
}

} // namespace Core
