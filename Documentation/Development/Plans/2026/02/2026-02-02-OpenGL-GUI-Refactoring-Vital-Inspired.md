---
name: OpenGL GUI Refactoring
overview: Refactoriser l'architecture GUI avec OpenGL pour améliorer drastiquement les performances de rendu, en s'inspirant du modèle éprouvé de Vital Audio (synthétiseur open-source de Matt Tytel).
todos:
  - id: git-rollback
    content: "Rollback Git: pull origin/main + reset vers v0.0.40-alpha + cherry-pick commits utiles"
    status: completed
  - id: phase1-infrastructure
    content: "Phase 1: Créer infrastructure OpenGL complète (OpenGlComponent, OpenGlImageComponent, Skin, OpenGlSection) avec support zoom/resize dès la conception"
    status: completed
  - id: phase1-test-label
    content: "Phase 1: Migrer Label vers OpenGL et valider (texte net, lazy rendering, performance, zoom fluide)"
    status: completed
  - id: phase2-text-widgets
    content: "Phase 2: Migrer GroupLabel, SectionHeader, ModuleHeader vers OpenGL"
    status: completed
  - id: phase3-button
    content: "Phase 3: Migrer Button vers OpenGL (cache multi-états, animations hover)"
    status: completed
  - id: phase4-slider-numberbox
    content: "Phase 4: Migrer Slider + NumberBox vers OpenGL (rendu hybride, optimisation majeure NumberBox)"
    status: completed
  - id: phase5-remaining
    content: "Phase 5: Migrer ComboBox, Displays, Séparateurs, ModulationBusHeader vers OpenGL"
    status: completed
  - id: phase6-cleanup
    content: "Phase 6: Cleanup (supprimer Theme), profiling Instruments, documentation finale"
    status: in_progress
  - id: phase7-zoom
    content: "Phase 7: Implémenter système de zoom GUI (menu déroulant + resizer coin) avec rendu vectoriel net à toutes échelles"
    status: pending
isProject: false
---

# Refactorisation OpenGL de la GUI Matrix-Control

## Contexte

L'implémentation actuelle avec cache ARGB simple présente plusieurs problèmes critiques :

- **Labels flous** : pas de support HiDPI/Retina proper
- **Boutons déformés** : cache d'images sans pixel scaling
- **Performances médiocres** : régénération systématique des caches lors des changements de thème
- **Lourdeur ressentie** : pas de lazy rendering, tout est redessiné
- **Pas de zoom/resize fluide** : architecture non prévue pour le redimensionnement dynamique

L'analyse du code source de **Vital Audio** révèle une architecture OpenGL sophistiquée qui résout tous ces problèmes.

## Objectif additionnel : GUI redimensionnable

**Exigence critique** : L'architecture OpenGL doit permettre un **zoom/resize fluide et net** de l'interface :

- Menu déroulant avec facteurs de zoom pré-établis (50%, 75%, 100%, 125%, 150%, 200%)
- Resizer dans un coin de la fenêtre pour zoom libre
- **Rendu vectoriel pur** : aucun flou à aucune échelle
- Performance 60 FPS même pendant le resize
- Texte toujours net (re-rendering à la résolution cible, pas de scaling bitmap)

**Comment Vital le fait :**

- Les textures OpenGL sont régénérées à la bonne résolution lors du resize
- Le pixel scaling est recalculé dynamiquement
- Les widgets utilisent des coordonnées relatives (pas de pixels hardcodés)
- Le rendu vectoriel JUCE (Graphics) est scalé AVANT la rasterisation vers texture

## Pré-requis : Rollback Git

Avant toute chose, restaurer le code au tag `v0.0.40-alpha` (avant la refacto cache problématique) :

```bash
# 1. Récupérer les modifications distantes (README + MIT license)
git pull origin main

# 2. Reset vers le tag v0.0.40-alpha
git reset --hard v0.0.40-alpha

# 3. Cherry-pick les commits utiles de origin/main
git cherry-pick 1b1dc97  # MIT license
git cherry-pick b166b44  # README update

# 4. Créer un nouveau tag pour marquer le début de la refacto OpenGL
git tag -a v0.0.41-alpha -m "Pre-OpenGL refactoring baseline"
```

