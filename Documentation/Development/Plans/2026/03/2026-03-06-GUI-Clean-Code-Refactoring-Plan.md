---
name: Refactorisation Clean Code GUI
overview: Plan de refactorisation pour éliminer les violations DRY, SOLID et Clean Architecture identifiées dans la codebase GUI/Shared, en préparation de la fonctionnalité zoom.
todos:
  - id: phase1-exceptions
    content: Déplacer WidgetFactoryExceptions vers Source/Shared/Exceptions/
    status: completed
  - id: phase2-skin-init
    content: Factoriser initializeBlackVariantColours/initializeCreamVariantColours avec lambda
    status: completed
  - id: phase2-remove-getters
    content: Supprimer getters redondants Skin et migrer widgets vers getColour()
    status: completed
  - id: phase3-iskin
    content: Créer ISkin, faire implémenter par Skin, créer propagateSkin
    status: completed
  - id: phase3-migrate-widgets
    content: Migrer widgets/panels vers ISkin& et utiliser propagateSkin
    status: completed
  - id: phase4-headerpanel
    content: Éliminer magic numbers HeaderPanel (Skin/Zoom enums, kLeftPadding_)
    status: completed
  - id: phase5-toppanel
    content: Factoriser TopPanel::resized() avec tableau et boucle
    status: completed
  - id: phase6-buttonwidth
    content: Ajouter buttonWidth à StandaloneWidgetDescriptor et simplifier WidgetFactory
    status: completed
  - id: phase7-parameterpanel
    content: Implémenter getDimensionsForModuleType() dans ParameterPanel
    status: completed
  - id: phase8-skincolours
    content: Séparer SkinColours.h en fichiers par catégorie
    status: completed
isProject: false
---

# Plan de refactorisation Clean Code - GUI et Shared

## Réponses aux questions

### 2.4 Skin IDs et emplacement des constantes

**ColourVariant / SkinComboBoxItemId** : Option A retenue. Déclarer `enum class SkinComboBoxItemId : int { kBlack = 1, kCream = 2 };` dans [Source/GUI/Skins/Skin.h](Source/GUI/Skins/Skin.h), **immédiatement après le bloc ColourVariant** (lignes 15-19), en enum imbriqué dans la classe `Skin`. Référence : `tss::Skin::SkinComboBoxItemId`. Cet enum ne va pas dans PluginDisplayNames.h car il s'agit d'IDs, pas de noms d'affichage.

**Skin — Noms d'affichage** : Créer `ChoiceLists::SkinVariants` dans PluginDisplayNames.h avec `kBlack = "BLACK"` et `kCream = "CREAM"`.

**Zoom — Noms d'affichage** : Dans [Source/Shared/Definitions/PluginDisplayNames.h](Source/Shared/Definitions/PluginDisplayNames.h), créer le namespace `ZoomLevels` **à l'intérieur de `ChoiceLists`**, **juste avant** le sous-namespace `MidiChannel`. Contenu : `k50 = "50%"`, `k75 = "75%"`, `k90 = "90%"`, `k100 = "100%"`, `k125 = "125%"`, `k150 = "150%"`, `k200 = "200%"`.

**Zoom — IDs** : Dans [Source/Shared/Definitions/PluginIDs.h](Source/Shared/Definitions/PluginIDs.h), créer le namespace `Settings` **à l'intérieur de `PluginIDs`**, **juste avant** le namespace `Mode`. Dans `Settings`, créer le sous-namespace `ZoomLevels` pour y stocker les IDs : `k50 = 1`, `k75 = 2`, `k90 = 3`, `k100 = 4`, `k125 = 5`, `k150 = 6`, `k200 = 7`.

### 4.3 ComboBox addItem — Enum vs getNumItems()+1

**Cas 1 — Choix fixes (Skin, Zoom)** : Retenu. Les options sont connues à la compilation. Utiliser des enums/constantes :

```cpp
// Skin.h — SkinComboBoxItemId près de ColourVariant
enum class SkinComboBoxItemId : int { kBlack = 1, kCream = 2 };

// HeaderPanel — Skin ComboBox
skinComboBox_.addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kBlack, 
                     static_cast<int>(tss::Skin::SkinComboBoxItemId::kBlack));
skinComboBox_.addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kCream, 
                     static_cast<int>(tss::Skin::SkinComboBoxItemId::kCream));

// HeaderPanel — Zoom ComboBox (IDs dans PluginIDs::Settings::ZoomLevels)
zoomComboBox_.addItem(PluginDisplayNames::ChoiceLists::ZoomLevels::k50, 
                     PluginIDs::Settings::ZoomLevels::k50);
// ... etc.
```

