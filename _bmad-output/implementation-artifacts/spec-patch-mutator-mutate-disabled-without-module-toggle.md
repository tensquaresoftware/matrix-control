---
organization: Ten Square Software
project: Matrix-Control
title: Patch Mutator — Disable MUTATE Without Module Toggle
author: BMad Agent
type: bugfix
created: 2026-07-15
status: done
route: one-shot
updated: 2026-07-15
---

# Patch Mutator — Disable MUTATE Without Module Toggle

## Intent

**Problem:** With every recipe module toggle off, MUTATE stayed clickable and produced a no-op mutation experience.

**Approach:** Gate `patchMutatorMutateEnabled` on “history slot available AND at least one module toggle on,” refresh that mirror when toggles change, and block `mutate()`/`retry()` with a clear footer if the button path is bypassed.

## Suggested Review Order

**Enable predicate**

- Core recipe helper that defines “any module in scope”
  [`MutationAlgorithm.h:27`](../../Source/Core/Services/PatchMutator/MutationAlgorithm.h#L27)

- MUTATE enabled only when a root slot exists and scope is non-empty
  [`PatchMutatorEngine.cpp:652`](../../Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp#L652)

- Engine refresh writes the updated mutate mirror
  [`PatchMutatorEngine.cpp:677`](../../Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp#L677)

**Live toggle → button greying**

- Shared enable-ID table + property detector for listener and empty-history helper
  [`MutatorSessionPersistence.h:26`](../../Source/Core/Services/PatchMutator/MutatorSessionPersistence.h#L26)

- Processor refreshes mirrors when any module enable property changes
  [`PluginProcessor.cpp:1364`](../../Source/Core/PluginProcessor.cpp#L1364)

- Startup/session empty-history init prefers the engine refresh path
  [`PluginProcessor.cpp:1567`](../../Source/Core/PluginProcessor.cpp#L1567)

**Defense in depth**

- Block mutate/retry when scope is empty with a dedicated footer
  [`PatchMutatorEngine.cpp:190`](../../Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp#L190)

**Tests**

- Empty history, no toggle → mutate disabled
  [`PatchMutatorEngineTests.cpp:1792`](../../Tests/Unit/PatchMutatorEngineTests.cpp#L1792)

- mutate() rejects empty scope without history growth or SysEx
  [`PatchMutatorEngineTests.cpp:342`](../../Tests/Unit/PatchMutatorEngineTests.cpp#L342)

- Persistence helper: all toggles off disables mutate; LFO2 alone re-enables
  [`MutatorRecipePersistenceTests.cpp:201`](../../Tests/Unit/MutatorRecipePersistenceTests.cpp#L201)
