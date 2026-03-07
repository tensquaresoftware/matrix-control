---
name: EnvelopeDisplay Widget Completion
overview: Compléter le widget EnvelopeDisplay (courbe DADSR, setters/getters, cache) et le mettre en communication bidirectionnelle avec les sliders Delay, Attack, Decay, Sustain et Release des 3 modules ENV, en suivant le même pattern que TrackGeneratorDisplay. L’édition à la souris est laissée en deuxième intention.
todos: []
isProject: false
---

# Plan EnvelopeDisplay – Widget et liaison aux sliders ENV

## Contexte

- **EnvelopeDisplay** actuel : [Source/GUI/Widgets/EnvelopeDisplay.cpp](Source/GUI/Widgets/EnvelopeDisplay.cpp) dessine uniquement fond, bordure et triangle décoratif ; aucune courbe DADSR ni liaison aux paramètres.
- **Référence** : [TrackGeneratorDisplay](Source/GUI/Widgets/TrackGeneratorDisplay.h) (courbe 5 points, cache image, `setX(value, bool notify)`, callback `onValueChanged`) et le câblage dans [PatchEditPanel](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.cpp) (Slider ↔ Display, APVTS uniquement via attachments sur les sliders).
- **Paramètres** : Delay, Attack, Decay, Sustain, Release (0–63) pour env1, env2, env3. IDs dans [PluginIDs.h](Source/Shared/PluginIDs.h) (namespace `PluginDescriptors::ParameterIds`), sliders dans [Env1Panel](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/BottomPanel/Modules/Env1Panel.cpp) aux indices 0–4 (Delay, Attack, Decay, Sustain, Release).
- **Mockup** : [Documentation/Development/GUI/Mockups/EnvelopeDisplay (Exemple 1).png](Documentation/Development/GUI/Mockups/EnvelopeDisplay%20(Exemple%201).png) — courbe DADSR (turquoise), 6 points / 5 segments, fond sombre, triangle sous le graphique.

---

## 1. Widget EnvelopeDisplay – Données et dessin de la courbe

**Fichiers** : [Source/GUI/Widgets/EnvelopeDisplay.h](Source/GUI/Widgets/EnvelopeDisplay.h), [Source/GUI/Widgets/EnvelopeDisplay.cpp](Source/GUI/Widgets/EnvelopeDisplay.cpp)

- **Constantes** : `kMinValue_` = 0, `kMaxValue_` = 63, `kPadding_`, `kLineThickness_` (éventuellement `kEnvelopeParamCount_` = 5). Conserver les constantes existantes (border, padding vertical, triangle).
- **Membres** : `delay_`, `attack_`, `decay_`, `sustain_`, `release_` (int 0–63) ; cache image + `cacheValid_` + copie des valeurs pour invalidation ; callback `onValueChanged_(paramIndex, newValue)` (paramIndex 0=Delay, 1=Attack, 2=Decay, 3=Sustain, 4=Release).
- **API publique** :
  - `setDelay(int value)` / `setDelay(int value, bool notify)` (idem pour Attack, Decay, Sustain, Release) ; getters `getDelay()` etc.
  - Les setters à un argument délèguent à `setX(value, true)`. Avec deux arguments : clamp 0–63, mise à jour de la valeur, `invalidateCache()`, et si `notify` et valeur modifiée, appel du callback.
  - `setOnValueChanged(ValueChangedCallback)` avec signature `void(int paramIndex, int newValue)`.
- **Dessin de la courbe DADSR** :
  - Zone utile : `bounds.reduced(kPadding_)`, axe Y : bas = 0, haut = 63.
  - Segments : (1) Delay : niveau 0 ; (2) Attack : montée de 0 à 63 ; (3) Decay : descente de 63 à Sustain ; (4) Sustain : plateau à niveau Sustain ; (5) Release : descente de Sustain à 0.
  - Répartition en X : proportionnelle aux valeurs (Delay, Attack, Decay, Release) ; segment Sustain avec largeur fixe ou proportionnelle pour lisibilité (ex. 10–15 % de la largeur totale). Gérer les cas où une valeur est 0 (éviter division par zéro, segment de longueur nulle).
  - Couleur : `skin_->getEnvelopeDisplayEnvelopeColour()`. Tracer un `Path` (lignes) sans cercles sur les points pour la phase 1 (option : petits cercles comme le mockup, à définir dans les constantes).
