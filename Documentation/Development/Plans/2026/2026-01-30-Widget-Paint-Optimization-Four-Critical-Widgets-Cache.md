---
name: Widget Paint Optimization
overview: Optimiser les performances de rendu des 4 widgets critiques (Label, Slider, ComboBox, Button) qui représentent 89% du temps CPU lors des changements de thème, en implémentant un système de cache d'images pour les parties statiques.
todos:
  - id: optimize-label
    content: Optimiser tss::Label avec cache d'image complet (33.6% du temps CPU)
    status: completed
  - id: optimize-slider
    content: Optimiser tss::Slider avec cache partiel du track (21.6% du temps CPU)
    status: completed
  - id: optimize-combobox
    content: Optimiser tss::ComboBox avec cache du background et triangle (17.2% du temps CPU)
    status: completed
  - id: optimize-button
    content: Optimiser tss::Button avec cache des 4 états (16.6% du temps CPU)
    status: completed
  - id: update-documentation
    content: Mettre à jour GUI-Optimization-Plan.md avec résultats de profiling et optimisations
    status: completed
  - id: profile-results
    content: Faire un nouveau run Instruments pour valider les gains de performance
    status: pending
isProject: false
---

# Optimisation des Widgets - Phase 4

## Contexte

Les résultats de profiling Instruments montrent que 4 widgets consomment **89% du temps CPU** lors des changements de thème :

1. **tss::Label::paint()** - 9.49 s (33.6%) - PRIORITE ABSOLUE
2. **tss::Slider::paint()** - 6.10 s (21.6%)
3. **tss::ComboBox::paint()** - 4.86 s (17.2%)
4. **juce::Button::paint()** - 4.69 s (16.6%)

**Note importante sur Button** : Le profiling montre `juce::Button::paint()` et non `tss::Button::paint()`. Analyse du code confirme que `tss::Button` hérite de `juce::Button` et override correctement `paintButton()`. Le temps CPU est attribué à la classe de base JUCE, mais c'est bien notre code custom qui s'exécute. Pas de problème architectural.

## Stratégie d'Optimisation

### Principe du Cache d'Images

Actuellement, chaque widget recalcule son rendu vectoriel complet à chaque appel de `paint()`. Avec des centaines de widgets, cela devient très coûteux.

**Solution** : Pré-rendre les parties statiques en images (juce::Image) et les réutiliser.

### Classification des Widgets

#### Label (33.6% du temps) - Cache Complet

**Analyse du code actuel** ([Label.cpp](Source/GUI/Widgets/Label.cpp)):

```cpp
void Label::paint(juce::Graphics& g)
{
    if (theme_ == nullptr || labelText_.isEmpty())
        return;

    const auto textColour = theme_->getLabelTextColour();  // Appel fonction
    const auto font = theme_->getBaseFont();               // Appel fonction
    auto textBounds = getLocalBounds().toFloat();
    textBounds.removeFromLeft(kTextLeftPadding_);
    
    g.setColour(textColour);
    g.setFont(font);
    g.drawText(labelText_, textBounds, juce::Justification::centredLeft, false);
}
```

**Problèmes identifiés** :

- Appels `theme_->getLabelTextColour()` et `theme_->getBaseFont()` à chaque paint()
- Calcul de `textBounds` à chaque paint()
- `drawText()` recalcule le layout du texte à chaque fois

**Solution** :

- Cacher la couleur et la font dans des variables membres
- Pré-rendre le texte dans une image lors de `resized()` ou `setText()`
- Dans `paint()`, simplement dessiner l'image

#### Slider (21.6%) - Cache Partiel

**Analyse du code actuel** ([Slider.cpp](Source/GUI/Widgets/Slider.cpp)):

```cpp
void Slider::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    const auto enabled = isEnabled();
    const auto trackBounds = calculateTrackBounds(bounds);        // Calcul
    const auto valueBarBounds = calculateValueBarBounds(...);     // Calcul
    
    drawTrack(g, trackBounds, enabled);                           // Statique
    drawValueBar(g, valueBarBounds, enabled);                     // DYNAMIQUE
    drawText(g, bounds, enabled);                                 // DYNAMIQUE
    drawFocusBorderIfNeeded(g, trackBounds, hasFocus_);          // DYNAMIQUE
}
```

**Parties statiques** : Track (fond du slider)
**Parties dynamiques** : Value bar, texte de valeur, bordure de focus

**Solution** :

- Cacher le track dans une image
- Recalculer uniquement value bar + texte + focus border à chaque paint()

#### ComboBox (17.2%) - Cache Partiel

