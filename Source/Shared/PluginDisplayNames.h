#pragma once

#include <juce_core/juce_core.h>

// PluginDisplayNames.h
// Central file for all plugin display names (GUI strings)
// Contains ONLY constant string definitions - no code/logic
// Single source of truth for all plugin display names

namespace PluginDescriptors
{

    // ============================================================================
    // Display Names | Plugin Name
    // ============================================================================

    constexpr const char* kPluginName                       = "MATRIX-CONTROL";
    
    // ============================================================================
    // Display Names | Plugin Modes
    // ============================================================================

    namespace ModeDisplayNames
    {
        constexpr const char* kMaster                       = "MASTER";
        constexpr const char* kPatch                        = "PATCH";
    }

    // ============================================================================
    // Display Names | Plugin Sections
    // ============================================================================

    namespace SectionDisplayNames
    {
        constexpr const char* kMasterEdit                   = "MASTER EDIT";
        constexpr const char* kPatchEdit                    = "PATCH EDIT";
        constexpr const char* kMatrixModulation             = "MATRIX MODULATION";
        constexpr const char* kPatchManager                 = "PATCH MANAGER";
    }

    // ============================================================================
    // Display Names | Plugin Modules
    // ============================================================================

    namespace ModuleDisplayNames
    {
        // Master Edit Module Display Names
        constexpr const char* kMidi                         = "MIDI";
        constexpr const char* kVibrato                      = "VIBRATO";
        constexpr const char* kMisc                         = "MISC";
        
        // Patch Edit Module Display Names
        constexpr const char* kDco1                         = "DCO 1";
        constexpr const char* kDco2                         = "DCO 2";
        constexpr const char* kVcfVca                       = "VCF/VCA";
        constexpr const char* kFmTrack                      = "FM/TRACK";
        constexpr const char* kRampPortamento               = "RAMP/PORTAMENTO";
        constexpr const char* kEnvelope1                    = "ENV 1";
        constexpr const char* kEnvelope2                    = "ENV 2";
        constexpr const char* kEnvelope3                    = "ENV 3";
        constexpr const char* kLfo1                         = "LFO 1";
        constexpr const char* kLfo2                         = "LFO 2";
        constexpr const char* kPatchName                    = "PATCH NAME";

        // Patch Manager Module Display Names
        constexpr const char* kBankUtility                  = "BANK UTILITY";
        constexpr const char* kInternalPatches              = "INTERNAL PATCHES";
        constexpr const char* kComputerPatches              = "COMPUTER PATCHES";
        constexpr const char* kPatchMutator                 = "PATCH MUTATOR";
    }

    // ============================================================================
    // Display Names | Plugin Modulation Busses
    // ============================================================================

    namespace ModulationBusDisplayNames
    {
        constexpr const char* kModulationBus0               = "0";
        constexpr const char* kModulationBus1               = "1";
        constexpr const char* kModulationBus2               = "2";
        constexpr const char* kModulationBus3               = "3";
        constexpr const char* kModulationBus4               = "4";
        constexpr const char* kModulationBus5               = "5";
        constexpr const char* kModulationBus6               = "6";
        constexpr const char* kModulationBus7               = "7";
        constexpr const char* kModulationBus8               = "8";
        constexpr const char* kModulationBus9               = "9";
    }

    // ============================================================================
    // Display Names | Synth Parameters
    // ============================================================================

    namespace ParameterDisplayNames
    {
        // Master Edit | MIDI Module Parameter Display Names
        constexpr const char* kMidiChannel                  = "CHANNEL";
        constexpr const char* kMidiEcho                     = "MIDI ECHO";
        constexpr const char* kMidiControllers              = "CONTROLLERS";
        constexpr const char* kMidiPatchChanges             = "PATCH CHANGES";
        constexpr const char* kMidiPedal1Select             = "PEDAL 1 SELECT";
        constexpr const char* kMidiPedal2Select             = "PEDAL 2 SELECT";
        constexpr const char* kMidiLever2Select             = "LEVER 2 SELECT";
        constexpr const char* kMidiLever3Select             = "LEVER 3 SELECT";