**Cas 2 — Choix dynamiques (descripteurs)** : Boucle avec `itemId++` pour clarté (pas de changement fonctionnel).

### 3.3 ISkin et pattern setSkin()

**ISkin** = interface du fournisseur de skin (couleurs, polices). **ISkinnable** = interface des composants qui reçoivent un skin.

- **ISkin** : découple les widgets de la classe `Skin` (réutilisabilité, tests avec mocks).
- **setSkin()** : propagation du skin aux enfants.

Ces deux aspects sont complémentaires. **Implémenter ISkin règle les deux problématiques** : découplage des widgets ET factorisation du pattern setSkin() via le helper `propagateSkin()`.

En introduisant ISkin :

1. Les widgets dépendent de `ISkin&` au lieu de `tss::Skin&`.
2. Un helper `propagateSkin(ISkin& skin, T*... components)` peut factoriser les appels répétés à `setSkin()`.

**Exemple :**

```cpp
// ISkin.h
class ISkin {
public:
    virtual juce::Colour getColour(SkinColourId) const = 0;
    virtual juce::Font getBaseFont() const = 0;
    virtual juce::Font getBaseFontBold() const = 0;
    // ... (méthodes avec logique conservées)
};

// Helper
template <typename... T>
void propagateSkin(ISkin& skin, T*... components) {
    ((components && (components->setSkin(skin), true)), ...);
}

// TopPanel::setSkin
void TopPanel::setSkin(ISkin& skin) {
    skin_ = &skin;
    propagateSkin(skin, dco1Panel_.get(), dco2Panel_.get(), vcfVcaPanel_.get(), 
                 fmTrackPanel_.get(), rampPortamentoPanel_.get());
}
```

`Skin` implémente `ISkin`. Les widgets passent de `setSkin(tss::Skin&)` à `setSkin(ISkin&)`.

---

## Phase 1 : Exceptions (Clean Architecture)

### 1.1 Créer Source/Shared/Exceptions/

- Créer le dossier `Source/Shared/Exceptions/`
- Créer [Source/Shared/Exceptions/WidgetFactoryExceptions.h](Source/Shared/Exceptions/WidgetFactoryExceptions.h) en déplaçant le contenu actuel de [Source/GUI/Exceptions/WidgetFactoryExceptions.h](Source/GUI/Exceptions/WidgetFactoryExceptions.h)

### 1.2 Mettre à jour les includes

- [Source/Core/Exceptions/ExceptionPropagator.h](Source/Core/Exceptions/ExceptionPropagator.h) : `#include "GUI/Exceptions/WidgetFactoryExceptions.h"` → `#include "Shared/Exceptions/WidgetFactoryExceptions.h"`
- [Source/GUI/Factories/WidgetFactoryValidator.h](Source/GUI/Factories/WidgetFactoryValidator.h) : `#include "GUI/Exceptions/WidgetFactoryExceptions.h"` → `#include "Shared/Exceptions/WidgetFactoryExceptions.h"`
- Mettre à jour CMakeLists.txt si nécessaire (aucune nouvelle source, uniquement déplacement)
- Supprimer [Source/GUI/Exceptions/WidgetFactoryExceptions.h](Source/GUI/Exceptions/WidgetFactoryExceptions.h) et le dossier `Source/GUI/Exceptions/` s'il est vide

---

## Phase 2 : Skin — DRY et suppression des getters

### 2.1 Factoriser l'initialisation des couleurs (Skin.cpp)

Remplacer `initializeBlackVariantColours()` et `initializeCreamVariantColours()` par une fonction template/lambda :

```cpp
template <typename Accessor>
void initializeVariantColours(Accessor accessColour)
{
    using namespace SkinColours;
    colours_[SkinColourId::kHeaderPanelBackground] = juce::Colour(accessColour(Panels::kHeaderPanelBackground));
    colours_[SkinColourId::kHeaderPanelLabelText] = juce::Colour(accessColour(Panels::kHeaderPanelLabelText));
    // ... (toutes les lignes, en utilisant accessColour(Element) au lieu de Element.blackVariant/creamVariant)
}

void Skin::initializeDefaultColours()
{
    if (currentVariant_ == ColourVariant::Black)
        initializeVariantColours([](const auto& el) { return el.blackVariant; });
    else
        initializeVariantColours([](const auto& el) { return el.creamVariant; });
}
```

