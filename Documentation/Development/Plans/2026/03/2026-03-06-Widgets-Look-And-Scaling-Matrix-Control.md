---
name: Widgets Look et scaling Matrix-Control
overview: "Refactoriser les widgets custom de Matrix-Control en appliquant les patterns validés dans Test-GuiScaling : concept Look (remplacement d’ISkin par des structs de données injectés), scaling factor propagé, dessin vectoriel en float avec un seul arrondi, suppression du cache d’images, et correctifs popups (hit-test, inset, largeur, scrollbar). L’échelle sera appliquée par layout et dessin plutôt que par AffineTransform global."
todos: []
isProject: false
---

# Refactorisation widgets Matrix-Control : Look, scaling factor, dessin direct

## Contexte actuel

- **Scale** : appliquée via `juce::AffineTransform::scale(scaleFactor)` sur tout le `MainComponent` dans [PluginEditor.cpp](Source/GUI/PluginEditor.cpp) (l.104), ce qui provoque le flou. Le facteur est déjà lu depuis le combo (50 %–400 %) et persisté dans l’APVTS.
- **Widgets** : tous dépendent d’`ISkin` (pointeur/référence) et beaucoup utilisent un cache d’images (`cachedImage_` / `cachedImages_`, `regenerateCache()`). Aucun n’a de `scalingFactor_` ni de dessin en float avec un seul arrondi.
- **Référence** : document de passation [PASSATION-MATRIX-CONTROL.md](/Volumes/Guillaume/Dev/Tests/JUCE/Concepts/Test-GuiScaling/docs/PASSATION-MATRIX-CONTROL.md) et implémentation Test-GuiScaling dans `Source/Widgets/` (lecture seule).

---

## 1. Infrastructure : scaling et Look

### 1.1 Scaling factor au niveau racine et propagation

- **PluginEditor** : dans `applyGuiScale(float scaleFactor)` :
  - Conserver `setSize(roundToInt(baseWidth * scaleFactor), roundToInt(baseHeight * scaleFactor))`.
  - **Supprimer** `setTransform(juce::AffineTransform::scale(scaleFactor))`.
  - Appeler `MainComponent::setScalingFactor(scaleFactor)` puis déclencher un recalcul de layout (ex. `mainComponent->resized()` ou méthode dédiée).
- **MainComponent** :
  - Ajouter `float scalingFactor_ = 1.0f` et `void setScalingFactor(float)`.
  - Dans `setScalingFactor` : mise à jour de `scalingFactor_`, puis propagation de `setScalingFactor(scalingFactor_)` à `headerPanel`, `bodyPanel`, `footerPanel`.
  - Dans `resized()` et `layoutHeaderPanel` / `layoutBodyPanel` / `layoutFooterPanel` : utiliser des dimensions scalées avec **un seul** arrondi en fin de calcul, ex. `juce::roundToInt(static_cast<float>(PluginDimensions::Panels::Header::kHeight) * scalingFactor_)`.
- **Panels** (HeaderPanel, BodyPanel, FooterPanel, puis tous les sous-panels qui posent des bounds) :
  - Chaque panel qui effectue du layout reçoit le scale (via paramètre, getter du parent, ou stockage local mis à jour par le parent).
  - Dans leur `resized()` : appeler `setScalingFactor(scalingFactor_)` sur **tous** les widgets enfants, et calculer les bounds avec `roundToInt(static_cast<float>(baseValue) * scalingFactor_)`.

La chaîne de propagation doit couvrir tous les widgets (Button, Slider, Label, ComboBox, HorizontalSeparator, Toggle, etc.) jusqu’aux feuilles. Les panels qui créent des widgets (ParameterPanel, ModulationBusPanel, HeaderPanel, etc.) devront donc avoir accès au `scalingFactor_` (remonté depuis MainComponent) et appeler `setScalingFactor` sur chaque enfant.

### 1.2 Structs Look et builders

- **Emplacement** : ajouter un module dédié (ex. `Source/GUI/Looks/`) pour garder les widgets indépendants d’ISkin :
  - Fichiers types : `WidgetLooks.h` (déclarations des structs) et `LookBuilders.cpp` (définitions des fonctions `xxxLookFromSkin(const ISkin&)`).
- **Structs** (données uniquement : `juce::Colour`, `juce::Font`) :
  - `ButtonLook`, `SliderLook`, `LabelLook`, `HorizontalSeparatorLook`, `ComboBoxLook` (couleurs pour Standard et ButtonLike si besoin), `PopupMenuLook` (ou réutilisation de champs ComboBoxLook / séparé).
  - Pour les widgets du document : Button, Slider, Label, HorizontalSeparator, ComboBox ; les popups peuvent partager un Look dérivé des couleurs ComboBox/Popup (voir §3.2 du document).