        // Master Edit | VIBRATO Module Parameter Display Names
        constexpr const char* kVibratoSpeed                 = "SPEED";
        constexpr const char* kVibratoSpeedModSource        = "SPEED MOD SOURCE";
        constexpr const char* kVibratoSpeedModAmount        = "SPEED MOD AMOUNT";
        constexpr const char* kVibratoWaveform              = "WAVEFORM";
        constexpr const char* kVibratoAmplitude             = "AMPLITUDE";
        constexpr const char* kVibratoAmpModSource          = "AMP MOD SOURCE";
        constexpr const char* kVibratoAmpModAmount          = "AMP MOD AMOUNT";

        // Master Edit | MISC Module Parameter Display Names
        constexpr const char* kMasterTune                   = "MASTER TUNE";
        constexpr const char* kMasterTranspose              = "MASTER TRANSPOSE";
        constexpr const char* kBendRange                    = "BEND RANGE (+/-)";
        constexpr const char* kUnisonEnable                 = "UNISON";
        constexpr const char* kVolumeInvertEnable           = "VOLUME INVERT";
        constexpr const char* kBankLockEnable               = "BANK LOCK";
        constexpr const char* kMemoryProtectEnable          = "MEMORY PROTECT";

        // Patch Edit | DCO 1 Module Parameter Display Names
        constexpr const char* kDco1Frequency                = "FREQUENCY";
        constexpr const char* kDco1FrequencyModByLfo1       = "FREQ < LFO 1";
        constexpr const char* kDco1PulseWidth               = "PULSE WIDTH";
        constexpr const char* kDco1PulseWidthModByLfo2      = "PW < LFO 2";
        constexpr const char* kDco1WaveShape                = "WAVE SHAPE";
        constexpr const char* kDco1Sync                     = "SYNC";
        constexpr const char* kDco1WaveSelect               = "WAVE SELECT";
        constexpr const char* kDco1Levers                   = "LEVERS";
        constexpr const char* kDco1KeyboardPortamento       = "KEYBD/PORTA";
        constexpr const char* kDco1KeyClick                 = "KEY CLICK";

        // Patch Edit | DCO 2 Module Parameter Display Names   
        constexpr const char* kDco2Frequency                = "FREQUENCY";
        constexpr const char* kDco2FrequencyModByLfo1       = "FREQ < LFO 1";
        constexpr const char* kDco2Detune                   = "DETUNE";
        constexpr const char* kDco2PulseWidth               = "PULSE WIDTH";
        constexpr const char* kDco2PulseWidthModByLfo2      = "PW < LFO 2";
        constexpr const char* kDco2WaveShape                = "WAVE SHAPE";
        constexpr const char* kDco2WaveSelect               = "WAVE SELECT";
        constexpr const char* kDco2Levers                   = "LEVERS";
        constexpr const char* kDco2KeyboardPortamento       = "KEYBD/PORTA";
        constexpr const char* kDco2KeyClick                 = "KEY CLICK";

        // Patch Edit | VCF/VCA Module Parameter Display Names
        constexpr const char* kVcfBalance                   = "BALANCE";
        constexpr const char* kVcfFequency                  = "FREQUENCY";
        constexpr const char* kVcfFrequencyModByEnv1        = "FREQ < ENV 1";
        constexpr const char* kVcfFrequencyModByPressure    = "FREQ < PRESSURE";
        constexpr const char* kVcfResonance                 = "RESONANCE";
        constexpr const char* kVca1Volume                   = "VCA 1 VOLUME";
        constexpr const char* kVca1ModByVelocity            = "VCA 1 < VELOCITY";
        constexpr const char* kVca2ModByEnv2                = "VCA 2 < ENV 2";
        constexpr const char* kVcfLevers                    = "LEVERS";
        constexpr const char* kVcfKeyboardPortamento        = "KEYBD/PORTA";

