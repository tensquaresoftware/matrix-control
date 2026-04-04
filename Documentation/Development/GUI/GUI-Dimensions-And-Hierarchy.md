# GUI — Dimensions (`PluginDimensions.h`) et hiérarchie des composants

**Source des dimensions :** `Source/Shared/Definitions/PluginDimensions.h`  
**Date de référence :** 2026-04-04  

Les valeurs ci-dessous sont les **tailles de design** (échelle 1:1). L’éditeur applique l’**échelle d’affichage** (presets UI) via `ScaledLayout` ; certaines zones **redistribuent** la hauteur disponible (colonne Matrix + Patch Manager, bandeaux du `PatchEditPanel`, modules dans `MasterEditPanel` / `PatchManagerPanel`).

---

## 1. Dimensions documentées

### 1.1 Fenêtre et panneaux principaux


| Élément                              | Width | Height | Remarque                                                          |
| ------------------------------------ | ----- | ------ | ----------------------------------------------------------------- |
| `GUI::kWidth` / `kHeight`            | 1312  | 800    | Taille de design de la fenêtre (`PluginEditor` / `MainComponent`) |
| `GUI::kBodyInnerWidth`               | 1288  | —      | Largeur utile body : colonnes + séparateurs verticaux             |
| `Panels::Header::kWidth` / `kHeight` | 1312  | 24     | `HeaderPanel`                                                     |
| `Panels::Body::kWidth` / `kHeight`   | 1312  | 752    | `BodyPanel`                                                       |
| `Panels::Body::kPadding`             | 12    | —      | Marge interne du body (haut / bas / gauche / droite)              |
| `Panels::Body::kEffectiveHeight`     | —     | 720    | `kHeight - 2 * kPadding` : hauteur utile des colonnes centrales   |
| `Panels::Footer::kWidth` / `kHeight` | 1312  | 24     | `FooterPanel`                                                     |


### 1.2 Panneaux — `MasterEditSection`


| Élément                                     | Width | Height | Remarque                                          |
| ------------------------------------------- | ----- | ------ | ------------------------------------------------- |
| `MasterEditSection::kWidth` / `kHeight`     | 160   | 720    | `MasterEditPanel`                                 |
| `MasterEditSection::ChildModules::kWidth`   | 160   | —      | Largeur des sous-panneaux (Midi / Vibrato / Misc) |
| `MasterEditSection::MidiModule::kHeight`    | —     | 232    | `MidiPanel`                                       |
| `MasterEditSection::VibratoModule::kHeight` | —     | 232    | `VibratoPanel`                                    |
| `MasterEditSection::MiscModule::kHeight`    | —     | 232    | `MiscPanel`                                       |


### 1.3 Panneaux — `PatchEditSection`


| Élément                                                             | Width | Height | Remarque                                                                               |
| ------------------------------------------------------------------- | ----- | ------ | -------------------------------------------------------------------------------------- |
| `PatchEditSection::kWidth` / `kHeight`                              | 800   | 720    | `PatchEditPanel`                                                                       |
| `PatchEditSection::kModuleWidth`                                    | 144   | —      | Largeur d’un module « enfant »                                                         |
| `PatchEditSection::kInterModuleGap`                                 | 20    | —      | Espacement horizontal entre modules (`TopPanel` / `BottomPanel`)                     |
| `PatchEditSection::kTopBottomPanelHeight`                           | —     | 284    | Hauteur de design `TopPanel` et `BottomPanel`                                          |
| `PatchEditSection::TopModules::kWidth` / `kHeight`                  | 800   | 284    | Bandeau supérieur                                                                      |
| `PatchEditSection::TopModules::ChildModules::kWidth` / `kHeight`    | 144   | 284    | Chaque module du `TopPanel`                                                            |
| `PatchEditSection::MiddleModules::kWidth` / `kHeight`               | 800   | 128    | `MiddlePanel` (espacement 16 px entre displays)                                          |
| `PatchEditSection::MiddleModules::ChildModules::kWidth` / `kHeight` | 144   | 128    | Displays ; zone nom de patch occupe le reste de la largeur                               |
| `PatchEditSection::BottomModules::kWidth` / `kHeight`               | 800   | 284    | Bandeau inférieur                                                                      |
| `PatchEditSection::BottomModules::ChildModules::kWidth` / `kHeight` | 144   | 284    | Chaque module du `BottomPanel`                                                         |


