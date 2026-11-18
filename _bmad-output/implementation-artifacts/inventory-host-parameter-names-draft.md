# Draft host parameter names (for human review)

Convention: `{Module Title Case} {Leaf Title Case}`; product tokens DCO/VCF/VCA/LFO/ENV/FM/MIDI kept.
Separators: `/` joins a compound with no spaces (`Keyboard/Portamento`); `|` separates Mix poles with spaces (`DCO 2 | DCO 1`) and keeps GUI/hardware polarity (0 = DCO 2, 63 = DCO 1).
Redundant module echoes in the leaf are intentional when they aid flat-host readability (`FM/Track Track Point 1`, `VCF/VCA VCA 1 Volume`), except avoid stutter like `MIDI MIDI Echo` → use `MIDI Echo`.

| Host name | UI short label | Module | Parameter symbol |
|---|---|---|---|
| `Bus 0 Amount` | `AMOUNT` | `Bus 0` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Amount` |
| `Bus 0 Destination` | `DESTINATION` | `Bus 0` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Destination` |
| `Bus 0 Source` | `SOURCE` | `Bus 0` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Source` |
| `Bus 1 Amount` | `AMOUNT` | `Bus 1` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Amount` |
| `Bus 1 Destination` | `DESTINATION` | `Bus 1` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Destination` |
| `Bus 1 Source` | `SOURCE` | `Bus 1` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Source` |
| `Bus 2 Amount` | `AMOUNT` | `Bus 2` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Amount` |
| `Bus 2 Destination` | `DESTINATION` | `Bus 2` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Destination` |
| `Bus 2 Source` | `SOURCE` | `Bus 2` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Source` |
| `Bus 3 Amount` | `AMOUNT` | `Bus 3` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Amount` |
| `Bus 3 Destination` | `DESTINATION` | `Bus 3` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Destination` |
| `Bus 3 Source` | `SOURCE` | `Bus 3` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Source` |
| `Bus 4 Amount` | `AMOUNT` | `Bus 4` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Amount` |
| `Bus 4 Destination` | `DESTINATION` | `Bus 4` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Destination` |
| `Bus 4 Source` | `SOURCE` | `Bus 4` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Source` |
| `Bus 5 Amount` | `AMOUNT` | `Bus 5` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Amount` |
| `Bus 5 Destination` | `DESTINATION` | `Bus 5` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Destination` |
| `Bus 5 Source` | `SOURCE` | `Bus 5` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Source` |
| `Bus 6 Amount` | `AMOUNT` | `Bus 6` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Amount` |
| `Bus 6 Destination` | `DESTINATION` | `Bus 6` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Destination` |
| `Bus 6 Source` | `SOURCE` | `Bus 6` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Source` |
| `Bus 7 Amount` | `AMOUNT` | `Bus 7` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Amount` |
| `Bus 7 Destination` | `DESTINATION` | `Bus 7` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Destination` |
| `Bus 7 Source` | `SOURCE` | `Bus 7` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Source` |
| `Bus 8 Amount` | `AMOUNT` | `Bus 8` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Amount` |
| `Bus 8 Destination` | `DESTINATION` | `Bus 8` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Destination` |
| `Bus 8 Source` | `SOURCE` | `Bus 8` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Source` |
| `Bus 9 Amount` | `AMOUNT` | `Bus 9` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Amount` |
| `Bus 9 Destination` | `DESTINATION` | `Bus 9` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Destination` |
| `Bus 9 Source` | `SOURCE` | `Bus 9` | `MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Source` |
| `DCO 1 Frequency < LFO 1` | `FREQ < LFO 1` | `DCO 1` | `PatchEditSection::Dco1Module::ParameterWidgets::kFrequencyModByLfo1` |
| `DCO 1 Frequency` | `FREQUENCY` | `DCO 1` | `PatchEditSection::Dco1Module::ParameterWidgets::kFrequency` |
| `DCO 1 Key Click` | `KEY CLICK` | `DCO 1` | `PatchEditSection::Dco1Module::ParameterWidgets::kKeyClick` |
| `DCO 1 Keyboard/Portamento` | `KEYBD/PORTA` | `DCO 1` | `PatchEditSection::Dco1Module::ParameterWidgets::kKeyboardPortamento` |
| `DCO 1 Levers` | `LEVERS` | `DCO 1` | `PatchEditSection::Dco1Module::ParameterWidgets::kLevers` |
| `DCO 1 Pulse Width` | `PULSE WIDTH` | `DCO 1` | `PatchEditSection::Dco1Module::ParameterWidgets::kPulseWidth` |
| `DCO 1 Pulse Width < LFO 2` | `PW < LFO 2` | `DCO 1` | `PatchEditSection::Dco1Module::ParameterWidgets::kPulseWidthModByLfo2` |
| `DCO 1 Sync` | `SYNC` | `DCO 1` | `PatchEditSection::Dco1Module::ParameterWidgets::kSync` |
| `DCO 1 Wave Select` | `WAVE SELECT` | `DCO 1` | `PatchEditSection::Dco1Module::ParameterWidgets::kWaveSelect` |
| `DCO 1 Wave Shape` | `WAVE SHAPE` | `DCO 1` | `PatchEditSection::Dco1Module::ParameterWidgets::kWaveShape` |
| `DCO 2 Detune` | `DETUNE` | `DCO 2` | `PatchEditSection::Dco2Module::ParameterWidgets::kDetune` |
| `DCO 2 Frequency < LFO 1` | `FREQ < LFO 1` | `DCO 2` | `PatchEditSection::Dco2Module::ParameterWidgets::kFrequencyModByLfo1` |
| `DCO 2 Frequency` | `FREQUENCY` | `DCO 2` | `PatchEditSection::Dco2Module::ParameterWidgets::kFrequency` |
| `DCO 2 Key Click` | `KEY CLICK` | `DCO 2` | `PatchEditSection::Dco2Module::ParameterWidgets::kKeyClick` |
| `DCO 2 Keyboard/Portamento` | `KEYBD/PORTA` | `DCO 2` | `PatchEditSection::Dco2Module::ParameterWidgets::kKeyboardPortamento` |
| `DCO 2 Levers` | `LEVERS` | `DCO 2` | `PatchEditSection::Dco2Module::ParameterWidgets::kLevers` |
| `DCO 2 Pulse Width` | `PULSE WIDTH` | `DCO 2` | `PatchEditSection::Dco2Module::ParameterWidgets::kPulseWidth` |
| `DCO 2 Pulse Width < LFO 2` | `PW < LFO 2` | `DCO 2` | `PatchEditSection::Dco2Module::ParameterWidgets::kPulseWidthModByLfo2` |
| `DCO 2 Wave Select` | `WAVE SELECT` | `DCO 2` | `PatchEditSection::Dco2Module::ParameterWidgets::kWaveSelect` |
| `DCO 2 Wave Shape` | `WAVE SHAPE` | `DCO 2` | `PatchEditSection::Dco2Module::ParameterWidgets::kWaveShape` |
| `ENV 1 Amplitude < Velocity` | `AMP < VELOCITY` | `ENV 1` | `PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitudeModByVelocity` |
| `ENV 1 Amplitude` | `AMPLITUDE` | `ENV 1` | `PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitude` |
| `ENV 1 Attack` | `ATTACK` | `ENV 1` | `PatchEditSection::Envelope1Module::ParameterWidgets::kAttack` |
| `ENV 1 Decay` | `DECAY` | `ENV 1` | `PatchEditSection::Envelope1Module::ParameterWidgets::kDecay` |
| `ENV 1 Delay` | `DELAY` | `ENV 1` | `PatchEditSection::Envelope1Module::ParameterWidgets::kDelay` |
| `ENV 1 Envelope Mode` | `ENVELOPE MODE` | `ENV 1` | `PatchEditSection::Envelope1Module::ParameterWidgets::kEnvelopeMode` |
| `ENV 1 LFO 1 Trigger` | `LFO 1 TRIGGER` | `ENV 1` | `PatchEditSection::Envelope1Module::ParameterWidgets::kLfo1Trigger` |
| `ENV 1 Release` | `RELEASE` | `ENV 1` | `PatchEditSection::Envelope1Module::ParameterWidgets::kRelease` |
| `ENV 1 Sustain` | `SUSTAIN` | `ENV 1` | `PatchEditSection::Envelope1Module::ParameterWidgets::kSustain` |
| `ENV 1 Trigger Mode` | `TRIGGER MODE` | `ENV 1` | `PatchEditSection::Envelope1Module::ParameterWidgets::kTriggerMode` |
| `ENV 2 Amplitude < Velocity` | `AMP < VELOCITY` | `ENV 2` | `PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitudeModByVelocity` |
| `ENV 2 Amplitude` | `AMPLITUDE` | `ENV 2` | `PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitude` |
| `ENV 2 Attack` | `ATTACK` | `ENV 2` | `PatchEditSection::Envelope2Module::ParameterWidgets::kAttack` |
| `ENV 2 Decay` | `DECAY` | `ENV 2` | `PatchEditSection::Envelope2Module::ParameterWidgets::kDecay` |
| `ENV 2 Delay` | `DELAY` | `ENV 2` | `PatchEditSection::Envelope2Module::ParameterWidgets::kDelay` |
| `ENV 2 Envelope Mode` | `ENVELOPE MODE` | `ENV 2` | `PatchEditSection::Envelope2Module::ParameterWidgets::kEnvelopeMode` |
| `ENV 2 LFO 1 Trigger` | `LFO 1 TRIGGER` | `ENV 2` | `PatchEditSection::Envelope2Module::ParameterWidgets::kLfo1Trigger` |
| `ENV 2 Release` | `RELEASE` | `ENV 2` | `PatchEditSection::Envelope2Module::ParameterWidgets::kRelease` |
| `ENV 2 Sustain` | `SUSTAIN` | `ENV 2` | `PatchEditSection::Envelope2Module::ParameterWidgets::kSustain` |
| `ENV 2 Trigger Mode` | `TRIGGER MODE` | `ENV 2` | `PatchEditSection::Envelope2Module::ParameterWidgets::kTriggerMode` |
| `ENV 3 Amplitude < Velocity` | `AMP < VELOCITY` | `ENV 3` | `PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitudeModByVelocity` |
| `ENV 3 Amplitude` | `AMPLITUDE` | `ENV 3` | `PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitude` |
| `ENV 3 Attack` | `ATTACK` | `ENV 3` | `PatchEditSection::Envelope3Module::ParameterWidgets::kAttack` |
| `ENV 3 Decay` | `DECAY` | `ENV 3` | `PatchEditSection::Envelope3Module::ParameterWidgets::kDecay` |
| `ENV 3 Delay` | `DELAY` | `ENV 3` | `PatchEditSection::Envelope3Module::ParameterWidgets::kDelay` |
| `ENV 3 Envelope Mode` | `ENVELOPE MODE` | `ENV 3` | `PatchEditSection::Envelope3Module::ParameterWidgets::kEnvelopeMode` |
| `ENV 3 LFO 1 Trigger` | `LFO 1 TRIGGER` | `ENV 3` | `PatchEditSection::Envelope3Module::ParameterWidgets::kLfo1Trigger` |
| `ENV 3 Release` | `RELEASE` | `ENV 3` | `PatchEditSection::Envelope3Module::ParameterWidgets::kRelease` |
| `ENV 3 Sustain` | `SUSTAIN` | `ENV 3` | `PatchEditSection::Envelope3Module::ParameterWidgets::kSustain` |
| `ENV 3 Trigger Mode` | `TRIGGER MODE` | `ENV 3` | `PatchEditSection::Envelope3Module::ParameterWidgets::kTriggerMode` |
| `FM/Track FM < ENV 3` | `FM < ENV 3` | `FM/TRACK` | `PatchEditSection::FmTrackModule::ParameterWidgets::kFmModByEnv3` |
| `FM/Track FM < Pressure` | `FM < PRESSURE` | `FM/TRACK` | `PatchEditSection::FmTrackModule::ParameterWidgets::kFmModByPressure` |
| `FM/Track Track Input` | `TRACK INPUT` | `FM/TRACK` | `PatchEditSection::FmTrackModule::ParameterWidgets::kTrackInput` |
| `FM/Track Track Point 1` | `TRACK POINT 1` | `FM/TRACK` | `PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint1` |
| `FM/Track Track Point 2` | `TRACK POINT 2` | `FM/TRACK` | `PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint2` |
| `FM/Track Track Point 3` | `TRACK POINT 3` | `FM/TRACK` | `PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3` |
| `FM/Track Track Point 4` | `TRACK POINT 4` | `FM/TRACK` | `PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint4` |
| `FM/Track Track Point 5` | `TRACK POINT 5` | `FM/TRACK` | `PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint5` |
| `FM/Track VCF FM Amount` | `VCF FM AMOUNT` | `FM/TRACK` | `PatchEditSection::FmTrackModule::ParameterWidgets::kFmAmount` |
| `LFO 1 Amplitude < Ramp 1` | `AMP < RAMP 1` | `LFO 1` | `PatchEditSection::Lfo1Module::ParameterWidgets::kAmplitudeModByRamp1` |
| `LFO 1 Amplitude` | `AMPLITUDE` | `LFO 1` | `PatchEditSection::Lfo1Module::ParameterWidgets::kAmplitude` |
| `LFO 1 Lag` | `LAG` | `LFO 1` | `PatchEditSection::Lfo1Module::ParameterWidgets::kLag` |
| `LFO 1 Retrigger Point` | `RETRIGGER POINT` | `LFO 1` | `PatchEditSection::Lfo1Module::ParameterWidgets::kRetriggerPoint` |
| `LFO 1 Sample Input` | `SAMPLE INPUT` | `LFO 1` | `PatchEditSection::Lfo1Module::ParameterWidgets::kSampleInput` |
| `LFO 1 Speed` | `SPEED` | `LFO 1` | `PatchEditSection::Lfo1Module::ParameterWidgets::kSpeed` |
| `LFO 1 Speed < Pressure` | `SPEED < PRESSURE` | `LFO 1` | `PatchEditSection::Lfo1Module::ParameterWidgets::kSpeedModByPressure` |
| `LFO 1 Trigger Mode` | `TRIGGER MODE` | `LFO 1` | `PatchEditSection::Lfo1Module::ParameterWidgets::kTriggerMode` |
| `LFO 1 Waveform` | `WAVEFORM` | `LFO 1` | `PatchEditSection::Lfo1Module::ParameterWidgets::kWaveform` |
| `LFO 2 Amplitude < Ramp 2` | `AMP < RAMP 2` | `LFO 2` | `PatchEditSection::Lfo2Module::ParameterWidgets::kAmplitudeModByRamp2` |
| `LFO 2 Amplitude` | `AMPLITUDE` | `LFO 2` | `PatchEditSection::Lfo2Module::ParameterWidgets::kAmplitude` |
| `LFO 2 Lag` | `LAG` | `LFO 2` | `PatchEditSection::Lfo2Module::ParameterWidgets::kLag` |
| `LFO 2 Retrigger Point` | `RETRIGGER POINT` | `LFO 2` | `PatchEditSection::Lfo2Module::ParameterWidgets::kRetriggerPoint` |
| `LFO 2 Sample Input` | `SAMPLE INPUT` | `LFO 2` | `PatchEditSection::Lfo2Module::ParameterWidgets::kSampleInput` |
| `LFO 2 Speed` | `SPEED` | `LFO 2` | `PatchEditSection::Lfo2Module::ParameterWidgets::kSpeed` |
| `LFO 2 Speed < Keyboard` | `SPEED < KEYBD` | `LFO 2` | `PatchEditSection::Lfo2Module::ParameterWidgets::kSpeedModByKeyboard` |
| `LFO 2 Trigger Mode` | `TRIGGER MODE` | `LFO 2` | `PatchEditSection::Lfo2Module::ParameterWidgets::kTriggerMode` |
| `LFO 2 Waveform` | `WAVEFORM` | `LFO 2` | `PatchEditSection::Lfo2Module::ParameterWidgets::kWaveform` |
| `MIDI Channel` | `CHANNEL` | `MIDI` | `MasterEditSection::MidiModule::ParameterWidgets::kChannel` |
| `MIDI Controllers` | `CONTROLLERS` | `MIDI` | `MasterEditSection::MidiModule::ParameterWidgets::kControllers` |
| `MIDI Lever 2 Select` | `LEVER 2 SELECT` | `MIDI` | `MasterEditSection::MidiModule::ParameterWidgets::kLever2Select` |
| `MIDI Lever 3 Select` | `LEVER 3 SELECT` | `MIDI` | `MasterEditSection::MidiModule::ParameterWidgets::kLever3Select` |
| `MIDI Echo` | `MIDI ECHO` | `MIDI` | `MasterEditSection::MidiModule::ParameterWidgets::kMidiEcho` |
| `MIDI Patch Changes` | `PATCH CHANGES` | `MIDI` | `MasterEditSection::MidiModule::ParameterWidgets::kPatchChanges` |
| `MIDI Pedal 1 Select` | `PEDAL 1 SELECT` | `MIDI` | `MasterEditSection::MidiModule::ParameterWidgets::kPedal1Select` |
| `MIDI Pedal 2 Select` | `PEDAL 2 SELECT` | `MIDI` | `MasterEditSection::MidiModule::ParameterWidgets::kPedal2Select` |
| `Misc Bank Lock` | `BANK LOCK` | `MISC` | `MasterEditSection::MiscModule::ParameterWidgets::kBankLockEnable` |
| `Misc Bend Range (+/-)` | `BEND RANGE (+/-)` | `MISC` | `MasterEditSection::MiscModule::ParameterWidgets::kBendRange` |
| `Misc Master Transpose` | `MASTER TRANSPOSE` | `MISC` | `MasterEditSection::MiscModule::ParameterWidgets::kMasterTranspose` |
| `Misc Master Tune` | `MASTER TUNE` | `MISC` | `MasterEditSection::MiscModule::ParameterWidgets::kMasterTune` |
| `Misc Memory Protect` | `MEMORY PROTECT` | `MISC` | `MasterEditSection::MiscModule::ParameterWidgets::kMemoryProtectEnable` |
| `Misc Unison` | `UNISON` | `MISC` | `MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable` |
| `Misc Unison Detune` | `UNISON DETUNE` | `MISC` | `MasterEditSection::MiscModule::ParameterWidgets::kUnisonDetune` |
| `Misc Volume Invert` | `VOLUME INVERT` | `MISC` | `MasterEditSection::MiscModule::ParameterWidgets::kVolumeInvertEnable` |
| `Patch Mutator Amount` | `AMOUNT` | `PATCH MUTATOR` | `PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount` |
| `Patch Mutator Random` | `RANDOM` | `PATCH MUTATOR` | `PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom` |
| `Ramp/Portamento Keyboard Mode` | `KEYBOARD MODE` | `RAMP/PORTAMENTO` | `PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode` |
| `Ramp/Portamento Legato Portamento` | `LEGATO PORTA` | `RAMP/PORTAMENTO` | `PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoLegato` |
| `Ramp/Portamento Portamento < Velocity` | `PORTA < VELOCITY` | `RAMP/PORTAMENTO` | `PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoModByVelocity` |
| `Ramp/Portamento Portamento Mode` | `PORTA MODE` | `RAMP/PORTAMENTO` | `PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoMode` |
| `Ramp/Portamento Portamento Rate` | `PORTAMENTO RATE` | `RAMP/PORTAMENTO` | `PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoRate` |
| `Ramp/Portamento Ramp 1 Rate` | `RAMP 1 RATE` | `RAMP/PORTAMENTO` | `PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Rate` |
| `Ramp/Portamento Ramp 1 Trigger` | `RAMP 1 TRIGGER` | `RAMP/PORTAMENTO` | `PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Trigger` |
| `Ramp/Portamento Ramp 2 Rate` | `RAMP 2 RATE` | `RAMP/PORTAMENTO` | `PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Rate` |
| `Ramp/Portamento Ramp 2 Trigger` | `RAMP 2 TRIGGER` | `RAMP/PORTAMENTO` | `PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Trigger` |
| `VCF/VCA DCO 2 | DCO 1 Mix` | `DCO 2 | DCO 1 MIX` | `VCF/VCA` | `PatchEditSection::VcfVcaModule::ParameterWidgets::kBalance` |
| `VCF/VCA Frequency < ENV 1` | `FREQ < ENV 1` | `VCF/VCA` | `PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequencyModByEnv1` |
| `VCF/VCA Frequency < Pressure` | `FREQ < PRESSURE` | `VCF/VCA` | `PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequencyModByPressure` |
| `VCF/VCA Frequency` | `FREQUENCY` | `VCF/VCA` | `PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequency` |
| `VCF/VCA Keyboard/Portamento` | `KEYBD/PORTA` | `VCF/VCA` | `PatchEditSection::VcfVcaModule::ParameterWidgets::kKeyboardPortamento` |
| `VCF/VCA Levers` | `LEVERS` | `VCF/VCA` | `PatchEditSection::VcfVcaModule::ParameterWidgets::kLevers` |
| `VCF/VCA Resonance` | `RESONANCE` | `VCF/VCA` | `PatchEditSection::VcfVcaModule::ParameterWidgets::kResonance` |
| `VCF/VCA VCA 1 < Velocity` | `VCA 1 < VELOCITY` | `VCF/VCA` | `PatchEditSection::VcfVcaModule::ParameterWidgets::kVca1ModByVelocity` |
| `VCF/VCA VCA 1 Volume` | `VCA 1 VOLUME` | `VCF/VCA` | `PatchEditSection::VcfVcaModule::ParameterWidgets::kVca1Volume` |
| `VCF/VCA VCA 2 < ENV 2` | `VCA 2 < ENV 2` | `VCF/VCA` | `PatchEditSection::VcfVcaModule::ParameterWidgets::kVca2ModByEnv2` |
| `Vibrato Amplitude Modulation Amount` | `AMP MOD AMOUNT` | `VIBRATO` | `MasterEditSection::VibratoModule::ParameterWidgets::kAmpModAmount` |
| `Vibrato Amplitude Modulation Source` | `AMP MOD SOURCE` | `VIBRATO` | `MasterEditSection::VibratoModule::ParameterWidgets::kAmpModSource` |
| `Vibrato Amplitude` | `AMPLITUDE` | `VIBRATO` | `MasterEditSection::VibratoModule::ParameterWidgets::kAmplitude` |
| `Vibrato Speed` | `SPEED` | `VIBRATO` | `MasterEditSection::VibratoModule::ParameterWidgets::kSpeed` |
| `Vibrato Speed Modulation Amount` | `SPEED MOD AMOUNT` | `VIBRATO` | `MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModAmount` |
| `Vibrato Speed Modulation Source` | `SPEED MOD SOURCE` | `VIBRATO` | `MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModSource` |
| `Vibrato Waveform` | `WAVEFORM` | `VIBRATO` | `MasterEditSection::VibratoModule::ParameterWidgets::kWaveform` |

Total: 151 parameters
