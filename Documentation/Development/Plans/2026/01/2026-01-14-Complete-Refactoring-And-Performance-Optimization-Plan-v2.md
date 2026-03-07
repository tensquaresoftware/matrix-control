---
name: Refactorisation complète et optimisation des performances
overview: Plan complet de refactorisation incluant la création de composants réutilisables, la centralisation des dimensions dans PluginDimensions.h, la refactorisation des widgets pour accepter des largeurs en pixels, l'application de la convention de nommage avec underscore suffix, et l'optimisation des performances.
todos:
  - id: create-plugin-dimensions
    content: Créer PluginDimensions.h dans Source/Shared/ avec toutes les constantes de dimensions
    status: pending
  - id: refactor-button
    content: Refactoriser Button pour accepter int width au lieu de ButtonWidth enum
    status: pending
  - id: refactor-combobox
    content: Refactoriser ComboBox pour accepter int width au lieu de ComboBoxWidth enum
    status: pending
  - id: refactor-label
    content: Refactoriser Label pour accepter int width au lieu de LabelWidth enum
    status: pending
  - id: refactor-grouplabel
    content: Refactoriser GroupLabel pour accepter int width au lieu de GroupWidth enum
    status: pending
  - id: refactor-moduleheader
    content: Refactoriser ModuleHeader pour accepter int width au lieu de ModuleWidth enum
    status: pending
  - id: refactor-sectionheader
    content: Refactoriser SectionHeader pour accepter int width au lieu de SectionWidth enum
    status: pending
  - id: refactor-horizontalseparator
    content: Refactoriser HorizontalSeparator pour accepter int width au lieu de SeparatorWidth enum
    status: pending
  - id: apply-naming-convention
    content: Appliquer la convention de nommage avec underscore suffix à tous les fichiers (membres privés avec '_', paramètres sans préfixe 'in')
    status: pending
  - id: update-cursorrules
    content: Mettre à jour .cursorrules avec la nouvelle convention de nommage
    status: pending
  - id: create-module-header-panel
    content: Créer ModuleHeaderPanel dans Source/GUI/ReusablePanels/
    status: pending
  - id: create-parameter-panel
    content: Créer ParameterPanel dans Source/GUI/ReusablePanels/ (support complet et vide)
    status: pending
  - id: refactor-dco1-panel
    content: Refactoriser Dco1Panel pour utiliser ModuleHeaderPanel et ParameterPanel
    status: pending
  - id: refactor-other-modules
    content: Refactoriser les autres modules similaires (Dco2Panel, VcfVcaPanel, FmTrackPanel, RampPortamentoPanel, Env1/2/3Panel, Lfo1/2Panel, MidiPanel, VibratoPanel, MiscPanel)
    status: pending
  - id: create-modulation-bus-component
    content: Créer ModulationBusComponent héritant de juce::Component
    status: pending
  - id: refactor-matrix-modulation-panel
    content: Refactoriser MatrixModulationPanel pour utiliser ModulationBusComponent
    status: pending
  - id: profile-with-instruments
    content: Profiler le plugin avec Xcode Instruments pour identifier les vrais goulots d'étranglement
    status: pending
  - id: optimize-widget-factory
    content: Optimiser WidgetFactory si nécessaire basé sur les résultats du profiling
    status: pending
  - id: implement-lazy-initialization
    content: Implémenter la lazy initialization des widgets si nécessaire basé sur les résultats du profiling
    status: pending
  - id: test-and-validate
    content: Tester fonctionnellement et valider les améliorations de performance
    status: pending
---

# Plan complet de refactorisation et d'amélioration des performances

## Objectifs

1. **Clarté et maintenabilité** : Réduire la répétition de code dans les panels de modules
2. **Réutilisabilité** : Rendre les widgets génériques et réutilisables pour d'autres projets
3. **Centralisation** : Créer une source unique de vérité pour les dimensions (PluginDimensions.h)
4. **Convention de nommage** : Adopter la convention Google C++ avec underscore suffix pour les membres privés
5. **Performance** : Améliorer le temps de chargement et la réactivité de l'interface

