# Document de passation : widgets et scaling → Matrix-Control

Ce document décrit les transformations et patterns validés dans le projet **Test-GuiScaling**, afin de permettre la reprise du chantier dans le projet **Matrix-Control** (refonte des widgets custom, scaling factor, et injection de Look à la place d’ISkin).

---

## 1. Contexte et objectifs

- **Test-GuiScaling** : projet de test où les widgets ont été découplés d’ISkin (couleurs/polices en dur) pour valider rapidement le scaling et le dessin.
- **Chantier à poursuivre dans Matrix-Control** :
  1. Appliquer les **mêmes patterns** (scaling factor, dessin en float, suppression de cache inutile) aux widgets Matrix-Control non encore traités.
  2. Introduire le **concept Look** dans tous les widgets : plus de dépendance à ISkin dans les classes widget ; les panels (ou une couche au-dessus) construisent des structs Look à partir d’ISkin et les injectent via `setLook(...)`.

---

## 2. Patterns communs

### 2.1 Scaling factor

- **Membre** : chaque widget possède `float scalingFactor_ = 1.0f`.
- **Setter** : `void setScalingFactor(float inScale)` avec garde pour éviter les repaints inutiles :
  ```cpp
  void setScalingFactor(float scalingFactor)
  {
      if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
          return;
      scalingFactor_ = scalingFactor;
      repaint();
  }
  ```
- **Propagation** : le composant parent (ex. MainComponent) garde un `scalingFactor_` et, quand il change, appelle `setScalingFactor(scalingFactor_)` sur **tous** les widgets enfants (buttons, labels, sliders, combos, separators, etc.).
- **Usage** : toute dimension ou épaisseur “logique” est multipliée par `scalingFactor_` au moment du calcul (paint, layout, bounds). Exemples : `kFontSize_ * scalingFactor_`, `static_cast<float>(kBorderThickness_) * scalingFactor_`, `static_cast<float>(kItemHeight_) * scalingFactor_`.

### 2.2 Dessin vectoriel : float partout, un seul arrondi

- **Bounds en float** : dans `paint()` et dans les helpers de dessin, travailler avec `juce::Rectangle<float>` (ex. `getLocalBounds().toFloat()`).
- **Dimensions scalées en float** : les constantes (épaisseur de bordure, hauteur de piste, padding, etc.) sont converties en float puis multipliées par `scalingFactor_`. Ex. : `const float borderThickness = std::max(1.0f, static_cast<float>(kBorderThickness_) * scalingFactor_);`
- **Un seul arrondi en dernière opération** : quand une API exige des `int` (ex. `setBounds`, `setSize`, `setScrollBarThickness`, dimensions du popup), faire **un seul** arrondi à la fin : `juce::roundToInt(...)` ou `juce::jmax(1, juce::roundToInt(...))` pour éviter des valeurs nulles. Ne pas chaîner plusieurs arrondis intermédiaires qui créent des décalages (ex. 4 px à gauche et 2 px à droite au lieu de 3 px partout).
- **Épaisseurs minimales** : pour les traits (bordure, ligne) et espacements, utiliser `std::max(1.0f, ...)` ou `juce::jmax(1, ...)` pour qu’à 50 % de scale on ne tombe pas à 0.

### 2.3 Suppression du cache d’images

- Dans Matrix-Control, les mauvaises performances initiales du plugin imputées (à priori à tort) au grand nombre de widgets utilisés avaient conduit à gérer ces widgets avec un pré-rendu de tous les états dans des `juce::Image` (cache HiDPI) pour éviter de redessiner. Dans Test-GuiScaling ce cache a été **supprimé** et les performances étaient excellentes avec près de 730 widgets créés dans la GUI : le widget redessine directement dans `paintButton` / `paint` à chaque frame avec les bounds et le scaling factor actuels. C’est plus simple et évite les invalidations de cache et les bugs de synchronisation. À conserver pour les widgets refactorisés : **dessin direct**, pas de `cachedImages_` ni `regenerateCache()`.

### 2.4 Dimensions de base (getBaseWidth / getBaseHeight)

