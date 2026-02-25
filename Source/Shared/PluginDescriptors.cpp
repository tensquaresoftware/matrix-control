#include "PluginDescriptors.h"

#include <array>
#include <initializer_list>

#include "PluginDisplayNames.h"

namespace PluginDescriptors
{

// ============================================================================
// Helper function for StringArray construction
// ============================================================================

namespace
{
    juce::StringArray makeStringArray(std::initializer_list<const char*> strings)
    {
        juce::StringArray result;
        for (const char* str : strings)
        {
            result.add(str);
        }
        return result;
    }
}

// ============================================================================
// Data Definitions | APVTS Groups
// ============================================================================

const std::vector<ApvtsGroupDescriptor> kAllApvtsGroups = {
    // Plugin Modes
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
    // Plugin Sections
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
    // Master Edit Modules
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
    // Patch Edit Modules
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
    // Matrix Modulation Busses
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
    // Patch Manager Modules
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

// ============================================================================
// Data Definitions | Master Edit | Int Parameters
// ============================================================================

const std::vector<IntParameterDescriptor> kMasterEditIntParameters = {
    // MIDI Module Int Parameters
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
    },
    // VIBRATO Module Int Parameters
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
    },
    // MISC Module Int Parameters
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

// ============================================================================
// Data Definitions | Master Edit | Choice Parameters
// ============================================================================

const std::vector<ChoiceParameterDescriptor> kMasterEditChoiceParameters = {
    // MIDI Module Choice Parameters
    {
        .parameterId = PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel,
        .displayName = PluginDisplayNames::MasterEditSection::MidiModule::ParameterWidgets::kChannel,
        .parentGroupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::MidiChannel::kOmni, PluginDisplayNames::ChoiceLists::MidiChannel::kChannel1, PluginDisplayNames::ChoiceLists::MidiChannel::kChannel2, PluginDisplayNames::ChoiceLists::MidiChannel::kChannel3,
                                     PluginDisplayNames::ChoiceLists::MidiChannel::kChannel4, PluginDisplayNames::ChoiceLists::MidiChannel::kChannel5, PluginDisplayNames::ChoiceLists::MidiChannel::kChannel6, PluginDisplayNames::ChoiceLists::MidiChannel::kChannel7,
                                     PluginDisplayNames::ChoiceLists::MidiChannel::kChannel8, PluginDisplayNames::ChoiceLists::MidiChannel::kChannel9, PluginDisplayNames::ChoiceLists::MidiChannel::kChannel10, PluginDisplayNames::ChoiceLists::MidiChannel::kChannel11,
                                     PluginDisplayNames::ChoiceLists::MidiChannel::kChannel12, PluginDisplayNames::ChoiceLists::MidiChannel::kChannel13, PluginDisplayNames::ChoiceLists::MidiChannel::kChannel14, PluginDisplayNames::ChoiceLists::MidiChannel::kChannel15,
                                     PluginDisplayNames::ChoiceLists::MidiChannel::kChannel16, PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG1, PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG2, PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG3,
                                     PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG4, PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG5, PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG6, PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG7,
                                     PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG8, PluginDisplayNames::ChoiceLists::MidiChannel::kMonoG9 }),
        .defaultIndex = 0,
        .sysExOffset = 11, // Combination of offsets 11, 12, 35
        .sysExId = kNoSysExId
    },
    {
        .parameterId = PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kMidiEcho,
        .displayName = PluginDisplayNames::MasterEditSection::MidiModule::ParameterWidgets::kMidiEcho,
        .parentGroupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::OnOff::kOff, PluginDisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 32,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kControllers,
        .displayName = PluginDisplayNames::MasterEditSection::MidiModule::ParameterWidgets::kControllers,
        .parentGroupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::OnOff::kOff, PluginDisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 1,
        .sysExOffset = 13,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPatchChanges,
        .displayName = PluginDisplayNames::MasterEditSection::MidiModule::ParameterWidgets::kPatchChanges,
        .parentGroupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::OnOff::kOff, PluginDisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 1,
        .sysExOffset = 14,
        .sysExId = kNoSysExId
    },
    // VIBRATO Module Choice Parameters
    {
        .parameterId = PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModSource,
        .displayName = PluginDisplayNames::MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModSource,
        .parentGroupId = PluginIDs::MasterEditSection::VibratoModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::OnOff::kOff, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal1 }),
        .defaultIndex = 0,
        .sysExOffset = 2,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kWaveform,
        .displayName = PluginDisplayNames::MasterEditSection::VibratoModule::ParameterWidgets::kWaveform,
        .parentGroupId = PluginIDs::MasterEditSection::VibratoModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::LfoWaveform::kTriangle, PluginDisplayNames::ChoiceLists::LfoWaveform::kUpsaw, PluginDisplayNames::ChoiceLists::LfoWaveform::kDnsaw,
                                     PluginDisplayNames::ChoiceLists::LfoWaveform::kSquare, PluginDisplayNames::ChoiceLists::LfoWaveform::kRandom, PluginDisplayNames::ChoiceLists::LfoWaveform::kNoise, 
                                     PluginDisplayNames::ChoiceLists::LfoWaveform::kSampled }),
        .defaultIndex = 0,
        .sysExOffset = 4,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmpModSource,
        .displayName = PluginDisplayNames::MasterEditSection::VibratoModule::ParameterWidgets::kAmpModSource,
        .parentGroupId = PluginIDs::MasterEditSection::VibratoModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::OnOff::kOff, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal1 }),
        .defaultIndex = 0,
        .sysExOffset = 6,
        .sysExId = kNoSysExId
    },
    // MISC Module Choice Parameters
    {
        .parameterId = PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable,
        .displayName = PluginDisplayNames::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable,
        .parentGroupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::OnOff::kOff, PluginDisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 169,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kVolumeInvertEnable,
        .displayName = PluginDisplayNames::MasterEditSection::MiscModule::ParameterWidgets::kVolumeInvertEnable,
        .parentGroupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::OnOff::kOff, PluginDisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 170,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kBankLockEnable,
        .displayName = PluginDisplayNames::MasterEditSection::MiscModule::ParameterWidgets::kBankLockEnable,
        .parentGroupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::OnOff::kOff, PluginDisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 165,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMemoryProtectEnable,
        .displayName = PluginDisplayNames::MasterEditSection::MiscModule::ParameterWidgets::kMemoryProtectEnable,
        .parentGroupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::OnOff::kOff, PluginDisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 171,
        .sysExId = kNoSysExId
    }
};

