---
name: NumberBox Validation Enhancement
overview: Améliorer le widget NumberBox avec validation de saisie, bornage des valeurs min/max, et ajouter les constantes Matrix-1000 pour les limites de patch/banque dans PluginDescriptors.
todos:
  - id: add-matrix1000-constants
    content: Ajouter le namespace Matrix1000Limits avec les constantes min/max pour bank et patch dans PluginDescriptors.h
    status: completed
  - id: update-numberbox-constructor
    content: Modifier le constructeur NumberBox pour accepter minValue et maxValue, ajouter les membres privés correspondants
    status: completed
  - id: implement-validation-logic
    content: "Implémenter la validation dans NumberBox : retirer le '-' des caractères autorisés, ajouter le clamping dans handleEditorReturn() et setValue()"
    status: completed
  - id: update-internalpatchespanel
    content: Mettre à jour setupCurrentBankNumberBox() et setupCurrentPatchNumberBox() pour passer les limites Matrix1000 au constructeur NumberBox
    status: completed
  - id: test-validation
    content: "Tester la validation : saisie de caractères non-numériques, valeurs hors limites, valeurs négatives"
    status: completed
isProject: false
---

# Amélioration du widget NumberBox avec validation

## Analyse de l'approche proposée

Ton approche est **très pertinente** et respecte bien l'architecture du projet :