        // Patch Edit | FM/TRACK Module Parameter Display Names
        constexpr const char* kFmAmount                     = "VCF FM AMOUNT";
        constexpr const char* kFmModByEnv3                  = "FM < ENV 3";
        constexpr const char* kFmModByPressure              = "FM < PRESSURE";
        constexpr const char* kTrackPoint1                  = "TRACK POINT 1";
        constexpr const char* kTrackPoint2                  = "TRACK POINT 2";
        constexpr const char* kTrackPoint3                  = "TRACK POINT 3";
        constexpr const char* kTrackPoint4                  = "TRACK POINT 4";
        constexpr const char* kTrackPoint5                  = "TRACK POINT 5";
        constexpr const char* kTrackInput                   = "TRACK INPUT";

        // Patch Edit | RAMP/PORTAMENTO Module Parameter Display Names
        constexpr const char* kRamp1Rate                    = "RAMP 1 RATE";
        constexpr const char* kRamp2Rate                    = "RAMP 2 RATE";
        constexpr const char* kPortamentoRate               = "PORTAMENTO RATE";
        constexpr const char* kPortamentoModByVelocity      = "PORT < VELOCITY";
        constexpr const char* kRamp1Trigger                 = "RAMP 1 TRIGGER";
        constexpr const char* kRamp2Trigger                 = "RAMP 2 TRIGGER";
        constexpr const char* kPortamentoMode               = "PORTA MODE";
        constexpr const char* kPortamentoLegato             = "LEGATO PORTA";
        constexpr const char* kPortamentoKeyboardMode       = "KEYBOARD MODE";

        // Patch Edit | ENV 1 Module Parameter Display Names
        constexpr const char* kEnv1Delay                    = "DELAY";
        constexpr const char* kEnv1Attack                   = "ATTACK";
        constexpr const char* kEnv1Decay                    = "DECAY";
        constexpr const char* kEnv1Sustain                  = "SUSTAIN";
        constexpr const char* kEnv1Release                  = "RELEASE";
        constexpr const char* kEnv1Amplitude                = "AMPLITUDE";
        constexpr const char* kEnv1AmplitudeModByVelocity   = "AMP < VELOCITY";
        constexpr const char* kEnv1TriggerMode              = "TRIGGER MODE";
        constexpr const char* kEnv1EnvelopeMode             = "ENVELOPE MODE";
        constexpr const char* kEnv1Lfo1Trigger              = "LFO 1 TRIGGER";

        // Patch Edit | ENV 2 Module Parameter Display Names
        constexpr const char* kEnv2Delay                    = "DELAY";
        constexpr const char* kEnv2Attack                   = "ATTACK";
        constexpr const char* kEnv2Decay                    = "DECAY";
        constexpr const char* kEnv2Sustain                  = "SUSTAIN";
        constexpr const char* kEnv2Release                  = "RELEASE";
        constexpr const char* kEnv2Amplitude                = "AMPLITUDE";
        constexpr const char* kEnv2AmplitudeModByVelocity   = "AMP < VELOCITY";
        constexpr const char* kEnv2TriggerMode              = "TRIGGER MODE";
        constexpr const char* kEnv2EnvelopeMode             = "ENVELOPE MODE";
        constexpr const char* kEnv2Lfo1Trigger              = "LFO 1 TRIGGER";

        // Patch Edit | ENV 3 Module Parameter Display Names
        constexpr const char* kEnv3Delay                    = "DELAY";
        constexpr const char* kEnv3Attack                   = "ATTACK";
        constexpr const char* kEnv3Decay                    = "DECAY";
        constexpr const char* kEnv3Sustain                  = "SUSTAIN";
        constexpr const char* kEnv3Release                  = "RELEASE";
        constexpr const char* kEnv3Amplitude                = "AMPLITUDE";
        constexpr const char* kEnv3AmplitudeModByVelocity   = "AMP < VELOCITY";
        constexpr const char* kEnv3TriggerMode              = "TRIGGER MODE";
        constexpr const char* kEnv3EnvelopeMode             = "ENVELOPE MODE";
        constexpr const char* kEnv3Lfo1Trigger              = "LFO 1 TRIGGER";

