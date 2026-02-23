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
        .groupId = ModeIds::kMaster,
        .displayName = DisplayNames::Mode::kMaster
    },
    {
        .parentId = kNoParentId,
        .groupId = ModeIds::kPatch,
        .displayName = DisplayNames::Mode::kPatch
    },
    // Plugin Sections
    {
        .parentId = ModeIds::kMaster,
        .groupId = SectionIds::kMasterEdit,
        .displayName = DisplayNames::Section::kMasterEdit
    },
    {
        .parentId = ModeIds::kPatch,
        .groupId = SectionIds::kPatchEdit,
        .displayName = DisplayNames::Section::kPatchEdit
    },
    {
        .parentId = ModeIds::kPatch,
        .groupId = SectionIds::kMatrixModulation,
        .displayName = DisplayNames::Section::kMatrixModulation
    },
    {
        .parentId = ModeIds::kPatch,
        .groupId = SectionIds::kPatchManager,
        .displayName = DisplayNames::Section::kPatchManager
    },
    // Master Edit Modules
    {
        .parentId = SectionIds::kMasterEdit,
        .groupId = ModuleIds::kMidi,
        .displayName = DisplayNames::Module::kMidi
    },
    {
        .parentId = SectionIds::kMasterEdit,
        .groupId = ModuleIds::kVibrato,
        .displayName = DisplayNames::Module::kVibrato
    },
    {
        .parentId = SectionIds::kMasterEdit,
        .groupId = ModuleIds::kMisc,
        .displayName = DisplayNames::Module::kMisc
    },
    // Patch Edit Modules
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kDco1,
        .displayName = DisplayNames::Module::kDco1
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kDco2,
        .displayName = DisplayNames::Module::kDco2
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kVcfVca,
        .displayName = DisplayNames::Module::kVcfVca
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kFmTrack,
        .displayName = DisplayNames::Module::kFmTrack
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kRampPortamento,
        .displayName = DisplayNames::Module::kRampPortamento
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kEnvelope1,
        .displayName = DisplayNames::Module::kEnvelope1
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kEnvelope2,
        .displayName = DisplayNames::Module::kEnvelope2
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kEnvelope3,
        .displayName = DisplayNames::Module::kEnvelope3
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kLfo1,
        .displayName = DisplayNames::Module::kLfo1
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kLfo2,
        .displayName = DisplayNames::Module::kLfo2
    },
    // Matrix Modulation Busses
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus0,
        .displayName = DisplayNames::MatrixModulation::ModulationBus::kBus0
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus1,
        .displayName = DisplayNames::MatrixModulation::ModulationBus::kBus1
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus2,
        .displayName = DisplayNames::MatrixModulation::ModulationBus::kBus2
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus3,
        .displayName = DisplayNames::MatrixModulation::ModulationBus::kBus3
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus4,
        .displayName = DisplayNames::MatrixModulation::ModulationBus::kBus4
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus5,
        .displayName = DisplayNames::MatrixModulation::ModulationBus::kBus5
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus6,
        .displayName = DisplayNames::MatrixModulation::ModulationBus::kBus6
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus7,
        .displayName = DisplayNames::MatrixModulation::ModulationBus::kBus7
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus8,
        .displayName = DisplayNames::MatrixModulation::ModulationBus::kBus8
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus9,
        .displayName = DisplayNames::MatrixModulation::ModulationBus::kBus9
    },
    // Patch Manager Modules
    {
        .parentId = SectionIds::kPatchManager,
        .groupId = ModuleIds::kBankUtility,
        .displayName = DisplayNames::Module::kBankUtility
    },
    {
        .parentId = SectionIds::kPatchManager,
        .groupId = ModuleIds::kInternalPatches,
        .displayName = DisplayNames::Module::kInternalPatches
    },
    {
        .parentId = SectionIds::kPatchManager,
        .groupId = ModuleIds::kComputerPatches,
        .displayName = DisplayNames::Module::kComputerPatches
    },
    {
        .parentId = SectionIds::kPatchManager,
        .groupId = ModuleIds::kPatchMutator,
        .displayName = DisplayNames::Module::kPatchMutator
    }
};

// ============================================================================
// Data Definitions | Master Edit | Int Parameters
// ============================================================================

