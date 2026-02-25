#pragma once

// PluginDescriptors.h
// Central file for plugin descriptor structures and declarations
// Contains ONLY data structures and declarations - no code/logic
// Single source of truth for plugin structure description

#include <array>
#include <vector>

#include <juce_core/juce_core.h>

#include "PluginIDs.h"
#include "PluginDisplayNames.h"


namespace PluginDescriptors
{

// ============================================================================
// Constants
// ============================================================================

// Used for root groups (= Plugin Master & Patch Modes) that have no parent
constexpr const char* kNoParentId = "";

// Used for modulation bus parameters that don't have a SysEx ID
constexpr int kNoSysExId = -1;

// Matrix-1000 Hardware Limits
namespace Matrix1000Limits
{
    constexpr int kMinBankNumber = 0;
    constexpr int kMaxBankNumber = 9;
    constexpr int kMinPatchNumber = 0;
    constexpr int kMaxPatchNumber = 99;
}

// ============================================================================
// APVTS Group Descriptors
// ============================================================================

struct ApvtsGroupDescriptor
{
    juce::String parentId;     // Empty string for root groups (= Plugin Modes)
    juce::String groupId;
    juce::String displayName;
};

// ============================================================================
// Parameter Descriptors
// ============================================================================

enum class ParameterType
{
    kInt,   // AudioParameterInt (attached to Slider)
    kChoice // AudioParameterChoice (attached to ComboBox)
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

// ============================================================================
// Standalone Widget Descriptors
// ============================================================================

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
};

// ============================================================================
// Data Declarations (definitions in PluginDescriptors.cpp)
// ============================================================================

// All APVTS Groups
extern const std::vector<ApvtsGroupDescriptor> kAllApvtsGroups;

// Master Edit | Parameter & Standalone Widgets
extern const std::vector<IntParameterDescriptor> kMasterEditIntParameters;
extern const std::vector<ChoiceParameterDescriptor> kMasterEditChoiceParameters;
extern const std::vector<StandaloneWidgetDescriptor> kMasterEditStandaloneWidgets;

// Patch Edit | DCO 1 Parameter Widgets
extern const std::vector<IntParameterDescriptor> kDco1IntParameters;
extern const std::vector<ChoiceParameterDescriptor> kDco1ChoiceParameters;

// Patch Edit | DCO 2 Parameter Widgets
extern const std::vector<IntParameterDescriptor> kDco2IntParameters;
extern const std::vector<ChoiceParameterDescriptor> kDco2ChoiceParameters;

// Patch Edit | VCF/VCA Parameter Widgets
extern const std::vector<IntParameterDescriptor> kVcfVcaIntParameters;
extern const std::vector<ChoiceParameterDescriptor> kVcfVcaChoiceParameters;

// Patch Edit | FM/TRACK Parameter Widgets
extern const std::vector<IntParameterDescriptor> kFmTrackIntParameters;
extern const std::vector<ChoiceParameterDescriptor> kFmTrackChoiceParameters;

// Patch Edit | RAMP/PORTAMENTO Parameter Widgets
extern const std::vector<IntParameterDescriptor> kRampPortamentoIntParameters;
extern const std::vector<ChoiceParameterDescriptor> kRampPortamentoChoiceParameters;

// Patch Edit | ENV 1 Parameter Widgets
extern const std::vector<IntParameterDescriptor> kEnv1IntParameters;
extern const std::vector<ChoiceParameterDescriptor> kEnv1ChoiceParameters;

// Patch Edit | ENV 2 Parameter Widgets
extern const std::vector<IntParameterDescriptor> kEnv2IntParameters;
extern const std::vector<ChoiceParameterDescriptor> kEnv2ChoiceParameters;

// Patch Edit | ENV 3 Parameter Widgets
extern const std::vector<IntParameterDescriptor> kEnv3IntParameters;
extern const std::vector<ChoiceParameterDescriptor> kEnv3ChoiceParameters;

// Patch Edit | LFO 1 Parameter Widgets
extern const std::vector<IntParameterDescriptor> kLfo1IntParameters;
extern const std::vector<ChoiceParameterDescriptor> kLfo1ChoiceParameters;

// Patch Edit | LFO 2 Parameter Widgets
extern const std::vector<IntParameterDescriptor> kLfo2IntParameters;
extern const std::vector<ChoiceParameterDescriptor> kLfo2ChoiceParameters;

// Patch Edit | Standalone Widgets
extern const std::vector<StandaloneWidgetDescriptor> kDco1StandaloneWidgets;
extern const std::vector<StandaloneWidgetDescriptor> kDco2StandaloneWidgets;
extern const std::vector<StandaloneWidgetDescriptor> kVcfVcaStandaloneWidgets;
extern const std::vector<StandaloneWidgetDescriptor> kFmTrackStandaloneWidgets;
extern const std::vector<StandaloneWidgetDescriptor> kRampPortamentoStandaloneWidgets;
extern const std::vector<StandaloneWidgetDescriptor> kEnv1StandaloneWidgets;
extern const std::vector<StandaloneWidgetDescriptor> kEnv2StandaloneWidgets;
extern const std::vector<StandaloneWidgetDescriptor> kEnv3StandaloneWidgets;
extern const std::vector<StandaloneWidgetDescriptor> kLfo1StandaloneWidgets;
extern const std::vector<StandaloneWidgetDescriptor> kLfo2StandaloneWidgets;

// Matrix Modulation | Parameter Widgets
extern const std::array<std::vector<IntParameterDescriptor>, PluginIDs::MatrixModulationSection::kModulationBusCount> kModulationBusIntParameters;
extern const std::array<std::vector<ChoiceParameterDescriptor>, PluginIDs::MatrixModulationSection::kModulationBusCount> kModulationBusChoiceParameters;

// Patch Manager | Standalone Widgets
extern const std::vector<StandaloneWidgetDescriptor> kBankUtilityWidgets;
extern const std::vector<StandaloneWidgetDescriptor> kInternalPatchesWidgets;
extern const std::vector<StandaloneWidgetDescriptor> kComputerPatchesWidgets;
extern const std::vector<StandaloneWidgetDescriptor> kPatchMutatorWidgets;

// ============================================================================
// Helper Functions
// ============================================================================

juce::String getGroupDisplayName(const juce::String& groupId);
juce::String getSectionDisplayName(const char* sectionId);

} // namespace PluginDescriptors
