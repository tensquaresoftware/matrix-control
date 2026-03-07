---
name: Refonte widgets custom et purge LookAndFeel
overview: Créer la classe McSlider custom, nettoyer PluginEditor des widgets de test, et purger McLookAndFeel pour ne conserver que les couleurs/thèmes.
todos:
  - id: "1"
    content: Créer Source/UI/Widgets/McSlider.h avec héritage de juce::Slider, constructeur configurant LinearBarVertical, taille 60x20, sensibilité 400
    status: completed
  - id: "2"
    content: Créer Source/UI/Widgets/McSlider.cpp avec override paint() utilisant McLookAndFeel pour les couleurs et le rendu custom
    status: completed
  - id: "3"
    content: "Nettoyer PluginEditor.h : supprimer parameterLabel, saveButton, slider1, slider2, comboBoxSmall, comboBoxLarge"
    status: completed
  - id: "4"
    content: "Nettoyer PluginEditor.cpp : supprimer toute l initialisation des widgets de test dans le constructeur et resized()"
    status: completed
  - id: "5"
    content: "Purger McLookAndFeel.h : supprimer toutes les méthodes draw* et get* de rendu, ne plus hériter de LookAndFeel_V4"
    status: completed
  - id: "6"
    content: "Purger McLookAndFeel.cpp : supprimer toutes les implémentations de rendu, ne garder que les getters de couleurs et utilitaires"
    status: completed
  - id: "7"
    content: Mettre à jour CMakeLists.txt pour ajouter Source/UI/Widgets/McSlider.cpp à PLUGIN_SOURCES
    status: completed
---

# Refonte : Widgets Custom et Purge LookAndFeel

## Objectifs
1. Créer la classe `McSlider` custom avec taille par défaut 60x20
2. Nettoyer `PluginEditor` pour supprimer tous les widgets de test
3. Purger `McLookAndFeel` pour ne conserver que les couleurs/thèmes

## Structure des fichiers

### Nouveaux fichiers à créer
- `Source/UI/Widgets/McSlider.h` - Déclaration de la classe McSlider
- `Source/UI/Widgets/McSlider.cpp` - Implémentation de McSlider

### Fichiers à modifier
- `Source/PluginEditor.h` - Supprimer les widgets de test
- `Source/PluginEditor.cpp` - Nettoyer le constructeur et resized()
- `Source/UI/LookAndFeel/McLookAndFeel.h` - Supprimer toutes les méthodes draw* et ne garder que couleurs/thèmes
- `Source/UI/LookAndFeel/McLookAndFeel.cpp` - Purger les implémentations de rendu
- `CMakeLists.txt` - Ajouter `Source/UI/Widgets/McSlider.cpp` à PLUGIN_SOURCES

## Détails d'implémentation

### McSlider (`Source/UI/Widgets/McSlider.h` et `.cpp`)
- Hérite de `juce::Slider`
- Constructeur :
  - Style : `LinearBarVertical` (glisser vertical)
  - Taille par défaut : 60x20 pixels (`setSize(60, 20)`)
  - Sensibilité : `setMouseDragSensitivity(400)`
  - Pas de TextBox (`NoTextBox`)
- Override `paint()` :
  - Utilise `McLookAndFeel` pour obtenir les couleurs (via pointeur ou référence)
  - Fond : `getSliderBackgroundColour()`
  - Track rempli : `getSliderTrackColour()` (proportionnel à la valeur)
  - Texte valeur : `getSliderTextColour()` centré, police `getDefaultFont()`
  - Calcul de la largeur du track : `normalizedValue * width`
- Méthode publique `setLookAndFeel(McLookAndFeel*)` pour recevoir le LookAndFeel

### PluginEditor (`Source/PluginEditor.h` et `.cpp`)
- Supprimer tous les membres de test :
  - `parameterLabel`
  - `saveButton`
  - `slider1`, `slider2`
  - `comboBoxSmall`, `comboBoxLarge`
- Conserver uniquement :
  - `pluginVersionLabel` (pour afficher la version)
  - `lookAndFeel` (pour les couleurs/thèmes)
- Nettoyer `resized()` : ne garder que le positionnement de `pluginVersionLabel`
- Nettoyer le constructeur : supprimer toute l'initialisation des widgets de test

### McLookAndFeel (`Source/UI/LookAndFeel/McLookAndFeel.h` et `.cpp`)
- **Conserver** :
  - `enum class Theme` et méthodes `setTheme()`, `getTheme()`
  - Tous les getters de couleurs (public) :
    - `getParameterLabelBackgroundColour()`, `getParameterLabelTextColour()`
    - `getButtonBackgroundColour()`, `getButtonBorderColour()`, `getButtonTextColour()`
    - `getSliderBackgroundColour()`, `getSliderTrackColour()`, `getSliderTextColour()`
    - `getComboBoxBackgroundColour()`, `getComboBoxTextColour()`, `getComboBoxTriangleColour()`
    - `getPopupMenuBackgroundColour()`, `getPopupMenuTextColour()`, `getPopupMenuHighlightColour()`, `getPopupMenuBorderColour()`
  - Méthodes privées utilitaires :
    - `makeDisabledColour()`
    - `getDefaultFont()`
- **Supprimer** :
  - Toutes les méthodes `draw*()` (drawLabel, drawButtonBackground, drawButtonText, drawLinearSlider, drawComboBox, drawComboBoxTextWhenNothingSelected, drawPopupMenuBackground, drawPopupMenuItem)
  - Toutes les méthodes `get*()` de rendu (getComboBoxFont, getPopupMenuFont, getPopupMenuBorderSize, getPopupMenuItemHeight, getMenuWindowFlags)
  - Ne plus hériter de `juce::LookAndFeel_V4` (devenir une classe simple de gestion de couleurs/thèmes)

### CMakeLists.txt
- Ajouter `Source/UI/Widgets/McSlider.cpp` dans la liste `PLUGIN_SOURCES` après `Source/UI/LookAndFeel/McLookAndFeel.cpp`

## Notes importantes
- `McLookAndFeel` devient une classe utilitaire simple (pas un LookAndFeel JUCE)
- Les widgets custom utiliseront `McLookAndFeel` via pointeur/référence pour obtenir les couleurs
- La police par défaut reste à 14.0f (PT Sans Narrow si disponible)
- Les couleurs du thème Black sont conservées telles quelles