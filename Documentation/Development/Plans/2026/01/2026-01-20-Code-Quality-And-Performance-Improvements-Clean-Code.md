---
name: Code Quality and Performance Improvements
overview: Amélioration de la qualité du code selon les principes Clean Code, optimisation des performances GUI, et suppression des éléments de debug (drawBase, thème Debug). Refactorisation des patterns communs pour éliminer la duplication de code.
todos:
  - id: perf-disable-logging
    content: Désactiver le logging APVTS et MIDI en production (conditionner avec JUCE_DEBUG)
    status: completed
  - id: perf-set-opaque
    content: Ajouter setOpaque(true) sur tous les widgets opaques (Slider, ComboBox, Button, Label, NumberBox, etc.)
    status: completed
  - id: perf-buffer-image
    content: Ajouter setBufferedToImage(true) sur les composants avec paint() complexe (EnvelopeDisplay, TrackGeneratorDisplay, etc.)
    status: completed
  - id: perf-optimize-repaints
    content: Optimiser les repaints lors des changements de thème (éviter cascade, regrouper)
    status: completed
  - id: clean-refactor-valueTreePropertyChanged
    content: Refactoriser PluginProcessor::valueTreePropertyChanged en petites fonctions explicites (resolveParameterIdFromTree, findParameterIdInChildren, etc.)
    status: completed
  - id: refactor-module-panels-base
    content: Créer une classe de base BaseModulePanel pour éliminer la duplication entre Dco1Panel, Dco2Panel, VcfVcaPanel, Env1Panel, Env2Panel, Env3Panel, Lfo1Panel, Lfo2Panel, FmTrackPanel, RampPortamentoPanel
    status: completed
  - id: refactor-identify-other-patterns
    content: Identifier et refactoriser d'autres patterns communs dans le codebase (méthodes de création de widgets, patterns de layout, etc.)
    status: pending
  - id: cleanup-remove-drawbase
    content: "Supprimer drawBase() dans tous les widgets (20+ fichiers : Slider, ComboBox, Button, Label, NumberBox, GroupLabel, SectionHeader, ModuleHeader, ModulationBusHeader, HorizontalSeparator, VerticalSeparator, PopupMenu, EnvelopeDisplay, PatchNameDisplay, TrackGeneratorDisplay)"
    status: completed
  - id: cleanup-remove-basecolour-methods
    content: Supprimer toutes les méthodes getXxxBaseColour() du Theme (13 méthodes)
    status: completed
  - id: cleanup-remove-basecolour-themecolours
    content: Supprimer les entrées kXxxBase dans ThemeColours.h
    status: completed
  - id: cleanup-remove-debug-enum
    content: Supprimer ColourVariant::Debug de l'enum et le case correspondant dans Theme.cpp
    status: completed
  - id: cleanup-remove-debug-button
    content: Supprimer debugThemeButton_ et getButtonDebug() dans HeaderPanel
    status: completed
  - id: cleanup-remove-debug-callback
    content: Supprimer le callback getButtonDebug().onClick dans PluginEditor.cpp
    status: completed
---

# Plan d'action : Amélioration qualité code et performances GUI

## Objectifs

- Appliquer les principes Clean Code (Robert C. Martin) : code limpide, fonctions courtes aux noms explicites
- Optimiser les performances GUI (réduction des latences, amélioration de la réactivité)
- Supprimer les éléments de debug (drawBase, thème Debug)
- Refactoriser les patterns communs pour éliminer la duplication de code

## Phase 1 : Optimisations Performance GUI Critiques

### 1.1 Désactiver le logging en production

**Fichiers concernés :**

- `Source/Core/PluginProcessor.cpp` (lignes 26, 217-231)
- `Source/Core/Loggers/MidiLogger.cpp`
- `Source/Core/Loggers/ApvtsLogger.cpp`

**Actions :**

- Conditionner l'activation des loggers avec `#if JUCE_DEBUG` ou `#ifdef DEBUG`
- Désactiver `MidiLogger` et `ApvtsLogger` en mode release
- Vérifier que les appels de logging n'impactent pas les performances en production

### 1.2 Ajouter setOpaque(true) sur les widgets opaques

**Fichiers concernés :**

- `Source/GUI/Widgets/Slider.cpp`
- `Source/GUI/Widgets/ComboBox.cpp`
- `Source/GUI/Widgets/Button.cpp`
- `Source/GUI/Widgets/Label.cpp`
- `Source/GUI/Widgets/NumberBox.cpp`
- `Source/GUI/Widgets/GroupLabel.cpp`
- `Source/GUI/Widgets/SectionHeader.cpp`
- `Source/GUI/Widgets/ModuleHeader.cpp`
- `Source/GUI/Widgets/ModulationBusHeader.cpp`
- `Source/GUI/Widgets/HorizontalSeparator.cpp`
- `Source/GUI/Widgets/VerticalSeparator.cpp`
- `Source/GUI/Widgets/PatchNameDisplay.cpp`
- Tous les panels opaques dans `Source/GUI/Panels/`