        // Patch Edit | LFO 1 Module Parameter Display Names
        constexpr const char* kLfo1Speed                    = "SPEED";
        constexpr const char* kLfo1SpeedModByPressure       = "SPEED < PRESSURE";
        constexpr const char* kLfo1RetriggerPoint           = "RETRIGGER POINT";
        constexpr const char* kLfo1Amplitude                = "AMPLITUDE";
        constexpr const char* kLfo1AmplitudeModByRamp1      = "AMP < RAMP 1";
        constexpr const char* kLfo1Waveform                 = "WAVEFORM";
        constexpr const char* kLfo1TriggerMode              = "TRIGGER MODE";
        constexpr const char* kLfo1Lag                      = "LAG";
        constexpr const char* kLfo1SampleInput              = "SAMPLE INPUT";

        // Patch Edit | LFO 2 Module Parameter Display Names
        constexpr const char* kLfo2Speed                    = "SPEED";
        constexpr const char* kLfo2SpeedModByKeyboard       = "SPEED < KEYBD";
        constexpr const char* kLfo2RetriggerPoint           = "RETRIGGER POINT";
        constexpr const char* kLfo2Amplitude                = "AMPLITUDE";
        constexpr const char* kLfo2AmplitudeModByRamp2      = "AMP < RAMP 2";
        constexpr const char* kLfo2Waveform                 = "WAVEFORM";
        constexpr const char* kLfo2TriggerMode              = "TRIGGER MODE";
        constexpr const char* kLfo2Lag                      = "LAG";
        constexpr const char* kLfo2SampleInput              = "SAMPLE INPUT";

        // Matrix Modulation | Modulation Bus Header Display Names
        constexpr const char* kModulationBusNumber          = "#";
        constexpr const char* kModulationBusSource          = "SOURCE";
        constexpr const char* kModulationBusDestination     = "DESTINATION";
        constexpr const char* kModulationBusAmount          = "AMOUNT";
    }

    // ============================================================================
    // Display Names | Standalone Widgets
    // ============================================================================

    namespace StandaloneWidgetDisplayNames
    {
        // Short Label Display Names
        constexpr const char* kShortInitLabel               = "I";
        constexpr const char* kShortCopyLabel               = "C";
        constexpr const char* kShortPasteLabel              = "P";

        // Master Edit | MIDI Standalone Widget Display Names
        constexpr const char* kMidiInit                     = kShortInitLabel;

        // Master Edit | VIBRATO Standalone Widget Display Names
        constexpr const char* kVibratoInit                  = kShortInitLabel;

        // Master Edit | MISC Standalone Widget Display Names
        constexpr const char* kMiscInit                     = kShortInitLabel;

        // Patch Edit | DCO 1 Standalone Widget Display Names
        constexpr const char* kDco1Init                     = kShortInitLabel;
        constexpr const char* kDco1Copy                     = kShortCopyLabel;
        constexpr const char* kDco1Paste                    = kShortPasteLabel;

        // Patch Edit | DCO 2 Standalone Widget Display Names
        constexpr const char* kDco2Init                     = kShortInitLabel;
        constexpr const char* kDco2Copy                     = kShortCopyLabel;
        constexpr const char* kDco2Paste                    = kShortPasteLabel;

        // Patch Edit | VCF/VCA Standalone Widget Display Names
        constexpr const char* kVcfVcaInit                   = kShortInitLabel;

        // Patch Edit | FM/TRACK Standalone Widget Display Names
        constexpr const char* kFmTrackInit                  = kShortInitLabel;

        // Patch Edit | RAMP/PORTAMENTO Standalone Widget Display Names
        constexpr const char* kRampPortamentoInit           = kShortInitLabel;

        // Patch Edit | ENV 1 Standalone Widget Display Names
        constexpr const char* kEnv1Init                     = kShortInitLabel;
        constexpr const char* kEnv1Copy                     = kShortCopyLabel;
        constexpr const char* kEnv1Paste                    = kShortPasteLabel;