### 1.4 Panneaux — colonne partagée (Matrix + Patch Manager)


| Élément                                                          | Width | Height | Remarque                                                                           |
| ---------------------------------------------------------------- | ----- | ------ | ---------------------------------------------------------------------------------- |
| `SharedColumn::kWidth`                                           | 264   | —      | Largeur commune des colonnes Matrix / Patch Manager                                |
| `MatrixModulationSection::kWidth` / `kHeight`                    | 264   | 320    | `MatrixModulationPanel`                                                            |
| `PatchManagerSection::kWidth` / `kHeight`                        | 264   | 400    | `PatchManagerPanel`                                                              |
| `PatchManagerSection::BankUtilityModule::kWidth` / `kHeight`     | 264   | 88     | `BankUtilityPanel`                                                               |
| `PatchManagerSection::InternalPatchesModule::kWidth` / `kHeight` | 264   | 84     | `InternalPatchesPanel`                                                             |
| `PatchManagerSection::ComputerPatchesModule::kWidth` / `kHeight` | 264   | 84     | `ComputerPatchesPanel`                                                           |
| `PatchManagerSection::PatchMutatorModule::kWidth` / `kHeight`    | 264   | 120    | `PatchMutatorPanel`                                                              |


---

### 1.5 Widgets — largeurs (`Widgets::Widths`)

Les hauteurs associées, quand elles sont communes à toute la GUI, sont indiquées dans la section **1.6**.


| Élément                                     | Width | Hauteur typique (voir 1.6)  |
| ------------------------------------------- | ----- | --------------------------- |
| `SectionHeader::kMasterEdit`                | 160   | `kSectionHeader` (24)       |
| `SectionHeader::kPatchEdit`                 | 800   | 24                          |
| `SectionHeader::kMatrixModulation`          | 264   | 24                          |
| `SectionHeader::kPatchManager`              | 264   | 24                          |
| `ModuleHeader::kPatchEditModule`            | 144   | `kModuleHeader` (24)        |
| `ModuleHeader::kMasterEditModule`           | 160   | 24                          |
| `ModuleHeader::kPatchManagerModule`         | 264   | 24                          |
| `ModulationBusHeader::kStandard`            | 264   | `kModulationBusHeader` (24) |
| `GroupLabel::kInternalPatchesBrowser`       | 108   | `kGroupLabel` (24)          |
| `GroupLabel::kInternalPatchesMemory`        | 156   | 24                          |
| `GroupLabel::kComputerPatchesBrowser`       | 136   | 24                          |
| `GroupLabel::kComputerPatchesStorage`       | 128   | 24                          |
| `Label::kMasterEditModule`                  | 100   | `kLabel` (20)               |
| `Label::kPatchEditModule`                   | 88    | 20                          |
| `Label::kModulationBusNumber`               | 16    | 20                          |
| `Label::kPatchManagerBankSelector`          | 76    | 20                          |
| `Label::kPatchMutator`                      | 44    | 20                          |
| `Button::kHeaderPanelTheme`                 | 68    | `kButton` (20)              |
| `Button::kInit`                             | 20    | 20                          |
| `Button::kCopy`                             | 20    | 20                          |
| `Button::kPaste`                            | 20    | 20                          |
| `Button::kPatchManagerBankSelect`           | 36    | 20                          |
| `Button::kPatchManagerUnlockBank`           | 76    | 20                          |
| `Button::kInternalPatchesMemory`            | 36    | 20                          |
| `Button::kComputerPatchesStorage`           | 36    | 20                          |
| `Button::kComputerPatchesSaveAs`            | 48    | 20                          |
| `Button::kPatchMutatorMutate`               | 48    | 20                          |
| `Button::kPatchMutatorCompare`              | 48    | 20                          |
| `Button::kPatchMutatorDelete`               | 20    | 20                          |
| `Button::kPatchMutatorClear`                | 44    | 20                          |
| `Button::kPatchMutatorExport`               | 44    | 20                          |
| `Toggle::kPatchMutator`                     | 20    | `kToggle` (20)              |
| `Slider::kStandard`                         | 60    | `kSlider` (20)              |
| `Slider::kPatchMutator`                     | 44    | 20                          |
| `ComboBox::kMasterEditModule`               | 60    | `kComboBox` (20)            |
| `ComboBox::kPatchEditModule`                | 60    | 20                          |
| `ComboBox::kMatrixModulationSource`         | 60    | 20                          |
| `ComboBox::kMatrixModulationDestination`    | 104   | 20                          |
| `ComboBox::kPatchManagerComputerPatches`    | 84    | 20                          |
| `ComboBox::kPatchMutatorHistory`            | 44    | 20                          |
| `NumberBox::kPatchManagerBankNumber`        | 24    | `kNumberBox` (20)           |
| `NumberBox::kPatchManagerPatchNumber`       | 28    | 20                          |
| `HorizontalSeparator::kMasterEditModule`    | 160   | `kHorizontalSeparator` (4)    |
| `HorizontalSeparator::kPatchEditModule`     | 144   | 4                           |
| `HorizontalSeparator::kMatrixModulationBus` | 264   | 4                           |
| `VerticalSeparator::kStandard`              | 32    | `kVerticalSeparator` (720)  |


