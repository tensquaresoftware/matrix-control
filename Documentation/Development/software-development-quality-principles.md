# Software Development Quality Principles

Complete reference of acronyms and principles to know, organised by family.

> **Matrix-Control usage:** full human reference. Condensed agent rules in `CONVENTIONS.md` §6.16 and `.cursor/rules/`.

---

## SOLID — Object-oriented design

| Letter | Full name | Summary |
|--------|-----------|---------|
| **S** | Single Responsibility Principle (SRP) | One class = one reason to change |
| **O** | Open/Closed Principle (OCP) | Open for extension, closed for modification |
| **L** | Liskov Substitution Principle (LSP) | Subtypes must be substitutable without breaking behaviour |
| **I** | Interface Segregation Principle (ISP) | Prefer several specialised interfaces over one large general-purpose interface |
| **D** | Dependency Inversion Principle (DIP) | Depend on abstractions, not concrete implementations |

---

## Simplicity — Everyday pragmatism

| Acronym | Full name | Summary |
|---------|-----------|---------|
| **KISS** | Keep It Simple, Stupid | The simplest solution that works is almost always the best |
| **YAGNI** | You Aren't Gonna Need It | Implement only what you need now, not what you imagine for the future |
| **DRY** | Don't Repeat Yourself | Every piece of knowledge has a single, unambiguous representation in the system |
| **WET** | Write Everything Twice | Anti-principle: duplicate once rather than over-abstract prematurely |
| **LoD** | Law of Demeter (principle of least knowledge) | Talk only to immediate neighbours — avoid chains like `a.getB().getC().doSomething()` |

---

## Clean Code / Clean Architecture

| Acronym | Full name | Summary |
|---------|-----------|---------|
| **SoC** | Separation of Concerns | Each module or layer handles a distinct concern |
| **CoC** | Convention over Configuration | Sensible conventions reduce explicit configuration |
| **PI** | Persistence Ignorance | The domain model must not know about the persistence layer |
| **ETC** | Easy to Change | The real goal of good design: every choice makes future changes easier |
| — | Intention-revealing names | Name things to reveal the *why*, not the *how* |

---

## Cohesion & Coupling — Module structure

| Acronym | Full name | Summary |
|---------|-----------|---------|
| **HC** | High Cohesion | Elements within a module should be strongly related to each other |
| **LC** | Loose Coupling | Modules should depend on each other as little as possible |
| **CRP** | Common Reuse Principle | Group what is reused together; separate what changes independently |
| **CCP** | Common Closure Principle | What changes for the same reason should live in the same package |

---

## Testing & Quality

| Acronym | Full name | Summary |
|---------|-----------|---------|
| **TDD** | Test-Driven Development | Red → Green → Refactor: write the test before the code |
| **F.I.R.S.T.** | Fast, Independent, Repeatable, Self-validating, Timely | The five qualities of a good unit test |
| **AAA** | Arrange / Act / Assert | Standard structure for a readable unit test |
| — | Test pyramid | Many unit tests, fewer integration tests, few E2E tests |
| — | Mutation testing | Validate test relevance by injecting artificial bugs |

---

## Security & Robustness

| Acronym | Full name | Summary |
|---------|-----------|---------|
| **PoLP** | Principle of Least Privilege | Grant only the permissions strictly necessary |
| **DiD** | Defense in Depth | Layer multiple independent security controls |
| **FSB** | Fail-Safe / Fail-Fast | Fail early, loudly, and safely rather than silently |
| — | Postel's Law (Robustness Principle) | Be liberal in what you accept, strict in what you emit |

---

## APIs & Contracts between systems

| Acronym | Full name | Summary |
|---------|-----------|---------|
| **BC** | Backward Compatibility | API evolution must not break existing clients |
| **CQS** | Command Query Separation | A function is either a command (mutating) or a query (reading) — never both |
| **IdM** | Idempotency | Calling the same operation multiple times yields the same result |
| **DbC** | Design by Contract | Preconditions, postconditions, and invariants explicitly defined |

---

## Lean Software — Flow & waste elimination

| Acronym / Name | Summary |
|----------------|---------|
| **Small Batches** | Ship small increments frequently rather than rare large releases |
| **Waste elimination** | Remove everything that does not deliver value: unused features, waiting, technical debt |
| **Decide as late as possible** | Delay irreversible decisions until the last responsible moment (optionality principle) |
| **Amplify learning** | Prototypes, spikes, short iterations — fast failure is a source of information |
| **Continuous Delivery (CD)** | Every merged commit is potentially deployable to production |

---

## XP (Extreme Programming) — Technical practices

| Acronym / Name | Summary |
|----------------|---------|
| **CI** — Continuous Integration | Merge and validate code several times a day with an automated test suite |
| **TDD** — Test-Driven Development | (see Testing section above) |
| **Continuous refactoring** | Improve internal structure without changing observable behaviour — an ongoing practice |
| **Fast feedback** | Reduce the delay between a decision and its observable outcome (tests < 1 min, CI < 10 min) |
| **Collective code ownership** | Any developer may change any part of the code — no silos |
| **Pair programming** | Two developers at one keyboard: driver (codes) + navigator (strategy) |
| **Coding Standards** | Shared conventions for style, naming, and architecture — code should read as if written by one person |

---

## Process & Team culture

| Name | Summary |
|------|---------|
| **Boy Scout Rule** | Always leave the code cleaner than you found it |
| **Kaizen** | Small constant improvements beat rare large refactors |
| **PIE** — Program Intently & Expressively | Code is read far more often than it is written — optimise for reading |
| **CI/CD** | Integrate and deliver often to reduce the risk of change |

---

## Note on contextual priorities

These principles do not all apply with the same intensity in every context. Some projects (embedded, real-time, audio) have constraints that put KISS and YAGNI ahead of strict SOLID architecture. Others (public APIs, SDKs) make BC and DbC critical from day one.