// ============================================================================
// Data Definitions | Patch Edit | DCO 1 Module | Int Parameters
// ============================================================================

const std::vector<IntParameterDescriptor> kDco1IntParameters = {
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

// ============================================================================
// Data Definitions | Patch Edit | DCO 1 Module | Choice Parameters
// ============================================================================

const std::vector<ChoiceParameterDescriptor> kDco1ChoiceParameters = {
    {
        .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kSync,
        .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kSync,
        .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::Sync::kOff, PluginDisplayNames::ChoiceLists::Sync::kSoft, PluginDisplayNames::ChoiceLists::Sync::kMedium, PluginDisplayNames::ChoiceLists::Sync::kHard }),
        .defaultIndex = 0,
        .sysExOffset = 25,
        .sysExId = 2
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kWaveSelect,
        .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kWaveSelect,
        .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::WaveSelect::kOff, PluginDisplayNames::ChoiceLists::WaveSelect::kPulse, PluginDisplayNames::ChoiceLists::WaveSelect::kWave, PluginDisplayNames::ChoiceLists::WaveSelect::kBoth }),
        .defaultIndex = 2,
        .sysExOffset = 13,
        .sysExId = 6
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kLevers,
        .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kLevers,
        .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::Levers::kOff, PluginDisplayNames::ChoiceLists::Levers::kLever1Bend, PluginDisplayNames::ChoiceLists::Levers::kLever2Vibrato, PluginDisplayNames::ChoiceLists::Levers::kBoth }),
        .defaultIndex = 3,
        .sysExOffset = 12,
        .sysExId = 7
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kKeyboardPortamento,
        .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kKeyboardPortamento,
        .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::KeyboardPortamento::kKeyboard, PluginDisplayNames::ChoiceLists::KeyboardPortamento::kPortamento }),
        .defaultIndex = 0,
        .sysExOffset = 21,
        .sysExId = 8
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kKeyClick,
        .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kKeyClick,
        .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::OnOff::kOff, PluginDisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 22,
        .sysExId = 9
    }
};

