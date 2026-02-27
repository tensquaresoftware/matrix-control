---
name: McButton Implementation
overview: Créer la classe McButton avec rendu custom selon les spécifications, puis ajouter les boutons de test dans PluginEditor sous les sliders existants.
todos: []
---

# Plan d'implémentation de McButton

## 1. Création de la classe McButton

### Fichiers à créer

- `Source/UI/Widgets/McButton.h` - Déclaration de la classe
- `Source/UI/Widgets/McButton.cpp` - Implémentation

### Structure de la classe

- Hérite de `juce::Button` (ou `juce::TextButton`)
- Constructeur prenant la largeur en paramètre (hauteur fixe à 20px)
- Méthode `setLookAndFeel(McLookAndFeel*)` similaire à `McSlider`
- Override de `paintButton()` pour le rendu custom
- Gestion de l'état cliqué (pour la couleur de texte rouge)

### Spécifications de rendu

- **Forme** : Rectangle sans coins arrondis
- **Bordure** : 2 pixels d'épaisseur
- **Hauteur** : 20 pixels (fixe)
- **Largeur** : Spécifiée au constructeur
- **Police** : PT Sans Narrow, taille 14, style Normal (via `McLookAndFeel::getDefaultFont()`)
- **Alignement texte** : Centré horizontalement et verticalement
- **Couleurs actives** : 
- Background : `McLookAndFeel::getButtonBackgroundColour(true)`
- Bordure : `McLookAndFeel::getButtonBorderColour(true)`
- Texte : `McLookAndFeel::getButtonTextColour(true, isClicked)`
- **Couleurs désactivées** (mêmes que sliders désactivés) :
- Background : `McLookAndFeel::getSliderBackgroundColour(false)` → `kSliderBackgroundColourDisabled` (0xFF303030)
- Bordure : Utiliser la même couleur que le background désactivé ou `kSliderBackgroundColourDisabled`
- Texte : `McLookAndFeel::getSliderTextColour(false)` → `kSliderTextColourDisabled` (0xFF808080)

### Implémentation du rendu

- Dessiner le rectangle de bordure (2px)
- Dessiner le rectangle de fond (bounds réduits de 2px)
- Dessiner le texte centré avec la police PT Sans Narrow 14pt
- Gérer l'état cliqué pour changer la couleur du texte en rouge

## 2. Ajout des boutons de test dans PluginEditor

### Modifications dans `PluginEditor.h`

- Ajouter `#include "UI/Widgets/McButton.h"`
- Ajouter les membres boutons :
- `McButton buttonI`, `buttonC`, `buttonP` (20x20)
- `McButton buttonUnlock` (75x20)
- `McButton buttonInit` (35x20)
- `McButton buttonStore` (40x20, désactivé)
- `McButton buttonBigText` (200x50)

### Modifications dans `PluginEditor.cpp`

- Dans le constructeur :
- Initialiser chaque bouton avec sa largeur
- Appeler `setLookAndFeel()` sur chaque bouton
- Définir le texte de chaque bouton (`setButtonText()`)
- Désactiver `buttonStore` avec `setEnabled(false)`
- Appeler `addAndMakeVisible()` pour chaque bouton
- Dans `resized()` :
- Placer les 3 premiers boutons (I, C, P) côte à côte sans espacement, sous les sliders
- Placer les 4 autres boutons avec un espacement de 5px entre eux, sous les premiers

### Positionnement

- Les boutons seront placés sous les sliders existants
- Calculer la position Y en fonction de la position des sliders dans `resized()`