- **Builders** : une fonction par type, ex. `ButtonLook buttonLookFromSkin(const tss::ISkin& skin)` qui remplit le struct à partir de `skin.getColour(SkinColourId::...)` et `skin.getBaseFont()` (ou équivalent). S’appuyer sur [ISkinColours.h](Source/GUI/Skins/ISkinColours.h) et [SkinValues.h](Source/GUI/Skins/SkinValues.h) pour les IDs existants.

---

## 2. Widgets ciblés par le document de passation

Pour chaque widget ci-dessous, appliquer la **checklist §7** du document : suppression ISkin/cache, ajout Look + `setLook`, ajout `scalingFactor_` + `setScalingFactor` avec garde `approximatelyEqual` + `repaint()`, dessin en `Rectangle<float>` et dimensions/épaisseurs en `... * scalingFactor_` avec un seul arrondi en dernier, et `getBaseWidth()`/`getBaseHeight()` pour le layout parent.

### 2.1 Button

- [Source/GUI/Widgets/Button.h](Source/GUI/Widgets/Button.h), [Button.cpp](Source/GUI/Widgets/Button.cpp)
- Remplacer `ISkin`* et `setSkin` par `ButtonLook look_` et `setLook(const ButtonLook&)`.
- Supprimer `cachedImages_` et `regenerateCache()` ; dessin direct dans `paintButton` avec `getLocalBounds().toFloat()`, `borderThickness = std::max(1.0f, static_cast<float>(kBorderThickness_) * scalingFactor_)`, police avec `kFontSize_ * scalingFactor_`.
- Exposer `getBaseWidth()` / `getBaseHeight()` pour le layout (constantes déjà présentes ou à extraire).

### 2.2 Slider

- [Source/GUI/Widgets/Slider.h](Source/GUI/Widgets/Slider.h), [Slider.cpp](Source/GUI/Widgets/Slider.cpp)
- Idem : `SliderLook look_`, `setLook`, suppression cache.
- Tout le dessin en `Rectangle<float>` ; `calculateTrackBounds` / `calculateValueBarBounds` avec `kTrackHeight_ * scalingFactor_`, `reduction = std::max(1.0f, 1.0f * scalingFactor_)` ; focus border avec épaisseur scalée (min 1).

### 2.3 Label

- [Source/GUI/Widgets/Label.h](Source/GUI/Widgets/Label.h), [Label.cpp](Source/GUI/Widgets/Label.cpp)
- `LabelLook look_`, `setLook`, suppression cache.
- `paint()` avec bounds en float, `padding = kTextLeftPadding_ * scalingFactor_`, `kFontSize_ * scalingFactor_`.

### 2.4 HorizontalSeparator

- [Source/GUI/Widgets/HorizontalSeparator.h](Source/GUI/Widgets/HorizontalSeparator.h), [HorizontalSeparator.cpp](Source/GUI/Widgets/HorizontalSeparator.cpp)
- `HorizontalSeparatorLook look_` (une couleur de ligne), `setLook`, suppression cache.
- Ligne avec `lineThickness = std::max(1.0f, static_cast<float>(kLineThickness_) * scalingFactor_)`.

### 2.5 ComboBox

- [Source/GUI/Widgets/ComboBox.h](Source/GUI/Widgets/ComboBox.h), [ComboBox.cpp](Source/GUI/Widgets/ComboBox.cpp)
- `ComboBoxLook look_`, `setLook`, suppression `cachedImage_` et `regenerateCache()`.
- `paint()` et helpers (`drawBackground`, `drawText`, `drawTriangle`, `drawBorderIfNeeded`) en float ; paddings, épaisseurs, tailles (triangle, background height) multipliés par `scalingFactor_`.
- Ajouter `float getScalingFactor() const { return scalingFactor_; }` et `int getBaseComponentWidth() const { return width_; }` pour les popups (voir Test-GuiScaling ComboBox.h).

### 2.6 PopupMenuBase

- [Source/GUI/Widgets/PopupMenuBase.h](Source/GUI/Widgets/PopupMenuBase.h), [PopupMenuBase.cpp](Source/GUI/Widgets/PopupMenuBase.cpp)
- Récupérer le scale via `comboBox_.getScalingFactor()` (stockage dans un membre `float scalingFactor_` initialisé au constructeur) au lieu de dépendre d’ISkin pour le scale.
- Remplacer `ISkin`* par un Look (PopupMenuLook) ou par des couleurs passées par le renderer ; le renderer recevra le Look ou les couleurs (construites côté ComboBox/panel). Construire `cachedFont_` avec `kFontSize_ * scalingFactor_`.

### 2.7 PopupMenuRenderer