### 1.6 Widgets — hauteurs partagées (`Widgets::Heights`)


| Constante              | Height | Usage                                                                |
| ---------------------- | ------ | -------------------------------------------------------------------- |
| `kSectionHeader`       | 24     | En-têtes de section                                                  |
| `kModuleHeader`        | 24     | En-têtes de module (`ModuleHeader`)                                  |
| `kModulationBusHeader` | 24     | Ligne de titres des bus Matrix                                       |
| `kGroupLabel`          | 24     | Libellés de groupe (Patch Manager)                                   |
| `kLabel`               | 20     | Labels standard                                                      |
| `kButton`              | 20     | Boutons                                                              |
| `kToggle`              | 20     | Cases à cocher                                                       |
| `kSlider`              | 20     | Sliders (hauteur de la zone widget)                                  |
| `kComboBox`            | 20     | ComboBox                                                             |
| `kNumberBox`           | 20     | NumberBox                                                            |
| `kPatchNameDisplay`    | 72     | Affichage du nom de patch (`MiddlePanel`)                            |
| `kHorizontalSeparator` | 4      | Séparateurs horizontaux                                              |
| `kVerticalSeparator`   | 720    | Hauteur des `VerticalSeparator` du body (= `Body::kEffectiveHeight`) |


---

## 2. Hiérarchie de la GUI (composants JUCE)

L’éditeur audio instancie `PluginEditor`, qui contient `MainComponent`. Les dimensions entre crochets sont les **tailles de design** issues de `PluginDimensions` (avant échelle utilisateur), sauf indication.

