---
name: PluginDisplayNames namespace refactor
overview: Refactoriser [Source/Shared/PluginDisplayNames.h](Source/Shared/PluginDisplayNames.h) en introduisant une hiérarchie Section → Module → ParameterWidgets / StandaloneWidgets et en restructurant ChoiceLists en sous-namespaces, sans modifier le namespace racine PluginDescriptors ni le fichier PluginDescriptors.h.
todos:
  - id: rewrite-display-names-header
    content: Réécrire PluginDisplayNames.h avec la hiérarchie Section → Module → ParameterWidgets/StandaloneWidgets et ChoiceLists en sous-namespaces
    status: completed
  - id: update-plugin-descriptors-cpp
    content: Mettre à jour toutes les références dans PluginDescriptors.cpp (ParameterDisplayNames et ChoiceLists)
    status: completed
  - id: update-gui-files
    content: Mettre à jour les fichiers GUI (ModulationBusHeader, panels PatchManager, MatrixModulation, ModulationBusPanel, PatchNameDisplay, MiddlePanel, panels modules)
    status: completed
  - id: verify-build-and-grep
    content: Compiler le projet et vérifier qu’il ne reste aucun ancien chemin (grep ParameterDisplayNames::, StandaloneWidgetDisplayNames::)
    status: completed
  - id: archive-plan
    content: Copier le plan final dans Documentation/Development/Plans/ sous le nom 2026-02-23-PluginDisplayNames-Namespace-Hierarchy-Refactor.md
    status: completed
isProject: false
---

# Refactorisation PluginDisplayNames.h – Hiérarchie par namespaces

## Objectifs

- Supprimer la répétition des préfixes (kMidiX, kDco1X, kVibratoX, etc.) en portant le contexte dans la hiérarchie des namespaces.
- Regrouper par module : ParameterWidgets et StandaloneWidgets d’un même module sous le même namespace (Section → Module → ParameterWidgets / StandaloneWidgets).
- Restructurer ChoiceLists en sous-namespaces (Sync, WaveSelect, OnOff, MidiChannel, Source, Destination, etc.).
- Conserver le namespace racine `PluginDescriptors` et ne pas modifier [PluginDescriptors.h](Source/Shared/PluginDescriptors.h).

## Fichier unique modifié côté structure

- [Source/Shared/PluginDisplayNames.h](Source/Shared/PluginDisplayNames.h) : réécriture de la structure des namespaces et des constantes.

## Ce qui reste inchangé (au même niveau qu’aujourd’hui)

Sous `namespace PluginDescriptors` :

- `kPluginName` (à la racine de PluginDescriptors).
- `ModeDisplayNames` (kMaster, kPatch).
- `SectionDisplayNames` (kMasterEdit, kPatchEdit, kMatrixModulation, kPatchManager).
- `ModuleDisplayNames` (kMidi, kVibrato, kDco1, …).
- `ModulationBusDisplayNames` (kModulationBus0 … kModulationBus9).

## Nouvelle hiérarchie à introduire

Tout ce qui suit reste sous `namespace PluginDescriptors`.

### 1. Labels courts partagés (référencés par StandaloneWidgets)

Les constantes `kShortInitLabel`, `kShortCopyLabel`, `kShortPasteLabel` doivent rester accessibles depuis chaque `StandaloneWidgets`. Options :

- Soit un petit namespace dédié au même niveau que les sections, par ex. `ShortLabels` avec `kInit`, `kCopy`, `kPaste` (valeurs "I", "C", "P"), et dans chaque module `StandaloneWidgets::kInit = ShortLabels::kInit`, etc.
- Soit les laisser à la racine de PluginDescriptors avec des noms explicites (ex. `kStandaloneShortLabelInit`, `kStandaloneShortLabelCopy`, `kStandaloneShortLabelPaste`) et les référencer depuis chaque `StandaloneWidgets`.

