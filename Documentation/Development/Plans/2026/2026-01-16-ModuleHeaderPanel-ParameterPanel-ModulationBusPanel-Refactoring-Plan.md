---
name: Refactorisation avec ModuleHeaderPanel, ParameterPanel et ModulationBusPanel
overview: "Implémentation de trois classes intermédiaires pour éliminer la répétition de code dans les panneaux existants : ModuleHeaderPanel (header + boutons I/C/P ou I seul), ParameterPanel (Label + Slider/ComboBox + Separator), et ModulationBusPanel (composant réutilisable pour chaque bus de modulation)."
todos:
  - id: create-parameter-panel
    content: Créer ParameterPanel (h/cpp) avec support Slider et ComboBox, gestion du layout et setTheme
    status: completed
  - id: create-module-header-panel
    content: Créer ModuleHeaderPanel (h/cpp) avec support ButtonSet::InitCopyPaste et ButtonSet::InitOnly
    status: completed
  - id: refactor-vcfvca-panel
    content: Refactoriser VcfVcaPanel pour utiliser ModuleHeaderPanel et ParameterPanel (validation de l approche)
    status: completed
    dependencies:
      - create-parameter-panel
      - create-module-header-panel
  - id: refactor-dco-panels
    content: Refactoriser Dco1Panel et Dco2Panel pour utiliser ModuleHeaderPanel (avec I/C/P) et ParameterPanel
    status: completed
    dependencies:
      - refactor-vcfvca-panel
  - id: create-modulation-bus-panel
    content: Créer ModulationBusPanel (h/cpp) avec tous les widgets d un bus de modulation
    status: completed
  - id: refactor-matrix-modulation-panel
    content: Refactoriser MatrixModulationPanel pour utiliser ModulationBusPanel au lieu de la structure ModulationBus
    status: completed
    dependencies:
      - create-modulation-bus-panel
  - id: refactor-remaining-panels
    content: Refactoriser les autres panneaux (Env1Panel, Env2Panel, Env3Panel, Lfo1Panel, Lfo2Panel, FmTrackPanel, RampPortamentoPanel)
    status: completed
    dependencies:
      - refactor-dco-panels
---

# Plan de refactorisation : ModuleHeaderPanel, ParameterPanel et ModulationBusPanel

## Objectif

Implémenter trois classes intermédiaires pour éliminer la répétition de code dans les panneaux existants et améliorer la maintenabilité.

## Structure des nouvelles classes

### 1. ModuleHeaderPanel

**Fichiers :** `Source/GUI/Panels/Reusable/ModuleHeaderPanel.h` et `.cpp`

**Responsabilité :** Encapsuler le header de module avec ses boutons (I/C/P ou I seul).

**Interface :**

```cpp
class ModuleHeaderPanel : public juce::Component
{
public:
    enum class ButtonSet { InitCopyPaste, InitOnly };
    
    ModuleHeaderPanel(tss::Theme& theme,
                     WidgetFactory& factory,
                     const juce::String& moduleId,
                     ButtonSet buttonSet,
                     juce::AudioProcessorValueTreeState& apvts);
    
    void resized() override;
    void setTheme(tss::Theme& theme);
    static int getHeight() { return PluginDimensions::Widgets::Heights::kModuleHeader; }
};
```

**Composition :**

- `std::unique_ptr<tss::ModuleHeader>` pour le header
- `std::unique_ptr<tss::Button>` pour Init (toujours présent)
- `std::unique_ptr<tss::Button>` pour Copy (optionnel, nullptr si InitOnly)
- `std::unique_ptr<tss::Button>` pour Paste (optionnel, nullptr si InitOnly)

**Layout :** Header à gauche, boutons alignés à droite (I, puis C, puis P si présents).

### 2. ParameterPanel

**Fichiers :** `Source/GUI/Panels/Reusable/ParameterPanel.h` et `.cpp`

**Responsabilité :** Encapsuler un paramètre avec son label, son widget (Slider ou ComboBox) et son séparateur.

**Interface :**

```cpp
class ParameterPanel : public juce::Component
{
public:
    enum class ParameterType { Slider, ComboBox };
    
    ParameterPanel(tss::Theme& theme,
                   WidgetFactory& factory,
                   const juce::String& parameterId,
                   ParameterType type,
                   juce::AudioProcessorValueTreeState& apvts);
    
    void resized() override;
    void setTheme(tss::Theme& theme);
    int getTotalHeight() const; // labelHeight + separatorHeight
};
```

**Composition :**

- `std::unique_ptr<tss::Label>` pour le label
- `std::unique_ptr<tss::Slider>` OU `std::unique_ptr<tss::ComboBox>` (selon le type)
- `std::unique_ptr<tss::HorizontalSeparator>` pour le séparateur
- Attachments appropriés (SliderAttachment ou ComboBoxAttachment)

**Layout :** Label à gauche, widget à droite sur la même ligne, séparateur en dessous.

### 3. ModulationBusPanel

