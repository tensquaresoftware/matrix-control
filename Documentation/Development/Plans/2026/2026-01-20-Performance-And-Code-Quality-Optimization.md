---
name: Optimisation Performance et Qualité Code
overview: Plan d'action pour améliorer les performances GUI, refactoriser le code selon Clean Code, supprimer drawBase des widgets, et retirer le thème Debug.
todos:
  - id: perf-disable-logging
    content: Désactiver le logging APVTS et MIDI en production (conditionner avec JUCE_DEBUG)
    status: pending
  - id: perf-set-opaque
    content: Ajouter setOpaque(true) sur tous les widgets opaques (Slider, ComboBox, Button, Label, NumberBox, etc.)
    status: pending
  - id: perf-buffer-image
    content: Ajouter setBufferedToImage(true) sur les composants avec paint() complexe (EnvelopeDisplay, TrackGeneratorDisplay, etc.)
    status: pending
  - id: perf-optimize-repaints
    content: Optimiser les repaints lors des changements de thème (éviter cascade, regrouper)
    status: pending
  - id: clean-refactor-valueTreePropertyChanged
    content: Refactoriser PluginProcessor::valueTreePropertyChanged en petites fonctions explicites (resolveParameterIdFromTree, findParameterIdInChildren, etc.)
    status: pending
  - id: clean-factorize-widgetfactory
    content: Factoriser les méthodes addXParametersToMap dans WidgetFactory si opportun
    status: pending
  - id: cleanup-remove-drawbase
    content: "Supprimer drawBase() dans tous les widgets (20+ fichiers : Slider, ComboBox, Button, Label, NumberBox, GroupLabel, SectionHeader, ModuleHeader, ModulationBusHeader, HorizontalSeparator, VerticalSeparator, PopupMenu, EnvelopeDisplay, PatchNameDisplay, TrackGeneratorDisplay)"
    status: pending
  - id: cleanup-remove-basecolour-methods
    content: Supprimer toutes les méthodes getXxxBaseColour() du Theme (13 méthodes)
    status: pending
  - id: cleanup-remove-basecolour-themecolours
    content: Supprimer les entrées kXxxBase dans ThemeColours.h
    status: pending
  - id: cleanup-remove-debug-enum
    content: Supprimer ColourVariant::Debug de l'enum et le case correspondant dans Theme.cpp
    status: pending
  - id: cleanup-remove-debug-button
    content: Supprimer debugThemeButton_ et getButtonDebug() dans HeaderPanel
    status: pending
  - id: cleanup-remove-debug-callback
    content: Supprimer le callback getButtonDebug().onClick dans PluginEditor.cpp
    status: pending
---

# Plan d'action : Optimisation Performance GUI et Amélioration Qualité Code

## Objectifs

1. **Performance GUI** : Éliminer les lenteurs au démarrage et les lag d'interaction
2. **Qualité Code** : Appliquer les principes Clean Code (Robert C. Martin) en profondeur
3. **Nettoyage** : Supprimer drawBase et le thème Debug

## Problèmes identifiés

### Performance GUI
- Méthode `valueTreePropertyChanged` trop longue (116 lignes) avec logique complexe
- Logging APVTS et MIDI activé en production (overhead significatif)
- Absence de `setOpaque(true)` sur la plupart des composants (repaints inutiles)
- Absence de `setBufferedToImage` pour composants complexes
- Nombreux appels `repaint()` dans les callbacks de thème
- Propagation en cascade de `repaint()` lors des changements de thème

### Qualité Code
- `PluginProcessor::valueTreePropertyChanged` viole le principe de fonctions courtes (116 lignes)
- Logique complexe de résolution d'ID de paramètres non factorisée
- Nombreuses méthodes `addXParametersToMap` dans WidgetFactory (potentiel de factorisation)

### Nettoyage
- Méthode `drawBase()` présente dans 20+ widgets (inutile en production)
- Thème Debug utilisé uniquement pour le développement
- Méthodes `getXxxBaseColour()` dans Theme (liées au Debug)

## Actions à réaliser

### Phase 1 : Optimisations Performance GUI Critiques

#### 1.1 Désactiver le logging en production
- **Fichiers** : `Source/Core/PluginProcessor.cpp`
- **Actions** :
  - Commenter ou conditionner `enableApvtsLogging()` et `enableMidiLogging()` dans le constructeur
  - Garder uniquement pour builds Debug (via `#ifdef JUCE_DEBUG`)
  - Vérifier que `prepareToPlay()` n'active pas le logging en release

#### 1.2 Ajouter setOpaque(true) sur tous les composants opaques
- **Fichiers** : Tous les widgets dans `Source/GUI/Widgets/`
- **Actions** :
  - Ajouter `setOpaque(true)` dans les constructeurs de tous les widgets opaques
  - Vérifier que `PopupMenu` a déjà `setOpaque(true)` (ligne 31)
  - Widgets concernés : Slider, ComboBox, Button, Label, NumberBox, etc.

#### 1.3 Utiliser setBufferedToImage pour composants complexes
- **Fichiers** : Composants avec dessin complexe
- **Actions** :
  - Identifier les composants avec `paint()` complexe (EnvelopeDisplay, TrackGeneratorDisplay, etc.)
  - Ajouter `setBufferedToImage(true)` dans leurs constructeurs
  - Tester que cela n'affecte pas le rendu

#### 1.4 Optimiser les repaints lors des changements de thème
- **Fichiers** : `Source/GUI/PluginEditor.cpp`, `Source/GUI/MainComponent.cpp`, `Source/GUI/Panels/`
- **Actions** :
  - Dans `updateTheme()`, éviter `repaint()` immédiat sur tous les enfants
  - Utiliser `MessageManager::callAsync()` pour différer les repaints si nécessaire
  - Regrouper les repaints avec `repaint()` sur le parent uniquement

