# Synthèse : GUI JUCE redimensionnable, échelle et rendu net

Document de référence rapide pour un plugin **JUCE 8** (VST/AU) avec UI vectorielle custom, **facteur d’échelle** (presets %) + **coin redimensionnable** à ratio fixe, sans flou ni décalages cumulés.

---

## 1. Stratégie d’architecture recommandée

- **Une seule source de vérité pour l’échelle** : définir une taille de design de référence (100 %) et calculer  
  `scale = f(taille_actuelle_client, design)` (souvent via le minimum des ratios largeur/hauteur si le ratio est imposé).
- **Ne pas** utiliser un `AffineTransform` global sur toute l’arborescence comme moteur principal du redimensionnement utilisateur : risque de **flou** (texte, sous-pixel, filtrage) et de comportement moins prévisible qu’un layout explicite.
- **Recalculer le layout** dans `resized()` (ou équivalent) : coordonnées de design × `scale`, puis **appliquer une politique d’arrondi unique** pour les `setBounds` — éviter de multiplier le facteur à chaque niveau de la hiérarchie (source typique de **décalages** et **clipping**).
- **Ratio largeur/hauteur** : `ComponentBoundsConstrainer` sur l’éditeur ; redimensionnement manuel via un coin redimensionnable JUCE (`ResizableCornerComponent`, etc.) branché correctement sur la fenêtre / le peer.
- **ComboBox** : afficher le pourcentage (et éventuellement la valeur décimale) **dérivée du scale courant** pendant le drag du resizer ; distinguer si besoin le mode « preset choisi » vs « taille manuelle ».

---

## 2. Bounds entiers vs dessin flottant (et lien AffineTransform / flou)

- Les **`Component` JUCE ont des bounds en entiers** : `setBounds` travaille en pixels entiers dans l’espace logique JUCE.
- Le **dessin** dans `paint()` peut utiliser des **valeurs flottantes** (paths, rectangles flottants, épaisseurs) pour un rendu plus lisse **lorsque c’est cohérent avec votre grille**.
- Appliquer une **mise à l’échelle globale par `AffineTransform`** sur tout le composant pousse souvent le rendu (dont le texte) dans un régime **sous-pixel / filtré** → **bords flous**, surtout sur une UI minimaliste vectorielle.
- **Pratique** : **layout et tailles des composants** = entiers stables (après arrondi) ; **détails graphiques** dans `paint` = float si utile, avec **alignement / snap** contrôlé pour les traits qui doivent rester nets.

Référence utile (forum JUCE, cité notamment par [Melatonin — big list of tips](https://melatonin.dev/blog/big-list-of-juce-tips-and-tricks/)) : [AffineTransform scale blurry edges](https://forum.juce.com/t/affinetransform-scale-blurry-edges/42268/2).  
Article complémentaire : [How JUCE components work](https://melatonin.dev/blog/how-juce-components-work/) (melatonin.dev).

---

## 3. Pixels logiques (JUCE)

- Les dimensions que vous donnez à JUCE (taille de fenêtre, `setBounds`, etc.) sont en **pixels logiques** : elles tiennent compte du contexte d’affichage / DPI ; ce ne sont pas directement les **pixels physiques** de l’écran.
- Pour des règles du type « **1 px à l’écran** » pour une bordure à 50 % / 75 % / 100 % et **2 px** à 200 %, il faut raisonner en **épaisseur souhaitée en pixels d’affichage** (ou une règle explicite du type `max(1, round(scale_utilisateur))`) et la **reconcilier** avec l’échelle logique — sans mélanger deux fois la même échelle dans la même chaîne de calcul.

---

## 4. Bordures, traits et géométrie « continue »

- **Séparer** :
  - **Géométrie de mise en page** (marges, tailles des zones, rayons de layout) → suit `scale` à partir du design.
  - **Épaisseur des traits** (bordures, séparateurs) → règle dédiée en **pixels d’écran** (ou dérivée contrôlée de `scale` + DPI), pour éviter des traits qui disparaissent ou doublent l’échelle par erreur.
- **Arrondis et clipping** : une seule fonction utilitaire de **snap des rectangles** (floor/ceil cohérents) pour que les frères adjacents ne laissent pas de trou d’1 px ni ne débordent du parent ; attention à la somme des hauteurs/largeurs arrondies vs la taille du conteneur.

---

## 5. Performance : zéro allocation dans `paint()`

- Traiter le **thread d’affichage** avec la même discipline que l’audio quand c’est possible : **éviter les allocations dans `paint()`** (strings temporaires coûteuses, `Path` recréés à chaque frame, etc.).
- Préférer des **membres réutilisables** : `juce::Path`, `juce::Image`, `juce::ColourGradient`, etc., initialisés ou mis à jour hors du chemin chaud du paint quand la taille / le scale change.
- Sous **resize continu**, le nombre de `repaint` augmente : ce point devient vite sensible.

(Source : même esprit que la section *« For best performance, treat drawing like you would treat the audio thread »* dans [la liste Melatonin](https://melatonin.dev/blog/big-list-of-juce-tips-and-tricks/).)

---

## 6. Astuces complémentaires (blog Melatonin & outillage)

| Sujet | Conseil |
|--------|--------|
| **Débogage visuel** | `JUCE_ENABLE_REPAINT_DEBUGGING=1` : surligner les zones repeintes (détecter jank / sur-redessin pendant le resize). Voir aussi [Dealing with jank in JUCE](https://melatonin.dev/blog/dealing-with-jank-in-juce/). |
| **Réglage fin des constantes** | `JUCE_LIVE_CONSTANT(...)` pour ajuster ombres, opacités, rayons sans cycle recompile à chaque micro-valeur. |
| **Inspection hiérarchie** | [Melatonin Inspector](https://github.com/sudara/melatonin_inspector) pour vérifier tailles et empilement des composants. |
| **Widgets entièrement custom** | Dessin principal dans `paint` plutôt qu’une sur-ingénierie LookAndFeel *si* vous ne ciblez pas une API de thème générique ; hybride possible avec widgets stock. |
| **Paramètres / thread** | `parameterChanged` / `parameterValueChanged` peuvent être appelés depuis le **thread audio** : ne pas y faire de `repaint()` lourd ; patterns type flag atomique + `Timer`, ou `VBlankAttachment` (JUCE ≥ 7.0.6) pour rafraîchir proprement l’UI. |

---

## 7. Ce qu’il vaut mieux éviter (retour d’expérience)

- **Scale propagé manuellement** à chaque niveau + arrondis différents → décalages et clips.
- **Cache image** de toute l’UI pour le resize dynamique : souvent **flou** ou artefacts, peu adapté à une UI vectorielle qui doit rester nette à toutes les tailles.
- **`AffineTransform` global** comme unique levier d’échelle utilisateur pour une UI « produit » très nette.

---

## 8. Références externes

- [The big list of JUCE tips and tricks](https://melatonin.dev/blog/big-list-of-juce-tips-and-tricks/) (Melatonin, mis à jour 2024)  
- [AffineTransform scale blurry edges (JUCE forum)](https://forum.juce.com/t/affinetransform-scale-blurry-edges/42268/2)  
- [How JUCE components work](https://melatonin.dev/blog/how-juce-components-work/)  
- [Dealing with jank in JUCE](https://melatonin.dev/blog/dealing-with-jank-in-juce/)

---

*Document généré pour usage personnel — à adapter à votre structure de projet (éditeur seul vs fenêtre avec peer, CMake, etc.).*
