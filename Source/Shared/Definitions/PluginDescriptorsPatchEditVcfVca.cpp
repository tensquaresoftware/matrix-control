// This file defines the Patch Edit VCF/VCA module descriptor data.
// Extracted from PluginDescriptorsPatchEdit.cpp for modular maintenance.

#include "PluginDescriptors.h"

#include "PluginDisplayNames.h"
#include "PluginHelpers.h"
#include "PluginIDs.h"

namespace PluginDescriptors
{
    // =================================================================================================================
    // Patch Edit Section | VCF/VCA Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchEditSection::VcfVcaModule::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchEditSection::VcfVcaModule::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> PatchEditSection::VcfVcaModule::kIntParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kBalance,
            .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::ParameterWidgets::kBalance,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::VcfVcaModule::kBalance,
            .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 31,
            .sysExOffset = 20,
            .sysExId = 20
        },
        {
            .parameterId = PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequency,
            .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequency,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::VcfVcaModule::kFrequency,
            .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
            .minValue = 0,
            .maxValue = 127,
            .defaultValue = 55,
            .sysExOffset = 26,
            .sysExId = 21
        },
        {
            .parameterId = PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequencyModByEnv1,
            .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequencyModByEnv1,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::VcfVcaModule::kFrequencyModByEnv1,
            .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 42,
            .sysExOffset = 90,
            .sysExId = 22
        },
        {
            .parameterId = PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequencyModByPressure,
            .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequencyModByPressure,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::VcfVcaModule::kFrequencyModByPressure,
            .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 91,
            .sysExId = 23
        },
        {
            .parameterId = PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kResonance,
            .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::ParameterWidgets::kResonance,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::VcfVcaModule::kResonance,
            .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 27,
            .sysExId = 24
        },
        {
            .parameterId = PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca1Volume,
            .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca1Volume,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::VcfVcaModule::kVca1Volume,
            .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 63,
            .sysExOffset = 31,
            .sysExId = 27
        },
        {
            .parameterId = PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca1ModByVelocity,
            .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca1ModByVelocity,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::VcfVcaModule::kVca1ModByVelocity,
            .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 92,
            .sysExId = 28
        },
        {
            .parameterId = PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca2ModByEnv2,
            .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca2ModByEnv2,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::VcfVcaModule::kVca2ModByEnv2,
            .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 63,
            .sysExOffset = 93,
            .sysExId = 29
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> PatchEditSection::VcfVcaModule::kChoiceParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kLevers,
            .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::ParameterWidgets::kLevers,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::VcfVcaModule::kLevers,
            .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::Levers::kOff,
                PluginDisplayNames::ChoiceLists::Levers::kLever1Bend,
                PluginDisplayNames::ChoiceLists::Levers::kLever2Vibrato,
                PluginDisplayNames::ChoiceLists::Levers::kBoth }),
            .defaultIndex = 0,
            .sysExOffset = 28,
            .sysExId = 25
        },
        {
            .parameterId = PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kKeyboardPortamento,
            .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::ParameterWidgets::kKeyboardPortamento,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::VcfVcaModule::kKeyboardPortamento,
            .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::KeyboardPortamento::kOff, PluginDisplayNames::ChoiceLists::KeyboardPortamento::kPortamento, PluginDisplayNames::ChoiceLists::KeyboardPortamento::kKeyboard }),
            .defaultIndex = 2,
            .sysExOffset = 29,
            .sysExId = 26
        }
    };
}
