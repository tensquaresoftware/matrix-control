---
name: Intégration Look et scaling panels
overview: Corriger les erreurs de compilation en adaptant WidgetFactory et tous les panels pour utiliser les widgets refactorisés avec Look au lieu d'ISkin, et propager le scaling factor partout.
todos: []
isProject: false
---

# Integration Look et Scaling dans Matrix-Control

## Etat actuel

Les widgets suivants sont refactorises avec Look et scaling :

- [Button](Source/GUI/Widgets/Button.h), [Slider](Source/GUI/Widgets/Slider.h), [Label](Source/GUI/Widgets/Label.h)
- [HorizontalSeparator](Source/GUI/Widgets/HorizontalSeparator.h), [ComboBox](Source/GUI/Widgets/ComboBox.h)
- [PopupMenuBase](Source/GUI/Widgets/PopupMenuBase.h), [PopupMenuRenderer](Source/GUI/Widgets/PopupMenuRenderer.h)
- [ScrollablePopupMenu](Source/GUI/Widgets/ScrollablePopupMenu.h), [MultiColumnPopupMenu](Source/GUI/Widgets/MultiColumnPopupMenu.h)

**Probleme** : Le code ne compile pas car WidgetFactory et les panels utilisent encore les anciens constructeurs avec `ISkin&`.

## Plan d'action

### 1. Corriger WidgetFactory

Fichier : [WidgetFactory.cpp](Source/GUI/Factories/WidgetFactory.cpp)

Modifier les methodes de creation pour :

1. Creer les widgets sans ISkin (nouveaux constructeurs)
2. Appeler `setLook(xxxLookFromSkin(skin))` juste apres creation
3. Ajouter `#include "GUI/Looks/LookBuilders.h"` en tete

Exemple transformation :

```cpp
// Avant
auto slider = std::make_unique<tss::Slider>(skin, width, height, defaultValue);

// Apres
auto slider = std::make_unique<tss::Slider>(width, height, defaultValue);
slider->setLook(tss::sliderLookFromSkin(skin));
```

### 2. Adapter HeaderPanel (deja partiellement fait)

Fichier : [HeaderPanel.cpp](Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp)

Dans `resized()`, ajouter appels `setScalingFactor` sur les widgets :

```cpp
skinLabel_.setScalingFactor(scalingFactor_);
skinComboBox_.setScalingFactor(scalingFactor_);
guiScaleLabel_.setScalingFactor(scalingFactor_);
guiScaleComboBox_.setScalingFactor(scalingFactor_);
```

Dans constructeur et `setSkin`, remplacer creation avec `ISkin&` par creation + `setLook`.

### 3. Adapter tous les panels avec widgets

Pour chaque panel listant des widgets membres (Label, Button, Slider, ComboBox, HorizontalSeparator) :

**Fichiers concernes** (7 principaux) :

- [ParameterPanel](Source/GUI/Panels/Reusable/ParameterPanel.cpp)
- [ModulationBusPanel](Source/GUI/Panels/Reusable/ModulationBusPanel.cpp)
- [ModuleHeaderPanel](Source/GUI/Panels/Reusable/ModuleHeaderPanel.cpp)
- [InternalPatchesPanel](Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/InternalPatchesPanel.cpp)
- [ComputerPatchesPanel](Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/ComputerPatchesPanel.cpp)
- [BankUtilityPanel](Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/BankUtilityPanel.cpp)
- [PatchMutatorPanel](Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp)

**Actions pour chaque panel** :

1. **Ajouter includes** :

```cpp
#include "GUI/Looks/LookBuilders.h"
```

1. **Dans le constructeur**, remplacer creation widgets :

```cpp
// Avant
label_ = std::make_unique<tss::Label>(skin, width, height, text);

// Apres
label_ = std::make_unique<tss::Label>(width, height, text);
addAndMakeVisible(*label_);
label_->setLook(tss::labelLookFromSkin(skin));
```

1. **Dans `setSkin`**, remplacer `propagateSkin` par `setLook` :

```cpp
// Avant
tss::propagateSkin(skin, label_.get(), slider_.get());

// Apres
if (label_) label_->setLook(tss::labelLookFromSkin(skin));
if (slider_) slider_->setLook(tss::sliderLookFromSkin(skin));
```

