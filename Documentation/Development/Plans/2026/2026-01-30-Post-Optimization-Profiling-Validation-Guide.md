# Guide de Profiling Post-Optimisation

## Objectif

Valider les gains de performance réels après l'implémentation du système de cache d'images sur les 4 widgets critiques (Label, Slider, ComboBox, Button).

## Prérequis

✅ **Build RelWithDebInfo** avec les optimisations compilé
✅ **Fichier dSYM** présent dans `Builds/macOS/Matrix-Control_artefacts/RelWithDebInfo/Standalone/`

## Procédure Rapide

### 1. Vérifier le Build (Déjà fait)

Le build RelWithDebInfo avec les optimisations est prêt.

### 2. Lancer Instruments

```bash
open -a Instruments
```

**Configuration** :
- Template : **Time Profiler**
- Target : **Matrix-Control.app** (Standalone)
- Options :
  - ☐ Record Kernel Callstacks (désactivé)
  - ☐ Hide System Libraries (désactivé)
  - ☐ Invert Call Tree (désactivé)

### 3. Profiler le Même Scénario

**Important** : Reproduire exactement le même scénario que le profiling initial pour comparer.

**Scénario** : 10 changements de thème
1. Lancer le plugin standalone via Instruments (bouton rouge)
2. Attendre que l'interface soit complètement chargée
3. Cliquer 10 fois alternativement sur les boutons BLACK et CREAM
   - BLACK → CREAM → BLACK → CREAM (etc.)
   - Attendre ~1 seconde entre chaque clic
4. Arrêter l'enregistrement (bouton carré noir)

### 4. Analyser les Résultats

#### Filtre Input Filter

En bas à gauche, taper : `tss::`

#### Call Tree Options

- ☐ Separate by Thread
- ☐ Hide System Libraries (désactivé)
- ☐ Invert Call Tree (désactivé)

#### Trier par "Weight"

Cliquer sur l'en-tête de colonne **"Weight"** pour trier par temps total.

#### Déployer l'arborescence

- Cliquer sur les triangles pour déplier
- **Astuce** : ALT + clic pour déplier tous les niveaux d'un coup

### 5. Comparer avec les Résultats Initiaux

**Avant Optimisation** :
- Temps total : 28.26 s pour 10 changements
- `tss::Label::paint()` : 9.49 s (33.6%)
- `tss::Slider::paint()` : 6.10 s (21.6%)
- `tss::ComboBox::paint()` : 4.86 s (17.2%)
- `juce::Button::paint()` : 4.69 s (16.6%)

**Après Optimisation** (attendu) :
- Temps total : **~2-3 s** pour 10 changements (gain ~10x)
- `tss::Label::paint()` : **~0.2 s** (gain ~15x)
- `tss::Slider::paint()` : **~0.2 s** (gain ~7x)
- `tss::ComboBox::paint()` : **~0.1 s** (gain ~8x)
- `tss::Button::paintButton()` : **~0.1 s** (gain ~10x)

**Nouvelles fonctions visibles** :
- `tss::Label::regenerateCache()` - Doit être < 50 ms total
- `tss::Slider::regenerateTrackCache()` - Doit être < 50 ms total
- `tss::ComboBox::regenerateBackgroundCache()` - Doit être < 50 ms total
- `tss::Button::regenerateStateCache()` - Doit être < 100 ms total

### 6. Documenter les Résultats

Prendre des screenshots et noter les métriques :

1. **Temps total** pour 10 changements
2. **Temps CPU** pour chaque widget critique :
   - `tss::Label::paint()`
   - `tss::Slider::paint()`
   - `tss::ComboBox::paint()`
   - `tss::Button::paintButton()`
3. **Temps des fonctions de cache** :
   - `regenerateCache()` pour Label
   - `regenerateTrackCache()` pour Slider
   - `regenerateBackgroundCache()` pour ComboBox
   - `regenerateStateCache()` pour Button

### 7. Mettre à Jour la Documentation

Ajouter les résultats réels dans :
- `Documentation/Development/GUI/GUI-Optimization-Plan.md`
  - Section "Profiling Post-Optimisation"
  - Section "Métriques de Succès"

## Questions à Répondre

- ✅ Le temps total a-t-il été divisé par ~10 ?
- ✅ Les fonctions `paint()` des 4 widgets prennent-elles maintenant < 5% du temps total ?
- ✅ Les fonctions `regenerateCache()` sont-elles raisonnablement rapides (< 50-100 ms) ?
- ✅ Y a-t-il des nouveaux hotspots inattendus ?

## En Cas de Problème

### Les gains ne sont pas au rendez-vous

**Vérifier** :
- Le build RelWithDebInfo est bien utilisé (pas Debug)
- Le cache est bien invalidé uniquement quand nécessaire
- Les fonctions `regenerateCache()` ne sont pas appelées trop souvent

### Nouveaux hotspots identifiés

**Si d'autres widgets apparaissent maintenant comme critiques** (> 5% du temps) :
- Les documenter
- Considérer d'appliquer la même stratégie de cache

### Artefacts visuels ou bugs

**Si des problèmes visuels apparaissent** :
- Noter lesquels widgets/scénarios sont affectés
- Vérifier l'invalidation du cache (resized, setTheme, setText, etc.)
- Vérifier que les dimensions du cache correspondent bien aux dimensions du widget

---

**Date de création** : 30/01/2026
**Auteur** : Agent IA Cursor
