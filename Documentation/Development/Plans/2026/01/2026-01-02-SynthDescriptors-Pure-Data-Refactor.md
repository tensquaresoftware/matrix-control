---
name: Refactor SynthDescriptors to pure data
overview: Transformer SynthDescriptors.h en fichier de données pur (structures + constantes + déclarations), créer SynthDescriptors.cpp pour les définitions, ajouter des namespaces d'IDs constants, déplacer toute la logique dans ApvtsFactory avec méthodes de validation, et adapter les fichiers utilisateurs.
todos:
  - id: todo-1767368122103-9p1hi7fhk
    content: Il faut également créer la classe WidgetFactory tel que nous en avons discuté
    status: pending
  - id: todo-1767368144689-faij4q3jl
    content: ""
    status: pending
---

# Refactorisation SynthDescr

iptors : Données pures + Validation

## Objectif

Transformer `SynthDescriptors` en source de vérité unique contenant uniquement des données descriptives, déplacer toute la logique dans `ApvtsFactory`, et ajouter des méthodes de validation.

## Architecture cible

```javascript
SynthDescriptors.h  → Structures + Constantes + Déclarations extern
SynthDescriptors.cpp → Définitions de données (utilisant les constantes)
ApvtsFactory         → Logique d'interrogation + Validation + Construction APVTS
```



## Phase 1 : Refactoriser SynthDescriptors.h

### 1.1 Ajouter les namespaces d'IDs et constantes

- Créer `namespace ParameterIds` avec toutes les constantes d'IDs de paramètres

- Créer `namespace WidgetIds` avec toutes les constantes d'IDs de widgets autonomes

- Créer `namespace DisplayNames` avec tous les noms d'affichage

- Créer `namespace ChoiceLists` avec toutes les constantes de choix pour ComboBox
- Créer `namespace WidgetDisplayNames` avec les noms d'affichage des widgets autonomes

### 1.2 Transformer les fonctions inline en déclarations extern

- Remplacer `inline const std::vector<GroupDescriptor> getAllGroups()` par `extern const std::vector<GroupDescriptor> kAllGroups;`

- Remplacer toutes les fonctions `getXXXIntParameters()` par `extern const std::vector<IntParameterDescriptor> kXXXIntParameters;`
- Remplacer toutes les fonctions `getXXXChoiceParameters()` par `extern const std::vector<ChoiceParameterDescriptor> kXXXChoiceParameters;`

- Remplacer toutes les fonctions `getXXXStandaloneWidgets()` par `extern const std::vector<StandaloneWidgetDescriptor> kXXXStandaloneWidgets;`

- Supprimer `getSectionDisplayName()` (sera dans ApvtsFactory)

- Supprimer `getAllIntParameters()`, `getAllChoiceParameters()`, `getAllStandaloneWidgets()` (seront dans ApvtsFactory)

### 1.3 Conserver uniquement

- Structures de données (`GroupDescriptor`, `IntParameterDescriptor`, etc.)
- Enums (`ParameterType`, `StandaloneWidgetType`)
- Namespaces de constantes (`GroupIds`, `SectionIds`, `ModuleIds`, `BusIds`)
- Déclarations `extern` pour toutes les données complexes

## Phase 2 : Créer SynthDescriptors.cpp

### 2.1 Créer le fichier avec toutes les définitions

- Définir `kAllGroups` en utilisant les constantes `GroupIds`, `SectionIds`, `ModuleIds`, `BusIds`
- Définir tous les `kXXXIntParameters` en utilisant `ParameterIds` et `DisplayNames`

- Définir tous les `kXXXChoiceParameters` en utilisant `ParameterIds`, `DisplayNames`, et `ChoiceLists`

- Définir tous les `kXXXStandaloneWidgets` en utilisant `WidgetIds` et `WidgetDisplayNames`

### 2.2 Helper pour les StringArray

- Créer fonction helper `makeStringArray()` pour construire les `juce::StringArray` à partir des constantes `ChoiceLists`

### 2.3 Gérer les données générées dynamiquement

- Pour les Matrix Modulation Busses : pré-générer les 10 busses dans des constantes statiques

- Créer `kMatrixModBusChoiceParameters` et `kMatrixModBusIntParameters` comme arrays de 10 éléments

## Phase 3 : Refactoriser ApvtsFactory

### 3.1 Déplacer les fonctions helper depuis SynthDescriptors

- Ajouter `getGroupDisplayName(const juce::String& groupId)` qui interroge `kAllGroups`

- Ajouter `getSectionDisplayName(const char* sectionId)` qui utilise `getGroupDisplayName()`

- Ajouter `getAllIntParameters()` qui agrège tous les `kXXXIntParameters`

- Ajouter `getAllChoiceParameters()` qui agrège tous les `kXXXChoiceParameters`