- **Cache** : comme TrackGeneratorDisplay — `regenerateCache()`, `invalidateCache()`, `updateSkinCache()` ; dans `paint()` : fond, bordure, triangle puis dessin de `cachedImage_` sur la zone contenu. Invalider le cache dans tous les setters quand la valeur change, et dans `setSkin()`.
- **Conserver** le triangle décoratif en bas du widget (déjà présent).

Pas de `mouseDown`/`mouseDrag`/`mouseUp` dans cette phase (édition souris en deuxième intention).

---

## 2. MiddlePanel – Sync APVTS → EnvelopeDisplay et accès aux displays

**Fichiers** : [Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/MiddlePanel/MiddlePanel.h](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/MiddlePanel/MiddlePanel.h), [Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/MiddlePanel/MiddlePanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/MiddlePanel/MiddlePanel.cpp)

- Dans **valueTreePropertyChanged** : pour chaque `id` parmi `kEnv1Delay`, `kEnv1Attack`, `kEnv1Decay`, `kEnv1Sustain`, `kEnv1Release` (et idem env2, env3), appeler le setter correspondant sur `envelope1Display_` / `envelope2Display_` / `envelope3Display_` avec `**false**` (mise à jour silencieuse, pas de callback).
- **Sync initiale** : dans le constructeur, après `addListener(this)` et `syncTrackGeneratorDisplayFromApvts()`, appeler une fonction `syncEnvelopeDisplaysFromApvts()` qui, pour chaque display (1, 2, 3), lit les 5 paramètres depuis l’APVTS (comme `getTrackPointValueFromApvts`) et appelle `setDelay(..., false)` … `setRelease(..., false)`. Factoriser une helper du type `int getEnvParameterFromApvts(const juce::String& parameterId) const` pour éviter la duplication.
- **Exposition des displays** : ajouter `tss::EnvelopeDisplay& getEnvelope1Display();` (et idem `getEnvelope2Display()`, `getEnvelope3Display()`). Alternative : `tss::EnvelopeDisplay& getEnvelopeDisplayAt(int index);` avec index 0–2 — au choix, l’important est que PatchEditPanel puisse accéder aux 3 instances.

---

## 3. BottomPanel – Accès aux panels ENV

**Fichiers** : [Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/BottomPanel/BottomPanel.h](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/BottomPanel/BottomPanel.h), [Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/BottomPanel/BottomPanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/BottomPanel/BottomPanel.cpp)

- Ajouter **getEnv1Panel()**, **getEnv2Panel()**, **getEnv3Panel()** retournant `Env1Panel*` / `Env2Panel*` / `Env3Panel*` (ou `BaseModulePanel*` si on veut rester générique), de la même façon que `TopPanel::getFmTrackPanel()`.
- Les 5 premiers paramètres d’Env1Panel (indices 0–4) sont Delay, Attack, Decay, Sustain, Release ; idem pour Env2Panel et Env3Panel.

---

## 4. PatchEditPanel – Câblage bidirectionnel Sliders ENV ⇄ EnvelopeDisplay

**Fichiers** : [Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.h](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.h), [Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.cpp)

- **Stockage** : conserver le tableau actuel pour les 5 sliders Track Point ; ajouter un moyen d’identifier les 15 sliders ENV (3 × 5) et le display + paramètre associés. Options : (A) `std::array<std::array<juce::Slider*, 5>, 3> envSliders_` + dans `sliderValueChanged` une double boucle (envIndex, paramIndex) ; (B) un seul tableau de 15 sliders avec un mapping (envIndex, paramIndex) dérivé de l’ordre de remplissage. Option (A) plus lisible.
- **Setup** : dans une nouvelle fonction `setupEnvelopeSliderConnections()` (appelée après création de `bottomPanel_`), pour chaque `envIndex` 0, 1, 2 :
  - Récupérer le panel ENV via `bottomPanel_->getEnv1Panel()` (ou getEnv2, getEnv3).
  - Pour les indices 0–4, récupérer le `ParameterPanel*` via `getParameterPanelAt(i)`, puis le `Slider*` via `getSlider()`, stocker dans `envSliders_[envIndex][paramIndex]` et appeler `slider->addListener(this)`.
  - Câblage **Display → Slider** : `middlePanel_->getEnvelopeXDisplay().setOnValueChanged(...)` avec un callback qui, pour `(paramIndex, newValue)`, appelle `envSliders_[envIndex][paramIndex]->setValue(newValue, juce::sendNotificationSync)`.