## Architecture Cible (inspirée de Vital)

### Hiérarchie des composants OpenGL

```
Component (JUCE)
  └── OpenGlComponent (nouvelle classe de base)
       ├── OpenGlImageComponent (cache texture OpenGL)
       │    └── OpenGlAutoImageComponent<T> (template wrapper)
       │         ├── OpenGlLabel
       │         ├── OpenGlGroupLabel
       │         └── ... (autres widgets)
       └── OpenGlContainer (pour panels/sections)
```

### Système de Skin (remplacement du Theme actuel)

Au lieu d'appeler des méthodes `theme->getXxxColour()` répétitivement, utiliser un système de valeurs centralisé :

```cpp
enum class SkinValueId {
    kLabelFontSize,
    kSliderWidth,
    kButtonHeight,
    // ... ~50 valeurs
};

enum class SkinColourId {
    kLabelText,
    kSliderTrack,
    kButtonBackground,
    // ... ~100 couleurs
};
```

Chaque widget cache ses valeurs de skin localement dans `updateSkinCache()`, appelée uniquement lors des changements de thème.

## Phase 1 : Infrastructure OpenGL (Grosse étape unique)

### 1.1 Classes de base OpenGL

**Fichiers à créer :**

- `[Source/GUI/OpenGL/OpenGlComponent.h](Source/GUI/OpenGL/OpenGlComponent.h)`
- `[Source/GUI/OpenGL/OpenGlComponent.cpp](Source/GUI/OpenGL/OpenGlComponent.cpp)`
- `[Source/GUI/OpenGL/OpenGlImageComponent.h](Source/GUI/OpenGL/OpenGlImageComponent.h)`
- `[Source/GUI/OpenGL/OpenGlImageComponent.cpp](Source/GUI/OpenGL/OpenGlImageComponent.cpp)`
- `[Source/GUI/OpenGL/OpenGlAutoImageComponent.h](Source/GUI/OpenGL/OpenGlAutoImageComponent.h)` (template, header-only)

**Responsabilités :**

`OpenGlComponent` :

- Classe de base abstraite pour tous les composants OpenGL
- Gestion du rendu OpenGL (`render(OpenGLContext&)`)
- Flags `active_`, `visible_`, `alwaysOnTop_`
- Scissor/viewport management

`OpenGlImageComponent` :

- Hérite de `OpenGlComponent`
- Cache le rendu dans une `juce::Image` puis upload vers texture OpenGL
- Méthode `redrawImage(bool force)` pour lazy rendering
- Flag `staticImage_` pour composants qui ne changent jamais
- **Support HiDPI avec pixel scaling automatique** (crucial pour netteté)
- **Support resize/zoom** : régénération texture à la nouvelle résolution
- Thread-safety avec mutex pour l'accès à la texture
- Méthode `setScaleFactor(float scale)` pour zoom dynamique

`OpenGlAutoImageComponent<T>` :

- Template wrapper autour de n'importe quel `Component`
- Redessine automatiquement sur interactions (`mouseDown`, `mouseUp`, `mouseEnter`, `mouseExit`, etc.)
- Usage : `using OpenGlLabel = OpenGlAutoImageComponent<Label>;`

### 1.2 Système de Skin

**Fichiers à créer :**

- `[Source/GUI/Themes/Skin.h](Source/GUI/Themes/Skin.h)`
- `[Source/GUI/Themes/Skin.cpp](Source/GUI/Themes/Skin.cpp)`
- `[Source/GUI/Themes/SkinValues.h](Source/GUI/Themes/SkinValues.h)` (enums de valeurs)

**Responsabilités :**

`Skin` :