// ============================================================================
// Data Definitions | Patch Edit | DCO 2 Module | Int Parameters
// ============================================================================

const std::vector<IntParameterDescriptor> kDco2IntParameters = {
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

// ============================================================================
// Data Definitions | Patch Edit | DCO 2 Module | Choice Parameters
// ============================================================================

const std::vector<ChoiceParameterDescriptor> kDco2ChoiceParameters = {
    {
        .parameterId = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kWaveSelect,
        .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kWaveSelect,
        .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::WaveSelect::kOff, PluginDisplayNames::ChoiceLists::WaveSelect::kPulse, PluginDisplayNames::ChoiceLists::WaveSelect::kWave, PluginDisplayNames::ChoiceLists::WaveSelect::kBoth, PluginDisplayNames::ChoiceLists::WaveSelect::kNoise }),
        .defaultIndex = 1,
        .sysExOffset = 18,
        .sysExId = 16
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kLevers,
        .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kLevers,
        .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::Levers::kOff, PluginDisplayNames::ChoiceLists::Levers::kLever1Bend, PluginDisplayNames::ChoiceLists::Levers::kLever2Vibrato, PluginDisplayNames::ChoiceLists::Levers::kBoth }),
        .defaultIndex = 3,
        .sysExOffset = 17,
        .sysExId = 17
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kKeyboardPortamento,
        .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kKeyboardPortamento,
        .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::KeyboardPortamento::kOff, PluginDisplayNames::ChoiceLists::KeyboardPortamento::kPortamento, PluginDisplayNames::ChoiceLists::KeyboardPortamento::kKeyboard }),
        .defaultIndex = 2,
        .sysExOffset = 23,
        .sysExId = 18
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kKeyClick,
        .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kKeyClick,
        .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::OnOff::kOff, PluginDisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 24,
        .sysExId = 19
    }
};

// ============================================================================
// Data Definitions | Patch Edit | VCF/VCA Module | Int Parameters
// ============================================================================

const std::vector<IntParameterDescriptor> kVcfVcaIntParameters = {
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

// ============================================================================
// Data Definitions | Patch Edit | VCF/VCA Module | Choice Parameters
// ============================================================================

const std::vector<ChoiceParameterDescriptor> kVcfVcaChoiceParameters = {
    {
        .parameterId = PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kLevers,
        .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::ParameterWidgets::kLevers,
        .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::Levers::kOff, PluginDisplayNames::ChoiceLists::Levers::kLever1Bend, PluginDisplayNames::ChoiceLists::Levers::kLever2Vibrato, PluginDisplayNames::ChoiceLists::Levers::kBoth }),
        .defaultIndex = 0,
        .sysExOffset = 28,
        .sysExId = 25
    },
    {
        .parameterId = PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kKeyboardPortamento,
        .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::ParameterWidgets::kKeyboardPortamento,
        .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::KeyboardPortamento::kOff, PluginDisplayNames::ChoiceLists::KeyboardPortamento::kPortamento, PluginDisplayNames::ChoiceLists::KeyboardPortamento::kKeyboard }),
        .defaultIndex = 2,
        .sysExOffset = 29,
        .sysExId = 26
    }
};

// ============================================================================
// Data Definitions | Patch Edit | FM/TRACK Module | Int Parameters
// ============================================================================

const std::vector<IntParameterDescriptor> kFmTrackIntParameters = {
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

// ============================================================================
// Data Definitions | Patch Edit | FM/TRACK Module | Choice Parameters
// ============================================================================

const std::vector<ChoiceParameterDescriptor> kFmTrackChoiceParameters = {
    {
        .parameterId = PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackInput,
        .displayName = PluginDisplayNames::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackInput,
        .parentGroupId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::ModulationBus::Source::kNone, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope3, 
                                     PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVibrato, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp2,
                                     PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboard, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPortamento, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kTrack, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboardGate, 
                                     PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVelocity, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kReleaseVelocity, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPressure,
                                     PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever3 }),
        .defaultIndex = 9,
        .sysExOffset = 76,
        .sysExId = 33
    }
};