## Phase 0 : Préparation et refactorisation des widgets

### 0.1 Création de PluginDimensions.h

**Fichier à créer :**

- `Source/Shared/PluginDimensions.h`

**Structure proposée :**

```cpp
namespace PluginDimensions
{
    // Button widths
    inline constexpr int kInitCopyPasteButtonWidth = 20;
    inline constexpr int kPatchManagerBankSelectButtonWidth = 35;
    inline constexpr int kInternalPatchesUtilityButtonWidth = 35;
    inline constexpr int kPatchManagerUnlockButtonWidth = 75;
    inline constexpr int kHeaderPanelThemeButtonWidth = 70;
    
    // ComboBox widths
    inline constexpr int kPatchEditModuleComboBoxWidth = 60;
    inline constexpr int kMasterEditModuleComboBoxWidth = 60;
    inline constexpr int kMatrixModulationAmountComboBoxWidth = 60;
    inline constexpr int kMatrixModulationDestinationComboBoxWidth = 105;
    
    // Label widths
    inline constexpr int kPatchEditModuleLabelWidth = 90;
    inline constexpr int kMasterEditModuleLabelWidth = 100;
    inline constexpr int kModulationBusNumberLabelWidth = 15;
    inline constexpr int kPatchManagerBankSelectorLabelWidth = 75;
    
    // GroupLabel widths
    inline constexpr int kInternalPatchesBrowserGroupLabelWidth = 110;
    inline constexpr int kInternalPatchesUtilityGroupLabelWidth = 155;
    inline constexpr int kComputerPatchesBrowserGroupLabelWidth = 140;
    inline constexpr int kComputerPatchesFolderAndFilesGroupLabelWidth = 130;
    
    // ModuleHeader widths
    inline constexpr int kPatchEditModuleHeaderWidth = 150;
    inline constexpr int kMasterEditModuleHeaderWidth = 160;
    inline constexpr int kPatchManagerModuleHeaderWidth = 275;
    
    // SectionHeader widths
    inline constexpr int kPatchEditSectionHeaderWidth = 810;
    inline constexpr int kMatrixModulationSectionHeaderWidth = 275;
    inline constexpr int kPatchManagerSectionHeaderWidth = 275;
    inline constexpr int kMasterEditSectionHeaderWidth = 160;
    
    // HorizontalSeparator widths
    inline constexpr int kPatchEditModuleSeparatorWidth = 150;
    inline constexpr int kMasterEditModuleSeparatorWidth = 160;
    inline constexpr int kMatrixModulationBusSeparatorWidth = 275;
    
    // Panel widths
    inline constexpr int kPatchEditModulePanelWidth = 150;
    inline constexpr int kMasterEditModulePanelWidth = 160;
    inline constexpr int kMatrixModulationPanelWidth = 275;
    
    // Common heights
    inline constexpr int kButtonHeight = 20;
    inline constexpr int kComboBoxHeight = 20;
    inline constexpr int kLabelHeight = 20;
    inline constexpr int kModuleHeaderHeight = 30;
    inline constexpr int kSectionHeaderHeight = 30;
    inline constexpr int kGroupLabelHeight = 30;
    inline constexpr int kHorizontalSeparatorHeight = 5;
    // etc.
}
```

**Responsabilité :** Centraliser toutes les dimensions des widgets et panneaux, similaire à PluginDescriptors.h

### 0.2 Refactorisation de Button

**Fichiers à modifier :**

- `Source/GUI/Widgets/Button.h`
- `Source/GUI/Widgets/Button.cpp`
- Tous les fichiers qui utilisent `Button` (rechercher `ButtonWidth::`)

**Changements :**

- Supprimer l'enum `ButtonWidth`
- Modifier le constructeur : `Button(Theme& theme, int width, const juce::String& text = {})`
- Supprimer la méthode statique `getWidth(ButtonWidth)`
- Supprimer les constantes privées `kInitCopyPasteWidth`, etc.
- Stocker la largeur comme membre privé `int width_;`
- Mettre à jour tous les usages pour utiliser les constantes de `PluginDimensions`

