#pragma once

// PluginIDs.h
// Central file for all plugin IDs (unique identifiers)
// Contains ONLY constant string definitions - no code/logic
// Single source of truth for all plugin IDs

namespace PluginDescriptors
{

// ============================================================================
// APVTS Group IDs | Plugin Modes
// ============================================================================

namespace ModeIds
{
    constexpr const char* kMaster                     = "masterMode";
    constexpr const char* kPatch                      = "patchMode";
}

// ============================================================================
// APVTS Group IDs | Plugin Sections
// ============================================================================

namespace SectionIds
{
    constexpr const char* kMasterEdit                 = "masterEditSection";
    constexpr const char* kPatchEdit                  = "patchEditSection";
    constexpr const char* kMatrixModulation           = "matrixModulationSection";
    constexpr const char* kPatchManager               = "patchManagerSection";
}

// ============================================================================
// APVTS Group IDs | Plugin Modules
// ============================================================================

namespace ModuleIds
{
    // Master Edit Module IDs
    constexpr const char* kMidi                       = "midiModule";
    constexpr const char* kVibrato                    = "vibratoModule";
    constexpr const char* kMisc                       = "miscModule";
    
    // Patch Edit Module IDs
    constexpr const char* kDco1                       = "dco1Module";
    constexpr const char* kDco2                       = "dco2Module";
    constexpr const char* kVcfVca                     = "vcfVcaModule";
    constexpr const char* kFmTrack                    = "fmTrackModule";
    constexpr const char* kRampPortamento             = "rampPortamentoModule";
    constexpr const char* kEnvelope1                  = "envelope1Module";
    constexpr const char* kEnvelope2                  = "envelope2Module";
    constexpr const char* kEnvelope3                  = "envelope3Module";
    constexpr const char* kLfo1                       = "lfo1Module";
    constexpr const char* kLfo2                       = "lfo2Module";

    // Patch Manager Module IDs
    constexpr const char* kBankUtility                = "bankUtilityModule";
    constexpr const char* kInternalPatches            = "internalPatchesModule";
    constexpr const char* kComputerPatches            = "computerPatchesModule";
}

// ============================================================================
// APVTS Group IDs | Matrix Modulation Busses
// ============================================================================

constexpr int kModulationBusCount = 10;

namespace ModulationBusIds
{
    constexpr const char* kModulationBus0             = "modulationBus0";
    constexpr const char* kModulationBus1             = "modulationBus1";
    constexpr const char* kModulationBus2             = "modulationBus2";
    constexpr const char* kModulationBus3             = "modulationBus3";
    constexpr const char* kModulationBus4             = "modulationBus4";
    constexpr const char* kModulationBus5             = "modulationBus5";
    constexpr const char* kModulationBus6             = "modulationBus6";
    constexpr const char* kModulationBus7             = "modulationBus7";
    constexpr const char* kModulationBus8             = "modulationBus8";
    constexpr const char* kModulationBus9             = "modulationBus9";
}

// ============================================================================
// APVTS Group IDs | Standalone Widget Groups
// ============================================================================

namespace ParameterGroupIds
{
    // Patch Manager | Internal Patches Widget Group IDs
    constexpr const char* kInternalPatchesBrowser     = "internalPatchesBrowserGroup";
    constexpr const char* kInternalPatchesMemory     = "internalPatchesMemoryGroup";

    // Patch Manager | Computer Patches Widget Group IDs
    constexpr const char* kComputerPatchesBrowser     = "computerPatchesBrowserGroup";
    constexpr const char* kComputerPatchesStorage     = "computerPatchesStorageGroup";
}

// ============================================================================
// Synth Parameter IDs
// ============================================================================

namespace ParameterIds
{
    // Master Edit | MIDI Module Parameter IDs
    constexpr const char* kMidiChannel                = "midiChannel";
    constexpr const char* kMidiEcho                   = "midiEcho";
    constexpr const char* kMidiControllers            = "midiControllers";
    constexpr const char* kMidiPatchChanges           = "midiPatchChanges";
    constexpr const char* kMidiPedal1Select           = "midiPedal1Select";
    constexpr const char* kMidiPedal2Select           = "midiPedal2Select";
    constexpr const char* kMidiLever2Select           = "midiLever2Select";
    constexpr const char* kMidiLever3Select           = "midiLever3Select";

