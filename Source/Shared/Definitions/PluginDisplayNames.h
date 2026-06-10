#pragma once

#include <juce_core/juce_core.h>

// PluginDisplayNames.h
// Central file for all plugin display names (GUI strings)
// Contains ONLY constant string definitions - no code/logic
// Single source of truth for all plugin display names

namespace PluginDisplayNames
{
    constexpr const char* kPluginName = "MATRIX-CONTROL";

    namespace HeaderPanel
    {
        constexpr const char* kEditorMidiFromLabel = "EDITOR MIDI FROM :";
        constexpr const char* kMidiToLabel = "MIDI TO :";
        constexpr const char* kKeyboardFromLabel = "KEYBOARD FROM :";
        constexpr const char* kHostDisplay = "HOST";
        constexpr const char* kPortNoneSentinel = "\xe2\x80\x94";
        constexpr const char* kSettingsButton = "SETTINGS";
        constexpr const char* kUiElementsButton = "UI Elements";
    }

    namespace Settings
    {
        const juce::String kWindowTitle = juce::String::fromUTF8("MATRIX-CONTROL \xe2\x80\x94 SETTINGS");
        constexpr const char* kSkinLabel = "SKIN :";
        constexpr const char* kUiScaleLabel = "UI SCALE :";
        constexpr const char* kHardwareLatencyLabel = "HARDWARE LATENCY :";
        constexpr const char* kAudioFromLabel = "AUDIO FROM :";
        constexpr const char* kInputGainLabel = "INPUT GAIN :";
        constexpr const char* kMasterOperationsSection = "MASTER OPERATIONS";
        constexpr const char* kPoliciesSection = "POLICIES";
        constexpr const char* kDefragSection = "DEFRAG";
        constexpr const char* kLoggingSection = "LOGGING";
        constexpr const char* kComingSoon = "Coming soon";
    }

    namespace Units
    {
        constexpr const char* kPercent = "%";
    }

    namespace ShortLabels
    {
        constexpr const char* kInit  = "I"; // Init buttons
        constexpr const char* kCopy  = "C"; // Copy buttons
        constexpr const char* kPaste = "P"; // Paste buttons
    }

    namespace ChoiceLists
    {
        namespace SkinVariants
        {
            constexpr const char* kBlack = "BLACK";
            constexpr const char* kCream = "CREAM";
        }

        namespace ScaleLevels
        {
            constexpr const char* k50  = "50%";
            constexpr const char* k75  = "75%";
            constexpr const char* k100 = "100%";
            constexpr const char* k125 = "125%";
            constexpr const char* k150 = "150%";
            constexpr const char* k175 = "175%";
            constexpr const char* k200 = "200%";
        }

        namespace MidiChannel
        {
            constexpr const char* kOmni      = "OMNI";
            constexpr const char* kChannel1  = "CHANNEL 1";
            constexpr const char* kChannel2  = "CHANNEL 2";
            constexpr const char* kChannel3  = "CHANNEL 3";
            constexpr const char* kChannel4  = "CHANNEL 4";
            constexpr const char* kChannel5  = "CHANNEL 5";
            constexpr const char* kChannel6  = "CHANNEL 6";
            constexpr const char* kChannel7  = "CHANNEL 7";
            constexpr const char* kChannel8  = "CHANNEL 8";
            constexpr const char* kChannel9  = "CHANNEL 9";
            constexpr const char* kChannel10 = "CHANNEL 10";
            constexpr const char* kChannel11 = "CHANNEL 11";
            constexpr const char* kChannel12 = "CHANNEL 12";
            constexpr const char* kChannel13 = "CHANNEL 13";
            constexpr const char* kChannel14 = "CHANNEL 14";
            constexpr const char* kChannel15 = "CHANNEL 15";
            constexpr const char* kChannel16 = "CHANNEL 16";
            constexpr const char* kMonoG1    = "MONO G1";
            constexpr const char* kMonoG2    = "MONO G2";
            constexpr const char* kMonoG3    = "MONO G3";
            constexpr const char* kMonoG4    = "MONO G4";
            constexpr const char* kMonoG5    = "MONO G5";
            constexpr const char* kMonoG6    = "MONO G6";
            constexpr const char* kMonoG7    = "MONO G7";
            constexpr const char* kMonoG8    = "MONO G8";
            constexpr const char* kMonoG9    = "MONO G9";
        }
        
