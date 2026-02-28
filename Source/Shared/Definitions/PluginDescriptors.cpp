// This file defines all static descriptor data used by the plugin: APVTS group hierarchy,
// and per-section/per-module lists of standalone widgets, int parameters and choice parameters.
// Consumers (e.g. ApvtsFactory, WidgetFactory) use these definitions to build the APVTS and widget maps.

#include "PluginDescriptors.h"

#include <array>

#include "PluginDimensions.h"
#include "PluginDisplayNames.h"
#include "PluginHelpers.h"
#include "PluginIDs.h"


namespace PluginDescriptors
{
    // =================================================================================================================
    // APVTS Groups
    // =================================================================================================================

    const std::vector<ApvtsGroupDescriptor> kAllApvtsGroups =
    {
        // Plugin Modes ------------------------------------------------------------------------------------------------
        {
            .parentId = kNoParentId,
            .groupId = PluginIDs::Mode::kMaster,
            .displayName = PluginDisplayNames::Mode::kMaster
        },
        {
            .parentId = kNoParentId,
            .groupId = PluginIDs::Mode::kPatch,
            .displayName = PluginDisplayNames::Mode::kPatch
        },
        // Plugin Sections ---------------------------------------------------------------------------------------------
        {
            .parentId = PluginIDs::Mode::kMaster,
            .groupId = PluginIDs::MasterEditSection::kGroupId,
            .displayName = PluginDisplayNames::MasterEditSection::kName
        },
        {
            .parentId = PluginIDs::Mode::kPatch,
            .groupId = PluginIDs::PatchEditSection::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::kName
        },
        {
            .parentId = PluginIDs::Mode::kPatch,
            .groupId = PluginIDs::MatrixModulationSection::kGroupId,
            .displayName = PluginDisplayNames::MatrixModulationSection::kName
        },
        {
            .parentId = PluginIDs::Mode::kPatch,
            .groupId = PluginIDs::PatchManagerSection::kGroupId,
            .displayName = PluginDisplayNames::PatchManagerSection::kName
        },
        // Master Edit Section -----------------------------------------------------------------------------------------
        {
            .parentId = PluginIDs::MasterEditSection::kGroupId,
            .groupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
            .displayName = PluginDisplayNames::MasterEditSection::MidiModule::kName
        },
        {
            .parentId = PluginIDs::MasterEditSection::kGroupId,
            .groupId = PluginIDs::MasterEditSection::VibratoModule::kGroupId,
            .displayName = PluginDisplayNames::MasterEditSection::VibratoModule::kName
        },
        {
            .parentId = PluginIDs::MasterEditSection::kGroupId,
            .groupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
            .displayName = PluginDisplayNames::MasterEditSection::MiscModule::kName
        },
        // Patch Edit Section ------------------------------------------------------------------------------------------
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::FmTrackModule::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::kName
        },
        // Matrix Modulation Section -----------------------------------------------------------------------------------
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus0
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus1
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus2
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus3
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus4
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus5
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus6
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus7
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus8
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus9,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus9
        },
        // Patch Manager Section ---------------------------------------------------------------------------------------
        {
            .parentId = PluginIDs::PatchManagerSection::kGroupId,
            .groupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::kName
        },
        {
            .parentId = PluginIDs::PatchManagerSection::kGroupId,
            .groupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::kName
        },
        {
            .parentId = PluginIDs::PatchManagerSection::kGroupId,
            .groupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
            .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::kName
        },
        {
            .parentId = PluginIDs::PatchManagerSection::kGroupId,
            .groupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::kName
        }
    };


    // =================================================================================================================
    // Master Edit Section | MIDI Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> MasterEditSection::MidiModule::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::MasterEditSection::MidiModule::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::MasterEditSection::MidiModule::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> MasterEditSection::MidiModule::kIntParameters = {
        {
            .parameterId = PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPedal1Select,
            .displayName = PluginDisplayNames::MasterEditSection::MidiModule::ParameterWidgets::kPedal1Select,
            .parentGroupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
            .minValue = 0,
            .maxValue = 121,
            .defaultValue = 4,
            .sysExOffset = 17,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPedal2Select,
            .displayName = PluginDisplayNames::MasterEditSection::MidiModule::ParameterWidgets::kPedal2Select,
            .parentGroupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
            .minValue = 0,
            .maxValue = 121,
            .defaultValue = 64,
            .sysExOffset = 18,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kLever2Select,
            .displayName = PluginDisplayNames::MasterEditSection::MidiModule::ParameterWidgets::kLever2Select,
            .parentGroupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
            .minValue = 0,
            .maxValue = 121,
            .defaultValue = 1,
            .sysExOffset = 19,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kLever3Select,
            .displayName = PluginDisplayNames::MasterEditSection::MidiModule::ParameterWidgets::kLever3Select,
            .parentGroupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
            .minValue = 0,
            .maxValue = 121,
            .defaultValue = 2,
            .sysExOffset = 20,
            .sysExId = kNoSysExId
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> MasterEditSection::MidiModule::kChoiceParameters = {
        {
            .parameterId = PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel,
            .displayName = PluginDisplayNames::MasterEditSection::MidiModule::ParameterWidgets::kChannel,
            .parentGroupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::MidiChannel::kOmni,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel1,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel2,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel3,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel4,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel5,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel6,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel7,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel8,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel9,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel10,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel11,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel12,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel13,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel14,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel15,
                PluginDisplayNames::ChoiceLists::MidiChannel::kChannel16,
                PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG1,
                PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG2,
                PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG3,
                PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG4,
                PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG5,
                PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG6,
                PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG7,
                PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG8,
                PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG9 }),
            .defaultIndex = 0,
            .sysExOffset = 11, // Combination of offsets 11, 12, 35
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kMidiEcho,
            .displayName = PluginDisplayNames::MasterEditSection::MidiModule::ParameterWidgets::kMidiEcho,
            .parentGroupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::OnOff::kOn }),
            .defaultIndex = 0,
            .sysExOffset = 32,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kControllers,
            .displayName = PluginDisplayNames::MasterEditSection::MidiModule::ParameterWidgets::kControllers,
            .parentGroupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::OnOff::kOn }),
            .defaultIndex = 1,
            .sysExOffset = 13,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPatchChanges,
            .displayName = PluginDisplayNames::MasterEditSection::MidiModule::ParameterWidgets::kPatchChanges,
            .parentGroupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::OnOff::kOn }),
            .defaultIndex = 1,
            .sysExOffset = 14,
            .sysExId = kNoSysExId
        }
    };


    // =================================================================================================================
    // Master Edit Section | Vibrato Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> MasterEditSection::VibratoModule::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::MasterEditSection::VibratoModule::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::MasterEditSection::VibratoModule::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::MasterEditSection::VibratoModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> MasterEditSection::VibratoModule::kIntParameters = {
        {
            .parameterId = PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeed,
            .displayName = PluginDisplayNames::MasterEditSection::VibratoModule::ParameterWidgets::kSpeed,
            .parentGroupId = PluginIDs::MasterEditSection::VibratoModule::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 1,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModAmount,
            .displayName = PluginDisplayNames::MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModAmount,
            .parentGroupId = PluginIDs::MasterEditSection::VibratoModule::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 3,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmplitude,
            .displayName = PluginDisplayNames::MasterEditSection::VibratoModule::ParameterWidgets::kAmplitude,
            .parentGroupId = PluginIDs::MasterEditSection::VibratoModule::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 5,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmpModAmount,
            .displayName = PluginDisplayNames::MasterEditSection::VibratoModule::ParameterWidgets::kAmpModAmount,
            .parentGroupId = PluginIDs::MasterEditSection::VibratoModule::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 7,
            .sysExId = kNoSysExId
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> MasterEditSection::VibratoModule::kChoiceParameters = {
        {
            .parameterId = PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModSource,
            .displayName = PluginDisplayNames::MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModSource,
            .parentGroupId = PluginIDs::MasterEditSection::VibratoModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal1 }),
            .defaultIndex = 0,
            .sysExOffset = 2,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kWaveform,
            .displayName = PluginDisplayNames::MasterEditSection::VibratoModule::ParameterWidgets::kWaveform,
            .parentGroupId = PluginIDs::MasterEditSection::VibratoModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::LfoWaveform::kTriangle,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kUpsaw,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kDnsaw,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kSquare,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kRandom,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kNoise,
                PluginDisplayNames::ChoiceLists::LfoWaveform::kSampled }),
            .defaultIndex = 0,
            .sysExOffset = 4,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmpModSource,
            .displayName = PluginDisplayNames::MasterEditSection::VibratoModule::ParameterWidgets::kAmpModSource,
            .parentGroupId = PluginIDs::MasterEditSection::VibratoModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal1 }),
            .defaultIndex = 0,
            .sysExOffset = 6,
            .sysExId = kNoSysExId
        }
    };


    // =================================================================================================================
    // Master Edit Section | Misc Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> MasterEditSection::MiscModule::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::MasterEditSection::MiscModule::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::MasterEditSection::MiscModule::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> MasterEditSection::MiscModule::kIntParameters = {
        {
            .parameterId = PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMasterTune,
            .displayName = PluginDisplayNames::MasterEditSection::MiscModule::ParameterWidgets::kMasterTune,
            .parentGroupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
            .minValue = -31,
            .maxValue = 31,
            .defaultValue = 0,
            .sysExOffset = 8,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMasterTranspose,
            .displayName = PluginDisplayNames::MasterEditSection::MiscModule::ParameterWidgets::kMasterTranspose,
            .parentGroupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
            .minValue = -24,
            .maxValue = 24,
            .defaultValue = 0,
            .sysExOffset = 34,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kBendRange,
            .displayName = PluginDisplayNames::MasterEditSection::MiscModule::ParameterWidgets::kBendRange,
            .parentGroupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
            .minValue = 0,
            .maxValue = 24,
            .defaultValue = 2,
            .sysExOffset = 164,
            .sysExId = kNoSysExId
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> MasterEditSection::MiscModule::kChoiceParameters = {
        {
            .parameterId = PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable,
            .displayName = PluginDisplayNames::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable,
            .parentGroupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::OnOff::kOn }),
            .defaultIndex = 0,
            .sysExOffset = 169,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kVolumeInvertEnable,
            .displayName = PluginDisplayNames::MasterEditSection::MiscModule::ParameterWidgets::kVolumeInvertEnable,
            .parentGroupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::OnOff::kOn }),
            .defaultIndex = 0,
            .sysExOffset = 170,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kBankLockEnable,
            .displayName = PluginDisplayNames::MasterEditSection::MiscModule::ParameterWidgets::kBankLockEnable,
            .parentGroupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::OnOff::kOn }),
            .defaultIndex = 0,
            .sysExOffset = 165,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMemoryProtectEnable,
            .displayName = PluginDisplayNames::MasterEditSection::MiscModule::ParameterWidgets::kMemoryProtectEnable,
            .parentGroupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::OnOff::kOff,
                PluginDisplayNames::ChoiceLists::OnOff::kOn }),
            .defaultIndex = 0,
            .sysExOffset = 171,
            .sysExId = kNoSysExId
        }
    };


    // =================================================================================================================
    // Master Edit Section | Combined Vectors (consumers use these)
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> MasterEditSection::kStandaloneWidgets = []()
    {
        std::vector<StandaloneWidgetDescriptor> v;
        v.insert(v.end(), MasterEditSection::MidiModule::kStandaloneWidgets.begin(), MasterEditSection::MidiModule::kStandaloneWidgets.end());
        v.insert(v.end(), MasterEditSection::VibratoModule::kStandaloneWidgets.begin(), MasterEditSection::VibratoModule::kStandaloneWidgets.end());
        v.insert(v.end(), MasterEditSection::MiscModule::kStandaloneWidgets.begin(), MasterEditSection::MiscModule::kStandaloneWidgets.end());
        return v;
    }();

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> MasterEditSection::kIntParameters = []()
    {
        std::vector<IntParameterDescriptor> v;
        v.insert(v.end(), MasterEditSection::MidiModule::kIntParameters.begin(), MasterEditSection::MidiModule::kIntParameters.end());
        v.insert(v.end(), MasterEditSection::VibratoModule::kIntParameters.begin(), MasterEditSection::VibratoModule::kIntParameters.end());
        v.insert(v.end(), MasterEditSection::MiscModule::kIntParameters.begin(), MasterEditSection::MiscModule::kIntParameters.end());
        return v;
    }();

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> MasterEditSection::kChoiceParameters = []()
    {
        std::vector<ChoiceParameterDescriptor> v;
        v.insert(v.end(), MasterEditSection::MidiModule::kChoiceParameters.begin(), MasterEditSection::MidiModule::kChoiceParameters.end());
        v.insert(v.end(), MasterEditSection::VibratoModule::kChoiceParameters.begin(), MasterEditSection::VibratoModule::kChoiceParameters.end());
        v.insert(v.end(), MasterEditSection::MiscModule::kChoiceParameters.begin(), MasterEditSection::MiscModule::kChoiceParameters.end());
        return v;
    }();


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
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kCopy
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPaste
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
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kCopy
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPaste
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
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
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
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
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
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
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
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kCopy
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPaste
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
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kCopy
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPaste
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
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kCopy
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPaste
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
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kCopy
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPaste
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
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kCopy
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPaste
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


    // =================================================================================================================
    // Matrix Modulation Section | Header Standalone Widgets
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> MatrixModulationSection::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationInit,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::MatrixModulationSection::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        }
    };


    // =================================================================================================================
    // Matrix Modulation Section | Source/Destination Choices & Modulation Buses
    // =================================================================================================================

    namespace MatrixModulationDetail
    {
        // Source Choices ----------------------------------------------------------------------------------------------
        const juce::StringArray kSourceChoices = PluginHelpers::makeStringArray({
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
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever3
        });

        // Destination Choices -----------------------------------------------------------------------------------------
        const juce::StringArray kDestinationChoices = PluginHelpers::makeStringArray({
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kNone, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kDco1Frequency, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kDco1PulseWidth, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kDco1WaveShape,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kDco2Frequency, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kDco2PulseWidth, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kDco2WaveShape,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kDco1Dco2Mix, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kVcfFmByDco1, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kVcfFrequency, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kVcfResonance,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kVca1Volume, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kVca2Volume,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Delay, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Attack, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Decay, 
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Release, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Amplitude,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Delay, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Attack, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Decay, 
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Release, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Amplitude,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Delay, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Attack, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Decay, 
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Release, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Amplitude,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kLfo1Speed, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kLfo1Amplitude, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kLfo2Speed, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kLfo2Amplitude,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kPortamentoRate
        });

        
        // =============================================================================================================
        // Matrix Modulation Section | Bus 0
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus0Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus0Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus0,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus0Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 105,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus0Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 104,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 106,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 1
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus1Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus1Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus1,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus1Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 108,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus1Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 107,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 109,
            .sysExId = kNoSysExId
        } };

        
        // =============================================================================================================
        // Matrix Modulation Section | Bus 2
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus2Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus2Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus2,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus2Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 111,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus2Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 110,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 112,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 3
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus3Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus3Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus3,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus3Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 114,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus3Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 113,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 115,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 4
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus4Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus4Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus4,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus4Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 117,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus4Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 116,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 118,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 5
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus5Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus5Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus5,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus5Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 120,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus5Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 119,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 121,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 6
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus6Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus6Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus6,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus6Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 123,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus6Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 122,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 124,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 7
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus7Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus7Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus7,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus7Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 126,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus7Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 125,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 127,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 8
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus8Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus8Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus8,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus8Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 129,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus8Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 128,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 130,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 9
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus9Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus9Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus9,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus9,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus9Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus9,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 132,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus9Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus9,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 131,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus9,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 133,
            .sysExId = kNoSysExId
        } };
    }


    // =================================================================================================================
    // Matrix Modulation Section | Modulation Bus Arrays (public API)
    // =================================================================================================================

    const std::array<std::vector<StandaloneWidgetDescriptor>, ::Matrix1000Limits::kModulationBusCount> MatrixModulationSection::kModulationBusStandaloneWidgets = {{
        MatrixModulationDetail::bus0Standalone,
        MatrixModulationDetail::bus1Standalone,
        MatrixModulationDetail::bus2Standalone,
        MatrixModulationDetail::bus3Standalone,
        MatrixModulationDetail::bus4Standalone,
        MatrixModulationDetail::bus5Standalone,
        MatrixModulationDetail::bus6Standalone,
        MatrixModulationDetail::bus7Standalone,
        MatrixModulationDetail::bus8Standalone,
        MatrixModulationDetail::bus9Standalone
    }};

    const std::array<std::vector<IntParameterDescriptor>, ::Matrix1000Limits::kModulationBusCount> MatrixModulationSection::kModulationBusIntParameters = {{
        MatrixModulationDetail::bus0Int,
        MatrixModulationDetail::bus1Int,
        MatrixModulationDetail::bus2Int,
        MatrixModulationDetail::bus3Int,
        MatrixModulationDetail::bus4Int,
        MatrixModulationDetail::bus5Int,
        MatrixModulationDetail::bus6Int,
        MatrixModulationDetail::bus7Int,
        MatrixModulationDetail::bus8Int,
        MatrixModulationDetail::bus9Int
    }};

    const std::array<std::vector<ChoiceParameterDescriptor>, ::Matrix1000Limits::kModulationBusCount> MatrixModulationSection::kModulationBusChoiceParameters = {{
        MatrixModulationDetail::bus0Choice,
        MatrixModulationDetail::bus1Choice,
        MatrixModulationDetail::bus2Choice,
        MatrixModulationDetail::bus3Choice,
        MatrixModulationDetail::bus4Choice,
        MatrixModulationDetail::bus5Choice,
        MatrixModulationDetail::bus6Choice,
        MatrixModulationDetail::bus7Choice,
        MatrixModulationDetail::bus8Choice,
        MatrixModulationDetail::bus9Choice
    }};

    
    // =================================================================================================================
    // Patch Manager Section | Bank Utility Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchManagerSection::BankUtilityModule::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kUnlockBank,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kUnlockBank,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchManagerUnlockBank
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank0,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank0,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank1,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank1,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank2,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank2,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank3,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank3,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank4,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank4,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank5,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank5,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank6,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank6,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank7,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank7,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank8,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank8,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank9,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank9,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect
        }
    };


    // =================================================================================================================
    // Patch Manager Section | Internal Patches Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchManagerSection::InternalPatchesModule::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInternalPatchesMemory
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadNextPatch,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadNextPatch,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInternalPatchesMemory
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kNumber
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kNumber
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kInitPatch,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kInitPatch,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCopyPatch,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCopyPatch,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kCopy
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPaste
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInternalPatchesMemory
        }
    };


    // =================================================================================================================
    // Patch Manager Section | Computer Patches Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchManagerSection::ComputerPatchesModule::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile,
            .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile,
            .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInternalPatchesMemory
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile,
            .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile,
            .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInternalPatchesMemory
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kComboBox
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kOpenPatchFolder,
            .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kOpenPatchFolder,
            .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInternalPatchesMemory
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchAs,
            .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchAsFile,
            .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kComputerPatchesSaveAs
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchFile,
            .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchFile,
            .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInternalPatchesMemory
        }
    };


    // =================================================================================================================
    // Patch Manager Section | Patch Mutator Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchManagerSection::PatchMutatorModule::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kLabel
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kLabel
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistory,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistory,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kComboBox
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kMutate,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kMutate,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchMutatorMutate
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchMutatorMutate
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchMutatorMutate
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchMutatorDelete
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchMutatorClear
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kPatchMutatorClear
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco1,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco1,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco2,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco2,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kVcfVca,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kVcfVca,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kFmTrack,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kFmTrack,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRampPortamento,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRampPortamento,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope1,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope1,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope2,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope2,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope3,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope3,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo1,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo1,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo2,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo2,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
            .buttonWidth = PluginDimensions::Widgets::Widths::Button::kInit
        }
    };
}