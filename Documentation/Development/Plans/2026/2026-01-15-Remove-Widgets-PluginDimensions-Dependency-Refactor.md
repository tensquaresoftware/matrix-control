---
name: Refactoriser widgets pour supprimer dépendance PluginDimensions
overview: Refactoriser tous les widgets pour supprimer leur dépendance à PluginDimensions.h en ajoutant des paramètres width et height dans leurs constructeurs, et en déplaçant la responsabilité de fournir ces dimensions vers les panneaux qui les construisent.
todos:
  - id: "1"
    content: "Modifier Label.h/cpp : supprimer PluginDimensions.h, ajouter paramètre height et membre height_, modifier getHeight() et setSize()"
    status: completed
  - id: "2"
    content: "Modifier Button.h/cpp : supprimer PluginDimensions.h, ajouter paramètre height et membre height_, modifier getHeight() et setSize()"
    status: completed
  - id: "3"
    content: "Modifier SectionHeader.h/cpp : supprimer PluginDimensions.h, ajouter paramètre height et membre height_, modifier getHeight() et setSize()"
    status: completed
  - id: "4"
    content: "Modifier ModuleHeader.h/cpp : supprimer PluginDimensions.h, retirer valeur par défaut de width, ajouter paramètre height et membre height_, modifier getHeight() et setSize()"
    status: completed
  - id: "5"
    content: "Modifier ComboBox.h/cpp : supprimer PluginDimensions.h, retirer valeur par défaut de width, ajouter paramètre height et membre height_, modifier getHeight() et setSize()"
    status: completed
  - id: "6"
    content: "Modifier GroupLabel.h/cpp : supprimer PluginDimensions.h, ajouter paramètre height et membre height_, modifier getHeight() et setSize()"
    status: completed
  - id: "7"
    content: "Modifier HorizontalSeparator.h/cpp : supprimer PluginDimensions.h, ajouter paramètre height et membre height_, modifier getHeight() et setSize()"
    status: completed
  - id: "8"
    content: "Modifier WidgetFactory.h/cpp : ajouter paramètres height dans createStandaloneButton() et createComboBoxFromDescriptor(), récupérer dimensions depuis PluginDimensions"
    status: completed
  - id: "9"
    content: "Modifier tous les panneaux : ajouter #include PluginDimensions.h, passer height aux constructeurs de widgets, mettre à jour appels getHeight()"
    status: completed
---

# Plan de refactorisation : Supprimer la dépendance PluginDimensions des widgets

## Objectif

Rendre les widgets génériques et réutilisables en supprimant leur dépendance à `PluginDimensions.h`. Les widgets doivent recevoir leurs dimensions via leurs constructeurs, et les panneaux sont responsables de fournir ces dimensions depuis `PluginDimensions.h`.

## État actuel

### Widgets avec dépendance PluginDimensions.h

1. **Label** (`Source/GUI/Widgets/Label.h/cpp`)

- Utilise `PluginDimensions::WidgetHeights::kLabel` dans `getHeight()` et `setSize()`
- A déjà un paramètre `width` dans le constructeur
- Doit ajouter un paramètre `height` et un membre `height_`

2. **Button** (`Source/GUI/Widgets/Button.h/cpp`)

- Utilise `PluginDimensions::WidgetHeights::kButton` dans `getHeight()` et `setSize()`
- A déjà un paramètre `width` dans le constructeur
- Doit ajouter un paramètre `height` et un membre `height_`

3. **SectionHeader** (`Source/GUI/Widgets/SectionHeader.h/cpp`)

- Utilise `PluginDimensions::WidgetHeights::kSectionHeader` dans `getHeight()` et `setSize()`
- A déjà un paramètre `width` dans le constructeur
- Doit ajouter un paramètre `height` et un membre `height_`

4. **ModuleHeader** (`Source/GUI/Widgets/ModuleHeader.h/cpp`)

- Utilise `PluginDimensions::WidgetHeights::kModuleHeader` dans `getHeight()` et `setSize()`
- A un paramètre `width` avec valeur par défaut `PluginDimensions::ModuleHeaderWidths::kPatchEditModule`
- Doit retirer la valeur par défaut et ajouter un paramètre `height` et un membre `height_`

5. **ComboBox** (`Source/GUI/Widgets/ComboBox.h/cpp`)

- Utilise `PluginDimensions::WidgetHeights::kComboBox` dans `getHeight()` et `setSize()`
- A un paramètre `width` avec valeur par défaut `PluginDimensions::ComboBoxWidths::kPatchEditModule`
- Doit retirer la valeur par défaut et ajouter un paramètre `height` et un membre `height_`

6. **GroupLabel** (`Source/GUI/Widgets/GroupLabel.h/cpp`)

- Utilise `PluginDimensions::WidgetHeights::kGroupLabel` dans `getHeight()` et `setSize()`
- A déjà un paramètre `width` dans le constructeur
- Doit ajouter un paramètre `height` et un membre `height_`

7. **HorizontalSeparator** (`Source/GUI/Widgets/HorizontalSeparator.h/cpp`)

- Utilise `PluginDimensions::WidgetHeights::kHorizontalSeparator` dans `getHeight()` et `setSize()`
- A déjà un paramètre `width` dans le constructeur
- Doit ajouter un paramètre `height` et un membre `height_`

### Widgets sans dépendance (déjà génériques)

- **Slider** : a déjà `kWidth` et `kHeight` comme constantes statiques
- **NumberBox** : a déjà `kHeight` comme constante statique (width passé en paramètre)
- **ModulationBusHeader** : a déjà `kWidth` et `kHeight` comme constantes statiques
- **VerticalSeparator** : a déjà `kWidth` et `kHeight` comme constantes statiques

## Modifications à effectuer