// ============================================================================
// Data Definitions | Patch Edit | RAMP/PORTAMENTO Module | Int Parameters
// ============================================================================

const std::vector<IntParameterDescriptor> kRampPortamentoIntParameters = {
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

// ============================================================================
// Data Definitions | Patch Edit | RAMP/PORTAMENTO Module | Choice Parameters
// ============================================================================

const std::vector<ChoiceParameterDescriptor> kRampPortamentoChoiceParameters = {
    {
        .parameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Trigger,
        .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Trigger,
        .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::RampTrigger::kStrig, PluginDisplayNames::ChoiceLists::RampTrigger::kMtrig, PluginDisplayNames::ChoiceLists::RampTrigger::kExtrig, PluginDisplayNames::ChoiceLists::RampTrigger::kGatedx }),
        .defaultIndex = 0,
        .sysExOffset = 83,
        .sysExId = 41
    },
    {
        .parameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Trigger,
        .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Trigger,
        .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::RampTrigger::kStrig, PluginDisplayNames::ChoiceLists::RampTrigger::kMtrig, PluginDisplayNames::ChoiceLists::RampTrigger::kExtrig, PluginDisplayNames::ChoiceLists::RampTrigger::kGatedx }),
        .defaultIndex = 0,
        .sysExOffset = 85,
        .sysExId = 43
    },
    {
        .parameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoMode,
        .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoMode,
        .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::PortamentoMode::kLinear, PluginDisplayNames::ChoiceLists::PortamentoMode::kConst, PluginDisplayNames::ChoiceLists::PortamentoMode::kExpo }),
        .defaultIndex = 0,
        .sysExOffset = 33,
        .sysExId = 46
    },
    {
        .parameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoLegato,
        .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoLegato,
        .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::OnOff::kOff, PluginDisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 34,
        .sysExId = 47
    },
    {
        .parameterId = PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode,
        .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode,
        .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kRotate, PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kReasgn, 
                                     PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kUnison, PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kRearob }),
        .defaultIndex = 1,
        .sysExOffset = 8,
        .sysExId = 48
    }
};

// ============================================================================
// Data Definitions | Patch Edit | ENV 1 Module | Int Parameters
// ============================================================================

const std::vector<IntParameterDescriptor> kEnv1IntParameters = {
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

// ============================================================================
// Data Definitions | Patch Edit | ENV 1 Module | Choice Parameters
// ============================================================================

const std::vector<ChoiceParameterDescriptor> kEnv1ChoiceParameters = {
    {
        .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kTriggerMode,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kTriggerMode,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::TriggerMode::kStrig, PluginDisplayNames::ChoiceLists::TriggerMode::kMtrig, PluginDisplayNames::ChoiceLists::TriggerMode::kSreset, 
                                     PluginDisplayNames::ChoiceLists::TriggerMode::kMreset, PluginDisplayNames::ChoiceLists::TriggerMode::kXtrig, PluginDisplayNames::ChoiceLists::TriggerMode::kXmtrig, 
                                     PluginDisplayNames::ChoiceLists::TriggerMode::kXreset, PluginDisplayNames::ChoiceLists::TriggerMode::kXmrst }),
        .defaultIndex = 0,
        .sysExOffset = 49,
        .sysExId = 57
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kEnvelopeMode,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kEnvelopeMode,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::EnvelopeMode::kNormal, PluginDisplayNames::ChoiceLists::EnvelopeMode::kDadr, PluginDisplayNames::ChoiceLists::EnvelopeMode::kFree, PluginDisplayNames::ChoiceLists::EnvelopeMode::kBoth }),
        .defaultIndex = 0,
        .sysExOffset = 57,
        .sysExId = 58
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kLfo1Trigger,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kLfo1Trigger,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::Lfo1Trigger::kNormal, PluginDisplayNames::ChoiceLists::Lfo1Trigger::kLfo1, PluginDisplayNames::ChoiceLists::Lfo1Trigger::kGLfo1 }),
        .defaultIndex = 0,
        .sysExOffset = 56,
        .sysExId = 59
    }
};

// ============================================================================
// Data Definitions | Patch Edit | ENV 2 Module | Int Parameters
// ============================================================================

