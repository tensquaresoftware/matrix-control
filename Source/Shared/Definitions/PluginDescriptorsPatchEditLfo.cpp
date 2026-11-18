// Patch Edit Section descriptors — LFO 1 and LFO 2 modules.

#include "PluginDescriptors.h"

#include "PluginDisplayNames.h"
#include "PluginHelpers.h"
#include "PluginIDs.h"


namespace PluginDescriptors
{
    // =================================================================================================================
    // Patch Edit Section | LFO 1 Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchEditSection::Lfo1Module::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> PatchEditSection::Lfo1Module::kIntParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kSpeed,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::ParameterWidgets::kSpeed,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo1Module::kSpeed,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 40,
            .sysExOffset = 35,
            .sysExId = 80
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kSpeedModByPressure,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::ParameterWidgets::kSpeedModByPressure,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo1Module::kSpeedModByPressure,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 102,
            .sysExId = 81
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kRetriggerPoint,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::ParameterWidgets::kRetriggerPoint,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo1Module::kRetriggerPoint,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 39,
            .sysExId = 83
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kAmplitude,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::ParameterWidgets::kAmplitude,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo1Module::kAmplitude,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 41,
            .sysExId = 84
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kAmplitudeModByRamp1,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::ParameterWidgets::kAmplitudeModByRamp1,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo1Module::kAmplitudeModByRamp1,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 63,
            .sysExOffset = 97,
            .sysExId = 85
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> PatchEditSection::Lfo1Module::kChoiceParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kWaveform,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::ParameterWidgets::kWaveform,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo1Module::kWaveform,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::LfoWaveform::kTriangle,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kUpsaw,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kDnsaw,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kSquare,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kRandom,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kNoise,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kSampled }),
            .defaultIndex = 0,
            .sysExOffset = 38,
            .sysExId = 82
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kTriggerMode,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::ParameterWidgets::kTriggerMode,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo1Module::kTriggerMode,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::LfoTriggerMode::kOff,
                PluginDisplayNames::ChoiceLists::LfoTriggerMode::kStrig,
                PluginDisplayNames::ChoiceLists::LfoTriggerMode::kMtrig,
                PluginDisplayNames::ChoiceLists::LfoTriggerMode::kXtrig }),
            .defaultIndex = 0,
            .sysExOffset = 36,
            .sysExId = 86
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kLag,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::ParameterWidgets::kLag,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo1Module::kLag,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::OnOff::kOn }),
            .defaultIndex = 0,
            .sysExOffset = 37,
            .sysExId = 87
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kSampleInput,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::ParameterWidgets::kSampleInput,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo1Module::kSampleInput,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kNone,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope1,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope2,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope3,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo1,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo2,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVibrato,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp1,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp2,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboard,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPortamento,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kTrack,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboardGate,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVelocity,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kReleaseVelocity,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPressure,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal1,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal2,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever1,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever2,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever3 }),
            .defaultIndex = 9,
            .sysExOffset = 40,
            .sysExId = 88
        }
    };


    // =================================================================================================================
    // Patch Edit Section | LFO 2 Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchEditSection::Lfo2Module::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> PatchEditSection::Lfo2Module::kIntParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kSpeed,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::ParameterWidgets::kSpeed,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo2Module::kSpeed,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 30,
            .sysExOffset = 42,
            .sysExId = 90
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kSpeedModByKeyboard,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::ParameterWidgets::kSpeedModByKeyboard,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo2Module::kSpeedModByKeyboard,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 103,
            .sysExId = 91
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kRetriggerPoint,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::ParameterWidgets::kRetriggerPoint,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo2Module::kRetriggerPoint,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 46,
            .sysExId = 93
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kAmplitude,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::ParameterWidgets::kAmplitude,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo2Module::kAmplitude,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 48,
            .sysExId = 94
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kAmplitudeModByRamp2,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::ParameterWidgets::kAmplitudeModByRamp2,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo2Module::kAmplitudeModByRamp2,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 63,
            .sysExOffset = 98,
            .sysExId = 95
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> PatchEditSection::Lfo2Module::kChoiceParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kWaveform,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::ParameterWidgets::kWaveform,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo2Module::kWaveform,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::LfoWaveform::kTriangle,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kUpsaw,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kDnsaw,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kSquare,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kRandom,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kNoise,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kSampled }),
            .defaultIndex = 0,
            .sysExOffset = 45,
            .sysExId = 92
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kTriggerMode,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::ParameterWidgets::kTriggerMode,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo2Module::kTriggerMode,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::LfoTriggerMode::kOff,
                PluginDisplayNames::ChoiceLists::LfoTriggerMode::kStrig,
                PluginDisplayNames::ChoiceLists::LfoTriggerMode::kMtrig,
                PluginDisplayNames::ChoiceLists::LfoTriggerMode::kXtrig }),
            .defaultIndex = 0,
            .sysExOffset = 43,
            .sysExId = 96
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kLag,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::ParameterWidgets::kLag,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo2Module::kLag,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::OnOff::kOn }),
            .defaultIndex = 0,
            .sysExOffset = 44,
            .sysExId = 97
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kSampleInput,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::ParameterWidgets::kSampleInput,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Lfo2Module::kSampleInput,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kNone,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope1,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope2,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope3,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo1,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo2,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVibrato,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp1,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp2,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboard,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPortamento,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kTrack,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboardGate,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVelocity,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kReleaseVelocity,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPressure,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal1,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal2,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever1,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever2,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever3 }),
            .defaultIndex = 9,
            .sysExOffset = 47,
            .sysExId = 98
        }
    };
}
