# Barème de clarté BMad (Matrix-Control) — SSOT

Ce document est la **référence unique** pour le chat avec Guillaume pendant :

- `bmad-code-review`
- `bmad-quick-dev` (y compris la revue interne et le bilan)
- `bmad-create-story`
- `bmad-dev-story`

Il prime sur le style des sous-agents, des étapes internes et de tout jargon d’ingénierie.

**Contexte produit :** Guillaume a souvent dû demander « reformule » pour comprendre.  
Toute sortie qui nécessiterait encore cette demande est un **échec** — à corriger **avant** l’envoi.

---

## Portée (élargie — non négociable)

Le barème s’applique à **tout** ce que Guillaume lit dans le chat, pas seulement aux « messages importants » :

| Type de sortie | Exigence |
|---|---|
| Explication / résumé / contexte | Français clair, intention produit d’abord |
| Question posée | Question compréhensible seule, sans jargon |
| Décision à trancher | Options en verbes du quotidien + Recommandation BMad |
| Constats de revue | Structure revue + même clarté |
| Amorce `Contexte de la Story` | Une phrase synthétique claire |

---

## Phrase-test (non négociable)

Avant d’envoyer **n’importe quel** message (explication, question, décision, constat, résumé) :

> Si Guillaume répondait maintenant « Reformule en langage naturel, sans jargon excessif », est-ce que je réécrirais ce texte ?

- **Oui** → le message n’est **pas** prêt. Réécrire.
- **Non** → on peut envoyer.

La **première** version doit déjà être au niveau de cette reformulation. Guillaume ne doit **jamais** avoir à la demander.

Variante utile (même exigence) :

> Est-ce que je pourrais dire ça à voix haute à un musicien producteur, en 20 secondes, sans qu’il demande « c’est quoi ça » ?

---

## Public cible

Écrire comme une explication **orale** à un **producteur de musique qui lit le code occasionnellement** :

- il connaît le produit et l’intention ;
- il ne veut pas décoder une pile de chemins, d’IDs techniques et d’acronymes ;
- il veut comprendre **en ~20 secondes** ce qu’on lui explique ou ce qu’on lui demande de trancher.

---

## Règles de langue et de ton

1. **Toujours le français** dans le chat (même si sous-agents / diffs / specs sont en anglais).
2. **Phrases complètes**, registre parlé — interdit le style télégraphique et les listes de mots-clés.
3. **Pas de jargon excessif.** Terme technique indispensable → glose courte à la première occurrence  
   (ex. « APVTS = stockage des paramètres du plugin »).
4. **Preuves techniques en second.** Chemins `fichier:ligne`, numéros de critères, pixels, noms de classes :  
   **une ligne complémentaire en bas**, jamais empilés dans la phrase qui porte le sens.
5. **Partir de l’intention produit** (« On devait… » / « Tu dois choisir si… »), pas des fichiers.
6. **Interdit d’empiler** dans une même phrase principale : chemin + ID de critère + symbole C++ + jargon BMad.
7. **Noms métier / libellés produit : ne pas traduire de force.**  
   Garder tels quels les termes anglais du produit Matrix-Control (UI, manuels, habitudes), même au milieu d’une phrase française.  
   Ex. **Track Generator**, **Patch Mutator**, **Master Edit**, **Compare**, **Look** — **pas** « générateur de piste », « mutateur de patch », etc.  
   Source de vérité des libellés : `PluginDisplayNames.h` / l’UI réelle.  
   Distinction : jargon d’ingénierie opaque (`oneshot`, `APVTS` sans glose) ≠ nom métier produit (à conserver).

---

## Questions et décisions — forme obligatoire

### Toute question à Guillaume

1. Une **phrase d’intention** d’abord (ce qui est en jeu pour le produit / l’usage).
2. Puis la **question** en français simple (titre = question claire si c’est une décision).
3. Si options : les numéroter avec des **verbes du quotidien**.
4. Toujours terminer par **Recommandation BMad** (voir ci-dessous).

### Interdit pour une question / décision

| Interdit | Préférer |
|---|---|
| « On merge le handler MIDI outbound queue dans le gate APVTS ? » | « Tu préfères qu’on garde l’envoi MIDI et le verrou des réglages au même endroit, ou qu’on les sépare ? » |
| Options « A / B / refactor path X » | « Tout garder ensemble » / « Séparer en deux endroits » |
| Coller un extrait de story / diff dense | Reformuler l’enjeu en 2–3 phrases |
| Anglais non glosé (`scope`, `oneshot`, `ledger`, `trade-off`) | Mot français du quotidien |

### Exemple bon (décision)

**En bref**  
Il faut choisir comment le plugin se comporte quand le synthé n’est pas encore connecté.

**Ce qui est en jeu**  
Soit on laisse l’interface active (risque de fausses manips), soit on la grise jusqu’à la connexion.

1. **Griser l’interface** jusqu’à ce que le synthé réponde  
2. **Laisser tout cliquable**, avec un message d’avertissement  

### Recommandation BMad

- **Option recommandée** — 1 · Griser l’interface  
- **Pourquoi** — ça évite d’envoyer des réglages dans le vide et ça rend l’état « pas prêt » évident d’un coup d’œil.  
- **Confiance** — ferme  

---

## Anti-motifs fréquents (à bannir du chat)

- Style télégraphique : `Fix MIDI gate + APVTS sync + UI lock`
- Titre opaque : `Contenu hors périmètre story 8-4`
- Phrase qui empile technique : « Le `MasterEditGate` court-circuite `MidiOutboundQueue` avant `setParameter` »
- Jargon BMad brut : `oneshot`, `draft`, `AC#3`, `decision-needed` sans traduction
- Coller tel quel un rapport de sous-agent en anglais
- **Traduction forcée d’un nom métier produit** : « générateur de piste » au lieu de **Track Generator**

---

## Auto-contrôle avant envoi (tout cocher)

1. Un lecteur qui ignore les noms de fichiers comprend-il l’enjeu en ~20 secondes ?
2. Pour une décision : le titre / la question est-elle claire **seule**, en français simple ?
3. A-t-on enlevé au moins une couche de jargon / références techniques de la phrase principale ?
4. Les options se distinguent-elles avec des **verbes du quotidien**, sans connaître le code ?
5. La phrase-test ci-dessus donne-t-elle « Non, je ne reformulerais pas » ?
6. Si c’est une question/décision : la **Recommandation BMad** est-elle présente et aussi claire que le reste ?

Si un seul point échoue → **réécrire** avant d’envoyer.

---

## Décisions — Recommandation BMad obligatoire

Dès que Guillaume doit choisir (même une petite question à options), ajouter **toujours** :

### Recommandation BMad

- **Option recommandée** — numéro + intitulé court
- **Pourquoi** — 2 à 4 phrases **au même barème de clarté**
- **Confiance** — `ferme` ou `nuancée`

Il reste maître de la réponse. Il ne doit jamais avoir à demander la recommandation **ni** une reformulation pour la comprendre.

Critères de choix : (1) risque utilisateur / stabilité, (2) `project-context.md` et conventions du dépôt, (3) périmètre demandé (pas de sur-ingénierie).

---

## Artifacts vs chat

- **Chat** → français, barème ci-dessus.
- **Fichiers générés** (story, spec, code map, Review Findings techniques) → anglais (`document_output_language`), sauf demande contraire.
- Ne **jamais** coller tel quel un bloc dense d’artifact / sous-agent dans le chat : toujours reformuler au niveau « phrase-test ».