- Remplace progressivement `Theme` (coexistence au début)
- Maps centralisées : `std::map<SkinValueId, float> values_`
- Maps centralisées : `std::map<SkinColourId, Colour> colours_`
- Méthodes : `getValue(SkinValueId)`, `getColour(SkinColourId)`
- Support des variantes (Black/Cream) comme `Theme` actuel
- **Chargement JSON futur** (pas pour cette phase, mais prévu dans l'architecture)

`SkinValues.h` :

- Définition de tous les `enum class SkinValueId`
- Définition de tous les `enum class SkinColourId`
- Valeurs par défaut : `constexpr float kDefaultValues[]`
- Couleurs par défaut : `const Colour kDefaultColours[]`

### 1.3 Classe de base pour sections/panels

**Fichiers à créer :**

- `[Source/GUI/OpenGL/OpenGlSection.h](Source/GUI/OpenGL/OpenGlSection.h)`
- `[Source/GUI/OpenGL/OpenGlSection.cpp](Source/GUI/OpenGL/OpenGlSection.cpp)`

**Responsabilités :**

`OpenGlSection` :

- Base pour tous les panels (HeaderPanel, BodyPanel, etc.)
- Registry de composants : `std::map<std::string, Slider*> sliderLookup_`
- Registry OpenGL : `std::vector<OpenGlComponent*> openGlComponents_`
- Hierarchical repaint : `repaintChildBackground(Component*)`, `repaintOpenGlComponent(OpenGlComponent*)`
- Lookup de valeurs skin hiérarchique (avec overrides locaux)
- Méthode `renderOpenGlComponents(OpenGLContext&, bool animate)`

### 1.4 Intégration OpenGL dans PluginEditor

**Fichiers à modifier :**

- `[Source/GUI/PluginEditor.h](Source/GUI/PluginEditor.h)`
- `[Source/GUI/PluginEditor.cpp](Source/GUI/PluginEditor.cpp)`

**Modifications :**

- Ajouter `juce::OpenGLContext openGlContext_`
- Attacher le contexte dans le constructeur : `openGlContext_.attachTo(*this)`
- Implémenter `OpenGLRenderer` pour le rendu custom
- Callbacks OpenGL : `newOpenGLContextCreated()`, `renderOpenGL()`, `openGLContextClosing()`
- Maintenir une liste de composants OpenGL à rendre
- Batching du rendu : tous les composants en un seul pass

### 1.5 Tests avec Label (validation de l'infrastructure)

**Fichier à créer :**

- `[Source/GUI/Widgets/OpenGL/OpenGlLabel.h](Source/GUI/Widgets/OpenGL/OpenGlLabel.h)`
- `[Source/GUI/Widgets/OpenGL/OpenGlLabel.cpp](Source/GUI/Widgets/OpenGL/OpenGlLabel.cpp)`

**Objectif :**

Migrer `Label` vers l'architecture OpenGL pour valider que :

- Le texte est net (HiDPI fonctionne)
- Le lazy rendering fonctionne (`force` flag)
- Le cache de skin fonctionne (pas d'appels répétés à `theme->getXxx()`)
- Les performances sont bonnes
- **Le texte reste net lors du resize/zoom** (test crucial pour la suite)

**Tests de zoom avec Label :**

- Tester zoom 50%, 100%, 150%, 200%
- Vérifier que le texte est re-rendu à la bonne résolution (pas de scaling bitmap)
- Vérifier que le resize est fluide (pas de lag)

**Si les tests avec Label échouent**, on ajuste l'infrastructure avant de continuer.

### Livrables Phase 1

- Infrastructure OpenGL complète et testée
- Système de Skin fonctionnel (coexiste avec Theme)
- Label migré et fonctionnel avec OpenGL
- Documentation des patterns à suivre pour les autres widgets

**Estimation : 1 grosse session de travail (3-4h)**

---

## Phase 2 : Migration des widgets texte (Étape incrémentale 1)

### Widgets à migrer

- `GroupLabel`
- `SectionHeader`
- `ModuleHeader`

**Raison du regroupement :** Widgets similaires au Label, uniquement du texte + décoration simple.

### Pattern de migration

Pour chaque widget :

1. Créer `OpenGlXxx` qui hérite de `OpenGlImageComponent`
2. Implémenter `updateSkinCache()` pour cacher couleurs/fonts
3. Implémenter `redrawImage(bool force)` pour le rendu
4. Marquer comme `staticImage_` si le contenu ne change jamais
5. Tester individuellement

### Tests de validation

- Texte net sur écran Retina
- Pas de flou
- Changement de thème fluide
- Pas de ralentissement visible

**Estimation : 1 session (2h)**

---

## Phase 7 : Système de zoom/resize GUI

**Cette phase sera traitée APRÈS la Phase 6**, une fois que tous les widgets sont migrés et que l'infrastructure OpenGL est stable.

### 7.1 Architecture du zoom

**Deux modes de zoom :**

1. **Zoom discret** : Menu déroulant avec facteurs pré-établis
  - Options : 50%, 75%, 100%, 125%, 150%, 200%
  - Changement instantané avec régénération des textures
  - Sauvegarde de la préférence utilisateur
2. **Zoom continu** : Resizer dans un coin de la fenêtre
  - Drag libre pour n'importe quelle taille
  - Contraintes : ratio aspect maintenu, taille min/max
  - Régénération progressive des textures pendant le drag

### 7.2 Implémentation technique

**Fichiers à créer :**

- `[Source/GUI/Zoom/ZoomManager.h](Source/GUI/Zoom/ZoomManager.h)`
- `[Source/GUI/Zoom/ZoomManager.cpp](Source/GUI/Zoom/ZoomManager.cpp)`
- `[Source/GUI/Widgets/ZoomMenu.h](Source/GUI/Widgets/ZoomMenu.h)` (ComboBox pour sélection zoom)
- `[Source/GUI/Widgets/CornerResizer.h](Source/GUI/Widgets/CornerResizer.h)` (drag handle)

**Responsabilités ZoomManager :**

- Maintenir le facteur de zoom courant (`currentScale_`)
- Notifier tous les composants OpenGL lors du changement
- Calculer les nouvelles dimensions de la fenêtre
- Déclencher la régénération des textures avec `redrawImage(true)`
- Sauvegarder/restaurer la préférence utilisateur

**Pattern de régénération :**

```cpp
void ZoomManager::setZoomFactor(float newScale) {
    if (std::abs(currentScale_ - newScale) < 0.001f)
        return; // Pas de changement
    
    currentScale_ = newScale;
    
    // 1. Recalculer les dimensions de la fenêtre
    int newWidth = static_cast<int>(baseWidth_ * currentScale_);
    int newHeight = static_cast<int>(baseHeight_ * currentScale_);
    
    // 2. Redimensionner la fenêtre
    pluginEditor_->setSize(newWidth, newHeight);
    
    // 3. Notifier tous les composants OpenGL
    for (auto* component : openGlComponents_) {
        component->setScaleFactor(currentScale_);
        component->redrawImage(true); // Force redraw à la nouvelle résolution
    }
    
    // 4. Sauvegarder la préférence
    saveZoomPreference(currentScale_);
}
```

### 7.3 Rendu vectoriel net à toutes échelles

**Principe clé :** Les widgets doivent re-rendre leur contenu à la résolution cible, pas scaler une bitmap existante.

**Implémentation dans OpenGlImageComponent :**

```cpp
void OpenGlImageComponent::redrawImage(bool force) {
    // Calculer la résolution effective (HiDPI + zoom)
    float effectiveScale = displayScale_ * zoomScale_;
    
    int imageWidth = static_cast<int>(getWidth() * effectiveScale);
    int imageHeight = static_cast<int>(getHeight() * effectiveScale);
    
    // Créer image à la résolution cible
    drawImage_ = std::make_unique<Image>(
        Image::ARGB, imageWidth, imageHeight, true
    );
    
    Graphics g(*drawImage_);
    
    // Appliquer le scaling AVANT le rendu vectoriel
    g.addTransform(AffineTransform::scale(effectiveScale));
    
    // Rendre en vectoriel (texte, formes, etc.)
    paintToImage(g); // Appel virtuel, implémenté par les sous-classes
    
    // Upload vers texture OpenGL
    uploadToTexture();
}
```

**Résultat :** Le texte et les formes sont rendus en vectoriel à la résolution finale, garantissant la netteté.

### 7.4 Optimisations pour le resize continu

**Problème :** Régénérer toutes les textures à chaque frame pendant le drag serait trop coûteux.

**Solutions :**

1. **Throttling** : Limiter la fréquence de régénération (ex: 30 FPS pendant le drag)
2. **Scaling temporaire** : Pendant le drag, scaler les textures existantes (acceptable car temporaire)
3. **Régénération finale** : Une fois le drag terminé, régénérer toutes les textures à la résolution finale
4. **Composants statiques** : Ne régénérer que les composants visibles et non-statiques

**Implémentation :**

```cpp
void CornerResizer::mouseDrag(const MouseEvent& e) {
    // Calculer nouvelle taille
    float newScale = calculateScaleFromMousePosition(e.position);
    
    // Pendant le drag : scaling rapide (GPU)
    zoomManager_->setTemporaryScale(newScale);
    
    // Throttling : régénération partielle toutes les 33ms
    if (Time::getMillisecondCounter() - lastRegenTime_ > 33) {
        zoomManager_->regenerateVisibleComponents();
        lastRegenTime_ = Time::getMillisecondCounter();
    }
}

void CornerResizer::mouseUp(const MouseEvent&) {
    // Fin du drag : régénération complète à la résolution finale
    zoomManager_->finalizeZoom();
}
```

### 7.5 Interface utilisateur

**Menu déroulant de zoom :**

- Position : HeaderPanel (à côté des boutons de thème)
- Affichage : "100%" (texte + icône loupe)
- Items : 50%, 75%, 100%, 125%, 150%, 200%
- Raccourcis clavier : Cmd/Ctrl + / Cmd/Ctrl -

**Corner resizer :**

- Position : Coin inférieur droit de la fenêtre
- Visuel : Icône de redimensionnement (3 lignes diagonales)
- Comportement : Drag pour resize libre
- Contraintes : Ratio aspect 16:9 maintenu, taille min 800x600, max 3840x2160

### 7.6 Tests de validation

**Tests de netteté :**

- Texte net à 50%, 100%, 200%
- Formes vectorielles nettes (pas de crénelage excessif)
- Pas de flou sur les widgets

**Tests de performance :**

- Changement de zoom discret < 200ms
- Resize continu fluide (30+ FPS pendant le drag)
- Pas de lag après le resize
- Utilisation CPU/GPU raisonnable

**Tests fonctionnels :**

- Tous les widgets fonctionnent à toutes les échelles
- Interactions souris correctes (hit testing avec scaling)
- Keyboard navigation OK
- Popup menus positionnés correctement

**Tests de régression :**

- Sauvegarde/restauration du zoom entre sessions
- Compatibilité multi-écrans (HiDPI + non-HiDPI)
- Pas de crash lors de resize extrême

### Livrables Phase 7

- Système de zoom complet et fonctionnel
- Menu déroulant + corner resizer
- Rendu vectoriel net à toutes échelles
- Documentation utilisateur (comment utiliser le zoom)
- Tests de validation passés

**Estimation : 1-2 sessions (3-4h)**

---

## Phase 3 : Migration Button (Étape incrémentale 2)

### Widget à migrer

- `Button` (états multiples : Normal, Hover, Down, Disabled)

### Complexité supplémentaire

- Cache de 4 états différents
- Animations de hover (interpolation de `hoverAmount_`)
- Gestion des interactions souris

### Approche

**Option A : Cache multi-images**

- 4 images séparées (une par état)
- Switch rapide entre images lors du changement d'état
- Inspiré de l'approche actuelle avec `cachedStates_`

**Option B : Rendu dynamique avec shaders (avancé)**

- Un seul quad OpenGL
- Shader fragment pour les couleurs/effets
- Interpolation GPU pour les animations
- Plus complexe mais plus performant

**Recommandation : Option A** (plus simple, prouvée dans Vital)

### Tests de validation

- Tous les états visibles et corrects
- Animations de hover fluides
- Click feedback immédiat
- Pas de scintillement

**Estimation : 1 session (2-3h)**

---

## Phase 4 : Migration Slider + NumberBox (Étape incrémentale 3)

### Widgets à migrer

- `Slider` (widget le plus complexe)
- `NumberBox` (édition de valeurs)

### Complexité Slider

- Rendu hybride : track static + value bar dynamique
- Texte dynamique (valeur + unité)
- Bordure de focus
- Drag interaction
- Keyboard navigation

### Approche Slider

**Rendu en deux passes :**

1. **Image cache statique** : track background (ne change qu'au resize/theme)
2. **Rendu dynamique direct** : value bar, texte, bordure focus (changent souvent)

**Inspiré de Vital :** `SynthSlider` utilise exactement cette approche (mode "quad" pour la partie dynamique).

### Complexité NumberBox

- Actuellement **AUCUN cache** (pire performance)
- 4 appels Theme dans `paint()` (coûteux)
- `GlyphArrangement` créé à chaque frame (très coûteux)

### Approche NumberBox

- Cache complet de l'image avec texte
- Cache de la largeur du texte (éviter `GlyphArrangement` répété)
- Cache des couleurs skin
- Régénération uniquement sur changement de valeur

**Gain attendu : 10-20x sur NumberBox** (widget le plus problématique actuellement)

### Tests de validation

- Slider smooth lors du drag
- Texte net et lisible
- NumberBox fluide lors de l'édition
- Pas de ralentissement même avec beaucoup de sliders visibles

**Estimation : 1-2 sessions (3-4h)**

---

## Phase 5 : Migration ComboBox + widgets restants (Étape incrémentale 4)

### Widgets à migrer

- `ComboBox`
- `EnvelopeDisplay`
- `PatchNameDisplay`
- `TrackGeneratorDisplay`
- Séparateurs (`VerticalSeparator`, `HorizontalSeparator`)
- `ModulationBusHeader`

### Approche par priorité

**Priorité 1 : ComboBox**

- Déjà optimisé avec cache dans l'implémentation actuelle
- Migration directe vers OpenGL texture
- Tests avec popup menu

**Priorité 2 : Displays**

- `EnvelopeDisplay` : courbes dynamiques (peut bénéficier de shaders OpenGL)
- `PatchNameDisplay` : simple texte + bordure
- `TrackGeneratorDisplay` : courbes complexes (shaders)

**Priorité 3 : Séparateurs + headers**

- Widgets très simples (lignes)
- Marquer comme `staticImage_` (jamais de redraw sauf resize)

### Tests de validation

- Tous les widgets fonctionnels
- Interface complète fluide
- Changement de thème < 100ms (vs 4-5s actuellement)
- Pas de bugs visuels

**Estimation : 1-2 sessions (3-4h)**

---

## Phase 6 : Cleanup et optimisations finales

### Tâches

1. **Supprimer l'ancien système Theme** (remplacé par Skin)
2. **Supprimer les anciens widgets non-OpenGL** (si tous migrés)
3. **Profiling avec Instruments** (valider les gains de performance)
4. **Ajustements fins** :
  - Tuning des flags `staticImage_`
  - Optimisation des tailles de texture
  - Réduction des allocations
5. **Documentation** :
  - Guide de migration pour futurs widgets
  - Patterns OpenGL recommandés
  - Architecture finale

### Validation finale

**Critères de succès :**

- ✅ Texte net sur tous les écrans (HiDPI/Retina)
- ✅ Pas de déformation de widgets
- ✅ Changement de thème < 100ms (vs 4-5s avant)
- ✅ Interface fluide 60 FPS
- ✅ Utilisation CPU réduite de 50-70%
- ✅ Aucun bug visuel ou fonctionnel

**Tests de régression :**

- Tous les widgets interactifs fonctionnent
- Keyboard navigation OK
- Focus management OK
- Popup menus OK
- Redimensionnement fenêtre OK

**Estimation : 1 session (2h)**

---

## Risques et mitigation

### Risque 1 : OpenGL non disponible sur certains systèmes

**Mitigation :**

- Détecter disponibilité OpenGL au runtime
- Fallback vers rendu CPU classique si nécessaire
- Log warning si OpenGL indisponible

### Risque 2 : Bugs subtils avec threading OpenGL

**Mitigation :**

- Utiliser mutex pour accès aux textures (pattern Vital)
- `MessageManager::callAsync()` pour updates GUI depuis autres threads
- Tests sur différentes configurations (macOS, Windows)

### Risque 3 : Consommation mémoire des textures

**Mitigation :**

- Libérer textures des composants invisibles
- Partager textures pour widgets identiques (ex: séparateurs)
- Monitoring de la VRAM utilisée

### Risque 4 : Régression fonctionnelle

**Mitigation :**

- Tests manuels après chaque phase
- Captures d'écran avant/après pour comparaison visuelle
- Liste de checks fonctionnels (interactions, keyboard, etc.)

---

## Dépendances externes

### Bibliothèques tierces

**Optionnel (futur) :** Moodycamel's ConcurrentQueue pour communication lock-free

- URL : [https://github.com/cameron314/concurrentqueue](https://github.com/cameron314/concurrentqueue)
- Usage : Si on ajoute des updates audio → GUI (pas nécessaire pour Phase 1-6)

**Pas besoin de dépendances supplémentaires** pour l'instant, JUCE fournit tout ce qu'il faut :

- `juce::OpenGLContext`
- `juce::OpenGLTexture`
- `juce::OpenGLShaderProgram` (si on utilise des shaders)

---

## Ordre d'exécution recommandé

1. **Rollback Git** (pré-requis)
2. **Phase 1** : Infrastructure complète + test avec Label (incluant support zoom dès la conception)
  - **STOP si Label ne fonctionne pas bien** → debug infrastructure
  - **STOP si le zoom du Label n'est pas net** → ajuster le pixel scaling
3. **Phase 2** : Widgets texte simples (GroupLabel, Headers)
4. **Phase 3** : Button (états multiples)
5. **Phase 4** : Slider + NumberBox (complexes, gains majeurs)
6. **Phase 5** : ComboBox + displays + séparateurs
7. **Phase 6** : Cleanup, profiling, documentation
8. **Phase 7** : Système de zoom GUI complet (menu + resizer)

**Durée totale estimée : 13-19h de travail sur 5-7 jours (incluant Phase 7 zoom)**

---

## Références

### Documentation JUCE OpenGL

- OpenGL in JUCE: [https://docs.juce.com/master/tutorial_open_gl_application.html](https://docs.juce.com/master/tutorial_open_gl_application.html)
- OpenGLContext: [https://docs.juce.com/master/classOpenGLContext.html](https://docs.juce.com/master/classOpenGLContext.html)
- OpenGLTexture: [https://docs.juce.com/master/classOpenGLTexture.html](https://docs.juce.com/master/classOpenGLTexture.html)

### Code source Vital Audio

- Repo GitHub : [https://github.com/mtytel/vital](https://github.com/mtytel/vital)
- Fichiers clés :
  - `src/interface/look_and_feel/open_gl_component.h/cpp`
  - `src/interface/look_and_feel/open_gl_image_component.h/cpp`
  - `src/interface/editor_sections/synth_section.h/cpp`
  - `src/interface/look_and_feel/skin.h/cpp`

### Document d'analyse

- `[Documentation/References/MD/Vital-Audio-Architecture-Analysis.md](Documentation/References/MD/Vital-Audio-Architecture-Analysis.md)`

---

## Notes importantes

### Clean Code

- Respecter les principes SOLID à chaque étape
- Fonctions < 15 lignes (extraire helpers si nécessaire)
- Classes < 200 lignes (séparer responsabilités)
- Noms explicites révélant l'intention
- Pas de duplication de code
- Early returns, guard clauses

### Git workflow

- **Commit après chaque phase** (pas au milieu)
- Messages de commit descriptifs en anglais
- Tag Git annoté après Phase 1, 4 et 6 (milestones)
- Format tag : liste à puces des changements majeurs

### Testing

- Compiler après chaque phase
- Tests manuels : interactions, thème, focus, keyboard
- Profiling avec Instruments après Phase 6
- Captures d'écran avant/après pour comparaison

---

## Conclusion

Cette refactorisation hybride (infrastructure complète + migration incrémentale) nous permettra de :

1. **Résoudre tous les problèmes actuels** : flou, déformations, performances
2. **Avoir une architecture moderne et évolutive** : inspirée d'un projet pro (Vital)
3. **Minimiser les risques** : validation à chaque étape
4. **Documenter les patterns** : réutilisables pour futurs widgets
5. **Préparer le terrain pour le zoom/resize** : architecture conçue dès le départ pour supporter le scaling dynamique

Le modèle de Vital a fait ses preuves (utilisé par des milliers de musiciens, interface ultra-fluide avec zoom fonctionnel). En l'adaptant à Matrix-Control, on s'assure d'une solution robuste et performante qui permettra un zoom fluide et net à toutes les échelles.