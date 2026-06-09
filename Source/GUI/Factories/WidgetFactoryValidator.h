#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Exceptions/WidgetFactoryExceptions.h"

class WidgetFactoryValidator
{
public:
    explicit WidgetFactoryValidator(juce::AudioProcessorValueTreeState& inApvts);

    void throwIfParameterIdEmpty(const juce::String& inParameterId);
    void throwIfWidgetIdEmpty(const juce::String& inWidgetId);
    
    const PluginDescriptors::IntParameterDescriptor* getIntParameterDescriptorOrThrow(
        const PluginDescriptors::IntParameterDescriptor* inDescriptor,
        const juce::String& inParameterId) const;
    
    const PluginDescriptors::ChoiceParameterDescriptor* getChoiceParameterDescriptorOrThrow(
        const PluginDescriptors::ChoiceParameterDescriptor* inDescriptor,
        const juce::String& inParameterId) const;
    
    const PluginDescriptors::StandaloneWidgetDescriptor* getStandaloneWidgetDescriptorOrThrow(
        const PluginDescriptors::StandaloneWidgetDescriptor* inDescriptor,
        const juce::String& inWidgetId) const;
    
    void validateIntParameterValues(
        const PluginDescriptors::IntParameterDescriptor* inDescriptor,
        const juce::String& inParameterId) const;
    
    void validateChoiceParameterValues(
        const PluginDescriptors::ChoiceParameterDescriptor* inDescriptor,
        const juce::String& inParameterId) const;
    
    void validateWidgetType(
        const PluginDescriptors::StandaloneWidgetDescriptor* inDescriptor,
        const juce::String& inWidgetId) const;

    int resolveStandaloneButtonWidthOrThrow(const juce::String& inWidgetId) const;

private:
    juce::AudioProcessorValueTreeState& apvts;
    
    juce::String getWidgetTypeString(PluginDescriptors::StandaloneWidgetType inWidgetType) const;
};

