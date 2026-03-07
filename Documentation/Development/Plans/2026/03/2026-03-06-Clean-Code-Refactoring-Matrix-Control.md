---
name: Clean Code Refactoring
overview: "Refactorisation complète de la codebase Matrix-Control selon les principes Uncle Bob : corrections de magic numbers, extraction de sous-méthodes, découpage de fichiers monolithiques, création de sous-interfaces, et application de std::optional."
todos:
  - id: phase1-quick-fixes
    content: "Phase 1 : ApvtsTypes (#11), kThreadStopTimeoutMs (#5), Zoom mapping (#4)"
    status: completed
  - id: phase2-skin
    content: "Phase 2a : Extraire 8 sous-methodes de Skin::initializeVariantColours (#3)"
    status: completed
  - id: phase2-patch-mutator
    content: "Phase 2b : PatchMutator descriptors (#8) + layoutSliderLine (#7)"
    status: completed
  - id: phase2-widget-factory
    content: "Phase 2c : Factoriser duplication WidgetFactory (#6)"
    status: completed
  - id: phase3-optional
    content: "Phase 3a : Appliquer std::optional (#10)"
    status: completed
  - id: phase3-iskin
    content: "Phase 3b : Creer sous-interfaces ISkin (#9)"
    status: completed
  - id: phase4-descriptors-split
    content: "Phase 4a : Decouper PluginDescriptors.cpp en 5 fichiers (#1)"
    status: completed
  - id: phase4-apvts-split
    content: "Phase 4b : Separer ApvtsFactory en Builder + Validator (#2)"
    status: completed
  - id: cmake-build
    content: Mise a jour CMakeLists.txt + build final
    status: completed
isProject: false
---

# Refactorisation Clean Code - Matrix-Control

## Phase 1 - Corrections rapides (indépendantes)

### 1. Corriger kValue dans ApvtsTypes.h

- Fichier : [Source/Shared/Definitions/ApvtsTypes.h](Source/Shared/Definitions/ApvtsTypes.h)
- Changer `"VALUE"` en `"value"` (JUCE Identifier est case-sensitive)
- Ajouter un commentaire explicatif exceptionnel (justifié par la subtilité)
- Également dans [Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/MiddlePanel/MiddlePanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/MiddlePanel/MiddlePanel.cpp) (ligne 72) : remplacer `juce::Identifier("value")` par `juce::Identifier(ApvtsTypes::kValue)` et ajouter l'include

### 2. Constante kThreadStopTimeoutMs

- Fichier : [Source/Core/PluginProcessor.h](Source/Core/PluginProcessor.h)
- Ajouter `static constexpr int kThreadStopTimeoutMs_ {5000};` dans la section private
- Fichier : [Source/Core/PluginProcessor.cpp](Source/Core/PluginProcessor.cpp) (ligne 133)
- Remplacer `midiManager->stopThread(5000)` par `midiManager->stopThread(kThreadStopTimeoutMs_)`
- Supprimer le commentaire `// Wait up to 5 seconds` devenu inutile

### 3. Zoom - Eliminer les magic numbers

- Fichier : [Source/Shared/Definitions/PluginIDs.h](Source/Shared/Definitions/PluginIDs.h) (lignes 16-25)
- Enrichir `PluginIDs::Settings::ZoomLevels` :
  - Ajouter les niveaux manquants : `k250 = 8`, `k300 = 9`, `k400 = 10`
  - Ajouter les constantes : `kDefault = k100`, `kMin = k50`, `kMax = k400`
  - Ajouter un tableau `constexpr float kFactors[]` mappant chaque ID vers son facteur (0.5f, 0.75f, 0.9f, 1.0f, 1.25f, 1.5f, 2.0f, 2.5f, 3.0f, 4.0f)
  - Ajouter une fonction `constexpr float getZoomLevel(int zoomLevelId)` pour la conversion (nom coherent avec la terminologie existante)
- Fichier : [Source/GUI/PluginEditor.cpp](Source/GUI/PluginEditor.cpp) (lignes 48-68)
- Remplacer le switch/case par un appel a `PluginIDs::Settings::ZoomLevels::getZoomLevel(selectedId)`

---

## Phase 2 - Refactorings moderes

### 4. Skin::initializeVariantColours() - Extraction de sous-methodes

