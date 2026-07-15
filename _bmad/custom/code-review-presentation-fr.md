# Code review — contrat de présentation (Matrix-Control)

Ce document s’applique à **toute** sortie utilisateur de `bmad-code-review`, Bugbot et revues adversariales explicites.
Il prime sur le style des sous-agents et des étapes internes du workflow.

**Barème de clarté (obligatoire) :** la **première** présentation d’un constat doit déjà être aussi claire que si Guillaume venait de répondre « Reformule le problème en langage naturel et clair ». Il ne doit **jamais** avoir à demander cette reformulation.

---

## Langue

- **Toujours répondre en français** à Guillaume.
- Ne jamais coller tel quel un output sous-agent en anglais, ni des noms de champs JSON (`decision-needed`, `patch`, etc.) sans les traduire.
- Chemins `fichier:ligne` et noms de symboles : autorisés **en complément court**, jamais comme colonne vertébrale du texte.

## Ton

- Phrases **courtes et complètes**, langage naturel — comme une explication orale à un producteur de musique qui lit le code occasionnellement.
- Interdit : style télégraphique, pile de jargon, phrases qui empilent fichier + critère d’acceptance + pixels + noms de classes.
- Un terme technique indispensable → une courte glose à la première occurrence (ex. « APVTS = stockage des paramètres du plugin »).

| Interne | Dire à Guillaume |
|---------|------------------|
| `decision-needed` | décision à prendre |
| `patch` | correction à appliquer |
| `defer` | à reporter |
| `dismiss` | sans action |
| Blind Hunter | analyse du diff |
| Edge Case Hunter | cas limites |
| Acceptance Auditor | conformité à la spec |
| oneshot / File List / AC #N | oneshot → petite tâche hors story ; File List → liste de fichiers ; AC → critère d’acceptance (éviter les numéros d’AC dans le corps du problème) |

---

## Structure globale (avant le détail)

1. **En bref** — une phrase sur ce qui a été revu et l’enjeu principal **en langage de produit**, pas de fichiers.
2. **Verdict** — comptes : décisions à prendre / corrections à appliquer / à reporter.

---

## Format de chaque constat (présentation chat)

### Titre

- **Décision à prendre :** titre = **question claire** que Guillaume peut comprendre sans ouvrir le code  
  (ex. « Est-ce qu’on valide 6-16 alors que d’autres changements sont dans les mêmes fichiers ? »).  
  Interdit : titres abstraits du type « Contenu hors périmètre dans les fichiers de la story ».
- **Correction / à reporter :** titre = action ou risque en français simple.

### Corps — quatre blocs, dans cet ordre

1. **Ce qui est en jeu** — ce que ça change pour Guillaume ou pour l’utilisateur du plugin (quand tu valides, ce qui reste flou, ce qui peut casser). Max 2–3 phrases courtes. Zéro pile de noms de fichiers.
2. **Le problème** — raconter d’abord **l’intention** (« La story devait… »), puis **ce qui ne colle pas** en puces courtes, en français. Les preuves techniques (fichier, ligne, AC, pixels) viennent **après**, en une ligne ou une parenthèse, pas dans la phrase principale.
3. **Ce qu’il faut faire** — action unique, ou options numérotées avec des **verbes du quotidien** (« Tout garder ensemble », « Ne garder que cette story »), pas du jargon d’ingénierie.
4. **Si on ne fait rien** — conséquence concrète en une phrase.

### Recommandation BMad (obligatoire pour chaque décision)

- **Option recommandée** — numéro + intitulé court  
- **Pourquoi** — 2 à 4 phrases **accessibles** (même barème de clarté)  
- **Confiance** — `ferme` ou `nuancée`

Guillaume ne doit jamais devoir redemander la recommandation.

Critères de choix : (1) risque utilisateur / stabilité, (2) `project-context.md` et conventions, (3) périmètre de la story.

---

## Règles anti-densité (ce qui a fait échouer la revue 6-16)

**Interdit dans « Le problème » et « Ce qui est en jeu » :**

- Empiler dans une même phrase : chemin de fichier + numéro d’AC + cotes en pixels + noms de classes/modules
- Allusions opaque (« Option A′ », « oneshot `spec-…` », « volataires ») sans dire d’abord ce que ça signifie pour la décision
- Partir des fichiers pour raconter le problème (partir de l’intention produit)

**Autorisé :**

- Une courte liste à puces de « choses en trop / manquantes »
- Une ligne technique en bas : « Détail technique : `DesignAtoms.h`, largeurs History, … »

### Auto-contrôle avant d’envoyer

Relire le texte et répondre oui à tout, sinon **réécrire** avant envoi :

1. Un lecteur qui ne connaît pas les noms de fichiers comprend-il la question en 20 secondes ?
2. Le titre est-il une question (décision) ou une action claire (correction) ?
3. A-t-on enlevé au moins une couche de jargon / références techniques de la phrase principale ?
4. Les options se distinguent-elles sans connaître le code ?

Si Guillaume devrait dire « reformule en langage naturel », **le message n’est pas prêt**.

---

## Exemple — mauvais puis bon (tiré de la revue 6-16)

### Mauvais (trop dense — à ne plus produire)

> **Décision 1 — Contenu hors périmètre dans les fichiers de la story**  
> Dans `DesignAtoms.h` et `PatchMutatorPanel.cpp`, outre CLEAR→CL/20 et le toggle MM, le diff contient le recalage de largeurs (History/sliders → 56, MUTATE/RETRY/COMPARE → 48)… Les chiffres d’AC #4 (EXPORT à x=244, MM à x=248) ne tiennent qu’avec ce paquet… Il existe déjà une oneshot `spec-patch-mutator-history-control-widths`…

### Bon (niveau exigé dès la première version)

> **Décision 1 — Est-ce qu’on valide 6-16 alors que d’autres changements sont dans les mêmes fichiers ?**  
> **Ce qui est en jeu** — Quand tu diras « 6-16 est bon », on doit savoir si tu valides seulement le bouton MM, ou aussi d’autres retouches d’interface dans le même lot.  
> **Le problème** — La story devait ajouter le toggle MM et rétrécir CLEAR. Or, dans les mêmes fichiers, il y a aussi des largeurs déjà changées ailleurs, un détail de mise en page, et des changements sur le menu History. Sans ces largeurs, l’alignement MM ne tombe pas juste.  
> **Options** — (1) Tout garder ensemble · (2) Ne garder que 6-16 · (3) Garder les largeurs, sortir le History  
> **Recommandation BMad** — Option 1, parce que les largeurs sont déjà là et nécessaires pour l’alignement ; tout ressortir maintenant coûte cher pour peu de gain. Confiance : nuancée.

---

## Story file vs chat

- Section **Review Findings** dans la story : peut rester plus compacte / technique (pour la traçabilité).
- **Chat avec Guillaume** : toujours le niveau « Bon » ci-dessus. Ne jamais coller le texte dense de la story tel quel dans le chat.

## Sous-agents

Toujours reformuler en français clair avant présentation. Un tableau récap peut garder `fichier:ligne` dans une colonne Emplacement.

## Walkthrough des décisions (step 04)

Une décision à la fois, déjà au niveau « Bon », avec Recommandation BMad, puis attente du numéro de Guillaume.