Recommandation : namespace `ShortLabels` sous PluginDescriptors avec `kInit`, `kCopy`, `kPaste` pour garder des noms courts et un seul endroit de définition.

### 2. Section MasterEdit → Module → ParameterWidgets / StandaloneWidgets

```text
MasterEdit::
  Midi::
    ParameterWidgets::   kChannel, kEcho, kControllers, kPatchChanges, kPedal1Select, kPedal2Select, kLever2Select, kLever3Select
    StandaloneWidgets::  kInit (= ShortLabels::kInit)
  Vibrato::
    ParameterWidgets::   kSpeed, kSpeedModSource, kSpeedModAmount, kWaveform, kAmplitude, kAmpModSource, kAmpModAmount
    StandaloneWidgets::  kInit
  Misc::
    ParameterWidgets::   kMasterTune, kMasterTranspose, kBendRange, kUnisonEnable, kVolumeInvertEnable, kBankLockEnable, kMemoryProtectEnable
    StandaloneWidgets::  kInit
```

### 3. Section PatchEdit → Module → ParameterWidgets / StandaloneWidgets

- **Dco1** : ParameterWidgets (kFrequency, kFrequencyModByLfo1, kPulseWidth, … kKeyClick), StandaloneWidgets (kInit, kCopy, kPaste).
- **Dco2** : idem avec kDetune en plus.
- **VcfVca** : ParameterWidgets (kBalance, kFrequency, … kVcfKeyboardPortamento), StandaloneWidgets (kInit).
- **FmTrack** : ParameterWidgets (kFmAmount, kFmModByEnv3, … kTrackInput), StandaloneWidgets (kInit).
- **RampPortamento** : ParameterWidgets (kRamp1Rate, kRamp2Rate, … kPortamentoKeyboardMode), StandaloneWidgets (kInit).
- **Envelope1, Envelope2, Envelope3** : ParameterWidgets (kDelay, kAttack, … kLfo1Trigger), StandaloneWidgets (kInit, kCopy, kPaste).
- **Lfo1, Lfo2** : ParameterWidgets (kSpeed, … kSampleInput), StandaloneWidgets (kInit, kCopy, kPaste).
- **PatchName** (affichage seul) : un seul namespace par ex. StandaloneWidgets avec `kDefaultPatchName` = "--------".

### 4. Section MatrixModulation

- Pas de découpage en “modules” comme MasterEdit/PatchEdit. Un namespace par ex. `MatrixModulation::Header` pour les libellés d’en-tête : kBusNumber ("#"), kSource ("SOURCE"), kDestination ("DESTINATION"), kAmount ("AMOUNT"). Les bus 0–9 restent dans `ModulationBusDisplayNames` (inchangé).

### 5. Section PatchManager → Module → StandaloneWidgets uniquement

- **BankUtility** : kBankSelector, kUnlockBank, kSelectBank0 … kSelectBank9.
- **InternalPatches** : kBrowser, kMemory, kLoadPreviousPatch, kLoadNextPatch, kCurrentBankNumber, kCurrentPatchNumber, kInitPatch, kCopyPatch, kPastePatch, kStorePatch.
- **ComputerPatches** : kBrowser, kStorage, kLoadPreviousPatchFile, kLoadNextPatchFile, kSelectPatchFile, kOpenPatchFolder, kSavePatchAsFile, kSavePatch.
- **PatchMutator** : kAmount, kRandom, kHistory, kEmptyHistory, kMutate, kRetry, kCompare, kDelete (juce::String), kClear, kExport, kD1 … kL2, kUnitPercent.

Pour éviter les conflits de noms entre InternalPatches et ComputerPatches (ex. kBrowser), les noms de constantes peuvent rester légèrement préfixés dans le nom du symbole si nécessaire (ex. kInternalPatchesBrowser → InternalPatches::StandaloneWidgets::kBrowser), ou garder kBrowser dans chaque module (même nom, namespace différent).

