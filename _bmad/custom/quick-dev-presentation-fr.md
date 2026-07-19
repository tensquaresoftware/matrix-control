# Quick Dev — contrat de présentation (Matrix-Control)

Ce document s’applique à **toute** sortie utilisateur de `bmad-quick-dev` (clarification, plan, implémentation, **revue interne**, présentation finale).
Il prime sur le style télégraphique des étapes internes et des sous-agents.

**Barème partagé (obligatoire) :** charger et appliquer `clarity-bar-fr.md` en premier. Rien ici n’assouplit ce barème.

**Rappel phrase-test :** avant chaque message important, si Guillaume dirait « Reformule en langage naturel, sans jargon excessif », **réécrire**.

---

## Langue

- **Toujours répondre en français** à Guillaume — questions, résumés, options, recommandations, bilans, **constats de revue**.
- Ne jamais coller tel quel un output sous-agent ou une note interne en anglais.
- **Distinction importante :**
  - Chat avec Guillaume → français + barème de clarté
  - Artifacts générés (spec, code map, etc.) → anglais (`document_output_language`), sauf si Guillaume demande autrement
- Les chemins `fichier:ligne` et identifiants de code : **ligne complémentaire**, pas le cœur du message.

## Ton et clarté

- **Phrases complètes**, langage naturel — pas de style télégraphique ni de listes de mots-clés.
- **Pas de jargon excessif** : si un terme technique est indispensable, l’expliquer brièvement à la première occurrence (ex. « APVTS = stockage des paramètres du plugin »).
- **Pas de jargon de workflow BMad** sans traduction — utiliser le tableau ci-dessous.

| Interne | Dire à Guillaume |
|---------|------------------|
| `draft` | brouillon |
| `ready-for-dev` | prêt à développer |
| `in-progress` | en cours |
| `in-review` | prêt pour revue |
| `oneshot` | passage direct (sans grosse spec) |
| `spec` | cahier des charges / spec |
| Acceptance criteria (AC) | critères d’acceptance |
| Code Map | carte du code |
| Suggested Review Order | ordre de relecture suggéré |
| `decision-needed` / `patch` / `defer` | décision à prendre / correction à appliquer / à reporter |

---

## Structure des messages importants

### Clarification / choix de route

1. **En bref** — une phrase : ce que tu as compris de l’intention.
2. **Ce qu’il faut trancher** — questions numérotées, en français simple.
3. Pour chaque choix à options : **Recommandation BMad** (voir ci-dessous).

### Plan / avancement

1. **En bref** — ce qui va être fait (ou ce qui vient d’être fait).
2. **Détail** — liste courte en phrases complètes, pas de jargon opaque.
3. Si un choix apparaît : options + **Recommandation BMad**.

### Revue interne (phase critique — même exigence que code-review)

La revue intégrée à Quick Dev **n’a pas le droit** d’être plus dense que `bmad-code-review`.

1. **En bref** — enjeu principal en langage de produit.
2. **Verdict** — comptes : décisions à prendre / corrections à appliquer / à reporter.
3. **Chaque constat** dans cet ordre :
   - Titre = question claire (décision) ou action/risque simple (correction)
   - **Ce qui est en jeu**
   - **Le problème** (intention d’abord, écarts ensuite ; preuves techniques en bas)
   - **Ce qu’il faut faire** (verbes du quotidien)
   - **Si on ne fait rien**
   - **Recommandation BMad** si décision
4. Appliquer la checklist complète de `clarity-bar-fr.md` **avant** d’envoyer le premier constat.

### Bilan final (présentation)

1. **En bref** — résultat livré, en une phrase.
2. **Ce qui a changé** — points principaux en langage accessible.
3. **Où regarder** — chemins / ordre de relecture reformulé en français (les liens `fichier:ligne` peuvent rester tels quels).
4. **Suite possible** — prochaines actions proposées, avec recommandation si plusieurs options.

---

## Décisions à prendre — Recommandation BMad **obligatoire**

Dès que Guillaume doit choisir (périmètre, approche technique, reprise d’une spec, branche git, traiter ou reporter un point de revue, etc.), ajouter **toujours** :

### Recommandation BMad

- **Option recommandée** — numéro et intitulé court
- **Pourquoi** — 2 à 4 phrases accessibles (phrase-test)
- **Confiance** — `ferme` ou `nuancée`

**Règle :** ne jamais poser une décision à options sans cette section. Guillaume ne doit pas avoir à redemander « quelle est ta recommandation ? ».

Critères pour choisir l’option recommandée :

1. Minimise le risque pour l’utilisateur ou la stabilité du code
2. Respecte `project-context.md` et les conventions du dépôt
3. Correspond au périmètre demandé (pas de sur-ingénierie)

---

## Sous-agents et notes internes

Les sous-agents et les fichiers d’étapes peuvent produire de l’anglais dense. **Toujours reformuler** au niveau phrase-test avant de s’adresser à Guillaume — surtout pour la revue.
