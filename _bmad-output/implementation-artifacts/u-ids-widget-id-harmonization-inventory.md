---
organization: Ten Square Software
project: Matrix-Control
title: Story U-IDs — Widget ID Harmonization Inventory
author: BMad Brainstorming Session
status: ready
version: "1.0"
sources:
  - brainstorming/brainstorming-session-2026-06-07-ui-dimensions-strategy.md
  - Source/Shared/Definitions/PluginIDs.h
created: 2026-06-07
updated: 2026-06-07
---

# Story U-IDs: Widget ID Harmonization Inventory

Complete rename checklist derived from brainstorming session 2026-06-07.

**Convention:** `{moduleScope}{actionVerb}{optionalTarget}` for standalone widgets; `{moduleScope}{semanticName}` for APVTS `parameterId` values.

**Migration:** None — plugin not public; direct rename, remove dead references, no legacy aliases.

**Implementation order:** `PluginIDs.h` → `PluginDisplayNames.h` → `PluginDescriptors*.cpp` → all references (Factory, panels, Core mappers if any) → build + tests.

---

## Summary

| Category | Unchanged | Renamed |
|----------|-----------|---------|
| Standalone buttons | 34 | 32 |
| Standalone other (NumberBox, ComboBox) | 3 | 3 |
| APVTS parameters (`parameterId`) | 120 | 36 |
| Settings IDs | 0 | 3 |
| Mode IDs | 2 | 0 |
| APVTS group IDs | all | 0 |
| **Total string IDs reviewed** | **~159** | **74** |

---

## 1. Unchanged — already conformant

### 1.1 Standalone buttons (34)

| Module | IDs |
|--------|-----|
| Master Edit | `midiInit`, `vibratoInit`, `miscInit` |
| Patch Edit I/C/P | `dco1Init`, `dco1Copy`, `dco1Paste`, `dco2Init`, `dco2Copy`, `dco2Paste`, `env1Init`, `env1Copy`, `env1Paste`, `env2Init`, `env2Copy`, `env2Paste`, `env3Init`, `env3Copy`, `env3Paste`, `lfo1Init`, `lfo1Copy`, `lfo1Paste`, `lfo2Init`, `lfo2Copy`, `lfo2Paste` |
| Patch Edit Init only | `vcfVcaInit`, `fmTrackInit`, `rampPortamentoInit` |
| Matrix Mod | `matrixModulationInit`, `modulationBus0Init` … `modulationBus9Init` |
| Patch Mutator actions | `patchMutatorMutate`, `patchMutatorRetry`, `patchMutatorCompare`, `patchMutatorDelete`, `patchMutatorClear`, `patchMutatorExport` |

### 1.2 Standalone non-button (3)

| ID | Type |
|----|------|
| `patchMutatorAmount` | Label |
| `patchMutatorRandom` | Label |
| `patchMutatorHistory` | ComboBox |

### 1.3 APVTS parameters — DCO, Envelope, LFO, MIDI, Vibrato, Matrix Mod (120)

All `parameterId` values already start with module scope:

- **Dco1Module:** `dco1Frequency` … `dco1KeyClick` (10)
- **Dco2Module:** `dco2Frequency` … `dco2KeyClick` (10)
- **Envelope1Module:** `env1Delay` … `env1Lfo1Trigger` (9)
- **Envelope2Module:** `env2Delay` … `env2Lfo1Trigger` (9)
- **Envelope3Module:** `env3Delay` … `env3Lfo1Trigger` (9)
- **Lfo1Module:** `lfo1Speed` … `lfo1SampleInput` (9)
- **Lfo2Module:** `lfo2Speed` … `lfo2SampleInput` (9)
- **MidiModule:** `midiChannel` … `midiLever3Select` (8)
- **VibratoModule:** `vibratoSpeed` … `vibratoAmpModAmount` (7)
- **MatrixModulation ModulationBus:** `modulationBus0Source` … `modulationBus9Destination` (30)

### 1.4 Mode & group IDs

| Category | IDs | Note |
|----------|-----|------|
| Mode | `masterMode`, `patchMode` | `Mode` namespace scopes adequately |
| APVTS groups | `masterEditSection`, `dco1Module`, `internalPatchesBrowserGroup`, … | No change |

---

## 2. Renamed — standalone buttons (32)