const std::vector<IntParameterDescriptor> kEnv2IntParameters = {
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

// ============================================================================
// Data Definitions | Patch Edit | ENV 2 Module | Choice Parameters
// ============================================================================

const std::vector<ChoiceParameterDescriptor> kEnv2ChoiceParameters = {
    {
        .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kTriggerMode,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kTriggerMode,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::TriggerMode::kStrig, PluginDisplayNames::ChoiceLists::TriggerMode::kMtrig, PluginDisplayNames::ChoiceLists::TriggerMode::kSreset, 
                                     PluginDisplayNames::ChoiceLists::TriggerMode::kMreset, PluginDisplayNames::ChoiceLists::TriggerMode::kXtrig, PluginDisplayNames::ChoiceLists::TriggerMode::kXmtrig, 
                                     PluginDisplayNames::ChoiceLists::TriggerMode::kXreset, PluginDisplayNames::ChoiceLists::TriggerMode::kXmrst }),
        .defaultIndex = 0,
        .sysExOffset = 58,
        .sysExId = 67
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kEnvelopeMode,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kEnvelopeMode,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::EnvelopeMode::kNormal, PluginDisplayNames::ChoiceLists::EnvelopeMode::kDadr, PluginDisplayNames::ChoiceLists::EnvelopeMode::kFree, PluginDisplayNames::ChoiceLists::EnvelopeMode::kBoth }),
        .defaultIndex = 0,
        .sysExOffset = 66,
        .sysExId = 68
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kLfo1Trigger,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kLfo1Trigger,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::Lfo1Trigger::kNormal, PluginDisplayNames::ChoiceLists::Lfo1Trigger::kLfo1, PluginDisplayNames::ChoiceLists::Lfo1Trigger::kGLfo1 }),
        .defaultIndex = 0,
        .sysExOffset = 65,
        .sysExId = 69
    }
};

// ============================================================================
// Data Definitions | Patch Edit | ENV 3 Module | Int Parameters
// ============================================================================

const std::vector<IntParameterDescriptor> kEnv3IntParameters = {
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

// ============================================================================
// Data Definitions | Patch Edit | ENV 3 Module | Choice Parameters
// ============================================================================

const std::vector<ChoiceParameterDescriptor> kEnv3ChoiceParameters = {
    {
        .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kTriggerMode,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kTriggerMode,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::TriggerMode::kStrig, PluginDisplayNames::ChoiceLists::TriggerMode::kMtrig, PluginDisplayNames::ChoiceLists::TriggerMode::kSreset, 
                                     PluginDisplayNames::ChoiceLists::TriggerMode::kMreset, PluginDisplayNames::ChoiceLists::TriggerMode::kXtrig, PluginDisplayNames::ChoiceLists::TriggerMode::kXmtrig, 
                                     PluginDisplayNames::ChoiceLists::TriggerMode::kXreset, PluginDisplayNames::ChoiceLists::TriggerMode::kXmrst }),
        .defaultIndex = 0,
        .sysExOffset = 67,
        .sysExId = 77
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kEnvelopeMode,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kEnvelopeMode,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::EnvelopeMode::kNormal, PluginDisplayNames::ChoiceLists::EnvelopeMode::kDadr, PluginDisplayNames::ChoiceLists::EnvelopeMode::kFree, PluginDisplayNames::ChoiceLists::EnvelopeMode::kBoth }),
        .defaultIndex = 0,
        .sysExOffset = 75,
        .sysExId = 78
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kLfo1Trigger,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kLfo1Trigger,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::Lfo1Trigger::kNormal, PluginDisplayNames::ChoiceLists::Lfo1Trigger::kLfo1, PluginDisplayNames::ChoiceLists::Lfo1Trigger::kGatedLfo1Trigger }),
        .defaultIndex = 0,
        .sysExOffset = 74,
        .sysExId = 79
    }
};

// ============================================================================
// Data Definitions | Patch Edit | LFO 1 Module | Int Parameters
// ============================================================================