    // Master Edit | VIBRATO Module Parameter IDs
    constexpr const char* kVibratoSpeed               = "vibratoSpeed";
    constexpr const char* kVibratoSpeedModSource      = "vibratoSpeedModSource";
    constexpr const char* kVibratoSpeedModAmount      = "vibratoSpeedModAmount";
    constexpr const char* kVibratoWaveform            = "vibratoWaveform";
    constexpr const char* kVibratoAmplitude           = "vibratoAmplitude";
    constexpr const char* kVibratoAmpModSource        = "vibratoAmpModSource";
    constexpr const char* kVibratoAmpModAmount        = "vibratoAmpModAmount";

    // Master Edit | MISC Module Parameter IDs
    constexpr const char* kMasterTune                 = "masterTune";
    constexpr const char* kMasterTranspose            = "masterTranspose";
    constexpr const char* kBendRange                  = "bendRange";
    constexpr const char* kUnisonEnable               = "unisonEnable";
    constexpr const char* kVolumeInvertEnable         = "volumeInvertEnable";
    constexpr const char* kBankLockEnable             = "bankLockEnable";
    constexpr const char* kMemoryProtectEnable        = "memoryProtectEnable";

    // Patch Edit | DCO 1 Module Parameter IDs
    constexpr const char* kDco1Frequency              = "dco1Frequency";
    constexpr const char* kDco1FrequencyModByLfo1     = "dco1FrequencyModByLfo1";
    constexpr const char* kDco1PulseWidth             = "dco1PulseWidth";
    constexpr const char* kDco1PulseWidthModByLfo2    = "dco1PulseWidthModByLfo2";
    constexpr const char* kDco1WaveShape              = "dco1WaveShape";
    constexpr const char* kDco1Sync                   = "dco1Sync";
    constexpr const char* kDco1WaveSelect             = "dco1WaveSelect";
    constexpr const char* kDco1Levers                 = "dco1Levers";
    constexpr const char* kDco1KeyboardPortamento     = "dco1KeyboardPortamento";
    constexpr const char* kDco1KeyClick               = "dco1KeyClick";

    // Patch Edit | DCO 2 Module Parameter IDs
    constexpr const char* kDco2Frequency              = "dco2Frequency";
    constexpr const char* kDco2FrequencyModByLfo1     = "dco2FrequencyModByLfo1";
    constexpr const char* kDco2Detune                 = "dco2Detune";
    constexpr const char* kDco2PulseWidth             = "dco2PulseWidth";
    constexpr const char* kDco2PulseWidthModByLfo2    = "dco2PulseWidthModByLfo2";
    constexpr const char* kDco2WaveShape              = "dco2WaveShape";
    constexpr const char* kDco2WaveSelect             = "dco2WaveSelect";
    constexpr const char* kDco2Levers                 = "dco2Levers";
    constexpr const char* kDco2KeyboardPortamento     = "dco2KeyboardPortamento";
    constexpr const char* kDco2KeyClick               = "dco2KeyClick";

    // Patch Edit | VCF/VCA Module Parameter IDs
    constexpr const char* kVcfBalance                 = "vcfBalance";
    constexpr const char* kVcfFequency                = "vcfFequency";
    constexpr const char* kVcfFrequencyModByEnv1      = "vcfFrequencyModByEnv1";
    constexpr const char* kVcfFrequencyModByPressure  = "vcfFrequencyModByPressure";
    constexpr const char* kVcfResonance               = "vcfResonance";
    constexpr const char* kVca1Volume                 = "vca1Volume";
    constexpr const char* kVca1ModByVelocity          = "vca1ModByVelocity";
    constexpr const char* kVca2ModByEnv2              = "vca2ModByEnv2";
    constexpr const char* kVcfLevers                  = "vcfLevers";
    constexpr const char* kVcfKeyboardPortamento      = "vcfKeyboardPortamento";

    // Patch Edit | FM/TRACK Module Parameter IDs
    constexpr const char* kFmAmount                   = "fmAmount";
    constexpr const char* kFmModByEnv3                = "fmModByEnv3";
    constexpr const char* kFmModByPressure            = "fmModByPressure";
    constexpr const char* kTrackPoint1                = "trackPoint1";
    constexpr const char* kTrackPoint2                = "trackPoint2";
    constexpr const char* kTrackPoint3                = "trackPoint3";
    constexpr const char* kTrackPoint4                = "trackPoint4";
    constexpr const char* kTrackPoint5                = "trackPoint5";
    constexpr const char* kTrackInput                 = "trackInput";