- [Source/GUI/Widgets/PopupMenuRenderer.h](Source/GUI/Widgets/PopupMenuRenderer.h), [PopupMenuRenderer.cpp](Source/GUI/Widgets/PopupMenuRenderer.cpp)
- Remplacer `ISkin&` par un struct Look (PopupMenuLook) ou paramètres (couleurs + police) + `float scalingFactor` passé aux méthodes de dessin.
- `drawBorder` avec `kBorderThickness = std::max(1.0f, 1.0f * scalingFactor)` ; `drawItem` avec `getHighlightGap() = juce::jmax(1.0f, 1.0f * scalingFactor)` et `textPadding = kTextLeftPadding_ * scalingFactor`.

### 2.8 ScrollablePopupMenu

- [Source/GUI/Widgets/ScrollablePopupMenu.h](Source/GUI/Widgets/ScrollablePopupMenu.h), [ScrollablePopupMenu.cpp](Source/GUI/Widgets/ScrollablePopupMenu.cpp)
- Utiliser `comboBox_.getScalingFactor()` pour `scalingFactor_` (déjà prévu si PopupMenuBase le stocke depuis le ComboBox).
- **§4.1** : `getItemIndexAt(int x, int y)` : utiliser `contentComponent_->getLocalBounds().contains(x, y)` et calcul d’index basé sur la hauteur d’item scalée (coordonnées locales du content).
- **§4.2** : `resized()` : inset uniforme `insetPx = juce::jmax(1, juce::roundToInt(kBorderThickness_ * scalingFactor_))`, `viewport_->setBounds(getLocalBounds().reduced(insetPx))`.
- **§4.3** : gap minimum pour le rectangle de surbrillance : `getHighlightGap() = juce::jmax(1.0f, 1.0f * scalingFactor_)`.
- **§4.4** : dans `show()` : calcul de `popupWidth` / `popupHeight` avec `insetPx`, `scrollbarThicknessPx`, `rightMarginPx` comme dans le document (largeur popup = columnWidth scalé + rightMarginPx + 2*insetPx, etc.).
- **§4.5** : `setScrollBarThickness(juce::jmax(1, juce::roundToInt(kScrollbarWidth_ * scalingFactor_)))`.
- `columnWidth_` et `scrollableContentHeight_` calculés avec `scalingFactor_` ; `getItemBounds` en float / hauteur d’item scalée.
- Couleurs : fournies par un Look (injecté depuis le ComboBox ou le popup) au lieu de `skin_`.

### 2.9 MultiColumnPopupMenu

- [Source/GUI/Widgets/MultiColumnPopupMenu.h](Source/GUI/Widgets/MultiColumnPopupMenu.h), [MultiColumnPopupMenu.cpp](Source/GUI/Widgets/MultiColumnPopupMenu.cpp)
- Mêmes principes : bounds en float, `kItemHeight_ * scalingFactor_`, `kBorderThickness_ * scalingFactor_`, dimensions de popup cohérentes ; couleurs via Look.

### 2.10 PopupMenuPositioner

- [Source/GUI/Widgets/PopupMenuPositioner.h](Source/GUI/Widgets/PopupMenuPositioner.h), [PopupMenuPositioner.cpp](Source/GUI/Widgets/PopupMenuPositioner.cpp)
- L’appelant (ScrollablePopupMenu::show / MultiColumnPopupMenu) passera des dimensions déjà calculées avec le scale (largeur/hauteur popup en pixels entiers). S’assurer que la signature et les calculs de position restent cohérents avec les nouveaux calculs de dimensions (§4.4).

---

## 3. Panels et factory : injection Look et propagation scale

### 3.1 WidgetFactory

- [Source/GUI/Factories/WidgetFactory.h](Source/GUI/Factories/WidgetFactory.h), [WidgetFactory.cpp](Source/GUI/Factories/WidgetFactory.cpp)
- Les méthodes `createIntParameterSlider`, `createChoiceParameterComboBox`, `createStandaloneButton` prennent `tss::ISkin& skin`.
- Après création du widget (sans lui passer ISkin au constructeur si on passe à un constructeur sans skin) : appeler `widget->setLook(xxxLookFromSkin(skin))`. Les constructeurs des widgets devront être adaptés (ex. `Button(int width, int height, const juce::String& text)` puis `setLook(buttonLookFromSkin(skin))`).

### 3.2 Panels qui créent des widgets

- Chaque panel qui instancie des widgets (ParameterPanel, ModulationBusPanel, HeaderPanel, ComputerPatchesPanel, PatchMutatorPanel, etc.) :
  - À la construction : après `addAndMakeVisible`, appeler `widget->setLook(xxxLookFromSkin(skin))` (ou passer le Look si déjà construit).
  - Dans `setSkin` : reconstruire le Look avec `xxxLookFromSkin(skin)` et appeler `setLook` sur chaque widget (remplacer les appels actuels à `setSkin` sur les enfants par `setLook`).