- **sliderValueChanged** : après la branche existante pour `trackPointSliders_`, détecter si le slider fait partie d’un `envSliders_[e][p]` ; si oui, appeler `middlePanel_->getEnvelopeXDisplay().setX(value, false)` pour le bon paramètre (Delay/Attack/Decay/Sustain/Release).
- **Destructeur** : retirer les listeners pour les 15 sliders ENV (comme pour les 5 track point sliders).

Constantes : définir `kEnvelopeParamCount_ = 5` et les indices 0–4 pour Delay, Attack, Decay, Sustain, Release dans un seul endroit (header ou .cpp) pour éviter les magic numbers.

---

## 5. Récapitulatif des fichiers


| Fichier                                                                                                 | Modifications                                                                                                                                          |
| ------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------ |
| [EnvelopeDisplay.h](Source/GUI/Widgets/EnvelopeDisplay.h)                                               | Membres (5 valeurs, cache, callback), setters/getters avec `notify`, `setOnValueChanged`, constantes courbe/cache.                                     |
| [EnvelopeDisplay.cpp](Source/GUI/Widgets/EnvelopeDisplay.cpp)                                           | Implémentation courbe DADSR, cache, tous les setters (clamp, invalidateCache, callback si notify), pas de mouse.                                       |
| [MiddlePanel.h](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/MiddlePanel/MiddlePanel.h)     | `getEnvelope1Display()`, `getEnvelope2Display()`, `getEnvelope3Display()` (ou `getEnvelopeDisplayAt(int)`).                                            |
| [MiddlePanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/MiddlePanel/MiddlePanel.cpp) | valueTreePropertyChanged pour env1/2/3 Delay, Attack, Decay, Sustain, Release ; `syncEnvelopeDisplaysFromApvts()` ; helper `getEnvParameterFromApvts`. |
| [BottomPanel.h](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/BottomPanel/BottomPanel.h)     | Déclarations `getEnv1Panel()`, `getEnv2Panel()`, `getEnv3Panel()`.                                                                                     |
| [BottomPanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/BottomPanel/BottomPanel.cpp) | Implémentations des trois getters.                                                                                                                     |
| [PatchEditPanel.h](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.h)           | `std::array<std::array<juce::Slider*, 5>, 3> envSliders_` (ou équivalent), `setupEnvelopeSliderConnections()`.                                         |
| [PatchEditPanel.cpp](Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.cpp)       | `setupEnvelopeSliderConnections()`, extension de `sliderValueChanged`, nettoyage destructeur pour les 15 sliders.                                      |


Aucune modification du PluginProcessor, des descriptors ou de l’APVTS : les attachments restent sur les sliders ENV comme aujourd’hui.

---

## Ordre de mise en œuvre recommandé

1. **EnvelopeDisplay** : constantes, membres, cache, dessin DADSR, setters/getters avec `notify`, `setOnValueChanged`.
2. **MiddlePanel** : sync APVTS → 3 displays dans valueTreePropertyChanged + sync initiale ; exposers des 3 displays.
3. **BottomPanel** : getEnv1Panel(), getEnv2Panel(), getEnv3Panel().
4. **PatchEditPanel** : tableau envSliders_, setupEnvelopeSliderConnections(), extension sliderValueChanged, removeListener dans le destructeur.

---

## Deuxième intention (hors scope de ce plan)

- Édition de la courbe à la souris (glisser un point ou un segment pour modifier Delay/Attack/Decay/Sustain/Release), sur le même principe que TrackGeneratorDisplay (hit test, mouseDrag, callback vers le slider correspondant).