    // Patch Edit | RAMP/PORTAMENTO Module Parameter IDs
    constexpr const char* kRamp1Rate                  = "ramp1Rate";
    constexpr const char* kRamp2Rate                  = "ramp2Rate";
    constexpr const char* kPortamentoRate             = "portamentoRate";
    constexpr const char* kPortamentoModByVelocity    = "portamentoModByVelocity";
    constexpr const char* kRamp1Trigger               = "ramp1Trigger";
    constexpr const char* kRamp2Trigger               = "ramp2Trigger";
    constexpr const char* kPortamentoMode             = "portamentoMode";
    constexpr const char* kPortamentoLegato           = "portamentoLegato";
    constexpr const char* kPortamentoKeyboardMode     = "portamentoKeyboardMode";

    // Patch Edit | ENV 1 Module Parameter IDs
    constexpr const char* kEnv1Delay                  = "env1Delay";
    constexpr const char* kEnv1Attack                 = "env1Attack";
    constexpr const char* kEnv1Decay                  = "env1Decay";
    constexpr const char* kEnv1Sustain                = "env1Sustain";
    constexpr const char* kEnv1Release                = "env1Release";
    constexpr const char* kEnv1Amplitude              = "env1Amplitude";
    constexpr const char* kEnv1AmplitudeModByVelocity = "env1AmplitudeModByVelocity";
    constexpr const char* kEnv1TriggerMode            = "env1TriggerMode";
    constexpr const char* kEnv1EnvelopeMode           = "env1EnvelopeMode";
    constexpr const char* kEnv1Lfo1Trigger            = "env1Lfo1Trigger";

    // Patch Edit | ENV 2 Module Parameter IDs
    constexpr const char* kEnv2Delay                  = "env2Delay";
    constexpr const char* kEnv2Attack                 = "env2Attack";
    constexpr const char* kEnv2Decay                  = "env2Decay";
    constexpr const char* kEnv2Sustain                = "env2Sustain";
    constexpr const char* kEnv2Release                = "env2Release";
    constexpr const char* kEnv2Amplitude              = "env2Amplitude";
    constexpr const char* kEnv2AmplitudeModByVelocity = "env2AmplitudeModByVelocity";
    constexpr const char* kEnv2TriggerMode            = "env2TriggerMode";
    constexpr const char* kEnv2EnvelopeMode           = "env2EnvelopeMode";
    constexpr const char* kEnv2Lfo1Trigger            = "env2Lfo1Trigger";

    // Patch Edit | ENV 3 Module Parameter IDs
    constexpr const char* kEnv3Delay                  = "env3Delay";
    constexpr const char* kEnv3Attack                 = "env3Attack";
    constexpr const char* kEnv3Decay                  = "env3Decay";
    constexpr const char* kEnv3Sustain                = "env3Sustain";
    constexpr const char* kEnv3Release                = "env3Release";
    constexpr const char* kEnv3Amplitude              = "env3Amplitude";
    constexpr const char* kEnv3AmplitudeModByVelocity = "env3AmplitudeModByVelocity";
    constexpr const char* kEnv3TriggerMode            = "env3TriggerMode";
    constexpr const char* kEnv3EnvelopeMode           = "env3EnvelopeMode";
    constexpr const char* kEnv3Lfo1Trigger            = "env3Lfo1Trigger";

    // Patch Edit | LFO 1 Module Parameter IDs
    constexpr const char* kLfo1Speed                  = "lfo1Speed";
    constexpr const char* kLfo1SpeedModByPressure     = "lfo1SpeedModByPressure";
    constexpr const char* kLfo1RetriggerPoint         = "lfo1RetriggerPoint";
    constexpr const char* kLfo1Amplitude              = "lfo1Amplitude";
    constexpr const char* kLfo1AmplitudeModByRamp1    = "lfo1AmplitudeModByRamp1";
    constexpr const char* kLfo1Waveform               = "lfo1Waveform";
    constexpr const char* kLfo1TriggerMode            = "lfo1TriggerMode";
    constexpr const char* kLfo1Lag                    = "lfo1Lag";
    constexpr const char* kLfo1SampleInput            = "lfo1SampleInput";

