# Getting Started — décisions produit (2026-10-08)

Document de décisions avant Correct Course.  
Source : discussion BMad Help sur Device Setup / premier démarrage / capitalisation Settings.

Langue UI du produit : **anglais** (ci-dessous). Ce document est en **français**.

---

## 1. Contexte et problème

- Device Setup actuel couvre surtout MIDI + EPROM ; le chantier « First-run setup assistant » élargi était en attente.
- Settings est devenu beaucoup plus complet (audio + MIDI unifiés).
- En Standalone, l’utilisateur doit pouvoir : éditer en MIDI bidirectionnel, jouer (clavier maître si besoin), **et entendre** le synthé — sinon le reste n’a pas d’intérêt.
- En plugin (VST3 / AU), l’audio OS est en général déjà géré par le DAW ; il ne faut pas refaire le même parcours.
- Le UI Scale en premier n’est pas cosmétique : selon l’écran / HiDPI, une modale peut être illisible ou tronquée.

### Approches écartées

| Approche | Motif |
|----------|--------|
| Pastilles numérotées rouge/vert | Trop sophistiqué pour ce produit |
| Ouverture auto de Settings au premier lancement (seule) | Fragile (échelle UI) ; incomplet |
| Manuel utilisateur uniquement | Trop faible pour le chemin critique |
| Méga-modale unique « tout Settings » | Trop haute, duplique Settings, mauvais véhicule (déjà noté en deferred) |
| Device Setup MIDI seul pour Standalone | Insuffisant (pas d’écoute, clavier optionnel mal traité) |

---

## 2. Direction produit figée

**Chantier dédié : assistant multi-étapes `GETTING STARTED`** (Previous / Next), qui absorbe / remplace le one-shot Device Setup actuel.

Principes :

1. **Débloquer l’usage** selon le format (plugin vs standalone), pas « tout configurer comme un pro ».
2. **Peu de contrôles par étape** + 1–2 phrases d’aide en tête de corps.
3. **Titres d’étape dans la barre de titre** (pas dans le corps).
4. **Réutiliser** les mêmes briques / la même logique que Settings (et header) — **pas** une deuxième copie des listes MIDI / audio.
5. **UI Scale** et **Skin** aussi dans Settings → onglet User Interface (ordre ci-dessous), **raccourcis conservés** dans le menu du logo.
6. **Flags par étape** (fait / pas fait / sauté) ; ouverture auto seulement s’il reste des étapes **applicables** non faites ; reprise sur la première concernée.
7. Entrée Settings pour relancer ou couper l’ouverture auto (§3).

---

## 3. Settings — User Interface

Ordre des options :

1. UI SCALE  
2. SKIN  
3. INFO MESSAGE  
4. CONTEXTUAL HELP  
5. **GETTING STARTED**

Menu logo : UI SCALE / SKIN restent disponibles en raccourci.

### Bloc GETTING STARTED

- Label de rangée : `GETTING STARTED`
- **Combo** (préférence d’ouverture auto), puis **en dessous** le bouton `RUN SETUP AGAIN` (pas de second label à droite du bouton).

| Valeur combo | Sens |
|--------------|------|
| `SHOW WHEN INCOMPLETE` | Défaut. Ouverture auto s’il reste une étape applicable non faite |
| `NEVER AT LAUNCH` | Jamais d’ouverture auto ; uniquement Settings / `RUN SETUP AGAIN` |

(`NEVER AT LAUNCH` plutôt que `DO NOT SHOW AUTOMATICALLY` — trop long dans une combo.)

#### `RUN SETUP AGAIN` (un seul bouton — pas de « full » séparé)

- Remet à **non fait** les flags des étapes **applicables** au format courant.
- Ouvre le wizard à l’**étape 0**.
- L’ouverture auto au lancement, elle, ne fait que la **reprise** (première étape applicable non faite, sans reset). Pas besoin d’un second bouton.

---

## 4. Parcours du wizard

| ID | Titre barre | Contenu | Applicabilité |
|----|-------------|---------|---------------|
| 0 | `GETTING STARTED` | Intro — **pas** de contrôles | Tous — surtout au tout premier contact |
| 1 | `GETTING STARTED — STEP 1 : USER INTERFACE` | UI Scale, Skin | Tous |
| 2 | `GETTING STARTED — STEP 2 : SYNTH COMMUNICATION` | Synth From, Synth To, DEVICE, EPROM (+ suggestion firmware si pertinent) | Tous |
| 3 | `GETTING STARTED — STEP 3 : MIDI KEYBOARD` | Voir §4.1 | Tous (contenu Standalone ≠ plugin) |
| 4 | `GETTING STARTED — STEP 4 : AUDIO` | Voir §4.2 | **Standalone seulement** |

### 4.1 STEP 3 — MIDI KEYBOARD

`KEYBOARD FROM` est **Standalone only** ; en plugin = `HOST` (notes depuis le DAW).

| Format | Contenu |
|--------|---------|
| **Standalone** | Combo Keyboard From + Skip + copy §6 |
| **Plugin** | Pas de combo — étape **informative** : clavier maître via piste / entrée MIDI du DAW ; renvoi au manuel. Previous / Next (ou Finish). Next / Finish → flag « fait ». Pas de Skip. |

### 4.2 STEP 4 — AUDIO

Afficher **le maximum digeste** ; si la page s’étouffe, retirer d’abord sample rate + buffer (moins vitaux pour démarrer).

Ordre de garde :

