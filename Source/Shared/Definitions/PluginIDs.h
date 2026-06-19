#pragma once

// PluginIDs.h
// Central file for all plugin IDs (unique identifiers) and small constexpr helpers.
// Most entries are string IDs; Settings::ScaleLevels also defines UI scale factors (float[] + lookup).

#include <array>

#include "Matrix1000Limits.h"

namespace PluginIDs
{
    namespace Settings
    {
        constexpr const char* kGuiScale = "settingsGuiScale";
        constexpr const char* kSkinVariant = "settingsSkinVariant";
        constexpr const char* kHardwareLatencyMs = "settingsHardwareLatencyMs";
        constexpr const char* kInitTemplatesFolderPath = "settingsInitTemplatesFolderPath";
        constexpr const char* kTestWidget = "settingsTestWidget";

        namespace SkinVariants
        {
            constexpr int kBlack = 1;
            constexpr int kCream = 2;
            constexpr int kDefault = kBlack;
        }

        namespace ScaleLevels
        {
            constexpr int k50   = 1;
            constexpr int k75   = 2;
            constexpr int k100  = 3;
            constexpr int k125  = 4;
            constexpr int k150  = 5;
            constexpr int k175  = 6;
            constexpr int k200  = 7;

            constexpr int kDefault = k100;
            constexpr int kMin = k50;
            constexpr int kMax = k200;

            constexpr float kUiScales[] = {
                0.0f,
                0.5f,
                0.75f,
                1.0f,
                1.25f,
                1.5f,
                1.75f,
                2.0f
            };

            constexpr float getUiScale(int scaleId)
            {
                if (scaleId >= kMin && scaleId <= kMax)
                    return kUiScales[scaleId];
                return kUiScales[kDefault];
            }
        }
    }

    namespace Mode
    {
        constexpr const char* kMaster = "masterMode";
        constexpr const char* kPatch  = "patchMode";
    }

    namespace MasterEditSection
    {
        constexpr const char* kGroupId = "masterEditSection";

        namespace MidiModule
        {
            constexpr const char* kGroupId = "midiModule";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = "midiInit";
            }

            namespace ParameterWidgets
            {
                constexpr const char* kChannel      = "midiChannel";
                constexpr const char* kMidiEcho     = "midiEcho";
                constexpr const char* kControllers  = "midiControllers";
                constexpr const char* kPatchChanges = "midiPatchChanges";
                constexpr const char* kPedal1Select = "midiPedal1Select";
                constexpr const char* kPedal2Select = "midiPedal2Select";
                constexpr const char* kLever2Select = "midiLever2Select";
                constexpr const char* kLever3Select = "midiLever3Select";
            }
        }

        namespace VibratoModule
        {
            constexpr const char* kGroupId = "vibratoModule";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = "vibratoInit";
            }

