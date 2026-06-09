// Patch Edit Section descriptors: standalone widgets, int parameters,
// and choice parameters for the DCO, VCF/VCA, FM/Track, Ramp, Envelope, and LFO modules.
// Consumers (e.g. ApvtsFactory, WidgetFactory) use these definitions to build the APVTS and widget maps.

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
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::OnOff::kOn }),
            .defaultIndex = 0,
            .sysExOffset = 24,
            .sysExId = 19
        }
    };


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
            .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::KeyboardPortamento::kOff, PluginDisplayNames::ChoiceLists::KeyboardPortamento::kPortamento, PluginDisplayNames::ChoiceLists::KeyboardPortamento::kKeyboard }),
            .defaultIndex = 2,
            .sysExOffset = 29,
            .sysExId = 26
        }
    };


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
                PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kRotate, PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kReasgn,
                PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kUnison, PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kRearob }),
            .defaultIndex = 1,
            .sysExOffset = 8,
            .sysExId = 48
        }
    };


    // =================================================================================================================
    // Patch Edit Section | Envelope 1 Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchEditSection::Envelope1Module::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> PatchEditSection::Envelope1Module::kIntParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDelay,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kDelay,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 50,
            .sysExId = 50
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 51,
            .sysExId = 51
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDecay,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kDecay,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 10,
            .sysExOffset = 52,
            .sysExId = 52
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kSustain,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kSustain,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 50,
            .sysExOffset = 53,
            .sysExId = 53
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kRelease,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kRelease,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 10,
            .sysExOffset = 54,
            .sysExId = 54
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitude,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitude,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 40,
            .sysExOffset = 55,
            .sysExId = 55
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitudeModByVelocity,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitudeModByVelocity,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 63,
            .sysExOffset = 94,
            .sysExId = 56
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> PatchEditSection::Envelope1Module::kChoiceParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kTriggerMode,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kTriggerMode,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::TriggerMode::kStrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kMtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kSreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kMreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXmtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXmrst }),
            .defaultIndex = 0,
            .sysExOffset = 49,
            .sysExId = 57
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kEnvelopeMode,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kEnvelopeMode,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kNormal,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kDadr,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kFree,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kBoth }),
            .defaultIndex = 0,
            .sysExOffset = 57,
            .sysExId = 58
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kLfo1Trigger,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kLfo1Trigger,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kNormal,
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kLfo1,
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kGLfo1 }),
            .defaultIndex = 0,
            .sysExOffset = 56,
            .sysExId = 59
        }
    };


    // =================================================================================================================
    // Patch Edit Section | Envelope 2 Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchEditSection::Envelope2Module::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> PatchEditSection::Envelope2Module::kIntParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDelay,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kDelay,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 59,
            .sysExId = 60
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAttack,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kAttack,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 60,
            .sysExId = 61
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDecay,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kDecay,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 10,
            .sysExOffset = 61,
            .sysExId = 62
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kSustain,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kSustain,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 50,
            .sysExOffset = 62,
            .sysExId = 63
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kRelease,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kRelease,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 10,
            .sysExOffset = 63,
            .sysExId = 64
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitude,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitude,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 40,
            .sysExOffset = 64,
            .sysExId = 65
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitudeModByVelocity,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitudeModByVelocity,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 63,
            .sysExOffset = 95,
            .sysExId = 66
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> PatchEditSection::Envelope2Module::kChoiceParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kTriggerMode,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kTriggerMode,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::TriggerMode::kStrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kMtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kSreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kMreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXmtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXmrst }),
            .defaultIndex = 0,
            .sysExOffset = 58,
            .sysExId = 67
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kEnvelopeMode,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kEnvelopeMode,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kNormal,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kDadr,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kFree,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kBoth }),
            .defaultIndex = 0,
            .sysExOffset = 66,
            .sysExId = 68
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kLfo1Trigger,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kLfo1Trigger,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kNormal,
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kLfo1,
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kGLfo1 }),
            .defaultIndex = 0,
            .sysExOffset = 65,
            .sysExId = 69
        }
    };


    // =================================================================================================================
    // Patch Edit Section | Envelope 3 Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchEditSection::Envelope3Module::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> PatchEditSection::Envelope3Module::kIntParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDelay,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kDelay,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 68,
            .sysExId = 70
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAttack,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kAttack,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 69,
            .sysExId = 71
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDecay,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kDecay,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 20,
            .sysExOffset = 70,
            .sysExId = 72
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kSustain,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kSustain,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 71,
            .sysExId = 73
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kRelease,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kRelease,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 20,
            .sysExOffset = 72,
            .sysExId = 74
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitude,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitude,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 40,
            .sysExOffset = 73,
            .sysExId = 75
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitudeModByVelocity,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitudeModByVelocity,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 63,
            .sysExOffset = 96,
            .sysExId = 76
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> PatchEditSection::Envelope3Module::kChoiceParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kTriggerMode,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kTriggerMode,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::TriggerMode::kStrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kMtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kSreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kMreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXmtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXmrst }),
            .defaultIndex = 0,
            .sysExOffset = 67,
            .sysExId = 77
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kEnvelopeMode,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kEnvelopeMode,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kNormal,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kDadr,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kFree,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kBoth }),
            .defaultIndex = 0,
            .sysExOffset = 75,
            .sysExId = 78
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kLfo1Trigger,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kLfo1Trigger,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kNormal,
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kLfo1,
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kGatedLfo1Trigger }),
            .defaultIndex = 0,
            .sysExOffset = 74,
            .sysExId = 79
        }
    };


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
