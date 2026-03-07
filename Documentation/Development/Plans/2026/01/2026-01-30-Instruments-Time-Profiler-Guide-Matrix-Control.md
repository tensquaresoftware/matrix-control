# Matrix-Control / Guide de Profiling avec Instruments

**Date** : 30 janvier 2026  
**Phase** : Phase 3 - Mesure et Profiling  
**Build** : RelWithDebInfo (optimisations ON + symboles de debug ON)

---

## 1. Préparation ✅ TERMINÉ

Le build RelWithDebInfo est prêt :
- **Chemin** : `/Volumes/Guillaume/Dev/Projects/MAO/Plugins/Matrix-Control/Builds/macOS/Matrix-Control_artefacts/RelWithDebInfo/Standalone/Matrix-Control.app`
- **Taille** : 12 MB
- **Architecture** : arm64 (Apple Silicon)
- **Symboles de debug** : Présents

---

## 2. Lancer Instruments

### Option 1 : Depuis le terminal
```bash
open -a Instruments
```

### Option 2 : Depuis Spotlight
- Cmd + Espace
- Taper "Instruments"
- Entrée

---

## 3. Configuration du Profiling

### Étape 1 : Choisir le Template
1. Dans la fenêtre de sélection, **clique sur "Time Profiler"**
2. Clique sur "Choose"

### Étape 2 : Sélectionner le Processus
1. En haut à gauche, tu verras un menu déroulant "Choose Target"
2. Clique dessus
3. **Choisis "Matrix-Control"** (ton app standalone)
   - Si elle n'apparaît pas, lance-la d'abord :
     ```bash
     open /Volumes/Guillaume/Dev/Projects/MAO/Plugins/Matrix-Control/Builds/macOS/Matrix-Control_artefacts/RelWithDebInfo/Standalone/Matrix-Control.app
     ```

### Étape 3 : Configurer les Options
1. Dans la barre d'outils, clique sur le bouton "⚙️" (Settings)
2. Assure-toi que :
   - **"High Frequency"** est coché (échantillonnage toutes les 100 µs)
   - **"Record Kernel Callstacks"** est décoché (on ne profile pas le kernel)

---

## 4. Scénarios de Profiling

### 🎯 Scénario 1 : Lancement du Plugin (PRIORITÉ HAUTE)

**Objectif** : Mesurer le temps de création de l'interface

**Procédure** :
1. **Ferme l'application** Matrix-Control si elle est ouverte
2. **Clique sur le cercle rouge** dans Instruments pour démarrer l'enregistrement
3. **Lance l'application** :
   ```bash
   open /Volumes/Guillaume/Dev/Projects/MAO/Plugins/Matrix-Control/Builds/macOS/Matrix-Control_artefacts/RelWithDebInfo/Standalone/Matrix-Control.app
   ```
4. **Attends 5 secondes** après l'ouverture complète de l'interface
5. **Clique sur le carré noir** pour arrêter l'enregistrement
6. **Ferme l'application**

**Durée** : ~10 secondes

---

### 🎯 Scénario 2 : Changement de Thème (PRIORITÉ HAUTE)

**Objectif** : Mesurer le temps de `setTheme()` + repaint

**Procédure** :
1. **Lance l'application** Matrix-Control
2. **Clique sur le cercle rouge** dans Instruments
3. **Clique alternativement sur les boutons BLACK et CREAM** (10 fois chacun)
   - BLACK → CREAM → BLACK → CREAM → etc.
4. **Attends 2 secondes**
5. **Clique sur le carré noir** pour arrêter

**Durée** : ~15 secondes

---

### 🎯 Scénario 3 : Interactions avec les Sliders (PRIORITÉ MOYENNE)

**Objectif** : Mesurer la réactivité du rendu pendant le drag

**Procédure** :
1. **Lance l'application** Matrix-Control
2. **Clique sur le cercle rouge** dans Instruments
3. **Bouge plusieurs sliders** de manière continue (drag lent et fluide)
   - Essaye 5-10 sliders différents
   - Fais des mouvements lents pour générer beaucoup de repaints
4. **Attends 2 secondes**
5. **Clique sur le carré noir** pour arrêter

**Durée** : ~15 secondes

---

### 🎯 Scénario 4 : Ouverture de ComboBox (PRIORITÉ BASSE)

**Objectif** : Mesurer le rendu des menus déroulants

**Procédure** :
1. **Lance l'application** Matrix-Control
2. **Clique sur le cercle rouge** dans Instruments
3. **Ouvre et ferme plusieurs ComboBox** (5-10 fois)
4. **Attends 2 secondes**
5. **Clique sur le carré noir** pour arrêter

**Durée** : ~10 secondes

---

## 5. Analyser les Résultats

### Étape 1 : Vue d'ensemble
1. Une fois l'enregistrement arrêté, Instruments affiche un graphique du temps CPU
2. Les pics dans le graphique = moments de forte activité

