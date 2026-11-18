// Patch Edit Section descriptors — DCO 1, DCO 2, and VCF/VCA modules.
// Sibling translation units cover FM/Track, Ramp, Envelopes, and LFOs.

#include "PluginDescriptors.h"

#include "PluginDisplayNames.h"
#include "PluginHelpers.h"
#include "PluginIDs.h"


namespace PluginDescriptors
{
    // =================================================================================================================
    // Patch Edit Section | DCO 1 Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchEditSection::Dco1Module::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> PatchEditSection::Dco1Module::kIntParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco1Module::kFrequency,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 9,
            .sysExId = 0
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequencyModByLfo1,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kFrequencyModByLfo1,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco1Module::kFrequencyModByLfo1,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 86,
            .sysExId = 1
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kPulseWidth,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kPulseWidth,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco1Module::kPulseWidth,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 31,
            .sysExOffset = 11,
            .sysExId = 3
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kPulseWidthModByLfo2,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kPulseWidthModByLfo2,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco1Module::kPulseWidthModByLfo2,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 87,
            .sysExId = 4
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kWaveShape,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kWaveShape,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco1Module::kWaveShape,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 31,
            .sysExOffset = 10,
            .sysExId = 5
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> PatchEditSection::Dco1Module::kChoiceParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kSync,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kSync,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco1Module::kSync,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::Sync::kOff,
                PluginDisplayNames::ChoiceLists::Sync::kSoft,
                PluginDisplayNames::ChoiceLists::Sync::kMedium,
                PluginDisplayNames::ChoiceLists::Sync::kHard }),
            .defaultIndex = 0,
            .sysExOffset = 25,
            .sysExId = 2
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kWaveSelect,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kWaveSelect,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco1Module::kWaveSelect,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::WaveSelect::kOff,
                PluginDisplayNames::ChoiceLists::WaveSelect::kPulse,
                PluginDisplayNames::ChoiceLists::WaveSelect::kWave,
                PluginDisplayNames::ChoiceLists::WaveSelect::kBoth }),
            .defaultIndex = 2,
            .sysExOffset = 13,
            .sysExId = 6
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kLevers,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kLevers,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco1Module::kLevers,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::Levers::kOff,
                PluginDisplayNames::ChoiceLists::Levers::kLever1Bend,
                PluginDisplayNames::ChoiceLists::Levers::kLever2Vibrato,
                PluginDisplayNames::ChoiceLists::Levers::kBoth }),
            .defaultIndex = 3,
            .sysExOffset = 12,
            .sysExId = 7
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kKeyboardPortamento,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kKeyboardPortamento,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco1Module::kKeyboardPortamento,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::KeyboardPortamento::kKeyboard, PluginDisplayNames::ChoiceLists::KeyboardPortamento::kPortamento }),
            .defaultIndex = 0,
            .sysExOffset = 21,
            .sysExId = 8
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kKeyClick,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kKeyClick,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco1Module::kKeyClick,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::OnOff::kOn }),
            .defaultIndex = 0,
            .sysExOffset = 22,
            .sysExId = 9
        }
    };


    // =================================================================================================================
    // Patch Edit Section | DCO 2 Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchEditSection::Dco2Module::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> PatchEditSection::Dco2Module::kIntParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kFrequency,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kFrequency,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco2Module::kFrequency,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 14,
            .sysExId = 10
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kFrequencyModByLfo1,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kFrequencyModByLfo1,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco2Module::kFrequencyModByLfo1,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 88,
            .sysExId = 11
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kDetune,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kDetune,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco2Module::kDetune,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .minValue = -31,
            .maxValue = 31,
            .defaultValue = 2,
            .sysExOffset = 19,
            .sysExId = 12
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kPulseWidth,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kPulseWidth,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco2Module::kPulseWidth,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 24,
            .sysExOffset = 16,
            .sysExId = 13
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kPulseWidthModByLfo2,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kPulseWidthModByLfo2,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco2Module::kPulseWidthModByLfo2,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 89,
            .sysExId = 14
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kWaveShape,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kWaveShape,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco2Module::kWaveShape,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 31,
            .sysExOffset = 15,
            .sysExId = 15
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> PatchEditSection::Dco2Module::kChoiceParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kWaveSelect,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kWaveSelect,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco2Module::kWaveSelect,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::WaveSelect::kOff,
                PluginDisplayNames::ChoiceLists::WaveSelect::kPulse,
                PluginDisplayNames::ChoiceLists::WaveSelect::kWave,
                PluginDisplayNames::ChoiceLists::WaveSelect::kBoth,
                PluginDisplayNames::ChoiceLists::WaveSelect::kNoise }),
            .defaultIndex = 1,
            .sysExOffset = 18,
            .sysExId = 16
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kLevers,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kLevers,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco2Module::kLevers,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::Levers::kOff,
                PluginDisplayNames::ChoiceLists::Levers::kLever1Bend,
                PluginDisplayNames::ChoiceLists::Levers::kLever2Vibrato,
                PluginDisplayNames::ChoiceLists::Levers::kBoth }),
            .defaultIndex = 3,
            .sysExOffset = 17,
            .sysExId = 17
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kKeyboardPortamento,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kKeyboardPortamento,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco2Module::kKeyboardPortamento,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::KeyboardPortamento::kOff, PluginDisplayNames::ChoiceLists::KeyboardPortamento::kPortamento, PluginDisplayNames::ChoiceLists::KeyboardPortamento::kKeyboard }),
            .defaultIndex = 2,
            .sysExOffset = 23,
            .sysExId = 18
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kKeyClick,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kKeyClick,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Dco2Module::kKeyClick,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::OnOff::kOn }),
            .defaultIndex = 0,
            .sysExOffset = 24,
            .sysExId = 19
        }
    };
}
