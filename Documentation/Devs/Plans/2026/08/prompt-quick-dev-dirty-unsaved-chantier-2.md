# Prompt Quick Dev — Dirty / non sauvé — chantier 2

Copier-coller le bloc ci-dessous dans une **conversation Cursor fraîche**, puis lancer `/bmad-quick-dev` (ou coller avec le skill attaché).

---

```
/bmad-quick-dev

## Intention

Implémenter le **chantier 2** de l’UX « Dirty vs non sauvé » pour Matrix-Control.

Référence produit (décisions figées) :
`Documentation/Devs/Plans/2026/08/2026-08-03-Dirty-Vs-Unsaved-Patch-UX.md`
— surtout §0 décisions 5–6, §4.5, §5.2–5.3, §6 (chargement Computer Patches), §8 points 6–7.

Base déjà sur `main` (chantier 1 livré) :
`_bmad-output/implementation-artifacts/spec-dirty-unsaved-patch-ux-chantier-1.md`
Commit : `75f397d` — en quittant un patch : Cancel / Discard / Store (RAM) ou Save As (ROM) ; risque = dirty **ou** non encore STORED en RAM ; un INIT marque non-STORED ; PASTE reste silencieux à l’entrée.

## Problème

Deux manques restent après le chantier 1 :

1. **SAVE AS depuis un patch d’origine device** rafraîchit encore la photo « clean » et efface en pratique le signal « pas encore STORED en RAM ». On peut donc quitter le slot après un Save As **sans** alerte, alors que le slot RAM n’a jamais reçu de STORE. Décision produit : garder la **même** règle à risque que après un INIT (lecture B) jusqu’au STORE.

2. **Patch dirty venu d’un fichier `.syx`** : en partant, proposer Cancel / Discard / **Save** (écraser le fichier connu) ou **Save As** s’il n’y a pas de cible claire — pas seulement Store / Save As comme pour device / ROM. Pour le chantier 2, l’intention **fichier** prime ; ne pas ajouter un quatrième bouton « Store RAM en plus » dans la même modale (sauf décision produit ultérieure).

## Dans le périmètre

- Après un SAVE / SAVE AS réussi depuis un contexte **origine device** (ou INIT / pas encore figé en RAM) : garder `patchNotStoredInRam_` (ou équivalent) à vrai, pour qu’un départ suivant déclenche encore la famille de modale du chantier 1 (RAM : Cancel / Discard / Store).
- Suivre l’**origine fichier** (chemin `.syx` connu vs aucun) pour que le Persist d’un dirty d’origine fichier lance Save ou Save As selon le cas.
- Brancher libellés / boutons de modale pour dirty d’origine fichier : Cancel / Discard / Save (ou Save As).
- Respecter Settings « ne jamais prévenir » (coupe toute la famille).
- Garder la modale historique Mutator **après** la modale risque ; ne pas fusionner avec reconcil. de nom / IMPORT / Defrag / Master INIT.
- Tests unitaires : Save As depuis device conserve non-STORED ; Persist Save pour origine fichier ; never-warn reste silencieux ; Cancel abort inchangé.
- Chaînes UI en anglais uniquement. Core ne doit pas inclure AlertWindow / types de dialogue.
- Suivre CONVENTIONS.md / project-context ; lancer lint_touched sur le C++ touché et les tests liés.

## Hors périmètre

- Chantier 3 (alerte à la fermeture plugin / DAW / session).
- Modifier le silence PASTE à l’entrée ou le comportement INIT non-STORED du chantier 1 (réutiliser ; ne pas régresser).
- Fusionner des modales sans rapport ; textes UI en français ; inventer une modale à 4 boutons Store+Save.
- Gros refactors hors suivi d’origine / chemins Persist.

## Approche suggérée

1. Lire le plan §0 / §4.5 / §5.2 / §8 et la spec + Code Map du chantier 1.
2. Tracer les sites actuels SAVE / SAVE AS / `captureCleanSnapshot` / clear de `patchNotStoredInRam_`.
3. Spec puis implémentation : origine + sémantique des boutons Persist ; conserver le risque après Save As device ; Save / Save As pour origine fichier.
4. Étendre les tests gate / policy / handler ; vérifier build + tests ciblés.
5. Revue quick-dev ; n’ajouter à `deferred-work.md` que les vrais reports.

## Terminé quand

- Save As depuis device, puis départ sans STORE → modale à risque (Store si RAM autorisée).
- Départ d’un dirty d’origine `.syx` → Cancel / Discard / Save (chemin connu) ou Save As (pas de cible claire).
- Never-warn toujours silencieux ; ordre Mutator inchangé ; comportements INIT / PASTE du chantier 1 préservés.
- Artefact de spec sous `_bmad-output/implementation-artifacts/` prêt pour revue ; tests verts.
```

---

*Fichier d’aide — coller le bloc entre triples backticks, pas tout le fichier.*
