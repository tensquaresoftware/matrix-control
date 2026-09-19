---
organization: Ten Square Software
project: Matrix-Control
title: Application Support Product Folder
author: BMad Agent
type: 'chore'
created: '2026-09-19'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '0af74217497983fd89aa366247ea9db117fcc8aa'
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/Source/Shared/ProjectPaths.h'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Standalone `Matrix-Control.settings` is written at the Application Support root, while Device Setup and Init templates already live under `Ten Square Software/Matrix-Control/`. That splits product files and will collide with future Ten Square Software apps.

**Approach:** Make the per-user company/product Application Support folder the single layout for all Matrix-Control system files (standalone settings included), reuse `ProjectPaths` as the path policy, and write the new standalone path directly — no migration and no legacy path code.

**Decisions:**
- **No legacy migration** — pre-release / single developer; do not copy, delete, or detect the old root-level `Matrix-Control.settings` in code. Orphan on disk may be removed manually outside the app.
- **Per-user storage** — keep system files under the OS user account application-data directory (`userApplicationDataDirectory` / PropertiesFile per-user roots), never the machine-wide shared location (`commonToAllUsers = false`).
- **Single product folder** — Init templates, Device Connection settings, Standalone settings, and the ProjectPaths product root all resolve to the same per-user company/product directory that PropertiesFile uses with `osxLibrarySubFolder = "Application Support"` (macOS: under Application Support; Windows: under `%APPDATA%`; Linux: under `~/Ten Square Software/Matrix-Control/`). Pre-existing Init under `~/Library/Ten Square Software/…` (without Application Support) is abandoned without migration.

## Boundaries & Constraints

**Always:**
- Keep Device Setup, Init, and Standalone settings under the same per-user `…/Ten Square Software/Matrix-Control/` product folder (PropertiesFile-aligned).
- Keep distinct filenames: `Matrix-Control.settings` (standalone audio/MIDI prefs) vs `Matrix-Control-DeviceConnection.settings` (per-user Device Setup defaults shared by plugin + Standalone on that account).
- Use one path policy shared by Standalone `PropertiesFile` options, Device Connection `folderName`, and `ProjectPaths::getApplicationDataDirectory()` (no duplicated company/product string literals; Init follows the same root).
- Align Linux Standalone with the same company/product folder (drop the standalone `~/.config` special-case).
- Keep `commonToAllUsers = false` (and equivalent) so each OS account has its own product folder.

**Never:**
- Add migration, fallback, or dual-path lookup for the old root-level standalone settings file or the old macOS Init path under `~/Library/Ten Square Software/` (without Application Support).
- Move or rename Computer Patches / user patch libraries.
- Change APVTS / host session prefs storage.
- Touch Matrix-Simulator or other products’ settings files in this repo change.
- Write system files to a machine-wide / all-users Application Support location.
- Introduce a second Application Support layout or browse UI for system files.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Fresh / post-change Standalone | Product folder missing or empty | Create per-user product folder; write `Matrix-Control.settings` under it | If create fails, JUCE PropertiesFile fails soft; no crash |
| Orphan root settings present | Root `Matrix-Control.settings` still on disk | Ignored by the app; new file written only under product folder | N/A (manual cleanup outside scope) |
| Device Setup / Init | Reads/writes after unify | Same product folder as Standalone `.settings` (Init under `…/Init/`) | N/A |
| Second OS user on same machine | Other account launches the app | Separate product folder under that account’s application data | N/A |

</frozen-after-approval>

## Code Map