**Exemple de migration :**

```cpp
// Avant
button = std::make_unique<tss::Button>(theme, tss::Button::ButtonWidth::InitCopyPaste, "I");
const auto width = tss::Button::getWidth(tss::Button::ButtonWidth::InitCopyPaste);

// Après
button = std::make_unique<tss::Button>(theme, PluginDimensions::kInitCopyPasteButtonWidth, "I");
const auto width = PluginDimensions::kInitCopyPasteButtonWidth;
```

### 0.3 Refactorisation de ComboBox

**Fichiers à modifier :**

- `Source/GUI/Widgets/ComboBox.h`
- `Source/GUI/Widgets/ComboBox.cpp`
- Tous les fichiers qui utilisent `ComboBox` (rechercher `ComboBoxWidth::`)

**Changements :**

- Supprimer l'enum `ComboBoxWidth`
- Modifier le constructeur : `ComboBox(Theme& theme, int width = PluginDimensions::kPatchEditModuleComboBoxWidth)`
- Supprimer la méthode statique `getWidth(ComboBoxWidth)`
- Supprimer les constantes privées `kNormalWidth`, `kLargeWidth`, etc.
- Stocker la largeur comme membre privé `int width_;`
- Mettre à jour tous les usages

### 0.4 Refactorisation de Label

**Fichiers à modifier :**

- `Source/GUI/Widgets/Label.h`
- `Source/GUI/Widgets/Label.cpp`
- Tous les fichiers qui utilisent `Label` (rechercher `LabelWidth::`)

**Changements :**

- Supprimer l'enum `LabelWidth`
- Modifier le constructeur : `Label(Theme& theme, int width, const juce::String& text = {})`
- Supprimer la méthode statique `getWidth(LabelWidth)`
- Supprimer les constantes privées `kBusNumberWidth`, etc.
- Stocker la largeur comme membre privé `int width_;`
- Mettre à jour tous les usages

### 0.5 Refactorisation de GroupLabel

**Fichiers à modifier :**

- `Source/GUI/Widgets/GroupLabel.h`
- `Source/GUI/Widgets/GroupLabel.cpp`
- Tous les fichiers qui utilisent `GroupLabel` (rechercher `GroupWidth::`)

**Changements :**

- Supprimer l'enum `GroupWidth`
- Modifier le constructeur : `GroupLabel(Theme& theme, int width, const juce::String& text = {})`
- Supprimer la méthode statique `getWidth(GroupWidth)`
- Supprimer les constantes privées
- Stocker la largeur comme membre privé `int width_;`
- Mettre à jour tous les usages

### 0.6 Refactorisation de ModuleHeader

**Fichiers à modifier :**

- `Source/GUI/Widgets/ModuleHeader.h`
- `Source/GUI/Widgets/ModuleHeader.cpp`
- Tous les fichiers qui utilisent `ModuleHeader` (rechercher `ModuleWidth::`)

**Changements :**

- Supprimer l'enum `ModuleWidth`
- Modifier le constructeur : `ModuleHeader(Theme& theme, const juce::String& text = {}, int width = PluginDimensions::kPatchEditModuleHeaderWidth, ColourVariant variant = ColourVariant::Blue)`
- Supprimer la méthode statique `getWidth(ModuleWidth)`
- Supprimer les constantes privées
- Stocker la largeur comme membre privé `int width_;`
- Mettre à jour tous les usages

### 0.7 Refactorisation de SectionHeader

**Fichiers à modifier :**

- `Source/GUI/Widgets/SectionHeader.h`
- `Source/GUI/Widgets/SectionHeader.cpp`
- Tous les fichiers qui utilisent `SectionHeader` (rechercher `SectionWidth::`)

**Changements :**