const std::vector<IntParameterDescriptor> kLfo1IntParameters = {
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

const std::vector<ChoiceParameterDescriptor> kLfo1ChoiceParameters = {
    {
        .parameterId = PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kWaveform,
        .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::ParameterWidgets::kWaveform,
        .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::LfoWaveform::kTriangle, PluginDisplayNames::ChoiceLists::LfoWaveform::kUpsaw, PluginDisplayNames::ChoiceLists::LfoWaveform::kDnsaw, 
                                     PluginDisplayNames::ChoiceLists::LfoWaveform::kSquare, PluginDisplayNames::ChoiceLists::LfoWaveform::kRandom, PluginDisplayNames::ChoiceLists::LfoWaveform::kNoise, PluginDisplayNames::ChoiceLists::LfoWaveform::kSampled }),
        .defaultIndex = 0,
        .sysExOffset = 38,
        .sysExId = 82
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kTriggerMode,
        .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::ParameterWidgets::kTriggerMode,
        .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::LfoTriggerMode::kOff, PluginDisplayNames::ChoiceLists::LfoTriggerMode::kStrig, PluginDisplayNames::ChoiceLists::LfoTriggerMode::kMtrig, PluginDisplayNames::ChoiceLists::LfoTriggerMode::kXtrig }),
        .defaultIndex = 0,
        .sysExOffset = 36,
        .sysExId = 86
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kLag,
        .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::ParameterWidgets::kLag,
        .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::OnOff::kOff, PluginDisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 37,
        .sysExId = 87
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kSampleInput,
        .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::ParameterWidgets::kSampleInput,
        .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::ModulationBus::Source::kNone, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope3, 
                                     PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVibrato, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp2,
                                     PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboard, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPortamento, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kTrack, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboardGate, 
                                     PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVelocity, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kReleaseVelocity, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPressure,
                                     PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever3 }),
        .defaultIndex = 9,
        .sysExOffset = 40,
        .sysExId = 88
    }
};

// ============================================================================
// Data Definitions | Patch Edit | LFO 2 Module | Int Parameters
// ============================================================================

const std::vector<IntParameterDescriptor> kLfo2IntParameters = {
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

// ============================================================================
// Data Definitions | Patch Edit | LFO 2 Module | Choice Parameters
// ============================================================================

const std::vector<ChoiceParameterDescriptor> kLfo2ChoiceParameters = {
    {
        .parameterId = PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kWaveform,
        .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::ParameterWidgets::kWaveform,
        .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::LfoWaveform::kTriangle, PluginDisplayNames::ChoiceLists::LfoWaveform::kUpsaw, PluginDisplayNames::ChoiceLists::LfoWaveform::kDnsaw, 
                                     PluginDisplayNames::ChoiceLists::LfoWaveform::kSquare, PluginDisplayNames::ChoiceLists::LfoWaveform::kRandom, PluginDisplayNames::ChoiceLists::LfoWaveform::kNoise, PluginDisplayNames::ChoiceLists::LfoWaveform::kSampled }),
        .defaultIndex = 0,
        .sysExOffset = 45,
        .sysExId = 92
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kTriggerMode,
        .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::ParameterWidgets::kTriggerMode,
        .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::LfoTriggerMode::kOff, PluginDisplayNames::ChoiceLists::LfoTriggerMode::kStrig, PluginDisplayNames::ChoiceLists::LfoTriggerMode::kMtrig, PluginDisplayNames::ChoiceLists::LfoTriggerMode::kXtrig }),
        .defaultIndex = 0,
        .sysExOffset = 43,
        .sysExId = 96
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kLag,
        .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::ParameterWidgets::kLag,
        .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::OnOff::kOff, PluginDisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 44,
        .sysExId = 97
    },
    {
        .parameterId = PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kSampleInput,
        .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::ParameterWidgets::kSampleInput,
        .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
        .choices = makeStringArray({ PluginDisplayNames::ChoiceLists::ModulationBus::Source::kNone, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope3, 
                                     PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVibrato, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp2,
                                     PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboard, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPortamento, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kTrack, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboardGate, 
                                     PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVelocity, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kReleaseVelocity, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPressure,
                                     PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever3 }),
        .defaultIndex = 9,
        .sysExOffset = 47,
        .sysExId = 98
    }
};

// ============================================================================
// Data Definitions | Matrix Modulation | Source/Destination Choices
// ============================================================================

