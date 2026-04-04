# GUI — Dimensions (`PluginDimensions.h`) et hiérarchie des composants

**Source des dimensions :** `Source/Shared/Definitions/PluginDimensions.h`  
**Date de référence :** 2026-04-03  

Les valeurs ci-dessous sont les **tailles de design** (échelle 1:1). L’éditeur applique un facteur d’échelle via `ScaledLayout` ; certaines zones **redistribuent** la hauteur disponible (barre de statut, colonne Matrix + Patch Manager, bandeaux du `PatchEditPanel`, modules dans `MasterEditPanel` / `PatchManagerPanel`).

---

## 1. Dimensions documentées

### 1.1 Fenêtre et panneaux principaux


| Élément                              | Width | Height | Remarque                                                          |
| ------------------------------------ | ----- | ------ | ----------------------------------------------------------------- |
| `GUI::kWidth` / `kHeight`            | 1335  | 810    | Taille de design de la fenêtre (`PluginEditor` / `MainComponent`) |
| `Panels::Header::kWidth` / `kHeight` | 1335  | 30     | `HeaderPanel`                                                     |
| `Panels::Body::kWidth` / `kHeight`   | 1335  | 750    | `BodyPanel`                                                       |
| `Panels::Body::kPadding`             | 15    | —      | Marge interne du body (haut / bas / gauche / droite)              |
| `Panels::Body::kEffectiveHeight`     | —     | 720    | `kHeight - 2 * kPadding` : hauteur utile des colonnes centrales   |
| `Panels::Footer::kWidth` / `kHeight` | 1335  | 30     | `FooterPanel`                                                     |


### 1.2 Panneaux — `MasterEditSection`


| Élément                                     | Width | Height | Remarque                                          |
| ------------------------------------------- | ----- | ------ | ------------------------------------------------- |
| `MasterEditSection::kWidth` / `kHeight`     | 160   | 720    | `MasterEditPanel`                                 |
| `MasterEditSection::ChildModules::kWidth`   | 160   | —      | Largeur des sous-panneaux (Midi / Vibrato / Misc) |
| `MasterEditSection::MidiModule::kHeight`    | —     | 242    | Hauteur de design pour `MidiPanel`                |
| `MasterEditSection::VibratoModule::kHeight` | —     | 218    | `VibratoPanel`                                    |
| `MasterEditSection::MiscModule::kHeight`    | —     | 230    | `MiscPanel`                                       |


### 1.3 Panneaux — `PatchEditSection`


| Élément                                                             | Width | Height | Remarque                                                                               |
| ------------------------------------------------------------------- | ----- | ------ | -------------------------------------------------------------------------------------- |
| `PatchEditSection::kWidth` / `kHeight`                              | 810   | 720    | `PatchEditPanel`                                                                       |
| `PatchEditSection::kModuleWidth`                                    | 150   | —      | Largeur d’un module « enfant » (rangées haut / bas / milieu)                           |
| `PatchEditSection::kTopBottomPanelHeight`                           | —     | 280    | Hauteur de design `TopPanel` et `BottomPanel`                                          |
| `PatchEditSection::TopModules::kWidth` / `kHeight`                  | 810   | 280    | Bandeau supérieur                                                                      |
| `PatchEditSection::TopModules::ChildModules::kWidth` / `kHeight`    | 150   | 280    | Chaque module du `TopPanel`                                                            |
| `PatchEditSection::MiddleModules::kWidth` / `kHeight`               | 810   | 130    | `MiddlePanel`                                                                          |
| `PatchEditSection::MiddleModules::ChildModules::kWidth` / `kHeight` | 150   | 130    | Displays 150×130 ; zone nom de patch occupe le reste de la largeur (~150 px au design) |
| `PatchEditSection::BottomModules::kWidth` / `kHeight`               | 810   | 280    | Bandeau inférieur                                                                      |
| `PatchEditSection::BottomModules::ChildModules::kWidth` / `kHeight` | 150   | 280    | Chaque module du `BottomPanel`                                                         |


### 1.4 Panneaux — colonne partagée (Matrix + Patch Manager)


| Élément                                                          | Width | Height | Remarque                                                                           |
| ---------------------------------------------------------------- | ----- | ------ | ---------------------------------------------------------------------------------- |
| `SharedColumn::kWidth`                                           | 275   | —      | Largeur commune des colonnes Matrix / Patch Manager                                |
| `MatrixModulationSection::kWidth` / `kHeight`                    | 275   | 320    | `MatrixModulationPanel` (hauteur de design ; le layout peut étirer selon l’espace) |
| `PatchManagerSection::kWidth` / `kHeight`                        | 275   | 400    | `PatchManagerPanel`                                                                |
| `PatchManagerSection::BankUtilityModule::kWidth` / `kHeight`     | 275   | 90     | `BankUtilityPanel`                                                                 |
| `PatchManagerSection::InternalPatchesModule::kWidth` / `kHeight` | 275   | 85     | `InternalPatchesPanel`                                                             |
| `PatchManagerSection::ComputerPatchesModule::kWidth` / `kHeight` | 275   | 85     | `ComputerPatchesPanel`                                                             |
| `PatchManagerSection::PatchMutatorModule::kWidth` / `kHeight`    | 275   | 105    | `PatchMutatorPanel`                                                                |


