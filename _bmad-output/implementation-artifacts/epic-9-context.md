# Epic 9 Context: Unsaved Edit Protection

<!-- Compiled from planning artifacts. Edit freely. Regenerate with compile-epic-context if planning docs change. -->

## Goal

Prevent silent loss of patch work when users navigate away from an edited patch without storing it to the synth or saving it as a `.syx` file. The epic adds Core dirty-state tracking against the last loaded or synced snapshot, then intercepts internal patch and Computer Patches navigation with a confirmation dialog governed by a user-configurable Settings policy.

## Stories

- Story 9.1: DirtyPatchTracker
- Story 9.2: Unsaved Edit Confirmation Dialog

## Requirements & Constraints

- Treat the patch as **dirty** when the current PATCH APVTS state (or packed buffer) differs from the **last loaded or synced snapshot**.
- Refresh the clean snapshot after: internal patch load, Computer Patches file load, successful `requestCurrentPatch`, and post-STORE sync. Saving a `.syx` should also clear dirty state (Story 9.1 AC covers save/reset cases).
- **Warn before navigation away** when dirty and the user attempts to load a different patch via:
  - Internal Patches navigation (Prev/Next, patch NumberBox, bank change)
  - Computer Patches combobox selection change
  - Computer Patches Previous/Next file buttons (FR-52)
- Show a **confirmation dialog** explaining that edits were not stored to the synth or saved as `.syx`. User may **cancel** (stay on current edit) or **confirm** (discard edits and proceed with the requested load).
- **Settings policy** (wired in Epic 7 Settings): **warn always** (default) or **never warn** (expert mode). Policy persists across sessions with other plugin prefs.
- **Do not modal-block** silent flows covered elsewhere: bulk import name reconciliation (D-025), init template loads (D-037), and Mutator MUTATE/RETRY (in-session by design).
- Unit tests must cover load, edit, save, reset, and navigation-guard behaviour for the tracker and confirmation flow.

## Technical Decisions

- **`DirtyPatchTracker`** lives in Core at `src/Core/Services/DirtyPatchTracker.*`. It compares current PATCH data against the stored snapshot — not GUI state.
- Snapshot source of truth aligns with Epic 1: PATCH APVTS parameters and/or `PatchModel` packed buffer via descriptor mappers. No parallel offset tables.
- The tracker is owned by the composition root (`PluginProcessor` service graph per D-058) and consulted by navigation handlers — not by GUI panels directly.
- **Story 9.2** integrates with Epic 7 **`PatchManagerActionHandler`** navigation paths (internal patch + Computer Patches). Panels remain presentation-only; handlers gate loads before dispatching.
- User modals are rare by architecture policy — FR-51 unsaved warning is one of the explicitly permitted cases (alongside Defrag confirm and MASTER init confirm). Do not add blocking spinners on the MIDI path.
- Settings unsaved-warning policy is an APVTS preference exposed in Story 7.7; Epic 9 reads it when deciding whether to show the dialog.

## UX & Interaction Patterns

- Standard confirm/cancel modal — not a footer message. Cancel preserves the current edit context; confirm proceeds with the pending navigation load.
- Default behaviour warns on every qualifying navigation; expert "never warn" skips the dialog and loads immediately.
- Dialog copy should make clear both loss vectors: unsaved synth STORE and unsaved `.syx` save.

## Cross-Story Dependencies

- **Within epic:** 9.1 (tracker) → 9.2 (confirmation dialog on navigation).
- **Upstream:** Epic 1 (PatchModel/APVTS sync), Epic 4 (Computer Patches load/navigation via `PatchFileService`), Epic 7 (ActionDispatcher + `PatchManagerActionHandler` navigation wiring; Settings policy in Story 7.7).
- **Implementation sequence:** Epic 9 follows Epics 4 and 7 in the D-058 critical path (E8 may parallelize; E9 needs navigation handlers and file library in place).
