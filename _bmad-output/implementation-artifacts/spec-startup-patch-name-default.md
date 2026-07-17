---
title: 'Startup Patch Name Default'
type: 'bugfix'
created: '2026-07-17'
status: 'done'
review_loop_iteration: 0
baseline_commit: 'cb28f38b213fdd2d4e4e71be82a0ccf8ba0cdfba'
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md'
  - '{project-root}/_bmad-output/implementation-artifacts/1-6-wire-patch-name-display.md'
  - '{project-root}/_bmad-output/implementation-artifacts/7-8-header-footer-shell-and-persistence.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Session restore can show a leftover Patch Name (e.g. mutator `M00` or any previously loaded name) even though startup policy does **not** auto-load a patch — so the UI falsely claims a patch identity. Idle display must be `--------` (`kDefaultPatchName`).

**Approach:** Treat `kPatchName` like other non-patch prefs only for *live* session use: do **not** persist it across sessions, and on every session restore / ephemeral mutator reset path force it back to `kDefaultPatchName`, keeping APVTS and PatchModel name bytes aligned. Live in-session naming (mutate, `.syx` load, SAVE AS) is unchanged until the next save/reload boundary.

## Boundaries & Constraints

**Always:**
- Align with **D-010**: restore prefs, not a claimed loaded patch at startup; Mutator history remains non-persisted.
- Use `PluginDisplayNames::…::kDefaultPatchName` (`"--------"`) as the SSOT idle string.
- On `getStateInformation` (or equivalent strip-before-save): remove / reset `kPatchName` so clean shutdowns do not write a name into session XML.
- On `setStateInformation` / session-load ephemeral reset: unconditionally set `kPatchName` to `kDefaultPatchName` and sync PatchModel name bytes — covers crash / stale XML without a clean strip.
- Core owns the policy; GUI continues to read `kPatchName` only.

**Ask First:**
- None for this scope (policy chosen: unconditional default at session boundary).

**Never:**
- Do not auto-load a `.syx`, Internal patch, or synth edit buffer solely to populate the name at startup.
- Do not change live in-session MUTATE / load / SAVE AS naming behavior before a session boundary.
- Do not broaden SessionPersistencePolicy’s PARAM strip rules beyond what is needed for the name property.
- Do not add GUI→Core dependencies or editable Patch Name UX.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Mutate then clean round-trip | MUTATE → `M00`, then save/load | Saved XML has no meaningful persisted name; after load display/`kPatchName`/`model` name are `--------`; history empty | N/A |
| Real name then round-trip | Session had `MY-PATCH`, then save/load | After load: `--------` (name is not a restored pref) | N/A |
| Crash / stale XML | Restored state still contains `M00` or `MY-PATCH` | Load path forces `--------` anyway | N/A |
| Cold start / missing property | No `kPatchName` | Existing init → `--------` | N/A |
| Live session (no reload) | User mutates or loads `.syx` | Display updates to that name until next session boundary | N/A |

</frozen-after-approval>

## Code Map