namespace
{
    const juce::StringArray kSourceChoices = makeStringArray({
        PluginDisplayNames::ChoiceLists::ModulationBus::Source::kNone, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope3, 
        PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVibrato, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp2,
        PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboard, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPortamento, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kTrack, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboardGate, 
        PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVelocity, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kReleaseVelocity, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPressure,
        PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever1, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever2, PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever3
    });

    const juce::StringArray kDestinationChoices = makeStringArray({
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
}

// ============================================================================
// Data Definitions | Matrix Modulation | Choices Parameters
// ============================================================================

const std::array<std::vector<ChoiceParameterDescriptor>, PluginIDs::MatrixModulationSection::kModulationBusCount> kModulationBusChoiceParameters = {{
    // Modulation Bus 0
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 104,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 106,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 1
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 107,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 109,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 2
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 110,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 112,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 3
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 113,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 115,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 4
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 116,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 118,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 5
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 119,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 121,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 6
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 122,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 124,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 7
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 125,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 127,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 8
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 128,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 130,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 9
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus9,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 131,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus9,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 133,
            .sysExId = kNoSysExId
        }
    }
}};

// ============================================================================
// Data Definitions | Matrix Modulation | Int Parameters
// ============================================================================

const std::array<std::vector<IntParameterDescriptor>, PluginIDs::MatrixModulationSection::kModulationBusCount> kModulationBusIntParameters = {{
    // Modulation Bus 0
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 105,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 1
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 108,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 2
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 111,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 3
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 114,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 4
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 117,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 5
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 120,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 6
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 123,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 7
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 126,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 8
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 129,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 9
    {
        {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus9,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 132,
            .sysExId = kNoSysExId
        }
    }
}};

// ============================================================================
// Data Definitions | Standalone Widgets | Master Edit
// ============================================================================

