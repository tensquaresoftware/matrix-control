# Matrix-Control / Plan d'optimisation de la GUI

- **Objectif** : Optimiser les performances de la GUI (lancement, réactivité, redimensionnement)
- **Date de création** : 26/01/2026
- **Date de mise à jour** : 30/01/2026
- **Statut** : Phases 1-4 terminées ✅ | Profiling post-optimisation à effectuer

---

## Problèmes identifiés

- Lancement lent du plugin
- Lag lors des interactions (sliders, boutons)
- Ralentissements lors du déplacement de la fenêtre
- ~300 widgets vectoriels sans cache
- Cascade de `repaint()` non optimisée dans `setTheme()`

---

## Vue d'ensemble du plan en 4 phases

```
Phase 1 : Nettoyage du design ✅ TERMINÉ (30/01/2026)
  └─> Simplifier le code de rendu des widgets
  └─> Supprimer les éléments graphiques inutiles
  └─> Refactoriser avec Clean Code et DRY
  └─> Résultat : Tous les widgets optimisés, -1500 lignes de code

Phase 2 : Quick Win - Optimisation setTheme() ✅ TERMINÉ (30/01/2026)
  └─> Corriger la cascade de repaint()
  └─> Impact immédiat au lancement et changement de thème
  └─> Résultat : Un seul repaint() à la racine, architecture propre

Phase 3 : Mesure et Profiling ✅ TERMINÉ (30/01/2026)
  └─> Utiliser Instruments / Time Profiler
  └─> Identifier les hotspots réels
  └─> Résultat : 4 widgets = 89% du temps CPU
  └─> Label (33.6%), Slider (21.6%), ComboBox (17.2%), Button (16.6%)

Phase 4 : Optimisation ciblée ✅ TERMINÉ (30/01/2026)
  └─> Implémenter le cache d'images sur les 4 widgets critiques
  └─> Mise en cache des couleurs et fonts du thème
  └─> Gain attendu : ~10x sur les changements de thème
  └─> Consommation mémoire : ~2.7 MB (acceptable)
```

---

## Phase 1 : Nettoyage du Design des Widgets

### Objectif
Simplifier le code de rendu en supprimant les éléments graphiques inutiles (fonds, bordures, etc.) et clarifier la responsabilité de chaque widget.

### Approche : Revue Systématique

Pour **chaque widget**, se poser ces questions :

#### Questions de Design
1. **Quel est son rôle visuel final ?**
   - Texte seul ?
   - Fond + texte ?
   - Forme complexe ?
   - Éléments interactifs ?

2. **A-t-il besoin d'un fond opaque ?**
   - OUI → `setOpaque(true)` + `fillRect()` dans `paint()`
   - NON → `setOpaque(false)` + pas de `drawBackground()`

