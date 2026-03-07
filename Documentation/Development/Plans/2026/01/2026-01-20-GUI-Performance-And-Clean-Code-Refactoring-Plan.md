---
name: Optimisation Performance GUI et Refactorisation Clean Code
overview: Plan d'action pour résoudre les problèmes de performance GUI (démarrage lent, lag, curseur qui tourne) et améliorer la qualité du code selon les principes Clean Code de Robert C. Martin.
todos: []
---

# Plan d'Action : Optimisation Performance GUI et Refactorisation Clean Code

## Problèmes Identifiés

### Performance GUI

1. **Démarrage lent** : Plusieurs secondes avant l'apparition de la fenêtre
2. **Curseur qui tourne** : Pendant le démarrage et lors du déplacement de la fenêtre
3. **Lag d'interaction** : Retard dans la réponse des boutons et widgets
4. **Repaints excessifs** : Nombreux appels à `repaint()` dans les chemins de mise à jour de thème

### Qualité de Code

1. **Méthode trop longue** : `PluginProcessor::valueTreePropertyChanged` (116 lignes) viole le principe de fonctions courtes
2. **Complexité cyclomatique élevée** : Logique conditionnelle imbriquée difficile à maintenir
3. **Manque d'optimisations JUCE** : Pas de `setOpaque(true)` ni de `setBufferedToImage` sur les composants

## Solutions Proposées

### Phase 1 : Optimisations Performance GUI (Priorité Haute)

#### 1.1 Désactiver le logging en production

**Fichiers concernés :**

- `Source/Core/PluginProcessor.cpp` (lignes 26, 217-230, 236)

**Actions :**

- Créer une constante de compilation `ENABLE_LOGGING_IN_RELEASE` (définie uniquement en debug)
- Conditionner l'activation des loggers avec cette constante
- Le logging est actuellement activé dans le constructeur (ligne 26) et dans `prepareToPlay()` (lignes 217-230), ce qui ralentit le démarrage

**Impact attendu :** Réduction significative du temps de démarrage (élimination des I/O fichiers et formatage de strings)

#### 1.2 Ajouter `setOpaque(true)` sur tous les composants opaques

**Fichiers concernés :**

- Tous les widgets dans `Source/GUI/Widgets/` (Slider, ComboBox, Button, NumberBox, Label, etc.)
- Tous les panels dans `Source/GUI/Panels/`

**Actions :**

- Ajouter `setOpaque(true)` dans les constructeurs de tous les composants qui dessinent un fond opaque
- JUCE peut alors optimiser en évitant de dessiner les composants enfants derrière

**Impact attendu :** Réduction des repaints inutiles, amélioration de la fluidité

#### 1.3 Utiliser `setBufferedToImage` pour les composants complexes

**Fichiers concernés :**

- `Source/GUI/MainComponent.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/BodyPanel.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/MatrixModulationPanel/MatrixModulationPanel.cpp`
- Autres panels complexes avec beaucoup d'enfants

**Actions :**

- Appeler `setBufferedToImage(true)` sur les composants conteneurs complexes
- JUCE cache alors le rendu dans une image et ne redessine que si nécessaire

**Impact attendu :** Amélioration drastique de la performance lors du déplacement de fenêtre et des interactions

#### 1.4 Optimiser les repaints lors des changements de thème

**Fichiers concernés :**

- `Source/GUI/PluginEditor.cpp` (ligne 67)
- `Source/GUI/Panels/MainComponent/BodyPanel/BodyPanel.cpp` (méthode `setTheme`)
- Tous les widgets qui appellent `repaint()` dans `setTheme()`

**Actions :**

- Utiliser `repaint()` uniquement sur les composants visibles
- Regrouper les repaints avec `repaint()` sur le composant parent au lieu de tous les enfants
- Utiliser `setBufferedToImage` pour éviter les repaints complets lors des changements de thème

**Impact attendu :** Réduction du temps de réponse lors du changement de thème

#### 1.5 Optimiser l'initialisation de WidgetFactory

**Fichiers concernés :**

- `Source/GUI/Factories/WidgetFactory.cpp` (méthode `buildSearchMaps()`)

**Actions :**

- Vérifier si `buildSearchMaps()` peut être optimisé (pré-allocation, réduction de copies)
- Déplacer l'initialisation lourde si possible après l'affichage initial de la fenêtre

**Impact attendu :** Réduction du temps de démarrage

### Phase 2 : Refactorisation Clean Code (Priorité Moyenne)

#### 2.1 Refactoriser `PluginProcessor::valueTreePropertyChanged`

**Fichier concerné :**

- `Source/Core/PluginProcessor.cpp` (lignes 239-355)

**Actions :**

- Extraire la logique de résolution de `parameterId` dans des fonctions dédiées :
- `extractParameterIdFromTree()` : Extrait l'ID depuis un ValueTree PARAM
- `findParameterIdInParentTree()` : Cherche l'ID dans l'arbre parent
- `searchParameterIdInStateChildren()` : Parcourt les enfants de l'état APVTS
- `resolveParameterId()` : Orchestre la résolution complète
- Simplifier la logique de logging en créant `logParameterChange()`
- Réduire la méthode à moins de 20 lignes avec des appels à des fonctions bien nommées

**Impact attendu :** Code plus lisible, maintenable, et potentiellement plus performant (early returns)

#### 2.2 Extraire les constantes magiques

**Fichiers concernés :**

- Tous les fichiers du projet

**Actions :**

- Identifier et remplacer toutes les valeurs littérales par des constantes nommées
- Vérifier que les constantes exista