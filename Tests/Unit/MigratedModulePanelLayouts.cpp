#include "MigratedModulePanelLayouts.h"

#include "Shared/Definitions/PluginIDs.h"

// Layout order mirrors each panel's createLayout() — keep in sync when panel IDs change.

namespace MigratedModulePanelLayouts
{
    std::vector<std::pair<const char*, ModulePanelLayout>> all()
    {
        return {
            { "Dco1Panel",
              makePatchEditModuleLayout(
                  PluginIDs::PatchEditSection::Dco1Module::kGroupId,
                  PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kInit,
                  PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kCopy,
                  PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kPaste,
                  {
                      PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency,
                      PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequencyModByLfo1,
                      PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kSync,
                      PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kPulseWidth,
                      PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kPulseWidthModByLfo2,
                      PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kWaveShape,
                      PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kWaveSelect,
                      PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kLevers,
                      PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kKeyboardPortamento,
                      PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kKeyClick
                  }) },
            { "Dco2Panel",
              makePatchEditModuleLayout(
                  PluginIDs::PatchEditSection::Dco2Module::kGroupId,
                  PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kInit,
                  PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kCopy,
                  PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kPaste,
                  {
                      PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kFrequency,
                      PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kFrequencyModByLfo1,
                      PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kDetune,
                      PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kPulseWidth,
                      PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kPulseWidthModByLfo2,
                      PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kWaveShape,
                      PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kWaveSelect,
                      PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kLevers,
                      PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kKeyboardPortamento,
                      PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kKeyClick
                  }) },
            { "VcfVcaPanel",
              makePatchEditInitOnlyModuleLayout(
                  PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
                  PluginIDs::PatchEditSection::VcfVcaModule::StandaloneWidgets::kInit,
                  {
                      PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kBalance,
                      PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequency,
                      PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequencyModByEnv1,
                      PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequencyModByPressure,
                      PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kResonance,
                      PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca1Volume,
                      PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca1ModByVelocity,
                      PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca2ModByEnv2,
                      PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kLevers,
                      PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kKeyboardPortamento
                  }) },
            { "FmTrackPanel",
              makePatchEditInitOnlyModuleLayout(
                  PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
                  PluginIDs::PatchEditSection::FmTrackModule::StandaloneWidgets::kInit,
                  {
                      PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmAmount,
                      PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmModByEnv3,
                      PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmModByPressure,
                      PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint1,
                      PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint2,
                      PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3,
                      PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint4,
                      PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint5,
                      PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackInput,
                      ""
                  }) },
            { "RampPortamentoPanel",
              makePatchEditInitOnlyModuleLayout(
                  PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
                  PluginIDs::PatchEditSection::RampPortamentoModule::StandaloneWidgets::kInit,
                  {
                      PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Rate,
                      PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Trigger,
                      PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Rate,
                      PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Trigger,
                      PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoRate,
                      PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoModByVelocity,
                      PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoMode,
                      PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoLegato,
                      PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode,
                      ""
                  }) },
            { "Env1Panel",
              makePatchEditModuleLayout(
                  PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
                  PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kInit,
                  PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kCopy,
                  PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kPaste,
                  {
                      PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDelay,
                      PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack,
                      PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDecay,
                      PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kSustain,
                      PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kRelease,
                      PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitude,
                      PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitudeModByVelocity,
                      PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kTriggerMode,
                      PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kEnvelopeMode,
                      PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kLfo1Trigger
                  }) },
            { "Env2Panel",
              makePatchEditModuleLayout(
                  PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
                  PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kInit,
                  PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kCopy,
                  PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kPaste,
                  {
                      PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDelay,
                      PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAttack,
                      PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDecay,
                      PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kSustain,
                      PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kRelease,
                      PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitude,
                      PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitudeModByVelocity,
                      PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kTriggerMode,
                      PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kEnvelopeMode,
                      PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kLfo1Trigger
                  }) },
            { "Env3Panel",
              makePatchEditModuleLayout(
                  PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
                  PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kInit,
                  PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kCopy,
                  PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kPaste,
                  {
                      PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDelay,
                      PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAttack,
                      PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDecay,
                      PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kSustain,
                      PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kRelease,
                      PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitude,
                      PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitudeModByVelocity,
                      PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kTriggerMode,
                      PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kEnvelopeMode,
                      PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kLfo1Trigger
                  }) },
            { "Lfo1Panel",
              makePatchEditModuleLayout(
                  PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
                  PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kInit,
                  PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kCopy,
                  PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kPaste,
                  {
                      PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kSpeed,
                      PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kSpeedModByPressure,
                      PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kRetriggerPoint,
                      PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kAmplitude,
                      PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kAmplitudeModByRamp1,
                      PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kWaveform,
                      PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kTriggerMode,
                      PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kLag,
                      PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kSampleInput,
                      ""
                  }) },
            { "Lfo2Panel",
              makePatchEditModuleLayout(
                  PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
                  PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kInit,
                  PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kCopy,
                  PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kPaste,
                  {
                      PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kSpeed,
                      PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kSpeedModByKeyboard,
                      PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kRetriggerPoint,
                      PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kAmplitude,
                      PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kAmplitudeModByRamp2,
                      PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kWaveform,
                      PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kTriggerMode,
                      PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kLag,
                      PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kSampleInput,
                      ""
                  }) },
            { "MidiPanel",
              makeMasterEditModuleLayout(
                  PluginIDs::MasterEditSection::MidiModule::kGroupId,
                  PluginIDs::MasterEditSection::MidiModule::StandaloneWidgets::kInit,
                  {
                      PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel,
                      PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kMidiEcho,
                      PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kControllers,
                      PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPatchChanges,
                      PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPedal1Select,
                      PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPedal2Select,
                      PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kLever2Select,
                      PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kLever3Select
                  }) },
            { "VibratoPanel",
              makeMasterEditModuleLayout(
                  PluginIDs::MasterEditSection::VibratoModule::kGroupId,
                  PluginIDs::MasterEditSection::VibratoModule::StandaloneWidgets::kInit,
                  {
                      PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeed,
                      PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kWaveform,
                      PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmplitude,
                      PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModSource,
                      PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModAmount,
                      PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmpModSource,
                      PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmpModAmount
                  }) },
            { "MiscPanel",
              makeMasterEditModuleLayout(
                  PluginIDs::MasterEditSection::MiscModule::kGroupId,
                  PluginIDs::MasterEditSection::MiscModule::StandaloneWidgets::kInit,
                  {
                      PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMasterTune,
                      PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMasterTranspose,
                      PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kBendRange,
                      PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable,
                      PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kVolumeInvertEnable,
                      PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kBankLockEnable,
                      PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMemoryProtectEnable,
                      ""
                  }) }
        };
    }
}