```
PluginEditor [1312×800]
└── MainComponent [1312×800]
    ├── HeaderPanel [1312×24]
    │   ├── Label (skin)
    │   ├── ComboBox (skin)
    │   ├── Label (échelle GUI)
    │   └── ComboBox (échelle GUI)
    │
    ├── BodyPanel [1312×752]
    │   │   (zone utile intérieure : 1288×720 avec padding 12)
    │   │
    │   ├── PatchEditPanel [800×720]
    │   │   ├── SectionHeader [800×24]
    │   │   ├── TopPanel [800×284]   ← hauteurs redistribuées avec le reste du panel
    │   │   │   ├── Dco1Panel [144×284]
    │   │   │   ├── (espacement 20, `kInterModuleGap`)
    │   │   │   ├── Dco2Panel [144×284]
    │   │   │   ├── (espacement 20)
    │   │   │   ├── VcfVcaPanel [144×284]
    │   │   │   ├── (espacement 20)
    │   │   │   ├── FmTrackPanel [144×284]
    │   │   │   ├── (espacement 20)
    │   │   │   └── RampPortamentoPanel [largeur restante×284]
    │   │   ├── MiddlePanel [800×128]
    │   │   │   ├── EnvelopeDisplay [144×128]
    │   │   │   ├── EnvelopeDisplay [144×128]
    │   │   │   ├── EnvelopeDisplay [144×128]
    │   │   │   ├── TrackGeneratorDisplay [144×128]
    │   │   │   ├── ModuleHeader (patch name) [~144×24]
    │   │   │   └── PatchNameDisplay [~144×72]
    │   │   └── BottomPanel [800×284]
    │   │       ├── Env1Panel [144×284]
    │   │       ├── (espacement 20)
    │   │       ├── Env2Panel [144×284]
    │   │       ├── (espacement 20)
    │   │       ├── Env3Panel [144×284]
    │   │       ├── (espacement 20)
    │   │       ├── Lfo1Panel [144×284]
    │   │       ├── (espacement 20)
    │   │       └── Lfo2Panel [largeur restante×284]
    │   │
    │   ├── VerticalSeparator [32×720]
    │   │
    │   ├── MatrixModulationPanel [264×320]
    │   │   ├── SectionHeader [264×24]
    │   │   ├── Button « init all busses » [20×20], sous le `SectionHeader`, aligné à droite (même bande verticale que le `ModulationBusHeader`)
    │   │   ├── ModulationBusHeader [264×24]
    │   │   └── ModulationBusPanel × 10 (hauteur de ligne de design 24 px : label + séparateur)
    │   │       ├── Label (numéro de bus)
    │   │       ├── ComboBox (source)
    │   │       ├── Slider (amount)
    │   │       ├── ComboBox (destination)
    │   │       ├── Button (init)
    │   │       └── HorizontalSeparator
    │   │
    │   ├── PatchManagerPanel [264×400]
    │   │   ├── SectionHeader [264×24]
    │   │   ├── BankUtilityPanel [264×88]
    │   │   ├── InternalPatchesPanel [264×84]
    │   │   ├── ComputerPatchesPanel [264×84]
    │   │   └── PatchMutatorPanel [264×120]
    │   │       (composition propre à chaque panneau ; pas de `BaseModulePanel`)
    │   │
    │   ├── VerticalSeparator [32×720]
    │   │
    │   └── MasterEditPanel [160×720]
    │       ├── SectionHeader [160×24]
    │       ├── MidiPanel [160×232]
    │       ├── VibratoPanel [160×232]
    │       └── MiscPanel [160×232]
    │
    └── FooterPanel [1312×24]
        (message d’état / APVTS — pas de sous-composants dédiés dans le header du fichier)
```

### `ParameterPanel` — assemblage des widgets

Chaque `ParameterPanel` « actif » occupe la **pleine largeur** du module parent (colonne Patch Edit ou Master Edit). La mise en page est en **deux bandes verticales** :

1. **Bande supérieure (une seule ligne horizontale)** — le `**Label`** (libellé du paramètre, à gauche) et le contrôle principal sont **côte à côte sur la même rangée** : le `Label` est ancré au bord gauche du panneau ; le `**Slider`** ou la `**ComboBox**` est placé **immédiatement à droite** du label, avec une abscisse de départ égale à la largeur réservée au label (`Label::kPatchEditModule` ou `Label::kMasterEditModule` selon le type de module). Les deux widgets partagent la même ordonnée de base (`y = 0` dans le repère local du `ParameterPanel`) ; la hauteur de cette bande est le maximum entre la hauteur du label et celle du contrôle (`kLabel` vs `kSlider` / `kComboBox`), pour garder l’alignement visuel.
2. **Bande inférieure** — un `**HorizontalSeparator`** occupe toute la **largeur utile** du module (`HorizontalSeparator::kPatchEditModule` ou `kMasterEditModule`). Il est placé **directement sous** le duo label + slider/combo : l’ordonnée du séparateur suit la hauteur effective de la rangée label/contrôle (`max(kLabel, kSlider ou kComboBox)`), de sorte que le trait horizontal se situe **immédiatement sous** cette ligne. Si la cellule allouée au `ParameterPanel` est plus haute que cette rangée + `kHorizontalSeparator`, l’espace résiduel se trouve **sous** le séparateur (vers le bas de la cellule).

En résumé visuel pour une ligne de paramètre :

```
┌──────────────────────────────────────────┐
│  Label              │ Slider ou ComboBox │  ← même ligne (côte à côte)
├──────────────────────────────────────────┤
│  HorizontalSeparator (pleine largeur)    │  ← dessous
└──────────────────────────────────────────┘
```

Cas particulier `**ParameterType::None**` (ex. dernière entrée du `MiscPanel`) : pas de label ni de contrôle ; seul un `HorizontalSeparator` est créé, positionné dans la zone de ligne (souvent vers le bas de la hauteur allouée).

Les hauteurs de design de la « ligne compacte » label + séparateur sont `kLabel + kHorizontalSeparator` ; le `BaseModulePanel` peut allouer plus de hauteur par ligne pour étirer le module — l’espace supplémentaire s’accumule alors **sous** le séparateur, dans le bas de la cellule.

