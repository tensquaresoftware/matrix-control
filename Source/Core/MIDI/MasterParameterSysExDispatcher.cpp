#include "MasterParameterSysExDispatcher.h"

#include "Core/Models/ApvtsMasterMapper.h"

namespace Core
{

MasterParameterSysExDispatcher::MasterParameterSysExDispatcher(const MasterModel& masterModel,
                                                               EnqueueMasterFn enqueueMaster)
    : masterModel_(masterModel)
    , enqueueMaster_(std::move(enqueueMaster))
    , masterParameterIds_(buildMasterParameterIdSet())
{
    jassert(enqueueMaster_ != nullptr);
}

void MasterParameterSysExDispatcher::dispatch(const juce::String& parameterId) const
{
    if (enqueueMaster_ == nullptr)
        return;

    if (masterParameterIds_.count(parameterId) == 0)
        return;

    enqueueMaster_(masterModel_.data());
}

std::unordered_set<juce::String> MasterParameterSysExDispatcher::buildMasterParameterIdSet()
{
    std::unordered_set<juce::String> ids;

    for (const auto& d : ApvtsMasterMapper::buildIntDescriptors())
        ids.insert(d.parameterId);

    for (const auto& d : ApvtsMasterMapper::buildChoiceDescriptors())
        ids.insert(d.parameterId);

    return ids;
}

} // namespace Core