Supprimer `initializeBlackVariantColours` et `initializeCreamVariantColours`.

### 2.2 Supprimer les getters redondants et migrer vers getColour()

**Getters à supprimer** (tous les wrappers 1:1 vers `getColour`) :

- Panels : `getHeaderPanelBackgroundColour`, `getHeaderPanelLabelTextColour`, `getBodyPanelBackgroundColour`, `getFooterPanelBackgroundColour`
- Section/Module/Group : `getSectionHeaderTextColour`, `getSectionHeaderLineColourBlue/Orange`, `getModuleHeaderTextColour`, etc.
- Widgets : `getLabelTextColour`, `getVerticalSeparatorLineColour`, `getHorizontalSeparatorLineColour`
- Button, Toggle, Slider, ComboBox, PopupMenu, NumberBox, EnvelopeDisplay, PatchNameDisplay, TrackGeneratorDisplay : tous les getters simples

**Getters à conserver** (logique avec paramètres) :

- `getToggleBackgroundColour(bool)`, `getToggleTextColour(bool)`
- `getSliderTrackColour(bool)`, `getSliderValueBarColour(bool)`, `getSliderTextColour(bool)`
- `getComboBoxBackgroundColour(bool, bool)`, `getComboBoxBorderColour(bool, bool)`, etc.
- `getPopupMenuBackgroundColour(bool)`, etc.
- `getButtonBackgroundColourOff/On`, etc. (utilisés avec des conditions dans Button.cpp)

**Fichiers à modifier** : Tous les widgets et panels qui appellent ces getters. Remplacer par `skin_->getColour(SkinColourId::kXxx)`.

---

## Phase 3 : Interface ISkin et helper setSkin()

### 3.1 Créer ISkin

- Créer [Source/GUI/Skins/ISkin.h](Source/GUI/Skins/ISkin.h) avec les méthodes utilisées par les widgets :
  - `getColour(SkinColourId)`, `getValue(SkinValueId)`
  - `getBaseFont()`, `getBaseFontBold()`
  - Méthodes avec logique conservées : `getToggleBackgroundColour(bool)`, `getComboBoxBackgroundColour(bool, bool)`, etc.

### 3.2 Faire implémenter ISkin par Skin

- [Source/GUI/Skins/Skin.h](Source/GUI/Skins/Skin.h) : `class Skin : public ISkin`
- Adapter les signatures pour respecter l’interface

### 3.3 Créer le helper propagateSkin

- Créer [Source/GUI/Skins/SkinHelpers.h](Source/GUI/Skins/SkinHelpers.h) (ou ajouter dans ISkin.h) :

```cpp
  template <typename... T>
  void propagateSkin(ISkin& skin, T*... components);
  

```

### 3.4 Migrer les widgets vers ISkin

- Remplacer `tss::Skin&` par `ISkin&` dans les signatures `setSkin()` et les membres `ISkin* skin_`
- Mettre à jour tous les widgets et panels concernés

### 3.5 Utiliser propagateSkin dans les panels

- TopPanel, BodyPanel, BottomPanel, ParameterPanel, BaseModulePanel, HeaderPanel, FooterPanel, etc.
- Remplacer les blocs répétitifs `if (auto* x = ...) x->setSkin(skin)` par des appels à `propagateSkin(skin, ...)`.

---

## Phase 4 : HeaderPanel — Magic numbers

### 4.1 Skin ComboBox

- Dans [Source/GUI/Skins/Skin.h](Source/GUI/Skins/Skin.h) : ajouter `enum class SkinComboBoxItemId : int { kBlack = 1, kCream = 2 };` **à proximité du bloc ColourVariant** (lignes 15-19), en enum imbriqué dans `Skin`
- Dans [Source/Shared/Definitions/PluginDisplayNames.h](Source/Shared/Definitions/PluginDisplayNames.h) : ajouter `namespace SkinVariants` **dans ChoiceLists** (avant MidiChannel) avec `kBlack = "BLACK"`, `kCream = "CREAM"`
- Dans [Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp](Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp) : ajouter `#include "Shared/Definitions/PluginDisplayNames.h"` ; utiliser `PluginDisplayNames::ChoiceLists::SkinVariants::kBlack/kCream` et `static_cast<int>(tss::Skin::SkinComboBoxItemId::kBlack/kCream)`