- Supprimer l'enum `SectionWidth`
- Modifier le constructeur : `SectionHeader(Theme& theme, int width, const juce::String& text = {}, ColourVariant variant = ColourVariant::Blue)`
- Supprimer la méthode statique `getWidth(SectionWidth)`
- Supprimer les constantes privées
- Stocker la largeur comme membre privé `int width_;`
- Mettre à jour tous les usages

### 0.8 Refactorisation de HorizontalSeparator

**Fichiers à modifier :**

- `Source/GUI/Widgets/HorizontalSeparator.h`
- `Source/GUI/Widgets/HorizontalSeparator.cpp`
- Tous les fichiers qui utilisent `HorizontalSeparator` (rechercher `SeparatorWidth::`)

**Changements :**

- Supprimer l'enum `SeparatorWidth`
- Modifier le constructeur : `HorizontalSeparator(Theme& theme, int width)`
- Supprimer la méthode statique `getWidth(SeparatorWidth)`
- Supprimer les constantes privées
- Stocker la largeur comme membre privé `int width_;`
- Ajouter une méthode `int getWidth() const { return width_; }` pour compatibilité
- Mettre à jour tous les usages

### 0.9 Application de la convention de nommage avec underscore suffix

**Fichiers à modifier :**

- Tous les fichiers du projet (widgets, panels, factories, etc.)

**Changements :**

- Tous les membres privés : ajouter le suffix `_` (ex: `theme` → `theme_`, `apvts` → `apvts_`, `dragStartValue` → `dragStartValue_`)
- Tous les paramètres de méthodes : supprimer le préfixe "in" ou "new" (ex: `inTheme` → `theme`, `inParameterId` → `parameterId`)
- Mettre à jour les initialisations dans les constructeurs : `theme_(theme)` au lieu de `theme(inTheme)`
- Mettre à jour les assignations : `theme_ = &theme;` au lieu de `theme = &inTheme;`

**Exemple de migration :**

```cpp
// Avant
class Dco1Panel
{
private:
    tss::Theme* theme;
    juce::AudioProcessorValueTreeState& apvts;
    
    Dco1Panel(tss::Theme& inTheme, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& inApvts)
        : theme(&inTheme), apvts(inApvts) {}
};

// Après
class Dco1Panel
{
private:
    tss::Theme* theme_;
    juce::AudioProcessorValueTreeState& apvts_;
    
    Dco1Panel(tss::Theme& theme, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
        : theme_(&theme), apvts_(apvts) {}
};
```

### 0.10 Mise à jour des cursorrules

**Fichier à modifier :**

- `.cursorrules`

**Changements :**

- Section "Conventions de nommage" (lignes 80-92) :
  - Remplacer "Variables privées : pas de préfixe ni suffixe" par "Variables privées : suffix underscore `_` (convention Google C++ Style Guide)"
  - Remplacer "Paramètres de constructeur : préfixe "in" pour éviter le shadowing" par "Paramètres de méthodes : pas de préfixe (évite le shadowing grâce au suffix `_` des membres)"

## Phase 1 : Création des composants réutilisables

### 1.1 Création de ModuleHeaderPanel

**Fichiers à créer :**

- `Source/GUI/ReusablePanels/ModuleHeaderPanel.h`
- `Source/GUI/ReusablePanels/ModuleHeaderPanel.cpp`

**Responsabilité :** Encapsuler le header du module et les boutons Init/Copy/Paste (ou seulement Init selon le module).

**Structure :**

- Contient un `ModuleHeader` et 0-3 `Button` (Init, Copy, Paste)
- Constructeur : `ModuleHeaderPanel(Theme& theme, WidgetFactory& widgetFactory, const juce::String& moduleId, const juce::String& initWidgetId, const juce::String& copyWidgetId = {}, const juce::String& pasteWidgetId = {})`
- Gère son propre layout dans `resized()`
- Propage le thème dans `setTheme()`
- Utilise les constantes de `PluginDimensions` pour les largeurs

### 1.2 Création de ParameterPanel

**Fichiers à créer :**