**Analyse du code actuel** ([ComboBox.cpp](Source/GUI/Widgets/ComboBox.cpp)):

```cpp
void ComboBox::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    const auto enabled = isEnabled();
    const auto hasFocus = hasFocus_ || isPopupOpen_;
    const auto backgroundBounds = calculateBackgroundBounds(bounds);
    
    drawBackground(g, backgroundBounds, enabled);                 // Statique
    drawText(g, bounds, enabled);                                 // DYNAMIQUE
    drawTriangle(g, bounds, enabled);                             // Statique
    drawBorderIfNeeded(g, bounds, backgroundBounds, ...);        // DYNAMIQUE
}
```

**Parties statiques** : Background, triangle
**Parties dynamiques** : Texte sélectionné, bordure de focus

**Solution** :

- Cacher background + triangle dans une image
- Redessiner texte + bordure à chaque paint()

#### Button (16.6%) - Cache Multiple

**Analyse du code actuel** ([Button.cpp](Source/GUI/Widgets/Button.cpp)):

```cpp
void Button::paintButton(juce::Graphics& g, bool highlighted, bool down)
{
    const auto bounds = getLocalBounds().toFloat();
    const auto enabled = isEnabled();
    
    g.setColour(getBackgroundColour(enabled, highlighted, down));  // Calcul
    g.fillRect(bounds);
    
    g.setColour(getBorderColour(enabled));                         // Calcul
    g.drawRect(bounds, kBorderThickness_);
    
    if (!buttonText.isEmpty())
    {
        g.setColour(getTextColour(enabled, highlighted, down));    // Calcul
        g.setFont(theme_->getBaseFont());                          // Appel
        g.drawText(buttonText, bounds, ...);
    }
}
```

**Etats** : Normal, Hover, Down, Disabled (4 états × 2 variantes = 8 combinaisons possibles)

**Solution** :

- Pré-rendre les 4 états principaux (Normal, Hover, Down, Disabled) dans 4 images
- Dans `paint()`, choisir l'image appropriée selon l'état

## Architecture du Système de Cache

### Classe de Base : CachedComponent

Créer une classe utilitaire réutilisable pour gérer le cache d'images :

```cpp
// Source/GUI/Widgets/CachedComponent.h
class CachedComponent
{
protected:
    void invalidateCache();
    bool isCacheValid() const;
    juce::Image& getCache();
    void regenerateCache(std::function<void(juce::Graphics&)> drawFunction);
    
private:
    juce::Image cache_;
    bool cacheValid_ {false};
};
```

### Invalidation du Cache

Le cache doit être invalidé dans ces situations :

- `resized()` - Les dimensions changent
- `setTheme()` - Les couleurs changent
- `setText()` / `setValue()` - Le contenu change (pour les widgets avec texte dynamique)

## Implémentation par Widget

### 1. Label - Cache Complet

**Modifications** :

- Ajouter `juce::Image cachedImage_` et `bool cacheValid_`
- Ajouter `void regenerateCache()`
- Modifier `paint()` pour utiliser le cache
- Invalider le cache dans `resized()`, `setTheme()`, `setText()`

**Gain attendu** : 10-20x (de 33.6% à ~2-3%)

### 2. Slider - Cache Partiel

**Modifications** :

- Ajouter `juce::Image cachedTrack_` pour le track statique
- Cacher les couleurs dans des variables membres
- Modifier `paint()` pour dessiner le cache + parties dynamiques
- Invalider le cache dans `resized()`, `setTheme()`

**Gain attendu** : 5-10x (de 21.6% à ~2-4%)

### 3. ComboBox - Cache Partiel

**Modifications** :

- Ajouter `juce::Image cachedBackground_` pour background + triangle
- Modifier `paint()` pour dessiner le cache + texte + bordure
- Invalider le cache dans `resized()`, `setTheme()`

**Gain attendu** : 5-8x (de 17.2% à ~2-3%)

### 4. Button - Cache Multiple (4 états)

**Modifications** :

- Ajouter `std::array<juce::Image, 4> cachedStates_` (Normal, Hover, Down, Disabled)
- Pré-rendre les 4 états dans `regenerateCache()`
- Modifier `paintButton()` pour choisir l'image appropriée
- Invalider le cache dans `resized()`, `setTheme()`

**Gain attendu** : 8-15x (de 16.6% à ~1-2%)

## Optimisations Supplémentaires

### Mise en Cache des Appels Theme

Le profiling montre que les appels répétés à `theme_->getXxxColour()` et `theme_->getBaseFont()` sont coûteux.