        // Patch Edit | ENV 2 Standalone Widget Display Names
        constexpr const char* kEnv2Init                     = kShortInitLabel;
        constexpr const char* kEnv2Copy                     = kShortCopyLabel;
        constexpr const char* kEnv2Paste                    = kShortPasteLabel;

        // Patch Edit | ENV 3 Standalone Widget Display Names
        constexpr const char* kEnv3Init                     = kShortInitLabel;
        constexpr const char* kEnv3Copy                     = kShortCopyLabel;
        constexpr const char* kEnv3Paste                    = kShortPasteLabel;

        // Patch Edit | LFO 1 Standalone Widget Display Names
        constexpr const char* kLfo1Init                     = kShortInitLabel;
        constexpr const char* kLfo1Copy                     = kShortCopyLabel;
        constexpr const char* kLfo1Paste                    = kShortPasteLabel;

        // Patch Edit | LFO 2 Standalone Widget Display Names
        constexpr const char* kLfo2Init                     = kShortInitLabel;
        constexpr const char* kLfo2Copy                     = kShortCopyLabel;
        constexpr const char* kLfo2Paste                    = kShortPasteLabel;

        // Patch Edit | Displays Standalone Widget Display Names
        constexpr const char* kDefaultPatchName             = "--------";

        // Patch Manager | Bank Utility Standalone Widget Display Names
        constexpr const char* kBankSelector                 = "BANK SELECTOR";
        constexpr const char* kUnlockBank                   = "UNLOCK";
        constexpr const char* kSelectBank0                  = "0";
        constexpr const char* kSelectBank1                  = "1";
        constexpr const char* kSelectBank2                  = "2";
        constexpr const char* kSelectBank3                  = "3";
        constexpr const char* kSelectBank4                  = "4";
        constexpr const char* kSelectBank5                  = "5";
        constexpr const char* kSelectBank6                  = "6";
        constexpr const char* kSelectBank7                  = "7";
        constexpr const char* kSelectBank8                  = "8";
        constexpr const char* kSelectBank9                  = "9";

        // Patch Manager | Internal Patches Standalone Widget Display Names
        constexpr const char* kInternalPatchesBrowser       = "BROWSER";
        constexpr const char* kInternalPatchesMemory        = "MEMORY";
        constexpr const char* kLoadPreviousPatch            = "<";
        constexpr const char* kLoadNextPatch                = ">";
        constexpr const char* kCurrentBankNumber            = "CURRENT BANK";
        constexpr const char* kCurrentPatchNumber           = "CURRENT PATCH";
        constexpr const char* kInitPatch                    = "INIT";
        constexpr const char* kCopyPatch                    = "COPY";
        constexpr const char* kPastePatch                   = "PASTE";
        constexpr const char* kStorePatch                   = "STORE";

        // Patch Manager | Computer Patches Standalone Widget Display Names
        constexpr const char* kComputerPatchesBrowser       = "BROWSER";
        constexpr const char* kComputerPatchesStorage       = "STORAGE";
        constexpr const char* kLoadPreviousPatchFile        = "<";
        constexpr const char* kLoadNextPatchFile            = ">";
        constexpr const char* kSelectPatchFile              = "SELECT A PATCH";
        constexpr const char* kOpenPatchFolder              = "OPEN";
        constexpr const char* kSavePatchAsFile              = "SAVE AS";
        constexpr const char* kSavePatch                    = "SAVE";

        // Patch Manager | Patch Mutator Standalone Widget Display Names
        constexpr const char* kPatchMutatorAmount             = "AMOUNT";
        constexpr const char* kPatchMutatorRandom             = "RANDOM";
        constexpr const char* kPatchMutatorHistory            = "HISTORY";
        constexpr const char* kPatchMutatorEmptyHistory       = "EMPTY";
        
        constexpr const char* kPatchMutatorMutate             = "MUTATE";
        constexpr const char* kPatchMutatorRetry              = "RETRY";
        constexpr const char* kPatchMutatorCompare            = "COMPARE";
        const juce::String    kPatchMutatorDelete             = juce::String::fromUTF8("\xe2\x80\x94");