- Les widgets exposent des dimensions “de base” en pixels logiques (ex. `Button::getBaseWidth()` = 50, `getBaseHeight()` = 20). Le **parent** utilise ces valeurs pour le layout : `int w = juce::roundToInt(static_cast<float>(Button::getBaseWidth()) * scalingFactor_);` puis `button->setBounds(..., w, h);`. Les widgets ne stockent pas de “default” width/height s’ils reçoivent toujours width/height au constructeur ; les constantes de base servent uniquement au layout côté parent.

---

## 3. Concept Look

### 3.1 Principe

- Chaque type de widget a un **struct Look** (ex. `ButtonLook`, `SliderLook`, `ComboBoxLook`) qui agrège **uniquement des données** : `juce::Colour`, `juce::Font`, pas d’interface ni de pointeur ISkin.
- Le widget stocke un membre du type `ButtonLook look_` (ou équivalent) et n’a **plus** de membres du type `juce::Colour kBackgroundColour_` ni de référence à ISkin.
- Injection : soit au constructeur `Button(const ButtonLook& look, int width, int height, const juce::String& text)`, soit après construction avec `void setLook(const ButtonLook& look);`. Après `setLook`, appeler `repaint()`

### 3.2 Côté Matrix-Control (panels)

- Les **panels** (ou une factory / helper) connaissent ISkin. Ils construisent le Look une fois à partir du skin, puis l’injectent dans le widget :
  ```cpp
  tss::ButtonLook buttonLookFromSkin(const tss::ISkin& skin)
  {
      tss::ButtonLook look;
      look.backgroundNormal   = skin.getColour(SkinColourId::kButtonBackgroundOn);
      look.backgroundHover    = skin.getColour(SkinColourId::kButtonBackgroundHover);
      // ... tous les champs
      look.font               = skin.getBaseFont();
      return look;
  }
  // Création
  auto* button = new tss::Button(80, 24, "Apply");
  button->setLook(buttonLookFromSkin(*skin_));
  addAndMakeVisible(button);
  ```
- Le widget ne dépend plus d’aucune interface ISkin ; il ne dépend que de `juce::Colour`, `juce::Font` et de son struct Look.

### 3.3 Exemple de struct Look (Button)

```cpp
struct ButtonLook
{
    juce::Colour backgroundNormal{};
    juce::Colour backgroundHover{};
    juce::Colour backgroundPressed{};
    juce::Colour backgroundDisabled{};
    juce::Colour borderEnabled{};
    juce::Colour borderDisabled{};
    juce::Colour textNormal{};
    juce::Colour textHover{};
    juce::Colour textPressed{};
    juce::Colour textDisabled{};
    juce::Font font{};
};
```

Les getters internes du widget (ex. `getBackgroundColour(bool enabled, bool isHighlighted, bool isDown)`) lisent alors dans `look_` au lieu d’appeler `skin_->getColour(...)`.

---

## 4. Pièges et correctifs (ScrollablePopupMenu et généralisables)

Ces points ont été rencontrés et corrigés dans Test-GuiScaling ; ils sont réutilisables tels quels dans Matrix-Control pour les popups scrollables et tout widget avec viewport / zones cliquables.

### 4.1 Hit-test en coordonnées locales

- **Problème** : pour savoir sur quel item de liste la souris se trouve, un test du type `contentComponent_->getBounds().contains(x, y)` peut échouer car `getBounds()` est en coordonnées du parent. Les événements souris dans le content component sont en **coordonnées locales** du content.
- **Correctif** : utiliser `contentComponent_->getLocalBounds().contains(x, y)` dans `getItemIndexAt(int x, int y)`. Sinon, au-delà d’un certain nombre d’items (ex. à partir de l’item 23), le hit-test ne couvrait plus toute la zone visible et le survol ne mettait plus en surbrillance.

### 4.2 Inset uniforme (viewport / contenu)

