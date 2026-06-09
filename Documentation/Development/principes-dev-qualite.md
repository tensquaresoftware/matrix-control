# Principes de développement logiciel de qualité

Référence complète des acronymes et principes à connaître, organisée par famille.

> **Usage Matrix-Control :** référence humaine complète. Règles agent condensées dans `CONVENTIONS.md` §6.16 et `.cursor/rules/`.

---

## SOLID — Design orienté objet

| Lettre | Nom complet | Résumé |
|--------|-------------|--------|
| **S** | Single Responsibility Principle (SRP) | Une classe = une seule raison de changer |
| **O** | Open/Closed Principle (OCP) | Ouvert à l'extension, fermé à la modification |
| **L** | Liskov Substitution Principle (LSP) | Les sous-types doivent être substituables sans casser le comportement |
| **I** | Interface Segregation Principle (ISP) | Préférer plusieurs interfaces spécialisées à une grosse interface généraliste |
| **D** | Dependency Inversion Principle (DIP) | Dépendre des abstractions, pas des implémentations concrètes |

---

## Simplicité — Pragmatisme quotidien

| Acronyme | Nom complet | Résumé |
|----------|-------------|--------|
| **KISS** | Keep It Simple, Stupid | La solution la plus simple qui fonctionne est presque toujours la meilleure |
| **YAGNI** | You Aren't Gonna Need It | N'implémenter que ce dont on a besoin maintenant, pas ce qu'on imagine pour le futur |
| **DRY** | Don't Repeat Yourself | Chaque logique a une représentation unique et non ambiguë dans le système |
| **WET** | Write Everything Twice | Anti-principe : dupliquer une fois plutôt que de sur-abstraire prématurément |
| **LoD** | Law of Demeter (principe de moindre connaissance) | Ne parler qu'à ses voisins immédiats — éviter les chaînes `a.getB().getC().doSomething()` |

---

## Clean Code / Clean Architecture

| Acronyme | Nom complet | Résumé |
|----------|-------------|--------|
| **SoC** | Separation of Concerns | Chaque module ou couche gère une préoccupation distincte |
| **CoC** | Convention over Configuration | Des conventions sensées réduisent la configuration explicite |
| **PI** | Persistence Ignorance | Le domaine métier ne doit pas connaître la couche de persistance |
| **ETC** | Easy to Change | Le vrai objectif du bon design : tout choix facilite les changements futurs |
| — | Intention-revealing names | Nommer les choses pour révéler le *pourquoi*, pas le *comment* |

---

## Cohésion & Couplage — Structure des modules

| Acronyme | Nom complet | Résumé |
|----------|-------------|--------|
| **HC** | High Cohesion | Les éléments d'un module doivent être fortement liés entre eux |
| **LC** | Loose Coupling | Les modules doivent dépendre le moins possible les uns des autres |
| **CRP** | Common Reuse Principle | Regrouper ce qui est réutilisé ensemble ; séparer ce qui change indépendamment |
| **CCP** | Common Closure Principle | Ce qui change pour la même raison doit être dans le même package |

---

## Tests & Qualité

| Acronyme | Nom complet | Résumé |
|----------|-------------|--------|
| **TDD** | Test-Driven Development | Red → Green → Refactor : écrire le test avant le code |
| **F.I.R.S.T.** | Fast, Independent, Repeatable, Self-validating, Timely | Les cinq qualités d'un bon test unitaire |
| **AAA** | Arrange / Act / Assert | Structure standard d'un test unitaire lisible |
| — | Test pyramid | Beaucoup d'unitaires, moins d'intégration, peu d'E2E |
| — | Mutation testing | Valider la pertinence des tests en injectant des bugs artificiels |

---

## Sécurité & Robustesse

| Acronyme | Nom complet | Résumé |
|----------|-------------|--------|
| **PoLP** | Principle of Least Privilege | Donner uniquement les permissions strictement nécessaires |
| **DiD** | Defense in Depth | Multiplier les couches de sécurité indépendantes |
| **FSB** | Fail-Safe / Fail-Fast | Échouer tôt, bruyamment et en sécurité plutôt que silencieusement |
| — | Loi de Postel (Robustness Principle) | Être libéral dans ce qu'on accepte, strict dans ce qu'on émet |

---

## API & Contrats entre systèmes

| Acronyme | Nom complet | Résumé |
|----------|-------------|--------|
| **BC** | Backward Compatibility | Les évolutions d'une API ne doivent pas casser les clients existants |
| **CQS** | Command Query Separation | Une fonction est soit une commande (mutante), soit une requête (lisant) — jamais les deux |
| **IdM** | Idempotency | Appeler plusieurs fois la même opération donne le même résultat |
| **DbC** | Design by Contract | Préconditions, postconditions et invariants explicitement définis |

---

## Lean Software — Flux & Élimination des gaspillages

| Acronyme / Nom | Résumé |
|----------------|--------|
| **Small Batches** | Livrer de petits incréments fréquemment plutôt que de grosses releases rares |
| **Élimination des gaspillages** | Supprimer tout ce qui ne produit pas de valeur : features non utilisées, attentes, dette technique |
| **Décider le plus tard possible** | Retarder les décisions irréversibles jusqu'au dernier moment responsable (principe d'optionnalité) |
| **Amplifier l'apprentissage** | Prototypes, spikes, itérations courtes — l'échec rapide est une source d'information |
| **Livraison continue (CD)** | Tout commit mergé est potentiellement déployable en production |

---

## XP (Extreme Programming) — Pratiques techniques

| Acronyme / Nom | Résumé |
|----------------|--------|
| **CI** — Intégration continue | Merger et valider le code plusieurs fois par jour avec une suite de tests automatisés |
| **TDD** — Test-Driven Development | (voir section Tests ci-dessus) |
| **Refactoring continu** | Améliorer la structure interne sans changer le comportement observable — pratique permanente |
| **Feedback rapide** | Réduire le délai entre une décision et son résultat observable (tests < 1 min, CI < 10 min) |
| **Propriété collective du code** | N'importe quel développeur peut modifier n'importe quelle partie du code — aucun silo |
| **Pair programming** | Deux développeurs sur le même clavier : driver (code) + navigator (stratégie) |
| **Coding Standards** | Conventions partagées de style, nommage et architecture — le code semble écrit par une seule personne |

---

## Process & Culture d'équipe

| Nom | Résumé |
|-----|--------|
| **Règle du Boy Scout** | Toujours laisser le code plus propre qu'on ne l'a trouvé |
| **Kaizen** | Petites améliorations constantes valent mieux que les grandes refactos rares |
| **PIE** — Program Intently & Expressively | Le code est lu bien plus souvent qu'il n'est écrit — optimiser pour la lecture |
| **CI/CD** | Intégrer et livrer souvent pour réduire le risque des changements |

---

## Note sur les priorités contextuelles

Ces principes ne s'appliquent pas tous avec la même intensité selon le contexte. Certains projets (embarqué, temps réel, audio) ont des contraintes qui font passer KISS et YAGNI avant une architecture SOLID stricte. D'autres (APIs publiques, SDKs) rendent BC et DbC critiques dès le premier jour.