**Solution** : Cacher les valeurs du thème dans des variables membres :

```cpp
class Label
{
private:
    juce::Colour cachedTextColour_;
    juce::Font cachedFont_;
    
    void updateThemeCache()
    {
        cachedTextColour_ = theme_->getLabelTextColour();
        cachedFont_ = theme_->getBaseFont();
    }
};
```

Appeler `updateThemeCache()` uniquement dans `setTheme()`.

### Éviter les Allocations dans paint()

Actuellement, plusieurs widgets créent des objets temporaires dans `paint()` :

- `juce::String` pour les valeurs
- `juce::Rectangle<float>` pour les bounds
- `juce::Path` pour les formes

**Solution** : Pré-calculer et stocker ces objets en variables membres.

## Gestion de la Mémoire

### Estimation de la Consommation Mémoire

Supposons ~300 widgets dans l'interface :

- 150 Labels × 50×20 pixels × 4 bytes (ARGB) = 600 KB
- 50 Sliders × 60×24 pixels × 4 bytes = 288 KB
- 30 ComboBoxes × 80×24 pixels × 4 bytes = 230 KB
- 70 Buttons × 60×24 pixels × 4 bytes (×4 états) = 1.6 MB

**Total** : ~2.7 MB de cache d'images

**Acceptable** : Oui, négligeable comparé aux gains de performance.

### Stratégie de Libération

Si nécessaire (très peu probable), implémenter un système de purge du cache :

- Libérer les caches des widgets non visibles
- Régénérer à la demande lors du `setVisible(true)`

## Validation et Tests

### Scénarios de Test

1. **Lancement du plugin** - Mesurer le temps de création de l'interface
2. **Changement de thème** - Mesurer le temps de repaint global
3. **Interaction sliders** - Mesurer la fluidité (FPS)
4. **Redimensionnement** - Vérifier que le cache est bien régénéré

### Métriques de Succès

**Avant optimisation** (mesuré) :

- Changement de thème : 28.26 s pour 10 changements = ~2.8 s par changement

**Après optimisation** (objectif) :

- Changement de thème : < 0.5 s par changement (gain 5-6x minimum)
- FPS lors des interactions : > 60 fps stable

### Profiling Post-Optimisation

Refaire un run Instruments après chaque widget optimisé pour valider les gains réels.

## Ordre d'Implémentation

Optimiser dans l'ordre de priorité (impact × simplicité) :

1. **Label** (33.6%) - Cache complet, implémentation simple
2. **Slider** (21.6%) - Cache partiel, complexité moyenne
3. **ComboBox** (17.2%) - Cache partiel, complexité moyenne
4. **Button** (16.6%) - Cache multiple, complexité élevée

## Risques et Mitigations


| Risque                                       | Impact | Mitigation                                                  |
| -------------------------------------------- | ------ | ----------------------------------------------------------- |
| Bugs visuels (artefacts, mauvais alignement) | Moyen  | Tester exhaustivement chaque widget après implémentation    |
| Cache non invalidé correctement              | Élevé  | Ajouter des assertions en debug pour vérifier la cohérence  |
| Consommation mémoire excessive               | Faible | Monitorer avec Instruments, implémenter purge si nécessaire |
| Régénération du cache trop lente             | Faible | Profiler `regenerateCache()`, optimiser si > 10ms           |


## Fichiers à Modifier

- [Source/GUI/Widgets/Label.h](Source/GUI/Widgets/Label.h)
- [Source/GUI/Widgets/Label.cpp](Source/GUI/Widgets/Label.cpp)
- [Source/GUI/Widgets/Slider.h](Source/GUI/Widgets/Slider.h)
- [Source/GUI/Widgets/Slider.cpp](Source/GUI/Widgets/Slider.cpp)
- [Source/GUI/Widgets/ComboBox.h](Source/GUI/Widgets/ComboBox.h)
- [Source/GUI/Widgets/ComboBox.cpp](Source/GUI/Widgets/ComboBox.cpp)
- [Source/GUI/Widgets/Button.h](Source/GUI/Widgets/Button.h)
- [Source/GUI/Widgets/Button.cpp](Source/GUI/Widgets/Button.cpp)

## Documentation

Mettre à jour le plan d'optimisation :

- [Documentation/Development/GUI/GUI-Optimization-Plan.md](Documentation/Development/GUI/GUI-Optimization-Plan.md)
  - Marquer Phase 3 comme terminée
  - Ajouter les résultats de profiling dans Phase 3
  - Détailler Phase 4 avec les optimisations implémentées