1. Driver / type (comme Settings Audio, si place)  
2. Entrée (+ sortie si place)  
3. SYNTH FROM (canaux d’écoute)  
4. Sample rate + buffer si le layout le permet  

Sinon : FE / buffer restent dans Settings une fois qu’un signal est audible.

### 4.3 Boutons

Pas de `QUIT` (ambigu avec quitter l’app). Dernière étape applicable : `FINISH` (pas `COMPLETE` — trop « statut », moins action de fermeture).

| Étape | Boutons | Notes |
|-------|---------|--------|
| **0** Intro | `CONFIGURE LATER` · `CONTINUE` | Voir §5 |
| **1** User Interface | `PREVIOUS` · `NEXT` | Previous → intro |
| **2** Synth Communication | `PREVIOUS` · `NEXT` | Combos **live** (comme Settings) ; pas de Confirm dédié |
| **3** Keyboard Standalone | `PREVIOUS` · `SKIP` · `NEXT` | Skip = étape terminée sans port obligatoire |
| **3** Keyboard plugin | `PREVIOUS` · `NEXT` ou `FINISH` | Pas de Skip |
| **4** Audio | `PREVIOUS` · `FINISH` | Dernière étape Standalone |

Dernière étape : plugin = STEP 3 → `FINISH` ; Standalone = STEP 4 → `FINISH`.

Ancien `SPECIFY LATER` Device Setup : **retiré** (`CONFIGURE LATER` + Skip clavier + Settings).

---

## 5. Flags et reprise

- Un flag par étape : User Interface, Synth Communication, MIDI Keyboard, Audio.
- Skip clavier (Standalone) = étape **terminée** pour l’auto-ouverture.
- Plugin STEP 3 : Next / Finish → flag MIDI Keyboard **fait**.
- Ouverture auto si combo = `SHOW WHEN INCOMPLETE` et il reste une étape applicable non faite → première non faite (**sans** rejouer l’intro sauf premier contact / jamais Continue).
- Plugin d’abord : 1–3 peuvent être faits ; Audio non applicable → pas de boucle plugin.
- Premier Standalone suivant : Audio applicable + non fait → ouverture **ciblée** STEP 4 (copy reprise §6).
- Tout l’applicable fait → plus d’auto.
- `RUN SETUP AGAIN` : reset flags applicables + étape 0 (§3).

### Après `CONFIGURE LATER` (étape 0)

1. Fermer la modale.  
2. Ne pas marquer les étapes non visitées comme faites.  
3. **Un seul** rappel auto au prochain lancement (même format) s’il reste du non fait applicable.  
4. Second Configure later (ou équivalent) → silence jusqu’à `RUN SETUP AGAIN` ou passage combo → `SHOW WHEN INCOMPLETE`.  
5. **Exception** : nouvel applicable (ex. premier Standalone, seul Audio manque) → **réarme** une ouverture ciblée (STEP 4).

---

## 6. Copy UI (anglais)

### Étape 0 — `GETTING STARTED`

> Welcome to Matrix-Control, a modern SysEx editor for the Oberheim Matrix-1000, 6, and 6R synthesizers.  
> We'll set appearance, MIDI connection, optional keyboard input, and audio monitoring (standalone application only) so you can edit, play, and hear your synth. Continue, or choose Configure later and finish in Settings.

### STEP 1 — USER INTERFACE

> Start with UI scale and skin so the next steps stay readable on your screen. You can change these anytime from the logo menu or Settings.

### STEP 2 — SYNTH COMMUNICATION

> Select the MIDI ports wired to your synth and the EPROM type installed in it. Wait until the device is recognized when possible — this unlocks reliable editing and timing.

Suffixe firmware (si pertinent) :

> A suggestion is preselected from the reported firmware version when possible.

### STEP 3 — MIDI KEYBOARD

**Standalone :**

> If you use a separate MIDI keyboard, choose it here. Matrix-6 owners who play the built-in keys can skip this step.

**Plugin :**

> When Matrix-Control runs as a plugin, MIDI notes come from the host. Route your master keyboard on a DAW track (or MIDI input) to Matrix-Control — not in this window. See the user manual for host examples.

### STEP 4 — AUDIO

Premier passage Standalone :

> Choose the audio interface and input so you can hear your synth in Matrix-Control. Pick the SYNTH FROM channel(s) that carry the synth output.

Reprise (ex. après plugin) :

> MIDI is already set. One more step: route audio so the standalone application can monitor your synth.

---

## 7. Hors scope V1

- Pastilles / coach marks  
- Auto-open Settings « nu » au cold start  
- Méga-modale unique  
- Forcer FE / buffer dans STEP 4 si le layout étouffe  
- Second bouton « full setup »  
- Harcèlement après Configure later  
- Exemples DAW détaillés **dans** le wizard (manuel seulement)

---

## 8. Suite processus

1. **Correct Course** — aligner sprint / deferred « First-run setup assistant » / stories.  
2. **Spec** et/ou **Build** GETTING STARTED (+ Settings UI Scale / Skin / bloc GETTING STARTED).  
3. **Manuel utilisateur** — premier démarrage + clavier maître / DAW (STEP 3 plugin).  
4. Ne pas lancer l’ancienne revue Device Setup élargi tant que ce périmètre n’est pas celui implémenté.

---

## 9. Statut

**Rien d’ouvert côté produit pour Correct Course.**  
Détails de layout (largeur combo, hauteur STEP 4, drop exact FE/buffer) = décisions d’implémentation en Spec / Build, pas des arbitrages produit restants.
