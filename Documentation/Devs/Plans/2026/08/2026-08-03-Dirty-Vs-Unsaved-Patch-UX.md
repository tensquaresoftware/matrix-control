# Dirty vs non sauvé — bilan UX pour Matrix-Control

**Date :** 2026-08-03  
**Statut :** décisions produit figées (relecture GD + réponses aux questions)  
**Objectif :** Clarifier deux idées souvent confondues, lister les scénarios utiles, et figer ce que l’interface doit faire — notamment INIT puis changement de patch.

---

## 0. Décisions figées (synthèse)

Ces points ne sont plus ouverts :

1. **Dirty classique** (édition / renommage / INIT module / Mutator) → en quittant : Cancel / Discard / Store en RAM ; Cancel / Discard / Save As en ROM.
2. **INIT patch entier non stocké** = à risque (lecture B), même sans knob tourné → même famille de modale en changeant de patch ou en chargeant un `.syx`.
3. **PASTE** = **sans** avertissement à l’entrée (comme C / P des modules). L’alerte vient seulement si on **quitte** ensuite sans avoir figé le contenu collé.
4. **Second INIT** (INIT alors que le courant est déjà à risque) = **avec** avertissement Cancel / Discard / Store.
5. **SAVE AS** depuis une origine device ne « lave » pas le risque RAM : même règle à risque qu’un INIT non stocké tant que Store n’a pas été fait.
6. **Origine `.syx` dirty** → Cancel / Discard / Save (écraser le fichier connu) ou Save As si pas de cible claire. Chantier 2.
7. **Settings « ne jamais prévenir »** coupe **tout** : dirty et non persisté.
8. **IMPORT** : d’abord dirty / non persisté, puis confirm IMPORT. **EXPORT** : Cancel / Discard (Store optionnel).
9. **Fermeture plugin / DAW** : reportée (chantier 3).
10. Ordre Mutator inchangé : d’abord risque de perdre le patch, puis modale historique.

---

## 1. Deux idées distinctes

### Patch « dirty » (édité depuis la dernière référence)

Le plugin garde une photo de référence du patch. Dès qu’un réglage ou le nom change par rapport à cette photo, le patch est *dirty*.

Cette photo est aujourd’hui rafraîchie après un chargement réussi (synthétique ou fichier), un STORE, un SAVE / SAVE AS, un INIT, au démarrage / restore de session, et en fin d’IMPORT de banque.

Point important : *dirty* parle d’une différence avec la photo du plugin. Ce n’est pas, en soi, la preuve que le slot RAM du synthé ou un fichier `.syx` est à jour.

### Patch « non encore sauvé » / à risque de perte

La question utilisateur est : « Si je m’en vais maintenant, est-ce que mon travail est figé quelque part ? »

Deux destinations possibles :

- **Mémoire RAM du synthé** (emplacement courant) → il faut un **STORE**.
- **Fichier `.syx` sur l’ordinateur** → il faut un **SAVE** ou **SAVE AS**.

Sur Matrix-1000, l’édition « live » passe surtout par le tampon d’édition (audition). Tant qu’on n’a pas fait STORE, le slot RAM peut encore contenir l’ancien patch. L’interface doit guider au bon moment, sans obliger l’utilisateur à connaître le jargon hardware.

Décision produit : outre le dirty, on traite aussi comme **à risque** les cas « contenu courant non figé en RAM » même quand la photo plugin vient d’être rafraîchie (INIT non stocké ; après SAVE AS depuis le device sans Store ; et le départ après PASTE non stocké, déjà couvert par dirty aujourd’hui).

### Pourquoi la confusion était normale

INIT charge un patch neuf dans l’éditeur / le tampon, affiche un nom (`INIT`), et aujourd’hui le code prend aussitôt une nouvelle photo → le patch n’est **pas** dirty. Pourtant il n’est **pas** sauvé en RAM. Les deux réponses étaient vraies selon la question posée. La lecture B tranche : quitter un INIT non stocké doit être aussi sérieux que quitter un patch édité.