- **Problème** : en calculant les bounds du viewport avec `getLocalBounds().toFloat().reduced(borderThickness).toNearestIntEdges()`, avec `borderThickness = 1.5` (ex. à 150 %), l’arrondi par edge donnait 2 px à gauche/haut et 1 px à droite/bas → espacements asymétriques.
- **Correctif** : utiliser un **seul** inset entier pour les quatre côtés :
  ```cpp
  const int insetPx = juce::jmax(1, juce::roundToInt(kBorderThickness_ * scalingFactor_));
  viewport_->setBounds(getLocalBounds().reduced(insetPx));
  ```
  Et imposer un **minimum de 1** pour éviter un inset à 0 à 50 %.

### 4.3 Gap minimum pour le rectangle de surbrillance du choix survolé dans le PopupMenu

- **Problème** : à 50 %, `getHighlightGap()` retournait `1.0f * scalingFactor_` = 0.5, ce qui provoquait des arrondis de tracé et un débordement du rectangle de surbrillance sur les bords du menu.
- **Correctif** : `float getHighlightGap() const { return juce::jmax(1.0f, 1.0f * scalingFactor_); }` pour garantir au moins 1 px logique.

### 4.4 Largeur du popup scrollable et place pour la scrollbar

- **Problème** : à 50 %, la largeur du popup était `columnWidth_ + rightMarginPx + borderThickness` avec un seul “border” pour les dimensions. Après réduction du viewport par `2*insetPx`, il restait **moins** de place que la largeur de la scrollbar → BVA collée au bord droit.
- **Correctif** : calculer la largeur du popup de façon cohérente avec l’inset et la scrollbar :
  ```cpp
  const int insetPx = juce::jmax(1, juce::roundToInt(kBorderThickness_ * rawPtr->scalingFactor_));
  const int scrollbarThicknessPx = juce::jmax(1, juce::roundToInt(kScrollbarWidth_ * rawPtr->scalingFactor_));
  const int rightMarginPx = juce::jmax(
      juce::roundToInt(kRightMarginFromHighlightToEdge_ * rawPtr->scalingFactor_),
      scrollbarThicknessPx);
  const int popupWidth = juce::roundToInt(rawPtr->columnWidth_) + rightMarginPx + 2 * insetPx;
  const int popupHeight = juce::roundToInt(viewportHeight) + 2 * insetPx;
  ```
  Ainsi le viewport a une largeur `popupWidth - 2*insetPx` ≥ `columnWidth_ + scrollbarThicknessPx`.

### 4.5 Épaisseur de la scrollbar

- Utiliser une épaisseur scalée avec minimum 1 :  
`viewport_->setScrollBarThickness(juce::jmax(1, juce::roundToInt(kScrollbarWidth_ * scalingFactor_)));`

---

## 5. Résumé par widget (Test-GuiScaling)

### 5.1 Button

- **Fichiers** : `Source/Widgets/Button.h`, `Button.cpp`.
- **Changements** : pas de cache d’images ; dessin direct dans `paintButton` avec `getLocalBounds().toFloat()`, `borderThickness = std::max(1.0f, static_cast<float>(kBorderThickness_) * scalingFactor_)`, police `kFontSize_ * scalingFactor_`. Couleurs en dur (constantes statiques) ; à remplacer par un membre `ButtonLook look_` et `setLook()` dans Matrix-Control.
- **Membres** : `width_`, `height_`, `scalingFactor_`. Pas de `kDefaultWidth_`/`kDefaultHeight_` utilisés si la taille est toujours passée au constructeur ; `getBaseWidth()`/`getBaseHeight()` restent pour le layout du parent.

### 5.2 Slider

- **Fichiers** : `Source/Widgets/Slider.h`, `Slider.cpp`.
- **Changements** : tout le dessin en `juce::Rectangle<float>` ; `calculateTrackBounds`, `calculateValueBarBounds` en float avec `kTrackHeight_ * scalingFactor_`, `reduction = std::max(1.0f, 1.0f * scalingFactor_)` ; `drawFocusBorderIfNeeded` avec `borderThickness = std::max(1.0f, 1.0f * scalingFactor_)`. Pas de cache. Couleurs en dur → à remplacer par `SliderLook` + `setLook()`.

### 5.3 Label

- **Fichiers** : `Source/Widgets/Label.h`, `Label.cpp`.
- **Changements** : `paint()` avec bounds en float, `padding = kTextLeftPadding_ * scalingFactor_`, `kFontSize_ * scalingFactor_`. `setScalingFactor()` avec garde et `repaint()`. Couleurs en dur → à remplacer par `LabelLook` si besoin.

