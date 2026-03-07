---
name: Refactorisation et optimisation des performances
overview: Plan de refactorisation pour améliorer la clarté du code et optimiser les performances du plugin Matrix-Control, en introduisant des composants réutilisables (ModuleHeaderPanel, ParameterPanel, ModulationBusComponent) et en optimisant la création des widgets.
todos:
  - id: create-module-header-panel
    content: Créer ModuleHeaderPanel (h/cpp) pour encapsuler le header et les boutons Init/Copy/Paste
    status: pending
  - id: create-parameter-panel
    content: Créer ParameterPanel (h/cpp) pour encapsuler un paramètre avec label, widget (Slider/ComboBox), et séparateur
    status: pending
  - id: refactor-dco1-panel
    content: Refactoriser Dco1Panel pour utiliser ModuleHeaderPanel et ParameterPanel
    status: pending
  - id: refactor-other-modules
    content: Refactoriser les autres modules similaires (Dco2Panel, VcfVcaPanel, FmTrackPanel, RampPortamentoPanel, Env1/2/3Panel, Lfo1/2Panel)
    status: pending
  - id: create-modulation-bus-component
    content: Créer ModulationBusComponent (h/cpp) héritant de juce::Component pour encapsuler un bus de modulation
    status: pending
  - id: refactor-matrix-modulation-panel
    content: Refactoriser MatrixModulationPanel pour utiliser ModulationBusComponent
    status: pending
  - id: profile-with-instruments
    content: Profiler le plugin avec Xcode Instruments pour identifier les vrais goulots d'étranglement
    status: pending
  - id: optimize-widget-factory
    content: Optimiser WidgetFactory si nécessaire (unordered_map, cache, etc.) basé sur les résultats du profiling
    status: pending
  - id: implement-lazy-initialization
    content: Implémenter la lazy initialization des widgets si nécessaire basé sur les résultats du profiling
    status: pending
  - id: test-and-validate
    content: Tester fonctionnellement et valider les améliorations de performance
    status: pending
---

# Plan de refactorisation et d'amélioration des performances

## Objectifs

1. **Clarté et maintenabilité** : Réduire la répétition de code dans les panels de modules (Dco1Panel, Dco2Panel, Env1/2/3, Lfo1/2, etc.)
2. **Performance** : Améliorer le temps de chargement du plugin et la réactivité de l'interface utilisateur

## Phase 1 : Refactorisation pour la clarté

### 1.1 Création de ModuleHeaderPanel

**Fichiers à créer :**

- `Source/GUI/Panels/BodyPanel/PatchEditPanel/TopPanel/Modules/ModuleHeaderPanel.h`
- `Source/GUI/Panels/BodyPanel/PatchEditPanel/TopPanel/Modules/ModuleHeaderPanel.cpp`

**Responsabilité :** Encapsuler le header du module et les boutons Init/Copy/Paste (ou seulement Init selon le module).

**Structure :**

- Contient un `ModuleHeader` et 0-3 `Button` (Init, Copy, Paste)
- Gère son propre layout dans `resized()`
- Propage le thème dans `setTheme()`

### 1.2 Création de ParameterPanel

**Fichiers à créer :**

- `Source/GUI/Panels/BodyPanel/PatchEditPanel/TopPanel/Modules/ParameterPanel.h`
- `Source/GUI/Panels/BodyPanel/PatchEditPanel/TopPanel/Modules/ParameterPanel.cpp`

**Responsabilité :** Encapsuler un paramètre avec son label, son widget (Slider ou ComboBox), et son séparateur.

**Structure :**

- Contient un `Label`, un widget optionnel (`Slider` ou `ComboBox`), et un `HorizontalSeparator`
- Gère son propre layout dans `resized()`
- Propage le thème dans `setTheme()`
- Supporte les paramètres sans widget (label seul)

### 1.3 Refactorisation de Dco1Panel

**Fichiers à modifier :**

- `Source/GUI/Panels/BodyPanel/PatchEditPanel/TopPanel/Modules/Dco1Panel.h`
- `Source/GUI/Panels/BodyPanel/PatchEditPanel/TopPanel/Modules/Dco1Panel.cpp`

**Changements :**

- Remplacer les membres individuels (labels, sliders, comboboxes, separators) par :
- Un `std::unique_ptr<ModuleHeaderPanel>`
- Un `std::vector<std::unique_ptr<ParameterPanel>>` pour les paramètres
- Simplifier `resized()` : empiler verticalement le header et les ParameterPanels
- Simplifier `setTheme()` : propager le thème au header et itérer sur les ParameterPanels
- Réduire le constructeur : créer le header et les ParameterPanels via des méthodes helper

**Résultat attendu :** `resized()` passe de ~160 lignes à ~20 lignes, `setTheme()` de ~107 lignes à ~10 lignes

### 1.4 Refactorisation des autres modules similaires

**Modules à refactoriser (même pattern que Dco1Panel) :**

- `Dco2Panel` (TopPanel)
- `VcfVcaPanel` (TopPanel)
- `FmTrackPanel` (TopPanel)
- `RampPortamentoPanel` (TopPanel)
- `Env1Panel` (BottomPanel)
- `Env2Panel` (BottomPanel)
- `Env3Panel` (BottomPanel)
- `Lfo1Panel` (BottomPanel)
- `Lfo2Panel` (BottomPanel)

**Approche :** Appliquer le même pattern que pour Dco1Panel

### 1.5 Création de ModulationBusComponent