### Étape 2 : Filtrer par Namespace
1. En bas de la fenêtre, tu verras la **"Call Tree"** (arbre d'appels)
2. Dans la barre de recherche en bas à droite, tape : **`tss::`**
3. Cela filtre pour ne montrer que les fonctions de ton code

### Étape 3 : Trier par "Self Time"
1. En haut de la Call Tree, clique sur l'en-tête de colonne **"Self Time"**
2. Clique une deuxième fois pour trier par ordre décroissant (du plus grand au plus petit)
3. **"Self Time"** = temps passé dans cette fonction uniquement (sans les appels enfants)

### Étape 4 : Identifier les Hotspots
Cherche les fonctions avec **> 5% du temps CPU total** :

**Exemple de ce que tu pourrais voir** :
```
Function Name                           Self Time    Total Time
─────────────────────────────────────────────────────────────────
tss::Slider::paint()                    45.2%        48.5%     <- HOTSPOT CRITIQUE !
tss::ComboBox::paint()                  12.8%        15.2%     <- HOTSPOT IMPORTANT
tss::Button::paintButton()               8.5%        10.1%     <- À surveiller
tss::Label::paint()                      3.2%         3.5%     <- OK, pas critique
tss::MainComponent::paint()              2.1%        95.8%     <- Normal (appelle tout)
```

### Étape 5 : Analyser les Sous-Appels
1. **Double-clique** sur une fonction hotspot (ex: `tss::Slider::paint()`)
2. Cela ouvre le code source et montre les lignes les plus coûteuses
3. Regarde quelles opérations prennent le plus de temps :
   - `juce::Graphics::fillPath()` → Rendu vectoriel coûteux
   - `juce::Graphics::drawText()` → Calcul de glyphes
   - `juce::Graphics::drawRect()` → Dessin de bordures

---

## 6. Capturer les Données pour l'Analyse

Pour chaque scénario, note :

### 📊 Données à Collecter

#### Temps Total
- **Durée totale** de l'enregistrement : ________ ms
- **Temps CPU actif** : ________ ms

#### Top 5 des Hotspots
1. **Fonction** : ________________  **Self Time** : ______ %
2. **Fonction** : ________________  **Self Time** : ______ %
3. **Fonction** : ________________  **Self Time** : ______ %
4. **Fonction** : ________________  **Self Time** : ______ %
5. **Fonction** : ________________  **Self Time** : ______ %

#### Widgets Prioritaires pour Phase 4
- [ ] Slider (si > 10% Self Time)
- [ ] ComboBox (si > 10% Self Time)
- [ ] Button (si > 10% Self Time)
- [ ] Label (si > 5% Self Time)
- [ ] Autres : ________________

---

## 7. Exporter les Résultats

### Option 1 : Capture d'écran
1. Cmd + Shift + 4
2. Sélectionne la zone de la Call Tree
3. Sauvegarde dans `Documentation/Development/GUI/Profiling/`

### Option 2 : Exporter le Trace
1. Menu : `File > Save`
2. Sauvegarde le fichier `.trace` dans `Documentation/Development/GUI/Profiling/`
3. Tu pourras le rouvrir plus tard avec Instruments

---

## 8. Interpréter les Résultats (Aide IA)

Une fois que tu as les données, partage-les avec moi en me donnant :

1. **Les 5 fonctions les plus coûteuses** avec leur % de Self Time
2. **Une capture d'écran** de la Call Tree (optionnel mais utile)
3. **Ton impression** : Qu'est-ce qui t'a surpris ? Qu'est-ce qui semble lent ?

Je t'aiderai à :
- Interpréter les résultats
- Prioriser les optimisations
- Décider quels widgets méritent un cache d'images (Phase 4)

---

## 9. Conseils et Astuces

### ✅ Bonnes Pratiques
- **Enregistrements courts** : 10-30 secondes max (plus facile à analyser)
- **Gestes répétitifs** : Répète les actions pour amplifier les hotspots
- **Ferme les autres apps** : Pour éviter le bruit dans les mesures

### ❌ Pièges à Éviter
- **Ne pas profiler en Debug** : Toujours utiliser RelWithDebInfo ou Release
- **Ne pas profiler avec Xcode attaché** : Le debugger ralentit tout
- **Ne pas profiler avec d'autres apps lourdes** : Ferme Chrome, etc.

### 🔍 Si tu ne vois pas les symboles `tss::`
- Vérifie que le build est bien en RelWithDebInfo
- Relance Instruments et réattache le processus
- Vérifie que tu filtres bien par `tss::` dans la barre de recherche

---

## 10. Prochaines Étapes

Après le profiling :

1. **Partage les résultats** avec moi
2. **On analyse ensemble** les hotspots
3. **On priorise** les widgets à optimiser
4. **On passe à la Phase 4** : Implémentation du cache d'images

---

**Bonne chance avec le profiling ! N'hésite pas à me partager tes résultats dès que tu les as.** 🚀