### 6. ChoiceLists : sous-namespaces

Conserver `namespace ChoiceLists` à la racine de PluginDescriptors, avec des sous-namespaces (noms courts pour les symboles à l’intérieur) :

- **Sync** : kOff, kSoft, kMedium, kHard
- **WaveSelect** : kOff, kPulse, kWave, kBoth, kNoise
- **Levers** : kOff, kL1Bend, kL2Vib, kBoth
- **KeyboardPortamento** : kOff, kKeybd, kPorta
- **OnOff** : kOff, kOn
- **TriggerMode** (enveloppes) : kStrig, kMtrig, kSreset, kMreset, kXtrig, kXmtrig, kXreset, kXmrst
- **EnvelopeMode** : kNormal, kDadr, kFree, kBoth
- **Lfo1Trigger** : kNormal, kLfo1, kGLfo1, kGatedLfo1Trigger
- **LfoTriggerMode** : kOff, kStrig, kMtrig, kXtrig
- **LfoWaveform** : kTriangle, kUpsaw, kDnsaw, kSquare, kRandom, kNoise, kSampled
- **RampTrigger** : kStrig, kMtrig, kExtrig, kGatedx
- **PortamentoMode** : kLinear, kConst, kExpo
- **PortamentoKeyboardMode** : kRotate, kReasgn, kUnison, kRearob
- **Source** : kNone, kEnv1, kEnv2, … kLever3
- **Destination** : kNone, kDco1Frequency, … kPortamentoRate
- **MidiChannel** : kOmni, k1 … k16, kMonoG1 … kMonoG9

Usage après refactor : `ChoiceLists::Sync::kOff`, `ChoiceLists::OnOff::kOn`, `ChoiceLists::MidiChannel::kOmni`, etc.

## Mapping des usages (fichiers à mettre à jour)


| Fichier                                                                                                                                        | Type de changement                                                                                                                                                                                    |
| ---------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [Source/Shared/PluginDescriptors.cpp](Source/Shared/PluginDescriptors.cpp)                                                                     | ~219 références ParameterDisplayNames → nouveaux chemins (MasterEdit::Midi::ParameterWidgets::kChannel, etc.) ; ~50+ références ChoiceLists → ChoiceLists::Sync::kOff, ChoiceLists::OnOff::kOff, etc. |
| [Source/GUI/Widgets/ModulationBusHeader.cpp](Source/GUI/Widgets/ModulationBusHeader.cpp)                                                       | ParameterDisplayNames::kModulationBusNumber etc. → MatrixModulation::Header::kBusNumber (ou équivalent).                                                                                              |
| [Source/GUI/Panels/.../PatchMutatorPanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp)       | StandaloneWidgetDisplayNames::kPatchMutatorX → PatchManager::PatchMutator::StandaloneWidgets::kX.                                                                                                     |
| [Source/GUI/Panels/.../BankUtilityPanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/BankUtilityPanel.cpp)         | Idem BankUtility::StandaloneWidgets.                                                                                                                                                                  |
| [Source/GUI/Panels/.../InternalPatchesPanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/InternalPatchesPanel.cpp) | InternalPatches::StandaloneWidgets.                                                                                                                                                                   |
| [Source/GUI/Panels/.../ComputerPatchesPanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/ComputerPatchesPanel.cpp) | ComputerPatches::StandaloneWidgets.                                                                                                                                                                   |
| [Source/GUI/Panels/.../MatrixModulationPanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/MatrixModulationPanel/MatrixModulationPanel.cpp)   | Références display names matrix / header.                                                                                                                                                             |
| [Source/GUI/Panels/Reusable/ModulationBusPanel.cpp](Source/GUI/Panels/Reusable/ModulationBusPanel.cpp)                                         | Idem si utilisation de display names.                                                                                                                                                                 |
| [Source/GUI/Widgets/PatchNameDisplay.cpp](Source/GUI/Widgets/PatchNameDisplay.cpp)                                                             | kDefaultPatchName → PatchEdit::PatchName::StandaloneWidgets::kDefaultPatchName (ou équivalent).                                                                                                       |
| [Source/GUI/Panels/.../MiddlePanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/MiddlePanel/MiddlePanel.cpp)                  | Si utilisation de display names.                                                                                                                                                                      |
| Tous les panels de modules (Dco1Panel, Dco2Panel, MidiPanel, VibratoPanel, …)                                                                  | Remplacer ParameterDisplayNames::kXxx et StandaloneWidgetDisplayNames::kXxx par les chemins Section::Module::ParameterWidgets / StandaloneWidgets.                                                    |


