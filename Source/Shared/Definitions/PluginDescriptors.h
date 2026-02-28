#pragma once

// PluginDescriptors.h
// Central file for plugin descriptor structures and declarations
// Contains ONLY data structures and declarations - no code/logic
// Single source of truth for plugin structure description

#include <array>
#include <vector>

#include <juce_core/juce_core.h>

#include "Matrix1000Limits.h"
#include "PluginIDs.h"
#include "PluginDisplayNames.h"


namespace PluginDescriptors
{

    // =================================================================================================================
    // Constants
    // =================================================================================================================

    // Used for root groups (Master & Patch modes) that have no parent
    constexpr const char* kNoParentId = "";

    // Used for modulation bus parameters, which don't have a SysEx ID
    constexpr int kNoSysExId = -1;


    // =================================================================================================================
    // APVTS Group Descriptors
    // =================================================================================================================

    struct ApvtsGroupDescriptor
    {
        juce::String parentId;     // Empty string for root groups (Master & Patch modes)
        juce::String groupId;
        juce::String displayName;
    };


    // =================================================================================================================
    // Parameter Descriptors (APVTS + Widget Creation)
    // =================================================================================================================

    enum class ParameterType
    {
        kInt,   // AudioParameterInt (attached to Slider widgets)
        kChoice // AudioParameterChoice (attached to ComboBox widgets)
    };

    struct IntParameterDescriptor
    {
        juce::String parameterId;
        juce::String displayName;
        juce::String parentGroupId;
        int minValue;
        int maxValue;
        int defaultValue;
        int sysExOffset;
        int sysExId;
    };

    struct ChoiceParameterDescriptor
    {
        juce::String parameterId;
        juce::String displayName;
        juce::String parentGroupId;
        juce::StringArray choices;
        int defaultIndex;
        int sysExOffset;
        int sysExId;
    };


    // =================================================================================================================
    // Standalone Widget Descriptors
    // =================================================================================================================

    enum class StandaloneWidgetType
    {
        kButton,
        kComboBox,
        kNumber,
        kLabel
    };

    struct StandaloneWidgetDescriptor
    {
        juce::String widgetId;
        juce::String displayName;
        juce::String parentGroupId;
        StandaloneWidgetType widgetType;
        std::optional<int> buttonWidth;
    };


    // =================================================================================================================
    // Plugin Descriptor Collections
    // =================================================================================================================

    extern const std::vector<ApvtsGroupDescriptor> kAllApvtsGroups;

    namespace MasterEditSection
    {
        namespace MidiModule
        {
            extern const std::vector<IntParameterDescriptor>     kIntParameters;
            extern const std::vector<ChoiceParameterDescriptor>  kChoiceParameters;
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        namespace VibratoModule
        {
            extern const std::vector<IntParameterDescriptor>     kIntParameters;
            extern const std::vector<ChoiceParameterDescriptor>  kChoiceParameters;
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        namespace MiscModule
        {
            extern const std::vector<IntParameterDescriptor>     kIntParameters;
            extern const std::vector<ChoiceParameterDescriptor>  kChoiceParameters;
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        extern const std::vector<IntParameterDescriptor>     kIntParameters;
        extern const std::vector<ChoiceParameterDescriptor>  kChoiceParameters;
        extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
    }

    namespace PatchEditSection
    {
        namespace Dco1Module
        {
            extern const std::vector<IntParameterDescriptor>     kIntParameters;
            extern const std::vector<ChoiceParameterDescriptor>  kChoiceParameters;
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        namespace Dco2Module
        {
            extern const std::vector<IntParameterDescriptor>     kIntParameters;
            extern const std::vector<ChoiceParameterDescriptor>  kChoiceParameters;
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        namespace VcfVcaModule
        {
            extern const std::vector<IntParameterDescriptor>     kIntParameters;
            extern const std::vector<ChoiceParameterDescriptor>  kChoiceParameters;
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        namespace FmTrackModule
        {
            extern const std::vector<IntParameterDescriptor>     kIntParameters;
            extern const std::vector<ChoiceParameterDescriptor>  kChoiceParameters;
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        namespace RampPortamentoModule
        {
            extern const std::vector<IntParameterDescriptor>     kIntParameters;
            extern const std::vector<ChoiceParameterDescriptor>  kChoiceParameters;
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        namespace Envelope1Module
        {
            extern const std::vector<IntParameterDescriptor>     kIntParameters;
            extern const std::vector<ChoiceParameterDescriptor>  kChoiceParameters;
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        namespace Envelope2Module
        {
            extern const std::vector<IntParameterDescriptor>     kIntParameters;
            extern const std::vector<ChoiceParameterDescriptor>  kChoiceParameters;
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        namespace Envelope3Module
        {
            extern const std::vector<IntParameterDescriptor>     kIntParameters;
            extern const std::vector<ChoiceParameterDescriptor>  kChoiceParameters;
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        namespace Lfo1Module
        {
            extern const std::vector<IntParameterDescriptor>     kIntParameters;
            extern const std::vector<ChoiceParameterDescriptor>  kChoiceParameters;
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        namespace Lfo2Module
        {
            extern const std::vector<IntParameterDescriptor>     kIntParameters;
            extern const std::vector<ChoiceParameterDescriptor>  kChoiceParameters;
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }
    }

    namespace MatrixModulationSection
    {
        // Header standalone widgets
        extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;

        // Buses standalone widgets (indexed by bus index 0..kModulationBusCount-1)
        extern const std::array<std::vector<StandaloneWidgetDescriptor>, ::Matrix1000Limits::kModulationBusCount> kModulationBusStandaloneWidgets;
        extern const std::array<std::vector<IntParameterDescriptor>, ::Matrix1000Limits::kModulationBusCount>    kModulationBusIntParameters;
        extern const std::array<std::vector<ChoiceParameterDescriptor>, ::Matrix1000Limits::kModulationBusCount>  kModulationBusChoiceParameters;
    }

    namespace PatchManagerSection
    {
        namespace BankUtilityModule
        {
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        namespace InternalPatchesModule
        {
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        namespace ComputerPatchesModule
        {
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }

        namespace PatchMutatorModule
        {
            extern const std::vector<StandaloneWidgetDescriptor> kStandaloneWidgets;
        }
    }
}