1. **Dans `resized`**, ajouter appels `setScalingFactor` sur widgets :

```cpp
if (label_) label_->setScalingFactor(scalingFactor_);
if (slider_) slider_->setScalingFactor(scalingFactor_);
```

### 4. Propager scaling dans hierarchie panels

Les panels intermediaires doivent propager le scale aux sous-panels ET widgets.

**Ordre hierarchique** :

1. MainComponent → HeaderPanel, BodyPanel, FooterPanel (deja fait)
2. BodyPanel → PatchEditPanel, MatrixModulationPanel, PatchManagerPanel, MasterEditPanel
3. Chaque panel intermediaire → ses sous-panels et widgets

**Pour chaque panel intermediaire** :

1. Ajouter membre si absent : `float scalingFactor_ = 1.0f;`
2. Ajouter methode si absente : `void setScalingFactor(float scalingFactor);`
3. Dans `setScalingFactor`, propager aux enfants (panels ET widgets)
4. Dans `resized`, appeler `setScalingFactor` sur les widgets directs

### 5. Gestion PopupMenuRenderer

Le `PopupMenuRenderer` dans [PopupMenuBase.cpp](Source/GUI/Widgets/PopupMenuBase.cpp) doit recevoir le Look.

Ajouter dans le constructeur de `PopupMenuBase` :

```cpp
#include "GUI/Looks/LookBuilders.h"
// Dans le constructeur, apres initialisation du renderer
// Note: On doit passer un skin temporaire ou reconstruire le Look
// Solution : le popup devrait recevoir le Look depuis le ComboBox
```

**Alternative plus simple** : Ne pas utiliser Look dans les popups pour l'instant, garder les couleurs en dur comme dans Test-GuiScaling.

### 6. Widgets non encore refactorises

Ces widgets utilisent encore ISkin et cache, **ne pas les toucher pour l'instant** :

- Toggle, VerticalSeparator, ModuleHeader, GroupLabel, SectionHeader
- NumberBox, EnvelopeDisplay, PatchNameDisplay, TrackGeneratorDisplay, ModulationBusHeader

Ces widgets continueront a fonctionner avec `ISkin&` jusqu'a refactorisation ulterieure.

## Ordre d'execution

1. **WidgetFactory** : corriger creation widgets (bloque compilation)
2. **HeaderPanel** : finaliser (ajout setScalingFactor dans resized)
3. **Panels avec widgets** : corriger les 7 panels principaux un par un
4. **Propagation scaling** : ajouter setScalingFactor dans panels intermediaires
5. **Test compilation** : verifier que tout compile
6. **Test runtime** : verifier que le plugin se charge et fonctionne

## Fichiers cles a modifier


| Priorite | Fichier                  | Action                                |
| -------- | ------------------------ | ------------------------------------- |
| 1        | WidgetFactory.cpp        | Adapter creation widgets + setLook    |
| 2        | HeaderPanel.cpp          | Ajouter setScalingFactor dans resized |
| 3        | ParameterPanel.cpp       | Creation + setSkin + resized          |
| 4        | ModulationBusPanel.cpp   | Creation + setSkin + resized          |
| 5        | ModuleHeaderPanel.cpp    | Creation + setSkin + resized          |
| 6        | InternalPatchesPanel.cpp | Creation + setSkin + resized          |
| 7        | ComputerPatchesPanel.cpp | Creation + setSkin + resized          |
| 8        | BankUtilityPanel.cpp     | Creation + setSkin + resized          |
| 9        | PatchMutatorPanel.cpp    | Creation + setSkin + resized          |
| 10+      | Panels intermediaires    | Propagation setScalingFactor          |


## Notes importantes

- Les PopupMenuRenderer garderont les couleurs en dur pour simplifier (comme Test-GuiScaling)
- Les widgets non refactorises (Toggle, NumberBox, etc.) continueront avec ISkin
- Le scaling sera completement fonctionnel une fois tous les panels adaptes
- L'AffineTransform a deja ete supprime, le rendu sera net des la compilation reussie