### 5.4 HorizontalSeparator

- **Fichiers** : `Source/Widgets/HorizontalSeparator.h`, `HorizontalSeparator.cpp`.
- **Changements** : ligne horizontale avec `lineThickness = std::max(1.0f, static_cast<float>(kLineThickness_) * scalingFactor_)`. `setScalingFactor()` + repaint.

### 5.5 ComboBox

- **Fichiers** : `Source/Widgets/ComboBox.h`, `ComboBox.cpp`.
- **Changements** : `paint()` et helpers (`drawBackground`, `drawText`, `drawTriangle`, `drawBorderIfNeeded`) avec bounds en float ; tous les paddings, épaisseurs et tailles (triangle, background height) multipliés par `scalingFactor_`. `setScalingFactor()` propagé ; pas de cache. Styles Standard / ButtonLike avec couleurs en dur → à remplacer par Look (ex. `ComboBoxLook` avec champs pour les deux styles ou deux Look).

### 5.6 PopupMenuBase

- **Fichiers** : `Source/Widgets/PopupMenuBase.h`, `PopupMenuBase.cpp`.
- **Changements** : reçoit `ComboBox&` et récupère `scalingFactor_` via `comboBox.getScalingFactor()`. Constantes partagées : `kItemHeight_`, `kBorderThickness_`, `kHighlightGap_`, `kFontSize_`, `kFontTypefaceName_`. `cachedFont_` construite avec `kFontSize_ * scalingFactor_`. Pas d’ISkin ; couleurs déléguées au `PopupMenuRenderer`. Pour Matrix-Control : le Look du popup (couleurs, police) pourra être dérivé du ComboBoxLook ou passé séparément.

### 5.7 PopupMenuRenderer

- **Fichiers** : `Source/Widgets/PopupMenuRenderer.h`, `PopupMenuRenderer.cpp`.
- **Changements** : reçoit `(bool isButtonLike, float scalingFactor)` ; `drawBorder` avec `kBorderThickness = std::max(1.0f, 1.0f * scalingFactor_)` ; `drawItem` avec `getHighlightGap() = juce::jmax(1.0f, 1.0f * scalingFactor_)` et `textPadding = kTextLeftPadding_ * scalingFactor_`. Couleurs en dur → à terme fournies par un Look (ex. PopupMenuLook ou partie de ComboBoxLook).

### 5.8 ScrollablePopupMenu

- **Fichiers** : `Source/Widgets/ScrollablePopupMenu.h`, `ScrollablePopupMenu.cpp`.
- **Changements** :
  - `columnWidth_` et `scrollableContentHeight_` calculés avec `scalingFactor_` ; `getItemBounds` et `getItemIndexAt` en float / coordonnées locales (voir §4.1).
  - `resized()` : inset uniforme `insetPx = juce::jmax(1, juce::roundToInt(kBorderThickness_ * scalingFactor_))`, `viewport_->setBounds(getLocalBounds().reduced(insetPx))`.
  - `setupScrollableContent()` : `setScrollBarThickness(juce::jmax(1, juce::roundToInt(kScrollbarWidth_ * scalingFactor_)))`.
  - `show()` : calcul de `popupWidth` / `popupHeight` avec `insetPx`, `rightMarginPx`, `scrollbarThicknessPx` comme en §4.4 et §4.5.
- **Constantes** : `kMaxScrollableHeight_`, `kScrollbarWidth_`, `kRightMarginFromHighlightToEdge_` (ex. 8.0f). Couleurs (fond, scrollbar) en dur → à brancher sur Look si besoin.

### 5.9 MultiColumnPopupMenu

- **Fichiers** : `Source/Widgets/MultiColumnPopupMenu.h`, `MultiColumnPopupMenu.cpp`.
- **Changements** : mêmes principes que ScrollablePopupMenu pour bounds en float, `kItemHeight_ * scalingFactor_`, `kBorderThickness_ * scalingFactor_`, séparateurs et dimensions de popup. Pas de viewport ; layout en colonnes. Pour Matrix-Control : couleurs/polices via Look.

