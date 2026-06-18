#include "PatchModuleInitService.h"

#include "Core/MIDI/PatchParameterSysExDispatcher.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

PatchModuleInitService::PatchModuleInitService(
    PatchModel& patchModel,
    ApvtsPatchMapper& apvtsPatchMapper,
    InitTemplateLoader& initTemplateLoader,
    const PatchParameterSysExDispatcher& sysExDispatcher,
    TemplatesFolderSupplier templatesFolder)
    : patchModel_(patchModel)
    , apvtsPatchMapper_(apvtsPatchMapper)
    , initTemplateLoader_(initTemplateLoader)
    , sysExDispatcher_(sysExDispatcher)
    , templatesFolder_(std::move(templatesFolder))
{
}

InitTemplateLoadResult PatchModuleInitService::initModule(const juce::String& moduleGroupId)
{
    if (moduleGroupId.isEmpty())
        return {};

    PatchModel initTemplate;
    const auto result = initTemplateLoader_.loadPatch(initTemplate, templatesFolder_());

    copyModuleFromInitTemplate(initTemplate, moduleGroupId);
    apvtsPatchMapper_.pushModuleToApvts(moduleGroupId);
    sysExDispatcher_.dispatchModule(moduleGroupId);

    return result;
}

juce::String PatchModuleInitService::moduleGroupIdFromInitPropertyId(const juce::String& propertyId) noexcept
{
    using namespace PluginIDs::PatchEditSection;

    if (propertyId == Dco1Module::StandaloneWidgets::kInit) return Dco1Module::kGroupId;
    if (propertyId == Dco2Module::StandaloneWidgets::kInit) return Dco2Module::kGroupId;
    if (propertyId == VcfVcaModule::StandaloneWidgets::kInit) return VcfVcaModule::kGroupId;
    if (propertyId == FmTrackModule::StandaloneWidgets::kInit) return FmTrackModule::kGroupId;
    if (propertyId == RampPortamentoModule::StandaloneWidgets::kInit) return RampPortamentoModule::kGroupId;
    if (propertyId == Envelope1Module::StandaloneWidgets::kInit) return Envelope1Module::kGroupId;
    if (propertyId == Envelope2Module::StandaloneWidgets::kInit) return Envelope2Module::kGroupId;
    if (propertyId == Envelope3Module::StandaloneWidgets::kInit) return Envelope3Module::kGroupId;
    if (propertyId == Lfo1Module::StandaloneWidgets::kInit) return Lfo1Module::kGroupId;
    if (propertyId == Lfo2Module::StandaloneWidgets::kInit) return Lfo2Module::kGroupId;

    return {};
}

juce::String PatchModuleInitService::moduleGroupIdFromPatchModuleKind(PatchModuleKind kind) noexcept
{
    using namespace PluginIDs::PatchEditSection;

    switch (kind)
    {
        case PatchModuleKind::Dco1: return Dco1Module::kGroupId;
        case PatchModuleKind::Dco2: return Dco2Module::kGroupId;
        case PatchModuleKind::Env1: return Envelope1Module::kGroupId;
        case PatchModuleKind::Env2: return Envelope2Module::kGroupId;
        case PatchModuleKind::Env3: return Envelope3Module::kGroupId;
        case PatchModuleKind::Lfo1: return Lfo1Module::kGroupId;
        case PatchModuleKind::Lfo2: return Lfo2Module::kGroupId;
    }

    return {};
}

void PatchModuleInitService::copyModuleFromInitTemplate(const PatchModel& initTemplate,
                                                        const juce::String& moduleGroupId)
{
    for (const auto& descriptor : ApvtsPatchMapper::buildIntDescriptors())
    {
        if (descriptor.parentGroupId == moduleGroupId)
            patchModel_.setValue(descriptor, initTemplate.getValue(descriptor));
    }

    for (const auto& descriptor : ApvtsPatchMapper::buildChoiceDescriptors())
    {
        if (descriptor.parentGroupId == moduleGroupId)
            patchModel_.setChoiceIndex(descriptor, initTemplate.getChoiceIndex(descriptor));
    }
}

} // namespace Core