**Fichiers à créer :**

- `Source/GUI/Panels/BodyPanel/MatrixModulationPanel/ModulationBusComponent.h`
- `Source/GUI/Panels/BodyPanel/MatrixModulationPanel/ModulationBusComponent.cpp`

**Responsabilité :** Encapsuler un bus de modulation complet comme un `juce::Component` réutilisable.

**Structure :**

- Hérite de `juce::Component`
- Contient tous les widgets d'un bus (label, sourceComboBox, amountSlider, destinationComboBox, initButton, separator)
- Gère son propre layout dans `resized()`
- Propage le thème dans `setTheme()`

### 1.6 Refactorisation de MatrixModulationPanel

**Fichiers à modifier :**

- `Source/GUI/Panels/BodyPanel/MatrixModulationPanel/MatrixModulationPanel.h`
- `Source/GUI/Panels/BodyPanel/MatrixModulationPanel/MatrixModulationPanel.cpp`

**Changements :**

- Remplacer `std::array<ModulationBus, 10>` (struct) par `std::array<std::unique_ptr<ModulationBusComponent>, 10>`
- Simplifier `resized()` : itérer sur les ModulationBusComponents et appeler `setBounds()` pour chacun
- Simplifier `setTheme()` : itérer sur les ModulationBusComponents et appeler `setTheme()` pour chacun

## Phase 2 : Optimisation des performances

### 2.1 Optimisation de WidgetFactory

**Fichiers à modifier :**

- `Source/GUI/Factories/WidgetFactory.h`
- `Source/GUI/Factories/WidgetFactory.cpp`

**Problème identifié :** Chaque appel à `createIntParameterSlider()`, `createChoiceParameterComboBox()`, etc. effectue une recherche dans une map (`findIntParameter()`, `findChoiceParameter()`, etc.).

**Solution :**

- Les maps sont déjà construites une seule fois dans le constructeur (`buildSearchMaps()`)
- Le coût des lookups est négligeable (O(log n) avec std::map)
- **Vérification nécessaire :** Profiler avec Instruments pour confirmer si c'est vraiment un goulot d'étranglement

**Optimisations potentielles :**

- Si les lookups sont un problème : utiliser `std::unordered_map` au lieu de `std::map` (O(1) au lieu de O(log n))
- Cache des widgets créés récemment (peu probable d'être bénéfique)

### 2.2 Lazy Initialization des widgets

**Problème identifié :** Tous les widgets sont créés au démarrage du plugin dans les constructeurs des panels.

**Solution :**

- **Option A (simple) :** Créer les widgets de manière asynchrone après l'affichage initial
- **Option B (avancée) :** Créer les widgets seulement quand ils deviennent visibles (lazy loading)

**Fichiers à modifier :**

- Tous les panels de modules (Dco1Panel, Dco2Panel, etc.)
- `PluginEditor.cpp` pour gérer l'initialisation asynchrone

**Approche recommandée :**

- Commencer par l'Option A : utiliser `juce::MessageManager::callAsync()` pour créer les widgets après le premier repaint
- Mesurer l'amélioration avec Instruments
- Si nécessaire, implémenter l'Option B avec un système de visibilité

### 2.3 Optimisation de resized() et setTheme()

**Problème identifié :** Les méthodes `resized()` et `setTheme()` font de nombreux appels individuels à `setBounds()` et `setTheme()`.

**Solution :**

- La refactorisation avec `ModuleHeaderPanel` et `ParameterPanel` réduira déjà le nombre d'appels
- Les composants enfants gèrent leur propre layout, réduisant les calculs répétés
- **Vérification :** Profiler avant/après pour mesurer l'amélioration

### 2.4 Profiling avec Instruments

**Outils à utiliser :**

- Xcode Instruments (Time Profiler)
- Mesurer le temps de chargement du plugin
- Identifier les goulots d'étranglement réels

**Métriques à mesurer :**

- Temps de construction de `PluginEditor`
- Temps de construction de tous les panels
- Temps de première exécution de `resized()`
- Temps de première exécution de `setTheme()`
- Temps de premier clic sur un bouton

## Phase 3 : Tests et validation

### 3.1 Tests fonctionnels

- Vérifier que tous les widgets fonctionnent correctement après refactorisation
- Vérifier que les layouts sont corrects
- Vérifier que les thèmes s'appliquent correctement

### 3.2 Tests de performance

- Comparer les temps de chargement avant/après
- Comparer la réactivité de l'interface avant/après
- Valider que les optimisations apportent un bénéfice mesurable

## Ordre d'exécution recommandé

1. **Phase 1.1-1.3** : Créer ModuleHeaderPanel, ParameterPanel, et refactoriser Dco1Panel (validation du pattern)
2. **Phase 1.4** : Refactoriser les autres modules similaires
3. **Phase 1.5-1.6** : Créer ModulationBusComponent et refactoriser MatrixModulationPanel
4. **Phase 2.4** : Profiler avec Instruments pour identifier les vrais goulots d'étranglement
5. **Phase 2.1-2.3** : Appliquer les optimisations identifiées par le profiling
6. **Phase 3** : Tests et validation

## Notes importantes

- La refactorisation pour la clarté peut déjà améliorer les performances indirectement (moins de code à exécuter, meilleure localité des données)
- Le profiling avec Instruments est essentiel pour identifier les vrais problèmes de performance
- Ne pas optimiser prématurément : mesurer d'abord, optimiser ensuite
- Les optimisations doivent être validées par des mesures concrètes