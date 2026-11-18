---
title: 'Separate UI short labels from unique host parameter names'
type: 'feature'
created: '2026-11-18'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '800f5de43a047aec8bc196e1ad2a03dc77781f02'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/inventory-host-parameter-names-draft.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** One `displayName` per parameter feeds both Matrix-Control UI labels and the host-visible APVTS name, so flat DAW lists show colliding leaves like several `FREQUENCY` entries even though ParameterIDs are already unique.

**Approach:** Keep UI short names unchanged; add a separate unique Title Case host leaf name (`{Module} {Leaf}`) for every automatable Int/Choice parameter; pass host names into `AudioParameter*`; keep existing APVTS groups; do not encode the group path into the leaf string.

## Boundaries & Constraints

**Always:**
- UI reads short `displayName` only (`WidgetFactory` / `ParameterCell` unchanged in behavior).
- Host name is unique, self-sufficient, Title Case, uses real GUI module identity (`VCF/VCA`, `DCO 1`, `Bus N`, …).
- Preserve `|` inside the Mix leaf (`VCF/VCA DCO 2 | DCO 1 Mix`) with spaces around `|`; do not use `|` as a host path separator; keep DCO 2 before DCO 1 to match GUI slider polarity and automation (0 = DCO 2, 63 = DCO 1).
- ParameterIDs and APVTS group hierarchy stay unchanged.
- Apply the naming convention systematically to all automatable Int/Choice parameters (not only `FREQUENCY`).
- Approved host-name SSOT: `_bmad-output/implementation-artifacts/inventory-host-parameter-names-draft.md` (Guillaume polish: expand abbreviations, `MIDI Echo`, keep intentional module echoes such as `FM/Track Track Point N` / `Ramp/Portamento Portamento Rate` / `VCF/VCA VCA 1 Volume`).

**Never:**
- Path-style host names (`Patch Edit | DCO 1 | Frequency`).
- Lengthening UI labels to fix Live.
- Flattening/removing APVTS groups; changing group-name casing beyond minimal coherence; Hidden/non-automatable Max-style policy (separate story).
- Changing ParameterIDs or automation-by-ID mapping.
- Inverting Mix pole order relative to the UI (`DCO 1 | DCO 2`).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Flat host list | Params with same UI leaf (`FREQUENCY` ×3) | Distinct host names (`DCO 1 Frequency`, `DCO 2 Frequency`, `VCF/VCA Frequency`) | N/A |
| Hierarchical host | Same params under existing groups | Group tree unchanged; leaf is host name without duplicated path | N/A |
| Plugin UI | Any automatable param cell | Short ALL CAPS (or existing) `displayName` unchanged | N/A |
| Mix leaf | VCF/VCA balance | Host contains `DCO 2 \| DCO 1 Mix` with `|` as label text only | N/A |
| Bus params | Bus 3 destination | Host `Bus 3 Destination` (not bare `3` / `DESTINATION`) | N/A |
| Existing DAW projects | Automation mapped by ParameterID | Continues to resolve; listed labels may change | Document label remap; IDs stable |

</frozen-after-approval>

## Code Map

- `Source/Shared/Definitions/PluginDescriptors.h` -- Add `hostName` (or equivalent) on `IntParameterDescriptor` / `ChoiceParameterDescriptor` only; leave `StandaloneWidgetDescriptor` / group descriptors alone unless a compile need appears.
- `Source/Shared/Definitions/PluginDisplayNames.h` -- Keep UI short labels; add host-name string constants (parallel namespace or section) as SSOT for host copy; ASCII-only for display strings policy still applies to UI literals.
- `Source/Shared/Definitions/PluginDescriptors*.cpp` -- Fill `hostName` for every Int/Choice descriptor from the approved inventory (Master Edit, Patch Edit modules, Matrix Modulation buses, Patch Mutator automatable ints).
- `Source/Core/Factories/ApvtsLayoutBuilder.cpp` -- `addIntParameter` / `addChoiceParameter` pass `desc.hostName` into `AudioParameterInt` / `AudioParameterChoice` name argument; keep `kSubgroupSeparator` (`|`) for groups only; do not change group tree builders.
- `Source/GUI/Factories/WidgetFactory.cpp` -- Continue `getParameterDisplayName` → `displayName` (UI short); do not switch to hostName.
- `Source/GUI/Widgets/ParameterCell.cpp` -- No label source change.
- `Tests/Unit/*` harnesses that construct `AudioParameter*` from descriptors -- Prefer `hostName` for parameter name arg where the harness mirrors APVTS layout; do not assert UI labels equal host names.
- `_bmad-output/implementation-artifacts/inventory-host-parameter-names-draft.md` -- Human-approved SSOT list before coding; refresh if edits are requested.

**Reuse:** Existing module `kName` strings and bus numbering for module prefixes; existing ParameterIDs.

**Do not change:** ParameterID strings; APVTS group parent/child structure; UI short label values; standalone INIT/COPY/etc. widgets; group displayName casing unless a tiny compile/consistency fix is forced.

## Tasks & Acceptance