        constexpr const char* kPatchMutatorClear              = "CLEAR";
        constexpr const char* kPatchMutatorExport             = "EXPORT";

        constexpr const char* kPatchMutatorD1                 = "D1";
        constexpr const char* kPatchMutatorD2                 = "D2";
        constexpr const char* kPatchMutatorFByA               = "F/A";
        constexpr const char* kPatchMutatorFByT               = "F/T";
        constexpr const char* kPatchMutatorRByP               = "R/P";
        constexpr const char* kPatchMutatorE1                 = "E1";
        constexpr const char* kPatchMutatorE2                 = "E2";
        constexpr const char* kPatchMutatorE3                 = "E3";
        constexpr const char* kPatchMutatorL1                 = "L1";
        constexpr const char* kPatchMutatorL2                 = "L2";
        constexpr const char* kPatchMutatorUnitPercent        = "%";
    }

    // ============================================================================
    // Display Names | Choice Lists (for ComboBox)
    // ============================================================================

    namespace ChoiceLists
    {
        // Sync Choice Display Names
        constexpr const char* kSyncOff                      = "OFF";
        constexpr const char* kSyncSoft                     = "SOFT";
        constexpr const char* kSyncMedium                   = "MEDIUM";
        constexpr const char* kSyncHard                     = "HARD";

        // Wave Select Choice Display Names
        constexpr const char* kWaveSelectOff                = "OFF";
        constexpr const char* kWaveSelectPulse              = "PULSE";
        constexpr const char* kWaveSelectWave               = "WAVE";
        constexpr const char* kWaveSelectBoth               = "BOTH";
        constexpr const char* kWaveSelectNoise              = "NOISE";

        // Levers Choice Display Names
        constexpr const char* kLeversOff                    = "OFF";
        constexpr const char* kLeversL1Bend                 = "L1/BEND";
        constexpr const char* kLeversL2Vib                  = "L2/VIB";
        constexpr const char* kLeversBoth                   = "BOTH";

        // Keyboard/Portamento Choice Display Names
        constexpr const char* kKeyboardPortamentoOff        = "OFF";
        constexpr const char* kKeyboardPortamentoKeybd      = "KEYBD";
        constexpr const char* kKeyboardPortamentoPorta      = "PORTA";

        // On/Off Choice Display Names
        constexpr const char* kOnOffChoiceOff               = "OFF";
        constexpr const char* kOnOffChoiceOn                = "ON";

        // Trigger Mode Choice Display Names (Envelopes)
        constexpr const char* kTriggerModeStrig             = "STRIG";
        constexpr const char* kTriggerModeMtrig             = "MTRIG";
        constexpr const char* kTriggerModeSreset            = "SRESET";
        constexpr const char* kTriggerModeMreset            = "MRESET";
        constexpr const char* kTriggerModeXtrig             = "XTRIG";
        constexpr const char* kTriggerModeXmtrig            = "XMTRIG";
        constexpr const char* kTriggerModeXreset            = "XRESET";
        constexpr const char* kTriggerModeXmrst             = "XMRST";

        // Envelope Mode Choice Display Names
        constexpr const char* kEnvelopeModeNormal           = "NORMAL";
        constexpr const char* kEnvelopeModeDadr             = "DADR";
        constexpr const char* kEnvelopeModeFree             = "FREE";
        constexpr const char* kEnvelopeModeBoth             = "BOTH";

        // LFO 1 Trigger Choice Display Names
        constexpr const char* kLfo1TriggerNormal            = "NORMAL";
        constexpr const char* kLfo1TriggerLfo1              = "LFO 1";
        constexpr const char* kLfo1TriggerGLfo1             = "G-LFO 1";
        constexpr const char* kLfo1TriggerGatedLfo1Trigger  = "GATED LFO 1 TRIGGER";

        // LFO Trigger Mode Choice Display Names
        constexpr const char* kLfoTriggerModeOff            = "OFF";
        constexpr const char* kLfoTriggerModeStrig          = "STRIG";
        constexpr const char* kLfoTriggerModeMtrig          = "MTRIG";
        constexpr const char* kLfoTriggerModeXtrig          = "XTRIG";

