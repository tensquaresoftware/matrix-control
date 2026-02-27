---
name: Refactor SynthDescriptors to pure data + WidgetFactory
overview: Transformer SynthDescriptors.h en fichier de données pur (structures + constantes + déclarations), créer SynthDescriptors.cpp pour les définitions, ajouter des namespaces d'IDs constants, déplacer toute la logique dans ApvtsFactory avec méthodes de validation, créer WidgetFactory pour la construction des widgets GUI, et adapter tous les fichiers utilisateurs.
todos:
  - id: phase1-synthdescriptors-h
    content: "Phase 1: Refactoriser SynthDescriptors.h - Ajouter namespaces d IDs (ParameterIds, WidgetIds, DisplayNames, ChoiceLists, WidgetDisplayNames) et transformer fonctions inline en déclarations extern"
    status: completed
  - id: phase2-synthdescriptors-cpp
    content: "Phase 2: Créer SynthDescriptors.cpp - Définir toutes les données complexes en utilisant les constantes, créer makeStringArray helper, pré-générer les 10 Matrix Modulation Busses"
    status: completed
    dependencies:
      - phase1-synthdescriptors-h
  - id: phase3-apvtsfactory-helpers
    content: "Phase 3.1: ApvtsFactory - Déplacer fonctions helper depuis SynthDescriptors (getGroupDisplayName, getSectionDisplayName, getAllIntParameters, etc.)"
    status: completed
    dependencies:
      - phase2-synthdescriptors-cpp
  - id: phase3-apvtsfactory-adapt
    content: "Phase 3.2: ApvtsFactory - Adapter méthodes existantes pour utiliser les constantes (kAllGroups, kDco1IntParameters, etc.)"
    status: completed
    dependencies:
      - phase3-apvtsfactory-helpers
  - id: phase3-apvtsfactory-validation
    content: "Phase 3.3: ApvtsFactory - Ajouter méthodes de validation complètes (validateSynthDescriptors, validateGroups, validateParameters, checkForDuplicateIds, etc.)"
    status: completed
    dependencies:
      - phase3-apvtsfactory-adapt
  - id: phase4-widgetfactory-header
    content: "Phase 4.1: Créer WidgetFactory.h - Définir la classe avec méthodes publiques (createIntParameterSlider, createChoiceParameterComboBox, createStandaloneButton, getDisplayName helpers) et méthodes privées de recherche"
    status: completed
    dependencies:
      - phase2-synthdescriptors-cpp
  - id: phase4-widgetfactory-impl
    content: "Phase 4.2: Implémenter WidgetFactory.cpp - Créer les widgets en interrogeant SynthDescriptors et APVTS, implémenter les méthodes de recherche et getDisplayName"
    status: completed
    dependencies:
      - phase4-widgetfactory-header
  - id: phase5-dco1panel
    content: "Phase 5.1-5.2: Adapter Dco1Panel.h et Dco1Panel.cpp - Ajouter WidgetFactory& au constructeur, remplacer création manuelle par appels WidgetFactory"
    status: completed
    dependencies:
      - phase4-widgetfactory-impl
  - id: phase5-patcheditpanel
    content: "Phase 5.3-5.4: Adapter PatchEditPanel.h et PatchEditPanel.cpp - Ajouter WidgetFactory& au constructeur, passer WidgetFactory aux sous-panneaux"
    status: completed
    dependencies:
      - phase5-dco1panel
  - id: phase5-plugineditor
    content: "Phase 5.6: Adapter PluginEditor.cpp - Créer instance WidgetFactory avec référence APVTS, passer aux panneaux"
    status: completed
    dependencies:
      - phase5-patcheditpanel
  - id: phase5-other-panels
    content: "Phase 5.5: Vérifier et adapter autres panels (Dco2Panel, VcfVcaPanel, FmTrackPanel, RampPortamentoPanel) si nécessaire"
    status: pending
    dependencies:
      - phase5-patcheditpanel
  - id: phase6-compilation
    content: "Phase 6.1: Tests compilation - Vérifier compilation sans erreurs ni warnings"
    status: completed
    dependencies:
      - phase5-plugineditor
      - phase5-other-panels
  - id: phase6-validation
    content: "Phase 6.2: Validation runtime - Appeler ApvtsFactory::validateSynthDescriptors() au démarrage, vérifier passage sans erreurs"
    status: completed
    dependencies:
      - phase6-compilation
  - id: phase6-functional
    content: "Phase 6.3: Tests fonctionnels - Vérifier création widgets, attachments APVTS, display names corrects"
    status: pending
    dependencies:
      - phase6-validation