        namespace Sync
        {
            constexpr const char* kOff    = "OFF";
            constexpr const char* kSoft   = "SOFT";
            constexpr const char* kMedium = "MEDIUM";
            constexpr const char* kHard   = "HARD";
        }

        namespace WaveSelect
        {
            constexpr const char* kOff   = "OFF";
            constexpr const char* kPulse = "PULSE";
            constexpr const char* kWave  = "WAVE";
            constexpr const char* kBoth  = "BOTH";
            constexpr const char* kNoise = "NOISE";
        }

        namespace Levers
        {
            constexpr const char* kOff           = "OFF";
            constexpr const char* kLever1Bend    = "L1/BEND";
            constexpr const char* kLever2Vibrato = "L2/VIB";
            constexpr const char* kBoth          = "BOTH";
        }

        namespace KeyboardPortamento
        {
            constexpr const char* kOff        = "OFF";
            constexpr const char* kKeyboard   = "KEYBD";
            constexpr const char* kPortamento = "PORTA";
        }

        namespace OnOff
        {
            constexpr const char* kOff = "OFF";
            constexpr const char* kOn  = "ON";
        }

        namespace TriggerMode
        {
            constexpr const char* kStrig  = "STRIG";
            constexpr const char* kMtrig  = "MTRIG";
            constexpr const char* kSreset = "SRESET";
            constexpr const char* kMreset = "MRESET";
            constexpr const char* kXtrig  = "XTRIG";
            constexpr const char* kXmtrig = "XMTRIG";
            constexpr const char* kXreset = "XRESET";
            constexpr const char* kXmrst  = "XMRST";
        }

        namespace EnvelopeMode
        {
            constexpr const char* kNormal = "NORMAL";
            constexpr const char* kDadr   = "DADR";
            constexpr const char* kFree   = "FREE";
            constexpr const char* kBoth   = "BOTH";
        }

        namespace Lfo1Trigger
        {
            constexpr const char* kNormal           = "NORMAL";
            constexpr const char* kLfo1             = "LFO 1";
            constexpr const char* kGLfo1            = "G-LFO 1";
            constexpr const char* kGatedLfo1Trigger = "GATED LFO 1 TRIGGER";
        }

        namespace LfoTriggerMode
        {
            constexpr const char* kOff   = "OFF";
            constexpr const char* kStrig = "STRIG";
            constexpr const char* kMtrig = "MTRIG";
            constexpr const char* kXtrig = "XTRIG";
        }

        namespace LfoWaveform
        {
            constexpr const char* kTriangle = "TRIANGLE";
            constexpr const char* kUpsaw    = "UPSAW";
            constexpr const char* kDnsaw    = "DNSAW";
            constexpr const char* kSquare   = "SQUARE";
            constexpr const char* kRandom   = "RANDOM";
            constexpr const char* kNoise    = "NOISE";
            constexpr const char* kSampled  = "SAMPLED";
        }

        namespace RampTrigger
        {
            constexpr const char* kStrig  = "STRIG";
            constexpr const char* kMtrig  = "MTRIG";
            constexpr const char* kExtrig = "EXTRIG";
            constexpr const char* kGatedx = "GATEDX";
        }

        namespace PortamentoMode
        {
            constexpr const char* kLinear = "LINEAR";
            constexpr const char* kConst  = "CONST";
            constexpr const char* kExpo   = "EXPO";
        }

        namespace PortamentoKeyboardMode
        {
            constexpr const char* kRotate = "ROTATE";
            constexpr const char* kReasgn = "REASGN";
            constexpr const char* kUnison = "UNISON";
            constexpr const char* kRearob = "REAROB";
        }