            namespace ParameterWidgets
            {
                constexpr const char* kSpeed          = "vibratoSpeed";
                constexpr const char* kSpeedModSource = "vibratoSpeedModSource";
                constexpr const char* kSpeedModAmount = "vibratoSpeedModAmount";
                constexpr const char* kWaveform       = "vibratoWaveform";
                constexpr const char* kAmplitude      = "vibratoAmplitude";
                constexpr const char* kAmpModSource   = "vibratoAmpModSource";
                constexpr const char* kAmpModAmount   = "vibratoAmpModAmount";
            }
        }

        namespace MiscModule
        {
            constexpr const char* kGroupId = "miscModule";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = "miscInit";
            }

            namespace ParameterWidgets
            {
                constexpr const char* kMasterTune          = "miscMasterTune";
                constexpr const char* kMasterTranspose     = "miscMasterTranspose";
                constexpr const char* kBendRange           = "miscBendRange";
                constexpr const char* kUnisonEnable        = "miscUnisonEnable";
                constexpr const char* kVolumeInvertEnable  = "miscVolumeInvertEnable";
                constexpr const char* kBankLockEnable      = "miscBankLockEnable";
                constexpr const char* kMemoryProtectEnable = "miscMemoryProtectEnable";
            }
        }
    }

    namespace PatchEditSection
    {
        constexpr const char* kGroupId = "patchEditSection";

        namespace Dco1Module
        {
            constexpr const char* kGroupId = "dco1Module";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit         = "dco1Init";
                constexpr const char* kCopy         = "dco1Copy";
                constexpr const char* kPaste        = "dco1Paste";
                constexpr const char* kPasteEnabled = "dco1PasteEnabled";
            }

            namespace ParameterWidgets
            {
                constexpr const char* kFrequency           = "dco1Frequency";
                constexpr const char* kFrequencyModByLfo1  = "dco1FrequencyModByLfo1";
                constexpr const char* kPulseWidth          = "dco1PulseWidth";
                constexpr const char* kPulseWidthModByLfo2 = "dco1PulseWidthModByLfo2";
                constexpr const char* kWaveShape           = "dco1WaveShape";
                constexpr const char* kSync                = "dco1Sync";
                constexpr const char* kWaveSelect          = "dco1WaveSelect";
                constexpr const char* kLevers              = "dco1Levers";
                constexpr const char* kKeyboardPortamento  = "dco1KeyboardPortamento";
                constexpr const char* kKeyClick            = "dco1KeyClick";
            }
        }

        namespace Dco2Module
        {
            constexpr const char* kGroupId = "dco2Module";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit         = "dco2Init";
                constexpr const char* kCopy         = "dco2Copy";
                constexpr const char* kPaste        = "dco2Paste";
                constexpr const char* kPasteEnabled = "dco2PasteEnabled";
            }

            namespace ParameterWidgets
            {
                constexpr const char* kFrequency           = "dco2Frequency";
                constexpr const char* kFrequencyModByLfo1  = "dco2FrequencyModByLfo1";
                constexpr const char* kDetune              = "dco2Detune";
                constexpr const char* kPulseWidth          = "dco2PulseWidth";
                constexpr const char* kPulseWidthModByLfo2 = "dco2PulseWidthModByLfo2";
                constexpr const char* kWaveShape           = "dco2WaveShape";
                constexpr const char* kWaveSelect          = "dco2WaveSelect";
                constexpr const char* kLevers              = "dco2Levers";
                constexpr const char* kKeyboardPortamento  = "dco2KeyboardPortamento";
                constexpr const char* kKeyClick            = "dco2KeyClick";
            }
        }

        namespace VcfVcaModule
        {
            constexpr const char* kGroupId = "vcfVcaModule";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = "vcfVcaInit";
            }

            namespace ParameterWidgets
            {
                constexpr const char* kBalance                = "vcfVcaBalance";
                constexpr const char* kFrequency              = "vcfVcaFrequency";
                constexpr const char* kFrequencyModByEnv1     = "vcfVcaFrequencyModByEnv1";
                constexpr const char* kFrequencyModByPressure = "vcfVcaFrequencyModByPressure";
                constexpr const char* kResonance              = "vcfVcaResonance";
                constexpr const char* kVca1Volume             = "vcfVcaVca1Volume";
                constexpr const char* kVca1ModByVelocity      = "vcfVcaVca1ModByVelocity";
                constexpr const char* kVca2ModByEnv2          = "vcfVcaVca2ModByEnv2";
                constexpr const char* kLevers                 = "vcfVcaLevers";
                constexpr const char* kKeyboardPortamento     = "vcfVcaKeyboardPortamento";
            }
        }

        namespace FmTrackModule
        {
            constexpr const char* kGroupId = "fmTrackModule";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = "fmTrackInit";
            }

            namespace ParameterWidgets
            {
                constexpr const char* kFmAmount        = "fmTrackFmAmount";
                constexpr const char* kFmModByEnv3     = "fmTrackFmModByEnv3";
                constexpr const char* kFmModByPressure = "fmTrackFmModByPressure";
                constexpr const char* kTrackPoint1     = "fmTrackPoint1";
                constexpr const char* kTrackPoint2     = "fmTrackPoint2";
                constexpr const char* kTrackPoint3     = "fmTrackPoint3";
                constexpr const char* kTrackPoint4     = "fmTrackPoint4";
                constexpr const char* kTrackPoint5     = "fmTrackPoint5";
                constexpr const char* kTrackInput      = "fmTrackInput";
            }
        }

        namespace RampPortamentoModule
        {
            constexpr const char* kGroupId = "rampPortamentoModule";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit               = "rampPortamentoInit";
            }

            namespace ParameterWidgets
            {
                constexpr const char* kRamp1Rate               = "rampPortamentoRamp1Rate";
                constexpr const char* kRamp2Rate               = "rampPortamentoRamp2Rate";
                constexpr const char* kPortamentoRate          = "rampPortamentoPortamentoRate";
                constexpr const char* kPortamentoModByVelocity = "rampPortamentoPortamentoModByVelocity";
                constexpr const char* kRamp1Trigger            = "rampPortamentoRamp1Trigger";
                constexpr const char* kRamp2Trigger            = "rampPortamentoRamp2Trigger";
                constexpr const char* kPortamentoMode          = "rampPortamentoPortamentoMode";
                constexpr const char* kPortamentoLegato        = "rampPortamentoPortamentoLegato";
                constexpr const char* kPortamentoKeyboardMode  = "rampPortamentoPortamentoKeyboardMode";
            }
        }

        namespace Envelope1Module
        {
            constexpr const char* kGroupId = "envelope1Module";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit         = "env1Init";
                constexpr const char* kCopy         = "env1Copy";
                constexpr const char* kPaste        = "env1Paste";
                constexpr const char* kPasteEnabled = "env1PasteEnabled";
            }

            namespace ParameterWidgets
            {
                constexpr const char* kDelay                  = "env1Delay";
                constexpr const char* kAttack                 = "env1Attack";
                constexpr const char* kDecay                  = "env1Decay";
                constexpr const char* kSustain                = "env1Sustain";
                constexpr const char* kRelease                = "env1Release";
                constexpr const char* kAmplitude              = "env1Amplitude";
                constexpr const char* kAmplitudeModByVelocity = "env1AmplitudeModByVelocity";
                constexpr const char* kTriggerMode            = "env1TriggerMode";
                constexpr const char* kEnvelopeMode           = "env1EnvelopeMode";
                constexpr const char* kLfo1Trigger            = "env1Lfo1Trigger";
            } 
        }

        namespace Envelope2Module
        {
            constexpr const char* kGroupId = "envelope2Module";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit         = "env2Init";
                constexpr const char* kCopy         = "env2Copy";
                constexpr const char* kPaste        = "env2Paste";
                constexpr const char* kPasteEnabled = "env2PasteEnabled";
            }

            namespace ParameterWidgets
            {
                constexpr const char* kDelay                  = "env2Delay";
                constexpr const char* kAttack                 = "env2Attack";
                constexpr const char* kDecay                  = "env2Decay";
                constexpr const char* kSustain                = "env2Sustain";
                constexpr const char* kRelease                = "env2Release";
                constexpr const char* kAmplitude              = "env2Amplitude";
                constexpr const char* kAmplitudeModByVelocity = "env2AmplitudeModByVelocity";
                constexpr const char* kTriggerMode            = "env2TriggerMode";
                constexpr const char* kEnvelopeMode           = "env2EnvelopeMode";
                constexpr const char* kLfo1Trigger            = "env2Lfo1Trigger";
            }
        }

        namespace Envelope3Module
        {
            constexpr const char* kGroupId = "envelope3Module";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit         = "env3Init";
                constexpr const char* kCopy         = "env3Copy";
                constexpr const char* kPaste        = "env3Paste";
                constexpr const char* kPasteEnabled = "env3PasteEnabled";
            }

            namespace ParameterWidgets
            {
                constexpr const char* kDelay                  = "env3Delay";
                constexpr const char* kAttack                 = "env3Attack";
                constexpr const char* kDecay                  = "env3Decay";
                constexpr const char* kSustain                = "env3Sustain";
                constexpr const char* kRelease                = "env3Release";
                constexpr const char* kAmplitude              = "env3Amplitude";
                constexpr const char* kAmplitudeModByVelocity = "env3AmplitudeModByVelocity";
                constexpr const char* kTriggerMode            = "env3TriggerMode";
                constexpr const char* kEnvelopeMode           = "env3EnvelopeMode";
                constexpr const char* kLfo1Trigger            = "env3Lfo1Trigger";
            }
        }

        namespace Lfo1Module
        {
            constexpr const char* kGroupId = "lfo1Module";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit         = "lfo1Init";
                constexpr const char* kCopy         = "lfo1Copy";
                constexpr const char* kPaste        = "lfo1Paste";
                constexpr const char* kPasteEnabled = "lfo1PasteEnabled";
            }

            namespace ParameterWidgets
            {
                constexpr const char* kSpeed               = "lfo1Speed";
                constexpr const char* kSpeedModByPressure  = "lfo1SpeedModByPressure";
                constexpr const char* kRetriggerPoint      = "lfo1RetriggerPoint";
                constexpr const char* kAmplitude           = "lfo1Amplitude";
                constexpr const char* kAmplitudeModByRamp1 = "lfo1AmplitudeModByRamp1";
                constexpr const char* kWaveform            = "lfo1Waveform";
                constexpr const char* kTriggerMode         = "lfo1TriggerMode";
                constexpr const char* kLag                 = "lfo1Lag";
                constexpr const char* kSampleInput         = "lfo1SampleInput";
            }
        }

        namespace Lfo2Module
        {
            constexpr const char* kGroupId = "lfo2Module";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit         = "lfo2Init";
                constexpr const char* kCopy         = "lfo2Copy";
                constexpr const char* kPaste        = "lfo2Paste";
                constexpr const char* kPasteEnabled = "lfo2PasteEnabled";
            }

            namespace ParameterWidgets
            {
                constexpr const char* kSpeed               = "lfo2Speed";
                constexpr const char* kSpeedModByKeyboard  = "lfo2SpeedModByKeyboard";
                constexpr const char* kRetriggerPoint      = "lfo2RetriggerPoint";
                constexpr const char* kAmplitude           = "lfo2Amplitude";
                constexpr const char* kAmplitudeModByRamp2 = "lfo2AmplitudeModByRamp2";
                constexpr const char* kWaveform            = "lfo2Waveform";
                constexpr const char* kTriggerMode         = "lfo2TriggerMode";
                constexpr const char* kLag                 = "lfo2Lag";
                constexpr const char* kSampleInput         = "lfo2SampleInput";
            }
        }

        namespace PatchNameModule
        {
            constexpr const char* kPatchName = "patchEditPatchName";
        }
    }

    namespace MatrixModulationSection
    {
        constexpr const char* kGroupId = "matrixModulationSection";

        namespace StandaloneWidgets
        {
            constexpr const char* kMatrixModulationInit         = "matrixModulationInit";
            constexpr const char* kMatrixModulationCopy         = "matrixModulationCopy";
            constexpr const char* kMatrixModulationPaste        = "matrixModulationPaste";
            constexpr const char* kMatrixModulationPasteEnabled = "matrixModulationPasteEnabled";
        }

        namespace ModulationBus
        {
            constexpr const char* kBus0 = "modulationBus0";
            constexpr const char* kBus1 = "modulationBus1";
            constexpr const char* kBus2 = "modulationBus2";
            constexpr const char* kBus3 = "modulationBus3";
            constexpr const char* kBus4 = "modulationBus4";
            constexpr const char* kBus5 = "modulationBus5";
            constexpr const char* kBus6 = "modulationBus6";
            constexpr const char* kBus7 = "modulationBus7";
            constexpr const char* kBus8 = "modulationBus8";
            constexpr const char* kBus9 = "modulationBus9";

            namespace StandaloneWidgets
            {
                constexpr const char* kBus0Init = "modulationBus0Init";
                constexpr const char* kBus1Init = "modulationBus1Init";
                constexpr const char* kBus2Init = "modulationBus2Init";
                constexpr const char* kBus3Init = "modulationBus3Init";
                constexpr const char* kBus4Init = "modulationBus4Init";
                constexpr const char* kBus5Init = "modulationBus5Init";
                constexpr const char* kBus6Init = "modulationBus6Init";
                constexpr const char* kBus7Init = "modulationBus7Init";
                constexpr const char* kBus8Init = "modulationBus8Init";
                constexpr const char* kBus9Init = "modulationBus9Init";
            }

            namespace ParameterWidgets
            {
                constexpr const char* kBus0Source      = "modulationBus0Source";
                constexpr const char* kBus0Amount      = "modulationBus0Amount";
                constexpr const char* kBus0Destination = "modulationBus0Destination";

                constexpr const char* kBus1Source      = "modulationBus1Source";
                constexpr const char* kBus1Amount      = "modulationBus1Amount";
                constexpr const char* kBus1Destination = "modulationBus1Destination";

                constexpr const char* kBus2Source      = "modulationBus2Source";
                constexpr const char* kBus2Amount      = "modulationBus2Amount";
                constexpr const char* kBus2Destination = "modulationBus2Destination";

                constexpr const char* kBus3Source      = "modulationBus3Source";
                constexpr const char* kBus3Amount      = "modulationBus3Amount";
                constexpr const char* kBus3Destination = "modulationBus3Destination";

                constexpr const char* kBus4Source      = "modulationBus4Source";
                constexpr const char* kBus4Amount      = "modulationBus4Amount";
                constexpr const char* kBus4Destination = "modulationBus4Destination";

                constexpr const char* kBus5Source      = "modulationBus5Source";
                constexpr const char* kBus5Amount      = "modulationBus5Amount";
                constexpr const char* kBus5Destination = "modulationBus5Destination";

                constexpr const char* kBus6Source      = "modulationBus6Source";
                constexpr const char* kBus6Amount      = "modulationBus6Amount";
                constexpr const char* kBus6Destination = "modulationBus6Destination";

                constexpr const char* kBus7Source      = "modulationBus7Source";
                constexpr const char* kBus7Amount      = "modulationBus7Amount";
                constexpr const char* kBus7Destination = "modulationBus7Destination";

                constexpr const char* kBus8Source      = "modulationBus8Source";
                constexpr const char* kBus8Amount      = "modulationBus8Amount";
                constexpr const char* kBus8Destination = "modulationBus8Destination";

                constexpr const char* kBus9Source      = "modulationBus9Source";
                constexpr const char* kBus9Amount      = "modulationBus9Amount";
                constexpr const char* kBus9Destination = "modulationBus9Destination";
            }
        }

        inline const std::array<const char*, Matrix1000Limits::kModulationBusCount> kModulationBusGroupIds = {
            ModulationBus::kBus0,
            ModulationBus::kBus1,
            ModulationBus::kBus2,
            ModulationBus::kBus3,
            ModulationBus::kBus4,
            ModulationBus::kBus5,
            ModulationBus::kBus6,
            ModulationBus::kBus7,
            ModulationBus::kBus8,
            ModulationBus::kBus9
        };
    }

    namespace PatchManagerSection
    {
        constexpr const char* kGroupId = "patchManagerSection";

        namespace BankUtilityModule
        {
            constexpr const char* kGroupId = "bankUtilityModule";

            namespace StateProperties
            {
                constexpr const char* kSelectedBank = "patchManagerSelectedBank";
                constexpr const char* kBanksLocked = "patchManagerBanksLocked";
            }

            namespace StandaloneWidgets
            {
                constexpr const char* kUnlockBank  = "bankUtilityUnlockBank";
                constexpr const char* kSelectBank0 = "bankUtilitySelectBank0";
                constexpr const char* kSelectBank1 = "bankUtilitySelectBank1";
                constexpr const char* kSelectBank2 = "bankUtilitySelectBank2";
                constexpr const char* kSelectBank3 = "bankUtilitySelectBank3";
                constexpr const char* kSelectBank4 = "bankUtilitySelectBank4";
                constexpr const char* kSelectBank5 = "bankUtilitySelectBank5";
                constexpr const char* kSelectBank6 = "bankUtilitySelectBank6";
                constexpr const char* kSelectBank7 = "bankUtilitySelectBank7";
                constexpr const char* kSelectBank8 = "bankUtilitySelectBank8";
                constexpr const char* kSelectBank9 = "bankUtilitySelectBank9";
            }
        }

        namespace InternalPatchesModule
        {
            constexpr const char* kGroupId        = "internalPatchesModule";
            constexpr const char* kBrowserGroupId = "internalPatchesBrowserGroup";
            constexpr const char* kMemoryGroupId  = "internalPatchesMemoryGroup";

            namespace StandaloneWidgets
            {
                constexpr const char* kLoadPreviousPatch  = "internalPatchesLoadPrevious";
                constexpr const char* kLoadNextPatch      = "internalPatchesLoadNext";
                constexpr const char* kCurrentBankNumber  = "internalPatchesCurrentBankNumber";
                constexpr const char* kCurrentPatchNumber = "internalPatchesCurrentPatchNumber";
                constexpr const char* kInitPatch          = "internalPatchesInit";
                constexpr const char* kCopyPatch          = "internalPatchesCopy";
                constexpr const char* kPastePatch         = "internalPatchesPaste";
                constexpr const char* kPastePatchEnabled  = "internalPatchesPasteEnabled";
                constexpr const char* kStorePatch         = "internalPatchesStore";
            }
        }

        namespace ComputerPatchesModule
        {
            constexpr const char* kGroupId        = "computerPatchesModule";
            constexpr const char* kBrowserGroupId = "computerPatchesBrowserGroup";
            constexpr const char* kStorageGroupId = "computerPatchesStorageGroup";

            namespace StandaloneWidgets
            {
                constexpr const char* kLoadPreviousPatchFile = "computerPatchesLoadPrevious";
                constexpr const char* kLoadNextPatchFile     = "computerPatchesLoadNext";
                constexpr const char* kSelectPatchFile       = "computerPatchesSelectPatch";
                constexpr const char* kOpenPatchFolder       = "computerPatchesOpenFolder";
                constexpr const char* kSavePatchAs           = "computerPatchesSaveAs";
                constexpr const char* kSavePatchFile         = "computerPatchesSave";
            }
        }

        namespace PatchMutatorModule
        {
            constexpr const char* kGroupId = "patchMutatorModule";

            namespace StandaloneWidgets
            {
                constexpr const char* kAmount         = "patchMutatorAmount";
                constexpr const char* kRandom         = "patchMutatorRandom";
                constexpr const char* kHistory        = "patchMutatorHistory";

                constexpr const char* kMutate         = "patchMutatorMutate";
                constexpr const char* kRetry          = "patchMutatorRetry";
                constexpr const char* kCompare        = "patchMutatorCompare";
                constexpr const char* kDelete         = "patchMutatorDelete";
                constexpr const char* kClear          = "patchMutatorClear";
                constexpr const char* kExport         = "patchMutatorExport";

                constexpr const char* kEnableDco1           = "patchMutatorEnableDco1";
                constexpr const char* kEnableDco2           = "patchMutatorEnableDco2";
                constexpr const char* kEnableVcfVca         = "patchMutatorEnableVcfVca";
                constexpr const char* kEnableFmTrack        = "patchMutatorEnableFmTrack";
                constexpr const char* kEnableRampPortamento = "patchMutatorEnableRampPortamento";
                constexpr const char* kEnableEnvelope1      = "patchMutatorEnableEnvelope1";
                constexpr const char* kEnableEnvelope2      = "patchMutatorEnableEnvelope2";
                constexpr const char* kEnableEnvelope3      = "patchMutatorEnableEnvelope3";
                constexpr const char* kEnableLfo1           = "patchMutatorEnableLfo1";
                constexpr const char* kEnableLfo2           = "patchMutatorEnableLfo2";
            }
        }
    }
}