// Master Edit Standalone Widgets
const std::vector<StandaloneWidgetDescriptor> kMasterEditStandaloneWidgets = {
    {
        .widgetId = PluginIDs::MasterEditSection::MidiModule::StandaloneWidgets::kInit,
        .displayName = PluginDisplayNames::MasterEditSection::MidiModule::StandaloneWidgets::kInit,
        .parentGroupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::MasterEditSection::VibratoModule::StandaloneWidgets::kInit,
        .displayName = PluginDisplayNames::MasterEditSection::VibratoModule::StandaloneWidgets::kInit,
        .parentGroupId = PluginIDs::MasterEditSection::VibratoModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::MasterEditSection::MiscModule::StandaloneWidgets::kInit,
        .displayName = PluginDisplayNames::MasterEditSection::MiscModule::StandaloneWidgets::kInit,
        .parentGroupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ============================================================================
// Data Definitions | Standalone Widgets | Patch Edit
// ============================================================================

// DCO 1 Module
const std::vector<StandaloneWidgetDescriptor> kDco1StandaloneWidgets = {
    {
        .widgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kInit,
        .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::StandaloneWidgets::kInit,
        .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kCopy,
        .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::StandaloneWidgets::kCopy,
        .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kPaste,
        .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::StandaloneWidgets::kPaste,
        .parentGroupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// DCO 2 Module
const std::vector<StandaloneWidgetDescriptor> kDco2StandaloneWidgets = {
    {
        .widgetId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kInit,
        .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::StandaloneWidgets::kInit,
        .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kCopy,
        .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::StandaloneWidgets::kCopy,
        .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kPaste,
        .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::StandaloneWidgets::kPaste,
        .parentGroupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// VCF/VCA Module
const std::vector<StandaloneWidgetDescriptor> kVcfVcaStandaloneWidgets = {
    {
        .widgetId = PluginIDs::PatchEditSection::VcfVcaModule::StandaloneWidgets::kInit,
        .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::StandaloneWidgets::kInit,
        .parentGroupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// FM/TRACK Module
const std::vector<StandaloneWidgetDescriptor> kFmTrackStandaloneWidgets = {
    {
        .widgetId = PluginIDs::PatchEditSection::FmTrackModule::StandaloneWidgets::kInit,
        .displayName = PluginDisplayNames::PatchEditSection::FmTrackModule::StandaloneWidgets::kInit,
        .parentGroupId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// RAMP/PORTAMENTO Module
const std::vector<StandaloneWidgetDescriptor> kRampPortamentoStandaloneWidgets = {
    {
        .widgetId = PluginIDs::PatchEditSection::RampPortamentoModule::StandaloneWidgets::kInit,
        .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::StandaloneWidgets::kInit,
        .parentGroupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ENV 1 Module
const std::vector<StandaloneWidgetDescriptor> kEnv1StandaloneWidgets = {
    {
        .widgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kInit,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::StandaloneWidgets::kInit,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kCopy,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::StandaloneWidgets::kCopy,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kPaste,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::StandaloneWidgets::kPaste,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ENV 2 Module
const std::vector<StandaloneWidgetDescriptor> kEnv2StandaloneWidgets = {
    {
        .widgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kInit,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::StandaloneWidgets::kInit,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kCopy,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::StandaloneWidgets::kCopy,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kPaste,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::StandaloneWidgets::kPaste,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ENV 3 Module
const std::vector<StandaloneWidgetDescriptor> kEnv3StandaloneWidgets = {
    {
        .widgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kInit,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::StandaloneWidgets::kInit,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kCopy,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::StandaloneWidgets::kCopy,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kPaste,
        .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::StandaloneWidgets::kPaste,
        .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// LFO 1 Module
const std::vector<StandaloneWidgetDescriptor> kLfo1StandaloneWidgets = {
    {
        .widgetId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kInit,
        .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::StandaloneWidgets::kInit,
        .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kCopy,
        .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::StandaloneWidgets::kCopy,
        .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kPaste,
        .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::StandaloneWidgets::kPaste,
        .parentGroupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// LFO 2 Module
const std::vector<StandaloneWidgetDescriptor> kLfo2StandaloneWidgets = {
    {
        .widgetId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kInit,
        .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::StandaloneWidgets::kInit,
        .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kCopy,
        .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::StandaloneWidgets::kCopy,
        .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kPaste,
        .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::StandaloneWidgets::kPaste,
        .parentGroupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ============================================================================
// Data Definitions | Standalone Widgets | Patch Manager
// ============================================================================

// Bank Utility Module
const std::vector<StandaloneWidgetDescriptor> kBankUtilityWidgets = {
    {
        .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kUnlockBank,
        .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kUnlockBank,
        .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank0,
        .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank0,
        .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank1,
        .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank1,
        .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank2,
        .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank2,
        .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank3,
        .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank3,
        .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank4,
        .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank4,
        .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank5,
        .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank5,
        .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank6,
        .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank6,
        .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank7,
        .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank7,
        .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank8,
        .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank8,
        .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank9,
        .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank9,
        .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ============================================================================
// Patch Manager | Standalone Widgets
// ============================================================================

// Internal Patches Module
const std::vector<StandaloneWidgetDescriptor> kInternalPatchesWidgets = {
    {
        .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch,
        .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch,
        .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadNextPatch,
        .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadNextPatch,
        .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
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
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCopyPatch,
        .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCopyPatch,
        .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch,
        .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch,
        .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch,
        .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch,
        .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// Computer Patches Module
const std::vector<StandaloneWidgetDescriptor> kComputerPatchesWidgets = {
    {
        .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile,
        .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile,
        .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile,
        .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile,
        .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
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
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchAs,
        .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchAsFile,
        .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchFile,
        .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchFile,
        .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// Patch Mutator Module
const std::vector<StandaloneWidgetDescriptor> kPatchMutatorWidgets = {
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
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco1,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco1,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco2,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco2,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kVcfVca,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kVcfVca,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kFmTrack,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kFmTrack,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRampPortamento,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRampPortamento,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope1,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope1,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope2,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope2,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope3,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope3,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo1,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo1,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo2,
        .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo2,
        .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ============================================================================
// Helper Functions Implementation
// ============================================================================

juce::String getGroupDisplayName(const juce::String& groupId)
{
    for (const auto& group : kAllApvtsGroups)
    {
        if (group.groupId == groupId)
        {
            return group.displayName;
        }
    }
    return groupId;  // Fallback to ID if not found
}

juce::String getSectionDisplayName(const char* sectionId)
{
    return getGroupDisplayName(sectionId);
}

} // namespace PluginDescriptors