| Current | New | C++ symbol change |
|---------|-----|-------------------|
| `unlockBank` | `bankUtilityLockBank` | `kUnlockBank` → `kLockBank` |
| `selectBank0` | `bankUtilitySelectBank0` | — |
| `selectBank1` | `bankUtilitySelectBank1` | — |
| `selectBank2` | `bankUtilitySelectBank2` | — |
| `selectBank3` | `bankUtilitySelectBank3` | — |
| `selectBank4` | `bankUtilitySelectBank4` | — |
| `selectBank5` | `bankUtilitySelectBank5` | — |
| `selectBank6` | `bankUtilitySelectBank6` | — |
| `selectBank7` | `bankUtilitySelectBank7` | — |
| `selectBank8` | `bankUtilitySelectBank8` | — |
| `selectBank9` | `bankUtilitySelectBank9` | — |
| `loadPreviousPatch` | `internalPatchesLoadPrevious` | — |
| `loadNextPatch` | `internalPatchesLoadNext` | — |
| `initPatch` | `internalPatchesInit` | — |
| `copyPatch` | `internalPatchesCopy` | — |
| `pastePatch` | `internalPatchesPaste` | — |
| `storePatch` | `internalPatchesStore` | — |
| `loadPreviousPatchFile` | `computerPatchesLoadPrevious` | — |
| `loadNextPatchFile` | `computerPatchesLoadNext` | — |
| `openPatchFolder` | `computerPatchesOpenFolder` | — |
| `savePatchAsFile` | `computerPatchesSaveAs` | — |
| `savePatchFile` | `computerPatchesSave` | — |
| `patchMutatorDco1` | `patchMutatorEnableDco1` | `kDco1` → `kEnableDco1` |
| `patchMutatorDco2` | `patchMutatorEnableDco2` | `kDco2` → `kEnableDco2` |
| `patchMutatorVcfVca` | `patchMutatorEnableVcfVca` | `kVcfVca` → `kEnableVcfVca` |
| `patchMutatorFmTrack` | `patchMutatorEnableFmTrack` | `kFmTrack` → `kEnableFmTrack` |
| `patchMutatorRampPortamento` | `patchMutatorEnableRampPortamento` | — |
| `patchMutatorEnvelope1` | `patchMutatorEnableEnvelope1` | `kEnvelope1` → `kEnableEnvelope1` |
| `patchMutatorEnvelope2` | `patchMutatorEnableEnvelope2` | — |
| `patchMutatorEnvelope3` | `patchMutatorEnableEnvelope3` | — |
| `patchMutatorLfo1` | `patchMutatorEnableLfo1` | — |
| `patchMutatorLfo2` | `patchMutatorEnableLfo2` | — |

**Display name change (not ID):** `unlockBank` label → **BANK LOCK** (lock current bank behaviour).

---

## 3. Renamed — standalone non-button (3)

| Current | New | Type | C++ symbol |
|---------|-----|------|------------|
| `currentBankNumber` | `internalPatchesCurrentBankNumber` | NumberBox | `kCurrentBankNumber` → `kCurrentBankNumber` (string only) |
| `currentPatchNumber` | `internalPatchesCurrentPatchNumber` | NumberBox | — |
| `selectPatchFile` | `computerPatchesSelectPatch` | ComboBox | — |

---

## 4. Renamed — APVTS parameters (36)

### 4.1 MiscModule (7) — add `misc` scope prefix

| Current | New | C++ symbol |
|---------|-----|------------|
| `masterTune` | `miscMasterTune` | `kMasterTune` unchanged or align |
| `masterTranspose` | `miscMasterTranspose` | — |
| `bendRange` | `miscBendRange` | — |
| `unisonEnable` | `miscUnisonEnable` | — |
| `volumeInvertEnable` | `miscVolumeInvertEnable` | — |
| `bankLockEnable` | `miscBankLockEnable` | `kBankLockEnable` unchanged |
| `memoryProtectEnable` | `miscMemoryProtectEnable` | — |

**Note:** distinct from button `bankUtilityLockBank`.

### 4.2 VcfVcaModule (10) — unify under `vcfVca` scope