**Execution:**
- [x] `_bmad-output/implementation-artifacts/inventory-host-parameter-names-draft.md` -- Lock approved host-name list (apply Guillaume edits if any) -- SSOT before code.
- [x] `Source/Shared/Definitions/PluginDescriptors.h` -- Add `hostName` field on Int/Choice descriptors -- model split UI vs host.
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` -- Add host-name constants matching approved inventory -- single place for host copy.
- [x] `Source/Shared/Definitions/PluginDescriptors*.cpp` -- Wire each Int/Choice `hostName` -- cover all automatable params.
- [x] `Source/Core/Factories/ApvtsLayoutBuilder.cpp` -- Use `hostName` for `AudioParameter*` names -- host lists see unique leaves.
- [x] `Tests/Unit/` (affected harnesses + new focused uniqueness/layout check if practical) -- Ensure layout builds; host names unique; UI path still reads `displayName`.
- [x] Spec Implementation Notes -- Record host naming convention one-liner for future params.

**Acceptance Criteria:**
- Given the full APVTS layout, when collecting Int/Choice parameter names exposed to the host, then every name is unique and matches the approved inventory.
- Given ParameterCells / WidgetFactory lookups, when reading labels, then short `displayName` values are unchanged from pre-change behavior.
- Given existing ParameterIDs, when comparing before/after, then IDs and APVTS group hierarchy are unchanged.
- Given the VCF/VCA Mix parameter, when reading its host name, then it contains `DCO 2 | DCO 1 Mix` without treating `|` as a group path.
- Given a hierarchical host, when browsing groups, then leaves do not embed `Patch Edit | …` path prefixes.

## Implementation Notes

- Host leaf formula for future Int/Choice params: `{ModuleHost} {LeafTitleCase}` from the GUI module identity (Title Case / product tokens; buses use `Bus N`); store explicitly in `PluginDisplayNames::HostParameterNames` + descriptor `hostName` — never derive from the APVTS group path or change UI `displayName`.
- SSOT list: `inventory-host-parameter-names-draft.md` (151 names). Mix polarity text stays `DCO 2 | DCO 1 Mix` (`|` is label text only).
- UI continues to read `displayName` via `WidgetFactory::getParameterDisplayName`; `ApvtsLayoutBuilder` passes `hostName` into `AudioParameter*`.
- Existing DAW projects may show remapped automation labels after this change, while ParameterIDs stay stable so automation-by-ID continues to resolve.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---|---|---|
| Blind: inventory still named `*-draft.md` while treated as SSOT | `low` | Real naming confusion for agents only; product unaffected. Reject (cosmetic; rename is process hygiene). |
| Blind: Patch Mutator inventory symbols say `StandaloneWidgets` | `low` | Inventory metadata mismatch vs Int descriptors; IDs nest that way historically. Reject for this story. |
| Blind: no DAW label-remap user note | `medium` | Frozen I/O asks to document label remap. Spec Implementation Notes mention ID stability but lack an explicit DAW-facing remap warning. **patch**. |
| Blind + VG Other + Edge: `ApvtsValidator` / layout allow empty `hostName` | `medium` | Confirmed: validator only checks empty `displayName` (`ApvtsValidator.cpp`); layout passes `hostName` unchecked. Empty host leaf would ship to hosts. **patch**. |
| Blind: hard-coded `151` / `>= 149` in uniqueness tests | `low` | Brittle for future params but current counts match inventory; soft `>=149` intentionally excludes Patch Mutator from layout. Reject. |
| Blind: no markdown-SSOT vs code parse test | `medium` | Drift risk is real for developers; full markdown parser is non-trivial. **defer**. |
| Blind + VG: UI path never asserts `WidgetFactory::getParameterDisplayName` | `medium` | Pre-verified gap: Frequency test only checks descriptor fields; switching WidgetFactory to `hostName` would keep tests green while breaking UI AC. **patch**. |
| Blind: Matrix Modulation `.hostName` on same line as `.displayName` | `low` | Style-only; behavior correct. Reject. |
| Blind: host naming style mix / module echoes undocumented | `false` | Guillaume explicitly approved expanded names and intentional echoes; not a defect. |
| Blind: special-char truncation in short host `getName` limits untested | `maybe-false` | DAW truncation varies; multi-DAW intent rejects Live-first truncation hacks. **defer** (unverified host-limit collisions). |
| Blind: `CONVENTIONS.md` / project-context not updated | `low` | Spec Implementation Notes already record the rule; broader doc churn out of story urgency. **defer**. |
| Blind: spec still `in-progress` / verification unevidenced | `false` | Status already `in-review` at triage; Host parameter + APVTS suites and lint were run with 0 failures. |
| Edge: FREQUENCY/Mix tests null-deref after failed `findInt` | `medium` | `expect` does not abort; subsequent member access on nullptr can crash the suite. **patch**. |
| Edge: unmatched layout ParameterID silently skips hostName equality | `medium` | Confirmed fall-through with no `expect(false)`. **patch**. |
| VG Other: `ApvtsPatchMapperTests` bulk-flush still uses `freq.displayName` | `low` | Incomplete harness adoption; test does not assert the name string, but should mirror APVTS construction. **patch**. |

## Design Notes

**Host formula (approved intent):** `{ModuleHost} {LeafTitleCase}` where `ModuleHost` is the GUI module identity (Title Case / product tokens), and buses use `Bus N` even though APVTS group labels are currently `"0"`…`"9"`.

**Golden examples:**
- `DCO 1 Frequency` / `DCO 2 Frequency` / `VCF/VCA Frequency`
- `VCF/VCA DCO 2 | DCO 1 Mix`
- `Bus 3 Destination`
- `Vibrato Waveform`

**Storage:** Explicit `hostName` on descriptors (not derived at runtime from group path) so Live flattening cannot be “fixed” by accidentally changing UI strings, and so the inventory stays reviewable.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- build succeeds
- `ctest --preset macos-debug-arm64 -R 'Apvts|UndoManager|ModuleAction|Init' --output-on-failure` (adjust to available test filter) -- affected unit tests pass
- `python3 Scripts/quality/lint_touched.py` -- clean on touched C++ under `Source/` / `Tests/`

**Manual checks (if no CLI):**
- Standalone/plugin UI: DCO 1 / DCO 2 / VCF/VCA still show short `FREQUENCY`
- In a flat host parameter list: those three appear as distinct host names from the approved inventory