- `Source/GUI/ReusablePanels/ParameterPanel.h`
- `Source/GUI/ReusablePanels/ParameterPanel.cpp`

**Responsabilité :** Encapsuler un paramètre avec son label, son widget (Slider ou ComboBox), et son séparateur.

**Structure :**

- Peut être de deux types :
  - **Complet** : `Label` + `Slider` ou `ComboBox` + `HorizontalSeparator`
  - **Vide** : uniquement `HorizontalSeparator` (pour les séparateurs simples sans paramètre)
- Constructeurs :
  - `ParameterPanel(Theme& theme, WidgetFactory& widgetFactory, const juce::String& parameterId, int labelWidth, int widgetWidth, bool isChoiceParameter, juce::AudioProcessorValueTreeState& apvts)`
  - `ParameterPanel(Theme& theme, int separatorWidth)` (pour le cas vide)
- Gère son propre layout dans `resized()`
- Propage le thème dans `setTheme()`
- Utilise les constantes de `PluginDimensions` pour les largeurs

### 1.3 Création de ModulationBusComponent

**Fichiers à créer :**

- `Source/GUI/Panels/BodyPanel/MatrixModulationPanel/ModulationBusComponent.h`
- `Source/GUI/Panels/BodyPanel/MatrixModulationPanel/ModulationBusComponent.cpp`

**Responsabilité :** Encapsuler un bus de modulation complet comme un `juce::Component` réutilisable.

**Structure :**

- Hérite de `juce::Component`
- Contient tous les widgets d'un bus (label, sourceComboBox, amountSlider, destinationComboBox, initButton, separator)
- Constructeur : `ModulationBusComponent(Theme& theme, WidgetFactory& widgetFactory, int busNumber, const ModulationBusParameterArrays& parameterArrays, juce::AudioProcessorValueTreeState& apvts)`
- Gère son propre layout dans `resized()`
- Propage le thème dans `setTheme()`
- Utilise les constantes de `PluginDimensions` pour les largeurs

## Phase 2 : Refactorisation des panels existants

### 2.1 Refactorisation de Dco1Panel

**Fichiers à modifier :**

- `Source/GUI/Panels/BodyPanel/PatchEditPanel/TopPanel/Modules/Dco1Panel.h`
- `Source/GUI/Panels/BodyPanel/PatchEditPanel/TopPanel/Modules/Dco1Panel.cpp`

**Changements :**

- Remplacer les membres individuels (labels, sliders, comboboxes, separators) par :
  - Un `std::unique_ptr<ModuleHeaderPanel>`
  - Un `std::vector<std::unique_ptr<ParameterPanel>>` pour les paramètres
- Simplifier `resized()` : empiler verticalement le header et les ParameterPanels (~20 lignes au lieu de ~160)
- Simplifier `setTheme()` : propager le thème au header et itérer sur les ParameterPanels (~10 lignes au lieu de ~107)
- Réduire le constructeur : créer le header et les ParameterPanels via des méthodes helper
- Appliquer la convention de nommage avec underscore suffix

### 2.2 Refactorisation des autres modules similaires

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
- `MidiPanel` (MasterEditPanel)
- `VibratoPanel` (MasterEditPanel)
- `MiscPanel` (MasterEditPanel)

**Approche :** Appliquer le même pattern que pour Dco1Panel

### 2.3 Refactorisation de MatrixModulationPanel

**Fichiers à modifier :**

- `Source/GUI/Panels/BodyPanel/MatrixModulationPanel/MatrixModulationPanel.h`
- `Source/GUI/Panels/BodyPanel/MatrixModulationPanel/MatrixModulationPanel.cpp`

**Changements :**

- Remplacer `std::array<ModulationBus, 10>` (struct) par `std::array<std::unique_ptr<ModulationBusComponent>, 10>`
- Simplifier `resized()` : itérer sur les ModulationBusComponents et appeler `setBounds()` pour chacun
- Simplifier `setTheme()` : itérer sur les ModulationBusComponents et appeler `setTheme()` pour chacun
- Appliquer la convention de nommage avec underscore suffix