| Current | New |
|---------|-----|
| `vcfBalance` | `vcfVcaBalance` |
| `vcfFrequency` | `vcfVcaFrequency` |
| `vcfFrequencyModByEnv1` | `vcfVcaFrequencyModByEnv1` |
| `vcfFrequencyModByPressure` | `vcfVcaFrequencyModByPressure` |
| `vcfResonance` | `vcfVcaResonance` |
| `vca1Volume` | `vcfVcaVca1Volume` |
| `vca1ModByVelocity` | `vcfVcaVca1ModByVelocity` |
| `vca2ModByEnv2` | `vcfVcaVca2ModByEnv2` |
| `vcfLevers` | `vcfVcaLevers` |
| `vcfKeyboardPortamento` | `vcfVcaKeyboardPortamento` |

### 4.3 FmTrackModule (9) — add `fmTrack` scope prefix

| Current | New |
|---------|-----|
| `fmAmount` | `fmTrackFmAmount` |
| `fmModByEnv3` | `fmTrackFmModByEnv3` |
| `fmModByPressure` | `fmTrackFmModByPressure` |
| `trackPoint1` | `fmTrackPoint1` |
| `trackPoint2` | `fmTrackPoint2` |
| `trackPoint3` | `fmTrackPoint3` |
| `trackPoint4` | `fmTrackPoint4` |
| `trackPoint5` | `fmTrackPoint5` |
| `trackInput` | `fmTrackInput` |

### 4.4 RampPortamentoModule (9) — add `rampPortamento` scope prefix

| Current | New |
|---------|-----|
| `ramp1Rate` | `rampPortamentoRamp1Rate` |
| `ramp2Rate` | `rampPortamentoRamp2Rate` |
| `portamentoRate` | `rampPortamentoPortamentoRate` |
| `portamentoModByVelocity` | `rampPortamentoPortamentoModByVelocity` |
| `ramp1Trigger` | `rampPortamentoRamp1Trigger` |
| `ramp2Trigger` | `rampPortamentoRamp2Trigger` |
| `portamentoMode` | `rampPortamentoPortamentoMode` |
| `portamentoLegato` | `rampPortamentoPortamentoLegato` |
| `portamentoKeyboardMode` | `rampPortamentoPortamentoKeyboardMode` |

### 4.5 PatchNameModule (1)

| Current | New | Note |
|---------|-----|------|
| `patchName` | `patchEditPatchName` | APVTS patch name parameter in Patch Edit middle row |

---

## 5. Renamed — Settings (3)

| Current | New | C++ symbol suggestion |
|---------|-----|----------------------|
| `guiScaleId` | `settingsGuiScale` | `kGuiScaleId` → `kGuiScale` |
| `hardwareLatencyMsId` | `settingsHardwareLatencyMs` | `kHardwareLatencyMsId` → `kHardwareLatencyMs` |
| `testWidgetId` | `settingsTestWidget` | `kTestWidgetId` → `kTestWidget` |

---

## 6. Post-rename verification checklist

- [ ] `grep` old string IDs → zero hits in `Source/`
- [ ] `PluginDisplayNames.h` aligned with every renamed ID
- [ ] All `PluginDescriptors*.cpp` updated
- [ ] `WidgetFactory` / `WidgetFactoryValidator` maps rebuilt
- [ ] APVTS layout builders / mappers (`ApvtsPatchMapper`, `ApvtsMasterMapper`, …) updated
- [ ] Panel code referencing literal widget ID strings updated
- [ ] Remove `buttonWidth` from `StandaloneWidgetDescriptor` (Story U-0b — can follow U-IDs)
- [ ] Unit test: every standalone widget ID resolves in `WidgetDimensionRegistry`
- [ ] Full build Debug + Standalone smoke test

---

## 7. Registry rules preview (Story U-0b, after U-IDs)

After IDs are harmonized, `buttonWidthForWidgetId` hybrid rules simplify:

| Rule | IDs covered |
|------|-------------|
| `startsWith("bankUtilitySelectBank")` | 10 bank buttons |
| `== "bankUtilityLockBank"` | lock bank |
| `startsWith("internalPatches")` + action | load/init/copy/paste/store |
| `startsWith("computerPatches")` + action | load/open/save |
| `startsWith("patchMutatorEnable")` | 10 module enable toggles |
| `patchMutatorMutate/Retry/Compare/Delete/Clear/Export` | explicit atoms |
| `endsWith("Init")` / `Copy` / `Paste` | all module header I/C/P buttons |

---

*Inventory v1.0 — ready for Story U-IDs implementation.*
