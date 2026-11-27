---
title: 'Patch Mutator flat .syx export for Open'
type: 'feature'
created: '2026-09-05'
status: 'done'
baseline_commit: 'abd6720e23c5ae20dc7495485f0b675307d39e43'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Patch Mutator EXPORT nests each mutation under `Mxx/` folders. Computer Patches Open only lists `.syx` files in one chosen folder, so re-listening a mutation/retry series after export is awkward.

**Approach:** Keep the existing session folder + collision Keep/Overwrite flow. Inside that layout root, write a flat list of self-describing files (`INITIAL.syx`, `M00.syx`, `M00-R00.syx`, …) with no `Mxx/` (or retry) subdirectories. Do not change Open.

## Boundaries & Constraints

**Always:**
- Flat files only under the current layout root (session folder when present; otherwise the direct export folder).
- Filename stems stay `MutationNaming` / D-085 style: `Mxx`, `Mxx-Ryy`; export file for the initial snapshot is literally `INITIAL.syx` (all caps).
- Continue stamping live user patch name into SysEx name bytes 0–7 at write time (existing `writeExportPatchFile` behavior).
- Reuse `savePatchSysExFile`; fail-fast on write errors; empty-history / non-writable gates unchanged.
- Keep session basename wrapper + Keep/Overwrite collision UX unchanged.
- Update unit tests that assert nested `Mxx/` paths and `Initial.syx`.

**Ask First:**
- Changing Open / Computer Patches browser to recurse folders.
- Removing or renaming the session export folder wrapper.
- Adding an export preference or per-export prompt for nested vs flat.
- Rewriting historical PRD/epic text beyond a short comment/doc note that FR-33 layout is superseded by flat files.

**Never:**
- Nested `Mxx/` (or `Ryy/`) directories in new exports.
- Changing Computer Patches Open, bank export, or MutationNaming stem format (`Mxx` / `Mxx-Ryy`).
- Auto-export on mutate/retry.
- French strings in source or UI.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Happy path | History with initial + root 0 + retries 0,1 | Layout root contains `INITIAL.syx`, `M00.syx`, `M00-R00.syx`, `M00-R01.syx`; no `M00/` directory | N/A |
| Gap indices | Roots 0, 5, 99 | Files `M00.syx`, `M05.syx`, `M99.syx` at layout root; no contiguous renumbering; no `M01.syx` | N/A |
| Session export | Frozen basename + writable pick folder | Same flat files inside `{basename}/` (or Keep suffix); collision UX unchanged | Existing folder-not-writable / empty-history failures |
| Name bytes | Named history entries + non-empty user stamp | File basenames are `INITIAL` / `Mxx` / `Mxx-Ryy`; SysEx name bytes still receive user stamp | Save failure → fail-fast, existing error message |

</frozen-after-approval>

## Code Map