**Actions :**

- Ajouter `setOpaque(true)` dans les constructeurs de tous les widgets opaques
- Vérifier que `PopupMenu.cpp` a déjà `setOpaque(true)` (ligne 31) et servir de référence

### 1.3 Ajouter setBufferedToImage(true) sur les composants complexes

**Fichiers concernés :**

- `Source/GUI/Widgets/EnvelopeDisplay.cpp`
- `Source/GUI/Widgets/TrackGeneratorDisplay.cpp`
- Autres composants avec `paint()` complexe

**Actions :**

- Ajouter `setBufferedToImage(true)` dans les constructeurs des composants avec rendu complexe
- Tester l'impact sur les performances

### 1.4 Optimiser les repaints lors des changements de thème

**Fichiers concernés :**

- `Source/GUI/PluginEditor.cpp` (méthode `updateTheme()`, ligne 63-68)
- `Source/GUI/MainComponent.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/BodyPanel.cpp`
- Tous les widgets avec `setTheme()` qui appellent `repaint()`

**Actions :**

- Éviter les cascades de `repaint()` lors des changements de thème
- Regrouper les repaints en un seul appel si possible
- Utiliser `repaint()` de manière sélective plutôt que systématique

## Phase 2 : Refactorisation Clean Code

### 2.1 Refactoriser valueTreePropertyChanged

**Fichier :** `Source/Core/PluginProcessor.cpp` (lignes 239-355, 116 lignes)

**Actions :**

- Extraire la logique de résolution d'ID de paramètre en fonctions séparées :
  - `resolveParameterIdFromTree(juce::ValueTree&, const juce::Identifier&) -> juce::String`
  - `findParameterIdInDirectTree(juce::ValueTree&) -> juce::String`
  - `findParameterIdInChildren(juce::ValueTree&, const juce::var&) -> juce::String`
  - `getThreadNameForLogging() -> juce::String`
- Réduire la complexité cyclomatique (objectif : < 10 par fonction)
- Chaque fonction doit avoir une responsabilité unique et un nom explicite

### 2.2 Factoriser les méthodes de WidgetFactory (si opportun)

**Fichiers :**

- `Source/GUI/Factories/WidgetFactory.cpp` (méthodes `addXParametersToMap`)

**Actions :**

- Analyser si les méthodes `addPatchEditModuleIntParametersToMap()` et `addPatchEditModuleChoiceParametersToMap()` peuvent être simplifiées
- Vérifier si un pattern commun peut être extrait

## Phase 3 : Refactorisation des Patterns Communs

### 3.1 Créer une classe de base pour les Module Panels

**Fichiers concernés :**

- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/TopPanel/Modules/Dco1Panel.h/.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/TopPanel/Modules/Dco2Panel.h/.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/TopPanel/Modules/VcfVcaPanel.h/.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/TopPanel/Modules/FmTrackPanel.h/.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/TopPanel/Modules/RampPortamentoPanel.h/.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/BottomPanel/Modules/Env1Panel.h/.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/BottomPanel/Modules/Env2Panel.h/.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/BottomPanel/Modules/Env3Panel.h/.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/BottomPanel/Modules/Lfo1Panel.h/.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/BottomPanel/Modules/Lfo2Panel.h/.cpp`

**Pattern identifié :**

Tous ces panels ont exactement la même structure :

- Même header (`ModuleHeaderPanel`)
- Même pattern de création de `ParameterPanel` (push_back répété)
- Mêmes méthodes `paint()`, `resized()`, `setTheme()`
- Seule différence : les IDs des paramètres et des modules

**Actions :**

- Créer une classe de base `BaseModulePanel` ou utiliser un template
- Extraire la logique commune dans la classe de base
- Les classes dérivées ne fournissent que la configuration (IDs de paramètres, IDs de module, ButtonSet)
- Utiliser une structure de configuration pour passer les paramètres à la classe de base
- Réduire chaque panel de ~150 lignes à ~20-30 lignes de configuration

**Structure proposée :**

```cpp
struct ModulePanelConfig
{
    juce::Identifier moduleId;
    ModuleHeaderPanel::ButtonSet buttonSet;
    std::vector<juce::Identifier> parameterIds;
    std::vector<ParameterPanel::ParameterType> parameterTypes;
    // ... autres configurations
};

class BaseModulePanel : public juce::Component
{
    // Logique commune : constructeur, paint(), resized(), setTheme()
};