---

### 1.5 Widgets — largeurs (`Widgets::Widths`)

Les hauteurs associées, quand elles sont communes à toute la GUI, sont indiquées dans la section **1.6**.


| Élément                                     | Width | Hauteur typique (voir 1.6)  |
| ------------------------------------------- | ----- | --------------------------- |
| `SectionHeader::kMasterEdit`                | 160   | `kSectionHeader` (30)       |
| `SectionHeader::kPatchEdit`                 | 810   | 30                          |
| `SectionHeader::kMatrixModulation`          | 275   | 30                          |
| `SectionHeader::kPatchManager`              | 275   | 30                          |
| `ModuleHeader::kPatchEditModule`            | 150   | `kModuleHeader` (30)        |
| `ModuleHeader::kMasterEditModule`           | 160   | 30                          |
| `ModuleHeader::kPatchManagerModule`         | 275   | 30                          |
| `ModulationBusHeader::kStandard`            | 275   | `kModulationBusHeader` (30) |
| `GroupLabel::kInternalPatchesBrowser`       | 110   | `kGroupLabel` (25)          |
| `GroupLabel::kInternalPatchesMemory`        | 155   | 25                          |
| `GroupLabel::kComputerPatchesBrowser`       | 135   | 25                          |
| `GroupLabel::kComputerPatchesStorage`       | 130   | 25                          |
| `Label::kMasterEditModule`                  | 100   | `kLabel` (20)               |
| `Label::kPatchEditModule`                   | 90    | 20                          |
| `Label::kModulationBusNumber`               | 15    | 20                          |
| `Label::kPatchManagerBankSelector`          | 75    | 20                          |
| `Label::kPatchMutator`                      | 45    | 20                          |
| `Button::kHeaderPanelTheme`                 | 70    | `kButton` (20)              |
| `Button::kInit`                             | 20    | 20                          |
| `Button::kCopy`                             | 20    | 20                          |
| `Button::kPaste`                            | 20    | 20                          |
| `Button::kPatchManagerBankSelect`           | 35    | 20                          |
| `Button::kPatchManagerUnlockBank`           | 75    | 20                          |
| `Button::kInternalPatchesMemory`            | 35    | 20                          |
| `Button::kComputerPatchesStorage`           | 35    | 20                          |
| `Button::kComputerPatchesSaveAs`            | 50    | 20                          |
| `Button::kPatchMutatorMutate`               | 50    | 20                          |
| `Button::kPatchMutatorCompare`              | 50    | 20                          |
| `Button::kPatchMutatorDelete`               | 20    | 20                          |
| `Button::kPatchMutatorClear`                | 45    | 20                          |
| `Button::kPatchMutatorExport`               | 45    | 20                          |
| `Toggle::kPatchMutator`                     | 20    | `kToggle` (20)              |
| `Slider::kStandard`                         | 60    | `kSlider` (20)              |
| `Slider::kPatchMutator`                     | 45    | 20                          |
| `ComboBox::kMasterEditModule`               | 60    | `kComboBox` (20)            |
| `ComboBox::kPatchEditModule`                | 60    | 20                          |
| `ComboBox::kMatrixModulationSource`         | 60    | 20                          |
| `ComboBox::kMatrixModulationDestination`    | 105   | 20                          |
| `ComboBox::kPatchManagerComputerPatches`    | 85    | 20                          |
| `ComboBox::kPatchMutatorHistory`            | 45    | 20                          |
| `NumberBox::kPatchManagerBankNumber`        | 25    | `kNumberBox` (20)           |
| `NumberBox::kPatchManagerPatchNumber`       | 30    | 20                          |
| `HorizontalSeparator::kMasterEditModule`    | 160   | `kHorizontalSeparator` (5)  |
| `HorizontalSeparator::kPatchEditModule`     | 150   | 5                           |
| `HorizontalSeparator::kMatrixModulationBus` | 275   | 5                           |
| `VerticalSeparator::kStandard`              | 30    | `kVerticalSeparator` (720)  |


### 1.6 Widgets — hauteurs partagées (`Widgets::Heights`)


| Constante              | Height | Usage                                                                |
| ---------------------- | ------ | -------------------------------------------------------------------- |
| `kSectionHeader`       | 30     | En-têtes de section                                                  |
| `kModuleHeader`        | 30     | En-têtes de module (`ModuleHeader`)                                  |
| `kModulationBusHeader` | 30     | Ligne de titres des bus Matrix                                       |
| `kGroupLabel`          | 25     | Libellés de groupe (Patch Manager)                                   |
| `kLabel`               | 20     | Labels standard                                                      |
| `kButton`              | 20     | Boutons                                                              |
| `kToggle`              | 20     | Cases à cocher                                                       |
| `kSlider`              | 20     | Sliders (hauteur de la zone widget)                                  |
| `kComboBox`            | 20     | ComboBox                                                             |
| `kNumberBox`           | 20     | NumberBox                                                            |
| `kPatchNameDisplay`    | 70     | Affichage du nom de patch (`MiddlePanel`)                            |
| `kHorizontalSeparator` | 5      | Séparateurs horizontaux                                              |
| `kVerticalSeparator`   | 720    | Hauteur des `VerticalSeparator` du body (= `Body::kEffectiveHeight`) |


