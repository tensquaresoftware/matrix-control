// Patch Edit Section descriptors — FM/Track and Ramp/Portamento modules.

#include "PluginDescriptors.h"

#include "PluginDisplayNames.h"
#include "PluginHelpers.h"
#include "PluginIDs.h"


namespace PluginDescriptors
{
    // =================================================================================================================
    // Patch Edit Section | FM/Track Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchEditSection::FmTrackModule::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchEditSection::FmTrackModule::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::PatchEditSection::FmTrackModule::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> PatchEditSection::FmTrackModule::kIntParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmAmount,
            .displayName = PluginDisplayNames::PatchEditSection::FmTrackModule::ParameterWidgets::kFmAmount,
            .parentGroupId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 30,
            .sysExId = 30
        },
        {
            .parameterId = PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmModByEnv3,
            .displayName = PluginDisplayNames::PatchEditSection::FmTrackModule::ParameterWidgets::kFmModByEnv3,
            .parentGroupId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 100,
            .sysExId = 31
        },
        {
            .parameterId = PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmModByPressure,
            .displayName = PluginDisplayNames::PatchEditSection::FmTrackModule::ParameterWidgets::kFmModByPressure,
            .parentGroupId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 101,
            .sysExId = 32
        },
        {
            .parameterId = PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint1,
            .displayName = PluginDisplayNames::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint1,
            .parentGroupId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 77,
            .sysExId = 34
        },
        {
            .parameterId = PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint2,
            .displayName = PluginDisplayNames::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint2,
            .parentGroupId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 15,
            .sysExOffset = 78,
            .sysExId = 35
        },
        {
            .parameterId = PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3,
            .displayName = PluginDisplayNames::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3,
            .parentGroupId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 31,
            .sysExOffset = 79,
            .sysExId = 36
        },
        {
            .parameterId = PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint4,
            .displayName = PluginDisplayNames::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint4,
            .parentGroupId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 47,
            .sysExOffset = 80,
            .sysExId = 37
        },
        {
            .parameterId = PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint5,
            .displayName = PluginDisplayNames::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint5,
            .parentGroupId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 63,
            .sysExOffset = 81,
            .sysExId = 38
        }
    };


    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> PatchEditSection::FmTrackModule::kChoiceParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackInput,
            .displayName = PluginDisplayNames::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackInput,
            .parentGroupId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kNone,     PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope3,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVibrato, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp2,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboard, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPortamento, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kTrack, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboardGate,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVelocity, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kReleaseVelocity, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPressure,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever3 }),
            .defaultIndex = 9,
            .sysExOffset = 76,
            .sysExId = 33
        }
    };


    // =================================================================================================================
    // Patch Edit Section | Ramp/Portamento Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchEditSection::RampPortamentoModule::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchEditSection::RampPortamentoModule::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> PatchEditSection::RampPortamentoModule::kIntParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Rate,
            .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Rate,
            .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 82,
            .sysExId = 40
        },
        {
            .parameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Rate,
            .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Rate,
            .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 84,
            .sysExId = 42
        },
        {
            .parameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoRate,
            .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoRate,
            .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 32,
            .sysExId = 44
        },
        {
            .parameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoModByVelocity,
            .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoModByVelocity,
            .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 99,
            .sysExId = 45
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> PatchEditSection::RampPortamentoModule::kChoiceParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Trigger,
            .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Trigger,
            .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::RampTrigger::kStrig,
                PluginDisplayNames::ChoiceLists::RampTrigger::kMtrig,
                PluginDisplayNames::ChoiceLists::RampTrigger::kExtrig,
                PluginDisplayNames::ChoiceLists::RampTrigger::kGatedx }),
            .defaultIndex = 0,
            .sysExOffset = 83,
            .sysExId = 41
        },
        {
            .parameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Trigger,
            .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Trigger,
            .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::RampTrigger::kStrig,
                PluginDisplayNames::ChoiceLists::RampTrigger::kMtrig,
                PluginDisplayNames::ChoiceLists::RampTrigger::kExtrig,
                PluginDisplayNames::ChoiceLists::RampTrigger::kGatedx }),
            .defaultIndex = 0,
            .sysExOffset = 85,
            .sysExId = 43
        },
        {
            .parameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoMode,
            .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoMode,
            .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::PortamentoMode::kLinear,
                PluginDisplayNames::ChoiceLists::PortamentoMode::kConst,
                PluginDisplayNames::ChoiceLists::PortamentoMode::kExpo }),
            .defaultIndex = 0,
            .sysExOffset = 33,
            .sysExId = 46
        },
        {
            .parameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoLegato,
            .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoLegato,
            .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::OnOff::kOn }),
            .defaultIndex = 0,
            .sysExOffset = 34,
            .sysExId = 47
        },
        {
            .parameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode,
            .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode,
            .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kReasgn,
                PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kRotate,
                PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kUnison,
                PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kRearob }),
            .defaultIndex = 1,
            .sysExOffset = 8,
            .sysExId = 48
        }
    };
}