### Phase 2 : Refactorisation Clean Code

#### 2.1 Refactoriser valueTreePropertyChanged
- **Fichier** : `Source/Core/PluginProcessor.cpp` (lignes 239-355)
- **Actions** :
  - Extraire la résolution de `propertyId` en `resolveParameterIdFromTree()`
  - Extraire la recherche dans les enfants en `findParameterIdInChildren()`
  - Extraire la logique de thread name en `getCurrentThreadNameForLogging()`
  - Créer `logParameterChange()` pour centraliser le logging
  - Réduire la méthode principale à < 20 lignes

#### 2.2 Factoriser WidgetFactory
- **Fichier** : `Source/GUI/Factories/WidgetFactory.cpp`
- **Actions** :
  - Analyser les méthodes `addXParametersToMap` pour identifier les patterns communs
  - Créer des méthodes génériques si possible (templates ou callbacks)
  - Réduire la duplication de code

### Phase 3 : Suppression drawBase

#### 3.1 Supprimer drawBase dans tous les widgets
- **Fichiers** : Tous les widgets dans `Source/GUI/Widgets/`
- **Widgets concernés** : Slider, ComboBox, Button, Label, NumberBox, GroupLabel, SectionHeader, ModuleHeader, ModulationBusHeader, HorizontalSeparator, VerticalSeparator, PopupMenu, EnvelopeDisplay, PatchNameDisplay, TrackGeneratorDisplay
- **Actions** :
  - Supprimer l'appel à `drawBase()` dans chaque méthode `paint()`
  - Supprimer la méthode `drawBase()` dans chaque .cpp
  - Supprimer la déclaration `drawBase()` dans chaque .h
  - Vérifier que la géométrie des widgets reste identique (pas de changement de bounds)

#### 3.2 Supprimer les méthodes BaseColour du Theme
- **Fichiers** : `Source/GUI/Themes/Theme.h`, `Source/GUI/Themes/Theme.cpp`
- **Méthodes à supprimer** :
  - `getSliderBaseColour()`
  - `getComboBoxBaseColour()`
  - `getButtonBaseColour()`
  - `getLabelBaseColour()`
  - `getGroupLabelBaseColour()`
  - `getSectionHeaderBaseColour()`
  - `getModuleHeaderBaseColour()`
  - `getVerticalSeparatorBaseColour()`
  - `getHorizontalSeparatorBaseColour()`
  - `getPopupMenuBaseColour()`
  - `getEnvelopeDisplayBaseColour()`
  - `getPatchNameDisplayBaseColour()`
  - `getTrackGeneratorDisplayBaseColour()`

#### 3.3 Supprimer les couleurs BaseColour de ThemeColours
- **Fichier** : `Source/GUI/Themes/ThemeColours.h`
- **Actions** :
  - Identifier et supprimer toutes les entrées `kXxxBase` dans les structures ColourElement

### Phase 4 : Suppression thème Debug

#### 4.1 Supprimer ColourVariant::Debug
- **Fichiers** : `Source/GUI/Themes/Theme.h`, `Source/GUI/Themes/Theme.cpp`
- **Actions** :
  - Retirer `Debug` de l'enum `ColourVariant`
  - Supprimer le case `ColourVariant::Debug` dans `getColour()`
  - Supprimer toutes les références aux variantes Debug dans ThemeColours

#### 4.2 Supprimer le bouton Debug dans HeaderPanel
- **Fichiers** : `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h`, `HeaderPanel.cpp`
- **Actions** :
  - Supprimer le membre `debugThemeButton_`
  - Supprimer la méthode `getButtonDebug()`
  - Retirer le bouton du layout dans `resized()`
  - Ajuster le layout pour ne garder que Black et Cream

#### 4.3 Supprimer le callback Debug dans PluginEditor
- **Fichier** : `Source/GUI/PluginEditor.cpp`
- **Actions** :
  - Supprimer le bloc `headerPanel.getButtonDebug().onClick` (lignes 33-37)

## Ordre d'exécution recommandé

1. **Phase 1** (Performance) : Impact immédiat sur les lenteurs
2. **Phase 3** (Suppression drawBase) : Simplification du code de rendu
3. **Phase 4** (Suppression Debug) : Nettoyage final
4. **Phase 2** (Clean Code) : Amélioration maintenabilité (peut être fait en parallèle)

## Fichiers principaux à modifier

- `Source/Core/PluginProcessor.cpp` : Refactorisation valueTreePropertyChanged, désactivation logging
- `Source/GUI/Widgets/*.cpp` et `*.h` : Suppression drawBase, ajout setOpaque
- `Source/GUI/Themes/Theme.h` et `Theme.cpp` : Suppression Debug, suppression BaseColour
- `Source/GUI/Themes/ThemeColours.h` : Suppression variantes Debug
- `Source/GUI/Panels/MainComponent/HeaderPanel/*` : Suppression bouton Debug
- `Source/GUI/PluginEditor.cpp` : Suppression callback Debug
- `Source/GUI/Factories/WidgetFactory.cpp` : Factorisation si opportun

## Tests à effectuer

- Vérifier que le démarrage du plugin est instantané
- Vérifier que les interactions (clics, drags) sont réactives
- Vérifier que le changement de thème (Black/Cream) fonctionne
- Vérifier que la géométrie des widgets est inchangée
- Vérifier qu'il n'y a pas de régressions visuelles