---

## 2. Hiérarchie de la GUI (composants JUCE)

L’éditeur audio instancie `PluginEditor`, qui contient `MainComponent`. Les dimensions entre crochets sont les **tailles de design** issues de `PluginDimensions` (avant échelle utilisateur), sauf indication.

```
PluginEditor [1335×810]
└── MainComponent [1335×810]
    ├── HeaderPanel [1335×30]
    │   ├── Label (skin)
    │   ├── ComboBox (skin)
    │   ├── Label (échelle GUI)
    │   └── ComboBox (échelle GUI)
    │
    ├── BodyPanel [1335×750]
    │   │   (zone utile intérieure : 1305×720 avec padding 15)
    │   │
    │   ├── PatchEditPanel [810×720]
    │   │   ├── SectionHeader [810×30]
    │   │   ├── TopPanel [810×280]   ← hauteurs redistribuées avec le reste du panel
    │   │   │   ├── Dco1Panel [150×280]
    │   │   │   ├── (espacement 15)
    │   │   │   ├── Dco2Panel [150×280]
    │   │   │   ├── (espacement 15)
    │   │   │   ├── VcfVcaPanel [150×280]
    │   │   │   ├── (espacement 15)
    │   │   │   ├── FmTrackPanel [150×280]
    │   │   │   ├── (espacement 15)
    │   │   │   └── RampPortamentoPanel [largeur restante×280]
    │   │   ├── MiddlePanel [810×130]
    │   │   │   ├── EnvelopeDisplay [150×130]
    │   │   │   ├── EnvelopeDisplay [150×130]
    │   │   │   ├── EnvelopeDisplay [150×130]
    │   │   │   ├── TrackGeneratorDisplay [150×130]
    │   │   │   ├── ModuleHeader (patch name) [~150×30]
    │   │   │   └── PatchNameDisplay [~150×70]
    │   │   └── BottomPanel [810×280]
    │   │       ├── Env1Panel [150×280]
    │   │       ├── (espacement 15)
    │   │       ├── Env2Panel [150×280]
    │   │       ├── (espacement 15)
    │   │       ├── Env3Panel [150×280]
    │   │       ├── (espacement 15)
    │   │       ├── Lfo1Panel [150×280]
    │   │       ├── (espacement 15)
    │   │       └── Lfo2Panel [largeur restante×280]
    │   │
    │   ├── VerticalSeparator [30×720]
    │   │
    │   ├── MatrixModulationPanel [275×320]   ← hauteur de design ; partage la colonne avec Patch Manager
    │   │   ├── SectionHeader [275×30]
    │   │   ├── Button « init all busses » [20×20], sous le `SectionHeader`, aligné à droite (même bande verticale que le `ModulationBusHeader`)
    │   │   ├── ModulationBusHeader [275×30]
    │   │   └── ModulationBusPanel × 10 (hauteur de ligne de design 25 px par bus)
    │   │       ├── Label (numéro de bus)
    │   │       ├── ComboBox (source)
    │   │       ├── Slider (amount)
    │   │       ├── ComboBox (destination)
    │   │       ├── Button (init)
    │   │       └── HorizontalSeparator
    │   │
    │   ├── PatchManagerPanel [275×400]   ← empilé sous Matrix dans la même colonne (X identique)
    │   │   ├── SectionHeader [275×30]
    │   │   ├── BankUtilityPanel [275×90]
    │   │   ├── InternalPatchesPanel [275×85]
    │   │   ├── ComputerPatchesPanel [275×85]
    │   │   └── PatchMutatorPanel [275×105]
    │   │       (composition propre à chaque panneau ; pas de `BaseModulePanel`)
    │   │
    │   ├── VerticalSeparator [30×720]
    │   │
    │   └── MasterEditPanel [160×720]
    │       ├── SectionHeader [160×30]
    │       ├── MidiPanel [160×242]
    │       ├── VibratoPanel [160×218]
    │       └── MiscPanel [160×230]
    │
    └── FooterPanel [1335×30]
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
- **Espacement 15** entre modules sur une même rangée : `Panels::Body::kPadding` (même valeur que le padding du body), pas un widget `VerticalSeparator`.
- **Colonne 275 px** : `MatrixModulationPanel` et `PatchManagerPanel` partagent l’axe X ; les hauteurs réelles suivent `distributeHeights` sur la hauteur utile du body (au design, 320 + 400 = 720 pour la colonne).
- **Nombre de bus Matrix** : 10 (`Matrix1000Limits::kModulationBusCount`).

---

*Pour mettre à jour ce document : aligner les tableaux et l’arbre sur `PluginDimensions.h` et sur les fichiers `*Panel.cpp` concernés.*