        // LFO Waveform Choice Display Names
        constexpr const char* kLfoWaveformTriangle          = "TRIANGLE";
        constexpr const char* kLfoWaveformUpsaw             = "UPSAW";
        constexpr const char* kLfoWaveformDnsaw             = "DNSAW";
        constexpr const char* kLfoWaveformSquare            = "SQUARE";
        constexpr const char* kLfoWaveformRandom            = "RANDOM";
        constexpr const char* kLfoWaveformNoise             = "NOISE";
        constexpr const char* kLfoWaveformSampled           = "SAMPLED";

        // Ramp Trigger Choice Display Names
        constexpr const char* kRampTriggerStrig             = "STRIG";
        constexpr const char* kRampTriggerMtrig             = "MTRIG";
        constexpr const char* kRampTriggerExtrig            = "EXTRIG";
        constexpr const char* kRampTriggerGatedx            = "GATEDX";

        // Portamento Mode Choice Display Names
        constexpr const char* kPortamentoModeLinear         = "LINEAR";
        constexpr const char* kPortamentoModeConst          = "CONST";
        constexpr const char* kPortamentoModeExpo           = "EXPO";

        // Portamento Keyboard Mode Choice Display Names
        constexpr const char* kPortamentoKeyboardModeRotate = "ROTATE";
        constexpr const char* kPortamentoKeyboardModeReasgn = "REASGN";
        constexpr const char* kPortamentoKeyboardModeUnison = "UNISON";
        constexpr const char* kPortamentoKeyboardModeRearob = "REAROB";

        // Source/Destination Choice Display Names (Matrix Modulation and Sample Input)
        constexpr const char* kSourceNone                   = "NONE";
        constexpr const char* kSourceEnv1                   = "ENV 1";
        constexpr const char* kSourceEnv2                   = "ENV 2";
        constexpr const char* kSourceEnv3                   = "ENV 3";
        constexpr const char* kSourceLfo1                   = "LFO 1";
        constexpr const char* kSourceLfo2                   = "LFO 2";
        constexpr const char* kSourceVibrato                = "VIBRATO";
        constexpr const char* kSourceRamp1                  = "RAMP 1";
        constexpr const char* kSourceRamp2                  = "RAMP 2";
        constexpr const char* kSourceKeybd                  = "KEYBD";
        constexpr const char* kSourcePorta                  = "PORTA";
        constexpr const char* kSourceTrack                  = "TRACK";
        constexpr const char* kSourceKbGate                 = "KB GATE";
        constexpr const char* kSourceVelocity               = "VELOCITY";
        constexpr const char* kSourceRelVel                 = "REL VEL";
        constexpr const char* kSourcePressure               = "PRESSURE";
        constexpr const char* kSourcePedal1                 = "PEDAL 1";
        constexpr const char* kSourcePedal2                 = "PEDAL 2";
        constexpr const char* kSourceLever1                 = "LEVER 1";
        constexpr const char* kSourceLever2                 = "LEVER 2";
        constexpr const char* kSourceLever3                 = "LEVER 3";