---

# Refactorisation SynthDescriptors : Données pures + Validation + WidgetFactory

## Objectif
Transformer `SynthDescriptors` en source de vérité unique contenant uniquement des données descriptives, déplacer toute la logique dans `ApvtsFactory`, créer `WidgetFactory` pour simplifier la construction des widgets GUI, et ajouter des méthodes de validation complètes.

## Architecture cible

```
SynthDescriptors.h  → Structures + Constantes + Déclarations extern
SynthDescriptors.cpp → Définitions de données (utilisant les constantes)
ApvtsFactory         → Logique d'interrogation + Validation + Construction APVTS
WidgetFactory        → Création de widgets GUI + Attachement aux paramètres APVTS
```

## Phase 1 : Refactoriser SynthDescriptors.h

### 1.1 Ajouter les namespaces d'IDs et constantes
- Créer `namespace ParameterIds` avec toutes les constantes d'IDs de paramètres (ex: `kDco1Frequency`, `kDco1Sync`, etc.)
- Créer `namespace WidgetIds` avec toutes les constantes d'IDs de widgets autonomes (ex: `kDco1Init`, `kDco1Copy`, etc.)
- Créer `namespace DisplayNames` avec tous les noms d'affichage des paramètres (ex: `kDco1Frequency = "FREQUENCY"`, etc.)
- Créer `namespace ChoiceLists` avec toutes les constantes de choix pour ComboBox (ex: `kSyncOff`, `kSyncSoft`, `kWaveSelectOff`, etc.)
- Créer `namespace WidgetDisplayNames` avec les noms d'affichage des widgets autonomes (ex: `kDco1Init = "I"`, etc.)

### 1.2 Transformer les fonctions inline en déclarations extern
- Remplacer `inline const std::vector<GroupDescriptor> getAllGroups()` par `extern const std::vector<GroupDescriptor> kAllGroups;`
- Remplacer toutes les fonctions `getXXXIntParameters()` par `extern const std::vector<IntParameterDescriptor> kXXXIntParameters;`
- Remplacer toutes les fonctions `getXXXChoiceParameters()` par `extern const std::vector<ChoiceParameterDescriptor> kXXXChoiceParameters;`
- Remplacer toutes les fonctions `getXXXStandaloneWidgets()` par `extern const std::vector<StandaloneWidgetDescriptor> kXXXStandaloneWidgets;`
- Supprimer `getSectionDisplayName()` (sera dans ApvtsFactory)
- Supprimer `getAllIntParameters()`, `getAllChoiceParameters()`, `getAllStandaloneWidgets()` (seront dans ApvtsFactory)

### 1.3 Conserver uniquement
- Structures de données (`GroupDescriptor`, `IntParameterDescriptor`, `ChoiceParameterDescriptor`, `StandaloneWidgetDescriptor`)
- Enums (`ParameterType`, `StandaloneWidgetType`)
- Namespaces de constantes existants (`GroupIds`, `SectionIds`, `ModuleIds`, `BusIds`)
- Déclarations `extern` pour toutes les données complexes

## Phase 2 : Créer SynthDescriptors.cpp

### 2.1 Créer le fichier avec toutes les définitions
- Définir `kAllGroups` en utilisant les constantes `GroupIds`, `SectionIds`, `ModuleIds`, `BusIds`
- Définir tous les `kXXXIntParameters` en utilisant `ParameterIds` et `DisplayNames`
- Définir tous les `kXXXChoiceParameters` en utilisant `ParameterIds`, `DisplayNames`, et `ChoiceLists` via `makeStringArray()`
- Définir tous les `kXXXStandaloneWidgets` en utilisant `WidgetIds` et `WidgetDisplayNames`

### 2.2 Helper pour les StringArray
- Créer fonction helper anonyme `makeStringArray(std::initializer_list<const char*>)` pour construire les `juce::StringArray` à partir des constantes `ChoiceLists`

