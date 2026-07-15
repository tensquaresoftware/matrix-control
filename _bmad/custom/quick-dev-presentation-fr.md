# Quick Dev — contrat de présentation (Matrix-Control)

Ce document s’applique à **toute** sortie utilisateur de `bmad-quick-dev` (clarification, plan, implémentation, revue interne, présentation finale).
Il prime sur le style télégraphique des étapes internes et des sous-agents.

## Langue

- **Toujours répondre en français** à Guillaume — questions, résumés, options, recommandations, bilans.
- Ne jamais coller tel quel un output sous-agent ou une note interne en anglais.
- **Distinction importante :**
  - Chat avec Guillaume → français
  - Artifacts générés (spec, code map, etc.) → anglais (`document_output_language`), sauf si Guillaume demande autrement
- Les chemins `fichier:ligne` et identifiants de code peuvent rester en anglais ; le reste du texte de chat est en français.

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

## Structure des messages importants

### Clarification / choix de route

1. **En bref** — une phrase : ce que tu as compris de l’intention.
2. **Ce qu’il faut trancher** — questions numérotées, en français simple.
3. Pour chaque choix à options : **Recommandation BMad** (voir ci-dessous).

### Plan / avancement

1. **En bref** — ce qui va être fait (ou ce qui vient d’être fait).
2. **Détail** — liste courte en phrases complètes, pas de jargon opaque.
3. Si un choix apparaît : options + **Recommandation BMad**.

### Bilan final (présentation)

1. **En bref** — résultat livré, en une phrase.
2. **Ce qui a changé** — points principaux en langage accessible.
3. **Où regarder** — chemins / ordre de relecture reformulé en français (les liens `fichier:ligne` peuvent rester tels quels).
4. **Suite possible** — prochaines actions proposées, avec recommandation si plusieurs options.

## Décisions à prendre — Recommandation BMad **obligatoire**

Dès que Guillaume doit choisir (périmètre, approche technique, reprise d’une spec, branche git, traiter ou reporter un point, etc.), ajouter **toujours** :

### Recommandation BMad

- **Option recommandée** — numéro et intitulé court
- **Pourquoi** — 2 à 4 phrases accessibles
- **Confiance** — `ferme` ou `nuancée`

**Règle :** ne jamais poser une décision à options sans cette section. Guillaume ne doit pas avoir à redemander « quelle est ta recommandation ? ».

Critères pour choisir l’option recommandée :

1. Minimise le risque pour l’utilisateur ou la stabilité du code
2. Respecte `project-context.md` et les conventions du dépôt
3. Correspond au périmètre demandé (pas de sur-ingénierie)

## Sous-agents et notes internes

Les sous-agents et les fichiers d’étapes peuvent produire de l’anglais dense. **Toujours reformuler** en français clair avant de s’adresser à Guillaume.
