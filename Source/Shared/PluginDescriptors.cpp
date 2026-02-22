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
        .displayName = ModeDisplayNames::kMaster
    },
    {
        .parentId = kNoParentId,
        .groupId = ModeIds::kPatch,
        .displayName = ModeDisplayNames::kPatch
    },
    // Plugin Sections
    {
        .parentId = ModeIds::kMaster,
        .groupId = SectionIds::kMasterEdit,
        .displayName = SectionDisplayNames::kMasterEdit
    },
    {
        .parentId = ModeIds::kPatch,
        .groupId = SectionIds::kPatchEdit,
        .displayName = SectionDisplayNames::kPatchEdit
    },
    {
        .parentId = ModeIds::kPatch,
        .groupId = SectionIds::kMatrixModulation,
        .displayName = SectionDisplayNames::kMatrixModulation
    },
    {
        .parentId = ModeIds::kPatch,
        .groupId = SectionIds::kPatchManager,
        .displayName = SectionDisplayNames::kPatchManager
    },
    // Master Edit Modules
    {
        .parentId = SectionIds::kMasterEdit,
        .groupId = ModuleIds::kMidi,
        .displayName = ModuleDisplayNames::kMidi
    },
    {
        .parentId = SectionIds::kMasterEdit,
        .groupId = ModuleIds::kVibrato,
        .displayName = ModuleDisplayNames::kVibrato
    },
    {
        .parentId = SectionIds::kMasterEdit,
        .groupId = ModuleIds::kMisc,
        .displayName = ModuleDisplayNames::kMisc
    },
    // Patch Edit Modules
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kDco1,
        .displayName = ModuleDisplayNames::kDco1
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kDco2,
        .displayName = ModuleDisplayNames::kDco2
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kVcfVca,
        .displayName = ModuleDisplayNames::kVcfVca
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kFmTrack,
        .displayName = ModuleDisplayNames::kFmTrack
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kRampPortamento,
        .displayName = ModuleDisplayNames::kRampPortamento
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kEnvelope1,
        .displayName = ModuleDisplayNames::kEnvelope1
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kEnvelope2,
        .displayName = ModuleDisplayNames::kEnvelope2
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kEnvelope3,
        .displayName = ModuleDisplayNames::kEnvelope3
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kLfo1,
        .displayName = ModuleDisplayNames::kLfo1
    },
    {
        .parentId = SectionIds::kPatchEdit,
        .groupId = ModuleIds::kLfo2,
        .displayName = ModuleDisplayNames::kLfo2
    },
    // Matrix Modulation Busses
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus0,
        .displayName = ModulationBusDisplayNames::kModulationBus0
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus1,
        .displayName = ModulationBusDisplayNames::kModulationBus1
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus2,
        .displayName = ModulationBusDisplayNames::kModulationBus2
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus3,
        .displayName = ModulationBusDisplayNames::kModulationBus3
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus4,
        .displayName = ModulationBusDisplayNames::kModulationBus4
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus5,
        .displayName = ModulationBusDisplayNames::kModulationBus5
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus6,
        .displayName = ModulationBusDisplayNames::kModulationBus6
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus7,
        .displayName = ModulationBusDisplayNames::kModulationBus7
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus8,
        .displayName = ModulationBusDisplayNames::kModulationBus8
    },
    {
        .parentId = SectionIds::kMatrixModulation,
        .groupId = ModulationBusIds::kModulationBus9,
        .displayName = ModulationBusDisplayNames::kModulationBus9
    },
    // Patch Manager Modules
    {
        .parentId = SectionIds::kPatchManager,
        .groupId = ModuleIds::kBankUtility,
        .displayName = ModuleDisplayNames::kBankUtility
    },
    {
        .parentId = SectionIds::kPatchManager,
        .groupId = ModuleIds::kInternalPatches,
        .displayName = ModuleDisplayNames::kInternalPatches
    },
    {
        .parentId = SectionIds::kPatchManager,
        .groupId = ModuleIds::kComputerPatches,
        .displayName = ModuleDisplayNames::kComputerPatches
    },
    {
        .parentId = SectionIds::kPatchManager,
        .groupId = ModuleIds::kPatchMutator,
        .displayName = ModuleDisplayNames::kPatchMutator
    }
};

// ============================================================================
// Data Definitions | Master Edit | Int Parameters
// ============================================================================

