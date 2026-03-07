---
name: Widgets scaling et couleurs
overview: "Adapter les widgets de Test-GuiScaling/Source/Widgets : couleurs en dur (variante Black de Matrix-Control), suppression du cache image, et implémentation du scaling factor comme sur l’ancien Button, en ne modifiant que le projet Test-GuiScaling (aucune modification dans Matrix-Control)."
todos: []
isProject: false
---

# Plan : widgets Test-GuiScaling – couleurs en dur, sans cache, avec scaling factor

## Contexte

- **Objectif** : Pouvoir tester le scaling vectoriel avec les vrais widgets (Button, Slider, ComboBox, Label, HorizontalSeparator) dans Test-GuiScaling, avec les mêmes principes que l’ancien bouton de test : rendu net à tout facteur d’échelle, coordonnées et dimensions recalculées à la volée.
- **Contraintes** : Ne pas toucher au projet Matrix-Control (lecture seule pour récupérer les couleurs). Toutes les modifications se font dans [Test-GuiScaling/Source/Widgets](Source/Widgets) et dans [MainComponent](Source/MainComponent.h) / build.

---

## 1. Couleurs en dur (variante Black de Matrix-Control)

Les valeurs ci‑dessous viennent de [Matrix-Control/Source/GUI/Skins/ColourChart.h](file:///Volumes/Guillaume/Dev/Projects/MAO/Plugins/Matrix-Control/Source/GUI/Skins/ColourChart.h) et des `SkinColours`* (Black = premier champ de chaque `ColourElement`).

**Button** (SkinColoursWidgetsControls.h)  

- Fond : `0xFF000000` (kBlack)  
- Bordure : `0xFF303030` (kDarkGrey3)  
- Texte : `0xFFB9B9B9` (kLightGrey2)  
- Texte hover : `0xFFFFFFFF` (kWhite)  
- Texte cliqué : `0xFFFF0000` (kRed)

**Slider** (SkinColoursWidgetsControls.h)  

- Track : `0xFF002D0E` (kGreen1), désactivé : `0xFF505050` (kDarkGrey5)  
- Value bar : `0xFF00785E` (kGreen3), désactivé : `0xFF505050`  
- Texte : `0xFF00DDAD` (kGreen4), désactivé : `0xFF505050`  
- Focus border : `0xFF06471C` (kGreen2)

**ComboBox**  

- Standard : fond `0xFF002D0E`, triangle `0xFF00785E`, texte `0xFF00DDAD`, focus border `0xFF06471C`, états désactivés `0xFF303030` / `0xFF505050`.  
- ButtonLike : fond `0xFF000000`, bordure `0xFF303030`, texte `0xFFB9B9B9`, triangle `0xFF505050`, désactivés comme ci‑dessus.

**Label** (SkinColoursWidgetsLayout.h)  

- Texte : `0xFFB9B9B9` (kLightGrey2)

**HorizontalSeparator** (SkinColoursWidgetsLayout.h)  

- Ligne : `0xFF505050` (kDarkGrey5)

**Popup menus** (SkinColoursWidgetsSelection.h, variante Black)  

- Standard : fond `0xFF002D0E`, bordure/séparateur/texte `0xFF00785E` (kGreen4), hover fond `0xFF00785E`, texte hover `0xFF101010` (kDarkGrey1).  
- ButtonLike : fond `0xFF000000`, bordure `0xFF505050`, séparateur `0xFF303030`, texte `0xFFB9B9B9`, hover fond `0xFFB9B9B9`, texte hover `0xFF000000`, scrollbar `0xFF303030`.

À faire : dans chaque classe, définir des constantes privées (par ex. `static const juce::Colour kXxxColour_`) ou des `static constexpr uint32` puis `juce::Colour(xxx)`, et les utiliser partout à la place de `skin_->get...()`.

---

## 2. Suppression du mécanisme de cache

Dans chaque widget concerné :

- **Button**  
  - Supprimer : `std::map<ButtonState, juce::Image> cachedImages_`, `cacheValid_`, `regenerateCache()`, `invalidateCache()`, `getPixelScale()`.  
  - Dans `paintButton` : dessiner directement (comme dans l’ancien `renderButtonState`), sans image, en utilisant les couleurs en dur et une police / épaisseur de bordure dérivées du `scalingFactor_` (voir point 3).
- **Slider**  
  - Supprimer : `juce::Image cachedImage_`, `cacheValid_`, `cachedValue_`, `cachedTrackColour_`, `cachedValueBarColour_`, `cachedTextColour_`, `cachedFocusBorderColour_`, `cachedFont_`, `regenerateCache()`, `invalidateCache()`, `updateSkinCache()`, `getPixelScale()`.  
  - Dans `paint()` : appeler directement `drawTrack`, `drawValueBar`, `drawText`, `drawFocusBorderIfNeeded` en utilisant les constantes de couleurs et une font / hauteur de piste calculées avec `scalingFactor_`. Ne plus invalider de cache dans `resized()` / `focusGained` / `focusLost` (juste `repaint()` si besoin).
- **ComboBox**  
  - Même principe : supprimer `cachedImage_`, `cacheValid_`, `cachedSelectedIndex_`, cache de couleurs/font, `regenerateCache()`, `invalidateCache()`, `updateSkinCache()`, `getPixelScale()`.  
  - Dans `paint()` : dessiner directement fond, texte, triangle, bordure avec les constantes et le scaling (police, épaisseurs, tailles de triangle).
- **Label**  
  - Supprimer : `cachedImage_`, `cacheValid_`, `cachedTextColour_`, `cachedFont_`, `regenerateCache()`, `invalidateCache()`, `updateSkinCache()`, `getPixelScale()`.  
  - Dans `paint()` : dessiner le texte avec la couleur en dur et une police dont la hauteur dépend de `scalingFactor_`.
- **HorizontalSeparator**  
  - Supprimer : `cachedImage_`, `cacheValid_`, `cachedLineColour_`, `regenerateCache()`, `invalidateCache()`, `getPixelScale()`.  
  - Dans `paint()` : tracer la ligne avec la couleur en dur et une épaisseur éventuellement scaled.
- **PopupMenuBase / PopupMenuRenderer / ScrollablePopupMenu / MultiColumnPopupMenu**  
  - Remplacer l’usage de `ISkin` par des constantes de couleurs (valeurs PopupMenu ci‑dessus).  
  - Supprimer toute dépendance à `ISkin` (pointeurs, références, `setSkin`, etc.) et tout cache d’image si présent.  
  - Le `ComboBox` ne passera plus de `skin` aux popups ; les popups n’auront que des couleurs fixes (et optionnellement un facteur d’échelle si on veut les scaler plus tard).

---

## 3. Implémentation du scaling factor (comme l’ancien Button)

Pour chaque widget (Button, Slider, ComboBox, Label, HorizontalSeparator) :

- **API**  
  - Ajouter `void setScalingFactor(float inScale);` (avec garde du type `juce::approximatelyEqual` pour éviter repaint inutiles).  
  - Exposer les dimensions de base pour le layout du parent :  
    - Soit `static int getBaseWidth()` / `static int getBaseHeight()` (comme l’ancien Button),  
    - Soit conserver `width_` / `height_` comme dimensions de base et ajouter des accesseurs statics ou d’instance selon la convention actuelle (Button a déjà `getWidth()`/`getHeight()` qui renvoient les dimensions stockées ; le parent a besoin des dimensions “de design” pour calculer `scaledW = baseW * scale`).
- **Membre**  
  - `float scalingFactor_ = 1.0f;`
- **Dessin**  
  - Les **bounds** du widget sont déjà imposées par le parent (via `setBounds` dans `MainComponent::layoutButtons()`) : le parent applique `scale` aux positions et aux largeurs/hauteurs. Le widget reçoit donc déjà un rectangle “scaled”.  
  - À l’intérieur du widget :  
    - **Police** : hauteur de base (ex. 14.f ou celle utilisée dans Matrix-Control) multipliée par `scalingFactor_` pour le rendu (texte net à toute échelle).  
    - **Épaisseurs** (bordure, ligne, track height, etc.) : multiplier par `scalingFactor_` (avec un minimum à 1 si besoin).  
    - **Tailles internes** (ex. triangle ComboBox, padding) : idem, en proportion du scale.
  - Ne plus utiliser `getPixelScale()` (HiDPI) pour la mise en cache ; tout est piloté par le scaling factor logique du plugin.
- **Constructeurs**  
  - Supprimer le paramètre `ISkin&` et `setSkin`.  
  - Garder les paramètres de taille (width, height) comme dimensions de base ; le parent continuera à appeler `setBounds(x, y, baseW * scale, baseH * scale)`.
- **Popup menus**  
  - Pour cette phase, on peut laisser les popups sans scaling (ou leur passer un scale plus tard). Priorité : qu’ils compilent et fonctionnent avec des couleurs en dur, sans ISkin.

---

## 4. Ordre des modifications par fichier

- **Button** ([Source/Widgets/Button.h](Source/Widgets/Button.h), [Source/Widgets/Button.cpp](Source/Widgets/Button.cpp))  
  - Couleurs en dur, suppression cache et ISkin, ajout `setScalingFactor` + `getBaseWidth`/`getBaseHeight`, dessin direct dans `paintButton` avec font et bordure scalées.
- **Slider** ([Source/Widgets/Slider.h](Source/Widgets/Slider.h), [Source/Widgets/Slider.cpp](Source/Widgets/Slider.cpp))  
  - Idem : couleurs, suppression cache et ISkin, scaling factor, dessin direct dans `paint()` avec track height et font scalées.
- **Label** ([Source/Widgets/Label.h](Source/Widgets/Label.h), [Source/Widgets/Label.cpp](Source/Widgets/Label.cpp))  
  - Couleurs, suppression cache et ISkin, scaling factor, dessin direct dans `paint()`.
- **HorizontalSeparator** ([Source/Widgets/HorizontalSeparator.h](Source/Widgets/HorizontalSeparator.h), [Source/Widgets/HorizontalSeparator.cpp](Source/Widgets/HorizontalSeparator.cpp))  
  - Couleurs, suppression cache et ISkin, scaling factor, dessin direct.
- **ComboBox** ([Source/Widgets/ComboBox.h](Source/Widgets/ComboBox.h), [Source/Widgets/ComboBox.cpp](Source/Widgets/ComboBox.cpp))  
  - Couleurs (Standard + ButtonLike), suppression cache et ISkin, scaling factor, dessin direct ; `drawTriangle` et le reste utilisent les constantes et tailles scalées.
- **PopupMenuRenderer, PopupMenuBase, ScrollablePopupMenu, MultiColumnPopupMenu**  
  - Remplacer ISkin par des constantes de couleurs (PopupMenu Standard + ButtonLike), supprimer tout pointeur/référence ISkin et tout cache image. Adapter les constructeurs et appels depuis ComboBox (plus de passage de `skin`).

---

## 5. MainComponent et build

- **MainComponent** ([Source/MainComponent.h](Source/MainComponent.h), [Source/MainComponent.cpp](Source/MainComponent.cpp))  
  - Utiliser `tss::Button` depuis les widgets :  
    - Include : `#include "Widgets/Button.h"` (ou chemin adapté selon les includes du projet).  
    - Constructeur : `tss::Button(50, 20, "50%")` etc. (sans ISkin), en gardant les mêmes libellés 50%, 75%, …, 400%.  
    - Callback clic : utiliser `juce::Button::onClick` (ex. `guiScaling50Button.onClick = [this] { setScalingFactor(0.5f); };`).  
    - Dans `applyScalingFactorToButtons()` : appeler `setScalingFactor(scalingFactor_)` sur chaque bouton.  
    - Dans `layoutButtons()` : utiliser `tss::Button::getBaseWidth()` et `tss::Button::getBaseHeight()` (ou les accesseurs que tu auras définis) pour calculer `buttonW`, `buttonH` puis `setBounds` avec positions et tailles scalées.
- **CMake**  
  - S’assurer que les sources des widgets sont compilées : [CMakeLists.txt](CMakeLists.txt) doit référencer les `.cpp` de [Source/Widgets](Source/Widgets) (ex. `Source/Widgets/Button.cpp`, `Source/Widgets/Slider.cpp`, etc.) et plus un éventuel `Source/Button.cpp` orphelin. Adapter la liste `PLUGIN_SOURCES` en conséquence.

---

## 6. Résumé des fichiers à modifier (Test-GuiScaling uniquement)


| Fichier                                                                             | Actions principales                                                                                     |
| ----------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------- |
| [Source/Widgets/Button.h](Source/Widgets/Button.h) / .cpp                           | Couleurs, suppression cache/ISkin, setScalingFactor, getBaseWidth/Height, paintButton direct avec scale |
| [Source/Widgets/Slider.h](Source/Widgets/Slider.h) / .cpp                           | Idem + dessin direct dans paint()                                                                       |
| [Source/Widgets/Label.h](Source/Widgets/Label.h) / .cpp                             | Idem                                                                                                    |
| [Source/Widgets/HorizontalSeparator.h](Source/Widgets/HorizontalSeparator.h) / .cpp | Idem                                                                                                    |
| [Source/Widgets/ComboBox.h](Source/Widgets/ComboBox.h) / .cpp                       | Idem + triangle et styles Standard/ButtonLike                                                           |
| PopupMenu*.h/.cpp                                                                   | Remplacer ISkin par constantes couleurs, supprimer skin_ / setSkin                                      |
| [Source/MainComponent.h](Source/MainComponent.h) / .cpp                             | Utiliser tss::Button(50, 20, "50%"), onClick, setScalingFactor, getBaseWidth/Height pour layout         |
| [CMakeLists.txt](CMakeLists.txt)                                                    | Inclure tous les .cpp de Source/Widgets nécessaires, pas de Source/Button.cpp si supprimé               |


Aucune modification dans le projet Matrix-Control.