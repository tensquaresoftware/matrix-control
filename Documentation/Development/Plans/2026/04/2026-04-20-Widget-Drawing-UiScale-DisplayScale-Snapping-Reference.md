# Référence de travail : dessin des widgets avec UI Scale, Display Scale et snapping

## Objectif du document

Ce document résume la problématique de rendu que nous devons traiter de manière homogène dans toutes les classes de widgets custom.

Le but est d'obtenir un rendu net et stable, quel que soit :

- le facteur d'échelle fonctionnel de la GUI (UI Scale),
- le facteur d'échelle système de l'écran (Display Scale / DPI),
- le type d'écran (1x, 1.25x, 1.5x, 2x, 3x, etc.).

Ce sujet ne concerne pas uniquement `Button`, mais toute classe qui dessine des traits, bordures, séparateurs, ticks, curseurs, texte ou formes fines.

---

## 1) Les trois notions à distinguer

### UI Scale (projet)

Le `UI Scale` est le facteur métier de la GUI du plugin.
Il exprime comment on redimensionne le design (50 %, 75 %, 100 %, 125 %, 150 %, 175 %, 200 %).

Exemple :
- largeur design d'un bouton = 48
- UI Scale = 150 %
- largeur logique attendue = 72

### Display Scale (système)

Le `Display Scale` est le facteur de densité du système/écran (Retina, Windows scaling, etc.).
Il convertit des pixels logiques en pixels physiques.

Exemple :
- Display Scale = 2.0 (Retina)
- 1 px logique correspond à 2 px physiques

### Snapping

Le snapping consiste à aligner certaines valeurs de dessin sur une grille cible (souvent la grille de pixels physiques) pour éviter les valeurs intermédiaires qui provoquent de l'anti-aliasing perceptible (traits flous, contours "mous", variations visuelles).

---

## 2) Pourquoi le rendu du Button paraissait "nickel" sur Retina

Cas de base :
- bordure logique interne du `Button` = 2 px (à UI Scale 100 % avant conversion physique),
- écran Retina = Display Scale 2.0.

À 100 % :
- 2 px logiques deviennent 4 px physiques.

Ce point est favorable car 4 est une valeur entière confortable pour le rendu des traits.
Avec tes paliers d'UI Scale, tu as observé :

- 50 % -> 2 px physiques
- 75 % -> 3 px physiques
- 100 % -> 4 px physiques
- 125 % -> 5 px physiques
- 150 % -> 6 px physiques
- 175 % -> 7 px physiques
- 200 % -> 8 px physiques

Sur ce matériel, le rendu est régulier et propre.

---

## 3) Pourquoi cela se complique sur écran non-Retina (ou scaling intermédiaire)

Sur un écran proche de 1x :
- 75 % de 2 px logiques donne 1.5 px logiques,
- 125 % donne 2.5,
- 175 % donne 3.5.

Ces demi-valeurs tombent entre pixels physiques.
Le moteur de rendu doit lisser (anti-aliasing), ce qui peut donner des bordures moins nettes et une perception d'épaisseur variable.

Le problème existe aussi sur des facteurs système non entiers (1.25, 1.5, 1.75, etc.) si on ne contrôle pas explicitement la conversion logique -> physique.

---

## 4) Règle générale recommandée

Pour tous les traits critiques (bordure, ligne, séparateur, contour de focus), raisonner en 4 étapes :

1. Calculer l'épaisseur logique souhaitée depuis le design et le UI Scale.
2. Convertir cette épaisseur en pixels physiques avec le Display Scale réel.
3. Snapper en physique (arrondi contrôlé : `round`, `floor` ou `ceil` selon la politique visuelle).
4. Reconvertir en logique pour appeler les fonctions de dessin JUCE.

Formule conceptuelle :

- desiredLogical = f(designThickness, uiScale)
- physical = desiredLogical * displayScaleSystem
- snappedPhysical = snap(physical)
- snappedLogical = snappedPhysical / displayScaleSystem

Ainsi, le trait final correspond à une épaisseur physique maîtrisée.

---

## 5) Exemple concret : bordure de `Button`

### Données

- base design bordure = 2
- UI Scale variable
- Display Scale dépend de l'écran courant

### Intention produit

- garder un rendu naturel sur Retina,
- éviter les épaisseurs "à demi-pixel" visibles sur 1x,
- obtenir une progression cohérente sur tous les écrans.

### Progression possible en physique sur écran 1x (version conservative)

Si on snappe avec une logique de type "palier bas" :
- 50 % -> 1
- 75 % -> 1
- 100 % -> 2
- 125 % -> 2
- 150 % -> 3
- 175 % -> 3
- 200 % -> 4

Cette progression privilégie la stabilité visuelle et évite les demi-valeurs.

### Alternative

Une politique `round` est plus symétrique mais peut épaissir plus tôt certaines étapes.
Le choix `floor` vs `round` vs `ceil` est un choix de direction artistique.

---

## 6) Ce qu'il faut éviter

- Mélanger les rôles : UI Scale (métier) et Display Scale (système).
- Appliquer un snapping "à l'aveugle" sans tenir compte du Display Scale réel.
- Coder des cas spéciaux "Retina vs non-Retina" en dur.
- Laisser chaque widget inventer sa propre règle d'arrondi.

Le point clé : utiliser une règle unique, centralisée et réutilisable.

---

## 7) Stratégie de refactor pour toutes les classes widgets

### Cible architecture

Créer un helper commun de snapping de stroke, appelé par toutes les classes concernées (`Button`, `Slider`, `ComboBox`, `Label`, séparateurs, widgets d'affichage, etc.).

### Contrat souhaité

Entrées :
- épaisseur design,
- UI Scale,
- Display Scale système du display actif,
- stratégie de snap (`round`, `floor`, `ceil`).

Sortie :
- épaisseur logique finale prête pour `Graphics`.

### Bénéfices

- cohérence visuelle globale,
- comportement prévisible sur Mac/Windows/Linux,
- réduction des régressions lors de l'ajout de nouveaux widgets.

---

## 8) Plan de validation visuelle recommandé

Pour chaque widget :

1. Tester UI Scale : 50, 75, 100, 125, 150, 175, 200.
2. Tester au moins deux Display Scales : 1x et 2x (idéalement 1.25x/1.5x aussi).
3. Vérifier :
   - netteté des bordures,
   - stabilité de l'épaisseur entre états (normal, hover, down, disabled),
   - absence de clipping et de décalage de texte.
4. Capturer des screenshots de référence pour figer la baseline.

---

## 9) Décision de vocabulaire à garder

Pour éviter toute ambiguïté dans le code et la doc :

- `uiScale` = facteur d'échelle du plugin (métier GUI),
- `displayScale` (ou `systemDisplayScale`) = facteur d'échelle système de l'écran.

Ne pas réutiliser un seul nom pour les deux concepts.

---

## Conclusion

Le cas de la bordure de `Button` a servi de révélateur :

- le rendu peut paraître parfait sur Retina tout en restant fragile ailleurs,
- la bonne solution n'est pas un patch ponctuel par widget,
- il faut une politique de conversion logique -> physique -> snapping -> logique, partagée et appliquée partout.

Ce document sert de base pour la prochaine passe de nettoyage graphique sur l'ensemble des widgets custom.
