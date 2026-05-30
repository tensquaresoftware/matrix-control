#include "ApvtsLayoutBuilder.h"

#include "Shared/Definitions/PluginHelpers.h"
#include "Shared/Definitions/PluginIDs.h"

juce::AudioProcessorValueTreeState::ParameterLayout ApvtsLayoutBuilder::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout;

    const auto& allGroups = PluginDescriptors::kAllApvtsGroups;
    std::map<juce::String, std::unique_ptr<juce::AudioProcessorParameterGroup>> groupMap;

    createRootGroups(allGroups, groupMap);
    createChildGroups(allGroups, groupMap);
    addRootGroupsToLayout(groupMap, parameterLayout);

    return parameterLayout;
}

void ApvtsLayoutBuilder::createRootGroups(
    const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups,
    std::map<juce::String, std::unique_ptr<juce::AudioProcessorParameterGroup>>& groupMap)
{
    for (const auto& group : allGroups)
    {
        if (group.parentId.isEmpty())
        {
            groupMap[group.groupId] = std::make_unique<juce::AudioProcessorParameterGroup>(
                group.groupId, group.displayName, kSubgroupSeparator
            );
        }
    }
}

void ApvtsLayoutBuilder::createChildGroups(
    const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups,
    std::map<juce::String, std::unique_ptr<juce::AudioProcessorParameterGroup>>& groupMap)
{
    for (const auto& group : allGroups)
    {
        if (group.parentId.isEmpty() || groupMap.find(group.parentId) == groupMap.end())
            continue;

        auto childGroup = std::make_unique<juce::AudioProcessorParameterGroup>(
            group.groupId, group.displayName, kSubgroupSeparator
        );

        addParametersToChildGroup(group.groupId, *childGroup);

        groupMap[group.parentId]->addChild(std::move(childGroup));
    }
}

void ApvtsLayoutBuilder::addParametersToChildGroup(
    const juce::String& childGroupId,
    juce::AudioProcessorParameterGroup& childGroup)
{
    if (childGroupId == PluginIDs::PatchEditSection::kGroupId)
    {
        addPatchEditParameters(childGroup);
    }
    else if (childGroupId == PluginIDs::MatrixModulationSection::kGroupId)
    {
        addMatrixModulationParameters(childGroup);
    }
    else if (childGroupId == PluginIDs::PatchManagerSection::kGroupId)
    {
        // Patch Manager uses ValueTree properties, not APVTS parameters
    }
    else if (childGroupId == PluginIDs::MasterEditSection::kGroupId)
    {
        addMasterEditParameters(childGroup);
    }
}

void ApvtsLayoutBuilder::addRootGroupsToLayout(
    std::map<juce::String, std::unique_ptr<juce::AudioProcessorParameterGroup>>& groupMap,
    juce::AudioProcessorValueTreeState::ParameterLayout& parameterLayout)
{
    for (auto& [id, group] : groupMap)
    {
        if (group->getParent() == nullptr)
        {
            parameterLayout.add(std::move(group));
        }
    }
}

void ApvtsLayoutBuilder::addIntParameter(
    juce::AudioProcessorParameterGroup& group,
    const PluginDescriptors::IntParameterDescriptor& desc)
{
    auto param = std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID(desc.parameterId, 1),
        desc.displayName,
        desc.minValue,
        desc.maxValue,
        desc.defaultValue
    );
    group.addChild(std::move(param));
}

void ApvtsLayoutBuilder::addChoiceParameter(
    juce::AudioProcessorParameterGroup& group,
    const PluginDescriptors::ChoiceParameterDescriptor& desc)
{
    auto param = std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(desc.parameterId, 1),
        desc.displayName,
        desc.choices,
        desc.defaultIndex
    );
    group.addChild(std::move(param));
}

const char* ApvtsLayoutBuilder::getBusId(int busNumber)
{
    static constexpr const char* busIds[] =
    {
        PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
        PluginIDs::MatrixModulationSection::ModulationBus::kBus9
    };

    jassert(busNumber >= 0 && busNumber < Matrix1000Limits::kModulationBusCount);
    return busIds[busNumber];
}

void ApvtsLayoutBuilder::addModuleParameters(
    juce::AudioProcessorParameterGroup& parentGroup,
    const char* moduleId,
    const char* moduleDisplayName,
    const std::vector<PluginDescriptors::IntParameterDescriptor>& intParams,
    const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& choiceParams)
{
    auto moduleGroup = std::make_unique<juce::AudioProcessorParameterGroup>(
        moduleId, moduleDisplayName, kSubgroupSeparator
    );

    for (const auto& paramDesc : intParams)
    {
        addIntParameter(*moduleGroup, paramDesc);
    }

    for (const auto& paramDesc : choiceParams)
    {
        addChoiceParameter(*moduleGroup, paramDesc);
    }

    parentGroup.addChild(std::move(moduleGroup));
}

void ApvtsLayoutBuilder::addPatchEditParameters(juce::AudioProcessorParameterGroup& patchEditGroup)
{
    addPatchEditTopModulesParameters(patchEditGroup);
    addPatchEditEnvelopeAndLfoModulesParameters(patchEditGroup);
}