class Dco1Panel : public BaseModulePanel
{
    // Seulement la configuration spécifique
};
```

### 3.2 Identifier et refactoriser d'autres patterns communs

**Actions :**

- Analyser le codebase pour identifier d'autres duplications
- Chercher les patterns répétitifs dans :
  - Les méthodes de création de widgets
  - Les patterns de layout
  - Les patterns de gestion de thème
  - Les patterns de gestion d'événements

## Phase 4 : Suppression drawBase

### 4.1 Supprimer drawBase() dans tous les widgets

**Fichiers concernés (20+ fichiers) :**

- `Source/GUI/Widgets/Slider.cpp`
- `Source/GUI/Widgets/ComboBox.cpp`
- `Source/GUI/Widgets/Button.cpp`
- `Source/GUI/Widgets/Label.cpp`
- `Source/GUI/Widgets/NumberBox.cpp`
- `Source/GUI/Widgets/GroupLabel.cpp`
- `Source/GUI/Widgets/SectionHeader.cpp`
- `Source/GUI/Widgets/ModuleHeader.cpp`
- `Source/GUI/Widgets/ModulationBusHeader.cpp`
- `Source/GUI/Widgets/HorizontalSeparator.cpp`
- `Source/GUI/Widgets/VerticalSeparator.cpp`
- `Source/GUI/Widgets/PopupMenu.cpp`
- `Source/GUI/Widgets/EnvelopeDisplay.cpp`
- `Source/GUI/Widgets/PatchNameDisplay.cpp`
- `Source/GUI/Widgets/TrackGeneratorDisplay.cpp`
- Et leurs headers correspondants

**Actions :**

- Supprimer les appels à `drawBase(g, bounds)` dans les méthodes `paint()`
- Supprimer les déclarations et définitions de `drawBase()` dans les headers et .cpp
- Vérifier que la géométrie des widgets reste inchangée

### 4.2 Supprimer les méthodes getXxxBaseColour() du Theme

**Fichiers :**

- `Source/GUI/Themes/Theme.h` (13 méthodes `getXxxBaseColour()`)
- `Source/GUI/Themes/Theme.cpp` (implémentations correspondantes)

**Actions :**

- Supprimer toutes les méthodes `getXxxBaseColour()` (ex: `getSliderBaseColour()`, `getButtonBaseColour()`, etc.)
- Vérifier qu'aucune autre partie du code n'utilise ces méthodes

### 4.3 Supprimer les entrées kXxxBase dans ThemeColours.h

**Fichier :** `Source/GUI/Themes/ThemeColours.h`

**Actions :**

- Supprimer toutes les entrées `kXxxBase` (ex: `kSliderBase`, `kButtonBase`, etc.)
- Supprimer les variantes `debugVariant` associées si elles existent

## Phase 5 : Suppression thème Debug

### 5.1 Supprimer ColourVariant::Debug de l'enum

**Fichiers :**

- `Source/GUI/Themes/Theme.h` (enum `ColourVariant`, ligne 14-19)
- `Source/GUI/Themes/Theme.cpp` (case `ColourVariant::Debug` dans `getColour()`, ligne 23-24)

**Actions :**

- Supprimer `Debug` de l'enum `ColourVariant`
- Supprimer le case correspondant dans le switch de `getColour()`
- Vérifier qu'aucun autre code ne référence `ColourVariant::Debug`

### 5.2 Supprimer le bouton Debug dans HeaderPanel

**Fichiers :**

- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h` (ligne 25, 43)
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp` (si initialisation présente)

**Actions :**

- Supprimer `debugThemeButton_` de la classe
- Supprimer `getButtonDebug()` de la classe
- Supprimer toute initialisation du bouton dans le constructeur

### 5.3 Supprimer le callback Debug dans PluginEditor

**Fichier :** `Source/GUI/PluginEditor.cpp` (lignes 33-37)

**Actions :**

- Supprimer le callback `headerPanel.getButtonDebug().onClick`
- Vérifier qu'aucune autre référence au bouton Debug n'existe

### 5.4 Nettoyer ThemeColours.h des variantes Debug

**Fichier :** `Source/GUI/Themes/ThemeColours.h`

**Actions :**

- Supprimer toutes les propriétés `debugVariant` des structures `ColourElement`
- Vérifier que seules les variantes `blackVariant` et `creamVariant` restent

## Ordre d'exécution recommandé

1. **Phase 1** (Performance) : Impact immédiat sur l'expérience utilisateur
2. **Phase 3** (Patterns communs) : Réduit significativement la duplication avant les autres refactorisations
3. **Phase 2** (Clean Code) : Améliore la maintenabilité
4. **Phase 4** (drawBase) : Nettoyage simple
5. **Phase 5** (Debug theme) : Nettoyage simple

## Notes importantes

- Tester après chaque phase pour s'assurer que les fonctionnalités restent intactes
- La géométrie des widgets ne doit pas changer lors de la suppression de drawBase
- La refactorisation des panels doit maintenir la même API publique si possible
- Respecter strictement les conventions de nommage et de formatage définies dans `.cursorrules`