---

## 2. Ce que fait le code aujourd’hui (rappel)

Quand le patch est dirty et que Settings demande d’avertir, une modale **Cancel / Continue** apparaît avant de quitter le contexte. Continue = abandonner les edits et poursuivre. Pas de Store. Pas d’alerte après INIT sans édition. PASTE et nouvel INIT contournent l’avertissement dirty. SAVE / SAVE AS remettent « clean » même sans STORE RAM.

---

## 3. Scénarios dirty — cible UX

### 3.1 Édition de paramètres après une référence stable

- **Aujourd’hui :** dirty → Cancel / Continue.
- **Cible :** Cancel / Discard / Store (RAM). Sur ROM : Cancel / Discard / Save As.  
  📌 **GD :** validé.

### 3.2 Renommage du Patch Name

- **Cible :** même modale que 3.1.  
  📌 **GD :** validé.

### 3.3 INIT d’un seul module

- **Cible :** édition normale (dirty). Pas de cas spécial « déjà stocké ».  
  📌 **GD :** validé.

### 3.4 MUTATE / RETRY / historique Mutator

- **Cible :** d’abord modale risque patch, puis modale historique si besoin. Pas de fusion.  
  📌 **GD :** validé.

### 3.5 PASTE d’un patch entier

- **Aujourd’hui :** pas d’alerte avant PASTE ; après PASTE le buffer est en pratique dirty.
- **Cible figée :**
  - **À l’entrée :** PASTE **sans** avertissement (même friction que C / P des modules), y compris si le contenu précédent était un INIT non stocké ou dirty.
  - **En partant ensuite** sans Store / Save : alerte (voir 4.4).  
  📌 **GD :** coller sans demander ; prévenir seulement si le collé risque d’être perdu. Confirmé en décision 1-1.

### 3.6 Édition sur banque ROM

- **Cible :** Cancel / Discard / Save As (pas de Store RAM).  
  📌 **GD :** validé.

---

## 4. Scénarios « non encore sauvé en RAM » — cible UX

### 4.1 INIT patch entier, puis changement de patch sans édition

- **Aujourd’hui :** silence (pas dirty).
- **Cible :** lecture **B** — Cancel / Discard / Store.  
  📌 **GD :** B validé.

### 4.2 INIT puis chargement d’un `.syx` sans édition

- **Cible :** même règle B.  
  📌 **GD :** B validé.

### 4.3 INIT puis PASTE, ou INIT puis second INIT

- **PASTE :** pas d’alerte à l’entrée (3.5). Le risque INIT est remplacé par le contenu collé ; l’alerte reviendra au prochain départ si non stocké.
- **Second INIT :** **avec** alerte Cancel / Discard / Store si le courant est à risque.  
  📌 **GD :** B pour le risque INIT ; PASTE tranché à part (1-1) ; second INIT = demander (2-1).

### 4.4 PASTE réussi, pas encore de STORE

- **Cible :** en quittant → Cancel / Discard / Store (RAM).  
  📌 **GD :** validé.

### 4.5 Édition dirty, puis SAVE AS `.syx`, sans STORE

- **Aujourd’hui :** SAVE remet clean → silence en changeant de patch.
- **Cible :** piste **1** — après SAVE AS depuis une origine device, garder le signal « pas encore en RAM » ; quitter le slot déclenche la **même** modale à risque que B (Cancel / Discard / Store).  
  📌 **GD :** piste 1 validée ; décision 4-1 = même règle à risque.

### 4.6 Édition dirty, puis STORE

- **Cible :** clean pour la RAM. Pas d’obligation `.syx`.  
  📌 **GD :** validé.

---

## 5. Scénarios « non encore sauvé en `.syx` » — cible UX

### 5.1 Patch depuis le synthé, édité, jamais sauvé fichier

- **Cible :** en navigation interne, prioriser Store dans la modale. SAVE / SAVE AS restent dans Computer Patches (pas de quatrième bouton).  
  📌 **GD :** validé.

