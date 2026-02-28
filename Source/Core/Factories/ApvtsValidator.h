#pragma once

#include <map>
#include <set>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/Definitions/PluginDescriptors.h"

class ApvtsValidator
{
public:
    struct ValidationResult
    {
        bool isValid = true;
        juce::StringArray errors;
        juce::StringArray warnings;
    };

    static ValidationResult validatePluginDescriptors();

    static juce::StringArray validateGroups();
    static juce::StringArray validateParameters();
    static juce::StringArray validateStandaloneWidgets();
    static juce::StringArray checkForDuplicateIds();
    static juce::StringArray checkForOrphanedReferences();
    static juce::StringArray checkForCircularReferences();

private:
    static void addValidationErrorsToResult(
        const juce::StringArray& errors,
        ValidationResult& result);

    static juce::StringArray collectAllGroupIds();
    static juce::StringArray collectAllParameterIds();
    static juce::StringArray collectAllWidgetIds();
    static bool groupExists(const juce::String& groupId);
    static bool parameterIdExists(const juce::String& parameterId);
    static bool widgetIdExists(const juce::String& widgetId);

    static void checkWidgetIdUniqueness(
        const PluginDescriptors::StandaloneWidgetDescriptor& widget,
        std::set<juce::String>& seenWidgetIds,
        juce::StringArray& errors);

    static void checkWidgetParentGroupExists(
        const PluginDescriptors::StandaloneWidgetDescriptor& widget,
        juce::StringArray& errors);

    static void checkGroupIdUniqueness(
        const PluginDescriptors::ApvtsGroupDescriptor& group,
        std::set<juce::String>& seenIds,
        juce::StringArray& errors);

    static void checkGroupParentExists(
        const PluginDescriptors::ApvtsGroupDescriptor& group,
        const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups,
        juce::StringArray& errors);

    static void checkGroupDisplayNameNotEmpty(
        const PluginDescriptors::ApvtsGroupDescriptor& group,
        juce::StringArray& errors);

    static void checkParameterIdUniqueness(
        const juce::String& parameterId,
        std::set<juce::String>& seenParamIds,
        juce::StringArray& errors);

    static void checkIntParameterParentGroupExists(
        const PluginDescriptors::IntParameterDescriptor& param,
        juce::StringArray& errors);

    static void checkIntParameterDisplayNameNotEmpty(
        const PluginDescriptors::IntParameterDescriptor& param,
        juce::StringArray& errors);

    static void checkIntParameterValueRange(
        const PluginDescriptors::IntParameterDescriptor& param,
        juce::StringArray& errors);

    static void checkChoiceParameterParentGroupExists(
        const PluginDescriptors::ChoiceParameterDescriptor& param,
        juce::StringArray& errors);

    static void checkChoiceParameterDisplayNameNotEmpty(
        const PluginDescriptors::ChoiceParameterDescriptor& param,
        juce::StringArray& errors);

    static void checkChoiceParameterChoicesNotEmpty(
        const PluginDescriptors::ChoiceParameterDescriptor& param,
        juce::StringArray& errors);

    static void checkChoiceParameterDefaultIndexValid(
        const PluginDescriptors::ChoiceParameterDescriptor& param,
        juce::StringArray& errors);

    static void checkGroupIdCollisions(
        const juce::StringArray& groupIds,
        std::set<juce::String>& allIds,
        juce::StringArray& errors);

    static void checkParameterIdCollisions(
        const juce::StringArray& paramIds,
        std::set<juce::String>& allIds,
        juce::StringArray& errors);

    static void checkWidgetIdCollisions(
        const juce::StringArray& widgetIds,
        std::set<juce::String>& allIds,
        juce::StringArray& errors);

    static std::set<juce::String> buildDefinedGroupIdsSet(
        const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups);

    static void checkParentIdReferencesPointToDefinedGroups(
        const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups,
        const std::set<juce::String>& definedGroupIds,
        juce::StringArray& errors);

    static std::map<juce::String, juce::String> buildGroupParentMap(
        const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups);

    static void detectCyclesInGroupHierarchy(
        const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups,
        const std::map<juce::String, juce::String>& parentMap,
        juce::StringArray& errors);
};