### 4.2 Zoom ComboBox

- Dans [Source/Shared/Definitions/PluginDisplayNames.h](Source/Shared/Definitions/PluginDisplayNames.h) : ajouter `namespace ZoomLevels` **dans ChoiceLists**, **juste avant MidiChannel**, avec `k50 = "50%"`, `k75 = "75%"`, `k90 = "90%"`, `k100 = "100%"`, `k125 = "125%"`, `k150 = "150%"`, `k200 = "200%"`
- Dans [Source/Shared/Definitions/PluginIDs.h](Source/Shared/Definitions/PluginIDs.h) : créer `namespace Settings` **dans PluginIDs**, **juste avant Mode** ; dans Settings, créer `namespace ZoomLevels` avec `k50 = 1`, `k75 = 2`, `k90 = 3`, `k100 = 4`, `k125 = 5`, `k150 = 6`, `k200 = 7`
- Dans HeaderPanel.cpp : ajouter `#include "Shared/Definitions/PluginIDs.h"` ; utiliser `PluginDisplayNames::ChoiceLists::ZoomLevels::kXxx` et `PluginIDs::Settings::ZoomLevels::kXxx`

### 4.3 kLeftPadding_

- Dans [Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h](Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h) : ajouter `inline constexpr static int kLeftPadding_ = 15;`
- Dans HeaderPanel.cpp : remplacer `int currentX = 15` par `int currentX = kLeftPadding`_

---

## Phase 5 : TopPanel::resized()

- Dans [Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/TopPanel/TopPanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/TopPanel/TopPanel.cpp) : factoriser avec un tableau de panels et une boucle :

```cpp
void TopPanel::resized()
{
    const auto bounds = getLocalBounds();
    std::array<juce::Component*, 5> panels = {
        dco1Panel_.get(), dco2Panel_.get(), vcfVcaPanel_.get(),
        fmTrackPanel_.get(), rampPortamentoPanel_.get()
    };
    int x = 0;
    for (auto* panel : panels)
    {
        if (panel)
            panel->setBounds(bounds.getX() + x, bounds.getY(), childModuleWidth_, childModuleHeight_);
        x += childModuleWidth_ + spacing_;
    }
}
```

---

## Phase 6 : buttonWidth dans StandaloneWidgetDescriptor

### 6.1 Modifier la structure

Dans [Source/Shared/Definitions/PluginDescriptors.h](Source/Shared/Definitions/PluginDescriptors.h), dans `StandaloneWidgetDescriptor` (lignes 92-98), ajouter le champ `buttonWidth` :

```cpp
struct StandaloneWidgetDescriptor
{
    juce::String widgetId;
    juce::String displayName;
    juce::String parentGroupId;
    StandaloneWidgetType widgetType;
    std::optional<int> buttonWidth;  // Ligne 97 : pour kButton uniquement
};
```

**Emplacement exact** : après `StandaloneWidgetType widgetType;` (ligne 97).

### 6.2 Renseigner buttonWidth dans PluginDescriptors.cpp

Pour chaque entrée `StandaloneWidgetDescriptor` de type `kButton`, ajouter `.buttonWidth = PluginDimensions::Widgets::Widths::Button::kXxx` selon le mapping actuel de `getButtonWidthForWidgetId()`. Pour les widgets non-bouton (kComboBox, kNumber, kLabel), laisser `std::nullopt` ou omettre le champ.

### 6.3 Simplifier WidgetFactory

- Supprimer `getButtonWidthForWidgetId()`
- Dans `createStandaloneButton()` : récupérer `buttonWidth` depuis le descripteur via `desc->buttonWidth.value_or(PluginDimensions::Widgets::Widths::Button::kInit)` (ou lever une exception si absent pour un bouton)

---

## Phase 7 : ParameterPanel — getDimensionsForModuleType()

### 7.1 Struct et méthode

Dans [Source/GUI/Panels/Reusable/ParameterPanel.h](Source/GUI/Panels/Reusable/ParameterPanel.h) :

