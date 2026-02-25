#include "ApvtsFactory.h"

#include <set>
#include <functional>

#include "Shared/PluginIDs.h"

juce::AudioProcessorValueTreeState::ParameterLayout ApvtsFactory::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout;
    
    const auto& allGroups = PluginDescriptors::kAllApvtsGroups;
    std::map<juce::String, std::unique_ptr<juce::AudioProcessorParameterGroup>> groupMap;
    
    createRootGroups(allGroups, groupMap);
    createChildGroups(allGroups, groupMap);
    addRootGroupsToLayout(groupMap, parameterLayout);
    
    return parameterLayout;
}

void ApvtsFactory::createRootGroups(
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

void ApvtsFactory::createChildGroups(
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

void ApvtsFactory::addParametersToChildGroup(
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

void ApvtsFactory::addRootGroupsToLayout(
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

void ApvtsFactory::addIntParameter(
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

void ApvtsFactory::addChoiceParameter(
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

std::vector<PluginDescriptors::IntParameterDescriptor> ApvtsFactory::getAllIntParameters()
{
    std::vector<PluginDescriptors::IntParameterDescriptor> allParams;

    auto addParams = [&allParams](const std::vector<PluginDescriptors::IntParameterDescriptor>& params)
    {
        allParams.insert(allParams.end(), params.begin(), params.end());
    };

    addParams(PluginDescriptors::kDco1IntParameters);
    addParams(PluginDescriptors::kDco2IntParameters);
    addParams(PluginDescriptors::kVcfVcaIntParameters);
    addParams(PluginDescriptors::kFmTrackIntParameters);
    addParams(PluginDescriptors::kRampPortamentoIntParameters);
    addParams(PluginDescriptors::kEnv1IntParameters);
    addParams(PluginDescriptors::kEnv2IntParameters);
    addParams(PluginDescriptors::kEnv3IntParameters);
    addParams(PluginDescriptors::kLfo1IntParameters);
    addParams(PluginDescriptors::kLfo2IntParameters);

    addMatrixModulationBusIntParameters(allParams);

    addParams(PluginDescriptors::kMasterEditIntParameters);

    return allParams;
}

std::vector<PluginDescriptors::ChoiceParameterDescriptor> ApvtsFactory::getAllChoiceParameters()
{
    std::vector<PluginDescriptors::ChoiceParameterDescriptor> allParams;

    auto addParams = [&allParams](const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& params)
    {
        allParams.insert(allParams.end(), params.begin(), params.end());
    };

    addParams(PluginDescriptors::kDco1ChoiceParameters);
    addParams(PluginDescriptors::kDco2ChoiceParameters);
    addParams(PluginDescriptors::kVcfVcaChoiceParameters);
    addParams(PluginDescriptors::kFmTrackChoiceParameters);
    addParams(PluginDescriptors::kRampPortamentoChoiceParameters);
    addParams(PluginDescriptors::kEnv1ChoiceParameters);
    addParams(PluginDescriptors::kEnv2ChoiceParameters);
    addParams(PluginDescriptors::kEnv3ChoiceParameters);
    addParams(PluginDescriptors::kLfo1ChoiceParameters);
    addParams(PluginDescriptors::kLfo2ChoiceParameters);

    addMatrixModulationBusChoiceParameters(allParams);

    addParams(PluginDescriptors::kMasterEditChoiceParameters);

    return allParams;
}

std::vector<PluginDescriptors::StandaloneWidgetDescriptor> ApvtsFactory::getAllStandaloneWidgets()
{
    std::vector<PluginDescriptors::StandaloneWidgetDescriptor> allWidgets;

    auto addWidgets = [&allWidgets](const std::vector<PluginDescriptors::StandaloneWidgetDescriptor>& widgets)
    {
        allWidgets.insert(allWidgets.end(), widgets.begin(), widgets.end());
    };

    addWidgets(PluginDescriptors::kDco1StandaloneWidgets);
    addWidgets(PluginDescriptors::kDco2StandaloneWidgets);
    addWidgets(PluginDescriptors::kEnv1StandaloneWidgets);
    addWidgets(PluginDescriptors::kEnv2StandaloneWidgets);
    addWidgets(PluginDescriptors::kEnv3StandaloneWidgets);
    addWidgets(PluginDescriptors::kLfo1StandaloneWidgets);
    addWidgets(PluginDescriptors::kLfo2StandaloneWidgets);
    addWidgets(PluginDescriptors::kBankUtilityWidgets);
    addWidgets(PluginDescriptors::kInternalPatchesWidgets);
    addWidgets(PluginDescriptors::kComputerPatchesWidgets);
    addWidgets(PluginDescriptors::kPatchMutatorWidgets);
    addWidgets(PluginDescriptors::kMasterEditStandaloneWidgets);

    return allWidgets;
}

const char* ApvtsFactory::getBusId(int busNumber)
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
    
    jassert(busNumber >= 0 && busNumber < PluginIDs::MatrixModulationSection::kModulationBusCount);
    return busIds[busNumber];
}

void ApvtsFactory::addModuleParameters(
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
        ApvtsFactory::addIntParameter(*moduleGroup, paramDesc);
    }
    
    for (const auto& paramDesc : choiceParams)
    {
        ApvtsFactory::addChoiceParameter(*moduleGroup, paramDesc);
    }
    
    parentGroup.addChild(std::move(moduleGroup));
}

void ApvtsFactory::addPatchEditParameters(juce::AudioProcessorParameterGroup& patchEditGroup)
{
    addModuleParameters(
        patchEditGroup,
        PluginIDs::PatchEditSection::Dco1Module::kGroupId,
        PluginDescriptors::getGroupDisplayName(PluginIDs::PatchEditSection::Dco1Module::kGroupId).toRawUTF8(),
        PluginDescriptors::kDco1IntParameters,
        PluginDescriptors::kDco1ChoiceParameters
    );
    
    addModuleParameters(
        patchEditGroup,
        PluginIDs::PatchEditSection::Dco2Module::kGroupId,
        PluginDescriptors::getGroupDisplayName(PluginIDs::PatchEditSection::Dco2Module::kGroupId).toRawUTF8(),
        PluginDescriptors::kDco2IntParameters,
        PluginDescriptors::kDco2ChoiceParameters
    );
    
    addModuleParameters(
        patchEditGroup,
        PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
        PluginDescriptors::getGroupDisplayName(PluginIDs::PatchEditSection::VcfVcaModule::kGroupId).toRawUTF8(),
        PluginDescriptors::kVcfVcaIntParameters,
        PluginDescriptors::kVcfVcaChoiceParameters
    );
    
    addModuleParameters(
        patchEditGroup,
        PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
        PluginDescriptors::getGroupDisplayName(PluginIDs::PatchEditSection::FmTrackModule::kGroupId).toRawUTF8(),
        PluginDescriptors::kFmTrackIntParameters,
        PluginDescriptors::kFmTrackChoiceParameters
    );
    
    addModuleParameters(
        patchEditGroup,
        PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
        PluginDescriptors::getGroupDisplayName(PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId).toRawUTF8(),
        PluginDescriptors::kRampPortamentoIntParameters,
        PluginDescriptors::kRampPortamentoChoiceParameters
    );
    
    addModuleParameters(
        patchEditGroup,
        PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
        PluginDescriptors::getGroupDisplayName(PluginIDs::PatchEditSection::Envelope1Module::kGroupId).toRawUTF8(),
        PluginDescriptors::kEnv1IntParameters,
        PluginDescriptors::kEnv1ChoiceParameters
    );
    
    addModuleParameters(
        patchEditGroup,
        PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
        PluginDescriptors::getGroupDisplayName(PluginIDs::PatchEditSection::Envelope2Module::kGroupId).toRawUTF8(),
        PluginDescriptors::kEnv2IntParameters,
        PluginDescriptors::kEnv2ChoiceParameters
    );
    
    addModuleParameters(
        patchEditGroup,
        PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
        PluginDescriptors::getGroupDisplayName(PluginIDs::PatchEditSection::Envelope3Module::kGroupId).toRawUTF8(),
        PluginDescriptors::kEnv3IntParameters,
        PluginDescriptors::kEnv3ChoiceParameters
    );
    
    addModuleParameters(
        patchEditGroup,
        PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
        PluginDescriptors::getGroupDisplayName(PluginIDs::PatchEditSection::Lfo1Module::kGroupId).toRawUTF8(),
        PluginDescriptors::kLfo1IntParameters,
        PluginDescriptors::kLfo1ChoiceParameters
    );
    
    addModuleParameters(
        patchEditGroup,
        PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
        PluginDescriptors::getGroupDisplayName(PluginIDs::PatchEditSection::Lfo2Module::kGroupId).toRawUTF8(),
        PluginDescriptors::kLfo2IntParameters,
        PluginDescriptors::kLfo2ChoiceParameters
    );
}

void ApvtsFactory::addMasterEditParameters(juce::AudioProcessorParameterGroup& masterEditGroup)
{
    std::vector<PluginDescriptors::IntParameterDescriptor> midiIntParams;
    std::vector<PluginDescriptors::IntParameterDescriptor> vibratoIntParams;
    std::vector<PluginDescriptors::IntParameterDescriptor> miscIntParams;
    
    std::vector<PluginDescriptors::ChoiceParameterDescriptor> midiChoiceParams;
    std::vector<PluginDescriptors::ChoiceParameterDescriptor> vibratoChoiceParams;
    std::vector<PluginDescriptors::ChoiceParameterDescriptor> miscChoiceParams;
    
    for (const auto& param : PluginDescriptors::kMasterEditIntParameters)
    {
        if (param.parentGroupId == PluginIDs::MasterEditSection::MidiModule::kGroupId)
            midiIntParams.push_back(param);
        else if (param.parentGroupId == PluginIDs::MasterEditSection::VibratoModule::kGroupId)
            vibratoIntParams.push_back(param);
        else if (param.parentGroupId == PluginIDs::MasterEditSection::MiscModule::kGroupId)
            miscIntParams.push_back(param);
    }
    
    for (const auto& param : PluginDescriptors::kMasterEditChoiceParameters)
    {
        if (param.parentGroupId == PluginIDs::MasterEditSection::MidiModule::kGroupId)
            midiChoiceParams.push_back(param);
        else if (param.parentGroupId == PluginIDs::MasterEditSection::VibratoModule::kGroupId)
            vibratoChoiceParams.push_back(param);
        else if (param.parentGroupId == PluginIDs::MasterEditSection::MiscModule::kGroupId)
            miscChoiceParams.push_back(param);
    }
    
    addModuleParameters(
        masterEditGroup,
        PluginIDs::MasterEditSection::MidiModule::kGroupId,
        PluginDescriptors::getGroupDisplayName(PluginIDs::MasterEditSection::MidiModule::kGroupId).toRawUTF8(),
        midiIntParams,
        midiChoiceParams
    );
    
    addModuleParameters(
        masterEditGroup,
        PluginIDs::MasterEditSection::VibratoModule::kGroupId,
        PluginDescriptors::getGroupDisplayName(PluginIDs::MasterEditSection::VibratoModule::kGroupId).toRawUTF8(),
        vibratoIntParams,
        vibratoChoiceParams
    );
    
    addModuleParameters(
        masterEditGroup,
        PluginIDs::MasterEditSection::MiscModule::kGroupId,
        PluginDescriptors::getGroupDisplayName(PluginIDs::MasterEditSection::MiscModule::kGroupId).toRawUTF8(),
        miscIntParams,
        miscChoiceParams
    );
}

void ApvtsFactory::addMatrixModulationParameters(juce::AudioProcessorParameterGroup& matrixModulationGroup)
{
    for (int bus = 0; bus < PluginIDs::MatrixModulationSection::kModulationBusCount; ++bus)
    {
        const juce::String busId = getBusId(bus);
        const juce::String busDisplayName = PluginDescriptors::getGroupDisplayName(busId);
        
        auto busGroup = std::make_unique<juce::AudioProcessorParameterGroup>(
            busId, busDisplayName, kSubgroupSeparator
        );
        
        for (const auto& paramDesc : PluginDescriptors::kModulationBusIntParameters[static_cast<size_t>(bus)])
        {
            ApvtsFactory::addIntParameter(*busGroup, paramDesc);
        }
        
        for (const auto& paramDesc : PluginDescriptors::kModulationBusChoiceParameters[static_cast<size_t>(bus)])
        {
            ApvtsFactory::addChoiceParameter(*busGroup, paramDesc);
        }
        
        matrixModulationGroup.addChild(std::move(busGroup));
    }
}

void ApvtsFactory::addMatrixModulationBusIntParameters(
    std::vector<PluginDescriptors::IntParameterDescriptor>& allParams)
{
    for (int bus = 0; bus < PluginIDs::MatrixModulationSection::kModulationBusCount; ++bus)
    {
        allParams.insert(allParams.end(),
                        PluginDescriptors::kModulationBusIntParameters[static_cast<size_t>(bus)].begin(),
                        PluginDescriptors::kModulationBusIntParameters[static_cast<size_t>(bus)].end());
    }
}

void ApvtsFactory::addMatrixModulationBusChoiceParameters(
    std::vector<PluginDescriptors::ChoiceParameterDescriptor>& allParams)
{
    for (int bus = 0; bus < PluginIDs::MatrixModulationSection::kModulationBusCount; ++bus)
    {
        allParams.insert(allParams.end(),
                        PluginDescriptors::kModulationBusChoiceParameters[static_cast<size_t>(bus)].begin(),
                        PluginDescriptors::kModulationBusChoiceParameters[static_cast<size_t>(bus)].end());
    }
}

void ApvtsFactory::addValidationErrorsToResult(
    const juce::StringArray& errors,
    ValidationResult& result)
{
    if (!errors.isEmpty())
    {
        result.isValid = false;
        result.errors.addArray(errors);
    }
}

// ============================================================================
// Validation Implementation
// ============================================================================

ApvtsFactory::ValidationResult ApvtsFactory::validatePluginDescriptors()
{
    ValidationResult result;
    
    addValidationErrorsToResult(validateGroups(), result);
    addValidationErrorsToResult(validateParameters(), result);
    addValidationErrorsToResult(validateStandaloneWidgets(), result);
    addValidationErrorsToResult(checkForDuplicateIds(), result);
    addValidationErrorsToResult(checkForOrphanedReferences(), result);
    addValidationErrorsToResult(checkForCircularReferences(), result);
    
    return result;
}

juce::StringArray ApvtsFactory::validateGroups()
{
    juce::StringArray errors;
    const auto& allGroups = PluginDescriptors::kAllApvtsGroups;
    std::set<juce::String> seenIds;
    
    for (const auto& group : allGroups)
    {
        checkGroupIdUniqueness(group, seenIds, errors);
        checkGroupParentExists(group, allGroups, errors);
        checkGroupDisplayNameNotEmpty(group, errors);
    }
    
    return errors;
}

juce::StringArray ApvtsFactory::validateParameters()
{
    juce::StringArray errors;
    auto allIntParams = getAllIntParameters();
    auto allChoiceParams = getAllChoiceParameters();
    std::set<juce::String> seenParamIds;
    
    for (const auto& param : allIntParams)
    {
        checkParameterIdUniqueness(param.parameterId, seenParamIds, errors);
        checkIntParameterParentGroupExists(param, errors);
        checkIntParameterDisplayNameNotEmpty(param, errors);
        checkIntParameterValueRange(param, errors);
    }
    
    for (const auto& param : allChoiceParams)
    {
        checkParameterIdUniqueness(param.parameterId, seenParamIds, errors);
        checkChoiceParameterParentGroupExists(param, errors);
        checkChoiceParameterDisplayNameNotEmpty(param, errors);
        checkChoiceParameterChoicesNotEmpty(param, errors);
        checkChoiceParameterDefaultIndexValid(param, errors);
    }
    
    return errors;
}

juce::StringArray ApvtsFactory::validateStandaloneWidgets()
{
    juce::StringArray errors;
    auto allWidgets = getAllStandaloneWidgets();
    std::set<juce::String> seenWidgetIds;
    
    for (const auto& widget : allWidgets)
    {
        checkWidgetIdUniqueness(widget, seenWidgetIds, errors);
        checkWidgetParentGroupExists(widget, errors);
    }
    
    return errors;
}

juce::StringArray ApvtsFactory::checkForDuplicateIds()
{
    juce::StringArray errors;
    auto groupIds = collectAllGroupIds();
    auto paramIds = collectAllParameterIds();
    auto widgetIds = collectAllWidgetIds();
    std::set<juce::String> allIds;
    
    checkGroupIdCollisions(groupIds, allIds, errors);
    checkParameterIdCollisions(paramIds, allIds, errors);
    checkWidgetIdCollisions(widgetIds, allIds, errors);
    
    return errors;
}

juce::StringArray ApvtsFactory::checkForOrphanedReferences()
{
    juce::StringArray errors;
    const auto& allGroups = PluginDescriptors::kAllApvtsGroups;
    std::set<juce::String> definedGroupIds = buildDefinedGroupIdsSet(allGroups);
    
    checkParentIdReferencesPointToDefinedGroups(allGroups, definedGroupIds, errors);
    
    return errors;
}

juce::StringArray ApvtsFactory::checkForCircularReferences()
{
    juce::StringArray errors;
    const auto& allGroups = PluginDescriptors::kAllApvtsGroups;
    std::map<juce::String, juce::String> parentMap = buildGroupParentMap(allGroups);
    
    detectCyclesInGroupHierarchy(allGroups, parentMap, errors);
    
    return errors;
}

void ApvtsFactory::checkWidgetIdUniqueness(
    const PluginDescriptors::StandaloneWidgetDescriptor& widget,
    std::set<juce::String>& seenWidgetIds,
    juce::StringArray& errors)
{
    if (seenWidgetIds.find(widget.widgetId) != seenWidgetIds.end())
    {
        errors.add("Duplicate widget ID: " + widget.widgetId);
    }
    seenWidgetIds.insert(widget.widgetId);
}

void ApvtsFactory::checkWidgetParentGroupExists(
    const PluginDescriptors::StandaloneWidgetDescriptor& widget,
    juce::StringArray& errors)
{
    if (!groupExists(widget.parentGroupId))
    {
        errors.add("Standalone widget '" + widget.widgetId + "' references non-existent group: " + widget.parentGroupId);
    }
}

void ApvtsFactory::checkGroupIdUniqueness(
    const PluginDescriptors::ApvtsGroupDescriptor& group,
    std::set<juce::String>& seenIds,
    juce::StringArray& errors)
{
    if (seenIds.find(group.groupId) != seenIds.end())
    {
        errors.add("Duplicate group ID: " + group.groupId);
    }
    seenIds.insert(group.groupId);
}

void ApvtsFactory::checkGroupParentExists(
    const PluginDescriptors::ApvtsGroupDescriptor& group,
    const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups,
    juce::StringArray& errors)
{
    if (group.parentId.isEmpty())
        return;
    
    for (const auto& otherGroup : allGroups)
    {
        if (otherGroup.groupId == group.parentId)
            return;
    }
    
    errors.add("Group '" + group.groupId + "' references non-existent parent: " + group.parentId);
}

void ApvtsFactory::checkGroupDisplayNameNotEmpty(
    const PluginDescriptors::ApvtsGroupDescriptor& group,
    juce::StringArray& errors)
{
    if (group.displayName.isEmpty())
    {
        errors.add("Group '" + group.groupId + "' has empty display name");
    }
}

void ApvtsFactory::checkParameterIdUniqueness(
    const juce::String& parameterId,
    std::set<juce::String>& seenParamIds,
    juce::StringArray& errors)
{
    if (seenParamIds.find(parameterId) != seenParamIds.end())
    {
        errors.add("Duplicate parameter ID: " + parameterId);
    }
    seenParamIds.insert(parameterId);
}

void ApvtsFactory::checkIntParameterParentGroupExists(
    const PluginDescriptors::IntParameterDescriptor& param,
    juce::StringArray& errors)
{
    if (!groupExists(param.parentGroupId))
    {
        errors.add("Int parameter '" + param.parameterId + "' references non-existent group: " + param.parentGroupId);
    }
}

void ApvtsFactory::checkIntParameterDisplayNameNotEmpty(
    const PluginDescriptors::IntParameterDescriptor& param,
    juce::StringArray& errors)
{
    if (param.displayName.isEmpty())
    {
        errors.add("Int parameter '" + param.parameterId + "' has empty display name");
    }
}

void ApvtsFactory::checkIntParameterValueRange(
    const PluginDescriptors::IntParameterDescriptor& param,
    juce::StringArray& errors)
{
    if (param.minValue > param.maxValue)
    {
        errors.add("Int parameter '" + param.parameterId + "' has minValue > maxValue");
    }
    
    if (param.defaultValue < param.minValue || param.defaultValue > param.maxValue)
    {
        errors.add("Int parameter '" + param.parameterId + "' has defaultValue outside [minValue, maxValue]");
    }
}

void ApvtsFactory::checkChoiceParameterParentGroupExists(
    const PluginDescriptors::ChoiceParameterDescriptor& param,
    juce::StringArray& errors)
{
    if (!groupExists(param.parentGroupId))
    {
        errors.add("Choice parameter '" + param.parameterId + "' references non-existent group: " + param.parentGroupId);
    }
}

void ApvtsFactory::checkChoiceParameterDisplayNameNotEmpty(
    const PluginDescriptors::ChoiceParameterDescriptor& param,
    juce::StringArray& errors)
{
    if (param.displayName.isEmpty())
    {
        errors.add("Choice parameter '" + param.parameterId + "' has empty display name");
    }
}

void ApvtsFactory::checkChoiceParameterChoicesNotEmpty(
    const PluginDescriptors::ChoiceParameterDescriptor& param,
    juce::StringArray& errors)
{
    if (param.choices.isEmpty())
    {
        errors.add("Choice parameter '" + param.parameterId + "' has empty choices list");
    }
}

void ApvtsFactory::checkChoiceParameterDefaultIndexValid(
    const PluginDescriptors::ChoiceParameterDescriptor& param,
    juce::StringArray& errors)
{
    if (param.defaultIndex < 0 || param.defaultIndex >= param.choices.size())
    {
        errors.add("Choice parameter '" + param.parameterId + "' has invalid defaultIndex: " + 
                  juce::String(param.defaultIndex) + " (choices size: " + juce::String(param.choices.size()) + ")");
    }
}

void ApvtsFactory::checkGroupIdCollisions(
    const juce::StringArray& groupIds,
    std::set<juce::String>& allIds,
    juce::StringArray& errors)
{
    for (const auto& id : groupIds)
    {
        if (allIds.find(id) != allIds.end())
        {
            errors.add("ID collision: '" + id + "' is used as both group ID and another type");
        }
        allIds.insert(id);
    }
}

void ApvtsFactory::checkParameterIdCollisions(
    const juce::StringArray& paramIds,
    std::set<juce::String>& allIds,
    juce::StringArray& errors)
{
    for (const auto& id : paramIds)
    {
        if (allIds.find(id) != allIds.end())
        {
            errors.add("ID collision: '" + id + "' is used as both parameter ID and another type");
        }
        allIds.insert(id);
    }
}

void ApvtsFactory::checkWidgetIdCollisions(
    const juce::StringArray& widgetIds,
    std::set<juce::String>& allIds,
    juce::StringArray& errors)
{
    for (const auto& id : widgetIds)
    {
        if (allIds.find(id) != allIds.end())
        {
            errors.add("ID collision: '" + id + "' is used as both widget ID and another type");
        }
        allIds.insert(id);
    }
}

std::set<juce::String> ApvtsFactory::buildDefinedGroupIdsSet(
    const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups)
{
    std::set<juce::String> definedGroupIds;
    for (const auto& group : allGroups)
    {
        definedGroupIds.insert(group.groupId);
    }
    return definedGroupIds;
}

void ApvtsFactory::checkParentIdReferencesPointToDefinedGroups(
    const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups,
    const std::set<juce::String>& definedGroupIds,
    juce::StringArray& errors)
{
    for (const auto& group : allGroups)
    {
        if (!group.parentId.isEmpty() && definedGroupIds.find(group.parentId) == definedGroupIds.end())
        {
            errors.add("Orphaned group reference: '" + group.groupId + "' references undefined parent: " + group.parentId);
        }
    }
}

std::map<juce::String, juce::String> ApvtsFactory::buildGroupParentMap(
    const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups)
{
    std::map<juce::String, juce::String> parentMap;
    for (const auto& group : allGroups)
    {
        if (!group.parentId.isEmpty())
        {
            parentMap[group.groupId] = group.parentId;
        }
    }
    return parentMap;
}

void ApvtsFactory::detectCyclesInGroupHierarchy(
    const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups,
    const std::map<juce::String, juce::String>& parentMap,
    juce::StringArray& errors)
{
    std::set<juce::String> visited;
    std::set<juce::String> recursionStack;
    
    std::function<bool(const juce::String&)> hasCycle = [&](const juce::String& groupId) -> bool
    {
        if (recursionStack.find(groupId) != recursionStack.end())
            return true;
        
        if (visited.find(groupId) != visited.end())
            return false;
        
        visited.insert(groupId);
        recursionStack.insert(groupId);
        
        auto it = parentMap.find(groupId);
        if (it != parentMap.end())
        {
            if (hasCycle(it->second))
                return true;
        }
        
        recursionStack.erase(groupId);
        return false;
    };
    
    for (const auto& group : allGroups)
    {
        if (visited.find(group.groupId) == visited.end())
        {
            if (hasCycle(group.groupId))
            {
                errors.add("Circular reference detected in group hierarchy involving: " + group.groupId);
            }
        }
    }
}

// Helper methods for validation
juce::StringArray ApvtsFactory::collectAllGroupIds()
{
    juce::StringArray ids;
    for (const auto& group : PluginDescriptors::kAllApvtsGroups)
    {
        ids.add(group.groupId);
    }
    return ids;
}

juce::StringArray ApvtsFactory::collectAllParameterIds()
{
    juce::StringArray ids;
    
    auto allIntParams = getAllIntParameters();
    auto allChoiceParams = getAllChoiceParameters();
    
    for (const auto& param : allIntParams)
    {
        ids.add(param.parameterId);
    }
    
    for (const auto& param : allChoiceParams)
    {
        ids.add(param.parameterId);
    }
    
    return ids;
}

juce::StringArray ApvtsFactory::collectAllWidgetIds()
{
    juce::StringArray ids;
    
    auto allWidgets = getAllStandaloneWidgets();
    
    for (const auto& widget : allWidgets)
    {
        ids.add(widget.widgetId);
    }
    
    return ids;
}

bool ApvtsFactory::groupExists(const juce::String& groupId)
{
    for (const auto& group : PluginDescriptors::kAllApvtsGroups)
    {
        if (group.groupId == groupId)
        {
            return true;
        }
    }
    return false;
}

bool ApvtsFactory::parameterIdExists(const juce::String& parameterId)
{
    auto allIntParams = getAllIntParameters();
    auto allChoiceParams = getAllChoiceParameters();
    
    for (const auto& param : allIntParams)
    {
        if (param.parameterId == parameterId)
        {
            return true;
        }
    }
    
    for (const auto& param : allChoiceParams)
    {
        if (param.parameterId == parameterId)
        {
            return true;
        }
    }
    
    return false;
}

bool ApvtsFactory::widgetIdExists(const juce::String& widgetId)
{
    auto allWidgets = getAllStandaloneWidgets();
    
    for (const auto& widget : allWidgets)
    {
        if (widget.widgetId == widgetId)
        {
            return true;
        }
    }
    
    return false;
}