---

## 6. Layout et propagation du scale (MainComponent)

- Le parent garde `float scalingFactor_` et expose `setScalingFactor(float)`.
- Lors d’un changement de scale :
  1. Mettre à jour `scalingFactor_`.
  2. Appeler `setScalingFactor(scalingFactor_)` sur **tous** les widgets (boutons, labels, sliders, combos, separators).
  3. Appeler `resized()` (ou déclencher un recalcul de layout).
- Dans `resized()` (ou la fonction de layout), toutes les dimensions dérivées du scale sont calculées ainsi :  
`int w = juce::roundToInt(static_cast<float>(baseValue) * scalingFactor_);`  
puis `component->setBounds(x, y, w, h)`.
- Les popups (ScrollablePopupMenu, MultiColumnPopupMenu) héritent du scale via le `ComboBox` qui les ouvre : ils lisent `comboBox.getScalingFactor()` dans PopupMenuBase.

---

## 7. Checklist pour appliquer à un widget dans Matrix-Control

- [ ] Supprimer toute dépendance à `ISkin` dans la classe du widget (pointeur, référence, appels `skin_->...`).
- [ ] Introduire un struct `XxxLook` avec uniquement des champs `juce::Colour` et `juce::Font` (et valeurs numériques si besoin).
- [ ] Ajouter un membre `XxxLook look_` et une méthode `void setLook(const XxxLook& look);` (ou passer le Look au constructeur).
- [ ] Remplacer les lectures de couleurs/polices par des lectures dans `look_` (ou valeurs par défaut si pas encore de Look injecté).
- [ ] S’assurer que le widget a un membre `float scalingFactor_ = 1.0f` et une méthode `void setScalingFactor(float scalingFactor)` avec garde `approximatelyEqual` + `repaint()`.
- [ ] Vérifier que tout le dessin utilise des `juce::Rectangle<float>` et des dimensions/épaisseurs en `... * scalingFactor_`, avec un seul arrondi en dernier pour les API `int`.
- [ ] Supprimer tout cache d’images (pré-rendu d’états) si le dessin direct est suffisant.
- [ ] Côté panel/factory : ajouter une fonction (ex. `xxxLookFromSkin(const ISkin& skin)`) qui remplit le struct à partir d’ISkin et appeler `setLook(...)` après la création du widget.
- [ ] Pour les popups avec viewport / listes : appliquer les correctifs §4.1 à §4.5 (hit-test local, inset uniforme, gap minimum, largeur popup, épaisseur scrollbar).

---

## 8. Fichiers de référence dans Test-GuiScaling


| Widget / rôle        | Fichiers                                                            |
| -------------------- | ------------------------------------------------------------------- |
| Button               | `Source/Widgets/Button.h`, `Button.cpp`                             |
| Slider               | `Source/Widgets/Slider.h`, `Slider.cpp`                             |
| Label                | `Source/Widgets/Label.h`, `Label.cpp`                               |
| HorizontalSeparator  | `Source/Widgets/HorizontalSeparator.h`, `HorizontalSeparator.cpp`   |
| ComboBox             | `Source/Widgets/ComboBox.h`, `ComboBox.cpp`                         |
| PopupMenuBase        | `Source/Widgets/PopupMenuBase.h`, `PopupMenuBase.cpp`               |
| PopupMenuRenderer    | `Source/Widgets/PopupMenuRenderer.h`, `PopupMenuRenderer.cpp`       |
| ScrollablePopupMenu  | `Source/Widgets/ScrollablePopupMenu.h`, `ScrollablePopupMenu.cpp`   |
| MultiColumnPopupMenu | `Source/Widgets/MultiColumnPopupMenu.h`, `MultiColumnPopupMenu.cpp` |
| PopupMenuPositioner  | `Source/Widgets/PopupMenuPositioner.h`, `PopupMenuPositioner.cpp`   |
| Layout / propagation | `Source/MainComponent.h`, `Source/MainComponent.cpp`                |


Ce document peut être fourni à l’Agent IA dans Matrix-Control pour appliquer les mêmes transformations et le concept Look à l’ensemble des widgets custom.