```cpp
struct ParameterPanelDimensions
{
    int labelWidth;
    int comboBoxWidth;
    int separatorWidth;
};

ParameterPanelDimensions getDimensionsForModuleType(ModuleType moduleType) const;
```

### 7.2 Implémentation

Dans [Source/GUI/Panels/Reusable/ParameterPanel.cpp](Source/GUI/Panels/Reusable/ParameterPanel.cpp) : implémenter `getDimensionsForModuleType()` en centralisant les ternaires actuels, puis l’utiliser dans `createParameterLabel`, `createParameterWidget`, `createSeparator`, `layoutParameterLabel`, `layoutParameterWidget`, `layoutSeparator`.

---

## Phase 8 : SkinColours.h — Séparation par catégories

### 8.1 Structure des fichiers

Créer une structure inspirée de PluginDescriptors/PluginDisplayNames :

```
Source/GUI/Skins/
├── SkinColours/
│   ├── SkinColours.h          (include principal, inclut tous les sous-fichiers)
│   ├── ColourChart.h          (existant, déplacer si nécessaire)
│   ├── ColourElement.h        (struct ColourElement + namespace Common)
│   ├── Panels.h               (namespace Panels)
│   ├── SectionHeader.h        (namespace Widgets::SectionHeader)
│   ├── ModuleHeader.h         (namespace Widgets::ModuleHeader)
│   ├── GroupLabel.h
│   ├── Label.h
│   ├── Separators.h           (Vertical + Horizontal)
│   ├── Button.h
│   ├── Toggle.h
│   ├── Slider.h
│   ├── ComboBox.h             (Standard + ButtonLike)
│   ├── PopupMenu.h
│   ├── NumberBox.h
│   ├── EnvelopeDisplay.h
│   ├── PatchNameDisplay.h
│   └── TrackGeneratorDisplay.h
```

### 8.2 Migration

- Extraire chaque namespace dans son fichier
- [Source/GUI/Skins/SkinColours.h](Source/GUI/Skins/SkinColours.h) actuel devient le point d’entrée qui inclut tous les sous-fichiers
- Mettre à jour [Source/GUI/Skins/SkinColourId.h](Source/GUI/Skins/SkinColourId.h) (ou l’endroit où `SkinColourId` est défini) si les includes changent

---

## Ordre d’exécution recommandé

1. **Phase 1** (Exceptions) — indépendante
2. **Phase 4** (HeaderPanel) — indépendante, rapide
3. **Phase 2.1** (Skin init) — avant la suppression des getters
4. **Phase 2.2** (Suppression getters) — beaucoup de fichiers
5. **Phase 3** (ISkin + propagateSkin) — peut être faite avant ou après 2.2
6. **Phase 5** (TopPanel)
7. **Phase 6** (buttonWidth)
8. **Phase 7** (ParameterPanel)
9. **Phase 8** (SkinColours split) — plus lourd, à faire en dernier

---

## Fichiers impactés (résumé)


| Phase | Fichiers principaux                                                           |
| ----- | ----------------------------------------------------------------------------- |
| 1     | Shared/Exceptions/*, Core/ExceptionPropagator.h, GUI/WidgetFactoryValidator.h |
| 2     | Skin.cpp, Skin.h, ~25 widgets/panels                                          |
| 3     | ISkin.h, Skin.h, SkinHelpers.h, ~30 panels/widgets                            |
| 4     | HeaderPanel.h/cpp, PluginDisplayNames.h, Skin.h                               |
| 5     | TopPanel.cpp                                                                  |
| 6     | PluginDescriptors.h/cpp, WidgetFactory.cpp                                    |
| 7     | ParameterPanel.h/cpp                                                          |
| 8     | SkinColours/* (nouveaux fichiers), SkinColours.h                              |

---

## Confirmation — Informations complètes

Le plan intègre toutes les instructions de Guillaume :

- **SkinComboBoxItemId** : Skin.h, près de ColourVariant, enum imbriqué dans Skin
- **Zoom Display Names** : PluginDisplayNames.h → ChoiceLists::ZoomLevels (avant MidiChannel)
- **Zoom IDs** : PluginIDs.h → Settings::ZoomLevels (avant Mode)
- **Skin Display Names** : PluginDisplayNames.h → ChoiceLists::SkinVariants
- **ISkin** : implémentation pour découplage des widgets ET factorisation setSkin()
- **4.3** : Cas 1 (enums) pour Skin et Zoom