    // Patch Edit | LFO 2 Module Parameter IDs
    constexpr const char* kLfo2Speed                  = "lfo2Speed";
    constexpr const char* kLfo2SpeedModByKeyboard     = "lfo2SpeedModByKeyboard";
    constexpr const char* kLfo2RetriggerPoint         = "lfo2RetriggerPoint";
    constexpr const char* kLfo2Amplitude              = "lfo2Amplitude";
    constexpr const char* kLfo2AmplitudeModByRamp2    = "lfo2AmplitudeModByRamp2";
    constexpr const char* kLfo2Waveform               = "lfo2Waveform";
    constexpr const char* kLfo2TriggerMode            = "lfo2TriggerMode";
    constexpr const char* kLfo2Lag                    = "lfo2Lag";
    constexpr const char* kLfo2SampleInput            = "lfo2SampleInput";

    // Matrix Modulation | Modulation Bus Parameter IDs
    constexpr const char* kModulationBus0Source       = "modulationBus0Source";
    constexpr const char* kModulationBus0Amount       = "modulationBus0Amount";
    constexpr const char* kModulationBus0Destination  = "modulationBus0Destination";

    constexpr const char* kModulationBus1Source       = "modulationBus1Source";
    constexpr const char* kModulationBus1Amount       = "modulationBus1Amount";
    constexpr const char* kModulationBus1Destination  = "modulationBus1Destination";

    constexpr const char* kModulationBus2Source       = "modulationBus2Source";
    constexpr const char* kModulationBus2Amount       = "modulationBus2Amount";
    constexpr const char* kModulationBus2Destination  = "modulationBus2Destination";

    constexpr const char* kModulationBus3Source       = "modulationBus3Source";
    constexpr const char* kModulationBus3Amount       = "modulationBus3Amount";
    constexpr const char* kModulationBus3Destination  = "modulationBus3Destination";

    constexpr const char* kModulationBus4Source       = "modulationBus4Source";
    constexpr const char* kModulationBus4Amount       = "modulationBus4Amount";
    constexpr const char* kModulationBus4Destination  = "modulationBus4Destination";

    constexpr const char* kModulationBus5Source       = "modulationBus5Source";
    constexpr const char* kModulationBus5Amount       = "modulationBus5Amount";
    constexpr const char* kModulationBus5Destination  = "modulationBus5Destination";

    constexpr const char* kModulationBus6Source       = "modulationBus6Source";
    constexpr const char* kModulationBus6Amount       = "modulationBus6Amount";
    constexpr const char* kModulationBus6Destination  = "modulationBus6Destination";

    constexpr const char* kModulationBus7Source       = "modulationBus7Source";
    constexpr const char* kModulationBus7Amount       = "modulationBus7Amount";
    constexpr const char* kModulationBus7Destination  = "modulationBus7Destination";
    
    constexpr const char* kModulationBus8Source       = "modulationBus8Source";
    constexpr const char* kModulationBus8Amount       = "modulationBus8Amount";
    constexpr const char* kModulationBus8Destination  = "modulationBus8Destination";

    constexpr const char* kModulationBus9Source       = "modulationBus9Source";
    constexpr const char* kModulationBus9Amount       = "modulationBus9Amount";
    constexpr const char* kModulationBus9Destination  = "modulationBus9Destination";
}

// ============================================================================
// Standalone Widget IDs
// ============================================================================

namespace StandaloneWidgetIds
{
    // Master Edit | MIDI Standalone Widgets IDs
    constexpr const char* kMidiInit                   = "midiInit";

    // Master Edit | VIBRATO Standalone Widgets IDs
    constexpr const char* kVibratoInit                = "vibratoInit";

    // Master Edit | MISC Standalone Widgets IDs
    constexpr const char* kMiscInit                   = "miscInit";

    // Patch Edit | DCO 1 Standalone Widgets IDs
    constexpr const char* kDco1Init                   = "dco1Init";
    constexpr const char* kDco1Copy                   = "dco1Copy";
    constexpr const char* kDco1Paste                  = "dco1Paste";

    // Patch Edit | DCO 2 Standalone Widgets IDs
    constexpr const char* kDco2Init                   = "dco2Init";
    constexpr const char* kDco2Copy                   = "dco2Copy";
    constexpr const char* kDco2Paste                  = "dco2Paste";

    // Patch Edit | VCF/VCA Standalone Widgets IDs
    constexpr const char* kVcfVcaInit                 = "vcfVcaInit";

    // Patch Edit | FM/TRACK Standalone Widgets IDs
    constexpr const char* kFmTrackInit                = "fmTrackInit";