- Propagation du scale : dans `resized()` (ou méthode de layout), appeler `setScalingFactor(scalingFactor_)` sur chaque widget enfant. Pour cela, chaque panel doit connaître `scalingFactor_` (remonté depuis MainComponent → BodyPanel / HeaderPanel / FooterPanel → sous-panels). Options : passer le scale en paramètre aux constructeurs de panels, ou fournisseur (ex. interface/callback) remontant le scale depuis MainComponent.

### 3.3 SkinHelpers / setSkin des panels

- [Source/GUI/Skins/SkinHelpers.h](Source/GUI/Skins/SkinHelpers.h) : `propagateSkin` appelle aujourd’hui `setSkin` sur les composants. À terme, les widgets n’auront plus `setSkin` mais `setLook`. Deux options :
  - Garder `setSkin` sur les **panels** uniquement ; chaque panel dans son `setSkin` reconstruit les Looks et appelle `setLook` sur ses widgets (plus de `propagateSkin` vers les widgets, seulement vers les sous-panels).
  - Ou introduire `propagateLookFromSkin(skin, widget1, widget2, ...)` qui fait `widget->setLook(xxxLookFromSkin(skin))` selon le type (moins pratique sans surcharge par type). La première option est plus simple et respecte le document (« les panels construisent le Look et l’injectent »).

---

## 4. Ordre de mise en œuvre recommandé

1. **Infrastructure**
  - Structs Look + LookBuilders (Button, Slider, Label, HorizontalSeparator, ComboBox, PopupMenu).  
  - MainComponent + PluginEditor : `scalingFactor_`, `setScalingFactor`, suppression du transform, layout des panels en dimensions scalées.  
  - Remonter le scale jusqu’aux panels qui font le layout (Header, Body, Footer puis sous-panels) pour qu’ils puissent appeler `setScalingFactor` sur les enfants.
2. **Widgets “simples”** (sans popup)
  - Button, Slider, Label, HorizontalSeparator : Look + scaling + dessin direct (sans cache).  
  - Adapter WidgetFactory et les panels qui les créent (setLook après création, setSkin du panel appelle setLook, resized appelle setScalingFactor).
3. **ComboBox + popups**
  - ComboBox : Look + scaling + getScalingFactor + getBaseComponentWidth.  
  - PopupMenuBase, PopupMenuRenderer : Look / couleurs + scale.  
  - ScrollablePopupMenu : correctifs §4.1–4.5, dimensions et couleurs via scale + Look.  
  - MultiColumnPopupMenu : idem.  
  - PopupMenuPositioner : compatibilité avec dimensions passées par show().
4. **Propagation setSkin → setLook**
  - Remplacer partout les appels à `widget->setSkin(skin)` par la construction du Look et `widget->setLook(...)` dans le panel parent ; garder `setSkin` uniquement sur les panels pour mettre à jour leur référence skin et rappeler `setLook` sur les enfants.

---

## 5. Widgets supplémentaires Matrix-Control (hors document)

Les widgets suivants utilisent aussi ISkin et souvent un cache : Toggle, VerticalSeparator, ModuleHeader, GroupLabel, SectionHeader, NumberBox, EnvelopeDisplay, PatchNameDisplay, TrackGeneratorDisplay, ModulationBusHeader. Ils ne sont pas détaillés dans le document de passation mais font partie de la même architecture. Recommandation : les traiter dans une **phase suivante** avec les mêmes règles (Look, scalingFactor_, dessin direct, pas de cache), une fois les widgets du document stabilisés. On pourra ajouter les structs Look correspondants et les builders dans le même module `GUI/Looks/`.

---

## 6. Résumé des fichiers clés


| Rôle                       | Fichiers                                                                                                                                                                   |
| -------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Look / builders            | Nouveau : `GUI/Looks/WidgetLooks.h`, `GUI/Looks/LookBuilders.cpp` (ou équivalent)                                                                                          |
| Scale racine               | `PluginEditor.cpp`, `MainComponent.h/cpp`                                                                                                                                  |
| Layout / propagation scale | Tous les panels avec `resized()` et création de widgets                                                                                                                    |
| Widgets document           | `Button`, `Slider`, `Label`, `HorizontalSeparator`, `ComboBox`, `PopupMenuBase`, `PopupMenuRenderer`, `ScrollablePopupMenu`, `MultiColumnPopupMenu`, `PopupMenuPositioner` |
| Factory                    | `WidgetFactory.cpp` (création + setLook)                                                                                                                                   |
| Skin propagation           | `SkinHelpers.h`, `setSkin` des panels (rebuild Look + setLook sur enfants)                                                                                                 |


Ce plan respecte le document de passation et n’implique pas de copier-coller les fichiers de Test-GuiScaling : les modifications sont faites in-place dans Matrix-Control en s’appuyant sur le document et sur Test-GuiScaling comme référence d’implémentation.