### Phase 1 : Modifier les widgets

Pour chaque widget listé ci-dessus (Label, Button, SectionHeader, ModuleHeader, ComboBox, GroupLabel, HorizontalSeparator) :

1. **Supprimer l'inclusion de PluginDimensions.h**

- Retirer `#include "../../Shared/PluginDimensions.h"` du fichier `.h`

2. **Modifier le constructeur**

- Ajouter un paramètre `int height` après le paramètre `width`
- Pour ModuleHeader et ComboBox : retirer la valeur par défaut du paramètre `width`
- Stocker `height` dans un membre privé `height_` (à côté de `width_`)

3. **Modifier `getHeight()`**

- Remplacer `static constexpr int getHeight() { return PluginDimensions::WidgetHeights::kXXX; }`
- Par `int getHeight() const { return height_; }` (méthode d'instance, non statique)

4. **Modifier `setSize()` dans le constructeur**

- Remplacer `setSize(width_, PluginDimensions::WidgetHeights::kXXX);`
- Par `setSize(width_, height_);`

### Phase 2 : Modifier WidgetFactory

Le `WidgetFactory` crée des widgets (Button, ComboBox, Slider) et doit être mis à jour :

1. **WidgetFactory::createStandaloneButton()**

- Ajouter un paramètre `int height` ou récupérer depuis `PluginDimensions::Widgets::Heights::kButton`
- Passer `height` au constructeur de `Button`

2. **WidgetFactory::createComboBoxFromDescriptor()**

- Ajouter un paramètre `int height` ou récupérer depuis `PluginDimensions::Widgets::Heights::kComboBox`
- Passer `height` au constructeur de `ComboBox`
- Récupérer `width` depuis `PluginDimensions` selon le contexte (ne plus utiliser la valeur par défaut)

3. **WidgetFactory::createSliderFromDescriptor()**

- Slider est déjà générique, pas de modification nécessaire

### Phase 3 : Modifier les panneaux

Pour tous les panneaux qui créent des widgets, ajouter l'inclusion de `PluginDimensions.h` et passer les dimensions :

1. **Ajouter l'inclusion**

- Ajouter `#include "../../Shared/PluginDimensions.h"` dans les fichiers `.cpp` des panneaux

2. **Modifier les créations de widgets**

- Pour `Label` : passer `PluginDimensions::Widgets::Heights::kLabel` comme paramètre `height`
- Pour `Button` : passer `PluginDimensions::Widgets::Heights::kButton` comme paramètre `height`
- Pour `SectionHeader` : passer `PluginDimensions::Widgets::Heights::kSectionHeader` comme paramètre `height`
- Pour `ModuleHeader` : passer `PluginDimensions::Widgets::Heights::kModuleHeader` comme paramètre `height` et retirer la valeur par défaut de `width`
- Pour `ComboBox` : passer `PluginDimensions::Widgets::Heights::kComboBox` comme paramètre `height` et retirer la valeur par défaut de `width`
- Pour `GroupLabel` : passer `PluginDimensions::Widgets::Heights::kGroupLabel` comme paramètre `height`
- Pour `HorizontalSeparator` : passer `PluginDimensions::Widgets::Heights::kHorizontalSeparator` comme paramètre `height`

3. **Mettre à jour les appels à `getHeight()`**

- Remplacer `Widget::getHeight()` (statique) par `widget->getHeight()` (instance)
- Ou utiliser directement `PluginDimensions::Widgets::Heights::kXXX` dans les calculs de layout

### Phase 4 : Mettre à jour les références aux nouvelles constantes

Selon les renommages mentionnés :

- `kComputerPatchesFolderAndFiles` → `kComputerPatchesUtility` (déjà fait dans PluginDimensions.h)
- `kBankNumber` → `kPatchManagerBankNumber` (déjà fait dans PluginDimensions.h)
- `kPatchNumber` → `kPatchManagerPatchNumber` (déjà fait dans PluginDimensions.h)

Vérifier que toutes les références utilisent les nouveaux noms.

## Fichiers à modifier

### Widgets (7 fichiers .h + 7 fichiers .cpp)

- `Source/GUI/Widgets/Label.h` et `.cpp`
- `Source/GUI/Widgets/Button.h` et `.cpp`
- `Source/GUI/Widgets/SectionHeader.h` et `.cpp`
- `Source/GUI/Widgets/ModuleHeader.h` et `.cpp`
- `Source/GUI/Widgets/ComboBox.h` et `.cpp`
- `Source/GUI/Widgets/GroupLabel.h` et `.cpp`
- `Source/GUI/Widgets/HorizontalSeparator.h` et `.cpp`

### WidgetFactory (2 fichiers)

- `Source/GUI/Factories/WidgetFactory.h`
- `Source/GUI/Factories/WidgetFactory.cpp`

### Panneaux (tous les fichiers .cpp qui créent des widgets)

- Tous les fichiers dans `Source/GUI/Panels/` qui créent des widgets (environ 20+ fichiers)

## Ordre d'exécution recommandé

1. Modifier tous les widgets (Phase 1)
2. Modifier WidgetFactory (Phase 2)
3. Modifier les panneaux (Phase 3)
4. Vérifier et corriger les erreurs de compilation
5. Tester que l'interface fonctionne correctement

## Notes importantes

- Les widgets `Slider`, `NumberBox`, `ModulationBusHeader` et `VerticalSeparator` sont déjà génériques et ne nécessitent pas de modifications
- `getHeight()` devient une méthode d'instance au lieu d'une méthode statique pour les widgets modifiés
- Les panneaux doivent maintenant inclure `PluginDimensions.h` pour récupérer les dimensions appropriées
- Le `WidgetFactory` doit être mis à jour pour passer les dimensions aux widgets qu'il crée