- `Source/Shared/ProjectPaths.h` / `.cpp` — SSOT for `Ten Square Software` + `Matrix-Control` via `getApplicationDataDirectory()` (already per-user `userApplicationDataDirectory`); add a PropertiesFile `folderName` helper so Standalone and Device Connection share policy.
- `Source/Standalone/MatrixControlStandaloneApp.cpp:17-27` — empty `folderName` (macOS/Windows) and Linux `~/.config` special-case cause root-level `Matrix-Control.settings`; set `folderName` from ProjectPaths only; no migration helper.
- `Source/Core/Services/DeviceConnectionMachineDefaults.cpp:9-24` — already `folderName = "Ten Square Software/Matrix-Control"` and `commonToAllUsers = false`; switch `folderName` to ProjectPaths helper (on-disk path unchanged).
- `Source/Core/Init/InitTemplateWriter.cpp` + `ProjectPaths::getInitTemplatesDirectory()` — already under product folder `Init/`; do not relocate.
- `Tests/Unit/InitTemplateWriterTests.cpp` — asserts Init under `getApplicationDataDirectory()`; add a small ProjectPaths folder-name / product-root unit test if helpful.
- Out of scope: `~/Library/Application Support/Ten Square Software/Matrix-Simulator.settings` (sibling product).

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/ProjectPaths.h` / `.cpp` -- Expose a shared PropertiesFile folder-name helper from the existing company/product constants -- single path policy for all system files.
- [x] `Source/Standalone/MatrixControlStandaloneApp.cpp` -- Set `folderName` from that helper; drop Linux `~/.config` special-case; no legacy path code -- write standalone settings into the per-user product folder only.
- [x] `Source/Core/Services/DeviceConnectionMachineDefaults.cpp` -- Use the same folder-name helper; keep `commonToAllUsers = false` -- remove duplicated string literal without changing the on-disk Device Setup path.
- [x] `Tests/Unit/` -- Cover the path helper (product folder under user application data; folderName string) -- lock the layout without migration tests.

**Acceptance Criteria:**
- Given a Standalone launch on macOS/Windows/Linux, when application properties are stored, then `Matrix-Control.settings` is under the per-user company/product folder aligned with PropertiesFile (macOS: under Application Support; not Application Support root; not machine-wide).
- Given Device Setup machine defaults already on disk under that product folder, when the app reads/writes them after this change, then the same file path continues to be used.
- Given Init templates, when SAVE AS INIT / load runs, then they use `Init/` under the same product folder as the `.settings` files (not the old macOS `~/Library/Ten Square Software/…` path without Application Support).
- Given the codebase after this change, when searching for standalone settings path logic, then there is no dual-path / migrate-from-root / fallback-to-root code.

## Implementation Notes

- Unified `getFallbackRoot()` / `getApplicationDataDirectory()` via `makeProductPropertiesFileOptions(...).getDefaultFile().getParentDirectory()` so Init, Device Connection, Standalone settings, and fallback logs/`dev-project-root.txt` share one product folder.
- Added `makeProductPropertiesFileOptions` as shared Options factory (folderName, Application Support, per-user); Standalone and Device Connection both use it.
- macOS discovery: JUCE `userApplicationDataDirectory` is `~/Library` (not Application Support); previous Init lived under `~/Library/Ten Square Software/Matrix-Control/` while PropertiesFile used Application Support — fixed by derive-from-PropertiesFile.
- Empty `getDefaultFile()` on Windows returns empty product root (guarded).
- No migration of orphan `~/Library/Ten Square Software/Matrix-Control/` or root `Matrix-Control.settings`.
- Tests resolve paths via `getDefaultFile()` only (no live PropertiesFile I/O / no Init create side effects).
- Verified: build macos-debug-arm64; unit tests ProjectPaths / InitTemplateWriter / DeviceConnectionMachineDefaults (0 failures); lint_touched.py clean.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| Blind: tests construct live PropertiesFile / create Init | medium | Real — tests touched Application Support. **patch** — switched to `getDefaultFile()` and path-only Init assert. |
| Blind: relocates `dev-project-root.txt` / fallback logs | false | Intentional unify decision (no migration); orphan old tree is manual cleanup. |
| Blind: Linux layout not locked in tests | medium | Real gap. **patch** — `#if JUCE_LINUX` assert not under `/.config/`. |
| Blind / VG: Standalone wiring not covered by tests | medium | Real — ProjectPathsTests never saw Standalone options. **patch** — shared Options factory + assert Standalone/Device Connection resolve under product root. |
| Blind: Options fields still copy-pasted | medium | Real drift risk. **patch** — `makeProductPropertiesFileOptions`. |
| Blind: Init child assert nearly tautological | low | Rejected alone; strengthened as part of path-only Init assert under unified root. |
| Blind: `getInitTemplatesDirectory` mutates FS in tests | medium | Same root as live-PropertiesFile finding. **patch** applied. |
| Blind: header comment omits Init / fallback consumers | low | **patch** — comment updated to name Init + shared stores. |
| Blind: depends on JUCE `getDefaultFile` without independent fixture | false | Matching PropertiesFile is the SSOT; independent fixture would drift. |
| Blind: Standalone vs Device Connection `createDirectory` asymmetry | false | PropertiesFile creates parents on save; Device Connection pre-create is harmless belt-and-suspenders. |
| Edge: empty Windows `getDefaultFile` | medium | Real rare path. **patch** — guard returns empty File. |
| Edge: Init test hits live Application Support | medium | Same as Blind test side-effect. **patch** applied. |
| VG: Device Connection store path not observed | medium | Real — logic-only DeviceConnection tests. **patch** — options resolve under `getApplicationDataDirectory()` via shared factory. |

No deferrals. No intent_gap / bad_spec loopback.
## Design Notes

**Why company then product:** Matches common desktop convention (`Vendor/Product/`) and matches Device Setup and Init. Future Ten Square apps get sibling folders under the same company directory.

**Per-user, not all-users:** Audio/MIDI device choice and Device Setup defaults are personal. PropertiesFile with `commonToAllUsers = false` keeps each OS account isolated. Machine-wide shared data is out of scope.

**Do not merge the two `.settings` files:** Standalone audio device prefs and Device Setup defaults have different lifetimes and consumers (Standalone-only vs plugin+Standalone). Keep separate filenames in the same folder.

**No migration code:** Pre-release; write the new path only. Any orphan root or old Init folder is manual cleanup.

**Linux:** JUCE PropertiesFile places `folderName` under `~/` (not `~/.config`). Unified product root is therefore `~/Ten Square Software/Matrix-Control/`, matching Device Connection / Standalone.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `ctest --preset macos-debug-arm64 -R 'ProjectPaths|InitTemplate|DeviceConnection'` (or equivalent unit target filter after new tests) -- expected: relevant unit tests pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ files

**Manual checks:**
- Launch Standalone once; confirm `Matrix-Control.settings` appears under `~/Library/Application Support/Ten Square Software/Matrix-Control/` and Device Connection / Init files still there.
- Confirm the app does not read or rewrite the orphan root-level `Matrix-Control.settings` if it still exists.