Les références à ModeDisplayNames, SectionDisplayNames, ModuleDisplayNames et ModulationBusDisplayNames restent inchangées (mêmes noms de namespace et de constantes).

## Ordre d’exécution recommandé

1. Réécrire [PluginDisplayNames.h](Source/Shared/PluginDisplayNames.h) avec la nouvelle hiérarchie (ShortLabels, MasterEdit, PatchEdit, MatrixModulation, PatchManager, ChoiceLists en sous-namespaces). Vérifier que le projet compile après cette étape ne sera pas possible tant que les appels ne sont pas mis à jour ; donc soit faire en une fois, soit ajouter des alias temporaires (déconseillé pour la maintenance).
2. Mettre à jour [PluginDescriptors.cpp](Source/Shared/PluginDescriptors.cpp) en masse : remplacer chaque ancien chemin par le nouveau (ParameterDisplayNames::kMidiChannel → MasterEdit::Midi::ParameterWidgets::kChannel, ChoiceLists::kSyncOff → ChoiceLists::Sync::kOff, etc.).
3. Mettre à jour les fichiers GUI un par un (ModulationBusHeader, PatchMutatorPanel, BankUtilityPanel, InternalPatchesPanel, ComputerPatchesPanel, MatrixModulationPanel, ModulationBusPanel, PatchNameDisplay, MiddlePanel, puis chaque module panel Dco1, Dco2, VcfVca, FmTrack, RampPortamento, Env1/2/3, Lfo1/2, Midi, Vibrato, Misc).
4. Compilation complète et vérification (recherche globale de ParameterDisplayNames:: et StandaloneWidgetDisplayNames:: pour s’assurer qu’il ne reste aucun ancien chemin).
5. Archiver le plan : copier le fichier de plan final dans `Documentation/Development/Plans/` avec le nom `2026-02-23-PluginDisplayNames-Namespace-Hierarchy-Refactor.md` (convention .cursorrules §9.2).

## Convention de nommage (rappel)

- Namespaces en PascalCase (MasterEdit, PatchEdit, Midi, Dco1, ParameterWidgets, StandaloneWidgets).
- Constantes avec préfixe k (kChannel, kInit, kOff, kOn).
- Pas de magic numbers ; valeurs littérales des chaînes inchangées (texte affiché à l’utilisateur).

## Risques et points d’attention

- **Typo existante** : dans l’actuel PluginDisplayNames.h, `kVcfFequency` (ligne 150) — à corriger en `kVcfFrequency` dans VcfVca::ParameterWidgets si tu en profites pour uniformiser.
- **kPatchMutatorDelete** : actuellement `const juce::String` (UTF-8). À conserver avec le même type dans PatchManager::PatchMutator::StandaloneWidgets.
- Vérifier que tous les usages de `ParameterDisplayNames` et `StandaloneWidgetDisplayNames` sont bien recensés (grep sur tout le dépôt) avant de supprimer les anciens namespaces.

---

Une fois ce plan validé et l’implémentation terminée, le fichier de plan devra être déplacé/renommé dans `Documentation/Development/Plans/` en `2026-02-23-PluginDisplayNames-Namespace-Hierarchy-Refactor.md` conformément à tes cursorrules.