### Détail d’un module Patch Edit — `Dco1Panel`

`Dco1Panel` hérite de `BaseModulePanel` : la configuration des paramètres est déclarée dans `Dco1Panel.cpp` (`ModulePanelConfig`), puis matérialisée au runtime par `WidgetFactory` et l’APVTS. En tête de colonne, un `ModuleHeaderPanel` affiche le titre du module (`ModuleHeader`) et la rangée d’actions **Init**, **Copy** et **Paste** (jeu de boutons `InitCopyPaste`, propre au Patch Edit). En dessous, le corps du module est une pile verticale de **dix** `ParameterPanel`, chacun structuré comme décrit ci-dessus (duo label + contrôle, puis séparateur). La hauteur de ligne de design suit `kLabel + kHorizontalSeparator` ; le surplus de hauteur du module est absorbé en bas de la pile. L’ordre des paramètres DCO1 est : fréquence, modulation de fréquence par LFO1, sync, largeur d’impulsion, modulation PWM par LFO2, forme d’onde (slider), sélection de forme d’onde (combo), leviers, portamento clavier, key click.

Schéma condensé :

```
Dco1Panel (BaseModulePanel)
├── ModuleHeaderPanel
│   ├── ModuleHeader
│   ├── Button (Init)
│   ├── Button (Copy)
│   └── Button (Paste)
└── ParameterPanel × 10
    └── (chaque) [ Label | Slider ou ComboBox ]  (ligne 1)
                 [ HorizontalSeparator        ]  (ligne 2, dessous)
```

Les autres modules du Patch Edit construits sur `BaseModulePanel` (`Dco2Panel`, `VcfVcaPanel`, enveloppes, LFO, etc.) suivent le même gabarit ; seuls le jeu de boutons du header (`InitCopyPaste` vs `InitOnly` — ce dernier pour le Master Edit) et la liste des `ParameterPanel` changent.

### Détail des trois modules Master Edit

Les trois colonnes empilées sous le `SectionHeader` du `MasterEditPanel` sont encore des `BaseModulePanel`, mais en type **Master Edit** : en-tête avec **Init** seul, largeurs de label / combo / séparateur issues des constantes `kMasterEditModule`, et hauteurs de design distinctes par bloc (`MidiModule`, `VibratoModule`, `MiscModule`).

- `**MidiPanel`** : huit rangées de paramètres — canal MIDI, MIDI echo, contrôleurs, patch changes (combo), puis quatre sélections au slider (pédale 1, pédale 2, levier 2, levier 3).
- `**VibratoPanel**` : sept rangées — vitesse (slider), forme d’onde (combo), amplitude (slider), source de modulation de vitesse (combo), montant de modulation de vitesse (slider), source de modulation d’amplitude (combo), montant de modulation d’amplitude (slider).
- `**MiscPanel**` : sept paramètres (accord master, transpose, bend range, unison, inversion de volume, verrou de banque, protection mémoire — mélange sliders et combos selon la config), puis une entrée de configuration de type `None` qui ne crée qu’un **séparateur** (`ParameterPanel` sans label ni contrôle, utile pour l’espacement visuel en bas du module).

### Notes de lecture

- **Autres modules `BaseModulePanel`** (autres tranches du Patch Edit, modules Master Edit listés ci-dessus, etc.) : même ossature que l’exemple DCO1 ; le contenu exact des lignes reste défini par chaque `createConfig()` et les descripteurs, pas par `PluginDimensions.h`. Les panneaux du Patch Manager composent leurs propres sous-widgets sans passer par `BaseModulePanel`.
- **Espacement 20** entre modules Patch Edit sur une même rangée : `Panels::Body::PatchEditSection::kInterModuleGap` (distinct du padding du body).
- **Colonne 264 px** : `MatrixModulationPanel` et `PatchManagerPanel` partagent l’axe X ; les hauteurs réelles suivent `distributeHeights` sur la hauteur utile du body (320 + 400 = 720 au design).
- **Nombre de bus Matrix** : 10 (`Matrix1000Limits::kModulationBusCount`).

---

*Pour mettre à jour ce document : aligner les tableaux et l’arbre sur `PluginDimensions.h` et sur les fichiers `*Panel.cpp` concernés.*