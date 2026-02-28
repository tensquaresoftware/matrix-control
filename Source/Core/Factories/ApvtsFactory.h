#pragma once

#include <map>
#include <set>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

#include "ApvtsValidator.h"

class ApvtsFactory
{
public:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // ============================================================================
    // Helper Methods
    // ============================================================================
    
    static std::vector<PluginDescriptors::IntParameterDescriptor> getAllIntParameters();
    static std::vector<PluginDescriptors::ChoiceParameterDescriptor> getAllChoiceParameters();
    static std::vector<PluginDescriptors::StandaloneWidgetDescriptor> getAllStandaloneWidgets();

    // ============================================================================
    // Validation Methods (delegates to ApvtsValidator)
    // ============================================================================

    using ValidationResult = ApvtsValidator::ValidationResult;

    static ValidationResult validatePluginDescriptors();

private:
    static void addMatrixModulationBusIntParameters(
        std::vector<PluginDescriptors::IntParameterDescriptor>& allParams);

    static void addMatrixModulationBusChoiceParameters(
        std::vector<PluginDescriptors::ChoiceParameterDescriptor>& allParams);

};