### 2.3 Gérer les données générées dynamiquement
- Pour les Matrix Modulation Busses : pré-générer les 10 busses dans des constantes statiques
- Créer `kMatrixModBusChoiceParameters` comme `std::array<std::vector<ChoiceParameterDescriptor>, 10>`
- Créer `kMatrixModBusIntParameters` comme `std::array<std::vector<IntParameterDescriptor>, 10>`
- Utiliser des constantes helper anonymes pour `kSourceChoices` et `kDestinationChoices`

## Phase 3 : Refactoriser ApvtsFactory

### 3.1 Déplacer les fonctions helper depuis SynthDescriptors
- Ajouter `getGroupDisplayName(const juce::String& groupId)` qui interroge `kAllGroups` et retourne le displayName
- Ajouter `getSectionDisplayName(const char* sectionId)` qui utilise `getGroupDisplayName()`
- Ajouter `getAllIntParameters()` qui agrège tous les `kXXXIntParameters` (DCO1, DCO2, VCF/VCA, FM/TRACK, RAMP/PORTAMENTO, ENV1-3, LFO1-2, Matrix Busses, Master Edit)
- Ajouter `getAllChoiceParameters()` qui agrège tous les `kXXXChoiceParameters`
- Ajouter `getAllStandaloneWidgets()` qui agrège tous les `kXXXStandaloneWidgets`

### 3.2 Adapter les méthodes existantes
- Modifier `createParameterLayout()` pour utiliser `kAllGroups` au lieu de `getAllGroups()`
- Modifier `addPatchEditParameters()` pour utiliser les constantes `kDco1IntParameters`, `kDco1ChoiceParameters`, etc.
- Modifier `addMatrixModulationParameters()` pour utiliser les arrays pré-générés `kMatrixModBusChoiceParameters` et `kMatrixModBusIntParameters`

### 3.3 Ajouter les méthodes de validation
- Créer `struct ValidationResult` avec `bool isValid`, `juce::StringArray errors`, `juce::StringArray warnings`
- Implémenter `validateSynthDescriptors()` qui appelle toutes les validations et retourne un `ValidationResult`
- Implémenter `validateGroups()` : vérifier IDs uniques, parents valides, display names non vides
- Implémenter `validateParameters()` : vérifier IDs uniques, parentGroupId valide, valeurs cohérentes (min/max/default), defaultIndex valide pour Choice
- Implémenter `validateStandaloneWidgets()` : vérifier IDs uniques, parentGroupId valide
- Implémenter `checkForDuplicateIds()` : détecter collisions entre namespaces (group/parameter/widget)
- Implémenter `checkForOrphanedReferences()` : vérifier que tous les IDs référencés existent
- Implémenter `checkForCircularReferences()` : détecter cycles dans la hiérarchie des groupes avec DFS
- Ajouter helpers privés : `collectAllGroupIds()`, `collectAllParameterIds()`, `collectAllWidgetIds()`, `groupExists()`, `parameterIdExists()`, `widgetIdExists()`

## Phase 4 : Créer WidgetFactory

### 4.1 Créer WidgetFactory.h
- Créer la classe `WidgetFactory` avec membre privé `juce::AudioProcessorValueTreeState& apvts`
- Constructeur public : `explicit WidgetFactory(juce::AudioProcessorValueTreeState& apvts)`
- Méthodes publiques pour créer des widgets :
  - `std::unique_ptr<tss::Slider> createIntParameterSlider(const juce::String& parameterId, Skin& skin)`
  - `std::unique_ptr<tss::ComboBox> createChoiceParameterComboBox(const juce::String& parameterId, Skin& skin)`
  - `std::unique_ptr<tss::Button> createStandaloneButton(const juce::String& widgetId, Skin& skin)`
- Méthodes publiques helper pour obtenir les display names :
  - `juce::String getParameterDisplayName(const juce::String& parameterId)`
  - `juce::String getGroupDisplayName(const juce::String& groupId)`
  - `juce::String getStandaloneWidgetDisplayName(const juce::String& widgetId)`