- `Source/Core/Services/PatchMutator/MutatorSessionPersistence.h` -- strip ephemeral mutator keys on save; reset on load; extend both for unconditional `kPatchName` → `kDefaultPatchName` (or remove property on strip)
- `Source/Core/PluginProcessor.cpp` -- `getStateInformation` / `setStateInformation` / `resetEphemeralMutatorStateAfterSessionLoad`; ensure name default + `PatchNameSyncer` buffer sync after replaceState
- `Source/Core/Models/PatchNameSyncer.{h,cpp}` -- APVTS ↔ PatchModel bytes 0–7 after forcing default
- `Source/Shared/Definitions/PluginDisplayNames.h` -- `kDefaultPatchName` SSOT
- `Source/Shared/Definitions/PluginIDs.h` (PatchNameModule) -- `kPatchName` property id
- `Source/GUI/Panels/.../PatchNameDisplayPanel.cpp` -- already syncs from APVTS; no GUI policy change expected
- `Tests/Unit/MutatorRecipePersistenceTests.cpp` and/or `SessionPersistencePolicyTests.cpp` -- round-trip: name not restored; load forces default; live mutate still names in-session
- `Tests/Unit/PatchNameSyncerTests.cpp` -- only if sync-after-reset needs extra coverage

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Services/PatchMutator/MutatorSessionPersistence.h` -- on strip-for-persistence, clear `kPatchName` (remove or set default); on reset-after-load, unconditionally set `kPatchName` to `kDefaultPatchName` -- name is session-ephemeral like history
- [x] `Source/Core/PluginProcessor.cpp` -- after load-time reset, sync default name into PatchModel via `PatchNameSyncer` when available -- keep bytes 0–7 aligned
- [x] `Tests/Unit/MutatorRecipePersistenceTests.cpp` (and/or session persistence tests) -- cover I/O matrix: clean save omits/resets name; load forces `--------` even if stale XML had a name; in-session mutate still writes mutator name before reload -- prevent regression

**Acceptance Criteria:**
- Given any prior session name (`M00`, `MY-PATCH`, etc.), when state is saved then restored, then Patch Name display and `kPatchName` are `--------`.
- Given stale XML that still embeds a name (crash / old builds), when state is restored, then the load path still forces `--------`.
- Given an open session with no reload, when the user mutates or loads a named patch, then the display still shows that live name.
- Given missing/empty `kPatchName` on cold start, when existing init/display fallbacks run, then `--------` still applies.

## Spec Change Log

## Design Notes

**Policy (confirmed):** D-010 + no automatic patch load at startup → do not claim a patch identity after session restore. Unconditional `kDefaultPatchName` at the session boundary; mutator-format detection is **not** used.

**Defense in depth:**
1. Strip/clear `kPatchName` in `getStateInformation` so clean exits do not persist a name.
2. Force `kDefaultPatchName` on `setStateInformation` so crash leftovers and legacy XML cannot reappear.

**Not in scope:** whether offline (no MIDI ports) PARAM values still restore — that remains SessionPersistencePolicy / D-010 as already implemented; this bugfix only corrects patch **name** metadata at the session boundary.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- expected: build succeeds
- `Builds/macOS/ARM/Debug/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests` (MutatorRecipePersistence cases) -- expected: new patch-name persistence cases + existing mutator tests green
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_AU` -- expected: plugin rebuild succeeds

**Manual checks:**
- Mutate once (see `M00`), save/reload project or re-open with session restore → Patch Name is `--------`.
- Load a `.syx` with a real name in-session → name shows; after reload → `--------` again.
- Cold start with no state → `--------`.

## Suggested Review Order

**Session-boundary policy**

- Strip patch name from saved state so clean shutdowns do not persist identity.
  [`MutatorSessionPersistence.h:100`](../../Source/Core/Services/PatchMutator/MutatorSessionPersistence.h#L100)

- Force `--------` on every session-load reset (crash / legacy XML safety net).
  [`MutatorSessionPersistence.h:115`](../../Source/Core/Services/PatchMutator/MutatorSessionPersistence.h#L115)

**Processor wiring**

- Reset ephemeral state on the restored tree *before* `replaceState` to avoid stale name sync.
  [`PluginProcessor.cpp:626`](../../Source/Core/PluginProcessor.cpp#L626)

- After load reset, push default name into PatchModel bytes 0–7.
  [`PluginProcessor.cpp:1740`](../../Source/Core/PluginProcessor.cpp#L1740)

**Tests**

- Round-trip: saved XML omits name; restore yields `--------`.
  [`MutatorRecipePersistenceTests.cpp:175`](../../Tests/Unit/MutatorRecipePersistenceTests.cpp#L175)

- Stale embedded name still forced to default on reset.
  [`MutatorRecipePersistenceTests.cpp:193`](../../Tests/Unit/MutatorRecipePersistenceTests.cpp#L193)

- Live in-session name survives until persistence strip.
  [`MutatorRecipePersistenceTests.cpp:206`](../../Tests/Unit/MutatorRecipePersistenceTests.cpp#L206)