### 5.2 Patch depuis un `.syx`, édité, pas de SAVE

- **Cible :** Cancel / Discard / Save (écraser le fichier d’origine s’il est connu) ; sinon Save As. Store possible en plus seulement si on décide plus tard qu’il faut aussi figer la RAM dans le même départ — pour le chantier 2, l’intention fichier prime.  
  📌 **GD :** Cancel / Discard / Save (ou Save As) validé ; détail Save vs Save As = décision 3-1.

### 5.3 INIT puis SAVE AS, sans STORE

- **Cible :** comme 4.5 (risque RAM conservé).  
  📌 **GD :** piste 1.

### 5.4 Fermeture plugin / projet DAW

- **Cible :** reportée (chantier 3).  
  📌 **GD :** report OK.

---

## 6. Actions de départ — règles figées

### Navigation interne (précédent / suivant, numéro, banque)

Déclencher si dirty **ou** non figé en RAM (INIT non stocké, SAVE AS device sans Store, etc.).  
Modale : Cancel / Discard / Store (Store seulement si RAM autorisée). Sur ROM dirty : Cancel / Discard / Save As.

### Chargement Computer Patches

Même déclenchement. Boutons selon l’origine : Store si RAM pertinente ; en chantier 2, Save / Save As si origine fichier dirty.

### PASTE

Pas de porte à l’entrée. Le départ ultérieur suit dirty / non persisté.

### Nouvel INIT (second INIT)

Porte active si le courant est à risque → Cancel / Discard / Store.

### IMPORT de banque

D’abord dirty / non persisté, puis confirm IMPORT. Textes séparés.  
📌 **GD :** 6-1 gardé.

### EXPORT de banque

Si à risque : Cancel / Discard (Store optionnel).  
📌 **GD :** 6-1 gardé.

### Policy Settings « ne jamais prévenir »

Coupe **tous** les dialogues dirty et non persisté.  
📌 **GD :** 5-1.

---

## 7. INIT puis autre patch — décision

**Lecture B retenue.** Quitter un patch initialisé mais jamais stocké est aussi sérieux que quitter un patch édité. Cancel / Discard / Store.

Lecture A et piste médiane : écartées pour la cible produit actuelle.

---

## 8. Ordre de chantier (validé)

**Premier chantier** — **livré** (2026-08-06, `spec-dirty-unsaved-patch-ux-chantier-1`)

1. Clarifier au moment utile que Store fige en RAM.
2. Enrichir la modale : Cancel / Discard / Store (RAM) ; Cancel / Discard / Save As (ROM).
3. Traiter INIT non stocké comme à risque (B) sur nav interne et load `.syx`.
4. Second INIT : alerte si courant à risque.
5. PASTE : rester silencieux à l’entrée ; alerte au départ si non figé.

**Deuxième chantier** — à faire

6. SAVE AS depuis device : ne plus « laver » le risque RAM (même règle B).
7. Origine `.syx` dirty : Cancel / Discard / Save (ou Save As).

**Troisième chantier** — à faire

8. Avertissement à la fermeture de session / DAW.

📌 **GD :** OK pour l’ordre de recommandations ; précisions 1–6 des questions intégrées ci-dessus.

---

## 9. Ne pas mélanger dans la même modale

- Historique Mutator (Discard / Export).
- Reconciliation de nom au chargement `.syx`.
- Confirm IMPORT de banque.
- Confirm collision de dossier d’export Mutator.
- Master INIT.

Ces dialogues restent séparés. Ils peuvent s’afficher après, pas à la place.

---

## 10. Prêt pour la suite

Le document n’est plus un simple brainstorm : les choix produit principaux sont figés.

Prochaine étape naturelle : convertir le **premier chantier** en spec Quick Dev (modale enrichie + INIT à risque + second INIT + PASTE silencieux à l’entrée), puis chantier 2 (SAVE AS vs RAM + Save depuis `.syx`).

---

*Bilan de travail — prêt à dériver une spec d’implémentation pour le chantier 1.*