- Fichier : [Source/GUI/Skins/Skin.h](Source/GUI/Skins/Skin.h)
- Ajouter 8 declarations de methodes template privees
- Fichier : [Source/GUI/Skins/Skin.cpp](Source/GUI/Skins/Skin.cpp) (lignes 183-284)
- Eclater `initializeVariantColours()` en 8 sous-methodes template :
  - `initializePanelColours(Accessor)` : lignes 188-191 (4 couleurs)
  - `initializeLayoutColours(Accessor)` : lignes 193-207 (15 couleurs - headers, labels, separators)
  - `initializeButtonColours(Accessor)` : lignes 209-218 (10 couleurs)
  - `initializeToggleColours(Accessor)` : lignes 220-224 (5 couleurs)
  - `initializeSliderColours(Accessor)` : lignes 226-232 (7 couleurs)
  - `initializeComboBoxColours(Accessor)` : lignes 234-250 (18 couleurs)
  - `initializePopupMenuColours(Accessor)` : lignes 252-265 (14 couleurs)
  - `initializeDisplayColours(Accessor)` : lignes 267-283 (14 couleurs - NumberBox + Displays)
- `initializeVariantColours()` delegue a ces 8 methodes

### 5. PatchMutator - Ajouter les Int Parameters dans les descriptors

- Fichier : [Source/Shared/Definitions/PluginDescriptors.h](Source/Shared/Definitions/PluginDescriptors.h) (lignes 235-238)
- Ajouter dans `PatchManagerSection::PatchMutatorModule` : `extern const std::vector<IntParameterDescriptor> kIntParameters;`
- Fichier : [Source/Shared/Definitions/PluginDescriptors.cpp](Source/Shared/Definitions/PluginDescriptors.cpp) (apres ligne 2878)
- Ajouter `kIntParameters` avec 2 entries :
  - Amount : parameterId=kAmount, min=0, max=100, default=0, sysExId=kNoSysExId
  - Random : parameterId=kRandom, min=0, max=100, default=0, sysExId=kNoSysExId
- Fichier : [Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp) (lignes 51-53, 116-118)
- Remplacer les magic numbers `setRange(0.0, 100.0, 1.0)` par les valeurs issues des descriptors

### 6. PatchMutatorPanel - Extraire layoutSliderLine()

- Fichier : [Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/PatchMutatorPanel.h](Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/PatchMutatorPanel.h)
- Ajouter la declaration : `void layoutSliderLine(int x, int& y, tss::Label* label, tss::Slider* slider, tss::Button* button, const std::vector<tss::Toggle*>& toggles);`
- Fichier : PatchMutatorPanel.cpp
- Creer `layoutSliderLine()` qui factorise le pattern commun (label + slider + button + N toggles)
- `layoutAmountLine()` et `layoutRandomLine()` deviennent des one-liners delegant a `layoutSliderLine()` avec les bons widgets

### 7. WidgetFactory - Factoriser la duplication

- Fichier : [Source/GUI/Factories/WidgetFactory.h](Source/GUI/Factories/WidgetFactory.h)
- Supprimer les 9 methodes dupliquees : `addPatchEditModuleIntParametersToMap`, `addPatchEditModuleChoiceParametersToMap`, `addPatchEditStandaloneWidgetsToMap` (+ 6 appelantes MasterEdit/MatrixModulation/PatchManager)
- Les remplacer par 3 methodes consolidees : `addAllPatchEditDescriptorsToMap()`, `addAllMatrixModulationDescriptorsToMap()`, `addAllPatchManagerDescriptorsToMap()`
- Fichier : [Source/GUI/Factories/WidgetFactory.cpp](Source/GUI/Factories/WidgetFactory.cpp) (lignes 191-285)
- Approche : creer un tableau de pointeurs vers les 10 module-vectors PatchEdit, iterer avec une boucle
- Pattern :

```cpp
void WidgetFactory::addAllPatchEditDescriptorsToMap()
{
    const std::vector<const std::vector<PluginDescriptors::IntParameterDescriptor>*> intParamSets = {
        &PluginDescriptors::PatchEditSection::Dco1Module::kIntParameters,
        &PluginDescriptors::PatchEditSection::Dco2Module::kIntParameters,
        // ... 10 modules
    };
    for (const auto* params : intParamSets)
        addIntParametersToMap(*params);
    // Idem pour Choice et Standalone
}
```

---

## Phase 3 - Refactorings structurels

### 8. std::optional - Application ciblee

Appliquer `std::optional` sur les methodes publiques dont le retour vide signale "pas trouve" :