- Ajouter `getAllStandaloneWidgets()` qui agrège tous les `kXXXStandaloneWidgets`

### 3.2 Adapter les méthodes existantes

- Modifier `createParameterLayout()` pour utiliser `kAllGroups` au lieu de `getAllGroups()`

- Modifier `addPatchEditParameters()` pour utiliser les constantes `kDco1IntParameters`, etc.
- Modifier `addMatrixModulationParameters()` pour utiliser les arrays pré-générés

### 3.3 Ajouter les méthodes de validation

- Créer `struct ValidationResult` avec `isValid`, `errors`, `warnings`

- Implémenter `validateSynthDescriptors()` qui appelle toutes les validations

- Implémenter `validateGroups()` : IDs uniques, parents valides, display names non vides

- Implémenter `validateParameters()` : IDs uniques, parentGroupId valide, valeurs cohérentes

- Implémenter `validateStandaloneWidgets()` : IDs uniques, parentGroupId valide

- Implémenter `checkForDuplicateIds()` : collisions entre namespaces
- Implémenter `checkForOrphanedReferences()` : références à des IDs inexistants

- Implémenter `checkForCircularReferences()` : cycles dans la hiérarchie des groupes

- Ajouter helpers : `collectAllGroupIds()`, `collectAllParameterIds()`, `collectAllWidgetIds()`, `groupExists()`, etc.

## Phase 4 : Adapter les fichiers utilisateurs

### 4.1 Dco1Panel.cpp

- Remplacer `SynthDescriptors::getDco1IntParameters()` par `SynthDescriptors::kDco1IntParameters`

- Remplacer `SynthDescriptors::getDco1ChoiceParameters()` par `SynthDescriptors::kDco1ChoiceParameters`

- Remplacer `SynthDescriptors::getAllGroups()` par `SynthDescriptors::kAllGroups`

- Remplacer `SynthDescriptors::getDcoStandaloneWidgets()` par `SynthDescriptors::kDcoStandaloneWidgets`
- Utiliser les constantes `ParameterIds::kDco1Frequency` au lieu de strings hardcodées

- Utiliser les constantes `WidgetIds::kDco1Init` au lieu de strings hardcodées

### 4.2 PatchEditPanel.cpp

- Remplacer `SynthDescriptors::getSectionDisplayName()` par `ApvtsFactory::getSectionDisplayName()`

### 4.3 Vérifier les autres panels

- Vérifier si `Dco2Panel`, `VcfVcaPanel`, `FmTrackPanel`, `RampPortamentoPanel` utilisent SynthDescriptors

- Si oui, appliquer les mêmes transformations

## Phase 5 : Tests et validation

### 5.1 Compilation

- Vérifier que tout compile sans erreurs

- Vérifier qu'il n'y a pas de warnings

### 5.2 Validation runtime

- Appeler `ApvtsFactory::validateSynthDescriptors()` au démarrage du plugin
- Vérifier que la validation passe sans erreurs

- Tester avec des données invalides pour vérifier que la validation détecte les erreurs

## Fichiers à modifier

### Fichiers principaux

- `Source/Shared/SynthDescriptors.h` - Refactorisation complète

- `Source/Shared/SynthDescriptors.cpp` - **NOUVEAU FICHIER**

- `Source/Shared/ApvtsFactory.h` - Ajout méthodes validation + helpers

- `Source/Shared/ApvtsFactory.cpp` - Implémentation validation + adaptation

### Fichiers utilisateurs

- `Source/GUI/Panels/MainPanel/PatchEditPanel/Modules/Dco1Panel.cpp` - Adaptation aux nouvelles constantes

- `Source/GUI/Panels/MainPanel/PatchEditPanel/PatchEditPanel.cpp` - Utiliser ApvtsFactory pour getSectionDisplayName

- Autres panels si nécessaire (à vérifier)

## Points d'attention

1. **Constantes partagées** : Les choix comme "OFF"/"ON" doivent être définis une seule fois dans `ChoiceLists` et réutilisés

2. **Pré-génération des busses** : Les 10 Matrix Modulation Busses doivent être pré-générés dans SynthDescriptors.cpp

3. **Validation complète** : Toutes les validations doivent être implémentées avant de considérer la refactorisation terminée

4. **Rétrocompatibilité** : S'assurer que tous les fichiers utilisateurs sont adaptés pour éviter les erreurs de compilation

## Ordre d'exécution recommandé

1. Phase 1 (SynthDescriptors.h) - Créer les namespaces et déclarations

2. Phase 2 (SynthDescriptors.cpp) - Créer le fichier avec toutes les définitions

3. Phase 3 (ApvtsFactory) - Déplacer la logique et ajouter validation

4. Phase 4 (Fichiers utilisateurs) - Adapter tous les usages