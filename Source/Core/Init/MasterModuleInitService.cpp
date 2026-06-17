#include "MasterModuleInitService.h"

#include "Core/MIDI/MasterParameterSysExDispatcher.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/MasterModel.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

MasterModuleInitService::MasterModuleInitService(
    MasterModel& masterModel,
    ApvtsMasterMapper& apvtsMasterMapper,
    InitTemplateLoader& initTemplateLoader,
    const MasterParameterSysExDispatcher& sysExDispatcher,
    TemplatesFolderSupplier templatesFolder)
    : masterModel_(masterModel)
    , apvtsMasterMapper_(apvtsMasterMapper)
    , initTemplateLoader_(initTemplateLoader)
    , sysExDispatcher_(sysExDispatcher)
    , templatesFolder_(std::move(templatesFolder))
{
}

InitTemplateLoadResult MasterModuleInitService::initModule(MasterModuleKind module)
{
    const auto moduleGroupId = moduleGroupIdFor(module);
    if (moduleGroupId.isEmpty())
        return {};

    MasterModel initTemplate;
    const auto result = initTemplateLoader_.loadMaster(initTemplate, templatesFolder_());

    copyModuleFromInitTemplate(initTemplate, moduleGroupId);
    apvtsMasterMapper_.pushModuleToApvts(moduleGroupId);
    sysExDispatcher_.dispatchFull();

    return result;
}

juce::String MasterModuleInitService::moduleGroupIdFor(MasterModuleKind module) noexcept
{
    using namespace PluginIDs::MasterEditSection;

    switch (module)
    {
        case MasterModuleKind::kMidi:    return MidiModule::kGroupId;
        case MasterModuleKind::kVibrato: return VibratoModule::kGroupId;
        case MasterModuleKind::kMisc:    return MiscModule::kGroupId;
    }

    return {};
}

void MasterModuleInitService::copyModuleFromInitTemplate(const MasterModel& initTemplate,
                                                         const juce::String& moduleGroupId)
{
    using namespace PluginDescriptors::MasterEditSection;

    for (const auto& descriptor : kIntParameters)
    {
        if (descriptor.parentGroupId == moduleGroupId)
            masterModel_.setValue(descriptor, initTemplate.getValue(descriptor));
    }

    for (const auto& descriptor : kChoiceParameters)
    {
        if (descriptor.parentGroupId == moduleGroupId)
            masterModel_.setChoiceIndex(descriptor, initTemplate.getChoiceIndex(descriptor));
    }
}

} // namespace Core