- [Source/GUI/Factories/WidgetFactory.h](Source/GUI/Factories/WidgetFactory.h) / .cpp :
  - `getParameterDisplayName()` : `juce::String` -> `std::optional<juce::String>`
  - `getStandaloneWidgetDisplayName()` : idem
- [Source/Core/PluginProcessor.h](Source/Core/PluginProcessor.h) / .cpp :
  - `getChoiceLabel()` : `juce::String` -> `std::optional<juce::String>`

Les methodes internes `findX()` retournant des raw pointers non-owning restent inchangees (idiome C++ conforme aux regles).

Mettre a jour les sites d'appel pour utiliser `value_or("")` ou le pattern `if (auto label = getChoiceLabel(...))`.

### 9. ISkin - Sous-interfaces

Creer 3 fichiers dans [Source/GUI/Skins/](Source/GUI/Skins/) :

- **ISkinColours.h** : `getColour`, `setColour` + les 19 methodes helper (getToggle*, getSlider*, getComboBox*, getPopupMenu*)
- **ISkinValues.h** : `getValue`, `setValue`
- **ISkinFonts.h** : `getBaseFont`, `getBaseFontBold`

Modifier [Source/GUI/Skins/ISkin.h](Source/GUI/Skins/ISkin.h) :

- `ISkin` herite de `ISkinColours`, `ISkinValues`, `ISkinFonts`
- Retro-compatible : tout le code existant utilisant `ISkin&` continue de fonctionner

Modifier [Source/GUI/Skins/Skin.h](Source/GUI/Skins/Skin.h) :

- `Skin` herite toujours de `ISkin` (pas de changement, les sous-interfaces sont heritees transitivement)

---

## Phase 4 - Decoupages de fichiers

### 10. PluginDescriptors.cpp - Decoupage par domaine

Eclater [Source/Shared/Definitions/PluginDescriptors.cpp](Source/Shared/Definitions/PluginDescriptors.cpp) (2879 lignes) en 5 fichiers :

- **PluginDescriptors.cpp** (~194 lignes) : `kAllApvtsGroups` uniquement
- **PluginDescriptorsMasterEdit.cpp** (~366 lignes) : `MasterEditSection` (lignes 197-563)
- **PluginDescriptorsPatchEdit.cpp** (~1429 lignes) : `PatchEditSection` (lignes 564-1993)
- **PluginDescriptorsMatrixModulation.cpp** (~548 lignes) : `MatrixModulationSection` (lignes 1994-2542)
- **PluginDescriptorsPatchManager.cpp** (~337 lignes) : `PatchManagerSection` (lignes 2543-2879+)

Chaque fichier inclut uniquement `PluginDescriptors.h` et ouvre le namespace `PluginDescriptors`.

### 11. ApvtsFactory - Separation Builder/Validator

Eclater [Source/Core/Factories/ApvtsFactory.cpp](Source/Core/Factories/ApvtsFactory.cpp) (887 lignes) :

- **ApvtsLayoutBuilder.h/.cpp** : `createParameterLayout()` + toutes les methodes de creation de layout (createRootGroups, createChildGroups, addXParameters, getBusId, kSubgroupSeparator, etc.) ~400 lignes
- **ApvtsValidator.h/.cpp** : `ValidationResult`, `validatePluginDescriptors()` + toutes les methodes de validation (validateGroups, checkFor*, collectAll*, groupExists, etc.) ~500 lignes
- **ApvtsFactory.h/.cpp** : facade simplifie delegant a `ApvtsLayoutBuilder` et `ApvtsValidator` + methodes `getAllIntParameters()`, `getAllChoiceParameters()`, `getAllStandaloneWidgets()` (utilisees par les deux + WidgetFactory + PluginProcessor)

---

## Mise a jour CMakeLists.txt

Ajouter dans [CMakeLists.txt](CMakeLists.txt) les nouveaux fichiers source :

```
# Shared - Descriptors (split)
Source/Shared/Definitions/PluginDescriptorsMasterEdit.cpp
Source/Shared/Definitions/PluginDescriptorsPatchEdit.cpp
Source/Shared/Definitions/PluginDescriptorsMatrixModulation.cpp
Source/Shared/Definitions/PluginDescriptorsPatchManager.cpp

# Core - Factories (split)
Source/Core/Factories/ApvtsLayoutBuilder.cpp
Source/Core/Factories/ApvtsValidator.cpp
```

## Build final

Lancer `cmake --build Builds/macOS` pour valider la compilation.