const std::vector<IntParameterDescriptor> kMasterEditIntParameters = {
    // MIDI Module Int Parameters
    {
        .parameterId = ParameterIds::kMidiPedal1Select,
        .displayName = DisplayNames::MasterEdit::Midi::ParameterWidgets::kPedal1Select,
        .parentGroupId = ModuleIds::kMidi,
        .minValue = 0,
        .maxValue = 121,
        .defaultValue = 4,
        .sysExOffset = 17,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMidiPedal2Select,
        .displayName = DisplayNames::MasterEdit::Midi::ParameterWidgets::kPedal2Select,
        .parentGroupId = ModuleIds::kMidi,
        .minValue = 0,
        .maxValue = 121,
        .defaultValue = 64,
        .sysExOffset = 18,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMidiLever2Select,
        .displayName = DisplayNames::MasterEdit::Midi::ParameterWidgets::kLever2Select,
        .parentGroupId = ModuleIds::kMidi,
        .minValue = 0,
        .maxValue = 121,
        .defaultValue = 1,
        .sysExOffset = 19,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMidiLever3Select,
        .displayName = DisplayNames::MasterEdit::Midi::ParameterWidgets::kLever3Select,
        .parentGroupId = ModuleIds::kMidi,
        .minValue = 0,
        .maxValue = 121,
        .defaultValue = 2,
        .sysExOffset = 20,
        .sysExId = kNoSysExId
    },
    // VIBRATO Module Int Parameters
    {
        .parameterId = ParameterIds::kVibratoSpeed,
        .displayName = DisplayNames::MasterEdit::Vibrato::ParameterWidgets::kSpeed,
        .parentGroupId = ModuleIds::kVibrato,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 1,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kVibratoSpeedModAmount,
        .displayName = DisplayNames::MasterEdit::Vibrato::ParameterWidgets::kSpeedModAmount,
        .parentGroupId = ModuleIds::kVibrato,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 3,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kVibratoAmplitude,
        .displayName = DisplayNames::MasterEdit::Vibrato::ParameterWidgets::kAmplitude,
        .parentGroupId = ModuleIds::kVibrato,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 5,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kVibratoAmpModAmount,
        .displayName = DisplayNames::MasterEdit::Vibrato::ParameterWidgets::kAmpModAmount,
        .parentGroupId = ModuleIds::kVibrato,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 7,
        .sysExId = kNoSysExId
    },
    // MISC Module Int Parameters
    {
        .parameterId = ParameterIds::kMasterTune,
        .displayName = DisplayNames::MasterEdit::Misc::ParameterWidgets::kMasterTune,
        .parentGroupId = ModuleIds::kMisc,
        .minValue = -31,
        .maxValue = 31,
        .defaultValue = 0,
        .sysExOffset = 8,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMasterTranspose,
        .displayName = DisplayNames::MasterEdit::Misc::ParameterWidgets::kMasterTranspose,
        .parentGroupId = ModuleIds::kMisc,
        .minValue = -24,
        .maxValue = 24,
        .defaultValue = 0,
        .sysExOffset = 34,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kBendRange,
        .displayName = DisplayNames::MasterEdit::Misc::ParameterWidgets::kBendRange,
        .parentGroupId = ModuleIds::kMisc,
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
        .parameterId = ParameterIds::kMidiChannel,
        .displayName = DisplayNames::MasterEdit::Midi::ParameterWidgets::kChannel,
        .parentGroupId = ModuleIds::kMidi,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::MidiChannel::kOmni, DisplayNames::ChoiceLists::MidiChannel::k1, DisplayNames::ChoiceLists::MidiChannel::k2, DisplayNames::ChoiceLists::MidiChannel::k3,
                                     DisplayNames::ChoiceLists::MidiChannel::k4, DisplayNames::ChoiceLists::MidiChannel::k5, DisplayNames::ChoiceLists::MidiChannel::k6, DisplayNames::ChoiceLists::MidiChannel::k7,
                                     DisplayNames::ChoiceLists::MidiChannel::k8, DisplayNames::ChoiceLists::MidiChannel::k9, DisplayNames::ChoiceLists::MidiChannel::k10, DisplayNames::ChoiceLists::MidiChannel::k11,
                                     DisplayNames::ChoiceLists::MidiChannel::k12, DisplayNames::ChoiceLists::MidiChannel::k13, DisplayNames::ChoiceLists::MidiChannel::k14, DisplayNames::ChoiceLists::MidiChannel::k15,
                                     DisplayNames::ChoiceLists::MidiChannel::k16, DisplayNames::ChoiceLists::MidiChannel::kMonoG1, DisplayNames::ChoiceLists::MidiChannel::kMonoG2, DisplayNames::ChoiceLists::MidiChannel::kMonoG3,
                                     DisplayNames::ChoiceLists::MidiChannel::kMonoG4, DisplayNames::ChoiceLists::MidiChannel::kMonoG5, DisplayNames::ChoiceLists::MidiChannel::kMonoG6, DisplayNames::ChoiceLists::MidiChannel::kMonoG7,
                                     DisplayNames::ChoiceLists::MidiChannel::kMonoG8, DisplayNames::ChoiceLists::MidiChannel::kMonoG9 }),
        .defaultIndex = 0,
        .sysExOffset = 11, // Combination of offsets 11, 12, 35
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMidiEcho,
        .displayName = DisplayNames::MasterEdit::Midi::ParameterWidgets::kMidiEcho,
        .parentGroupId = ModuleIds::kMidi,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::OnOff::kOff, DisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 32,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMidiControllers,
        .displayName = DisplayNames::MasterEdit::Midi::ParameterWidgets::kControllers,
        .parentGroupId = ModuleIds::kMidi,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::OnOff::kOff, DisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 1,
        .sysExOffset = 13,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMidiPatchChanges,
        .displayName = DisplayNames::MasterEdit::Midi::ParameterWidgets::kPatchChanges,
        .parentGroupId = ModuleIds::kMidi,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::OnOff::kOff, DisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 1,
        .sysExOffset = 14,
        .sysExId = kNoSysExId
    },
    // VIBRATO Module Choice Parameters
    {
        .parameterId = ParameterIds::kVibratoSpeedModSource,
        .displayName = DisplayNames::MasterEdit::Vibrato::ParameterWidgets::kSpeedModSource,
        .parentGroupId = ModuleIds::kVibrato,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::OnOff::kOff, DisplayNames::ChoiceLists::ModulationBus::Source::kLever2, DisplayNames::ChoiceLists::ModulationBus::Source::kPedal1 }),
        .defaultIndex = 0,
        .sysExOffset = 2,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kVibratoWaveform,
        .displayName = DisplayNames::MasterEdit::Vibrato::ParameterWidgets::kWaveform,
        .parentGroupId = ModuleIds::kVibrato,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::LfoWaveform::kTriangle, DisplayNames::ChoiceLists::LfoWaveform::kUpsaw, DisplayNames::ChoiceLists::LfoWaveform::kDnsaw,
                                     DisplayNames::ChoiceLists::LfoWaveform::kSquare, DisplayNames::ChoiceLists::LfoWaveform::kRandom, DisplayNames::ChoiceLists::LfoWaveform::kNoise, 
                                     DisplayNames::ChoiceLists::LfoWaveform::kSampled }),
        .defaultIndex = 0,
        .sysExOffset = 4,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kVibratoAmpModSource,
        .displayName = DisplayNames::MasterEdit::Vibrato::ParameterWidgets::kAmpModSource,
        .parentGroupId = ModuleIds::kVibrato,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::OnOff::kOff, DisplayNames::ChoiceLists::ModulationBus::Source::kLever2, DisplayNames::ChoiceLists::ModulationBus::Source::kPedal1 }),
        .defaultIndex = 0,
        .sysExOffset = 6,
        .sysExId = kNoSysExId
    },
    // MISC Module Choice Parameters
    {
        .parameterId = ParameterIds::kUnisonEnable,
        .displayName = DisplayNames::MasterEdit::Misc::ParameterWidgets::kUnisonEnable,
        .parentGroupId = ModuleIds::kMisc,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::OnOff::kOff, DisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 169,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kVolumeInvertEnable,
        .displayName = DisplayNames::MasterEdit::Misc::ParameterWidgets::kVolumeInvertEnable,
        .parentGroupId = ModuleIds::kMisc,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::OnOff::kOff, DisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 170,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kBankLockEnable,
        .displayName = DisplayNames::MasterEdit::Misc::ParameterWidgets::kBankLockEnable,
        .parentGroupId = ModuleIds::kMisc,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::OnOff::kOff, DisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 165,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMemoryProtectEnable,
        .displayName = DisplayNames::MasterEdit::Misc::ParameterWidgets::kMemoryProtectEnable,
        .parentGroupId = ModuleIds::kMisc,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::OnOff::kOff, DisplayNames::ChoiceLists::OnOff::kOn }),
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
        .parameterId = ParameterIds::kDco1Frequency,
        .displayName = DisplayNames::PatchEdit::Dco1::ParameterWidgets::kFrequency,
        .parentGroupId = ModuleIds::kDco1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 9,
        .sysExId = 0
    },
    {
        .parameterId = ParameterIds::kDco1FrequencyModByLfo1,
        .displayName = DisplayNames::PatchEdit::Dco1::ParameterWidgets::kFrequencyModByLfo1,
        .parentGroupId = ModuleIds::kDco1,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 86,
        .sysExId = 1
    },
    {
        .parameterId = ParameterIds::kDco1PulseWidth,
        .displayName = DisplayNames::PatchEdit::Dco1::ParameterWidgets::kPulseWidth,
        .parentGroupId = ModuleIds::kDco1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 31,
        .sysExOffset = 11,
        .sysExId = 3
    },
    {
        .parameterId = ParameterIds::kDco1PulseWidthModByLfo2,
        .displayName = DisplayNames::PatchEdit::Dco1::ParameterWidgets::kPulseWidthModByLfo2,
        .parentGroupId = ModuleIds::kDco1,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 87,
        .sysExId = 4
    },
    {
        .parameterId = ParameterIds::kDco1WaveShape,
        .displayName = DisplayNames::PatchEdit::Dco1::ParameterWidgets::kWaveShape,
        .parentGroupId = ModuleIds::kDco1,
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
        .parameterId = ParameterIds::kDco1Sync,
        .displayName = DisplayNames::PatchEdit::Dco1::ParameterWidgets::kSync,
        .parentGroupId = ModuleIds::kDco1,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::Sync::kOff, DisplayNames::ChoiceLists::Sync::kSoft, DisplayNames::ChoiceLists::Sync::kMedium, DisplayNames::ChoiceLists::Sync::kHard }),
        .defaultIndex = 0,
        .sysExOffset = 25,
        .sysExId = 2
    },
    {
        .parameterId = ParameterIds::kDco1WaveSelect,
        .displayName = DisplayNames::PatchEdit::Dco1::ParameterWidgets::kWaveSelect,
        .parentGroupId = ModuleIds::kDco1,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::WaveSelect::kOff, DisplayNames::ChoiceLists::WaveSelect::kPulse, DisplayNames::ChoiceLists::WaveSelect::kWave, DisplayNames::ChoiceLists::WaveSelect::kBoth }),
        .defaultIndex = 2,
        .sysExOffset = 13,
        .sysExId = 6
    },
    {
        .parameterId = ParameterIds::kDco1Levers,
        .displayName = DisplayNames::PatchEdit::Dco1::ParameterWidgets::kLevers,
        .parentGroupId = ModuleIds::kDco1,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::Levers::kOff, DisplayNames::ChoiceLists::Levers::kL1Bend, DisplayNames::ChoiceLists::Levers::kL2Vib, DisplayNames::ChoiceLists::Levers::kBoth }),
        .defaultIndex = 3,
        .sysExOffset = 12,
        .sysExId = 7
    },
    {
        .parameterId = ParameterIds::kDco1KeyboardPortamento,
        .displayName = DisplayNames::PatchEdit::Dco1::ParameterWidgets::kKeyboardPortamento,
        .parentGroupId = ModuleIds::kDco1,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::KeyboardPortamento::kKeybd, DisplayNames::ChoiceLists::KeyboardPortamento::kPorta }),
        .defaultIndex = 0,
        .sysExOffset = 21,
        .sysExId = 8
    },
    {
        .parameterId = ParameterIds::kDco1KeyClick,
        .displayName = DisplayNames::PatchEdit::Dco1::ParameterWidgets::kKeyClick,
        .parentGroupId = ModuleIds::kDco1,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::OnOff::kOff, DisplayNames::ChoiceLists::OnOff::kOn }),
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
        .parameterId = ParameterIds::kDco2Frequency,
        .displayName = DisplayNames::PatchEdit::Dco2::ParameterWidgets::kFrequency,
        .parentGroupId = ModuleIds::kDco2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 14,
        .sysExId = 10
    },
    {
        .parameterId = ParameterIds::kDco2FrequencyModByLfo1,
        .displayName = DisplayNames::PatchEdit::Dco2::ParameterWidgets::kFrequencyModByLfo1,
        .parentGroupId = ModuleIds::kDco2,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 88,
        .sysExId = 11
    },
    {
        .parameterId = ParameterIds::kDco2Detune,
        .displayName = DisplayNames::PatchEdit::Dco2::ParameterWidgets::kDetune,
        .parentGroupId = ModuleIds::kDco2,
        .minValue = -31,
        .maxValue = 31,
        .defaultValue = 2,
        .sysExOffset = 19,
        .sysExId = 12
    },
    {
        .parameterId = ParameterIds::kDco2PulseWidth,
        .displayName = DisplayNames::PatchEdit::Dco2::ParameterWidgets::kPulseWidth,
        .parentGroupId = ModuleIds::kDco2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 24,
        .sysExOffset = 16,
        .sysExId = 13
    },
    {
        .parameterId = ParameterIds::kDco2PulseWidthModByLfo2,
        .displayName = DisplayNames::PatchEdit::Dco2::ParameterWidgets::kPulseWidthModByLfo2,
        .parentGroupId = ModuleIds::kDco2,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 89,
        .sysExId = 14
    },
    {
        .parameterId = ParameterIds::kDco2WaveShape,
        .displayName = DisplayNames::PatchEdit::Dco2::ParameterWidgets::kWaveShape,
        .parentGroupId = ModuleIds::kDco2,
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
        .parameterId = ParameterIds::kDco2WaveSelect,
        .displayName = DisplayNames::PatchEdit::Dco2::ParameterWidgets::kWaveSelect,
        .parentGroupId = ModuleIds::kDco2,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::WaveSelect::kOff, DisplayNames::ChoiceLists::WaveSelect::kPulse, DisplayNames::ChoiceLists::WaveSelect::kWave, DisplayNames::ChoiceLists::WaveSelect::kBoth, DisplayNames::ChoiceLists::WaveSelect::kNoise }),
        .defaultIndex = 1,
        .sysExOffset = 18,
        .sysExId = 16
    },
    {
        .parameterId = ParameterIds::kDco2Levers,
        .displayName = DisplayNames::PatchEdit::Dco2::ParameterWidgets::kLevers,
        .parentGroupId = ModuleIds::kDco2,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::Levers::kOff, DisplayNames::ChoiceLists::Levers::kL1Bend, DisplayNames::ChoiceLists::Levers::kL2Vib, DisplayNames::ChoiceLists::Levers::kBoth }),
        .defaultIndex = 3,
        .sysExOffset = 17,
        .sysExId = 17
    },
    {
        .parameterId = ParameterIds::kDco2KeyboardPortamento,
        .displayName = DisplayNames::PatchEdit::Dco2::ParameterWidgets::kKeyboardPortamento,
        .parentGroupId = ModuleIds::kDco2,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::KeyboardPortamento::kOff, DisplayNames::ChoiceLists::KeyboardPortamento::kPorta, DisplayNames::ChoiceLists::KeyboardPortamento::kKeybd }),
        .defaultIndex = 2,
        .sysExOffset = 23,
        .sysExId = 18
    },
    {
        .parameterId = ParameterIds::kDco2KeyClick,
        .displayName = DisplayNames::PatchEdit::Dco2::ParameterWidgets::kKeyClick,
        .parentGroupId = ModuleIds::kDco2,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::OnOff::kOff, DisplayNames::ChoiceLists::OnOff::kOn }),
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
        .parameterId = ParameterIds::kVcfBalance,
        .displayName = DisplayNames::PatchEdit::VcfVca::ParameterWidgets::kBalance,
        .parentGroupId = ModuleIds::kVcfVca,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 31,
        .sysExOffset = 20,
        .sysExId = 20
    },
    {
        .parameterId = ParameterIds::kVcfFequency,
        .displayName = DisplayNames::PatchEdit::VcfVca::ParameterWidgets::kFrequency,
        .parentGroupId = ModuleIds::kVcfVca,
        .minValue = 0,
        .maxValue = 127,
        .defaultValue = 55,
        .sysExOffset = 26,
        .sysExId = 21
    },
    {
        .parameterId = ParameterIds::kVcfFrequencyModByEnv1,
        .displayName = DisplayNames::PatchEdit::VcfVca::ParameterWidgets::kFrequencyModByEnv1,
        .parentGroupId = ModuleIds::kVcfVca,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 42,
        .sysExOffset = 90,
        .sysExId = 22
    },
    {
        .parameterId = ParameterIds::kVcfFrequencyModByPressure,
        .displayName = DisplayNames::PatchEdit::VcfVca::ParameterWidgets::kFrequencyModByPressure,
        .parentGroupId = ModuleIds::kVcfVca,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 91,
        .sysExId = 23
    },
    {
        .parameterId = ParameterIds::kVcfResonance,
        .displayName = DisplayNames::PatchEdit::VcfVca::ParameterWidgets::kResonance,
        .parentGroupId = ModuleIds::kVcfVca,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 27,
        .sysExId = 24
    },
    {
        .parameterId = ParameterIds::kVca1Volume,
        .displayName = DisplayNames::PatchEdit::VcfVca::ParameterWidgets::kVca1Volume,
        .parentGroupId = ModuleIds::kVcfVca,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 63,
        .sysExOffset = 31,
        .sysExId = 27
    },
    {
        .parameterId = ParameterIds::kVca1ModByVelocity,
        .displayName = DisplayNames::PatchEdit::VcfVca::ParameterWidgets::kVca1ModByVelocity,
        .parentGroupId = ModuleIds::kVcfVca,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 92,
        .sysExId = 28
    },
    {
        .parameterId = ParameterIds::kVca2ModByEnv2,
        .displayName = DisplayNames::PatchEdit::VcfVca::ParameterWidgets::kVca2ModByEnv2,
        .parentGroupId = ModuleIds::kVcfVca,
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
        .parameterId = ParameterIds::kVcfLevers,
        .displayName = DisplayNames::PatchEdit::VcfVca::ParameterWidgets::kLevers,
        .parentGroupId = ModuleIds::kVcfVca,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::Levers::kOff, DisplayNames::ChoiceLists::Levers::kL1Bend, DisplayNames::ChoiceLists::Levers::kL2Vib, DisplayNames::ChoiceLists::Levers::kBoth }),
        .defaultIndex = 0,
        .sysExOffset = 28,
        .sysExId = 25
    },
    {
        .parameterId = ParameterIds::kVcfKeyboardPortamento,
        .displayName = DisplayNames::PatchEdit::VcfVca::ParameterWidgets::kKeyboardPortamento,
        .parentGroupId = ModuleIds::kVcfVca,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::KeyboardPortamento::kOff, DisplayNames::ChoiceLists::KeyboardPortamento::kPorta, DisplayNames::ChoiceLists::KeyboardPortamento::kKeybd }),
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
        .parameterId = ParameterIds::kFmAmount,
        .displayName = DisplayNames::PatchEdit::FmTrack::ParameterWidgets::kFmAmount,
        .parentGroupId = ModuleIds::kFmTrack,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 30,
        .sysExId = 30
    },
    {
        .parameterId = ParameterIds::kFmModByEnv3,
        .displayName = DisplayNames::PatchEdit::FmTrack::ParameterWidgets::kFmModByEnv3,
        .parentGroupId = ModuleIds::kFmTrack,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 100,
        .sysExId = 31
    },
    {
        .parameterId = ParameterIds::kFmModByPressure,
        .displayName = DisplayNames::PatchEdit::FmTrack::ParameterWidgets::kFmModByPressure,
        .parentGroupId = ModuleIds::kFmTrack,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 101,
        .sysExId = 32
    },
    {
        .parameterId = ParameterIds::kTrackPoint1,
        .displayName = DisplayNames::PatchEdit::FmTrack::ParameterWidgets::kTrackPoint1,
        .parentGroupId = ModuleIds::kFmTrack,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 77,
        .sysExId = 34
    },
    {
        .parameterId = ParameterIds::kTrackPoint2,
        .displayName = DisplayNames::PatchEdit::FmTrack::ParameterWidgets::kTrackPoint2,
        .parentGroupId = ModuleIds::kFmTrack,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 15,
        .sysExOffset = 78,
        .sysExId = 35
    },
    {
        .parameterId = ParameterIds::kTrackPoint3,
        .displayName = DisplayNames::PatchEdit::FmTrack::ParameterWidgets::kTrackPoint3,
        .parentGroupId = ModuleIds::kFmTrack,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 31,
        .sysExOffset = 79,
        .sysExId = 36
    },
    {
        .parameterId = ParameterIds::kTrackPoint4,
        .displayName = DisplayNames::PatchEdit::FmTrack::ParameterWidgets::kTrackPoint4,
        .parentGroupId = ModuleIds::kFmTrack,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 47,
        .sysExOffset = 80,
        .sysExId = 37
    },
    {
        .parameterId = ParameterIds::kTrackPoint5,
        .displayName = DisplayNames::PatchEdit::FmTrack::ParameterWidgets::kTrackPoint5,
        .parentGroupId = ModuleIds::kFmTrack,
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
        .parameterId = ParameterIds::kTrackInput,
        .displayName = DisplayNames::PatchEdit::FmTrack::ParameterWidgets::kTrackInput,
        .parentGroupId = ModuleIds::kFmTrack,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::ModulationBus::Source::kNone, DisplayNames::ChoiceLists::ModulationBus::Source::kEnv1, DisplayNames::ChoiceLists::ModulationBus::Source::kEnv2, DisplayNames::ChoiceLists::ModulationBus::Source::kEnv3, 
                                     DisplayNames::ChoiceLists::ModulationBus::Source::kLfo1, DisplayNames::ChoiceLists::ModulationBus::Source::kLfo2, DisplayNames::ChoiceLists::ModulationBus::Source::kVibrato, DisplayNames::ChoiceLists::ModulationBus::Source::kRamp1, DisplayNames::ChoiceLists::ModulationBus::Source::kRamp2,
                                     DisplayNames::ChoiceLists::ModulationBus::Source::kKeybd, DisplayNames::ChoiceLists::ModulationBus::Source::kPorta, DisplayNames::ChoiceLists::ModulationBus::Source::kTrack, DisplayNames::ChoiceLists::ModulationBus::Source::kKbGate, 
                                     DisplayNames::ChoiceLists::ModulationBus::Source::kVelocity, DisplayNames::ChoiceLists::ModulationBus::Source::kRelVel, DisplayNames::ChoiceLists::ModulationBus::Source::kPressure,
                                     DisplayNames::ChoiceLists::ModulationBus::Source::kPedal1, DisplayNames::ChoiceLists::ModulationBus::Source::kPedal2, DisplayNames::ChoiceLists::ModulationBus::Source::kLever1, DisplayNames::ChoiceLists::ModulationBus::Source::kLever2, DisplayNames::ChoiceLists::ModulationBus::Source::kLever3 }),
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
        .parameterId = ParameterIds::kRamp1Rate,
        .displayName = DisplayNames::PatchEdit::RampPortamento::ParameterWidgets::kRamp1Rate,
        .parentGroupId = ModuleIds::kRampPortamento,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 82,
        .sysExId = 40
    },
    {
        .parameterId = ParameterIds::kRamp2Rate,
        .displayName = DisplayNames::PatchEdit::RampPortamento::ParameterWidgets::kRamp2Rate,
        .parentGroupId = ModuleIds::kRampPortamento,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 84,
        .sysExId = 42
    },
    {
        .parameterId = ParameterIds::kPortamentoRate,
        .displayName = DisplayNames::PatchEdit::RampPortamento::ParameterWidgets::kPortamentoRate,
        .parentGroupId = ModuleIds::kRampPortamento,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 32,
        .sysExId = 44
    },
    {
        .parameterId = ParameterIds::kPortamentoModByVelocity,
        .displayName = DisplayNames::PatchEdit::RampPortamento::ParameterWidgets::kPortamentoModByVelocity,
        .parentGroupId = ModuleIds::kRampPortamento,
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
        .parameterId = ParameterIds::kRamp1Trigger,
        .displayName = DisplayNames::PatchEdit::RampPortamento::ParameterWidgets::kRamp1Trigger,
        .parentGroupId = ModuleIds::kRampPortamento,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::RampTrigger::kStrig, DisplayNames::ChoiceLists::RampTrigger::kMtrig, DisplayNames::ChoiceLists::RampTrigger::kExtrig, DisplayNames::ChoiceLists::RampTrigger::kGatedx }),
        .defaultIndex = 0,
        .sysExOffset = 83,
        .sysExId = 41
    },
    {
        .parameterId = ParameterIds::kRamp2Trigger,
        .displayName = DisplayNames::PatchEdit::RampPortamento::ParameterWidgets::kRamp2Trigger,
        .parentGroupId = ModuleIds::kRampPortamento,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::RampTrigger::kStrig, DisplayNames::ChoiceLists::RampTrigger::kMtrig, DisplayNames::ChoiceLists::RampTrigger::kExtrig, DisplayNames::ChoiceLists::RampTrigger::kGatedx }),
        .defaultIndex = 0,
        .sysExOffset = 85,
        .sysExId = 43
    },
    {
        .parameterId = ParameterIds::kPortamentoMode,
        .displayName = DisplayNames::PatchEdit::RampPortamento::ParameterWidgets::kPortamentoMode,
        .parentGroupId = ModuleIds::kRampPortamento,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::PortamentoMode::kLinear, DisplayNames::ChoiceLists::PortamentoMode::kConst, DisplayNames::ChoiceLists::PortamentoMode::kExpo }),
        .defaultIndex = 0,
        .sysExOffset = 33,
        .sysExId = 46
    },
    {
        .parameterId = ParameterIds::kPortamentoLegato,
        .displayName = DisplayNames::PatchEdit::RampPortamento::ParameterWidgets::kPortamentoLegato,
        .parentGroupId = ModuleIds::kRampPortamento,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::OnOff::kOff, DisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 34,
        .sysExId = 47
    },
    {
        .parameterId = ParameterIds::kPortamentoKeyboardMode,
        .displayName = DisplayNames::PatchEdit::RampPortamento::ParameterWidgets::kPortamentoKeyboardMode,
        .parentGroupId = ModuleIds::kRampPortamento,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::PortamentoKeyboardMode::kRotate, DisplayNames::ChoiceLists::PortamentoKeyboardMode::kReasgn, 
                                     DisplayNames::ChoiceLists::PortamentoKeyboardMode::kUnison, DisplayNames::ChoiceLists::PortamentoKeyboardMode::kRearob }),
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
        .parameterId = ParameterIds::kEnv1Delay,
        .displayName = DisplayNames::PatchEdit::Envelope1::ParameterWidgets::kDelay,
        .parentGroupId = ModuleIds::kEnvelope1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 50,
        .sysExId = 50
    },
    {
        .parameterId = ParameterIds::kEnv1Attack,
        .displayName = DisplayNames::PatchEdit::Envelope1::ParameterWidgets::kAttack,
        .parentGroupId = ModuleIds::kEnvelope1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 51,
        .sysExId = 51
    },
    {
        .parameterId = ParameterIds::kEnv1Decay,
        .displayName = DisplayNames::PatchEdit::Envelope1::ParameterWidgets::kDecay,
        .parentGroupId = ModuleIds::kEnvelope1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 10,
        .sysExOffset = 52,
        .sysExId = 52
    },
    {
        .parameterId = ParameterIds::kEnv1Sustain,
        .displayName = DisplayNames::PatchEdit::Envelope1::ParameterWidgets::kSustain,
        .parentGroupId = ModuleIds::kEnvelope1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 50,
        .sysExOffset = 53,
        .sysExId = 53
    },
    {
        .parameterId = ParameterIds::kEnv1Release,
        .displayName = DisplayNames::PatchEdit::Envelope1::ParameterWidgets::kRelease,
        .parentGroupId = ModuleIds::kEnvelope1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 10,
        .sysExOffset = 54,
        .sysExId = 54
    },
    {
        .parameterId = ParameterIds::kEnv1Amplitude,
        .displayName = DisplayNames::PatchEdit::Envelope1::ParameterWidgets::kAmplitude,
        .parentGroupId = ModuleIds::kEnvelope1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 40,
        .sysExOffset = 55,
        .sysExId = 55
    },
    {
        .parameterId = ParameterIds::kEnv1AmplitudeModByVelocity,
        .displayName = DisplayNames::PatchEdit::Envelope1::ParameterWidgets::kAmplitudeModByVelocity,
        .parentGroupId = ModuleIds::kEnvelope1,
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
        .parameterId = ParameterIds::kEnv1TriggerMode,
        .displayName = DisplayNames::PatchEdit::Envelope1::ParameterWidgets::kTriggerMode,
        .parentGroupId = ModuleIds::kEnvelope1,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::TriggerMode::kStrig, DisplayNames::ChoiceLists::TriggerMode::kMtrig, DisplayNames::ChoiceLists::TriggerMode::kSreset, 
                                     DisplayNames::ChoiceLists::TriggerMode::kMreset, DisplayNames::ChoiceLists::TriggerMode::kXtrig, DisplayNames::ChoiceLists::TriggerMode::kXmtrig, 
                                     DisplayNames::ChoiceLists::TriggerMode::kXreset, DisplayNames::ChoiceLists::TriggerMode::kXmrst }),
        .defaultIndex = 0,
        .sysExOffset = 49,
        .sysExId = 57
    },
    {
        .parameterId = ParameterIds::kEnv1EnvelopeMode,
        .displayName = DisplayNames::PatchEdit::Envelope1::ParameterWidgets::kEnvelopeMode,
        .parentGroupId = ModuleIds::kEnvelope1,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::EnvelopeMode::kNormal, DisplayNames::ChoiceLists::EnvelopeMode::kDadr, DisplayNames::ChoiceLists::EnvelopeMode::kFree, DisplayNames::ChoiceLists::EnvelopeMode::kBoth }),
        .defaultIndex = 0,
        .sysExOffset = 57,
        .sysExId = 58
    },
    {
        .parameterId = ParameterIds::kEnv1Lfo1Trigger,
        .displayName = DisplayNames::PatchEdit::Envelope1::ParameterWidgets::kLfo1Trigger,
        .parentGroupId = ModuleIds::kEnvelope1,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::Lfo1Trigger::kNormal, DisplayNames::ChoiceLists::Lfo1Trigger::kLfo1, DisplayNames::ChoiceLists::Lfo1Trigger::kGLfo1 }),
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
        .parameterId = ParameterIds::kEnv2Delay,
        .displayName = DisplayNames::PatchEdit::Envelope2::ParameterWidgets::kDelay,
        .parentGroupId = ModuleIds::kEnvelope2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 59,
        .sysExId = 60
    },
    {
        .parameterId = ParameterIds::kEnv2Attack,
        .displayName = DisplayNames::PatchEdit::Envelope2::ParameterWidgets::kAttack,
        .parentGroupId = ModuleIds::kEnvelope2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 60,
        .sysExId = 61
    },
    {
        .parameterId = ParameterIds::kEnv2Decay,
        .displayName = DisplayNames::PatchEdit::Envelope2::ParameterWidgets::kDecay,
        .parentGroupId = ModuleIds::kEnvelope2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 10,
        .sysExOffset = 61,
        .sysExId = 62
    },
    {
        .parameterId = ParameterIds::kEnv2Sustain,
        .displayName = DisplayNames::PatchEdit::Envelope2::ParameterWidgets::kSustain,
        .parentGroupId = ModuleIds::kEnvelope2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 50,
        .sysExOffset = 62,
        .sysExId = 63
    },
    {
        .parameterId = ParameterIds::kEnv2Release,
        .displayName = DisplayNames::PatchEdit::Envelope2::ParameterWidgets::kRelease,
        .parentGroupId = ModuleIds::kEnvelope2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 10,
        .sysExOffset = 63,
        .sysExId = 64
    },
    {
        .parameterId = ParameterIds::kEnv2Amplitude,
        .displayName = DisplayNames::PatchEdit::Envelope2::ParameterWidgets::kAmplitude,
        .parentGroupId = ModuleIds::kEnvelope2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 40,
        .sysExOffset = 64,
        .sysExId = 65
    },
    {
        .parameterId = ParameterIds::kEnv2AmplitudeModByVelocity,
        .displayName = DisplayNames::PatchEdit::Envelope2::ParameterWidgets::kAmplitudeModByVelocity,
        .parentGroupId = ModuleIds::kEnvelope2,
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
        .parameterId = ParameterIds::kEnv2TriggerMode,
        .displayName = DisplayNames::PatchEdit::Envelope2::ParameterWidgets::kTriggerMode,
        .parentGroupId = ModuleIds::kEnvelope2,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::TriggerMode::kStrig, DisplayNames::ChoiceLists::TriggerMode::kMtrig, DisplayNames::ChoiceLists::TriggerMode::kSreset, 
                                     DisplayNames::ChoiceLists::TriggerMode::kMreset, DisplayNames::ChoiceLists::TriggerMode::kXtrig, DisplayNames::ChoiceLists::TriggerMode::kXmtrig, 
                                     DisplayNames::ChoiceLists::TriggerMode::kXreset, DisplayNames::ChoiceLists::TriggerMode::kXmrst }),
        .defaultIndex = 0,
        .sysExOffset = 58,
        .sysExId = 67
    },
    {
        .parameterId = ParameterIds::kEnv2EnvelopeMode,
        .displayName = DisplayNames::PatchEdit::Envelope2::ParameterWidgets::kEnvelopeMode,
        .parentGroupId = ModuleIds::kEnvelope2,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::EnvelopeMode::kNormal, DisplayNames::ChoiceLists::EnvelopeMode::kDadr, DisplayNames::ChoiceLists::EnvelopeMode::kFree, DisplayNames::ChoiceLists::EnvelopeMode::kBoth }),
        .defaultIndex = 0,
        .sysExOffset = 66,
        .sysExId = 68
    },
    {
        .parameterId = ParameterIds::kEnv2Lfo1Trigger,
        .displayName = DisplayNames::PatchEdit::Envelope2::ParameterWidgets::kLfo1Trigger,
        .parentGroupId = ModuleIds::kEnvelope2,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::Lfo1Trigger::kNormal, DisplayNames::ChoiceLists::Lfo1Trigger::kLfo1, DisplayNames::ChoiceLists::Lfo1Trigger::kGLfo1 }),
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
        .parameterId = ParameterIds::kEnv3Delay,
        .displayName = DisplayNames::PatchEdit::Envelope3::ParameterWidgets::kDelay,
        .parentGroupId = ModuleIds::kEnvelope3,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 68,
        .sysExId = 70
    },
    {
        .parameterId = ParameterIds::kEnv3Attack,
        .displayName = DisplayNames::PatchEdit::Envelope3::ParameterWidgets::kAttack,
        .parentGroupId = ModuleIds::kEnvelope3,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 69,
        .sysExId = 71
    },
    {
        .parameterId = ParameterIds::kEnv3Decay,
        .displayName = DisplayNames::PatchEdit::Envelope3::ParameterWidgets::kDecay,
        .parentGroupId = ModuleIds::kEnvelope3,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 20,
        .sysExOffset = 70,
        .sysExId = 72
    },
    {
        .parameterId = ParameterIds::kEnv3Sustain,
        .displayName = DisplayNames::PatchEdit::Envelope3::ParameterWidgets::kSustain,
        .parentGroupId = ModuleIds::kEnvelope3,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 71,
        .sysExId = 73
    },
    {
        .parameterId = ParameterIds::kEnv3Release,
        .displayName = DisplayNames::PatchEdit::Envelope3::ParameterWidgets::kRelease,
        .parentGroupId = ModuleIds::kEnvelope3,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 20,
        .sysExOffset = 72,
        .sysExId = 74
    },
    {
        .parameterId = ParameterIds::kEnv3Amplitude,
        .displayName = DisplayNames::PatchEdit::Envelope3::ParameterWidgets::kAmplitude,
        .parentGroupId = ModuleIds::kEnvelope3,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 40,
        .sysExOffset = 73,
        .sysExId = 75
    },
    {
        .parameterId = ParameterIds::kEnv3AmplitudeModByVelocity,
        .displayName = DisplayNames::PatchEdit::Envelope3::ParameterWidgets::kAmplitudeModByVelocity,
        .parentGroupId = ModuleIds::kEnvelope3,
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
        .parameterId = ParameterIds::kEnv3TriggerMode,
        .displayName = DisplayNames::PatchEdit::Envelope3::ParameterWidgets::kTriggerMode,
        .parentGroupId = ModuleIds::kEnvelope3,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::TriggerMode::kStrig, DisplayNames::ChoiceLists::TriggerMode::kMtrig, DisplayNames::ChoiceLists::TriggerMode::kSreset, 
                                     DisplayNames::ChoiceLists::TriggerMode::kMreset, DisplayNames::ChoiceLists::TriggerMode::kXtrig, DisplayNames::ChoiceLists::TriggerMode::kXmtrig, 
                                     DisplayNames::ChoiceLists::TriggerMode::kXreset, DisplayNames::ChoiceLists::TriggerMode::kXmrst }),
        .defaultIndex = 0,
        .sysExOffset = 67,
        .sysExId = 77
    },
    {
        .parameterId = ParameterIds::kEnv3EnvelopeMode,
        .displayName = DisplayNames::PatchEdit::Envelope3::ParameterWidgets::kEnvelopeMode,
        .parentGroupId = ModuleIds::kEnvelope3,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::EnvelopeMode::kNormal, DisplayNames::ChoiceLists::EnvelopeMode::kDadr, DisplayNames::ChoiceLists::EnvelopeMode::kFree, DisplayNames::ChoiceLists::EnvelopeMode::kBoth }),
        .defaultIndex = 0,
        .sysExOffset = 75,
        .sysExId = 78
    },
    {
        .parameterId = ParameterIds::kEnv3Lfo1Trigger,
        .displayName = DisplayNames::PatchEdit::Envelope3::ParameterWidgets::kLfo1Trigger,
        .parentGroupId = ModuleIds::kEnvelope3,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::Lfo1Trigger::kNormal, DisplayNames::ChoiceLists::Lfo1Trigger::kLfo1, DisplayNames::ChoiceLists::Lfo1Trigger::kGatedLfo1Trigger }),
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
        .parameterId = ParameterIds::kLfo1Speed,
        .displayName = DisplayNames::PatchEdit::Lfo1::ParameterWidgets::kSpeed,
        .parentGroupId = ModuleIds::kLfo1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 40,
        .sysExOffset = 35,
        .sysExId = 80
    },
    {
        .parameterId = ParameterIds::kLfo1SpeedModByPressure,
        .displayName = DisplayNames::PatchEdit::Lfo1::ParameterWidgets::kSpeedModByPressure,
        .parentGroupId = ModuleIds::kLfo1,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 102,
        .sysExId = 81
    },
    {
        .parameterId = ParameterIds::kLfo1RetriggerPoint,
        .displayName = DisplayNames::PatchEdit::Lfo1::ParameterWidgets::kRetriggerPoint,
        .parentGroupId = ModuleIds::kLfo1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 39,
        .sysExId = 83
    },
    {
        .parameterId = ParameterIds::kLfo1Amplitude,
        .displayName = DisplayNames::PatchEdit::Lfo1::ParameterWidgets::kAmplitude,
        .parentGroupId = ModuleIds::kLfo1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 41,
        .sysExId = 84
    },
    {
        .parameterId = ParameterIds::kLfo1AmplitudeModByRamp1,
        .displayName = DisplayNames::PatchEdit::Lfo1::ParameterWidgets::kAmplitudeModByRamp1,
        .parentGroupId = ModuleIds::kLfo1,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 63,
        .sysExOffset = 97,
        .sysExId = 85
    }
};

