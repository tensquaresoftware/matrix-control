---
name: Correction de la déformation du texte des boutons
overview: Corriger la déformation du texte des boutons INIT, COPY, PASTE en ajustant la stratégie de dessin de l'image cachée HiDPI.
todos:
  - id: approach1-only-reduce
    content: Tester RectanglePlacement::onlyReduceInSize | centred
    status: completed
  - id: approach2-fill-destination
    content: "Si échec : tester RectanglePlacement::fillDestination"
    status: cancelled
  - id: approach3-logical-size
    content: "Si échec : revenir à image taille logique avec clear explicite"
    status: completed
isProject: false
---

# Correction de la déformation du texte des boutons

## Diagnostic

Le problème actuel :

- Texte net ✅
- Pas de transparence ✅  
- Mais texte déformé/grossi sur INIT, COPY, PASTE ❌
- STORE semble correct ✅

**Hypothèse** : Le `juce::RectanglePlacement::stretchToFit` déforme légèrement l'image HiDPI lors du mapping vers les bounds logiques, probablement à cause d'arrondis de pixels lors de la création de l'image à `width * pixelScale`.

## Solution proposée

Essayer **3 approches différentes** pour trouver celle qui fonctionne :

### Approche 1 : Utiliser `onlyReduceInSize`

Remplacer `stretchToFit` par `onlyReduceInSize | centred` pour éviter toute déformation par agrandissement.

### Approche 2 : Utiliser `fillDestination` sans stretch

Utiliser `fillDestination` qui remplit sans déformer les proportions.

### Approche 3 : Revenir à `drawImageAt` mais avec image à taille logique ET clear explicite

Créer l'image à taille logique, mais s'assurer qu'elle est bien opaque avec `clear()` avant de dessiner.

## Fichiers concernés

- `[Source/GUI/Widgets/Button.cpp](Source/GUI/Widgets/Button.cpp)` : méthode `paintButton()` ligne ~90

## Stratégie

1. Tester **Approche 1** en premier (le plus conservateur)
2. Si échec, tester **Approche 2**
3. Si échec, tester **Approche 3**

Chaque test nécessite :

- Modification du code
- Compilation
- Test par l'utilisateur sur écran Retina
- Feedback visuel (capture d'écran)

