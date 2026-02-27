---
name: Refactorisation complète et optimisation des performances
overview: Plan complet de refactorisation incluant la création de composants réutilisables, la centralisation des dimensions dans PluginDimensions.h avec sous-namespaces, la refactorisation des widgets pour accepter des largeurs en pixels, l'application de la convention de nommage avec underscore suffix, et l'optimisation des performances.
todos:
  - id: create-plugin-dimensions
    content: Créer PluginDimensions.h dans Source/Shared/ avec sous-namespaces (Heights, ButtonWidths, ComboBoxWidths, etc.)
    status: completed
  - id: refactor-button
    content: Refactoriser Button pour accepter int width, supprimer kHeight privée, utiliser PluginDimensions et PluginDisplayNames
    status: completed
  - id: refactor-combobox
    content: Refactoriser ComboBox pour accepter int width, supprimer kHeight privée, utiliser PluginDimensions
    status: completed
  - id: refactor-label
    content: Refactoriser Label pour accepter int width, supprimer kHeight privée, utiliser PluginDimensions
    status: completed
  - id: refactor-grouplabel
    content: Refactoriser GroupLabel pour accepter int width, supprimer kHeight privée, utiliser PluginDimensions
    status: completed
  - id: refactor-moduleheader
    content: Refactoriser ModuleHeader pour accepter int width, supprimer kHeight privée, utiliser PluginDimensions
    status: completed
  - id: refactor-sectionheader
    content: Refactoriser SectionHeader pour accepter int width, supprimer kHeight privée, utiliser PluginDimensions
    status: completed
  - id: refactor-horizontalseparator
    content: Refactoriser HorizontalSeparator pour accepter int width, supprimer kHeight privée, utiliser PluginDimensions
    status: completed
  - id: apply-naming-convention
    content: Appliquer la convention de nommage avec underscore suffix à tous les fichiers (membres privés avec _, paramètres sans préfixe in)
    status: pending
  - id: update-cursorrules
    content: Mettre à jour .cursorrules avec la nouvelle convention de nommage
    status: pending
  - id: create-module-header-panel
    content: Créer ModuleHeaderPanel dans Source/GUI/ReusablePanels/ avec méthodes courtes et explicites
    status: pending
  - id: create-parameter-panel
    content: Créer ParameterPanel dans Source/GUI/ReusablePanels/ (support complet avec ParameterType et vide), méthodes courtes
    status: pending
  - id: refactor-dco1-panel
    content: Refactoriser Dco1Panel pour utiliser ModuleHeaderPanel et ParameterPanel avec méthodes courtes
    status: pending
  - id: refactor-other-modules
    content: Refactoriser les autres modules similaires (Dco2Panel, VcfVcaPanel, FmTrackPanel, RampPortamentoPanel, Env1/2/3Panel, Lfo1/2Panel, MidiPanel, VibratoPanel, MiscPanel)
    status: pending
  - id: create-modulation-bus-component
    content: Créer ModulationBusComponent héritant de juce::Component avec méthodes courtes
    status: pending
  - id: refactor-matrix-modulation-panel
    content: Refactoriser MatrixModulationPanel pour utiliser ModulationBusComponent avec méthodes courtes
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

# Plan détaillé de refactorisation complète et d'optimisation des performances

## Objectifs

1. **Clarté et maintenabilité** : Réduire la répétition de code dans les panels de modules
2. **Réutilisabilité** : Rendre les widgets génériques et réutilisables pour d'autres projets
3. **Centralisation** : Créer une source unique de vérité pour les dimensions (PluginDimensions.h) avec sous-namespaces
4. **Convention de nommage** : Adopter la convention Google C++ avec underscore suffix pour les membres privés
5. **Performance** : Améliorer le temps de chargement et la réactivité de l'interface
6. **Clean Code** : Méthodes courtes et explicites, pas de commentaires descriptifs du code

---

## Phase 0 : Préparation et refactorisation des widgets

### 0.1 Création de PluginDimensions.h

**Fichier à créer :**

- `Source/Shared/PluginDimensions.h`

**Structure avec sous-namespaces (inspirée de PluginIDs.h et PluginDisplayNames.h) :**