        // Destination Choice Display Names (Matrix Modulation)
        constexpr const char* kDestinationNone              = "NONE";
        constexpr const char* kDestinationDco1Frequency     = "DCO 1 FREQUENCY";
        constexpr const char* kDestinationDco1PulseWidth    = "DCO 1 PULSE WIDTH";
        constexpr const char* kDestinationDco1WaveShape     = "DCO 1 WAVE SHAPE";
        constexpr const char* kDestinationDco2Frequency     = "DCO 2 FREQUENCY";
        constexpr const char* kDestinationDco2PulseWidth    = "DCO 2 PULSE WIDTH";
        constexpr const char* kDestinationDco2WaveShape     = "DCO 2 WAVE SHAPE";
        constexpr const char* kDestinationDco1Dco2Mix       = "DCO 1 | DCO 2 MIX";
        constexpr const char* kDestinationVcfFmByDco1       = "VCF FM BY DCO 1";
        constexpr const char* kDestinationVcfFrequency      = "VCF FREQUENCY";
        constexpr const char* kDestinationVcfResonance      = "VCF RESONANCE";
        constexpr const char* kDestinationVca1Volume        = "VCA 1 VOLUME";
        constexpr const char* kDestinationVca2Volume        = "VCA 2 VOLUME";
        constexpr const char* kDestinationEnv1Delay         = "ENV 1 DELAY";
        constexpr const char* kDestinationEnv1Attack        = "ENV 1 ATTACK";
        constexpr const char* kDestinationEnv1Decay         = "ENV 1 DECAY";
        constexpr const char* kDestinationEnv1Release       = "ENV 1 RELEASE";
        constexpr const char* kDestinationEnv1Amplitude     = "ENV 1 AMPLITUDE";
        constexpr const char* kDestinationEnv2Delay         = "ENV 2 DELAY";
        constexpr const char* kDestinationEnv2Attack        = "ENV 2 ATTACK";
        constexpr const char* kDestinationEnv2Decay         = "ENV 2 DECAY";
        constexpr const char* kDestinationEnv2Release       = "ENV 2 RELEASE";
        constexpr const char* kDestinationEnv2Amplitude     = "ENV 2 AMPLITUDE";
        constexpr const char* kDestinationEnv3Delay         = "ENV 3 DELAY";
        constexpr const char* kDestinationEnv3Attack        = "ENV 3 ATTACK";
        constexpr const char* kDestinationEnv3Decay         = "ENV 3 DECAY";
        constexpr const char* kDestinationEnv3Release       = "ENV 3 RELEASE";
        constexpr const char* kDestinationEnv3Amplitude     = "ENV 3 AMPLITUDE";
        constexpr const char* kDestinationLfo1Speed         = "LFO 1 SPEED";
        constexpr const char* kDestinationLfo1Amplitude     = "LFO 1 AMPLITUDE";
        constexpr const char* kDestinationLfo2Speed         = "LFO 2 SPEED";
        constexpr const char* kDestinationLfo2Amplitude     = "LFO 2 AMPLITUDE";
        constexpr const char* kDestinationPortamentoRate    = "PORTAMENTO RATE";

        // MIDI Channel Choice Display Names
        constexpr const char* kMidiChannelOmni              = "OMNI";
        constexpr const char* kMidiChannel1                 = "1";
        constexpr const char* kMidiChannel2                 = "2";
        constexpr const char* kMidiChannel3                 = "3";
        constexpr const char* kMidiChannel4                 = "4";
        constexpr const char* kMidiChannel5                 = "5";
        constexpr const char* kMidiChannel6                 = "6";
        constexpr const char* kMidiChannel7                 = "7";
        constexpr const char* kMidiChannel8                 = "8";
        constexpr const char* kMidiChannel9                 = "9";
        constexpr const char* kMidiChannel10                = "10";
        constexpr const char* kMidiChannel11                = "11";
        constexpr const char* kMidiChannel12                = "12";
        constexpr const char* kMidiChannel13                = "13";
        constexpr const char* kMidiChannel14                = "14";
        constexpr const char* kMidiChannel15                = "15";
        constexpr const char* kMidiChannel16                = "16";
        constexpr const char* kMidiChannelMonoG1            = "MONO G1";
        constexpr const char* kMidiChannelMonoG2            = "MONO G2";
        constexpr const char* kMidiChannelMonoG3            = "MONO G3";
        constexpr const char* kMidiChannelMonoG4            = "MONO G4";
        constexpr const char* kMidiChannelMonoG5            = "MONO G5";
        constexpr const char* kMidiChannelMonoG6            = "MONO G6";
        constexpr const char* kMidiChannelMonoG7            = "MONO G7";
        constexpr const char* kMidiChannelMonoG8            = "MONO G8";
        constexpr const char* kMidiChannelMonoG9            = "MONO G9";
    }

} // namespace PluginDescriptors