- `Source/Core/Services/PatchFileServiceMutatorExport.cpp` — **layout SSOT**: `writeInitialSnapshot` (~30–35) uses `"Initial.syx"` → change to `"INITIAL.syx"`; `writeRootFolder` (~107–133) creates `Mxx/` then writes into it — write `writeRootEntry` / `writeRetryEntries` into `folder` instead (no `createDirectory` for roots); `writeAllRootFolders` / `writeHistoryLayout` / `exportMutatorHistory` / `exportMutatorHistorySession` stay orchestration only.
- `Source/Core/Services/PatchFileService.h` — comments still say FR-33 nested `Mxx/` / hybrid session layout (~84–90); update wording to flat files under session/layout root.
- `Source/Core/Services/PatchMutator/MutationNaming.cpp` — keep `formatRootLabel` / `formatExportStem`; do not change stems.
- `Source/Core/Services/PatchMutator/PatchMutatorEngineHistory.cpp` — session folder + collision; **read-only** unless a footer/message hard-codes nested layout (unlikely).
- `Source/GUI/Handlers/MutatorActionHandler.cpp` / `PluginProcessorGates.cpp` — picker + gates; **read-only**.
- `Tests/Unit/PatchFileServiceMutatorExportTests.cpp` — asserts `Initial.syx` and `M00/M00.syx` / nested retries / gap dirs / session `Initial.syx`; rewrite expectations to flat paths + `INITIAL.syx`.
- `Tests/Unit/PatchMutatorEngineExportTests.cpp` — session basename tests; update if they assert nested layout or `Initial.syx`.
- `Documentation/Users/manuel-utilisateur.md` (~439) — EXPORT row still mentions subfolders; update to flat list + `INITIAL.syx`.
- Historical FR-33 (`6-11-mutator-export-layout.md`, PRD addendum) — superseded by this spec for new behavior; do not mass-edit unless Ask First cleared.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Services/PatchFileServiceMutatorExport.cpp` -- Write initial + all root/retry `.syx` into the layout root with no `Mxx/` directories; rename export file to `INITIAL.syx` -- Flat Open-friendly export.
- [x] `Source/Core/Services/PatchFileService.h` -- Update export comments to describe flat session layout -- Avoid stale FR-33 nested guidance.
- [x] `Tests/Unit/PatchFileServiceMutatorExportTests.cpp` (+ engine export tests if needed) -- Assert flat paths, gap files, `INITIAL.syx`, session flat layout -- Lock I/O matrix.
- [x] `Documentation/Users/manuel-utilisateur.md` -- Document flat EXPORT contents -- User-facing accuracy.

**Acceptance Criteria:**
- Given a non-empty mutator history and a writable export destination, when EXPORT runs, then the layout root contains only flat `.syx` files named `INITIAL.syx` (if initial present), `Mxx.syx`, and `Mxx-Ryy.syx` with no mutation subdirectories.
- Given Open in Computer Patches pointed at that layout root, when the folder is scanned, then every exported mutation/retry file appears in the linear list without changing Open code.
- Given gap root indices (e.g. 0, 5, 99), when EXPORT runs, then matching `Mxx.syx` files appear at the layout root without inventing intermediate indices.
- Given Keep/Overwrite session collision rules, when EXPORT runs, then those rules still apply and only the inner layout is flattened.
- Given existing empty-history and non-writable folder cases, when EXPORT is attempted, then behavior and footers remain as today.

## Spec Change Log

## Design Notes

Supersedes story 6.11 / FR-33 **nested folder** shape for new exports. Session folder packaging from later mutator export work stays — Open should target the session folder (flat list), not invent multi-level browsing.

Golden example after export into session `WARMPAD/`:

```text
WARMPAD/
  INITIAL.syx
  M00.syx
  M00-R00.syx
  M01.syx
  M01-R00.syx
```

Optional cleanup: rename `writeRootFolder` → something that no longer implies a directory, if the analyser/style pass stays green.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- build succeeds
- `ctest --test-dir Builds/macOS/Debug --output-on-failure -R 'PatchFileServiceMutatorExport|PatchMutatorEngineExport'` -- those tests pass (adjust ctest filter to project conventions if needed)
- `python3 Scripts/quality/lint_touched.py` -- clean on touched C++ under `Source/` / `Tests/`

**Manual checks (if no CLI):**
- EXPORT a short history → open the session folder via Computer Patches Open → confirm full flat list including `INITIAL.syx`.

## Suggested Review Order

**Flat layout write path**

- Initial snapshot filename is now literal `INITIAL.syx`.
  [`PatchFileServiceMutatorExport.cpp:33`](../../Source/Core/Services/PatchFileServiceMutatorExport.cpp#L33)

- Roots and retries write into the layout root; no `Mxx/` directories.
  [`PatchFileServiceMutatorExport.cpp:107`](../../Source/Core/Services/PatchFileServiceMutatorExport.cpp#L107)

- Public comments document flat session layout.
  [`PatchFileService.h:84`](../../Source/Core/Services/PatchFileService.h#L84)

**Tests and docs**

- Exact on-disk `INITIAL.syx` casing via directory listing.
  [`PatchFileServiceMutatorExportTests.cpp:58`](../../Tests/Unit/PatchFileServiceMutatorExportTests.cpp#L58)

- Overwrite clears legacy nested `M00/` and old `Initial.syx`.
  [`PatchFileServiceMutatorExportTests.cpp:288`](../../Tests/Unit/PatchFileServiceMutatorExportTests.cpp#L288)

- User manual EXPORT row names flat stems and Open reuse.
  [`manuel-utilisateur.md:439`](../../Documentation/Users/manuel-utilisateur.md#L439)