```cpp
#pragma once

// PluginDimensions.h
// Central file for all plugin widget and panel dimensions
// Contains ONLY constant dimension definitions - no code/logic
// Single source of truth for all plugin dimensions

namespace PluginDimensions
{

    // ============================================================================
    // Widget Heights
    // ============================================================================

    namespace Heights
    {
        inline constexpr int kButton = 20;
        inline constexpr int kComboBox = 20;
        inline constexpr int kLabel = 20;
        inline constexpr int kModuleHeader = 30;
        inline constexpr int kSectionHeader = 30;
        inline constexpr int kGroupLabel = 30;
        inline constexpr int kHorizontalSeparator = 5;
        inline constexpr int kSlider = 20;
        inline constexpr int kNumberBox = 20;
        inline constexpr int kModulationBusHeader = 30;
        inline constexpr int kVerticalSeparator = 730;
    }

    // ============================================================================
    // Button Widths
    // ============================================================================

    namespace ButtonWidths
    {
        inline constexpr int kInit = 20;
        inline constexpr int kCopy = 20;
        inline constexpr int kPaste = 20;
        inline constexpr int kPatchManagerBankSelect = 35;
        inline constexpr int kInternalPatchesUtility = 35;
        inline constexpr int kPatchManagerUnlock = 75;
        inline constexpr int kHeaderPanelTheme = 70;
    }

    // ============================================================================
    // ComboBox Widths
    // ============================================================================

    namespace ComboBoxWidths
    {
        inline constexpr int kPatchEditModule = 60;
        inline constexpr int kMasterEditModule = 60;
        inline constexpr int kMatrixModulationAmount = 60;
        inline constexpr int kMatrixModulationDestination = 105;
    }

    // ============================================================================
    // Label Widths
    // ============================================================================

    namespace LabelWidths
    {
        inline constexpr int kPatchEditModule = 90;
        inline constexpr int kMasterEditModule = 100;
        inline constexpr int kModulationBusNumber = 15;
        inline constexpr int kPatchManagerBankSelector = 75;
    }

    // ============================================================================
    // GroupLabel Widths
    // ============================================================================

    namespace GroupLabelWidths
    {
        inline constexpr int kInternalPatchesBrowser = 110;
        inline constexpr int kInternalPatchesUtility = 155;
        inline constexpr int kComputerPatchesBrowser = 140;
        inline constexpr int kComputerPatchesFolderAndFiles = 130;
    }

    // ============================================================================
    // ModuleHeader Widths
    // ============================================================================

    namespace ModuleHeaderWidths
    {
        inline constexpr int kPatchEditModule = 150;
        inline constexpr int kMasterEditModule = 160;
        inline constexpr int kPatchManagerModule = 275;
    }

    // ============================================================================
    // SectionHeader Widths
    // ============================================================================

    namespace SectionHeaderWidths
    {
        inline constexpr int kPatchEdit = 810;
        inline constexpr int kMatrixModulation = 275;
        inline constexpr int kPatchManager = 275;
        inline constexpr int kMasterEdit = 160;
    }

    // ============================================================================
    // HorizontalSeparator Widths
    // ============================================================================

    namespace SeparatorWidths
    {
        inline constexpr int kPatchEditModule = 150;
        inline constexpr int kMasterEditModule = 160;
        inline constexpr int kMatrixModulationBus = 275;
    }

    // ============================================================================
    // Panel Widths
    // ============================================================================

    namespace PanelWidths
    {
        inline constexpr int kPatchEditModule = 150;
        inline constexpr int kMasterEditModule = 160;
        inline constexpr int kMatrixModulation = 275;
    }

} // namespace PluginDimensions
```

**Avantages :**

- Structure claire avec sous-namespaces évitant les préfixes répétitifs
- Commentaires de séparation pour faciliter la lecture (comme dans PluginDescriptors)
- Dimensions séparées pour chaque widget (Init, Copy, Paste séparés)
- Toutes les hauteurs centralisées dans le namespace Heights

---

### 0.2 à 0.8 : Refactorisation des widgets

**Widgets concernés :**

1. `Button` (ButtonWidth enum)
2. `ComboBox` (ComboBoxWidth enum)
3. `Label` (LabelWidth enum)
4. `GroupLabel` (GroupWidth enum)
5. `ModuleHeader` (ModuleWidth enum)
6. `SectionHeader` (SectionWidth enum)
7. `HorizontalSeparator` (SeparatorWidth enum)

**Changements communs pour chaque widget :**

**Dans le fichier .h :**