- Méthodes privées helper pour interroger SynthDescriptors :
  - `const SynthDescriptors::IntParameterDescriptor* findIntParameter(const juce::String& parameterId)`
  - `const SynthDescriptors::ChoiceParameterDescriptor* findChoiceParameter(const juce::String& parameterId)`
  - `const SynthDescriptors::StandaloneWidgetDescriptor* findStandaloneWidget(const juce::String& widgetId)`
  - `const SynthDescriptors::GroupDescriptor* findGroup(const juce::String& groupId)`

### 4.2 Implémenter WidgetFactory.cpp
- Implémenter `createIntParameterSlider()` :
  - Chercher le paramètre via `findIntParameter(parameterId)`
  - Vérifier existence avec `jassert(desc != nullptr)`
  - Récupérer le paramètre JUCE via `apvts.getParameter(parameterId)`
  - Créer le Slider avec dimensions depuis `SkinDimensions::Widget::Slider`
  - Créer et retourner le slider (les attachments seront gérés par les panels)
- Implémenter `createChoiceParameterComboBox()` :
  - Chercher le paramètre via `findChoiceParameter(parameterId)`
  - Vérifier existence
  - Récupérer le paramètre JUCE depuis l'APVTS
  - Créer le ComboBox avec les choix depuis `desc->choices`
  - Retourner le combobox créé
- Implémenter `createStandaloneButton()` :
  - Chercher le widget via `findStandaloneWidget(widgetId)`
  - Vérifier existence et type `StandaloneWidgetType::kButton`
  - Créer le Button avec `desc->displayName` et dimensions depuis `SkinDimensions::Widget::Button`
  - Retourner le button créé
- Implémenter les méthodes de recherche qui parcourent les constantes de SynthDescriptors
- Implémenter les méthodes getDisplayName qui utilisent les méthodes de recherche

### 4.3 Note sur les attachments
- WidgetFactory retourne uniquement les widgets créés
- Les attachments APVTS seront créés et gérés par les panels qui utilisent les widgets
- Cela permet une meilleure séparation des responsabilités

## Phase 5 : Adapter les fichiers utilisateurs

### 5.1 Dco1Panel.h
- Modifier le constructeur pour accepter `WidgetFactory& widgetFactory` en paramètre
- Ajouter `#include "WidgetFactory.h"` si nécessaire

### 5.2 Dco1Panel.cpp
- Modifier le constructeur pour accepter `WidgetFactory& widgetFactory`
- Supprimer toute la logique de recherche manuelle (lambdas `findIntParam`, `findChoiceParam`)
- Supprimer les appels directs à `SynthDescriptors::getDco1IntParameters()`, etc.
- Remplacer la création manuelle de widgets par :
  - `dco1FrequencySlider = widgetFactory.createIntParameterSlider(SynthDescriptors::ParameterIds::kDco1Frequency, *skin);`
  - `dco1SyncComboBox = widgetFactory.createChoiceParameterComboBox(SynthDescriptors::ParameterIds::kDco1Sync, *skin);`
  - `dco1InitButton = widgetFactory.createStandaloneButton(SynthDescriptors::WidgetIds::kDco1Init, *skin);`
- Remplacer la recherche du moduleDisplayName par `widgetFactory.getGroupDisplayName(SynthDescriptors::ModuleIds::kDco1)`
- Simplifier drastiquement : chaque widget créé en une ligne

### 5.3 PatchEditPanel.h
- Modifier le constructeur pour accepter `WidgetFactory& widgetFactory`
- Passer le WidgetFactory aux sous-panneaux dans la liste d'initialisation

### 5.4 PatchEditPanel.cpp
- Modifier le constructeur pour accepter `WidgetFactory& widgetFactory`
- Passer `widgetFactory` aux constructeurs des sous-panneaux (Dco1Panel, Dco2Panel, etc.)
- Remplacer `SynthDescriptors::getSectionDisplayName()` par `widgetFactory.getGroupDisplayName(SynthDescriptors::SectionIds::kPatchEdit)` ou `ApvtsFactory::getSectionDisplayName()`

### 5.5 Adapter les autres panels
- Vérifier et adapter `Dco2Panel`, `VcfVcaPanel`, `FmTrackPanel`, `RampPortamentoPanel` de la même manière
- Modifier leurs constructeurs pour accepter `WidgetFactory&`
- Remplacer la création manuelle de widgets par des appels à WidgetFactory

