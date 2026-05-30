#pragma once

#include <map>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/Definitions/PluginDescriptors.h"

class ApvtsLayoutBuilder
{
public:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    static constexpr const char* kSubgroupSeparator = "|";

    static const char* getBusId(int busNumber);

    static void createRootGroups(
        const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups,
        std::map<juce::String, std::unique_ptr<juce::AudioProcessorParameterGroup>>& groupMap);

    static void createChildGroups(
        const std::vector<PluginDescriptors::ApvtsGroupDescriptor>& allGroups,
        std::map<juce::String, std::unique_ptr<juce::AudioProcessorParameterGroup>>& groupMap);

    static void addParametersToChildGroup(
        const juce::String& childGroupId,
        juce::AudioProcessorParameterGroup& childGroup);

    static void addRootGroupsToLayout(
        std::map<juce::String, std::unique_ptr<juce::AudioProcessorParameterGroup>>& groupMap,
        juce::AudioProcessorValueTreeState::ParameterLayout& parameterLayout);

    static void addIntParameter(
        juce::AudioProcessorParameterGroup& group,
        const PluginDescriptors::IntParameterDescriptor& desc);

    static void addChoiceParameter(
        juce::AudioProcessorParameterGroup& group,
        const PluginDescriptors::ChoiceParameterDescriptor& desc);

    static void addModuleParameters(
        juce::AudioProcessorParameterGroup& parentGroup,
        const char* moduleId,
        const char* moduleDisplayName,
        const std::vector<PluginDescriptors::IntParameterDescriptor>& intParams,
        const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& choiceParams);

    static void addPatchEditParameters(juce::AudioProcessorParameterGroup& patchEditGroup);
    static void addPatchEditTopModulesParameters(juce::AudioProcessorParameterGroup& patchEditGroup);
    static void addPatchEditEnvelopeAndLfoModulesParameters(juce::AudioProcessorParameterGroup& patchEditGroup);
    static void addPatchEditModuleParameters(
        juce::AudioProcessorParameterGroup& patchEditGroup,
        const char* moduleGroupId,
        const std::vector<PluginDescriptors::IntParameterDescriptor>& intParams,
        const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& choiceParams);

    static void addMasterEditParameters(juce::AudioProcessorParameterGroup& masterEditGroup);
    static void addMasterEditModuleParameters(
        juce::AudioProcessorParameterGroup& masterEditGroup,
        const char* moduleGroupId);

    static std::vector<PluginDescriptors::IntParameterDescriptor> filterIntParametersByParentGroup(
        const std::vector<PluginDescriptors::IntParameterDescriptor>& allParams,
        const char* parentGroupId);

    static std::vector<PluginDescriptors::ChoiceParameterDescriptor> filterChoiceParametersByParentGroup(
        const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& allParams,
        const char* parentGroupId);

    static void addMatrixModulationParameters(juce::AudioProcessorParameterGroup& matrixModulationGroup);
};