3. **Quelles sont les parties statiques vs dynamiques ?**
   - Statique : ne change jamais (fond, bordure, labels)
   - Dynamique : change souvent (valeur d'un slider, texte d'un input)
   - → Utile pour la Phase 4 (cache d'images)

4. **Quelles opérations de dessin sont coûteuses ?**
   - `fillPath()` avec Path complexe
   - `drawText()` avec police custom
   - Anti-aliasing intensif
   - Gradients, ombres portées

#### Actions par Widget

- [x] **Label** - Texte simple, transparent ✅ **TERMINÉ** (26/01/2026)
- [x] **GroupLabel** - Texte + lignes gauche/droite ✅ **TERMINÉ** (26/01/2026)
- [x] **Button** - Fond + bordure + texte + états (hover/down) ✅ **TERMINÉ** (26/01/2026)
- [x] **NumberBox** - Champ numérique éditable + dot ✅ **TERMINÉ** (26/01/2026)
- [x] **SectionHeader** - Texte + lignes décoratives ✅ **TERMINÉ** (26/01/2026)
- [x] **ModuleHeader** - Texte en gras + ligne horizontale ✅ **TERMINÉ** (26/01/2026)
- [x] **HorizontalSeparator** - Ligne horizontale ✅ **TERMINÉ** (26/01/2026)
- [x] **VerticalSeparator** - Ligne verticale + padding ✅ **TERMINÉ** (26/01/2026)
- [x] **Slider** - Track + texte + bordure focus ✅ **TERMINÉ** (26/01/2026)
- [x] **ComboBox** - Fond + texte + flèche déroulante ✅ **TERMINÉ** (30/01/2026)
- [x] **PopupMenu** (pour ComboBox) - Menu déroulant stylisé ✅ **REFACTORISÉ** (30/01/2026)
  - Séparé en MultiColumnPopupMenu, ScrollablePopupMenu, PopupMenuBase
  - PopupMenuRenderer pour le rendu, PopupMenuPositioner pour le positionnement
- [x] **EnvelopeDisplay** - Courbe d'enveloppe (points + segments) ✅ **VÉRIFIÉ** (30/01/2026)
- [x] **TrackGeneratorDisplay** - Visualisation de séquence ✅ **VÉRIFIÉ** (30/01/2026)
- [x] **PatchNameDisplay** - Affichage du nom de patch ✅ **VÉRIFIÉ** (30/01/2026)
- [x] **ModulationBusHeader** - En-tête de bus de modulation ✅ **TERMINÉ** (30/01/2026)

### Refactorisation des Panneaux (30/01/2026)

#### Problème Initial
- 12 modules (Dco1, Dco2, VcfVca, FmTrack, RampPortamento, Env1, Env2, Env3, Lfo1, Lfo2, Midi, Vibrato, Misc)
- Chaque module : 120-150 lignes de code dupliqué
- Total : ~1800 lignes de code répétitif
- Maintenance difficile, violations du principe DRY

#### Solution : BaseModulePanel
Création d'une classe de base abstraite `BaseModulePanel` avec configuration déclarative :

```cpp
struct ModulePanelConfig
{
    juce::String moduleId;
    ModulePanelButtonSet buttonSet;
    ModulePanelModuleType moduleType;
    juce::String initWidgetId;
    juce::String copyWidgetId;
    juce::String pasteWidgetId;
    std::vector<ParameterConfig> parameters;
};
```

Chaque module définit maintenant sa configuration en ~40 lignes :

```cpp
ModulePanelConfig Dco1Panel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginDescriptors::ModuleIds::kDco1;
    config.buttonSet = ModulePanelButtonSet::InitCopyPaste;
    config.moduleType = ModulePanelModuleType::PatchEdit;
    config.parameters = {
        {PluginDescriptors::ParameterIds::kDco1Frequency, ModulePanelParameterType::Slider},
        // ... autres paramètres
    };
    return config;
}
```

#### Résultats
- ✅ **12 modules refactorisés** (100% des modules)
- ✅ **-1500 lignes de code** (-73% de réduction)
- ✅ **Architecture déclarative** (Clean Code)
- ✅ **Single Responsibility** respecté
- ✅ **DRY principe** appliqué rigoureusement
- ✅ **Maintenabilité** drastiquement améliorée

#### Corrections de Transparence
Pendant la refactorisation, correction du problème `setOpaque(true)` sans `paint()` dans :
- BaseModulePanel
- ModuleHeaderPanel
- ParameterPanel
- ModulationBusPanel
- ModulationBusHeader

---

## Phase 2 : Quick Win - Optimisation `setTheme()` ✅ TERMINÉ (30/01/2026)

### Problème Initial

```cpp
void MainComponent::setTheme(Theme& theme)
{
    theme_ = &theme;
    headerPanel.setTheme(theme);  // -> appelle repaint() dans headerPanel
    bodyPanel.setTheme(theme);    // -> appelle repaint() dans bodyPanel
    footerPanel.setTheme(theme);  // -> appelle repaint() dans footerPanel
    repaint();                    // -> repaint() redondant à la racine !
}
```

**Résultat** : Chaque niveau de la hiérarchie appelle `repaint()` → cascade de repaints redondants.

### Solution Implémentée

**Approche "bottom-up" : repaint unique à la racine**

```cpp
// Dans TOUS les widgets/panneaux intermédiaires
void MyWidget::setTheme(Theme& theme)
{
    theme_ = &theme;
    // PAS de repaint() ici !
}

// SEULEMENT à la racine (PluginEditor)
void PluginEditor::updateTheme()
{
    if (auto* widget = mainComponent.get())
        widget->setTheme(*theme);
    repaint();  // UN SEUL repaint à la racine absolue
}
```

### Fichiers Modifiés

Suppression des `repaint()` dans `setTheme()` de tous les fichiers sauf la racine :

- [x] `Source/GUI/Panels/Reusable/BaseModulePanel.cpp` ✅
- [x] `Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/ComputerPatchesPanel.cpp` ✅
- [x] `Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/InternalPatchesPanel.cpp` ✅
- [x] `Source/GUI/Panels/MainComponent/BodyPanel/PatchManagerPanel/Modules/BankUtilityPanel.cpp` ✅
- [x] `Source/GUI/MainComponent.cpp` ✅
- [x] Garder `repaint()` UNIQUEMENT dans `PluginEditor::updateTheme()` ✅

### Widgets Déjà Conformes (Phase 1)

Ces widgets n'avaient déjà pas de `repaint()` dans leur `setTheme()` :

- [x] `Source/GUI/Widgets/Label.cpp` ✅
- [x] `Source/GUI/Widgets/GroupLabel.cpp` ✅
- [x] `Source/GUI/Widgets/Button.cpp` ✅
- [x] `Source/GUI/Widgets/NumberBox.cpp` ✅
- [x] `Source/GUI/Widgets/Slider.cpp` ✅
- [x] `Source/GUI/Widgets/ComboBox.cpp` ✅
- [x] `Source/GUI/Widgets/SectionHeader.cpp` ✅
- [x] `Source/GUI/Widgets/ModuleHeader.cpp` ✅
- [x] `Source/GUI/Widgets/HorizontalSeparator.cpp` ✅
- [x] `Source/GUI/Widgets/VerticalSeparator.cpp` ✅
- [x] `Source/GUI/Widgets/ModulationBusHeader.cpp` ✅
- [x] `Source/GUI/Widgets/EnvelopeDisplay.cpp` ✅
- [x] `Source/GUI/Widgets/TrackGeneratorDisplay.cpp` ✅
- [x] `Source/GUI/Widgets/PatchNameDisplay.cpp` ✅

### Résultats

- ✅ **Cascade de repaint() éliminée** : Un seul `repaint()` à la racine (`PluginEditor::updateTheme()`)
- ✅ **Compilation réussie** : Aucune erreur de compilation
- ✅ **Architecture propre** : Propagation bottom-up du thème sans side-effects
- ✅ **Gain attendu** : 2x à 5x plus rapide au lancement et changement de thème instantané

---

## Phase 3 : Mesure et Profiling avec Instruments ✅ TERMINÉ (30/01/2026)

### Objectif
Identifier les **vraies** sources de lenteur avant d'implémenter des optimisations lourdes.

### Outils
- **Instruments** (macOS) - Time Profiler
- **Alternative** : `juce::PerformanceCounter` dans le code

### Procédure Suivie

#### 1. Compilation en mode RelWithDebInfo ✅

```bash
cd /Volumes/Guillaume/Dev/Projects/MAO/Plugins/Matrix-Control/Builds/macOS
ninja
```

**RelWithDebInfo** = optimisations ON + symboles de debug ON

#### 2. Génération du fichier dSYM ✅

Le fichier dSYM n'était pas généré automatiquement par CMake. Solution :

```bash
cd Builds/macOS/Matrix-Control_artefacts/RelWithDebInfo/Standalone
dsymutil Matrix-Control.app/Contents/MacOS/Matrix-Control -o Matrix-Control.app.dSYM
```

Vérification des symboles :
```bash
dwarfdump --lookup tss Matrix-Control.app.dSYM | head -20
```

#### 3. Profiling avec Instruments ✅

- **Version Instruments** : 26.2
- **Application profilée** : Matrix-Control Standalone (RelWithDebInfo)
- **Scénario testé** : 10 changements de thème (BLACK ↔ CREAM)
- **Durée totale** : 28.26 secondes

**Configuration Instruments** :
- Time Profiler activé
- "Record Kernel Callstacks" désactivé (éviter le bruit système)
- "Hide System Libraries" désactivé (pour voir tous les symboles)
- "Invert Call Tree" désactivé (pour voir la hiérarchie correcte)
- Filtre Input Filter : `tss::` pour isoler notre code

#### 4. Résultats du Profiling ✅

**Temps total mesuré** : 28.26 s pour 10 changements de thème = **~2.8 s par changement**

**Top 4 Hotspots identifiés (89% du temps CPU)** :

1. **`tss::Label::paint()`** - 9.49 s (33.6%) ← **PRIORITÉ ABSOLUE**
2. **`tss::Slider::paint()`** - 6.10 s (21.6%)
3. **`tss::ComboBox::paint()`** - 4.86 s (17.2%)
4. **`juce::Button::paint()`** - 4.69 s (16.6%)

**Note sur Button** : Instruments affiche `juce::Button::paint()` car `tss::Button` hérite de `juce::Button` et override `paintButton()`. Le temps CPU mesuré est bien notre code custom.

**Autres widgets** (< 5% chacun, total ~11%) :
- `tss::ModuleHeader::paint()` - 1.18 s (4.2%)
- `tss::SectionHeader::paint()` - 892 ms (3.2%)
- `tss::GroupLabel::paint()` - 589 ms (2.1%)
- `tss::NumberBox::paint()` - 294 ms (1.0%)
- Autres widgets - < 1 s total

### Analyse et Conclusions

#### Problèmes Identifiés

**1. Appels répétés aux getters du Theme**
- Chaque `paint()` appelle `theme_->getXxxColour()` et `theme_->getBaseFont()`
- Ces appels sont coûteux (lookup + switch selon variant Black/Cream)
- Multiplié par des centaines de widgets à chaque frame

**2. Calculs redondants dans paint()**
- `textBounds` recalculé à chaque frame
- `trackBounds`, `valueBarBounds` recalculés constamment
- `Path` pour les triangles recréés à chaque paint()

**3. Rendu vectoriel sans cache**
- `drawText()` recalcule le layout du texte à chaque fois
- `fillRect()`, `drawRect()` répétés inutilement
- Aucun réutilisation entre frames identiques

#### Priorisation des Optimisations

**Impact attendu global** : Optimiser les 4 widgets critiques peut réduire de **~90%** le temps de rendu lors des changements de thème.

**Ordre d'implémentation choisi** :
1. **Label** (33.6%) - Cache complet, implémentation simple
2. **Slider** (21.6%) - Cache partiel, complexité moyenne
3. **ComboBox** (17.2%) - Cache partiel, complexité moyenne
4. **Button** (16.6%) - Cache multiple (4 états), complexité élevée

---

## Phase 4 : Optimisation Ciblée - Cache d'Images ✅ TERMINÉ (30/01/2026)

### Objectif
Pré-rendre les parties statiques des widgets en images, puis dessiner ces images dans `paint()` au lieu de recalculer le rendu vectoriel à chaque frame.

### Principe du Cache d'Images

#### Avant (rendu vectoriel direct)

```cpp
void Slider::paint(juce::Graphics& g)
{
    // Recalcule tout à chaque paint() !
    g.fillAll(bgColour);              // 1. Fond
    g.drawRect(bounds, 1.0f);         // 2. Bordure
    g.fillRect(trackBounds);          // 3. Piste
    g.fillEllipse(thumbBounds);       // 4. Curseur (dynamique)
    g.drawText(valueText, ...);       // 5. Texte de valeur (dynamique)
}
```

**Coût** : ~100-500 µs par paint() × 50 sliders = 5-25 ms par frame

#### Après (cache d'image)

```cpp
class Slider : public juce::Component
{
public:
    void resized() override
    {
        regenerateCache();  // Pré-rendre le fond + bordure + piste
    }

    void paint(juce::Graphics& g) override
    {
        // 1. Dessiner le cache (très rapide !)
        g.drawImageAt(cachedBackground_, 0, 0);

        // 2. Dessiner uniquement les parties dynamiques
        g.fillEllipse(thumbBounds);       // Curseur
        g.drawText(valueText, ...);       // Valeur
    }

private:
    void regenerateCache()
    {
        cachedBackground_ = juce::Image(
            juce::Image::ARGB,
            getWidth(), getHeight(),
            true  // clear to transparent
        );

        juce::Graphics g(cachedBackground_);
        g.fillAll(bgColour);              // Une seule fois !
        g.drawRect(bounds, 1.0f);         // Une seule fois !
        g.fillRect(trackBounds);          // Une seule fois !
    }

    juce::Image cachedBackground_;
};
```

**Coût** : ~10-50 µs par paint() × 50 sliders = 0.5-2.5 ms par frame  
**Gain** : **10x à 20x** !

### Stratégie d'Implémentation

#### Widgets Statiques (cache complet)

Ces widgets ne changent jamais après création :

- **Label** - Texte fixe
- **GroupLabel** - Texte + cadre fixe
- **SectionHeader** - Titre de section
- **Separators** - Lignes fixes

**Stratégie** : Cache tout, dessine l'image dans `paint()`.

#### Widgets Semi-Dynamiques (cache partiel)

Ces widgets ont des parties fixes + des parties qui bougent :

- **Button** - Fond + bordure (fixe) + état hover/down (dynamique)
- **Slider** - Piste (fixe) + curseur + valeur (dynamique)
- **ComboBox** - Fond + bordure (fixe) + texte sélectionné (dynamique)

**Stratégie** : Cache le fond, dessine les parties dynamiques par-dessus.

#### Widgets Dynamiques (cache conditionnel)

Ces widgets changent fréquemment :

- **EnvelopeDisplay** - Courbe modifiée par l'utilisateur
- **TrackGeneratorDisplay** - Séquence modifiée
- **PatchNameDisplay** - Nom de patch change

**Stratégie** : 
- Cache quand même le fond + grille
- Redessine les données vectorielles (courbes, notes)
- Invalide le cache uniquement si les données changent

### Gestion du Zoom (Transformation Affine)

#### Problème
Si on applique un zoom avec `setTransform()`, les images mises à l'échelle deviennent floues.

#### Solution : Régénération du Cache au Changement de Zoom

```cpp
class MyWidget : public juce::Component
{
public:
    void setZoomFactor(float zoom)
    {
        if (juce::approximatelyEqual(zoomFactor_, zoom))
            return;  // Pas de changement

        zoomFactor_ = zoom;
        regenerateCache();  // Redessine en vectoriel au bon zoom
    }

private:
    float zoomFactor_ {1.0f};
};
```

**Note** : Le changement de zoom est rare (fait par l'utilisateur 1 fois au lancement), donc la régénération n'est pas un problème de performance.

### Implémentation Réalisée ✅

#### 1. Label - Cache Complet (33.6% → ~2-3% attendu)

**Modifications** :
- Ajout de `juce::Image cachedImage_` et `bool cacheValid_`
- Ajout de `void regenerateCache()` pour pré-rendre le texte
- Ajout de `void invalidateCache()` pour marquer le cache obsolète
- Ajout de `void resized()` override pour invalider le cache
- Modification de `paint()` : dessine simplement `cachedImage_` au lieu de recalculer

**Invalidation du cache** :
- `resized()` - Dimensions changent
- `setTheme()` - Couleurs changent
- `setText()` - Contenu change

**Fichiers modifiés** :
- `Source/GUI/Widgets/Label.h`
- `Source/GUI/Widgets/Label.cpp`

#### 2. Slider - Cache Partiel + Mise en Cache des Couleurs (21.6% → ~2-4% attendu)

**Modifications** :
- Ajout de `juce::Image cachedTrack_` pour le track statique
- Ajout de `bool cacheValid_`
- Ajout de variables membres pour cacher toutes les couleurs du thème :
  - `cachedTrackColourEnabled_`, `cachedTrackColourDisabled_`
  - `cachedValueBarColourEnabled_`, `cachedValueBarColourDisabled_`
  - `cachedTextColourEnabled_`, `cachedTextColourDisabled_`
  - `cachedFocusBorderColour_`
  - `cachedFont_`
- Ajout de `void regenerateTrackCache()` pour pré-rendre le track
- Ajout de `void updateThemeCache()` appelée dans constructeur et `setTheme()`
- Ajout de `void invalidateCache()`
- Ajout de `void resized()` override
- Modification de `paint()` : dessine le track en cache + parties dynamiques
- Modification de `drawTrack()`, `drawValueBar()`, `drawText()`, `drawFocusBorderIfNeeded()` pour utiliser les couleurs en cache

**Parties cachées** : Track (statique)
**Parties dynamiques** : Value bar, texte de valeur, bordure de focus

**Fichiers modifiés** :
- `Source/GUI/Widgets/Slider.h`
- `Source/GUI/Widgets/Slider.cpp`

#### 3. ComboBox - Cache Partiel + Mise en Cache des Couleurs (17.2% → ~2-3% attendu)

**Modifications** :
- Ajout de `juce::Image cachedBackground_` pour background + triangle
- Ajout de `bool cacheValid_`
- Ajout de variables membres pour cacher toutes les couleurs du thème :
  - `cachedBackgroundColourEnabled_`, `cachedBackgroundColourDisabled_`
  - `cachedBorderColour_`, `cachedFocusBorderColour_`
  - `cachedTriangleColourEnabled_`, `cachedTriangleColourDisabled_`
  - `cachedTextColourEnabled_`, `cachedTextColourDisabled_`
  - `cachedFont_`
- Ajout de `void regenerateBackgroundCache()` pour pré-rendre background + triangle + bordure (si ButtonLike)
- Ajout de `void updateThemeCache()` appelée dans constructeur et `setTheme()`
- Ajout de `void invalidateCache()`
- Ajout de `void resized()` override
- Modification de `paint()` : dessine le background en cache + texte + bordure dynamique
- Modification de `drawBackground()`, `drawBorderIfNeeded()`, `drawTriangle()`, `getTextColourForCurrentStyle()`, `drawTextInBounds()` pour utiliser les couleurs en cache

**Parties cachées** : Background, triangle, bordure ButtonLike
**Parties dynamiques** : Texte sélectionné, bordure de focus (mode Standard)

**Fichiers modifiés** :
- `Source/GUI/Widgets/ComboBox.h`
- `Source/GUI/Widgets/ComboBox.cpp`

#### 4. Button - Cache Multiple (4 États) (16.6% → ~1-2% attendu)

**Modifications** :
- Ajout de `enum class ButtonState { Normal, Hover, Down, Disabled }`
- Ajout de `std::array<juce::Image, 4> cachedStates_` pour les 4 états pré-rendus
- Ajout de `bool cacheValid_`
- Ajout de `void regenerateStateCache()` pour pré-rendre les 4 états (fond + bordure + texte)
- Ajout de `void invalidateCache()`
- Ajout de `ButtonState getCurrentState()` pour déterminer l'état actuel
- Ajout de `void resized()` override
- Modification de `paintButton()` : sélectionne et dessine l'image de l'état approprié

**États pré-rendus** :
- Normal (enabled, not highlighted, not down)
- Hover (enabled, highlighted, not down)
- Down (enabled, not highlighted, down)
- Disabled (not enabled)

**Fichiers modifiés** :
- `Source/GUI/Widgets/Button.h` (ajout `#include <array>`)
- `Source/GUI/Widgets/Button.cpp`

### Résultats de l'Implémentation

#### Compilation

✅ **Succès** : Le code compile sans erreurs avec le build RelWithDebInfo.

**Warnings corrigés** :
- Conversion signedness dans Button.cpp (int → size_t)
- Paramètre unused dans ComboBox.cpp

#### Estimation des Gains de Performance

**Avant optimisation** (mesuré) :
- Changement de thème : 28.26 s pour 10 changements = ~2.8 s par changement

**Après optimisation** (estimation) :
- Label : 33.6% → ~2% (gain ~15x)
- Slider : 21.6% → ~3% (gain ~7x)
- ComboBox : 17.2% → ~2% (gain ~8x)
- Button : 16.6% → ~1.5% (gain ~10x)

**Temps total estimé après optimisation** : ~10% du temps initial
- **2.8 s → ~0.28 s par changement de thème (gain ~10x)**

#### Consommation Mémoire

**Estimation pour ~300 widgets** :
- 150 Labels × 50×20 px × 4 bytes = 600 KB
- 50 Sliders × 60×24 px × 4 bytes = 288 KB
- 30 ComboBoxes × 80×24 px × 4 bytes = 230 KB
- 70 Buttons × 60×24 px × 4 bytes × 4 états = 1.6 MB

**Total** : ~2.7 MB de cache d'images

✅ **Acceptable** : Négligeable comparé aux gains de performance (~0.1% de la RAM typique)

### Checklist d'Implémentation

- [x] Implémenter le cache sur Label (widget pilote) ✅
- [x] Implémenter le cache sur Slider ✅
- [x] Implémenter le cache sur ComboBox ✅
- [x] Implémenter le cache sur Button ✅
- [x] Vérifier la compilation sans erreurs ✅
- [ ] Profiler avec Instruments pour valider les gains réels
- [ ] Implémenter la régénération du cache au changement de zoom (si nécessaire)
- [ ] Tester avec différentes résolutions d'écran

---

## Optimisations Complémentaires

### 1. Lazy Loading des Composants

**Principe** : Ne créer que les widgets visibles au démarrage.

```cpp
void BodyPanel::showModule(int moduleIndex)
{
    if (modules_[moduleIndex] == nullptr)
    {
        // Créer le module à la demande
        modules_[moduleIndex] = std::make_unique<DcoPanel>(...);
        addAndMakeVisible(*modules_[moduleIndex]);
    }

    modules_[moduleIndex]->setVisible(true);
}
```

**Gain** : Lancement 3-10x plus rapide.

### 2. `setOpaque(true)` pour les Composants Opaques

JUCE optimise le rendu si un composant est opaque (pas besoin de dessiner les composants en dessous).

```cpp
MyPanel::MyPanel()
{
    setOpaque(true);  // Ce composant a un fond opaque, pas de transparence
}
```

**Règle** :
- `setOpaque(true)` → si le widget remplit tout son bounds avec une couleur opaque
- `setOpaque(false)` → si le widget est transparent ou partiellement transparent

### 3. `setBufferedToImage(false)` par Défaut

JUCE peut bufferiser automatiquement un composant en image. **Désactive cette option** car on gère notre propre cache.

```cpp
MyWidget::MyWidget()
{
    setBufferedToImage(false);  // On gère notre propre cache
}
```

---

## Métriques de Succès

### Avant Optimisation (Mesuré avec Instruments - 30/01/2026)

Profiling effectué avec Instruments 26.2, build RelWithDebInfo, 10 changements de thème :

- Temps total : 28.26 s pour 10 changements
- **Temps de changement de thème : ~2.8 s par changement**
- Top 4 widgets : 89% du temps CPU
  - Label : 9.49 s (33.6%)
  - Slider : 6.10 s (21.6%)
  - ComboBox : 4.86 s (17.2%)
  - Button : 4.69 s (16.6%)

### Après Optimisation (Objectifs)

**Cibles de performance** :
- Temps de changement de thème : **< 0.5 s par changement** (gain ~5-6x minimum)
- FPS lors des interactions : **> 60 fps** stable
- Temps de lancement : **< 500 ms** (objectif idéal : < 200 ms)
- Temps de redimensionnement : **< 100 ms**

**Prochaine étape** :
- Effectuer un nouveau profiling avec Instruments pour valider les gains réels
- Comparer avec les métriques "Avant Optimisation"
- Ajuster si nécessaire les implémentations

---

## Notes et Décisions

### Décisions Architecturales

- **Hiérarchie de panneaux** : On garde la structure actuelle (6 niveaux), le gain de tout refactoriser n'en vaut pas la peine.
- **Widgets custom** : On garde les `Component` JUCE, on optimise leur rendu.
- **Zoom** : Régénération du cache à chaque changement de zoom (solution pro, rendu net).

### Risques et Mitigations

| Risque | Impact | Mitigation |
|--------|--------|-----------|
| Cache consomme trop de mémoire | Moyen | Partager les caches entre widgets identiques |
| Régénération du cache au zoom est lente | Faible | Le zoom est rare, 1-2s de régénération est acceptable |
| Bugs visuels lors du cache | Moyen | Tester exhaustivement chaque widget après implémentation |

---

## Ressources et Références

### Documentation JUCE
- [Component::setOpaque()](https://docs.juce.com/master/classComponent.html#a8c6b6e4c4c9e9d8e5e5e5e5e5e5e5e5e)
- [Component::setBufferedToImage()](https://docs.juce.com/master/classComponent.html#a7d5c5e4d4d9e9e8e5e5e5e5e5e5e5e5e)
- [Graphics class](https://docs.juce.com/master/classGraphics.html)
- [Image class](https://docs.juce.com/master/classImage.html)

### Plugins de Référence (à analyser)
- **Vital** - Rendu ultra-performant, zoom jusqu'à 200%
- **Serum** - Interface vectorielle rapide
- **Diva** - GUI simple mais fluide

### Fichiers Clés du Projet
- `Source/GUI/Themes/Theme.h` - Système de thème
- `Source/GUI/MainComponent.cpp` - Racine de la hiérarchie
- `Source/GUI/Widgets/*.cpp` - Tous les widgets custom

### Fichiers Modifiés (Phase 4)
- `Source/GUI/Widgets/Label.h` / `Label.cpp` - Cache complet
- `Source/GUI/Widgets/Slider.h` / `Slider.cpp` - Cache partiel + couleurs
- `Source/GUI/Widgets/ComboBox.h` / `ComboBox.cpp` - Cache partiel + couleurs
- `Source/GUI/Widgets/Button.h` / `Button.cpp` - Cache multiple (4 états)

---

## Profiling Post-Optimisation

### À Effectuer

Pour valider les gains de performance réels :

1. **Rebuild en RelWithDebInfo** avec les optimisations
2. **Lancer Instruments** avec Time Profiler
3. **Reproduire le même scénario** : 10 changements de thème BLACK ↔ CREAM
4. **Comparer les métriques** :
   - Temps total pour 10 changements
   - Pourcentage CPU de `tss::Label::paint()`, `tss::Slider::paint()`, etc.
   - Vérifier que les fonctions `regenerateCache()` ne prennent pas trop de temps
5. **Documenter les résultats** dans ce fichier

### Résultats Attendus

- **Label** : 33.6% → ~2% (gain ~15x)
- **Slider** : 21.6% → ~3% (gain ~7x)
- **ComboBox** : 17.2% → ~2% (gain ~8x)
- **Button** : 16.6% → ~1.5% (gain ~10x)

**Temps total** : 28.26 s → **~2-3 s** (gain ~10x)

---