## Phase 3 : Optimisation des performances

### 3.1 Profiling avec Instruments

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

### 3.2 Optimisation de WidgetFactory

**Fichiers à modifier :**

- `Source/GUI/Factories/WidgetFactory.h`
- `Source/GUI/Factories/WidgetFactory.cpp`

**Problème identifié :** Chaque appel à `createIntParameterSlider()`, `createChoiceParameterComboBox()`, etc. effectue une recherche dans une map.

**Solution :**

- Les maps sont déjà construites une seule fois dans le constructeur
- Le coût des lookups est négligeable (O(log n) avec std::map)
- **Vérification nécessaire :** Profiler avec Instruments pour confirmer si c'est vraiment un goulot d'étranglement

**Optimisations potentielles (si nécessaire) :**

- Utiliser `std::unordered_map` au lieu de `std::map` (O(1) au lieu de O(log n))
- Cache des widgets créés récemment (peu probable d'être bénéfique)

### 3.3 Lazy Initialization des widgets

**Problème identifié :** Tous les widgets sont créés au démarrage du plugin dans les constructeurs des panels.

**Solution :**

- **Option A (simple) :** Créer les widgets de manière asynchrone après l'affichage initial
- **Option B (avancée) :** Créer les widgets seulement quand ils deviennent visibles (lazy loading)

**Fichiers à modifier :**

- Tous les panels de modules
- `PluginEditor.cpp` pour gérer l'initialisation asynchrone

**Approche recommandée :**

- Commencer par l'Option A : utiliser `juce::MessageManager::callAsync()` pour créer les widgets après le premier repaint
- Mesurer l'amélioration avec Instruments
- Si nécessaire, implémenter l'Option B avec un système de visibilité

## Phase 4 : Tests et validation

### 4.1 Tests fonctionnels

- Vérifier que tous les widgets fonctionnent correctement après refactorisation
- Vérifier que les layouts sont corrects
- Vérifier que les thèmes s'appliquent correctement
- Vérifier que la convention de nommage est appliquée partout

### 4.2 Tests de performance

- Comparer les temps de chargement avant/après
- Comparer la réactivité de l'interface avant/après
- Valider que les optimisations apportent un bénéfice mesurable

## Ordre d'exécution recommandé

1. **Phase 0.1** : Créer PluginDimensions.h avec toutes les constantes
2. **Phase 0.2-0.8** : Refactoriser tous les widgets pour accepter des largeurs en pixels (Button, ComboBox, Label, GroupLabel, ModuleHeader, SectionHeader, HorizontalSeparator)
3. **Phase 0.9** : Appliquer la convention de nommage avec underscore suffix à tous les fichiers
4. **Phase 0.10** : Mettre à jour les cursorrules
5. **Phase 1.1-1.2** : Créer ModuleHeaderPanel et ParameterPanel (validation du pattern)
6. **Phase 2.1** : Refactoriser Dco1Panel pour utiliser les nouveaux composants
7. **Phase 2.2** : Refactoriser les autres modules similaires
8. **Phase 1.3 et 2.3** : Créer ModulationBusComponent et refactoriser MatrixModulationPanel
9. **Phase 3.1** : Profiler avec Instruments pour identifier les vrais goulots d'étranglement
10. **Phase 3.2-3.3** : Appliquer les optimisations identifiées par le profiling
11. **Phase 4** : Tests et validation

## Notes importantes

- La refactorisation des widgets (Phase 0) doit être faite AVANT la création de ModuleHeaderPanel et ParameterPanel
- La convention de nommage avec underscore suffix doit être appliquée progressivement, fichier par fichier
- PluginDimensions.h doit être créé en premier pour servir de référence pour tous les widgets
- Le profiling avec Instruments est essentiel pour identifier les vrais problèmes de performance
- Ne pas optimiser prématurément : mesurer d'abord, optimiser ensuite
- Les optimisations doivent être validées par des mesures concrètes