    // Patch Edit | RAMP/PORTAMENTO Standalone Widgets IDs
    constexpr const char* kRampPortamentoInit         = "rampPortamentoInit";

    // Patch Edit | ENV 1 Standalone Widgets IDs
    constexpr const char* kEnv1Init                   = "env1Init";
    constexpr const char* kEnv1Copy                   = "env1Copy";
    constexpr const char* kEnv1Paste                  = "env1Paste";

    // Patch Edit | ENV 2 Standalone Widgets IDs
    constexpr const char* kEnv2Init                   = "env2Init";
    constexpr const char* kEnv2Copy                   = "env2Copy";
    constexpr const char* kEnv2Paste                  = "env2Paste";

    // Patch Edit | ENV 3 Standalone Widgets IDs
    constexpr const char* kEnv3Init                   = "env3Init";
    constexpr const char* kEnv3Copy                   = "env3Copy";
    constexpr const char* kEnv3Paste                  = "env3Paste";

    // Patch Edit | LFO 1 Standalone Widgets IDs
    constexpr const char* kLfo1Init                   = "lfo1Init";
    constexpr const char* kLfo1Copy                   = "lfo1Copy";
    constexpr const char* kLfo1Paste                  = "lfo1Paste";

    // Patch Edit | LFO 2 Standalone Widgets IDs
    constexpr const char* kLfo2Init                   = "lfo2Init";
    constexpr const char* kLfo2Copy                   = "lfo2Copy";
    constexpr const char* kLfo2Paste                  = "lfo2Paste";

    // Matrix Modulation Standalone Widgets IDs
    constexpr const char* kMatrixModulationInit       = "matrixModulationInit";
    constexpr const char* kModulationBus0Init         = "modulationBus0Init";
    constexpr const char* kModulationBus1Init         = "modulationBus1Init";
    constexpr const char* kModulationBus2Init         = "modulationBus2Init";
    constexpr const char* kModulationBus3Init         = "modulationBus3Init";
    constexpr const char* kModulationBus4Init         = "modulationBus4Init";
    constexpr const char* kModulationBus5Init         = "modulationBus5Init";
    constexpr const char* kModulationBus6Init         = "modulationBus6Init";
    constexpr const char* kModulationBus7Init         = "modulationBus7Init";
    constexpr const char* kModulationBus8Init         = "modulationBus8Init";
    constexpr const char* kModulationBus9Init         = "modulationBus9Init";

    // Patch Manager | Bank Utility Standalone Widgets IDs
    constexpr const char* kUnlockBank                 = "unlockBank";
    constexpr const char* kSelectBank0                = "selectBank0";
    constexpr const char* kSelectBank1                = "selectBank1";
    constexpr const char* kSelectBank2                = "selectBank2";
    constexpr const char* kSelectBank3                = "selectBank3";
    constexpr const char* kSelectBank4                = "selectBank4";
    constexpr const char* kSelectBank5                = "selectBank5";
    constexpr const char* kSelectBank6                = "selectBank6";
    constexpr const char* kSelectBank7                = "selectBank7";
    constexpr const char* kSelectBank8                = "selectBank8";
    constexpr const char* kSelectBank9                = "selectBank9";

    // Patch Manager | Internal Patches Standalone Widgets IDs
    constexpr const char* kLoadPreviousPatch          = "loadPreviousPatch";
    constexpr const char* kLoadNextPatch              = "loadNextPatch";
    constexpr const char* kCurrentBankNumber          = "currentBankNumber";
    constexpr const char* kCurrentPatchNumber         = "currentPatchNumber";
    constexpr const char* kInitPatch                  = "initPatch";
    constexpr const char* kCopyPatch                  = "copyPatch";
    constexpr const char* kPastePatch                 = "pastePatch";
    constexpr const char* kStorePatch                 = "storePatch";

    // Patch Manager | Computer Patches Standalone Widgets IDs
    constexpr const char* kLoadPreviousPatchFile      = "loadPreviousPatchFile";
    constexpr const char* kLoadNextPatchFile          = "loadNextPatchFile";
    constexpr const char* kSelectPatchFile            = "selectPatchFile";
    constexpr const char* kOpenPatchFolder            = "openPatchFolder";
    constexpr const char* kSavePatchAs                = "savePatchAsFile";
    constexpr const char* kSavePatchFile              = "savePatchFile";    
}

} // namespace PluginDescriptors