void ApvtsLayoutBuilder::addPatchEditTopModulesParameters(juce::AudioProcessorParameterGroup& patchEditGroup)
{
    addPatchEditModuleParameters(patchEditGroup, PluginIDs::PatchEditSection::Dco1Module::kGroupId,
        PluginDescriptors::PatchEditSection::Dco1Module::kIntParameters,
        PluginDescriptors::PatchEditSection::Dco1Module::kChoiceParameters);
    addPatchEditModuleParameters(patchEditGroup, PluginIDs::PatchEditSection::Dco2Module::kGroupId,
        PluginDescriptors::PatchEditSection::Dco2Module::kIntParameters,
        PluginDescriptors::PatchEditSection::Dco2Module::kChoiceParameters);
    addPatchEditModuleParameters(patchEditGroup, PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
        PluginDescriptors::PatchEditSection::VcfVcaModule::kIntParameters,
        PluginDescriptors::PatchEditSection::VcfVcaModule::kChoiceParameters);
    addPatchEditModuleParameters(patchEditGroup, PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
        PluginDescriptors::PatchEditSection::FmTrackModule::kIntParameters,
        PluginDescriptors::PatchEditSection::FmTrackModule::kChoiceParameters);
    addPatchEditModuleParameters(patchEditGroup, PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
        PluginDescriptors::PatchEditSection::RampPortamentoModule::kIntParameters,
        PluginDescriptors::PatchEditSection::RampPortamentoModule::kChoiceParameters);
}

void ApvtsLayoutBuilder::addPatchEditEnvelopeAndLfoModulesParameters(juce::AudioProcessorParameterGroup& patchEditGroup)
{
    addPatchEditModuleParameters(patchEditGroup, PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
        PluginDescriptors::PatchEditSection::Envelope1Module::kIntParameters,
        PluginDescriptors::PatchEditSection::Envelope1Module::kChoiceParameters);
    addPatchEditModuleParameters(patchEditGroup, PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
        PluginDescriptors::PatchEditSection::Envelope2Module::kIntParameters,
        PluginDescriptors::PatchEditSection::Envelope2Module::kChoiceParameters);
    addPatchEditModuleParameters(patchEditGroup, PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
        PluginDescriptors::PatchEditSection::Envelope3Module::kIntParameters,
        PluginDescriptors::PatchEditSection::Envelope3Module::kChoiceParameters);
    addPatchEditModuleParameters(patchEditGroup, PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
        PluginDescriptors::PatchEditSection::Lfo1Module::kIntParameters,
        PluginDescriptors::PatchEditSection::Lfo1Module::kChoiceParameters);
    addPatchEditModuleParameters(patchEditGroup, PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
        PluginDescriptors::PatchEditSection::Lfo2Module::kIntParameters,
        PluginDescriptors::PatchEditSection::Lfo2Module::kChoiceParameters);
}

void ApvtsLayoutBuilder::addPatchEditModuleParameters(
    juce::AudioProcessorParameterGroup& patchEditGroup,
    const char* moduleGroupId,
    const std::vector<PluginDescriptors::IntParameterDescriptor>& intParams,
    const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& choiceParams)
{
    addModuleParameters(patchEditGroup, moduleGroupId,
        PluginHelpers::getGroupDisplayName(moduleGroupId).toRawUTF8(),
        intParams, choiceParams);
}

void ApvtsLayoutBuilder::addMasterEditParameters(juce::AudioProcessorParameterGroup& masterEditGroup)
{
    addMasterEditModuleParameters(masterEditGroup, PluginIDs::MasterEditSection::MidiModule::kGroupId);
    addMasterEditModuleParameters(masterEditGroup, PluginIDs::MasterEditSection::VibratoModule::kGroupId);
    addMasterEditModuleParameters(masterEditGroup, PluginIDs::MasterEditSection::MiscModule::kGroupId);
}

void ApvtsLayoutBuilder::addMasterEditModuleParameters(
    juce::AudioProcessorParameterGroup& masterEditGroup,
    const char* moduleGroupId)
{
    auto intParams = filterIntParametersByParentGroup(
        PluginDescriptors::MasterEditSection::kIntParameters, moduleGroupId);
    auto choiceParams = filterChoiceParametersByParentGroup(
        PluginDescriptors::MasterEditSection::kChoiceParameters, moduleGroupId);
    addModuleParameters(masterEditGroup, moduleGroupId,
        PluginHelpers::getGroupDisplayName(moduleGroupId).toRawUTF8(),
        intParams, choiceParams);
}

std::vector<PluginDescriptors::IntParameterDescriptor> ApvtsLayoutBuilder::filterIntParametersByParentGroup(
    const std::vector<PluginDescriptors::IntParameterDescriptor>& allParams,
    const char* parentGroupId)
{
    std::vector<PluginDescriptors::IntParameterDescriptor> filtered;
    for (const auto& param : allParams)
    {
        if (param.parentGroupId == parentGroupId)
            filtered.push_back(param);
    }
    return filtered;
}

std::vector<PluginDescriptors::ChoiceParameterDescriptor> ApvtsLayoutBuilder::filterChoiceParametersByParentGroup(
    const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& allParams,
    const char* parentGroupId)
{
    std::vector<PluginDescriptors::ChoiceParameterDescriptor> filtered;
    for (const auto& param : allParams)
    {
        if (param.parentGroupId == parentGroupId)
            filtered.push_back(param);
    }
    return filtered;
}

void ApvtsLayoutBuilder::addMatrixModulationParameters(juce::AudioProcessorParameterGroup& matrixModulationGroup)
{
    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
    {
        const juce::String busId = getBusId(bus);
        const juce::String busDisplayName = PluginHelpers::getGroupDisplayName(busId);

        auto busGroup = std::make_unique<juce::AudioProcessorParameterGroup>(
            busId, busDisplayName, kSubgroupSeparator
        );

        for (const auto& paramDesc : PluginDescriptors::MatrixModulationSection::kModulationBusIntParameters[static_cast<size_t>(bus)])
        {
            addIntParameter(*busGroup, paramDesc);
        }

        for (const auto& paramDesc : PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[static_cast<size_t>(bus)])
        {
            addChoiceParameter(*busGroup, paramDesc);
        }

        matrixModulationGroup.addChild(std::move(busGroup));
    }
}