1. ✅ **Validation de saisie** : Filtrer les caractères non-numériques est essentiel pour une bonne UX
2. ✅ **Bornage min/max** : Utiliser un clamping automatique (comme tu l'as choisi) est cohérent avec les pratiques audio/MIDI
3. ✅ **Constantes dans PluginDescriptors** : Centraliser les caractéristiques du Matrix-1000 dans `PluginDescriptors` est la bonne approche (Single Source of Truth)
4. ✅ **Architecture APVTS** : Ton idée de passer par `NumberBox → APVTS → PluginProcessor → APVTS → InternalPatchesPanel` est architecturalement correcte et respecte la séparation GUI/Core

## Points d'attention identifiés

### 1. Statut actuel des NumberBox

Les `NumberBox` pour `currentPatchNumber` et `currentBankNumber` sont actuellement :
- Créés manuellement dans [`InternalPatchesPanel.cpp`](Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/InternalPatchesPanel.cpp) (lignes 157-171)
- **NON connectés à l'APVTS** (pas de paramètres APVTS correspondants)
- Déclarés comme `StandaloneWidgetType::kNumber` dans [`PluginDescriptors.cpp`](Source/Shared/PluginDescriptors.cpp) (lignes 2208-2217)
- Le `WidgetFactory` ne gère pas encore la création de `NumberBox`

### 2. Architecture à mettre en place

Pour implémenter ton approche APVTS, il faudra :

**Option A - Paramètres APVTS dédiés (recommandé)** :
- Créer deux `IntParameterDescriptor` dans `PluginDescriptors` : `kCurrentBankNumber` et `kCurrentPatchNumber`
- Ajouter ces paramètres à l'APVTS via `ApvtsFactory`
- Connecter les `NumberBox` à ces paramètres via des attachments
- Le `PluginProcessor` écoute ces paramètres et déclenche les actions (chargement de patch, etc.)

**Option B - État interne + callbacks** :
- Les `NumberBox` restent des standalone widgets (pas de paramètres APVTS)
- Utiliser un système de callbacks/listeners pour notifier les changements
- Plus simple mais moins cohérent avec l'architecture APVTS existante

**Recommandation** : Option A, car elle est plus cohérente avec l'architecture actuelle du plugin et permet une meilleure séparation des responsabilités.

## Modifications à effectuer

### 1. Ajouter les constantes Matrix-1000 dans PluginDescriptors

Fichier : [`Source/Shared/PluginDescriptors.h`](Source/Shared/PluginDescriptors.h)

Ajouter dans la section "Constants" (après ligne 28) :

```cpp
// Matrix-1000 Hardware Limits
namespace Matrix1000Limits
{
    constexpr int kMinBankNumber = 0;
    constexpr int kMaxBankNumber = 9;
    constexpr int kMinPatchNumber = 0;
    constexpr int kMaxPatchNumber = 99;
}
```

### 2. Modifier le constructeur NumberBox

Fichier : [`Source/GUI/Widgets/NumberBox.h`](Source/GUI/Widgets/NumberBox.h)

Modifier le constructeur pour accepter min/max :

```cpp
explicit NumberBox(Skin& skin, int width, bool editable, int minValue, int maxValue);
```

Ajouter les membres privés :

```cpp
int minValue_ = 0;
int maxValue_ = 99;
```

### 3. Implémenter la validation dans NumberBox

Fichier : [`Source/GUI/Widgets/NumberBox.cpp`](Source/GUI/Widgets/NumberBox.cpp)

**Modifications à effectuer** :

1. **Constructeur** (ligne 9) : Ajouter les paramètres min/max et les stocker
2. **Méthode `setValue()`** (ligne 26) : Ajouter le clamping de la valeur
3. **Méthode `showEditor()`** (ligne 200) : Retirer le caractère `-` de `setInputRestrictions()` (uniquement chiffres `"0123456789"`)
4. **Méthode `handleEditorReturn()`** (ligne 222) : 
   - Valider que le texte n'est pas vide
   - Convertir en int avec `getIntValue()`
   - Clamper la valeur entre min et max
   - Appeler `setValue()` avec la valeur clampée

Exemple de logique de validation dans `handleEditorReturn()` :

```cpp
void NumberBox::handleEditorReturn()
{
    if (editor_ == nullptr)
        return;

    const auto text = editor_->getText();
    
    // Vérifier que la saisie n'est pas vide
    if (text.isEmpty())
    {
        hideEditor();
        return;
    }
    
    // Convertir et clamper la valeur
    const int rawValue = text.getIntValue();
    const int clampedValue = juce::jlimit(minValue_, maxValue_, rawValue);
    
    setValue(clampedValue);
    hideEditor();
}
```

### 4. Mettre à jour InternalPatchesPanel

Fichier : [`Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/InternalPatchesPanel.cpp`](Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/InternalPatchesPanel.cpp)

Modifier les méthodes de création des `NumberBox` (lignes 155-172) :

```cpp
void InternalPatchesPanel::setupCurrentBankNumberBox(tss::Skin& skin)
{
    currentBankNumber = std::make_unique<tss::NumberBox>(
        skin, 
        PluginDimensions::Widgets::Widths::NumberBox::kPatchManagerBankNumber, 
        false,  // non éditable (mis à jour par les boutons [0]-[9])
        PluginDescriptors::Matrix1000Limits::kMinBankNumber,
        PluginDescriptors::Matrix1000Limits::kMaxBankNumber);
    currentBankNumber->setShowDot(true);
    addAndMakeVisible(*currentBankNumber);
}

void InternalPatchesPanel::setupCurrentPatchNumberBox(tss::Skin& skin)
{
    currentPatchNumber = std::make_unique<tss::NumberBox>(
        skin, 
        PluginDimensions::Widgets::Widths::NumberBox::kPatchManagerPatchNumber, 
        true,  // éditable
        PluginDescriptors::Matrix1000Limits::kMinPatchNumber,
        PluginDescriptors::Matrix1000Limits::kMaxPatchNumber);
    addAndMakeVisible(*currentPatchNumber);
}
```

## Étapes suivantes (hors scope de ce plan)

Pour compléter l'intégration APVTS que tu souhaites, il faudra dans un second temps :

1. Créer des `IntParameterDescriptor` pour `currentBankNumber` et `currentPatchNumber`
2. Ajouter une méthode `createIntParameterNumberBox()` dans `WidgetFactory`
3. Créer des attachments APVTS pour synchroniser les `NumberBox` avec les paramètres
4. Implémenter la logique dans `PluginProcessor` pour réagir aux changements de ces paramètres

Ces étapes peuvent être traitées dans un plan séparé si tu le souhaites.

## Validation

Après implémentation, vérifier que :

- ✓ La saisie de caractères non-numériques est ignorée dans `currentPatchNumber`
- ✓ La saisie de valeurs hors limites (ex: 150) est clampée à 99
- ✓ La saisie de valeurs négatives (ex: -5) est clampée à 0
- ✓ Le `currentBankNumber` utilise bien les limites 0-9 (même si non éditable)
- ✓ Les constantes `Matrix1000Limits` sont utilisées partout (pas de magic numbers)