const std::vector<IntParameterDescriptor> kMasterEditIntParameters = {
    // MIDI Module Int Parameters
    {
        .parameterId = ParameterIds::kMidiPedal1Select,
        .displayName = ParameterDisplayNames::kMidiPedal1Select,
        .parentGroupId = ModuleIds::kMidi,
        .minValue = 0,
        .maxValue = 121,
        .defaultValue = 4,
        .sysExOffset = 17,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMidiPedal2Select,
        .displayName = ParameterDisplayNames::kMidiPedal2Select,
        .parentGroupId = ModuleIds::kMidi,
        .minValue = 0,
        .maxValue = 121,
        .defaultValue = 64,
        .sysExOffset = 18,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMidiLever2Select,
        .displayName = ParameterDisplayNames::kMidiLever2Select,
        .parentGroupId = ModuleIds::kMidi,
        .minValue = 0,
        .maxValue = 121,
        .defaultValue = 1,
        .sysExOffset = 19,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMidiLever3Select,
        .displayName = ParameterDisplayNames::kMidiLever3Select,
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
        .displayName = ParameterDisplayNames::kVibratoSpeed,
        .parentGroupId = ModuleIds::kVibrato,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 1,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kVibratoSpeedModAmount,
        .displayName = ParameterDisplayNames::kVibratoSpeedModAmount,
        .parentGroupId = ModuleIds::kVibrato,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 3,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kVibratoAmplitude,
        .displayName = ParameterDisplayNames::kVibratoAmplitude,
        .parentGroupId = ModuleIds::kVibrato,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 5,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kVibratoAmpModAmount,
        .displayName = ParameterDisplayNames::kVibratoAmpModAmount,
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
        .displayName = ParameterDisplayNames::kMasterTune,
        .parentGroupId = ModuleIds::kMisc,
        .minValue = -31,
        .maxValue = 31,
        .defaultValue = 0,
        .sysExOffset = 8,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMasterTranspose,
        .displayName = ParameterDisplayNames::kMasterTranspose,
        .parentGroupId = ModuleIds::kMisc,
        .minValue = -24,
        .maxValue = 24,
        .defaultValue = 0,
        .sysExOffset = 34,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kBendRange,
        .displayName = ParameterDisplayNames::kBendRange,
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
        .displayName = ParameterDisplayNames::kMidiChannel,
        .parentGroupId = ModuleIds::kMidi,
        .choices = makeStringArray({ ChoiceLists::kMidiChannelOmni, ChoiceLists::kMidiChannel1, ChoiceLists::kMidiChannel2, ChoiceLists::kMidiChannel3,
                                     ChoiceLists::kMidiChannel4, ChoiceLists::kMidiChannel5, ChoiceLists::kMidiChannel6, ChoiceLists::kMidiChannel7,
                                     ChoiceLists::kMidiChannel8, ChoiceLists::kMidiChannel9, ChoiceLists::kMidiChannel10, ChoiceLists::kMidiChannel11,
                                     ChoiceLists::kMidiChannel12, ChoiceLists::kMidiChannel13, ChoiceLists::kMidiChannel14, ChoiceLists::kMidiChannel15,
                                     ChoiceLists::kMidiChannel16, ChoiceLists::kMidiChannelMonoG1, ChoiceLists::kMidiChannelMonoG2, ChoiceLists::kMidiChannelMonoG3,
                                     ChoiceLists::kMidiChannelMonoG4, ChoiceLists::kMidiChannelMonoG5, ChoiceLists::kMidiChannelMonoG6, ChoiceLists::kMidiChannelMonoG7,
                                     ChoiceLists::kMidiChannelMonoG8, ChoiceLists::kMidiChannelMonoG9 }),
        .defaultIndex = 0,
        .sysExOffset = 11, // Combination of offsets 11, 12, 35
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMidiEcho,
        .displayName = ParameterDisplayNames::kMidiEcho,
        .parentGroupId = ModuleIds::kMidi,
        .choices = makeStringArray({ ChoiceLists::kOnOffChoiceOff, ChoiceLists::kOnOffChoiceOn }),
        .defaultIndex = 0,
        .sysExOffset = 32,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMidiControllers,
        .displayName = ParameterDisplayNames::kMidiControllers,
        .parentGroupId = ModuleIds::kMidi,
        .choices = makeStringArray({ ChoiceLists::kOnOffChoiceOff, ChoiceLists::kOnOffChoiceOn }),
        .defaultIndex = 1,
        .sysExOffset = 13,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMidiPatchChanges,
        .displayName = ParameterDisplayNames::kMidiPatchChanges,
        .parentGroupId = ModuleIds::kMidi,
        .choices = makeStringArray({ ChoiceLists::kOnOffChoiceOff, ChoiceLists::kOnOffChoiceOn }),
        .defaultIndex = 1,
        .sysExOffset = 14,
        .sysExId = kNoSysExId
    },
    // VIBRATO Module Choice Parameters
    {
        .parameterId = ParameterIds::kVibratoSpeedModSource,
        .displayName = ParameterDisplayNames::kVibratoSpeedModSource,
        .parentGroupId = ModuleIds::kVibrato,
        .choices = makeStringArray({ ChoiceLists::kOnOffChoiceOff, ChoiceLists::kSourceLever2, ChoiceLists::kSourcePedal1 }),
        .defaultIndex = 0,
        .sysExOffset = 2,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kVibratoWaveform,
        .displayName = ParameterDisplayNames::kVibratoWaveform,
        .parentGroupId = ModuleIds::kVibrato,
        .choices = makeStringArray({ ChoiceLists::kLfoWaveformTriangle, ChoiceLists::kLfoWaveformUpsaw, ChoiceLists::kLfoWaveformDnsaw,
                                     ChoiceLists::kLfoWaveformSquare, ChoiceLists::kLfoWaveformRandom, ChoiceLists::kLfoWaveformNoise, 
                                     ChoiceLists::kLfoWaveformSampled }),
        .defaultIndex = 0,
        .sysExOffset = 4,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kVibratoAmpModSource,
        .displayName = ParameterDisplayNames::kVibratoAmpModSource,
        .parentGroupId = ModuleIds::kVibrato,
        .choices = makeStringArray({ ChoiceLists::kOnOffChoiceOff, ChoiceLists::kSourceLever2, ChoiceLists::kSourcePedal1 }),
        .defaultIndex = 0,
        .sysExOffset = 6,
        .sysExId = kNoSysExId
    },
    // MISC Module Choice Parameters
    {
        .parameterId = ParameterIds::kUnisonEnable,
        .displayName = ParameterDisplayNames::kUnisonEnable,
        .parentGroupId = ModuleIds::kMisc,
        .choices = makeStringArray({ ChoiceLists::kOnOffChoiceOff, ChoiceLists::kOnOffChoiceOn }),
        .defaultIndex = 0,
        .sysExOffset = 169,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kVolumeInvertEnable,
        .displayName = ParameterDisplayNames::kVolumeInvertEnable,
        .parentGroupId = ModuleIds::kMisc,
        .choices = makeStringArray({ ChoiceLists::kOnOffChoiceOff, ChoiceLists::kOnOffChoiceOn }),
        .defaultIndex = 0,
        .sysExOffset = 170,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kBankLockEnable,
        .displayName = ParameterDisplayNames::kBankLockEnable,
        .parentGroupId = ModuleIds::kMisc,
        .choices = makeStringArray({ ChoiceLists::kOnOffChoiceOff, ChoiceLists::kOnOffChoiceOn }),
        .defaultIndex = 0,
        .sysExOffset = 165,
        .sysExId = kNoSysExId
    },
    {
        .parameterId = ParameterIds::kMemoryProtectEnable,
        .displayName = ParameterDisplayNames::kMemoryProtectEnable,
        .parentGroupId = ModuleIds::kMisc,
        .choices = makeStringArray({ ChoiceLists::kOnOffChoiceOff, ChoiceLists::kOnOffChoiceOn }),
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
        .displayName = ParameterDisplayNames::kDco1Frequency,
        .parentGroupId = ModuleIds::kDco1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 9,
        .sysExId = 0
    },
    {
        .parameterId = ParameterIds::kDco1FrequencyModByLfo1,
        .displayName = ParameterDisplayNames::kDco1FrequencyModByLfo1,
        .parentGroupId = ModuleIds::kDco1,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 86,
        .sysExId = 1
    },
    {
        .parameterId = ParameterIds::kDco1PulseWidth,
        .displayName = ParameterDisplayNames::kDco1PulseWidth,
        .parentGroupId = ModuleIds::kDco1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 31,
        .sysExOffset = 11,
        .sysExId = 3
    },
    {
        .parameterId = ParameterIds::kDco1PulseWidthModByLfo2,
        .displayName = ParameterDisplayNames::kDco1PulseWidthModByLfo2,
        .parentGroupId = ModuleIds::kDco1,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 87,
        .sysExId = 4
    },
    {
        .parameterId = ParameterIds::kDco1WaveShape,
        .displayName = ParameterDisplayNames::kDco1WaveShape,
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
        .displayName = ParameterDisplayNames::kDco1Sync,
        .parentGroupId = ModuleIds::kDco1,
        .choices = makeStringArray({ ChoiceLists::kSyncOff, ChoiceLists::kSyncSoft, ChoiceLists::kSyncMedium, ChoiceLists::kSyncHard }),
        .defaultIndex = 0,
        .sysExOffset = 25,
        .sysExId = 2
    },
    {
        .parameterId = ParameterIds::kDco1WaveSelect,
        .displayName = ParameterDisplayNames::kDco1WaveSelect,
        .parentGroupId = ModuleIds::kDco1,
        .choices = makeStringArray({ ChoiceLists::kWaveSelectOff, ChoiceLists::kWaveSelectPulse, ChoiceLists::kWaveSelectWave, ChoiceLists::kWaveSelectBoth }),
        .defaultIndex = 2,
        .sysExOffset = 13,
        .sysExId = 6
    },
    {
        .parameterId = ParameterIds::kDco1Levers,
        .displayName = ParameterDisplayNames::kDco1Levers,
        .parentGroupId = ModuleIds::kDco1,
        .choices = makeStringArray({ ChoiceLists::kLeversOff, ChoiceLists::kLeversL1Bend, ChoiceLists::kLeversL2Vib, ChoiceLists::kLeversBoth }),
        .defaultIndex = 3,
        .sysExOffset = 12,
        .sysExId = 7
    },
    {
        .parameterId = ParameterIds::kDco1KeyboardPortamento,
        .displayName = ParameterDisplayNames::kDco1KeyboardPortamento,
        .parentGroupId = ModuleIds::kDco1,
        .choices = makeStringArray({ ChoiceLists::kKeyboardPortamentoKeybd, ChoiceLists::kKeyboardPortamentoPorta }),
        .defaultIndex = 0,
        .sysExOffset = 21,
        .sysExId = 8
    },
    {
        .parameterId = ParameterIds::kDco1KeyClick,
        .displayName = ParameterDisplayNames::kDco1KeyClick,
        .parentGroupId = ModuleIds::kDco1,
        .choices = makeStringArray({ ChoiceLists::kOnOffChoiceOff, ChoiceLists::kOnOffChoiceOn }),
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
        .displayName = ParameterDisplayNames::kDco2Frequency,
        .parentGroupId = ModuleIds::kDco2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 14,
        .sysExId = 10
    },
    {
        .parameterId = ParameterIds::kDco2FrequencyModByLfo1,
        .displayName = ParameterDisplayNames::kDco2FrequencyModByLfo1,
        .parentGroupId = ModuleIds::kDco2,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 88,
        .sysExId = 11
    },
    {
        .parameterId = ParameterIds::kDco2Detune,
        .displayName = ParameterDisplayNames::kDco2Detune,
        .parentGroupId = ModuleIds::kDco2,
        .minValue = -31,
        .maxValue = 31,
        .defaultValue = 2,
        .sysExOffset = 19,
        .sysExId = 12
    },
    {
        .parameterId = ParameterIds::kDco2PulseWidth,
        .displayName = ParameterDisplayNames::kDco2PulseWidth,
        .parentGroupId = ModuleIds::kDco2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 24,
        .sysExOffset = 16,
        .sysExId = 13
    },
    {
        .parameterId = ParameterIds::kDco2PulseWidthModByLfo2,
        .displayName = ParameterDisplayNames::kDco2PulseWidthModByLfo2,
        .parentGroupId = ModuleIds::kDco2,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 89,
        .sysExId = 14
    },
    {
        .parameterId = ParameterIds::kDco2WaveShape,
        .displayName = ParameterDisplayNames::kDco2WaveShape,
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
        .displayName = ParameterDisplayNames::kDco2WaveSelect,
        .parentGroupId = ModuleIds::kDco2,
        .choices = makeStringArray({ ChoiceLists::kWaveSelectOff, ChoiceLists::kWaveSelectPulse, ChoiceLists::kWaveSelectWave, ChoiceLists::kWaveSelectBoth, ChoiceLists::kWaveSelectNoise }),
        .defaultIndex = 1,
        .sysExOffset = 18,
        .sysExId = 16
    },
    {
        .parameterId = ParameterIds::kDco2Levers,
        .displayName = ParameterDisplayNames::kDco2Levers,
        .parentGroupId = ModuleIds::kDco2,
        .choices = makeStringArray({ ChoiceLists::kLeversOff, ChoiceLists::kLeversL1Bend, ChoiceLists::kLeversL2Vib, ChoiceLists::kLeversBoth }),
        .defaultIndex = 3,
        .sysExOffset = 17,
        .sysExId = 17
    },
    {
        .parameterId = ParameterIds::kDco2KeyboardPortamento,
        .displayName = ParameterDisplayNames::kDco2KeyboardPortamento,
        .parentGroupId = ModuleIds::kDco2,
        .choices = makeStringArray({ ChoiceLists::kKeyboardPortamentoOff, ChoiceLists::kKeyboardPortamentoPorta, ChoiceLists::kKeyboardPortamentoKeybd }),
        .defaultIndex = 2,
        .sysExOffset = 23,
        .sysExId = 18
    },
    {
        .parameterId = ParameterIds::kDco2KeyClick,
        .displayName = ParameterDisplayNames::kDco2KeyClick,
        .parentGroupId = ModuleIds::kDco2,
        .choices = makeStringArray({ ChoiceLists::kOnOffChoiceOff, ChoiceLists::kOnOffChoiceOn }),
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
        .displayName = ParameterDisplayNames::kVcfBalance,
        .parentGroupId = ModuleIds::kVcfVca,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 31,
        .sysExOffset = 20,
        .sysExId = 20
    },
    {
        .parameterId = ParameterIds::kVcfFequency,
        .displayName = ParameterDisplayNames::kVcfFequency,
        .parentGroupId = ModuleIds::kVcfVca,
        .minValue = 0,
        .maxValue = 127,
        .defaultValue = 55,
        .sysExOffset = 26,
        .sysExId = 21
    },
    {
        .parameterId = ParameterIds::kVcfFrequencyModByEnv1,
        .displayName = ParameterDisplayNames::kVcfFrequencyModByEnv1,
        .parentGroupId = ModuleIds::kVcfVca,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 42,
        .sysExOffset = 90,
        .sysExId = 22
    },
    {
        .parameterId = ParameterIds::kVcfFrequencyModByPressure,
        .displayName = ParameterDisplayNames::kVcfFrequencyModByPressure,
        .parentGroupId = ModuleIds::kVcfVca,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 91,
        .sysExId = 23
    },
    {
        .parameterId = ParameterIds::kVcfResonance,
        .displayName = ParameterDisplayNames::kVcfResonance,
        .parentGroupId = ModuleIds::kVcfVca,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 27,
        .sysExId = 24
    },
    {
        .parameterId = ParameterIds::kVca1Volume,
        .displayName = ParameterDisplayNames::kVca1Volume,
        .parentGroupId = ModuleIds::kVcfVca,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 63,
        .sysExOffset = 31,
        .sysExId = 27
    },
    {
        .parameterId = ParameterIds::kVca1ModByVelocity,
        .displayName = ParameterDisplayNames::kVca1ModByVelocity,
        .parentGroupId = ModuleIds::kVcfVca,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 92,
        .sysExId = 28
    },
    {
        .parameterId = ParameterIds::kVca2ModByEnv2,
        .displayName = ParameterDisplayNames::kVca2ModByEnv2,
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
        .displayName = ParameterDisplayNames::kVcfLevers,
        .parentGroupId = ModuleIds::kVcfVca,
        .choices = makeStringArray({ ChoiceLists::kLeversOff, ChoiceLists::kLeversL1Bend, ChoiceLists::kLeversL2Vib, ChoiceLists::kLeversBoth }),
        .defaultIndex = 0,
        .sysExOffset = 28,
        .sysExId = 25
    },
    {
        .parameterId = ParameterIds::kVcfKeyboardPortamento,
        .displayName = ParameterDisplayNames::kVcfKeyboardPortamento,
        .parentGroupId = ModuleIds::kVcfVca,
        .choices = makeStringArray({ ChoiceLists::kKeyboardPortamentoOff, ChoiceLists::kKeyboardPortamentoPorta, ChoiceLists::kKeyboardPortamentoKeybd }),
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
        .displayName = ParameterDisplayNames::kFmAmount,
        .parentGroupId = ModuleIds::kFmTrack,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 30,
        .sysExId = 30
    },
    {
        .parameterId = ParameterIds::kFmModByEnv3,
        .displayName = ParameterDisplayNames::kFmModByEnv3,
        .parentGroupId = ModuleIds::kFmTrack,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 100,
        .sysExId = 31
    },
    {
        .parameterId = ParameterIds::kFmModByPressure,
        .displayName = ParameterDisplayNames::kFmModByPressure,
        .parentGroupId = ModuleIds::kFmTrack,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 101,
        .sysExId = 32
    },
    {
        .parameterId = ParameterIds::kTrackPoint1,
        .displayName = ParameterDisplayNames::kTrackPoint1,
        .parentGroupId = ModuleIds::kFmTrack,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 77,
        .sysExId = 34
    },
    {
        .parameterId = ParameterIds::kTrackPoint2,
        .displayName = ParameterDisplayNames::kTrackPoint2,
        .parentGroupId = ModuleIds::kFmTrack,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 15,
        .sysExOffset = 78,
        .sysExId = 35
    },
    {
        .parameterId = ParameterIds::kTrackPoint3,
        .displayName = ParameterDisplayNames::kTrackPoint3,
        .parentGroupId = ModuleIds::kFmTrack,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 31,
        .sysExOffset = 79,
        .sysExId = 36
    },
    {
        .parameterId = ParameterIds::kTrackPoint4,
        .displayName = ParameterDisplayNames::kTrackPoint4,
        .parentGroupId = ModuleIds::kFmTrack,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 47,
        .sysExOffset = 80,
        .sysExId = 37
    },
    {
        .parameterId = ParameterIds::kTrackPoint5,
        .displayName = ParameterDisplayNames::kTrackPoint5,
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
        .displayName = ParameterDisplayNames::kTrackInput,
        .parentGroupId = ModuleIds::kFmTrack,
        .choices = makeStringArray({ ChoiceLists::kSourceNone, ChoiceLists::kSourceEnv1, ChoiceLists::kSourceEnv2, ChoiceLists::kSourceEnv3, 
                                     ChoiceLists::kSourceLfo1, ChoiceLists::kSourceLfo2, ChoiceLists::kSourceVibrato, ChoiceLists::kSourceRamp1, ChoiceLists::kSourceRamp2,
                                     ChoiceLists::kSourceKeybd, ChoiceLists::kSourcePorta, ChoiceLists::kSourceTrack, ChoiceLists::kSourceKbGate, 
                                     ChoiceLists::kSourceVelocity, ChoiceLists::kSourceRelVel, ChoiceLists::kSourcePressure,
                                     ChoiceLists::kSourcePedal1, ChoiceLists::kSourcePedal2, ChoiceLists::kSourceLever1, ChoiceLists::kSourceLever2, ChoiceLists::kSourceLever3 }),
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
        .displayName = ParameterDisplayNames::kRamp1Rate,
        .parentGroupId = ModuleIds::kRampPortamento,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 82,
        .sysExId = 40
    },
    {
        .parameterId = ParameterIds::kRamp2Rate,
        .displayName = ParameterDisplayNames::kRamp2Rate,
        .parentGroupId = ModuleIds::kRampPortamento,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 84,
        .sysExId = 42
    },
    {
        .parameterId = ParameterIds::kPortamentoRate,
        .displayName = ParameterDisplayNames::kPortamentoRate,
        .parentGroupId = ModuleIds::kRampPortamento,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 32,
        .sysExId = 44
    },
    {
        .parameterId = ParameterIds::kPortamentoModByVelocity,
        .displayName = ParameterDisplayNames::kPortamentoModByVelocity,
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
        .displayName = ParameterDisplayNames::kRamp1Trigger,
        .parentGroupId = ModuleIds::kRampPortamento,
        .choices = makeStringArray({ ChoiceLists::kRampTriggerStrig, ChoiceLists::kRampTriggerMtrig, ChoiceLists::kRampTriggerExtrig, ChoiceLists::kRampTriggerGatedx }),
        .defaultIndex = 0,
        .sysExOffset = 83,
        .sysExId = 41
    },
    {
        .parameterId = ParameterIds::kRamp2Trigger,
        .displayName = ParameterDisplayNames::kRamp2Trigger,
        .parentGroupId = ModuleIds::kRampPortamento,
        .choices = makeStringArray({ ChoiceLists::kRampTriggerStrig, ChoiceLists::kRampTriggerMtrig, ChoiceLists::kRampTriggerExtrig, ChoiceLists::kRampTriggerGatedx }),
        .defaultIndex = 0,
        .sysExOffset = 85,
        .sysExId = 43
    },
    {
        .parameterId = ParameterIds::kPortamentoMode,
        .displayName = ParameterDisplayNames::kPortamentoMode,
        .parentGroupId = ModuleIds::kRampPortamento,
        .choices = makeStringArray({ ChoiceLists::kPortamentoModeLinear, ChoiceLists::kPortamentoModeConst, ChoiceLists::kPortamentoModeExpo }),
        .defaultIndex = 0,
        .sysExOffset = 33,
        .sysExId = 46
    },
    {
        .parameterId = ParameterIds::kPortamentoLegato,
        .displayName = ParameterDisplayNames::kPortamentoLegato,
        .parentGroupId = ModuleIds::kRampPortamento,
        .choices = makeStringArray({ ChoiceLists::kOnOffChoiceOff, ChoiceLists::kOnOffChoiceOn }),
        .defaultIndex = 0,
        .sysExOffset = 34,
        .sysExId = 47
    },
    {
        .parameterId = ParameterIds::kPortamentoKeyboardMode,
        .displayName = ParameterDisplayNames::kPortamentoKeyboardMode,
        .parentGroupId = ModuleIds::kRampPortamento,
        .choices = makeStringArray({ ChoiceLists::kPortamentoKeyboardModeRotate, ChoiceLists::kPortamentoKeyboardModeReasgn, 
                                     ChoiceLists::kPortamentoKeyboardModeUnison, ChoiceLists::kPortamentoKeyboardModeRearob }),
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
        .displayName = ParameterDisplayNames::kEnv1Delay,
        .parentGroupId = ModuleIds::kEnvelope1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 50,
        .sysExId = 50
    },
    {
        .parameterId = ParameterIds::kEnv1Attack,
        .displayName = ParameterDisplayNames::kEnv1Attack,
        .parentGroupId = ModuleIds::kEnvelope1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 51,
        .sysExId = 51
    },
    {
        .parameterId = ParameterIds::kEnv1Decay,
        .displayName = ParameterDisplayNames::kEnv1Decay,
        .parentGroupId = ModuleIds::kEnvelope1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 10,
        .sysExOffset = 52,
        .sysExId = 52
    },
    {
        .parameterId = ParameterIds::kEnv1Sustain,
        .displayName = ParameterDisplayNames::kEnv1Sustain,
        .parentGroupId = ModuleIds::kEnvelope1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 50,
        .sysExOffset = 53,
        .sysExId = 53
    },
    {
        .parameterId = ParameterIds::kEnv1Release,
        .displayName = ParameterDisplayNames::kEnv1Release,
        .parentGroupId = ModuleIds::kEnvelope1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 10,
        .sysExOffset = 54,
        .sysExId = 54
    },
    {
        .parameterId = ParameterIds::kEnv1Amplitude,
        .displayName = ParameterDisplayNames::kEnv1Amplitude,
        .parentGroupId = ModuleIds::kEnvelope1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 40,
        .sysExOffset = 55,
        .sysExId = 55
    },
    {
        .parameterId = ParameterIds::kEnv1AmplitudeModByVelocity,
        .displayName = ParameterDisplayNames::kEnv1AmplitudeModByVelocity,
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
        .displayName = ParameterDisplayNames::kEnv1TriggerMode,
        .parentGroupId = ModuleIds::kEnvelope1,
        .choices = makeStringArray({ ChoiceLists::kTriggerModeStrig, ChoiceLists::kTriggerModeMtrig, ChoiceLists::kTriggerModeSreset, 
                                     ChoiceLists::kTriggerModeMreset, ChoiceLists::kTriggerModeXtrig, ChoiceLists::kTriggerModeXmtrig, 
                                     ChoiceLists::kTriggerModeXreset, ChoiceLists::kTriggerModeXmrst }),
        .defaultIndex = 0,
        .sysExOffset = 49,
        .sysExId = 57
    },
    {
        .parameterId = ParameterIds::kEnv1EnvelopeMode,
        .displayName = ParameterDisplayNames::kEnv1EnvelopeMode,
        .parentGroupId = ModuleIds::kEnvelope1,
        .choices = makeStringArray({ ChoiceLists::kEnvelopeModeNormal, ChoiceLists::kEnvelopeModeDadr, ChoiceLists::kEnvelopeModeFree, ChoiceLists::kEnvelopeModeBoth }),
        .defaultIndex = 0,
        .sysExOffset = 57,
        .sysExId = 58
    },
    {
        .parameterId = ParameterIds::kEnv1Lfo1Trigger,
        .displayName = ParameterDisplayNames::kEnv1Lfo1Trigger,
        .parentGroupId = ModuleIds::kEnvelope1,
        .choices = makeStringArray({ ChoiceLists::kLfo1TriggerNormal, ChoiceLists::kLfo1TriggerLfo1, ChoiceLists::kLfo1TriggerGLfo1 }),
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
        .displayName = ParameterDisplayNames::kEnv2Delay,
        .parentGroupId = ModuleIds::kEnvelope2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 59,
        .sysExId = 60
    },
    {
        .parameterId = ParameterIds::kEnv2Attack,
        .displayName = ParameterDisplayNames::kEnv2Attack,
        .parentGroupId = ModuleIds::kEnvelope2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 60,
        .sysExId = 61
    },
    {
        .parameterId = ParameterIds::kEnv2Decay,
        .displayName = ParameterDisplayNames::kEnv2Decay,
        .parentGroupId = ModuleIds::kEnvelope2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 10,
        .sysExOffset = 61,
        .sysExId = 62
    },
    {
        .parameterId = ParameterIds::kEnv2Sustain,
        .displayName = ParameterDisplayNames::kEnv2Sustain,
        .parentGroupId = ModuleIds::kEnvelope2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 50,
        .sysExOffset = 62,
        .sysExId = 63
    },
    {
        .parameterId = ParameterIds::kEnv2Release,
        .displayName = ParameterDisplayNames::kEnv2Release,
        .parentGroupId = ModuleIds::kEnvelope2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 10,
        .sysExOffset = 63,
        .sysExId = 64
    },
    {
        .parameterId = ParameterIds::kEnv2Amplitude,
        .displayName = ParameterDisplayNames::kEnv2Amplitude,
        .parentGroupId = ModuleIds::kEnvelope2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 40,
        .sysExOffset = 64,
        .sysExId = 65
    },
    {
        .parameterId = ParameterIds::kEnv2AmplitudeModByVelocity,
        .displayName = ParameterDisplayNames::kEnv2AmplitudeModByVelocity,
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
        .displayName = ParameterDisplayNames::kEnv2TriggerMode,
        .parentGroupId = ModuleIds::kEnvelope2,
        .choices = makeStringArray({ ChoiceLists::kTriggerModeStrig, ChoiceLists::kTriggerModeMtrig, ChoiceLists::kTriggerModeSreset, 
                                     ChoiceLists::kTriggerModeMreset, ChoiceLists::kTriggerModeXtrig, ChoiceLists::kTriggerModeXmtrig, 
                                     ChoiceLists::kTriggerModeXreset, ChoiceLists::kTriggerModeXmrst }),
        .defaultIndex = 0,
        .sysExOffset = 58,
        .sysExId = 67
    },
    {
        .parameterId = ParameterIds::kEnv2EnvelopeMode,
        .displayName = ParameterDisplayNames::kEnv2EnvelopeMode,
        .parentGroupId = ModuleIds::kEnvelope2,
        .choices = makeStringArray({ ChoiceLists::kEnvelopeModeNormal, ChoiceLists::kEnvelopeModeDadr, ChoiceLists::kEnvelopeModeFree, ChoiceLists::kEnvelopeModeBoth }),
        .defaultIndex = 0,
        .sysExOffset = 66,
        .sysExId = 68
    },
    {
        .parameterId = ParameterIds::kEnv2Lfo1Trigger,
        .displayName = ParameterDisplayNames::kEnv2Lfo1Trigger,
        .parentGroupId = ModuleIds::kEnvelope2,
        .choices = makeStringArray({ ChoiceLists::kLfo1TriggerNormal, ChoiceLists::kLfo1TriggerLfo1, ChoiceLists::kLfo1TriggerGLfo1 }),
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
        .displayName = ParameterDisplayNames::kEnv3Delay,
        .parentGroupId = ModuleIds::kEnvelope3,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 68,
        .sysExId = 70
    },
    {
        .parameterId = ParameterIds::kEnv3Attack,
        .displayName = ParameterDisplayNames::kEnv3Attack,
        .parentGroupId = ModuleIds::kEnvelope3,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 69,
        .sysExId = 71
    },
    {
        .parameterId = ParameterIds::kEnv3Decay,
        .displayName = ParameterDisplayNames::kEnv3Decay,
        .parentGroupId = ModuleIds::kEnvelope3,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 20,
        .sysExOffset = 70,
        .sysExId = 72
    },
    {
        .parameterId = ParameterIds::kEnv3Sustain,
        .displayName = ParameterDisplayNames::kEnv3Sustain,
        .parentGroupId = ModuleIds::kEnvelope3,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 71,
        .sysExId = 73
    },
    {
        .parameterId = ParameterIds::kEnv3Release,
        .displayName = ParameterDisplayNames::kEnv3Release,
        .parentGroupId = ModuleIds::kEnvelope3,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 20,
        .sysExOffset = 72,
        .sysExId = 74
    },
    {
        .parameterId = ParameterIds::kEnv3Amplitude,
        .displayName = ParameterDisplayNames::kEnv3Amplitude,
        .parentGroupId = ModuleIds::kEnvelope3,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 40,
        .sysExOffset = 73,
        .sysExId = 75
    },
    {
        .parameterId = ParameterIds::kEnv3AmplitudeModByVelocity,
        .displayName = ParameterDisplayNames::kEnv3AmplitudeModByVelocity,
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
        .displayName = ParameterDisplayNames::kEnv3TriggerMode,
        .parentGroupId = ModuleIds::kEnvelope3,
        .choices = makeStringArray({ ChoiceLists::kTriggerModeStrig, ChoiceLists::kTriggerModeMtrig, ChoiceLists::kTriggerModeSreset, 
                                     ChoiceLists::kTriggerModeMreset, ChoiceLists::kTriggerModeXtrig, ChoiceLists::kTriggerModeXmtrig, 
                                     ChoiceLists::kTriggerModeXreset, ChoiceLists::kTriggerModeXmrst }),
        .defaultIndex = 0,
        .sysExOffset = 67,
        .sysExId = 77
    },
    {
        .parameterId = ParameterIds::kEnv3EnvelopeMode,
        .displayName = ParameterDisplayNames::kEnv3EnvelopeMode,
        .parentGroupId = ModuleIds::kEnvelope3,
        .choices = makeStringArray({ ChoiceLists::kEnvelopeModeNormal, ChoiceLists::kEnvelopeModeDadr, ChoiceLists::kEnvelopeModeFree, ChoiceLists::kEnvelopeModeBoth }),
        .defaultIndex = 0,
        .sysExOffset = 75,
        .sysExId = 78
    },
    {
        .parameterId = ParameterIds::kEnv3Lfo1Trigger,
        .displayName = ParameterDisplayNames::kEnv3Lfo1Trigger,
        .parentGroupId = ModuleIds::kEnvelope3,
        .choices = makeStringArray({ ChoiceLists::kLfo1TriggerNormal, ChoiceLists::kLfo1TriggerLfo1, ChoiceLists::kLfo1TriggerGatedLfo1Trigger }),
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
        .displayName = ParameterDisplayNames::kLfo1Speed,
        .parentGroupId = ModuleIds::kLfo1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 40,
        .sysExOffset = 35,
        .sysExId = 80
    },
    {
        .parameterId = ParameterIds::kLfo1SpeedModByPressure,
        .displayName = ParameterDisplayNames::kLfo1SpeedModByPressure,
        .parentGroupId = ModuleIds::kLfo1,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 102,
        .sysExId = 81
    },
    {
        .parameterId = ParameterIds::kLfo1RetriggerPoint,
        .displayName = ParameterDisplayNames::kLfo1RetriggerPoint,
        .parentGroupId = ModuleIds::kLfo1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 39,
        .sysExId = 83
    },
    {
        .parameterId = ParameterIds::kLfo1Amplitude,
        .displayName = ParameterDisplayNames::kLfo1Amplitude,
        .parentGroupId = ModuleIds::kLfo1,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 41,
        .sysExId = 84
    },
    {
        .parameterId = ParameterIds::kLfo1AmplitudeModByRamp1,
        .displayName = ParameterDisplayNames::kLfo1AmplitudeModByRamp1,
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
        .displayName = ParameterDisplayNames::kLfo1Waveform,
        .parentGroupId = ModuleIds::kLfo1,
        .choices = makeStringArray({ ChoiceLists::kLfoWaveformTriangle, ChoiceLists::kLfoWaveformUpsaw, ChoiceLists::kLfoWaveformDnsaw, 
                                     ChoiceLists::kLfoWaveformSquare, ChoiceLists::kLfoWaveformRandom, ChoiceLists::kLfoWaveformNoise, ChoiceLists::kLfoWaveformSampled }),
        .defaultIndex = 0,
        .sysExOffset = 38,
        .sysExId = 82
    },
    {
        .parameterId = ParameterIds::kLfo1TriggerMode,
        .displayName = ParameterDisplayNames::kLfo1TriggerMode,
        .parentGroupId = ModuleIds::kLfo1,
        .choices = makeStringArray({ ChoiceLists::kLfoTriggerModeOff, ChoiceLists::kLfoTriggerModeStrig, ChoiceLists::kLfoTriggerModeMtrig, ChoiceLists::kLfoTriggerModeXtrig }),
        .defaultIndex = 0,
        .sysExOffset = 36,
        .sysExId = 86
    },
    {
        .parameterId = ParameterIds::kLfo1Lag,
        .displayName = ParameterDisplayNames::kLfo1Lag,
        .parentGroupId = ModuleIds::kLfo1,
        .choices = makeStringArray({ ChoiceLists::kOnOffChoiceOff, ChoiceLists::kOnOffChoiceOn }),
        .defaultIndex = 0,
        .sysExOffset = 37,
        .sysExId = 87
    },
    {
        .parameterId = ParameterIds::kLfo1SampleInput,
        .displayName = ParameterDisplayNames::kLfo1SampleInput,
        .parentGroupId = ModuleIds::kLfo1,
        .choices = makeStringArray({ ChoiceLists::kSourceNone, ChoiceLists::kSourceEnv1, ChoiceLists::kSourceEnv2, ChoiceLists::kSourceEnv3, 
                                     ChoiceLists::kSourceLfo1, ChoiceLists::kSourceLfo2, ChoiceLists::kSourceVibrato, ChoiceLists::kSourceRamp1, ChoiceLists::kSourceRamp2,
                                     ChoiceLists::kSourceKeybd, ChoiceLists::kSourcePorta, ChoiceLists::kSourceTrack, ChoiceLists::kSourceKbGate, 
                                     ChoiceLists::kSourceVelocity, ChoiceLists::kSourceRelVel, ChoiceLists::kSourcePressure,
                                     ChoiceLists::kSourcePedal1, ChoiceLists::kSourcePedal2, ChoiceLists::kSourceLever1, ChoiceLists::kSourceLever2, ChoiceLists::kSourceLever3 }),
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
        .displayName = ParameterDisplayNames::kLfo2Speed,
        .parentGroupId = ModuleIds::kLfo2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 30,
        .sysExOffset = 42,
        .sysExId = 90
    },
    {
        .parameterId = ParameterIds::kLfo2SpeedModByKeyboard,
        .displayName = ParameterDisplayNames::kLfo2SpeedModByKeyboard,
        .parentGroupId = ModuleIds::kLfo2,
        .minValue = -63,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 103,
        .sysExId = 91
    },
    {
        .parameterId = ParameterIds::kLfo2RetriggerPoint,
        .displayName = ParameterDisplayNames::kLfo2RetriggerPoint,
        .parentGroupId = ModuleIds::kLfo2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 46,
        .sysExId = 93
    },
    {
        .parameterId = ParameterIds::kLfo2Amplitude,
        .displayName = ParameterDisplayNames::kLfo2Amplitude,
        .parentGroupId = ModuleIds::kLfo2,
        .minValue = 0,
        .maxValue = 63,
        .defaultValue = 0,
        .sysExOffset = 48,
        .sysExId = 94
    },
    {
        .parameterId = ParameterIds::kLfo2AmplitudeModByRamp2,
        .displayName = ParameterDisplayNames::kLfo2AmplitudeModByRamp2,
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
        .displayName = ParameterDisplayNames::kLfo2Waveform,
        .parentGroupId = ModuleIds::kLfo2,
        .choices = makeStringArray({ ChoiceLists::kLfoWaveformTriangle, ChoiceLists::kLfoWaveformUpsaw, ChoiceLists::kLfoWaveformDnsaw, 
                                     ChoiceLists::kLfoWaveformSquare, ChoiceLists::kLfoWaveformRandom, ChoiceLists::kLfoWaveformNoise, ChoiceLists::kLfoWaveformSampled }),
        .defaultIndex = 0,
        .sysExOffset = 45,
        .sysExId = 92
    },
    {
        .parameterId = ParameterIds::kLfo2TriggerMode,
        .displayName = ParameterDisplayNames::kLfo2TriggerMode,
        .parentGroupId = ModuleIds::kLfo2,
        .choices = makeStringArray({ ChoiceLists::kLfoTriggerModeOff, ChoiceLists::kLfoTriggerModeStrig, ChoiceLists::kLfoTriggerModeMtrig, ChoiceLists::kLfoTriggerModeXtrig }),
        .defaultIndex = 0,
        .sysExOffset = 43,
        .sysExId = 96
    },
    {
        .parameterId = ParameterIds::kLfo2Lag,
        .displayName = ParameterDisplayNames::kLfo2Lag,
        .parentGroupId = ModuleIds::kLfo2,
        .choices = makeStringArray({ ChoiceLists::kOnOffChoiceOff, ChoiceLists::kOnOffChoiceOn }),
        .defaultIndex = 0,
        .sysExOffset = 44,
        .sysExId = 97
    },
    {
        .parameterId = ParameterIds::kLfo2SampleInput,
        .displayName = ParameterDisplayNames::kLfo2SampleInput,
        .parentGroupId = ModuleIds::kLfo2,
        .choices = makeStringArray({ ChoiceLists::kSourceNone, ChoiceLists::kSourceEnv1, ChoiceLists::kSourceEnv2, ChoiceLists::kSourceEnv3, 
                                     ChoiceLists::kSourceLfo1, ChoiceLists::kSourceLfo2, ChoiceLists::kSourceVibrato, ChoiceLists::kSourceRamp1, ChoiceLists::kSourceRamp2,
                                     ChoiceLists::kSourceKeybd, ChoiceLists::kSourcePorta, ChoiceLists::kSourceTrack, ChoiceLists::kSourceKbGate, 
                                     ChoiceLists::kSourceVelocity, ChoiceLists::kSourceRelVel, ChoiceLists::kSourcePressure,
                                     ChoiceLists::kSourcePedal1, ChoiceLists::kSourcePedal2, ChoiceLists::kSourceLever1, ChoiceLists::kSourceLever2, ChoiceLists::kSourceLever3 }),
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
        ChoiceLists::kSourceNone, ChoiceLists::kSourceEnv1, ChoiceLists::kSourceEnv2, ChoiceLists::kSourceEnv3, 
        ChoiceLists::kSourceLfo1, ChoiceLists::kSourceLfo2, ChoiceLists::kSourceVibrato, ChoiceLists::kSourceRamp1, ChoiceLists::kSourceRamp2,
        ChoiceLists::kSourceKeybd, ChoiceLists::kSourcePorta, ChoiceLists::kSourceTrack, ChoiceLists::kSourceKbGate, 
        ChoiceLists::kSourceVelocity, ChoiceLists::kSourceRelVel, ChoiceLists::kSourcePressure,
        ChoiceLists::kSourcePedal1, ChoiceLists::kSourcePedal2, ChoiceLists::kSourceLever1, ChoiceLists::kSourceLever2, ChoiceLists::kSourceLever3
    });

    const juce::StringArray kDestinationChoices = makeStringArray({
        ChoiceLists::kDestinationNone, ChoiceLists::kDestinationDco1Frequency, ChoiceLists::kDestinationDco1PulseWidth, ChoiceLists::kDestinationDco1WaveShape,
        ChoiceLists::kDestinationDco2Frequency, ChoiceLists::kDestinationDco2PulseWidth, ChoiceLists::kDestinationDco2WaveShape,
        ChoiceLists::kDestinationDco1Dco2Mix, ChoiceLists::kDestinationVcfFmByDco1, ChoiceLists::kDestinationVcfFrequency, ChoiceLists::kDestinationVcfResonance,
        ChoiceLists::kDestinationVca1Volume, ChoiceLists::kDestinationVca2Volume,
        ChoiceLists::kDestinationEnv1Delay, ChoiceLists::kDestinationEnv1Attack, ChoiceLists::kDestinationEnv1Decay, 
        ChoiceLists::kDestinationEnv1Release, ChoiceLists::kDestinationEnv1Amplitude,
        ChoiceLists::kDestinationEnv2Delay, ChoiceLists::kDestinationEnv2Attack, ChoiceLists::kDestinationEnv2Decay, 
        ChoiceLists::kDestinationEnv2Release, ChoiceLists::kDestinationEnv2Amplitude,
        ChoiceLists::kDestinationEnv3Delay, ChoiceLists::kDestinationEnv3Attack, ChoiceLists::kDestinationEnv3Decay, 
        ChoiceLists::kDestinationEnv3Release, ChoiceLists::kDestinationEnv3Amplitude,
        ChoiceLists::kDestinationLfo1Speed, ChoiceLists::kDestinationLfo1Amplitude, ChoiceLists::kDestinationLfo2Speed, ChoiceLists::kDestinationLfo2Amplitude,
        ChoiceLists::kDestinationPortamentoRate
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
            .displayName = ParameterDisplayNames::kModulationBusSource,
            .parentGroupId = ModulationBusIds::kModulationBus0,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 104,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus0Destination,
            .displayName = ParameterDisplayNames::kModulationBusDestination,
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
            .displayName = ParameterDisplayNames::kModulationBusSource,
            .parentGroupId = ModulationBusIds::kModulationBus1,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 107,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus1Destination,
            .displayName = ParameterDisplayNames::kModulationBusDestination,
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
            .displayName = ParameterDisplayNames::kModulationBusSource,
            .parentGroupId = ModulationBusIds::kModulationBus2,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 110,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus2Destination,
            .displayName = ParameterDisplayNames::kModulationBusDestination,
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
            .displayName = ParameterDisplayNames::kModulationBusSource,
            .parentGroupId = ModulationBusIds::kModulationBus3,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 113,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus3Destination,
            .displayName = ParameterDisplayNames::kModulationBusDestination,
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
            .displayName = ParameterDisplayNames::kModulationBusSource,
            .parentGroupId = ModulationBusIds::kModulationBus4,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 116,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus4Destination,
            .displayName = ParameterDisplayNames::kModulationBusDestination,
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
            .displayName = ParameterDisplayNames::kModulationBusSource,
            .parentGroupId = ModulationBusIds::kModulationBus5,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 119,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus5Destination,
            .displayName = ParameterDisplayNames::kModulationBusDestination,
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
            .displayName = ParameterDisplayNames::kModulationBusSource,
            .parentGroupId = ModulationBusIds::kModulationBus6,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 122,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus6Destination,
            .displayName = ParameterDisplayNames::kModulationBusDestination,
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
            .displayName = ParameterDisplayNames::kModulationBusSource,
            .parentGroupId = ModulationBusIds::kModulationBus7,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 125,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus7Destination,
            .displayName = ParameterDisplayNames::kModulationBusDestination,
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
            .displayName = ParameterDisplayNames::kModulationBusSource,
            .parentGroupId = ModulationBusIds::kModulationBus8,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 128,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus8Destination,
            .displayName = ParameterDisplayNames::kModulationBusDestination,
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
            .displayName = ParameterDisplayNames::kModulationBusSource,
            .parentGroupId = ModulationBusIds::kModulationBus9,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 131,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = ParameterIds::kModulationBus9Destination,
            .displayName = ParameterDisplayNames::kModulationBusDestination,
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
            .displayName = ParameterDisplayNames::kModulationBusAmount,
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
            .displayName = ParameterDisplayNames::kModulationBusAmount,
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
            .displayName = ParameterDisplayNames::kModulationBusAmount,
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
            .displayName = ParameterDisplayNames::kModulationBusAmount,
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
            .displayName = ParameterDisplayNames::kModulationBusAmount,
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
            .displayName = ParameterDisplayNames::kModulationBusAmount,
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
            .displayName = ParameterDisplayNames::kModulationBusAmount,
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
            .displayName = ParameterDisplayNames::kModulationBusAmount,
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
            .displayName = ParameterDisplayNames::kModulationBusAmount,
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
            .displayName = ParameterDisplayNames::kModulationBusAmount,
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
        .displayName = StandaloneWidgetDisplayNames::kMidiInit,
        .parentGroupId = ModuleIds::kMidi,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kVibratoInit,
        .displayName = StandaloneWidgetDisplayNames::kVibratoInit,
        .parentGroupId = ModuleIds::kVibrato,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kMiscInit,
        .displayName = StandaloneWidgetDisplayNames::kMiscInit,
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
        .displayName = StandaloneWidgetDisplayNames::kDco1Init,
        .parentGroupId = ModuleIds::kDco1,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kDco1Copy,
        .displayName = StandaloneWidgetDisplayNames::kDco1Copy,
        .parentGroupId = ModuleIds::kDco1,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kDco1Paste,
        .displayName = StandaloneWidgetDisplayNames::kDco1Paste,
        .parentGroupId = ModuleIds::kDco1,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// DCO 2 Module
const std::vector<StandaloneWidgetDescriptor> kDco2StandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kDco2Init,
        .displayName = StandaloneWidgetDisplayNames::kDco2Init,
        .parentGroupId = ModuleIds::kDco2,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kDco2Copy,
        .displayName = StandaloneWidgetDisplayNames::kDco2Copy,
        .parentGroupId = ModuleIds::kDco2,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kDco2Paste,
        .displayName = StandaloneWidgetDisplayNames::kDco2Paste,
        .parentGroupId = ModuleIds::kDco2,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// VCF/VCA Module
const std::vector<StandaloneWidgetDescriptor> kVcfVcaStandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kVcfVcaInit,
        .displayName = StandaloneWidgetDisplayNames::kVcfVcaInit,
        .parentGroupId = ModuleIds::kVcfVca,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// FM/TRACK Module
const std::vector<StandaloneWidgetDescriptor> kFmTrackStandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kFmTrackInit,
        .displayName = StandaloneWidgetDisplayNames::kFmTrackInit,
        .parentGroupId = ModuleIds::kFmTrack,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// RAMP/PORTAMENTO Module
const std::vector<StandaloneWidgetDescriptor> kRampPortamentoStandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kRampPortamentoInit,
        .displayName = StandaloneWidgetDisplayNames::kRampPortamentoInit,
        .parentGroupId = ModuleIds::kRampPortamento,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ENV 1 Module
const std::vector<StandaloneWidgetDescriptor> kEnv1StandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kEnv1Init,
        .displayName = StandaloneWidgetDisplayNames::kEnv1Init,
        .parentGroupId = ModuleIds::kEnvelope1,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kEnv1Copy,
        .displayName = StandaloneWidgetDisplayNames::kEnv1Copy,
        .parentGroupId = ModuleIds::kEnvelope1,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kEnv1Paste,
        .displayName = StandaloneWidgetDisplayNames::kEnv1Paste,
        .parentGroupId = ModuleIds::kEnvelope1,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ENV 2 Module
const std::vector<StandaloneWidgetDescriptor> kEnv2StandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kEnv2Init,
        .displayName = StandaloneWidgetDisplayNames::kEnv2Init,
        .parentGroupId = ModuleIds::kEnvelope2,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kEnv2Copy,
        .displayName = StandaloneWidgetDisplayNames::kEnv2Copy,
        .parentGroupId = ModuleIds::kEnvelope2,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kEnv2Paste,
        .displayName = StandaloneWidgetDisplayNames::kEnv2Paste,
        .parentGroupId = ModuleIds::kEnvelope2,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// ENV 3 Module
const std::vector<StandaloneWidgetDescriptor> kEnv3StandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kEnv3Init,
        .displayName = StandaloneWidgetDisplayNames::kEnv3Init,
        .parentGroupId = ModuleIds::kEnvelope3,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kEnv3Copy,
        .displayName = StandaloneWidgetDisplayNames::kEnv3Copy,
        .parentGroupId = ModuleIds::kEnvelope3,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kEnv3Paste,
        .displayName = StandaloneWidgetDisplayNames::kEnv3Paste,
        .parentGroupId = ModuleIds::kEnvelope3,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// LFO 1 Module
const std::vector<StandaloneWidgetDescriptor> kLfo1StandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kLfo1Init,
        .displayName = StandaloneWidgetDisplayNames::kLfo1Init,
        .parentGroupId = ModuleIds::kLfo1,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kLfo1Copy,
        .displayName = StandaloneWidgetDisplayNames::kLfo1Copy,
        .parentGroupId = ModuleIds::kLfo1,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kLfo1Paste,
        .displayName = StandaloneWidgetDisplayNames::kLfo1Paste,
        .parentGroupId = ModuleIds::kLfo1,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// LFO 2 Module
const std::vector<StandaloneWidgetDescriptor> kLfo2StandaloneWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kLfo2Init,
        .displayName = StandaloneWidgetDisplayNames::kLfo2Init,
        .parentGroupId = ModuleIds::kLfo2,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kLfo2Copy,
        .displayName = StandaloneWidgetDisplayNames::kLfo2Copy,
        .parentGroupId = ModuleIds::kLfo2,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kLfo2Paste,
        .displayName = StandaloneWidgetDisplayNames::kLfo2Paste,
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
        .displayName = StandaloneWidgetDisplayNames::kUnlockBank,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank0,
        .displayName = StandaloneWidgetDisplayNames::kSelectBank0,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank1,
        .displayName = StandaloneWidgetDisplayNames::kSelectBank1,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank2,
        .displayName = StandaloneWidgetDisplayNames::kSelectBank2,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank3,
        .displayName = StandaloneWidgetDisplayNames::kSelectBank3,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank4,
        .displayName = StandaloneWidgetDisplayNames::kSelectBank4,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank5,
        .displayName = StandaloneWidgetDisplayNames::kSelectBank5,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank6,
        .displayName = StandaloneWidgetDisplayNames::kSelectBank6,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank7,
        .displayName = StandaloneWidgetDisplayNames::kSelectBank7,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank8,
        .displayName = StandaloneWidgetDisplayNames::kSelectBank8,
        .parentGroupId = ModuleIds::kBankUtility,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectBank9,
        .displayName = StandaloneWidgetDisplayNames::kSelectBank9,
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
        .displayName = StandaloneWidgetDisplayNames::kLoadPreviousPatch,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kLoadNextPatch,
        .displayName = StandaloneWidgetDisplayNames::kLoadNextPatch,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kCurrentBankNumber,
        .displayName = StandaloneWidgetDisplayNames::kCurrentBankNumber,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kNumber
    },
    {
        .widgetId = StandaloneWidgetIds::kCurrentPatchNumber,
        .displayName = StandaloneWidgetDisplayNames::kCurrentPatchNumber,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kNumber
    },
    {
        .widgetId = StandaloneWidgetIds::kInitPatch,
        .displayName = StandaloneWidgetDisplayNames::kInitPatch,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kCopyPatch,
        .displayName = StandaloneWidgetDisplayNames::kCopyPatch,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPastePatch,
        .displayName = StandaloneWidgetDisplayNames::kPastePatch,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kStorePatch,
        .displayName = StandaloneWidgetDisplayNames::kStorePatch,
        .parentGroupId = ModuleIds::kInternalPatches,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// Computer Patches Module
const std::vector<StandaloneWidgetDescriptor> kComputerPatchesWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kLoadPreviousPatchFile,
        .displayName = StandaloneWidgetDisplayNames::kLoadPreviousPatchFile,
        .parentGroupId = ModuleIds::kComputerPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kLoadNextPatchFile,
        .displayName = StandaloneWidgetDisplayNames::kLoadNextPatchFile,
        .parentGroupId = ModuleIds::kComputerPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSelectPatchFile,
        .displayName = StandaloneWidgetDisplayNames::kSelectPatchFile,
        .parentGroupId = ModuleIds::kComputerPatches,
        .widgetType = StandaloneWidgetType::kComboBox
    },
    {
        .widgetId = StandaloneWidgetIds::kOpenPatchFolder,
        .displayName = StandaloneWidgetDisplayNames::kOpenPatchFolder,
        .parentGroupId = ModuleIds::kComputerPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSavePatchAs,
        .displayName = StandaloneWidgetDisplayNames::kSavePatchAsFile,
        .parentGroupId = ModuleIds::kComputerPatches,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kSavePatchFile,
        .displayName = StandaloneWidgetDisplayNames::kSavePatch,
        .parentGroupId = ModuleIds::kComputerPatches,
        .widgetType = StandaloneWidgetType::kButton
    }
};

// Patch Mutator Module
const std::vector<StandaloneWidgetDescriptor> kPatchMutatorWidgets = {
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorAmount,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorAmount,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kLabel
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorRandom,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorRandom,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kLabel
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorHistory,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorHistory,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kComboBox
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorMutate,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorMutate,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorRetry,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorRetry,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorCompare,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorCompare,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorDelete,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorDelete,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorClear,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorClear,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorExport,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorExport,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorDco1,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorD1,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorDco2,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorD2,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorVcfVca,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorFByA,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorFmTrack,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorFByT,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorRampPortamento,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorRByP,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorEnv1,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorE1,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorEnv2,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorE2,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorEnv3,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorE3,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorLfo1,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorL1,
        .parentGroupId = ModuleIds::kPatchMutator,
        .widgetType = StandaloneWidgetType::kButton
    },
    {
        .widgetId = StandaloneWidgetIds::kPatchMutatorLfo2,
        .displayName = StandaloneWidgetDisplayNames::kPatchMutatorL2,
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