**Fichiers :** `Source/GUI/Panels/Reusable/ModulationBusPanel.h` et `.cpp`

**Responsabilité :** Encapsuler un bus de modulation complet avec tous ses widgets.

**Interface :**

```cpp
class ModulationBusPanel : public juce::Component
{
public:
    ModulationBusPanel(int busNumber,
                      WidgetFactory& factory,
                      tss::Theme& theme,
                      juce::AudioProcessorValueTreeState& apvts,
                      const juce::String& sourceParamId,
                      const juce::String& amountParamId,
                      const juce::String& destinationParamId,
                      const juce::String& busId);
    
    void paint(juce::Graphics&) override;
    void resized() override;
    void setTheme(tss::Theme& theme);
    static int getHeight(); // labelHeight + separatorHeight
};
```

**Composition :**

- `std::unique_ptr<tss::Label>` pour le numéro de bus
- `std::unique_ptr<tss::ComboBox>` pour Source
- `std::unique_ptr<tss::Slider>` pour Amount
- `std::unique_ptr<tss::ComboBox>` pour Destination
- `std::unique_ptr<tss::Button>` pour Init
- `std::unique_ptr<tss::HorizontalSeparator>` pour le séparateur
- Attachments appropriés

**Layout :** Ligne horizontale avec tous les widgets, séparateur en dessous.

## Modifications des panneaux existants

### Dco1Panel et Dco2Panel

**Changements :**

1. Remplacer `dco1ModuleHeader_` et les boutons I/C/P par un `std::unique_ptr<ModuleHeaderPanel> moduleHeaderPanel_`
2. Remplacer chaque triplet (label + slider/comboBox + separator) par un `std::vector<std::unique_ptr<ParameterPanel>> parameterPanels_`
3. Simplifier `resized()` pour utiliser les nouvelles classes
4. Simplifier `setTheme()` pour déléguer aux nouvelles classes
5. Supprimer les méthodes helper `setupModuleHeader()`, `setupInitCopyPasteButtons()`, `setupIntParameterWithSlider()`, `setupChoiceParameterWithComboBox()`

**Nouveau `resized()` :**

```cpp
void Dco1Panel::resized()
{
    auto bounds = getLocalBounds();
    
    if (auto* header = moduleHeaderPanel_.get())
        header->setBounds(bounds.removeFromTop(header->getHeight()));
    
    for (auto* paramPanel : parameterPanels_)
        if (paramPanel != nullptr)
            paramPanel->setBounds(bounds.removeFromTop(paramPanel->getTotalHeight()));
}
```

### VcfVcaPanel et autres panneaux avec Init seul

**Changements similaires** mais avec `ButtonSet::InitOnly` pour `ModuleHeaderPanel`.

### MatrixModulationPanel

**Changements :**

1. Remplacer `std::array<ModulationBus, 10>` par `std::array<ModulationBusPanel, 10>`
2. Simplifier `resized()` pour itérer sur les `ModulationBusPanel`
3. Supprimer les méthodes helper de création de bus individuels
4. Simplifier `setTheme()` pour déléguer aux `ModulationBusPanel`

**Nouveau `resized()` :**

```cpp
void MatrixModulationPanel::resized()
{
    auto bounds = getLocalBounds();
    
    if (auto* header = sectionHeader_.get())
        header->setBounds(bounds.removeFromTop(header->getHeight()));
    
    if (auto* busHeader = modulationBusHeader_.get())
        busHeader->setBounds(bounds.removeFromTop(busHeader->getHeight()));
    
    // Position initAllBussesButton...
    
    for (auto& bus : modulationBuses_)
        bus.setBounds(bounds.removeFromTop(bus.getHeight()));
}
```

## Organisation des fichiers

Créer un nouveau dossier `Source/GUI/Panels/Reusable/` pour les classes réutilisables :

- `ModuleHeaderPanel.h` / `.cpp`
- `ParameterPanel.h` / `.cpp`
- `ModulationBusPanel.h` / `.cpp`

## Ordre d'implémentation

1. **Créer `ParameterPanel`** (le plus simple, utilisé partout)
2. **Créer `ModuleHeaderPanel`** (utilisé par tous les panneaux de modules)
3. **Refactoriser un panneau simple** (ex: VcfVcaPanel) pour valider l'approche
4. **Refactoriser Dco1Panel et Dco2Panel** (avec I/C/P)
5. **Créer `ModulationBusPanel`**
6. **Refactoriser MatrixModulationPanel**
7. **Refactoriser les autres panneaux** (Env1Panel, Env2Panel, Env3Panel, Lfo1Panel, Lfo2Panel, etc.)

## Notes importantes

- Respecter les conventions de nommage (lowerCamelCase pour variables, PascalCase pour classes)
- Utiliser les constantes de `PluginDimensions` existantes
- Maintenir la compatibilité avec `WidgetFactory` et `Theme`
- Les attachments APVTS doivent être créés dans les constructeurs des nouvelles classes
- Tester chaque étape avant de passer à la suivante