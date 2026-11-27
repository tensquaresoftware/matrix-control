# Prompt Quick Dev — Dirty / non sauvé — chantier 3

Copier-coller le bloc ci-dessous dans une **conversation Cursor fraîche**, puis lancer `/bmad-quick-dev` (ou coller avec le skill attaché).

**Prérequis :** chantier 2 idéalement déjà sur `main` (Save As device conserve le risque RAM ; dirty d’origine `.syx` → Save / Save As). Si le chantier 2 n’est pas encore mergé, partir de `main` après le 2, ou noter explicitement la dépendance.

---

```
/bmad-quick-dev

## Intention

Implémenter le **chantier 3** de l’UX « Dirty vs non sauvé » pour Matrix-Control : avertir à la **fermeture** du plugin / de la session DAW si le patch courant est encore à risque.

Référence produit (décisions figées) :
`Documentation/Devs/Plans/2026/08/2026-08-03-Dirty-Vs-Unsaved-Patch-UX.md`
— surtout §0 décision 9, §5.4, §8 point 8.

Travaux déjà faits :
- Chantier 1 sur `main` (`spec-dirty-unsaved-patch-ux-chantier-1.md`, commit `75f397d`) : risque = dirty **ou** non-STORED en RAM ; modale de départ Cancel / Discard / Persist (Store ou Save As).
- Chantier 2 (doit être fait, ou traité comme dépendance) : Save As depuis device conserve le risque RAM ; dirty d’origine `.syx` utilise Save / Save As au départ.
- Note reportée liée : `patchNotStoredInRam_` n’est aujourd’hui qu’en mémoire — la fermeture de session est exactement le moment où une dernière chance (ou un drapeau de session) compte (`deferred-work.md`, revue chantier 1).

## Problème

Aujourd’hui, fermer l’éditeur, décharger le plugin ou quitter l’hôte peut faire perdre un travail à risque (édits dirty et/ou INIT / Save As sans STORE) **sans** alerte du type FR-51. L’utilisateur attend une dernière chance cohérente avec les règles de départ de contexte, sans inventer un nouveau langage produit.

## Dans le périmètre

- Détecter l’état à risque sur les chemins de fermeture / déchargement que JUCE expose vraiment dans ce projet (fermeture AudioProcessorEditor, contraintes du destructeur processor, quit standalone, `getStateInformation` / close hôte — choisir les crochets qui peuvent encore montrer une modale sans danger sur le message thread).
- Réutiliser le même prédicat de risque et, autant que les API hôte le permettent, la même famille Cancel / Discard / Persist (Store RAM vs Save / Save As fichier-ROM selon chantiers 1–2).
- Honorer Settings « ne jamais prévenir » (supprime toute cette famille).
- Chaînes en anglais uniquement ; Core ↛ GUI (gate / callback depuis l’éditeur ou le binding UI message-thread du processor).
- Documenter honnêtement les limites hôte : certains DAW ignorent ou ne peuvent pas bloquer le déchargement ; viser le meilleur effort + fiabilité standalone, sans fausse garantie universelle.
- Couverture unitaire ou harness pour le prédicat / la policy à la fermeture ; checklist manuelle pour quit standalone et au moins un chemin AU/VST3 si possible.
- Suivre CONVENTIONS.md / project-context ; lint_touched sur le C++ touché.

## Hors périmètre

- Rouvrir les libellés des modales chantiers 1–2 sauf partage nécessaire pour la fermeture.
- Persister les octets du patch non sauvé dans l’état projet DAW à la place d’un Store/Save (sauf drapeau minimal pour re-prompt après reload — **demander d’abord** si tu juges ce drapeau indispensable).
- Fusionner historique Mutator / IMPORT / reconcil. de nom dans la dialogue de fermeture.
- Garantir que chaque hôte bloquera le quit (impossible) ; ne pas promettre une couverture DAW totale.

## Demander d’abord si ce n’est pas clair

- Sur Cancel à la fermeture : faut-il **bloquer** le quit hôte quand l’API le permet, ou seulement Discard / Persist avec Cancel = rester dans l’UI quand c’est possible ?
- Le drapeau non-STORED doit-il survivre à `getStateInformation` / restore pour re-prompt après reload (reporté du chantier 1) — seulement si la fermeture seule ne suffit pas ?

## Approche suggérée

1. Lire le plan §5.4 / §8 et les specs chantiers 1–2 + le binding gate actuel (`PluginEditorPatchBindings` / `PluginProcessorGates`).
2. Cartographier les vrais points d’entrée close / unload dans ce dépôt (standalone vs plugin).
3. Spec : quels crochets, comportement modale, never-warn, limites hôte.
4. Implémenter le prompt message-thread-safe ; Persist réutilise les chemins Store / Save / Save As existants.
5. Tests + checklist manuelle standalone ; revue ; reporter les impossibilités hôte réelles dans `deferred-work.md`.

## Terminé quand

- Quit standalone avec patch à risque → famille de modale risque (ou équivalent documenté), sauf never-warn.
- Persist / Discard / Cancel se comportent comme spécifié là où l’hôte permet de bloquer.
- Pas de régression des départs de contexte chantiers 1–2.
- Spec sous `_bmad-output/implementation-artifacts/` avec notes honnêtes sur les limites hôte ; tests ciblés verts.
```

---

*Fichier d’aide — coller le bloc entre triples backticks, pas tout le fichier.*