        namespace ModulationBus
        {
            namespace Source
            {
                constexpr const char* kNone            = "NONE";
                constexpr const char* kEnvelope1       = "ENV 1";
                constexpr const char* kEnvelope2       = "ENV 2";
                constexpr const char* kEnvelope3       = "ENV 3";
                constexpr const char* kLfo1            = "LFO 1";
                constexpr const char* kLfo2            = "LFO 2";
                constexpr const char* kVibrato         = "VIBRATO";
                constexpr const char* kRamp1           = "RAMP 1";
                constexpr const char* kRamp2           = "RAMP 2";
                constexpr const char* kKeyboard        = "KEYBD";
                constexpr const char* kPortamento      = "PORTA";
                constexpr const char* kTrack           = "TRACK";
                constexpr const char* kKeyboardGate    = "KB GATE";
                constexpr const char* kVelocity        = "VELOCITY";
                constexpr const char* kReleaseVelocity = "REL VEL";
                constexpr const char* kPressure        = "PRESSURE";
                constexpr const char* kPedal1          = "PEDAL 1";
                constexpr const char* kPedal2          = "PEDAL 2";
                constexpr const char* kLever1          = "LEVER 1";
                constexpr const char* kLever2          = "LEVER 2";
                constexpr const char* kLever3          = "LEVER 3";
            }

            namespace Destination
            {
                constexpr const char* kNone           = "NONE";
                constexpr const char* kDco1Frequency  = "DCO 1 FREQUENCY";
                constexpr const char* kDco1PulseWidth = "DCO 1 PULSE WIDTH";
                constexpr const char* kDco1WaveShape  = "DCO 1 WAVE SHAPE";
                constexpr const char* kDco2Frequency  = "DCO 2 FREQUENCY";
                constexpr const char* kDco2PulseWidth = "DCO 2 PULSE WIDTH";
                constexpr const char* kDco2WaveShape  = "DCO 2 WAVE SHAPE";
                constexpr const char* kDco1Dco2Mix    = "DCO 1 | DCO 2 MIX";
                constexpr const char* kVcfFmByDco1    = "VCF FM BY DCO 1";
                constexpr const char* kVcfFrequency   = "VCF FREQUENCY";
                constexpr const char* kVcfResonance   = "VCF RESONANCE";
                constexpr const char* kVca1Volume     = "VCA 1 VOLUME";
                constexpr const char* kVca2Volume     = "VCA 2 VOLUME";
                constexpr const char* kEnv1Delay      = "ENV 1 DELAY";
                constexpr const char* kEnv1Attack     = "ENV 1 ATTACK";
                constexpr const char* kEnv1Decay      = "ENV 1 DECAY";
                constexpr const char* kEnv1Release    = "ENV 1 RELEASE";
                constexpr const char* kEnv1Amplitude  = "ENV 1 AMPLITUDE";
                constexpr const char* kEnv2Delay      = "ENV 2 DELAY";
                constexpr const char* kEnv2Attack     = "ENV 2 ATTACK";
                constexpr const char* kEnv2Decay      = "ENV 2 DECAY";
                constexpr const char* kEnv2Release    = "ENV 2 RELEASE";
                constexpr const char* kEnv2Amplitude  = "ENV 2 AMPLITUDE";
                constexpr const char* kEnv3Delay      = "ENV 3 DELAY";
                constexpr const char* kEnv3Attack     = "ENV 3 ATTACK";
                constexpr const char* kEnv3Decay      = "ENV 3 DECAY";
                constexpr const char* kEnv3Release    = "ENV 3 RELEASE";
                constexpr const char* kEnv3Amplitude  = "ENV 3 AMPLITUDE";
                constexpr const char* kLfo1Speed      = "LFO 1 SPEED";
                constexpr const char* kLfo1Amplitude  = "LFO 1 AMPLITUDE";
                constexpr const char* kLfo2Speed      = "LFO 2 SPEED";
                constexpr const char* kLfo2Amplitude  = "LFO 2 AMPLITUDE";
                constexpr const char* kPortamentoRate = "PORTAMENTO RATE";
            }
        }
    }

