#include "ApvtsValidator.h"

#include <functional>

#include "ApvtsFactory.h"

void ApvtsValidator::addValidationErrorsToResult(
    const juce::StringArray& errors,
    ValidationResult& result)
{
    if (!errors.isEmpty())
    {
        result.isValid = false;
        result.errors.addArray(errors);
    }
}

ApvtsValidator::ValidationResult ApvtsValidator::validatePluginDescriptors()
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

juce::StringArray ApvtsValidator::validateGroups()
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

juce::StringArray ApvtsValidator::validateParameters()
{
    juce::StringArray errors;
    auto allIntParams = ApvtsFactory::getAllIntParameters();
    auto allChoiceParams = ApvtsFactory::getAllChoiceParameters();
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

juce::StringArray ApvtsValidator::validateStandaloneWidgets()
{
    juce::StringArray errors;
    auto allWidgets = ApvtsFactory::getAllStandaloneWidgets();
    std::set<juce::String> seenWidgetIds;

    for (const auto& widget : allWidgets)
    {
        checkWidgetIdUniqueness(widget, seenWidgetIds, errors);
        checkWidgetParentGroupExists(widget, errors);
    }

    return errors;
}

juce::StringArray ApvtsValidator::checkForDuplicateIds()
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

juce::StringArray ApvtsValidator::checkForOrphanedReferences()
{
    juce::StringArray errors;
    const auto& allGroups = PluginDescriptors::kAllApvtsGroups;
    std::set<juce::String> definedGroupIds = buildDefinedGroupIdsSet(allGroups);

    checkParentIdReferencesPointToDefinedGroups(allGroups, definedGroupIds, errors);

    return errors;
}

juce::StringArray ApvtsValidator::checkForCircularReferences()
{
    juce::StringArray errors;
    const auto& allGroups = PluginDescriptors::kAllApvtsGroups;
    std::map<juce::String, juce::String> parentMap = buildGroupParentMap(allGroups);

    detectCyclesInGroupHierarchy(allGroups, parentMap, errors);

    return errors;
}

void ApvtsValidator::checkWidgetIdUniqueness(
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

void ApvtsValidator::checkWidgetParentGroupExists(
    const PluginDescriptors::StandaloneWidgetDescriptor& widget,
    juce::StringArray& errors)
{
    if (!groupExists(widget.parentGroupId))
    {
        errors.add("Standalone widget '" + widget.widgetId + "' references non-existent group: " + widget.parentGroupId);
    }
}

void ApvtsValidator::checkGroupIdUniqueness(
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

void ApvtsValidator::checkGroupParentExists(
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

void ApvtsValidator::checkGroupDisplayNameNotEmpty(
    const PluginDescriptors::ApvtsGroupDescriptor& group,
    juce::StringArray& errors)
{
    if (group.displayName.isEmpty())
    {
        errors.add("Group '" + group.groupId + "' has empty display name");
    }
}

void ApvtsValidator::checkParameterIdUniqueness(
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

void ApvtsValidator::checkIntParameterParentGroupExists(
    const PluginDescriptors::IntParameterDescriptor& param,
    juce::StringArray& errors)
{
    if (!groupExists(param.parentGroupId))
    {
        errors.add("Int parameter '" + param.parameterId + "' references non-existent group: " + param.parentGroupId);
    }
}

void ApvtsValidator::checkIntParameterDisplayNameNotEmpty(
    const PluginDescriptors::IntParameterDescriptor& param,
    juce::StringArray& errors)
{
    if (param.displayName.isEmpty())
    {
        errors.add("Int parameter '" + param.parameterId + "' has empty display name");
    }
}

void ApvtsValidator::checkIntParameterValueRange(
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

void ApvtsValidator::checkChoiceParameterParentGroupExists(
    const PluginDescriptors::ChoiceParameterDescriptor& param,
    juce::StringArray& errors)
{
    if (!groupExists(param.parentGroupId))
    {
        errors.add("Choice parameter '" + param.parameterId + "' references non-existent group: " + param.parentGroupId);
    }
}

void ApvtsValidator::checkChoiceParameterDisplayNameNotEmpty(
    const PluginDescriptors::ChoiceParameterDescriptor& param,
    juce::StringArray& errors)
{
    if (param.displayName.isEmpty())
    {
        errors.add("Choice parameter '" + param.parameterId + "' has empty display name");
    }
}

void ApvtsValidator::checkChoiceParameterChoicesNotEmpty(
    const PluginDescriptors::ChoiceParameterDescriptor& param,
    juce::StringArray& errors)
{
    if (param.choices.isEmpty())
    {
        errors.add("Choice parameter '" + param.parameterId + "' has empty choices list");
    }
}

void ApvtsValidator::checkChoiceParameterDefaultIndexValid(
    const PluginDescriptors::ChoiceParameterDescriptor& param,
    juce::StringArray& errors)
{
    if (param.defaultIndex < 0 || param.defaultIndex >= param.choices.size())
    {
        errors.add("Choice parameter '" + param.parameterId + "' has invalid defaultIndex: " +
                  juce::String(param.defaultIndex) + " (choices size: " + juce::String(param.choices.size()) + ")");
    }
}

void ApvtsValidator::checkGroupIdCollisions(
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

void ApvtsValidator::checkParameterIdCollisions(
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

void ApvtsValidator::checkWidgetIdCollisions(
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

std::set<juce::String> ApvtsValidator::buildDefinedGroupIdsSet(
    const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups)
{
    std::set<juce::String> definedGroupIds;
    for (const auto& group : allGroups)
    {
        definedGroupIds.insert(group.groupId);
    }
    return definedGroupIds;
}

void ApvtsValidator::checkParentIdReferencesPointToDefinedGroups(
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

std::map<juce::String, juce::String> ApvtsValidator::buildGroupParentMap(
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

void ApvtsValidator::detectCyclesInGroupHierarchy(
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

juce::StringArray ApvtsValidator::collectAllGroupIds()
{
    juce::StringArray ids;
    for (const auto& group : PluginDescriptors::kAllApvtsGroups)
    {
        ids.add(group.groupId);
    }
    return ids;
}

juce::StringArray ApvtsValidator::collectAllParameterIds()
{
    juce::StringArray ids;

    auto allIntParams = ApvtsFactory::getAllIntParameters();
    auto allChoiceParams = ApvtsFactory::getAllChoiceParameters();

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

juce::StringArray ApvtsValidator::collectAllWidgetIds()
{
    juce::StringArray ids;

    auto allWidgets = ApvtsFactory::getAllStandaloneWidgets();

    for (const auto& widget : allWidgets)
    {
        ids.add(widget.widgetId);
    }

    return ids;
}

bool ApvtsValidator::groupExists(const juce::String& groupId)
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

bool ApvtsValidator::parameterIdExists(const juce::String& parameterId)
{
    auto allIntParams = ApvtsFactory::getAllIntParameters();
    auto allChoiceParams = ApvtsFactory::getAllChoiceParameters();

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

bool ApvtsValidator::widgetIdExists(const juce::String& widgetId)
{
    auto allWidgets = ApvtsFactory::getAllStandaloneWidgets();

    for (const auto& widget : allWidgets)
    {
        if (widget.widgetId == widgetId)
        {
            return true;
        }
    }

    return false;
}