### 5.6 PluginEditor.cpp
- Créer une instance de `WidgetFactory` avec référence à l'APVTS : `WidgetFactory widgetFactory(apvts);`
- Passer cette instance aux panneaux qui en ont besoin lors de leur construction
- Vérifier la chaîne de passage du WidgetFactory depuis PluginEditor jusqu'aux sous-panneaux

## Phase 6 : Tests et validation

### 6.1 Compilation
- Vérifier que tout compile sans erreurs
- Vérifier qu'il n'y a pas de warnings
- Vérifier que tous les includes sont corrects

### 6.2 Validation runtime
- Appeler `ApvtsFactory::validateSynthDescriptors()` au démarrage du plugin (dans PluginProcessor ou PluginEditor)
- Vérifier que la validation passe sans erreurs
- Logger les erreurs de validation si elles existent (DBG ou juce::Logger)
- Tester avec des données invalides pour vérifier que la validation détecte les erreurs

### 6.3 Tests fonctionnels
- Vérifier que les widgets sont créés correctement
- Vérifier que les attachments APVTS fonctionnent
- Vérifier que les display names sont corrects
- Tester la création de widgets pour différents modules

## Fichiers à modifier

### Fichiers principaux
- `Source/Shared/SynthDescriptors.h` - Refactorisation complète avec namespaces d'IDs
- `Source/Shared/SynthDescriptors.cpp` - **NOUVEAU FICHIER** avec toutes les définitions
- `Source/Shared/ApvtsFactory.h` - Ajout méthodes validation + helpers
- `Source/Shared/ApvtsFactory.cpp` - Implémentation validation + adaptation aux constantes
- `Source/Shared/WidgetFactory.h` - **NOUVEAU FICHIER**
- `Source/Shared/WidgetFactory.cpp` - **NOUVEAU FICHIER**

### Fichiers utilisateurs
- `Source/GUI/Panels/MainPanel/PatchEditPanel/Modules/Dco1Panel.h` - Ajouter WidgetFactory& au constructeur
- `Source/GUI/Panels/MainPanel/PatchEditPanel/Modules/Dco1Panel.cpp` - Utiliser WidgetFactory
- `Source/GUI/Panels/MainPanel/PatchEditPanel/PatchEditPanel.h` - Ajouter WidgetFactory& au constructeur
- `Source/GUI/Panels/MainPanel/PatchEditPanel/PatchEditPanel.cpp` - Créer et passer WidgetFactory
- `Source/PluginEditor.h` - Potentiellement ajouter WidgetFactory comme membre
- `Source/PluginEditor.cpp` - Créer WidgetFactory et le passer aux panneaux
- Autres panels si nécessaire (Dco2Panel, VcfVcaPanel, FmTrackPanel, RampPortamentoPanel)

## Points d'attention

1. **Constantes partagées** : Les choix comme "OFF"/"ON" doivent être définis une seule fois dans `ChoiceLists` et réutilisés partout
2. **Pré-génération des busses** : Les 10 Matrix Modulation Busses doivent être pré-générés dans SynthDescriptors.cpp
3. **Validation complète** : Toutes les validations doivent être implémentées avant de considérer la refactorisation terminée
4. **Rétrocompatibilité** : S'assurer que tous les fichiers utilisateurs sont adaptés pour éviter les erreurs de compilation
5. **Attachments APVTS** : Les attachments doivent être créés dans les panels, pas dans WidgetFactory
6. **Chaîne de passage** : Vérifier que WidgetFactory est correctement passé depuis PluginEditor jusqu'aux sous-panneaux

## Ordre d'exécution recommandé

1. Phase 1 (SynthDescriptors.h) - Créer les namespaces et déclarations
2. Phase 2 (SynthDescriptors.cpp) - Créer le fichier avec toutes les définitions utilisant les constantes
3. Phase 3 (ApvtsFactory) - Déplacer la logique, adapter aux constantes, ajouter validation
4. Phase 4 (WidgetFactory) - Créer WidgetFactory.h et .cpp avec toutes les méthodes
5. Phase 5 (Fichiers utilisateurs) - Adapter tous les usages avec WidgetFactory
6. Phase 6 (Tests) - Vérifier compilation, validation runtime, et tests fonctionnels