    namespace Mode
    {
        constexpr const char* kMaster = "MASTER";
        constexpr const char* kPatch  = "PATCH";
    }

    namespace MasterEditSection
    {
        constexpr const char* kName = "MASTER EDIT";

        namespace MidiModule
        {
            constexpr const char* kName = "MIDI";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = ShortLabels::kInit;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kChannel      = "CHANNEL";
                constexpr const char* kMidiEcho     = "MIDI ECHO";
                constexpr const char* kControllers  = "CONTROLLERS";
                constexpr const char* kPatchChanges = "PATCH CHANGES";
                constexpr const char* kPedal1Select = "PEDAL 1 SELECT";
                constexpr const char* kPedal2Select = "PEDAL 2 SELECT";
                constexpr const char* kLever2Select = "LEVER 2 SELECT";
                constexpr const char* kLever3Select = "LEVER 3 SELECT";
            }
        }

        namespace VibratoModule
        {
            constexpr const char* kName = "VIBRATO";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = ShortLabels::kInit;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kSpeed          = "SPEED";
                constexpr const char* kSpeedModSource = "SPEED MOD SOURCE";
                constexpr const char* kSpeedModAmount = "SPEED MOD AMOUNT";
                constexpr const char* kWaveform       = "WAVEFORM";
                constexpr const char* kAmplitude      = "AMPLITUDE";
                constexpr const char* kAmpModSource   = "AMP MOD SOURCE";
                constexpr const char* kAmpModAmount   = "AMP MOD AMOUNT";
            }
        }

        namespace MiscModule
        {
            constexpr const char* kName = "MISC";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = ShortLabels::kInit;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kMasterTune          = "MASTER TUNE";
                constexpr const char* kMasterTranspose     = "MASTER TRANSPOSE";
                constexpr const char* kBendRange           = "BEND RANGE (+/-)";
                constexpr const char* kUnisonEnable        = "UNISON";
                constexpr const char* kVolumeInvertEnable  = "VOLUME INVERT";
                constexpr const char* kBankLockEnable      = "BANK LOCK";
                constexpr const char* kMemoryProtectEnable = "MEMORY PROTECT";
            }
        }
    }

    namespace PatchEditSection
    {
        constexpr const char* kName = "PATCH EDIT";

        namespace Dco1Module
        {
            constexpr const char* kName = "DCO 1";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kFrequency           = "FREQUENCY";
                constexpr const char* kFrequencyModByLfo1  = "FREQ < LFO 1";
                constexpr const char* kPulseWidth          = "PULSE WIDTH";
                constexpr const char* kPulseWidthModByLfo2 = "PW < LFO 2";
                constexpr const char* kWaveShape           = "WAVE SHAPE";
                constexpr const char* kSync                = "SYNC";
                constexpr const char* kWaveSelect          = "WAVE SELECT";
                constexpr const char* kLevers              = "LEVERS";
                constexpr const char* kKeyboardPortamento  = "KEYBD/PORTA";
                constexpr const char* kKeyClick            = "KEY CLICK";
            }
        }

        namespace Dco2Module
        {
            constexpr const char* kName = "DCO 2";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kFrequency           = "FREQUENCY";
                constexpr const char* kFrequencyModByLfo1  = "FREQ < LFO 1";
                constexpr const char* kDetune              = "DETUNE";
                constexpr const char* kPulseWidth          = "PULSE WIDTH";
                constexpr const char* kPulseWidthModByLfo2 = "PW < LFO 2";
                constexpr const char* kWaveShape           = "WAVE SHAPE";
                constexpr const char* kWaveSelect          = "WAVE SELECT";
                constexpr const char* kLevers              = "LEVERS";
                constexpr const char* kKeyboardPortamento  = "KEYBD/PORTA";
                constexpr const char* kKeyClick            = "KEY CLICK";
            }
        }

        namespace VcfVcaModule
        {
            constexpr const char* kName = "VCF/VCA";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = ShortLabels::kInit;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kBalance                = "BALANCE";
                constexpr const char* kFrequency              = "FREQUENCY";
                constexpr const char* kFrequencyModByEnv1     = "FREQ < ENV 1";
                constexpr const char* kFrequencyModByPressure = "FREQ < PRESSURE";
                constexpr const char* kResonance              = "RESONANCE";
                constexpr const char* kVca1Volume             = "VCA 1 VOLUME";
                constexpr const char* kVca1ModByVelocity      = "VCA 1 < VELOCITY";
                constexpr const char* kVca2ModByEnv2          = "VCA 2 < ENV 2";
                constexpr const char* kLevers                 = "LEVERS";
                constexpr const char* kKeyboardPortamento     = "KEYBD/PORTA";
            }
        }

        namespace FmTrackModule
        {
            constexpr const char* kName = "FM/TRACK";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = ShortLabels::kInit;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kFmAmount        = "VCF FM AMOUNT";
                constexpr const char* kFmModByEnv3     = "FM < ENV 3";
                constexpr const char* kFmModByPressure = "FM < PRESSURE";
                constexpr const char* kTrackPoint1     = "TRACK POINT 1";
                constexpr const char* kTrackPoint2     = "TRACK POINT 2";
                constexpr const char* kTrackPoint3     = "TRACK POINT 3";
                constexpr const char* kTrackPoint4     = "TRACK POINT 4";
                constexpr const char* kTrackPoint5     = "TRACK POINT 5";
                constexpr const char* kTrackInput      = "TRACK INPUT";
            }
        }

        namespace RampPortamentoModule
        {
            constexpr const char* kName = "RAMP/PORTAMENTO";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = ShortLabels::kInit;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kRamp1Rate               = "RAMP 1 RATE";
                constexpr const char* kRamp2Rate               = "RAMP 2 RATE";
                constexpr const char* kPortamentoRate          = "PORTAMENTO RATE";
                constexpr const char* kPortamentoModByVelocity = "PORTA < VELOCITY";
                constexpr const char* kRamp1Trigger            = "RAMP 1 TRIGGER";
                constexpr const char* kRamp2Trigger            = "RAMP 2 TRIGGER";
                constexpr const char* kPortamentoMode          = "PORTA MODE";
                constexpr const char* kPortamentoLegato        = "LEGATO PORTA";
                constexpr const char* kPortamentoKeyboardMode  = "KEYBOARD MODE";
            }
        }

        namespace Envelope1Module
        {
            constexpr const char* kName = "ENV 1";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kDelay                  = "DELAY";
                constexpr const char* kAttack                 = "ATTACK";
                constexpr const char* kDecay                  = "DECAY";
                constexpr const char* kSustain                = "SUSTAIN";
                constexpr const char* kRelease                = "RELEASE";
                constexpr const char* kAmplitude              = "AMPLITUDE";
                constexpr const char* kAmplitudeModByVelocity = "AMP < VELOCITY";
                constexpr const char* kTriggerMode            = "TRIGGER MODE";
                constexpr const char* kEnvelopeMode           = "ENVELOPE MODE";
                constexpr const char* kLfo1Trigger            = "LFO 1 TRIGGER";
            }
        }

        namespace Envelope2Module
        {
            constexpr const char* kName = "ENV 2";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kDelay                  = "DELAY";
                constexpr const char* kAttack                 = "ATTACK";
                constexpr const char* kDecay                  = "DECAY";
                constexpr const char* kSustain                = "SUSTAIN";
                constexpr const char* kRelease                = "RELEASE";
                constexpr const char* kAmplitude              = "AMPLITUDE";
                constexpr const char* kAmplitudeModByVelocity = "AMP < VELOCITY";
                constexpr const char* kTriggerMode            = "TRIGGER MODE";
                constexpr const char* kEnvelopeMode           = "ENVELOPE MODE";
                constexpr const char* kLfo1Trigger            = "LFO 1 TRIGGER";
            }
        }

        namespace Envelope3Module
        {
            constexpr const char* kName = "ENV 3";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kDelay                  = "DELAY";
                constexpr const char* kAttack                 = "ATTACK";
                constexpr const char* kDecay                  = "DECAY";
                constexpr const char* kSustain                = "SUSTAIN";
                constexpr const char* kRelease                = "RELEASE";
                constexpr const char* kAmplitude              = "AMPLITUDE";
                constexpr const char* kAmplitudeModByVelocity = "AMP < VELOCITY";
                constexpr const char* kTriggerMode            = "TRIGGER MODE";
                constexpr const char* kEnvelopeMode           = "ENVELOPE MODE";
                constexpr const char* kLfo1Trigger            = "LFO 1 TRIGGER";
            }
        }

        namespace Lfo1Module
        {
            constexpr const char* kName = "LFO 1";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kSpeed               = "SPEED";
                constexpr const char* kSpeedModByPressure  = "SPEED < PRESSURE";
                constexpr const char* kRetriggerPoint      = "RETRIGGER POINT";
                constexpr const char* kAmplitude           = "AMPLITUDE";
                constexpr const char* kAmplitudeModByRamp1 = "AMP < RAMP 1";
                constexpr const char* kWaveform            = "WAVEFORM";
                constexpr const char* kTriggerMode         = "TRIGGER MODE";
                constexpr const char* kLag                 = "LAG";
                constexpr const char* kSampleInput         = "SAMPLE INPUT";
            }
        }

        namespace Lfo2Module
        {
            constexpr const char* kName = "LFO 2";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kSpeed               = "SPEED";
                constexpr const char* kSpeedModByKeyboard  = "SPEED < KEYBD";
                constexpr const char* kRetriggerPoint      = "RETRIGGER POINT";
                constexpr const char* kAmplitude           = "AMPLITUDE";
                constexpr const char* kAmplitudeModByRamp2 = "AMP < RAMP 2";
                constexpr const char* kWaveform            = "WAVEFORM";
                constexpr const char* kTriggerMode         = "TRIGGER MODE";
                constexpr const char* kLag                 = "LAG";
                constexpr const char* kSampleInput         = "SAMPLE INPUT";
            }
        }

        namespace PatchNameModule
        {
            constexpr const char* kName = "PATCH NAME";

            namespace StandaloneWidgets
            {
                // Patch names are 8 characters long in the Oberheim Matrix-1000 :
                constexpr const char* kDefaultPatchName = "--------";
            }
        }
    }

    namespace MatrixModulationSection
    {
        constexpr const char* kName = "MATRIX MODULATION";

        namespace Header
        {
            constexpr const char* kBusNumber   = "#";
            constexpr const char* kSource      = "SOURCE";
            constexpr const char* kAmount      = "AMOUNT";
            constexpr const char* kDestination = "DESTINATION";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = ShortLabels::kInit;
            }
        }

        namespace ModulationBus
        {
            constexpr const char* kBus0 = "0";
            constexpr const char* kBus1 = "1";
            constexpr const char* kBus2 = "2";
            constexpr const char* kBus3 = "3";
            constexpr const char* kBus4 = "4";
            constexpr const char* kBus5 = "5";
            constexpr const char* kBus6 = "6";
            constexpr const char* kBus7 = "7";
            constexpr const char* kBus8 = "8";
            constexpr const char* kBus9 = "9";

            namespace StandaloneWidgets
            {
                constexpr const char* kInitBus0 = ShortLabels::kInit;
                constexpr const char* kInitBus1 = ShortLabels::kInit;
                constexpr const char* kInitBus2 = ShortLabels::kInit;
                constexpr const char* kInitBus3 = ShortLabels::kInit;
                constexpr const char* kInitBus4 = ShortLabels::kInit;
                constexpr const char* kInitBus5 = ShortLabels::kInit;
                constexpr const char* kInitBus6 = ShortLabels::kInit;
                constexpr const char* kInitBus7 = ShortLabels::kInit;
                constexpr const char* kInitBus8 = ShortLabels::kInit;
                constexpr const char* kInitBus9 = ShortLabels::kInit;
            }
        }
    }

    namespace PatchManagerSection
    {
        constexpr const char* kName = "PATCH MANAGER";

        namespace BankUtilityModule
        {
            constexpr const char* kName = "BANK UTILITY";

            namespace StandaloneWidgets
            {
                constexpr const char* kBankSelector = "SELECT BANK";
                constexpr const char* kLockBank     = "BANK LOCK";
                constexpr const char* kSelectBank0  = "0";
                constexpr const char* kSelectBank1  = "1";
                constexpr const char* kSelectBank2  = "2";
                constexpr const char* kSelectBank3  = "3";
                constexpr const char* kSelectBank4  = "4";
                constexpr const char* kSelectBank5  = "5";
                constexpr const char* kSelectBank6  = "6";
                constexpr const char* kSelectBank7  = "7";
                constexpr const char* kSelectBank8  = "8";
                constexpr const char* kSelectBank9  = "9";
            }
        }

        namespace InternalPatchesModule
        {
            constexpr const char* kName = "INTERNAL PATCHES";

            namespace StandaloneWidgets
            {
                constexpr const char* kBrowser            = "BROWSER";
                constexpr const char* kMemory             = "MEMORY";
                constexpr const char* kLoadPreviousPatch  = "<";
                constexpr const char* kLoadNextPatch      = ">";
                constexpr const char* kCurrentBankNumber  = "CURRENT BANK";
                constexpr const char* kCurrentPatchNumber = "CURRENT PATCH";
                constexpr const char* kInitPatch          = "INIT";
                constexpr const char* kCopyPatch          = "COPY";
                constexpr const char* kPastePatch         = "PASTE";
                constexpr const char* kStorePatch         = "STORE";
            }
        }

        namespace ComputerPatchesModule
        {
            constexpr const char* kName = "COMPUTER PATCHES";

            namespace StandaloneWidgets
            {
                constexpr const char* kBrowser               = "BROWSER";
                constexpr const char* kStorage               = "STORAGE";
                constexpr const char* kLoadPreviousPatchFile = "<";
                constexpr const char* kLoadNextPatchFile     = ">";
                constexpr const char* kSelectPatchFile       = "SELECT A PATCH";
                constexpr const char* kOpenPatchFolder       = "OPEN";
                constexpr const char* kSavePatchAsFile       = "SAVE AS";
                constexpr const char* kSavePatchFile         = "SAVE";
            }
        }

        namespace PatchMutatorModule
        {
            constexpr const char* kName = "PATCH MUTATOR";

            namespace StandaloneWidgets
            {
                constexpr const char* kAmount         = "AMOUNT";
                constexpr const char* kRandom         = "RANDOM";
                constexpr const char* kHistory        = "HISTORY";
                constexpr const char* kEmptyHistory   = "EMPTY";
                
                constexpr const char* kMutate         = "MUTATE";
                constexpr const char* kRetry          = "RETRY";
                constexpr const char* kCompare        = "COMPARE";
                const juce::String    kDelete         = juce::String::fromUTF8("\xe2\x80\x94"); // displays a @'–'
                constexpr const char* kClear          = "CLEAR";
                constexpr const char* kExport         = "EXPORT";

                constexpr const char* kEnableDco1           = "D1";
                constexpr const char* kEnableDco2           = "D2";
                constexpr const char* kEnableVcfVca         = "F/A";
                constexpr const char* kEnableFmTrack        = "F/T";
                constexpr const char* kEnableRampPortamento = "R/P";
                constexpr const char* kEnableEnvelope1      = "E1";
                constexpr const char* kEnableEnvelope2      = "E2";
                constexpr const char* kEnableEnvelope3      = "E3";
                constexpr const char* kEnableLfo1           = "L1";
                constexpr const char* kEnableLfo2           = "L2";
            }
        }
    } 
}