const std::vector<ChoiceParameterDescriptor> kLfo1ChoiceParameters = {
    {
        .parameterId = ParameterIds::kLfo1Waveform,
        .displayName = DisplayNames::PatchEdit::Lfo1::ParameterWidgets::kWaveform,
        .parentGroupId = ModuleIds::kLfo1,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::LfoWaveform::kTriangle, DisplayNames::ChoiceLists::LfoWaveform::kUpsaw, DisplayNames::ChoiceLists::LfoWaveform::kDnsaw, 
                                     DisplayNames::ChoiceLists::LfoWaveform::kSquare, DisplayNames::ChoiceLists::LfoWaveform::kRandom, DisplayNames::ChoiceLists::LfoWaveform::kNoise, DisplayNames::ChoiceLists::LfoWaveform::kSampled }),
        .defaultIndex = 0,
        .sysExOffset = 38,
        .sysExId = 82
    },
    {
        .parameterId = ParameterIds::kLfo1TriggerMode,
        .displayName = DisplayNames::PatchEdit::Lfo1::ParameterWidgets::kTriggerMode,
        .parentGroupId = ModuleIds::kLfo1,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::LfoTriggerMode::kOff, DisplayNames::ChoiceLists::LfoTriggerMode::kStrig, DisplayNames::ChoiceLists::LfoTriggerMode::kMtrig, DisplayNames::ChoiceLists::LfoTriggerMode::kXtrig }),
        .defaultIndex = 0,
        .sysExOffset = 36,
        .sysExId = 86
    },
    {
        .parameterId = ParameterIds::kLfo1Lag,
        .displayName = DisplayNames::PatchEdit::Lfo1::ParameterWidgets::kLag,
        .parentGroupId = ModuleIds::kLfo1,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::OnOff::kOff, DisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 37,
        .sysExId = 87
    },
    {
        .parameterId = ParameterIds::kLfo1SampleInput,
        .displayName = DisplayNames::PatchEdit::Lfo1::ParameterWidgets::kSampleInput,
        .parentGroupId = ModuleIds::kLfo1,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::ModulationBus::Source::kNone, DisplayNames::ChoiceLists::ModulationBus::Source::kEnv1, DisplayNames::ChoiceLists::ModulationBus::Source::kEnv2, DisplayNames::ChoiceLists::ModulationBus::Source::kEnv3, 
                                     DisplayNames::ChoiceLists::ModulationBus::Source::kLfo1, DisplayNames::ChoiceLists::ModulationBus::Source::kLfo2, DisplayNames::ChoiceLists::ModulationBus::Source::kVibrato, DisplayNames::ChoiceLists::ModulationBus::Source::kRamp1, DisplayNames::ChoiceLists::ModulationBus::Source::kRamp2,
                                     DisplayNames::ChoiceLists::ModulationBus::Source::kKeybd, DisplayNames::ChoiceLists::ModulationBus::Source::kPorta, DisplayNames::ChoiceLists::ModulationBus::Source::kTrack, DisplayNames::ChoiceLists::ModulationBus::Source::kKbGate, 
                                     DisplayNames::ChoiceLists::ModulationBus::Source::kVelocity, DisplayNames::ChoiceLists::ModulationBus::Source::kRelVel, DisplayNames::ChoiceLists::ModulationBus::Source::kPressure,
                                     DisplayNames::ChoiceLists::ModulationBus::Source::kPedal1, DisplayNames::ChoiceLists::ModulationBus::Source::kPedal2, DisplayNames::ChoiceLists::ModulationBus::Source::kLever1, DisplayNames::ChoiceLists::ModulationBus::Source::kLever2, DisplayNames::ChoiceLists::ModulationBus::Source::kLever3 }),
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
        .parameterId = ParameterIds::kLfo2Speed,
        .displayName = DisplayNames::PatchEdit::Lfo2::ParameterWidgets::kSpeed,
        .parentGroupId = ModuleIds::kLfo2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 30,
        .sysExOffset = 42,
        .sysExId = 90
    },
    {
        .parameterId = ParameterIds::kLfo2SpeedModByKeyboard,
        .displayName = DisplayNames::PatchEdit::Lfo2::ParameterWidgets::kSpeedModByKeyboard,
        .parentGroupId = ModuleIds::kLfo2,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 103,
        .sysExId = 91
    },
    {
        .parameterId = ParameterIds::kLfo2RetriggerPoint,
        .displayName = DisplayNames::PatchEdit::Lfo2::ParameterWidgets::kRetriggerPoint,
        .parentGroupId = ModuleIds::kLfo2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 46,
        .sysExId = 93
    },
    {
        .parameterId = ParameterIds::kLfo2Amplitude,
        .displayName = DisplayNames::PatchEdit::Lfo2::ParameterWidgets::kAmplitude,
        .parentGroupId = ModuleIds::kLfo2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 48,
        .sysExId = 94
    },
    {
        .parameterId = ParameterIds::kLfo2AmplitudeModByRamp2,
        .displayName = DisplayNames::PatchEdit::Lfo2::ParameterWidgets::kAmplitudeModByRamp2,
        .parentGroupId = ModuleIds::kLfo2,
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
        .parameterId = ParameterIds::kLfo2Waveform,
        .displayName = DisplayNames::PatchEdit::Lfo2::ParameterWidgets::kWaveform,
        .parentGroupId = ModuleIds::kLfo2,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::LfoWaveform::kTriangle, DisplayNames::ChoiceLists::LfoWaveform::kUpsaw, DisplayNames::ChoiceLists::LfoWaveform::kDnsaw, 
                                     DisplayNames::ChoiceLists::LfoWaveform::kSquare, DisplayNames::ChoiceLists::LfoWaveform::kRandom, DisplayNames::ChoiceLists::LfoWaveform::kNoise, DisplayNames::ChoiceLists::LfoWaveform::kSampled }),
        .defaultIndex = 0,
        .sysExOffset = 45,
        .sysExId = 92
    },
    {
        .parameterId = ParameterIds::kLfo2TriggerMode,
        .displayName = DisplayNames::PatchEdit::Lfo2::ParameterWidgets::kTriggerMode,
        .parentGroupId = ModuleIds::kLfo2,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::LfoTriggerMode::kOff, DisplayNames::ChoiceLists::LfoTriggerMode::kStrig, DisplayNames::ChoiceLists::LfoTriggerMode::kMtrig, DisplayNames::ChoiceLists::LfoTriggerMode::kXtrig }),
        .defaultIndex = 0,
        .sysExOffset = 43,
        .sysExId = 96
    },
    {
        .parameterId = ParameterIds::kLfo2Lag,
        .displayName = DisplayNames::PatchEdit::Lfo2::ParameterWidgets::kLag,
        .parentGroupId = ModuleIds::kLfo2,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::OnOff::kOff, DisplayNames::ChoiceLists::OnOff::kOn }),
        .defaultIndex = 0,
        .sysExOffset = 44,
        .sysExId = 97
    },
    {
        .parameterId = ParameterIds::kLfo2SampleInput,
        .displayName = DisplayNames::PatchEdit::Lfo2::ParameterWidgets::kSampleInput,
        .parentGroupId = ModuleIds::kLfo2,
        .choices = makeStringArray({ DisplayNames::ChoiceLists::ModulationBus::Source::kNone, DisplayNames::ChoiceLists::ModulationBus::Source::kEnv1, DisplayNames::ChoiceLists::ModulationBus::Source::kEnv2, DisplayNames::ChoiceLists::ModulationBus::Source::kEnv3, 
                                     DisplayNames::ChoiceLists::ModulationBus::Source::kLfo1, DisplayNames::ChoiceLists::ModulationBus::Source::kLfo2, DisplayNames::ChoiceLists::ModulationBus::Source::kVibrato, DisplayNames::ChoiceLists::ModulationBus::Source::kRamp1, DisplayNames::ChoiceLists::ModulationBus::Source::kRamp2,
                                     DisplayNames::ChoiceLists::ModulationBus::Source::kKeybd, DisplayNames::ChoiceLists::ModulationBus::Source::kPorta, DisplayNames::ChoiceLists::ModulationBus::Source::kTrack, DisplayNames::ChoiceLists::ModulationBus::Source::kKbGate, 
                                     DisplayNames::ChoiceLists::ModulationBus::Source::kVelocity, DisplayNames::ChoiceLists::ModulationBus::Source::kRelVel, DisplayNames::ChoiceLists::ModulationBus::Source::kPressure,
                                     DisplayNames::ChoiceLists::ModulationBus::Source::kPedal1, DisplayNames::ChoiceLists::ModulationBus::Source::kPedal2, DisplayNames::ChoiceLists::ModulationBus::Source::kLever1, DisplayNames::ChoiceLists::ModulationBus::Source::kLever2, DisplayNames::ChoiceLists::ModulationBus::Source::kLever3 }),
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
        DisplayNames::ChoiceLists::ModulationBus::Source::kNone, DisplayNames::ChoiceLists::ModulationBus::Source::kEnv1, DisplayNames::ChoiceLists::ModulationBus::Source::kEnv2, DisplayNames::ChoiceLists::ModulationBus::Source::kEnv3, 
        DisplayNames::ChoiceLists::ModulationBus::Source::kLfo1, DisplayNames::ChoiceLists::ModulationBus::Source::kLfo2, DisplayNames::ChoiceLists::ModulationBus::Source::kVibrato, DisplayNames::ChoiceLists::ModulationBus::Source::kRamp1, DisplayNames::ChoiceLists::ModulationBus::Source::kRamp2,
        DisplayNames::ChoiceLists::ModulationBus::Source::kKeybd, DisplayNames::ChoiceLists::ModulationBus::Source::kPorta, DisplayNames::ChoiceLists::ModulationBus::Source::kTrack, DisplayNames::ChoiceLists::ModulationBus::Source::kKbGate, 
        DisplayNames::ChoiceLists::ModulationBus::Source::kVelocity, DisplayNames::ChoiceLists::ModulationBus::Source::kRelVel, DisplayNames::ChoiceLists::ModulationBus::Source::kPressure,
        DisplayNames::ChoiceLists::ModulationBus::Source::kPedal1, DisplayNames::ChoiceLists::ModulationBus::Source::kPedal2, DisplayNames::ChoiceLists::ModulationBus::Source::kLever1, DisplayNames::ChoiceLists::ModulationBus::Source::kLever2, DisplayNames::ChoiceLists::ModulationBus::Source::kLever3
    });

    const juce::StringArray kDestinationChoices = makeStringArray({
        DisplayNames::ChoiceLists::ModulationBus::Destination::kNone, DisplayNames::ChoiceLists::ModulationBus::Destination::kDco1Frequency, DisplayNames::ChoiceLists::ModulationBus::Destination::kDco1PulseWidth, DisplayNames::ChoiceLists::ModulationBus::Destination::kDco1WaveShape,
        DisplayNames::ChoiceLists::ModulationBus::Destination::kDco2Frequency, DisplayNames::ChoiceLists::ModulationBus::Destination::kDco2PulseWidth, DisplayNames::ChoiceLists::ModulationBus::Destination::kDco2WaveShape,
        DisplayNames::ChoiceLists::ModulationBus::Destination::kDco1Dco2Mix, DisplayNames::ChoiceLists::ModulationBus::Destination::kVcfFmByDco1, DisplayNames::ChoiceLists::ModulationBus::Destination::kVcfFrequency, DisplayNames::ChoiceLists::ModulationBus::Destination::kVcfResonance,
        DisplayNames::ChoiceLists::ModulationBus::Destination::kVca1Volume, DisplayNames::ChoiceLists::ModulationBus::Destination::kVca2Volume,
        DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Delay, DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Attack, DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Decay, 
        DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Release, DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Amplitude,
        DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Delay, DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Attack, DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Decay, 
        DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Release, DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Amplitude,
        DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Delay, DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Attack, DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Decay, 
        DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Release, DisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Amplitude,
        DisplayNames::ChoiceLists::ModulationBus::Destination::kLfo1Speed, DisplayNames::ChoiceLists::ModulationBus::Destination::kLfo1Amplitude, DisplayNames::ChoiceLists::ModulationBus::Destination::kLfo2Speed, DisplayNames::ChoiceLists::ModulationBus::Destination::kLfo2Amplitude,
        DisplayNames::ChoiceLists::ModulationBus::Destination::kPortamentoRate
    });
}

