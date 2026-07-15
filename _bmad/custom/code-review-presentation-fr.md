# Code review — contrat de présentation (Matrix-Control)

Ce document s’applique à **toute** sortie utilisateur de `bmad-code-review`, Bugbot et revues adversariales explicites.
Il prime sur le style télégraphique des sous-agents et des étapes internes du workflow.

## Langue

- **Toujours répondre en français** à Guillaume — titres, résumés, constats, options, recommandations.
- Ne jamais coller tel quel un output sous-agent en anglais, ni des noms de champs JSON (`decision-needed`, `patch`, etc.) sans les traduire.
- Les chemins `fichier:ligne` et identifiants de code peuvent rester en anglais ; le reste du texte est en français.

## Ton et clarté

- **Phrases complètes**, langage naturel — pas de style télégraphique ni de listes de mots-clés.
- **Pas de jargon excessif** : si un terme technique est indispensable, l’expliquer brièvement à la première occurrence (ex. « APVTS = stockage des paramètres du plugin »).
- **Pas de noms de couches internes** dans le texte utilisateur — utiliser les libellés français du tableau ci-dessous.

| Interne | Dire à Guillaume |
|---------|------------------|
| `decision-needed` | décision à prendre |
| `patch` | correction à appliquer |
| `defer` | à reporter |
| `dismiss` | sans action |
| Blind Hunter | analyse du diff |
| Edge Case Hunter | cas limites |
| Acceptance Auditor | conformité à la spec |

## Structure obligatoire (avant le détail)

### 1. En bref

Une phrase : ce qui est revu, pourquoi c’est important pour le produit ou la story.

### 2. Verdict

Compter et annoncer clairement :

- **Décisions à prendre** — choix d’intention ou d’architecture ; Guillaume doit trancher
- **Corrections à appliquer** — bug ou lacune avec correctif évident
- **À reporter** — hors périmètre ou dette préexistante

## Format de chaque constat

Pour **chaque** constat présenté à Guillaume :

1. **Ce qui est en jeu** — impact utilisateur ou risque concret
2. **Le problème** — description claire (fichier/ligne si utile)
3. **Ce qu’il faut faire** — action directe, ou options numérotées avec avantages/inconvénients en langage simple
4. **Si on ne fait rien** — conséquence de l’inaction

## Décisions à prendre — Recommandation BMad **obligatoire**

Pour **chaque** décision à prendre, ajouter **toujours** une section :

### Recommandation BMad

- **Option recommandée** — numéro et intitulé court (ex. « Option 2 — laisser tel quel pour cette story »)
- **Pourquoi** — 2 à 4 phrases accessibles, sans jargon
- **Confiance** — `ferme` (choix nettement préférable) ou `nuancée` (plusieurs options défendables ; expliquer le compromis)

**Règle :** ne jamais présenter une décision à prendre sans cette section. Guillaume ne doit pas avoir à redemander « quelle est ta recommandation ? ».

Même si plusieurs options sont valables, la recommandation BMad doit nommer celle qui :

1. minimise le risque pour l’utilisateur ou la stabilité du code ;
2. respecte le mieux `project-context.md` et les conventions du dépôt ;
3. correspond au périmètre de la story en cours.

## Sous-agents et triage interne

Les sous-agents peuvent produire de l’anglais dense ou des structures JSON. **Toujours reformuler** en français clair avant présentation. Le tableau récapitulatif peut conserver `fichier:ligne` dans la colonne Emplacement.

## Étape « Resolve decision-needed » (workflow)

Lors du walkthrough des décisions (step 04), présenter chaque décision **avec sa Recommandation BMad déjà incluse**, puis les options numérotées. Attendre le choix de Guillaume avant de continuer.