- Supprimer l'enum `*Width` (ex: `ButtonWidth`, `ComboBoxWidth`, etc.)
- Modifier le constructeur pour accepter `int width` au lieu de l'enum
- Supprimer la méthode statique `getWidth(*Width)`
- Supprimer les constantes privées de largeur (ex: `kInitCopyPasteWidth`, `kNormalWidth`, etc.)
- Supprimer la constante privée `kHeight` (elle sera dans PluginDimensions)
- Supprimer la méthode statique `getHeight()` (remplacée par une méthode d'instance ou constante)
- Ajouter un membre privé `int width_;` (avec underscore suffix)
- Ajouter une méthode d'instance `int getWidth() const { return width_; }`
- Ajouter une méthode statique `static constexpr int getHeight() { return PluginDimensions::Heights::k*; }`

**Dans le fichier .cpp :**

- Mettre à jour l'implémentation du constructeur pour stocker `width_`
- Supprimer l'implémentation de `getWidth(*Width)`
- Mettre à jour `setSize()` pour utiliser `PluginDimensions::Heights::k*` au lieu de `kHeight`

**Dans tous les fichiers utilisateurs :**

- Remplacer les appels avec enum par des appels avec constantes de `PluginDimensions`
- Remplacer les appels à `Widget::getWidth(Enum)` par les constantes directement
- Utiliser `PluginDisplayNames::StandaloneWidgetDisplayNames::kShortInitLabel` au lieu de `"I"` hardcodé

**Exemple détaillé pour Button :**

**Avant (Button.h) :**

```cpp
class Button : public juce::Button
{
public:
    enum class ButtonWidth
    {
        InitCopyPaste,
        PatchManagerBankSelect,
        // ...
    };
    
    explicit Button(Theme& inTheme, ButtonWidth width, const juce::String& text = {});
    
    static constexpr int getWidth(ButtonWidth width)
    {
        switch (width)
        {
            case ButtonWidth::InitCopyPaste:
                return kInitCopyPasteWidth;
            // ...
        }
    }
    
    static constexpr int getHeight() { return kHeight; }
    
private:
    inline constexpr static int kInitCopyPasteWidth = 20;
    // ...
    inline constexpr static int kHeight = 20;
    Theme* theme = nullptr;
};
```

**Après (Button.h) :**

```cpp
class Button : public juce::Button
{
public:
    explicit Button(Theme& theme, int width, const juce::String& text = {});
    
    int getWidth() const { return width_; }
    static constexpr int getHeight() { return PluginDimensions::Heights::kButton; }
    
private:
    Theme* theme_ = nullptr;
    int width_;
};
```

**Avant (usage) :**

```cpp
button = std::make_unique<tss::Button>(theme, tss::Button::ButtonWidth::InitCopyPaste, "I");
const auto width = tss::Button::getWidth(tss::Button::ButtonWidth::InitCopyPaste);
```

**Après (usage) :**

```cpp
button = std::make_unique<tss::Button>(
    theme, 
    PluginDimensions::ButtonWidths::kInit,
    PluginDescriptors::StandaloneWidgetDisplayNames::kShortInitLabel);
const auto width = PluginDimensions::ButtonWidths::kInit;
```

**Note importante :** Tous les widgets doivent utiliser `PluginDisplayNames` pour les textes au lieu de strings hardcodées.

---

### 0.9 Application de la convention de nommage avec underscore suffix

**Portée :** Tous les fichiers du projet (widgets, panels, factories, etc.)

**Règles à appliquer :**

1. Membres privés : ajouter le suffix `_`

      - `theme` → `theme_`
      - `apvts` → `apvts_`
      - `dragStartValue` → `dragStartValue_`
      - `widgetFactory` → `widgetFactory_`

2. Paramètres de méthodes : supprimer le préfixe "in" ou "new"

      - `inTheme` → `theme`
      - `inParameterId` → `parameterId`
      - `inApvts` → `apvts`
      - `newText` → `text`

3. Initialisations dans les constructeurs :

      - `theme_(theme)` au lieu de `theme(inTheme)`
      - `apvts_(apvts)` au lieu de `apvts(inApvts)`

4. Assignations :

      - `theme_ = &theme;` au lieu de `theme = &inTheme;`
      - `labelText_ = newText;` au lieu de `labelText = newText;`

**Exemple complet de migration :**

**Avant :**

```cpp
class Dco1Panel : public juce::Component
{
public:
    Dco1Panel(tss::Theme& inTheme, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& inApvts)
        : theme(&inTheme), apvts(inApvts) {}
    
    void setTheme(tss::Theme& inTheme)
    {
        theme = &inTheme;
    }
    
private:
    tss::Theme* theme;
    juce::AudioProcessorValueTreeState& apvts;
    std::unique_ptr<tss::Label> dco1FrequencyLabel;
};
```

**Après :**

```cpp
class Dco1Panel : public juce::Component
{
public:
    Dco1Panel(tss::Theme& theme, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
        : theme_(&theme), apvts_(apvts) {}
    
    void setTheme(tss::Theme& theme)
    {
        theme_ = &theme;
    }
    
private:
    tss::Theme* theme_;
    juce::AudioProcessorValueTreeState& apvts_;
    std::unique_ptr<tss::Label> dco1FrequencyLabel_;
};
```

**Ordre d'application recommandé :**

1. Commencer par les widgets refactorisés
2. Puis les panels
3. Enfin les autres classes

---

### 0.10 Mise à jour des cursorrules

**Fichier à modifier :**

- `.cursorrules`

**Section à modifier :** "Conventions de nommage" (lignes 80-92)

**Changements exacts :**

**Avant :**

```
[Conventions de nommage]
- Variables & Méthodes : lowerCamelCase
 * Exemple : audioBuffer, processMidiEvents(), getSampleRate()
- Variables privées : pas de préfixe ni suffixe (underscore ou autre)
 * Exemple : apvts
- Constantes : kConstantName (private par défaut)
 * Exemple : kMaxBufferSize, kDefaultChannels
- Classes : PascalCase
 * Exemple : PluginProcessor, PluginEditor, AudioAnalyzer
- Éviter les underscore_case, sauf cas très particulier
- Paramètres de constructeur : préfixe "in" pour éviter le shadowing
 * Exemple : inParameterId, inReason, inWidgetId
 * Permet d'initialiser clairement les membres : parameterId(inParameterId)
```

**Après :**

```
[Conventions de nommage]
- Variables & Méthodes : lowerCamelCase
 * Exemple : audioBuffer, processMidiEvents(), getSampleRate()
- Variables privées : suffix underscore `_` (convention Google C++ Style Guide)
 * Exemple : theme_, apvts_, dragStartValue_
 * Permet d'éviter le shadowing avec les paramètres de méthodes
- Constantes : kConstantName (private par défaut)
 * Exemple : kMaxBufferSize, kDefaultChannels
- Classes : PascalCase
 * Exemple : PluginProcessor, PluginEditor, AudioAnalyzer
- Éviter les underscore_case, sauf cas très particulier
- Paramètres de méthodes : pas de préfixe (évite le shadowing grâce au suffix `_` des membres)
 * Exemple : parameterId, reason, widgetId
 * Permet d'initialiser clairement les membres : parameterId_(parameterId)
```

---

## Phase 1 : Création des composants réutilisables

### 1.1 Création de ModuleHeaderPanel

**Fichiers à créer :**

- `Source/GUI/ReusablePanels/ModuleHeaderPanel.h`
- `Source/GUI/ReusablePanels/ModuleHeaderPanel.cpp`

**Emplacement :** `Source/GUI/ReusablePanels/` (et non dans les Modules spécifiques) car il sera utilisé par les modules de Patch Edit ET Master Edit.

**Responsabilité :** Encapsuler le header du module et les boutons Init/Copy/Paste (ou seulement Init selon le module).

**Structure proposée :**

```cpp
class ModuleHeaderPanel : public juce::Component
{
public:
    ModuleHeaderPanel(Theme& theme, 
                     WidgetFactory& widgetFactory,
                     const juce::String& moduleId,
                     const juce::String& initWidgetId,
                     const juce::String& copyWidgetId = {},
                     const juce::String& pasteWidgetId = {});
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void setTheme(Theme& theme);
    
    static int getHeight();
    
private:
    Theme* theme_;
    std::unique_ptr<ModuleHeader> moduleHeader_;
    std::unique_ptr<Button> initButton_;
    std::unique_ptr<Button> copyButton_;
    std::unique_ptr<Button> pasteButton_;
    
    void setupModuleHeader(Theme& theme, WidgetFactory& widgetFactory, const juce::String& moduleId);
    void setupInitButton(Theme& theme, WidgetFactory& widgetFactory, const juce::String& initWidgetId);
    void setupCopyButton(Theme& theme, WidgetFactory& widgetFactory, const juce::String& copyWidgetId);
    void setupPasteButton(Theme& theme, WidgetFactory& widgetFactory, const juce::String& pasteWidgetId);
    void positionModuleHeader();
    void positionButtons();
};
```

**Layout dans resized() :**

- ModuleHeader à gauche, hauteur fixe
- Boutons alignés à droite (Paste, Copy, Init de droite à gauche)
- Utilise les constantes de `PluginDimensions` pour les largeurs
- Utilise `PluginDisplayNames` pour les textes des boutons

**Note :** Méthodes courtes et explicites (`setupModuleHeader`, `setupInitButton`, etc.) plutôt qu'une méthode longue avec commentaires.

---

### 1.2 Création de ParameterPanel

**Fichiers à créer :**

- `Source/GUI/ReusablePanels/ParameterPanel.h`
- `Source/GUI/ReusablePanels/ParameterPanel.cpp`

**Emplacement :** `Source/GUI/ReusablePanels/` (même raison que ModuleHeaderPanel)

**Responsabilité :** Encapsuler un paramètre avec son label, son widget (Slider ou ComboBox), et son séparateur.

**Structure proposée :**

**Deux constructeurs pour deux cas d'usage :**

**Cas 1 : ParameterPanel complet (avec paramètre)**

```cpp
ParameterPanel(Theme& theme,
               WidgetFactory& widgetFactory,
               const juce::String& parameterId,
               int labelWidth,
               int widgetWidth,
               PluginDescriptors::ParameterType parameterType,
               juce::AudioProcessorValueTreeState& apvts);
```

**Cas 2 : ParameterPanel vide (séparateur uniquement)**

```cpp
ParameterPanel(Theme& theme, int separatorWidth);
```

**Structure interne :**

```cpp
class ParameterPanel : public juce::Component
{
public:
    ParameterPanel(Theme& theme, WidgetFactory& widgetFactory, 
                 const juce::String& parameterId, int labelWidth, int widgetWidth,
                 PluginDescriptors::ParameterType parameterType, juce::AudioProcessorValueTreeState& apvts);
    
    explicit ParameterPanel(Theme& theme, int separatorWidth);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void setTheme(Theme& theme);
    
    static int getHeight();
    
private:
    Theme* theme_;
    
    std::unique_ptr<Label> label_;
    std::unique_ptr<Slider> slider_;
    std::unique_ptr<ComboBox> comboBox_;
    std::unique_ptr<HorizontalSeparator> separator_;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboBoxAttachment_;
    
    bool isEmpty_;
    
    void setupParameterPanel(Theme& theme, WidgetFactory& widgetFactory,
                            const juce::String& parameterId, int labelWidth, int widgetWidth,
                            PluginDescriptors::ParameterType parameterType, juce::AudioProcessorValueTreeState& apvts);
    void setupSeparatorOnly(Theme& theme, int separatorWidth);
    void setupLabel(Theme& theme, WidgetFactory& widgetFactory, const juce::String& parameterId);
    void setupSlider(Theme& theme, WidgetFactory& widgetFactory, const juce::String& parameterId, int widgetWidth, juce::AudioProcessorValueTreeState& apvts);
    void setupComboBox(Theme& theme, WidgetFactory& widgetFactory, const juce::String& parameterId, int widgetWidth, juce::AudioProcessorValueTreeState& apvts);
    void positionLabel();
    void positionWidget();
    void positionSeparator();
};
```

**Layout dans resized() :**

- Si complet : Label à gauche, widget (Slider/ComboBox) à droite du label, séparateur en dessous
- Si vide : uniquement le séparateur sur toute la largeur
- Utilise les constantes de `PluginDimensions` pour les largeurs

**Note :** Utilise `PluginDescriptors::ParameterType` (kInt pour Slider, kChoice pour ComboBox) au lieu d'un bool `isChoiceParameter`.

---

### 1.3 Création de ModulationBusComponent

**Fichiers à créer :**

- `Source/GUI/Panels/BodyPanel/MatrixModulationPanel/ModulationBusComponent.h`
- `Source/GUI/Panels/BodyPanel/MatrixModulationPanel/ModulationBusComponent.cpp`

**Emplacement :** Dans le même dossier que `MatrixModulationPanel` car spécifique à cette section.

**Responsabilité :** Encapsuler un bus de modulation complet comme un `juce::Component` réutilisable.

**Structure proposée :**

```cpp
class ModulationBusComponent : public juce::Component
{
public:
    ModulationBusComponent(Theme& theme,
                          WidgetFactory& widgetFactory,
                          int busNumber,
                          const ModulationBusParameterArrays& parameterArrays,
                          juce::AudioProcessorValueTreeState& apvts);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void setTheme(Theme& theme);
    
    static int getHeight();
    
private:
    Theme* theme_;
    
    std::unique_ptr<Label> busNumberLabel_;
    std::unique_ptr<ComboBox> sourceComboBox_;
    std::unique_ptr<Slider> amountSlider_;
    std::unique_ptr<ComboBox> destinationComboBox_;
    std::unique_ptr<Button> initButton_;
    std::unique_ptr<HorizontalSeparator> separator_;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sourceAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> destinationAttachment_;
    
    void setupWidgets(Theme& theme, WidgetFactory& widgetFactory,
                     int busNumber, const ModulationBusParameterArrays& parameterArrays,
                     juce::AudioProcessorValueTreeState& apvts);
    void setupBusNumberLabel(Theme& theme, WidgetFactory& widgetFactory, int busNumber);
    void setupSourceComboBox(Theme& theme, WidgetFactory& widgetFactory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts);
    void setupAmountSlider(Theme& theme, WidgetFactory& widgetFactory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts);
    void setupDestinationComboBox(Theme& theme, WidgetFactory& widgetFactory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts);
    void setupInitButton(Theme& theme, WidgetFactory& widgetFactory, const juce::String& initWidgetId);
    void setupSeparator(Theme& theme);
    void positionWidgetsRow();
    void positionSeparator();
};
```

**Layout dans resized() :**

- Ligne horizontale : Label (#) + Source ComboBox + Amount Slider + Destination ComboBox + Init Button
- Séparateur en dessous
- Utilise les constantes de `PluginDimensions` pour les largeurs
- Utilise `PluginDisplayNames` pour les textes

---

## Phase 2 : Refactorisation des panels existants

### 2.1 Refactorisation de Dco1Panel

**Fichiers à modifier :**

- `Source/GUI/Panels/BodyPanel/PatchEditPanel/TopPanel/Modules/Dco1Panel.h`
- `Source/GUI/Panels/BodyPanel/PatchEditPanel/TopPanel/Modules/Dco1Panel.cpp`

**Changements détaillés :**

**Dans Dco1Panel.h :**

- Supprimer tous les membres individuels (labels, sliders, comboboxes, separators, attachments)
- Remplacer par :
    - `std::unique_ptr<ModuleHeaderPanel> moduleHeaderPanel_;`
    - `std::vector<std::unique_ptr<ParameterPanel>> parameterPanels_;`
- Supprimer les méthodes helper `setupModuleHeader`, `setupInitCopyPasteButtons`, `setupIntParameterWithSlider`, `setupChoiceParameterWithComboBox`
- Ajouter une méthode helper `void createParameterPanels(Theme& theme, WidgetFactory& widgetFactory);`
- Appliquer la convention de nommage avec underscore suffix

**Dans Dco1Panel.cpp :**

**Constructeur (simplifié) :**

```cpp
Dco1Panel::Dco1Panel(Theme& theme, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : theme_(&theme)
    , apvts_(apvts)
{
    createModuleHeaderPanel(theme, widgetFactory);
    createParameterPanels(theme, widgetFactory);
    setSize(getWidth(), getHeight());
}

void Dco1Panel::createModuleHeaderPanel(Theme& theme, WidgetFactory& widgetFactory)
{
    moduleHeaderPanel_ = std::make_unique<ModuleHeaderPanel>(
        theme, widgetFactory,
        PluginDescriptors::ModuleIds::kDco1,
        PluginDescriptors::StandaloneWidgetIds::kDco1Init,
        PluginDescriptors::StandaloneWidgetIds::kDco1Copy,
        PluginDescriptors::StandaloneWidgetIds::kDco1Paste);
    addAndMakeVisible(*moduleHeaderPanel_);
}

void Dco1Panel::createParameterPanels(Theme& theme, WidgetFactory& widgetFactory)
{
    const auto labelWidth = PluginDimensions::LabelWidths::kPatchEditModule;
    const auto sliderWidth = PluginDimensions::ComboBoxWidths::kPatchEditModule;
    const auto comboBoxWidth = PluginDimensions::ComboBoxWidths::kPatchEditModule;
    const auto separatorWidth = PluginDimensions::SeparatorWidths::kPatchEditModule;
    
    addIntParameterPanel(theme, widgetFactory, PluginDescriptors::ParameterIds::kDco1Frequency, labelWidth, sliderWidth);
    addIntParameterPanel(theme, widgetFactory, PluginDescriptors::ParameterIds::kDco1FrequencyModByLfo1, labelWidth, sliderWidth);
    addChoiceParameterPanel(theme, widgetFactory, PluginDescriptors::ParameterIds::kDco1Sync, labelWidth, comboBoxWidth);
    addIntParameterPanel(theme, widgetFactory, PluginDescriptors::ParameterIds::kDco1PulseWidth, labelWidth, sliderWidth);
    addIntParameterPanel(theme, widgetFactory, PluginDescriptors::ParameterIds::kDco1PulseWidthModByLfo2, labelWidth, sliderWidth);
    addIntParameterPanel(theme, widgetFactory, PluginDescriptors::ParameterIds::kDco1WaveShape, labelWidth, sliderWidth);
    addChoiceParameterPanel(theme, widgetFactory, PluginDescriptors::ParameterIds::kDco1WaveSelect, labelWidth, comboBoxWidth);
    addChoiceParameterPanel(theme, widgetFactory, PluginDescriptors::ParameterIds::kDco1Levers, labelWidth, comboBoxWidth);
    addChoiceParameterPanel(theme, widgetFactory, PluginDescriptors::ParameterIds::kDco1KeyboardPortamento, labelWidth, comboBoxWidth);
    addChoiceParameterPanel(theme, widgetFactory, PluginDescriptors::ParameterIds::kDco1KeyClick, labelWidth, comboBoxWidth);
}

void Dco1Panel::addIntParameterPanel(Theme& theme, WidgetFactory& widgetFactory, const juce::String& parameterId, int labelWidth, int widgetWidth)
{
    parameterPanels_.push_back(std::make_unique<ParameterPanel>(
        theme, widgetFactory, parameterId, labelWidth, widgetWidth,
        PluginDescriptors::ParameterType::kInt, apvts_));
    addAndMakeVisible(*parameterPanels_.back());
}

void Dco1Panel::addChoiceParameterPanel(Theme& theme, WidgetFactory& widgetFactory, const juce::String& parameterId, int labelWidth, int widgetWidth)
{
    parameterPanels_.push_back(std::make_unique<ParameterPanel>(
        theme, widgetFactory, parameterId, labelWidth, widgetWidth,
        PluginDescriptors::ParameterType::kChoice, apvts_));
    addAndMakeVisible(*parameterPanels_.back());
}
```

**resized() (simplifié) :**

```cpp
void Dco1Panel::resized()
{
    int y = 0;
    
    positionModuleHeaderPanel(y);
    positionParameterPanels(y);
}

void Dco1Panel::positionModuleHeaderPanel(int& y)
{
    if (auto* header = moduleHeaderPanel_.get())
    {
        const auto headerHeight = ModuleHeaderPanel::getHeight();
        header->setBounds(0, y, getWidth(), headerHeight);
        y += headerHeight;
    }
}

void Dco1Panel::positionParameterPanels(int& y)
{
    for (auto& panel : parameterPanels_)
    {
        if (panel)
        {
            const auto panelHeight = ParameterPanel::getHeight();
            panel->setBounds(0, y, getWidth(), panelHeight);
            y += panelHeight;
        }
    }
}
```

**setTheme() (simplifié) :**

```cpp
void Dco1Panel::setTheme(Theme& theme)
{
    theme_ = &theme;
    propagateThemeToModuleHeaderPanel(theme);
    propagateThemeToParameterPanels(theme);
    repaint();
}

void Dco1Panel::propagateThemeToModuleHeaderPanel(Theme& theme)
{
    if (auto* header = moduleHeaderPanel_.get())
        header->setTheme(theme);
}

void Dco1Panel::propagateThemeToParameterPanels(Theme& theme)
{
    for (auto& panel : parameterPanels_)
    {
        if (panel)
            panel->setTheme(theme);
    }
}
```

**Résultat attendu :**

- `resized()` : ~160 lignes → ~20 lignes (avec méthodes helper courtes)
- `setTheme()` : ~107 lignes → ~10 lignes (avec méthodes helper courtes)
- Constructeur : ~66 lignes → ~15 lignes (+ méthodes helper courtes)

---

### 2.2 Refactorisation des autres modules similaires

**Modules à refactoriser (même pattern que Dco1Panel) :**

**TopPanel (Patch Edit) :**

- `Dco2Panel`
- `VcfVcaPanel`
- `FmTrackPanel`
- `RampPortamentoPanel`

**BottomPanel (Patch Edit) :**

- `Env1Panel`
- `Env2Panel`
- `Env3Panel`
- `Lfo1Panel`
- `Lfo2Panel`

**MasterEditPanel :**

- `MidiPanel`
- `VibratoPanel`
- `MiscPanel`

**Approche :** Appliquer le même pattern que pour Dco1Panel avec méthodes courtes et explicites.

---

### 2.3 Refactorisation de MatrixModulationPanel

**Fichiers à modifier :**

- `Source/GUI/Panels/BodyPanel/MatrixModulationPanel/MatrixModulationPanel.h`
- `Source/GUI/Panels/BodyPanel/MatrixModulationPanel/MatrixModulationPanel.cpp`

**Changements détaillés :**

**Dans MatrixModulationPanel.h :**

- Supprimer la struct `ModulationBus`
- Remplacer `std::array<ModulationBus, 10> modulationBuses;` par `std::array<std::unique_ptr<ModulationBusComponent>, 10> modulationBusComponents_;`
- Supprimer toutes les méthodes helper de création
- Ajouter une méthode helper `void createModulationBusComponents(Theme& theme, WidgetFactory& widgetFactory);`
- Appliquer la convention de nommage avec underscore suffix

**Dans MatrixModulationPanel.cpp :**

**Constructeur :**

```cpp
MatrixModulationPanel::MatrixModulationPanel(Theme& theme, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : theme_(&theme)
    , apvts_(apvts)
{
    createSectionHeader(theme, widgetFactory);
    createModulationBusHeader(theme);
    createInitAllBussesButton(theme);
    createModulationBusComponents(theme, widgetFactory);
    setSize(getWidth(), getHeight());
}

void MatrixModulationPanel::createModulationBusComponents(Theme& theme, WidgetFactory& widgetFactory)
{
    const auto parameterArrays = createModulationBusParameterArrays();
    for (int busNumber = 0; busNumber < PluginDescriptors::kModulationBusCount; ++busNumber)
    {
        modulationBusComponents_[busNumber] = std::make_unique<ModulationBusComponent>(
            theme, widgetFactory, busNumber, parameterArrays, apvts_);
        addAndMakeVisible(*modulationBusComponents_[busNumber]);
    }
}
```

**resized() (simplifié) :**

```cpp
void MatrixModulationPanel::resized()
{
    int y = 0;
    positionSectionHeader(y);
    positionModulationBusHeader(y);
    positionModulationBusComponents(y);
}

void MatrixModulationPanel::positionSectionHeader(int& y)
{
    if (auto* header = sectionHeader_.get())
    {
        header->setBounds(0, y, getWidth(), header->getHeight());
        y += header->getHeight();
    }
}

void MatrixModulationPanel::positionModulationBusHeader(int& y)
{
    if (auto* busHeader = modulationBusHeader_.get())
    {
        busHeader->setBounds(0, y, busHeader->getWidth(), busHeader->getHeight());
        positionInitAllBussesButton(y);
        y += busHeader->getHeight();
    }
}

void MatrixModulationPanel::positionModulationBusComponents(int& y)
{
    for (auto& busComponent : modulationBusComponents_)
    {
        if (busComponent)
        {
            const auto busHeight = ModulationBusComponent::getHeight();
            busComponent->setBounds(0, y, getWidth(), busHeight);
            y += busHeight;
        }
    }
}
```

**setTheme() (simplifié) :**

```cpp
void MatrixModulationPanel::setTheme(Theme& theme)
{
    theme_ = &theme;
    propagateThemeToSectionHeader(theme);
    propagateThemeToModulationBusHeader(theme);
    propagateThemeToInitAllBussesButton(theme);
    propagateThemeToModulationBusComponents(theme);
    repaint();
}

void MatrixModulationPanel::propagateThemeToModulationBusComponents(Theme& theme)
{
    for (auto& busComponent : modulationBusComponents_)
    {
        if (busComponent)
            busComponent->setTheme(theme);
    }
}
```

---

## Phase 3 : Optimisation des performances

### 3.1 Profiling avec Instruments

**Objectif :** Identifier les vrais goulots d'étranglement avant d'optimiser.

**Métriques à mesurer :**

- Temps de construction de `PluginEditor`
- Temps de construction de tous les panels individuels
- Temps de première exécution de `resized()`
- Temps de première exécution de `setTheme()`
- Temps de premier clic sur un bouton

---

### 3.2 Optimisation de WidgetFactory (si nécessaire)

**Optimisations potentielles (basées sur le profiling) :**

- Remplacer `std::map` par `std::unordered_map` si les lookups sont un problème
- Cache des widgets créés récemment (peu probable d'être bénéfique)

**Note :** Ne pas optimiser prématurément. Attendre les résultats du profiling.

---

### 3.3 Lazy Initialization des widgets (si nécessaire)

**Solutions possibles :**

- Option A : Création asynchrone après l'affichage initial
- Option B : Lazy loading basé sur la visibilité

**Approche recommandée :**

- Commencer par l'Option A si le profiling montre que la création de widgets est un problème
- Mesurer l'amélioration avec Instruments
- Si nécessaire, implémenter l'Option B

---

## Phase 4 : Tests et validation

### 4.1 Tests fonctionnels

**Checklist :**

- Tous les widgets fonctionnent correctement
- Les layouts sont corrects
- Les thèmes s'appliquent correctement
- La convention de nommage est appliquée partout
- Les paramètres sont correctement attachés à l'APVTS
- Les boutons Init/Copy/Paste fonctionnent correctement

### 4.2 Tests de performance

**Comparaisons avant/après :**

- Temps de chargement du plugin
- Réactivité de l'interface
- Temps d'exécution de `resized()` et `setTheme()`

---

## Ordre d'exécution recommandé

1. **Phase 0.1** : Créer PluginDimensions.h avec sous-namespaces
2. **Phase 0.2-0.8** : Refactoriser tous les widgets (Button, ComboBox, Label, GroupLabel, ModuleHeader, SectionHeader, HorizontalSeparator)
3. **Phase 0.9** : Appliquer la convention de nommage avec underscore suffix
4. **Phase 0.10** : Mettre à jour les cursorrules
5. **Phase 1.1-1.2** : Créer ModuleHeaderPanel et ParameterPanel
6. **Phase 2.1** : Refactoriser Dco1Panel pour utiliser les nouveaux composants
7. **Phase 2.2** : Refactoriser les autres modules similaires
8. **Phase 1.3 et 2.3** : Créer ModulationBusComponent et refactoriser MatrixModulationPanel
9. **Phase 3.1** : Profiler avec Instruments
10. **Phase 3.2-3.3** : Appliquer les optimisations identifiées
11. **Phase 4** : Tests et validation

---

## Points d'attention

1. Ordre critique : La Phase 0 doit être complétée avant la Phase 1
2. Tests incrémentaux : Tester après chaque widget refactorisé
3. Convention de nommage : Appliquer progressivement, fichier par fichier
4. PluginDimensions.h : Créer en premier avec sous-namespaces
5. Profiling : Essentiel avant d'optimiser
6. Clean Code : Méthodes courtes et explicites, pas de commentaires descriptifs
7. Strings hardcodées : Toujours utiliser PluginDisplayNames
8. ParameterType : Utiliser `PluginDescriptors::ParameterType` au lieu de bool