// ============================================================================
// Data Definitions | Matrix Modulation | Choices Parameters
// ============================================================================

const std::array<std::vector<ChoiceParameterDescriptor>, kModulationBusCount> kModulationBusChoiceParameters = {{
    // Modulation Bus 0
    {
        {
            .parameterId = ParameterIds::kModulationBus0Source,
            .displayName = DisplayNames::MatrixModulation::Header::kSource,
            .parentGroupId = ModulationBusIds::kModulationBus0,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 104,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus0Destination,
            .displayName = DisplayNames::MatrixModulation::Header::kDestination,
            .parentGroupId = ModulationBusIds::kModulationBus0,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 106,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 1
    {
        {
            .parameterId = ParameterIds::kModulationBus1Source,
            .displayName = DisplayNames::MatrixModulation::Header::kSource,
            .parentGroupId = ModulationBusIds::kModulationBus1,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 107,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus1Destination,
            .displayName = DisplayNames::MatrixModulation::Header::kDestination,
            .parentGroupId = ModulationBusIds::kModulationBus1,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 109,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 2
    {
        {
            .parameterId = ParameterIds::kModulationBus2Source,
            .displayName = DisplayNames::MatrixModulation::Header::kSource,
            .parentGroupId = ModulationBusIds::kModulationBus2,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 110,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus2Destination,
            .displayName = DisplayNames::MatrixModulation::Header::kDestination,
            .parentGroupId = ModulationBusIds::kModulationBus2,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 112,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 3
    {
        {
            .parameterId = ParameterIds::kModulationBus3Source,
            .displayName = DisplayNames::MatrixModulation::Header::kSource,
            .parentGroupId = ModulationBusIds::kModulationBus3,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 113,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus3Destination,
            .displayName = DisplayNames::MatrixModulation::Header::kDestination,
            .parentGroupId = ModulationBusIds::kModulationBus3,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 115,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 4
    {
        {
            .parameterId = ParameterIds::kModulationBus4Source,
            .displayName = DisplayNames::MatrixModulation::Header::kSource,
            .parentGroupId = ModulationBusIds::kModulationBus4,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 116,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus4Destination,
            .displayName = DisplayNames::MatrixModulation::Header::kDestination,
            .parentGroupId = ModulationBusIds::kModulationBus4,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 118,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 5
    {
        {
            .parameterId = ParameterIds::kModulationBus5Source,
            .displayName = DisplayNames::MatrixModulation::Header::kSource,
            .parentGroupId = ModulationBusIds::kModulationBus5,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 119,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus5Destination,
            .displayName = DisplayNames::MatrixModulation::Header::kDestination,
            .parentGroupId = ModulationBusIds::kModulationBus5,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 121,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 6
    {
        {
            .parameterId = ParameterIds::kModulationBus6Source,
            .displayName = DisplayNames::MatrixModulation::Header::kSource,
            .parentGroupId = ModulationBusIds::kModulationBus6,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 122,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus6Destination,
            .displayName = DisplayNames::MatrixModulation::Header::kDestination,
            .parentGroupId = ModulationBusIds::kModulationBus6,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 124,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 7
    {
        {
            .parameterId = ParameterIds::kModulationBus7Source,
            .displayName = DisplayNames::MatrixModulation::Header::kSource,
            .parentGroupId = ModulationBusIds::kModulationBus7,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 125,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus7Destination,
            .displayName = DisplayNames::MatrixModulation::Header::kDestination,
            .parentGroupId = ModulationBusIds::kModulationBus7,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 127,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 8
    {
        {
            .parameterId = ParameterIds::kModulationBus8Source,
            .displayName = DisplayNames::MatrixModulation::Header::kSource,
            .parentGroupId = ModulationBusIds::kModulationBus8,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 128,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus8Destination,
            .displayName = DisplayNames::MatrixModulation::Header::kDestination,
            .parentGroupId = ModulationBusIds::kModulationBus8,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 130,
            .sysExId = kNoSysExId
        }
    },
    // Modulation Bus 9
    {
        {
            .parameterId = ParameterIds::kModulationBus9Source,
            .displayName = DisplayNames::MatrixModulation::Header::kSource,
            .parentGroupId = ModulationBusIds::kModulationBus9,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 131,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus9Destination,
            .displayName = DisplayNames::MatrixModulation::Header::kDestination,
            .parentGroupId = ModulationBusIds::kModulationBus9,
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

const std::array<std::vector<IntParameterDescriptor>, kModulationBusCount> kModulationBusIntParameters = {{
    // Modulation Bus 0
    {
        {
            .parameterId = ParameterIds::kModulationBus0Amount,
            .displayName = DisplayNames::MatrixModulation::Header::kAmount,
            .parentGroupId = ModulationBusIds::kModulationBus0,
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
            .parameterId = ParameterIds::kModulationBus1Amount,
            .displayName = DisplayNames::MatrixModulation::Header::kAmount,
            .parentGroupId = ModulationBusIds::kModulationBus1,
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
            .parameterId = ParameterIds::kModulationBus2Amount,
            .displayName = DisplayNames::MatrixModulation::Header::kAmount,
            .parentGroupId = ModulationBusIds::kModulationBus2,
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
            .parameterId = ParameterIds::kModulationBus3Amount,
            .displayName = DisplayNames::MatrixModulation::Header::kAmount,
            .parentGroupId = ModulationBusIds::kModulationBus3,
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
            .parameterId = ParameterIds::kModulationBus4Amount,
            .displayName = DisplayNames::MatrixModulation::Header::kAmount,
            .parentGroupId = ModulationBusIds::kModulationBus4,
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
            .parameterId = ParameterIds::kModulationBus5Amount,
            .displayName = DisplayNames::MatrixModulation::Header::kAmount,
            .parentGroupId = ModulationBusIds::kModulationBus5,
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
            .parameterId = ParameterIds::kModulationBus6Amount,
            .displayName = DisplayNames::MatrixModulation::Header::kAmount,
            .parentGroupId = ModulationBusIds::kModulationBus6,
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
            .parameterId = ParameterIds::kModulationBus7Amount,
            .displayName = DisplayNames::MatrixModulation::Header::kAmount,
            .parentGroupId = ModulationBusIds::kModulationBus7,
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
            .parameterId = ParameterIds::kModulationBus8Amount,
            .displayName = DisplayNames::MatrixModulation::Header::kAmount,
            .parentGroupId = ModulationBusIds::kModulationBus8,
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
            .parameterId = ParameterIds::kModulationBus9Amount,
            .displayName = DisplayNames::MatrixModulation::Header::kAmount,
            .parentGroupId = ModulationBusIds::kModulationBus9,
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
        .widgetId = StandaloneWidgetIds::kMidiInit,
        .displayName = DisplayNames::MasterEdit::Midi::StandaloneWidgets::kInit,
        .parentGroupId = ModuleIds::kMidi,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kVibratoInit,
        .displayName = DisplayNames::MasterEdit::Vibrato::StandaloneWidgets::kInit,
        .parentGroupId = ModuleIds::kVibrato,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kMiscInit,
        .displayName = DisplayNames::MasterEdit::Misc::StandaloneWidgets::kInit,
        .parentGroupId = ModuleIds::kMisc,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ============================================================================
// Data Definitions | Standalone Widgets | Patch Edit
// ============================================================================

// DCO 1 Module
const std::vector<StandaloneWidgetDescriptor> kDco1StandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kDco1Init,
        .displayName = DisplayNames::PatchEdit::Dco1::StandaloneWidgets::kInit,
        .parentGroupId = ModuleIds::kDco1,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kDco1Copy,
        .displayName = DisplayNames::PatchEdit::Dco1::StandaloneWidgets::kCopy,
        .parentGroupId = ModuleIds::kDco1,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kDco1Paste,
        .displayName = DisplayNames::PatchEdit::Dco1::StandaloneWidgets::kPaste,
        .parentGroupId = ModuleIds::kDco1,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// DCO 2 Module
const std::vector<StandaloneWidgetDescriptor> kDco2StandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kDco2Init,
        .displayName = DisplayNames::PatchEdit::Dco2::StandaloneWidgets::kInit,
        .parentGroupId = ModuleIds::kDco2,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kDco2Copy,
        .displayName = DisplayNames::PatchEdit::Dco2::StandaloneWidgets::kCopy,
        .parentGroupId = ModuleIds::kDco2,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kDco2Paste,
        .displayName = DisplayNames::PatchEdit::Dco2::StandaloneWidgets::kPaste,
        .parentGroupId = ModuleIds::kDco2,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// VCF/VCA Module
const std::vector<StandaloneWidgetDescriptor> kVcfVcaStandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kVcfVcaInit,
        .displayName = DisplayNames::PatchEdit::VcfVca::StandaloneWidgets::kInit,
        .parentGroupId = ModuleIds::kVcfVca,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// FM/TRACK Module
const std::vector<StandaloneWidgetDescriptor> kFmTrackStandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kFmTrackInit,
        .displayName = DisplayNames::PatchEdit::FmTrack::StandaloneWidgets::kInit,
        .parentGroupId = ModuleIds::kFmTrack,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// RAMP/PORTAMENTO Module
const std::vector<StandaloneWidgetDescriptor> kRampPortamentoStandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kRampPortamentoInit,
        .displayName = DisplayNames::PatchEdit::RampPortamento::StandaloneWidgets::kInit,
        .parentGroupId = ModuleIds::kRampPortamento,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ENV 1 Module
const std::vector<StandaloneWidgetDescriptor> kEnv1StandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kEnv1Init,
        .displayName = DisplayNames::PatchEdit::Envelope1::StandaloneWidgets::kInit,
        .parentGroupId = ModuleIds::kEnvelope1,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kEnv1Copy,
        .displayName = DisplayNames::PatchEdit::Envelope1::StandaloneWidgets::kCopy,
        .parentGroupId = ModuleIds::kEnvelope1,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kEnv1Paste,
        .displayName = DisplayNames::PatchEdit::Envelope1::StandaloneWidgets::kPaste,
        .parentGroupId = ModuleIds::kEnvelope1,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ENV 2 Module
const std::vector<StandaloneWidgetDescriptor> kEnv2StandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kEnv2Init,
        .displayName = DisplayNames::PatchEdit::Envelope2::StandaloneWidgets::kInit,
        .parentGroupId = ModuleIds::kEnvelope2,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kEnv2Copy,
        .displayName = DisplayNames::PatchEdit::Envelope2::StandaloneWidgets::kCopy,
        .parentGroupId = ModuleIds::kEnvelope2,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kEnv2Paste,
        .displayName = DisplayNames::PatchEdit::Envelope2::StandaloneWidgets::kPaste,
        .parentGroupId = ModuleIds::kEnvelope2,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ENV 3 Module
const std::vector<StandaloneWidgetDescriptor> kEnv3StandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kEnv3Init,
        .displayName = DisplayNames::PatchEdit::Envelope3::StandaloneWidgets::kInit,
        .parentGroupId = ModuleIds::kEnvelope3,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kEnv3Copy,
        .displayName = DisplayNames::PatchEdit::Envelope3::StandaloneWidgets::kCopy,
        .parentGroupId = ModuleIds::kEnvelope3,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kEnv3Paste,
        .displayName = DisplayNames::PatchEdit::Envelope3::StandaloneWidgets::kPaste,
        .parentGroupId = ModuleIds::kEnvelope3,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// LFO 1 Module
const std::vector<StandaloneWidgetDescriptor> kLfo1StandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kLfo1Init,
        .displayName = DisplayNames::PatchEdit::Lfo1::StandaloneWidgets::kInit,
        .parentGroupId = ModuleIds::kLfo1,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kLfo1Copy,
        .displayName = DisplayNames::PatchEdit::Lfo1::StandaloneWidgets::kCopy,
        .parentGroupId = ModuleIds::kLfo1,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kLfo1Paste,
        .displayName = DisplayNames::PatchEdit::Lfo1::StandaloneWidgets::kPaste,
        .parentGroupId = ModuleIds::kLfo1,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// LFO 2 Module
const std::vector<StandaloneWidgetDescriptor> kLfo2StandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kLfo2Init,
        .displayName = DisplayNames::PatchEdit::Lfo2::StandaloneWidgets::kInit,
        .parentGroupId = ModuleIds::kLfo2,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kLfo2Copy,
        .displayName = DisplayNames::PatchEdit::Lfo2::StandaloneWidgets::kCopy,
        .parentGroupId = ModuleIds::kLfo2,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kLfo2Paste,
        .displayName = DisplayNames::PatchEdit::Lfo2::StandaloneWidgets::kPaste,
        .parentGroupId = ModuleIds::kLfo2,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ============================================================================
// Data Definitions | Standalone Widgets | Patch Manager
// ============================================================================

// Bank Utility Module
const std::vector<StandaloneWidgetDescriptor> kBankUtilityWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kUnlockBank,
        .displayName = DisplayNames::PatchManager::BankUtility::StandaloneWidgets::kUnlockBank,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank0,
        .displayName = DisplayNames::PatchManager::BankUtility::StandaloneWidgets::kSelectBank0,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank1,
        .displayName = DisplayNames::PatchManager::BankUtility::StandaloneWidgets::kSelectBank1,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank2,
        .displayName = DisplayNames::PatchManager::BankUtility::StandaloneWidgets::kSelectBank2,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank3,
        .displayName = DisplayNames::PatchManager::BankUtility::StandaloneWidgets::kSelectBank3,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank4,
        .displayName = DisplayNames::PatchManager::BankUtility::StandaloneWidgets::kSelectBank4,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank5,
        .displayName = DisplayNames::PatchManager::BankUtility::StandaloneWidgets::kSelectBank5,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank6,
        .displayName = DisplayNames::PatchManager::BankUtility::StandaloneWidgets::kSelectBank6,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank7,
        .displayName = DisplayNames::PatchManager::BankUtility::StandaloneWidgets::kSelectBank7,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank8,
        .displayName = DisplayNames::PatchManager::BankUtility::StandaloneWidgets::kSelectBank8,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank9,
        .displayName = DisplayNames::PatchManager::BankUtility::StandaloneWidgets::kSelectBank9,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ============================================================================
// Patch Manager | Standalone Widgets
// ============================================================================

// Internal Patches Module
const std::vector<StandaloneWidgetDescriptor> kInternalPatchesWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kLoadPreviousPatch,
        .displayName = DisplayNames::PatchManager::InternalPatches::StandaloneWidgets::kLoadPreviousPatch,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kLoadNextPatch,
        .displayName = DisplayNames::PatchManager::InternalPatches::StandaloneWidgets::kLoadNextPatch,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kCurrentBankNumber,
        .displayName = DisplayNames::PatchManager::InternalPatches::StandaloneWidgets::kCurrentBankNumber,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kNumber
    },
    {
        .widgetId = StandaloneWidgetIds::kCurrentPatchNumber,
        .displayName = DisplayNames::PatchManager::InternalPatches::StandaloneWidgets::kCurrentPatchNumber,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kNumber
    },
    {
        .widgetId = StandaloneWidgetIds::kInitPatch,
        .displayName = DisplayNames::PatchManager::InternalPatches::StandaloneWidgets::kInitPatch,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kCopyPatch,
        .displayName = DisplayNames::PatchManager::InternalPatches::StandaloneWidgets::kCopyPatch,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPastePatch,
        .displayName = DisplayNames::PatchManager::InternalPatches::StandaloneWidgets::kPastePatch,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kStorePatch,
        .displayName = DisplayNames::PatchManager::InternalPatches::StandaloneWidgets::kStorePatch,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// Computer Patches Module
const std::vector<StandaloneWidgetDescriptor> kComputerPatchesWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kLoadPreviousPatchFile,
        .displayName = DisplayNames::PatchManager::ComputerPatches::StandaloneWidgets::kLoadPreviousPatchFile,
        .parentGroupId = ModuleIds::kComputerPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kLoadNextPatchFile,
        .displayName = DisplayNames::PatchManager::ComputerPatches::StandaloneWidgets::kLoadNextPatchFile,
        .parentGroupId = ModuleIds::kComputerPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectPatchFile,
        .displayName = DisplayNames::PatchManager::ComputerPatches::StandaloneWidgets::kSelectPatchFile,
        .parentGroupId = ModuleIds::kComputerPatches,
        .widgetType = StandaloneWidgetType::kComboBox
    },
    {
        .widgetId = StandaloneWidgetIds::kOpenPatchFolder,
        .displayName = DisplayNames::PatchManager::ComputerPatches::StandaloneWidgets::kOpenPatchFolder,
        .parentGroupId = ModuleIds::kComputerPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSavePatchAs,
        .displayName = DisplayNames::PatchManager::ComputerPatches::StandaloneWidgets::kSavePatchAsFile,
        .parentGroupId = ModuleIds::kComputerPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSavePatchFile,
        .displayName = DisplayNames::PatchManager::ComputerPatches::StandaloneWidgets::kSavePatchFile,
        .parentGroupId = ModuleIds::kComputerPatches,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// Patch Mutator Module
const std::vector<StandaloneWidgetDescriptor> kPatchMutatorWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorAmount,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kAmount,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kLabel
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorRandom,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kRandom,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kLabel
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorHistory,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kHistory,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kComboBox
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorMutate,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kMutate,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorRetry,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kRetry,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorCompare,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kCompare,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorDelete,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kDelete,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorClear,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kClear,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorExport,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kExport,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorDco1,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kDco1,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorDco2,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kDco2,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorVcfVca,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kVcfVca,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorFmTrack,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kFmTrack,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorRampPortamento,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kRampPortamento,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorEnv1,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kEnvelope1,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorEnv2,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kEnvelope2,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorEnv3,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kEnvelope3,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorLfo1,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kLfo1,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorLfo2,
        .displayName = DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kLfo2,
        .parentGroupId = ModuleIds::kPatchMutator,
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

