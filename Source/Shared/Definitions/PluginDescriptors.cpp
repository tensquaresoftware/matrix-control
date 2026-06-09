// This file defines all static descriptor data used by the plugin: APVTS group hierarchy,
// and per-section/per-module lists of standalone widgets, int parameters and choice parameters.
// Consumers (e.g. ApvtsFactory, WidgetFactory) use these definitions to build the APVTS and widget maps.

#include "PluginDescriptors.h"

#include <array>

#include "GUI/Layout/Design/Design.h"
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
            .buttonWidth = TSS::Design::Atoms::Widths::Button::kInit
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
            .buttonWidth = TSS::Design::Atoms::Widths::Button::kInit
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
            .buttonWidth = TSS::Design::Atoms::Widths::Button::kInit
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


}
