---
organization: Ten Square Software
project: Matrix-Control
title: Story 11.2 — CD Release Pipeline
author: BMad Agent
status: review
epic: 11
story: 2
story_key: 11-2-cd-release-pipeline
depends_on: [11-1]
blocks: []
implementation_order: 2
correct_course_date: 2026-07-11
baseline_commit: 0eaee973
baseline_workflow: .github/workflows/build-and-test.yml
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/sprint-change-proposal-2026-07-11-ci-infrastructure.md
  - implementation-artifacts/11-1-ci-multi-platform-build-and-tests.md
  - Luthier 10-2-cd-release-pipeline (external reference)
created: 2026-07-11
updated: 2026-07-11
---

# Story 11.2: CD Release Pipeline

Status: done

<!-- Epic 11 — CI & Release Infrastructure. Originally deferred pre-v1.0.0; story created 2026-07-11 for implementation planning. -->

## Story

As a maintainer,
I want pushing a semver git tag to trigger multi-OS **Release** plugin builds and a GitHub Release with downloadable artefacts,
So that v1.0.0 distribution does not require manual per-OS packaging, signing, and upload.

## Context

**Correct Course 2026-07-11:** Story **11.1** delivers Debug build + unit tests on push/PR. Distribution is still **manual**: local Release presets, personal `ARTEFACTS_DIR_*` paths in `CMakeUserPresets.json`, and ad-hoc copy to Dropbox.

**Target workflow:** bump `project(Matrix-Control VERSION …)` in `CMakeLists.txt` → commit → `git tag vX.Y.Z[-suffix]` → `git push origin vX.Y.Z[-suffix]` → CI builds Release binaries, packages per-OS zips, signs/notarizes macOS, and publishes a GitHub Release.

**Luthier reference (Story 10.2):** Tag-triggered `.github/workflows/release.yml`; packaging logic in `publish/prepare-release.py`; **`publish-ci`** subcommand that calls `gh release create` only (tag already exists — no tag recreation, no clean-tree check). **Adapt for JUCE:** Matrix-Control has no `publish/` tree yet; plugin artefacts are AU/VST3/Standalone bundles, not PyInstaller zips; **macOS codesign + notarization are in scope** (Luthier explicitly deferred signing).

**Originally deferred because:** AU/VST3 codesign, macOS notarization, and v1 distribution packaging are substantially harder than Luthier's Python app. Story is now ready-for-dev — **implementation may still wait until v1.0.0 release planning** if Apple Developer credentials or Epic U-10 release gate are not satisfied.

**Planning references:**
- `_bmad-output/planning-artifacts/sprint-change-proposal-2026-07-11-ci-infrastructure.md` §4.3
- `_bmad-output/planning-artifacts/epics.md` — Epic 11 Story 11.2
- Luthier: `10-2-cd-release-pipeline.md` (GitHub: tensquaresoftware/luthier)
- Pamplejuce / Melatonin CI signing patterns (external — see Latest Tech Information)

### Release constraints (Matrix-Control specific)

| Concern | CD approach |
|---------|-------------|
| Version SSOT | `project(Matrix-Control VERSION X.Y.Z)` in `CMakeLists.txt` → `MATRIX_CONTROL_PROJECT_VERSION` compile define → `PluginVersion::getVersionString()` |
| Pre-release suffix | `MATRIX_CONTROL_PRERELEASE_SUFFIX` CACHE (default `"alpha"`) — **must be empty string for stable v1.0.0**; for `v1.0.0-rc1` set suffix to `rc1` or derive from tag |
| Tag convention | Existing repo tags use **`v` prefix** (`v0.0.66-alpha-pre-bmad`) — **keep `v` prefix**; validate tag (strip `v`) matches `PROJECT_VERSION` + optional suffix |
| Build type | **Release** presets only (`macos-release-*`, `windows-release`, `linux-release`) — not Debug |
| Formats per OS | macOS: AU + VST3 + Standalone; Windows/Linux: VST3 + Standalone (AU stripped by CMake on non-Apple) |
| macOS arch (CI) | `macos-latest` = Apple Silicon → default **`macos-release-arm64`**; document optional **`macos-release-universal`** if v1 ships fat binary |
| Copy side effects | `-DUSER_COPY_TO_SYSTEM_FOLDERS=OFF -DUSER_COPY_TO_ARTEFACTS_DIR=OFF` (same as CI 11.1) |
| Tests gate | Run `Matrix-Control_Tests` on each matrix leg **before** packaging (Release build) |
| Signing | macOS: Developer ID Application + `notarytool` (**required for public macOS distribution**); Windows: **optional v1** — document unsigned fallback or defer Azure Trusted Signing |
| Release binary hygiene | Epic U Story U-10 (`TestComponent` excluded from Release) is the **v1.0.0 release gate** — CD must build Release config; verify sandbox not in Release binary before first public tag |

## Acceptance Criteria

### AC1 — Tag trigger only

**Given** a semver tag push matching `v*.*.*` (e.g. `v1.0.0`, `v1.0.0-rc1`, `v0.2.0-alpha`)  
**When** GitHub Actions runs  
**Then** `.github/workflows/release.yml` triggers on `push: tags:` only — **not** on push/PR to `main`  
**And** **`workflow_dispatch` is out of scope** (match Luthier 10.2 — first validation is a real tag push)

### AC2 — Release build matrix

**Given** the release workflow  
**When** build jobs run  
**Then** jobs execute on `macos-latest`, `windows-latest`, and `ubuntu-latest`  
**And** each leg checks out JUCE **8.0.12**, configures with **Release** preset, `MATRIX_BUILD_TESTS=ON`, copy flags OFF  
**And** builds `Matrix-Control` (all format targets) + `Matrix-Control_Tests`, runs tests, then packages artefacts  
**And** `fail-fast: false` and generous `timeout-minutes` (Release + signing legs exceed Debug CI)

### AC3 — Per-OS artefact packages

**Given** a successful Release build on each OS  
**When** packaging runs  
**Then** each leg produces a versioned zip uploaded as a workflow artifact:

| OS | Zip contents (minimum) |
|----|------------------------|
| macOS | `Matrix-Control.vst3`, `Matrix-Control.component`, `Matrix-Control.app` |
| Windows | `Matrix-Control.vst3/`, `Matrix-Control.exe` |
| Linux | `Matrix-Control.vst3/`, `Matrix-Control` standalone binary |

**And** zip names follow a consistent pattern, e.g. `Matrix-Control-v{VERSION}-macOS-{arch}.zip`  
**And** packaging logic lives in **`Scripts/release/`** (or `publish/`) — **not** duplicated as raw shell in YAML beyond thin wrappers

### AC4 — macOS codesign and notarization

**Given** macOS build artefacts and repository secrets configured  
**When** the macOS leg completes build  
**Then** AU, VST3, and Standalone bundles are signed with **Developer ID Application** (`--options=runtime --timestamp`)  
**And** signed bundles are submitted via **`xcrun notarytool submit --wait`** and stapled (or documented zip-notarize flow)  
**And** workflow **fails clearly** if signing secrets are missing (no silent skip of notarization on a stable release tag)  
**And** required secrets are documented in `CONTRIBUTING.md` (names only — never values)

### AC5 — Publish job and GitHub Release

**Given** all three build jobs succeed  
**When** the publish job runs  
**Then** it downloads per-OS zip artifacts  
**And** generates `SHA256SUMS.txt` (and optionally `RELEASE_NOTES.md` from tag message or template)  
**And** runs **`publish-ci`** (script subcommand) to invoke `gh release create` with all assets attached  
**And** workflow has `permissions: contents: write`

### AC6 — publish-ci (no tag recreation)

**Given** the tag already exists on the remote when the workflow runs  
**When** `publish-ci` executes  
**Then** it does **not** create, move, or push a git tag  
**And** it does **not** require a clean working tree  
**And** it invokes `gh release create` (idempotent or fail-with-clear-message on re-run — see Luthier 10.2 review patch)

### AC7 — Prerelease detection

**Given** tag `v1.0.0-rc1` (or any `vX.Y.Z-<suffix>` where suffix is not `alpha`/`beta` alone — see Dev Notes)  
**When** GitHub Release is created  
**Then** it is marked **`--prerelease`**  
**Given** tag `v1.0.0` with empty `MATRIX_CONTROL_PRERELEASE_SUFFIX`  
**Then** stable (non-prerelease) release

### AC8 — Version/tag alignment

**Given** the tagged commit  
**When** `validate-tag` job runs (before or as first step of matrix)  
**Then** semver parsed from tag (strip leading `v`) matches `project(Matrix-Control VERSION …)` in `CMakeLists.txt` at that commit  
**And** pre-release suffix in tag aligns with `MATRIX_CONTROL_PRERELEASE_SUFFIX` when non-empty  
**And** mismatch **fails the workflow** before expensive builds

### AC9 — Documentation

**Given** the workflow is merged  
**When** a maintainer reads **`CONTRIBUTING.md`**  
**Then** a **Releasing** section documents: version bump, suffix clearing, tag format, required secrets, RC → smoke test → stable tag flow  
**And** **`README.md`** mentions tag-triggered releases (link to Releases page)  
**And** manual local release steps remain documented as fallback

### AC10 — Tests

**Given** story completion  
**Then** unit tests cover: prerelease detection helper, tag/version parsing, `publish-ci` command assembly (no git tag creation)  
**And** workflow YAML structure is validated in tests (parse jobs, required steps — match Luthier 10.2 test pattern)  
**And** existing `Matrix-Control_Tests` still pass in CI Debug workflow (11.1 regression)

## Tasks / Subtasks

- [x] Create `Scripts/release/prepare_release.py` (or `publish/prepare-release.py`) — Luthier 10.2 port (AC: 3, 5, 6, 7)
  - [x] `pack` — collect JUCE artefact paths into versioned zip per OS
  - [x] `finalize` — merge artifacts, write `SHA256SUMS.txt`, optional release notes
  - [x] `publish-ci` — `gh release create` only; `--yes` for CI
  - [x] `is_prerelease_tag()` / `parse_version_from_tag()` helpers
  - [x] Shared helper extracted from any future local `publish` flow (avoid YAML duplication)

- [x] Create `.github/workflows/release.yml` (AC: 1, 2, 5, 8)
  - [x] `on.push.tags: ['v*.*.*']` — document that filter is broad; `validate-tag` job enforces semver
  - [x] `permissions: contents: write`; `cancel-in-progress: false`
  - [x] `validate-tag` job: regex + `CMakeLists.txt` version extract
  - [x] Matrix build: JUCE cache, Linux apt deps (reuse 11.1 list), Release configure/build/test/package
  - [x] macOS job: import certs → keychain → codesign → notarize → staple → zip
  - [x] `actions/upload-artifact` per platform; publish job: download → finalize → publish-ci

- [x] CMake / version hygiene (AC: 8)
  - [x] Document Release configure flag: `-DMATRIX_CONTROL_PRERELEASE_SUFFIX=""` for stable releases
  - [x] Optional: CI sets suffix from tag via `-D` override (avoid manual CMake edit for RC tags)

- [x] Repository secrets checklist (AC: 4, 9) — Guillaume configures in GitHub Settings
  - [x] `DEV_ID_APP_CERT`, `DEV_ID_APP_PASSWORD`, `DEVELOPER_ID_APPLICATION`
  - [x] `NOTARIZATION_USERNAME`, `NOTARIZATION_PASSWORD` (app-specific), `APPLE_TEAM_ID`
  - [x] (Optional v1) Windows Authenticode secrets

- [x] Update `CONTRIBUTING.md` — Releasing section (AC: 9)
- [x] Update `README.md` — release badge or Releases link (AC: 9)
- [x] Add tests under `Tests/` or `Scripts/release/tests/` (AC: 10)
- [x] Dry-run validation: push **`v0.2.0-alpha`** test tag (or RC) after secrets configured; confirm GitHub Release assets — **deferred to maintainer post-merge** (AC1 forbids `workflow_dispatch`; requires GitHub secrets + real tag push)

### Review Findings

- [x] [Review][Patch] Ajouter les tests `Scripts/release/tests/` dans `build-and-test.yml` (décision #1 : exécution CI) [.github/workflows/build-and-test.yml]
- [x] [Review][Patch] Notarisation macOS : staple sur les bundles originaux après soumission d'un meta-zip [.github/workflows/release.yml:237-245]
- [x] [Review][Patch] Aucun test unitaire pour `validate_tag_against_cmake` (alignement tag ↔ CMakeLists) [Scripts/release/tests/test_prepare_release.py]
- [x] [Review][Patch] Pas de test pour le chemin idempotent `publish-ci` quand la release GitHub existe déjà [Scripts/release/tests/test_prepare_release.py]
- [x] [Review][Patch] `verify_release` n'exige pas les 3 archives plateforme avant publication [Scripts/release/prepare_release.py:295]
- [x] [Review][Patch] Re-publication : notes de release non mises à jour quand la release GitHub existe déjà [Scripts/release/prepare_release.py:402]
- [x] [Review][Patch] Bundles vides non détectés dans `discover_artefact_paths` [Scripts/release/prepare_release.py:171]
- [x] [Review][Patch] Tests YAML workflow incomplets (AC10 : JUCE pin, copy flags, timeouts, étapes sign/notarize) [Scripts/release/tests/test_prepare_release.py]
- [x] [Review][Patch] Ajouter `__pycache__/` au `.gitignore` pour éviter commit accidentel des caches Python [/.gitignore]

- [x] [Review][Defer] Dry-run E2E (push tag réel + assets GitHub Release) — deferred, post-merge maintainer (déjà documenté story L187)
- [x] [Review][Defer] Windows Authenticode non signé pour v1 — deferred, documenté CONTRIBUTING (choix spec)
- [x] [Review][Defer] macOS arm64-only en CI (pas universal) — deferred, décision spec / Dev Notes
- [x] [Review][Defer] Pins pytest/PyYAML sans borne supérieure — deferred, dette mineure reproductibilité

## Dev Notes

### JUCE Release artefact paths (reference — verify after configure)

Paths follow JUCE CMake layout under preset `binaryDir`:

| OS | Preset (CI default) | VST3 | AU | Standalone |
|----|---------------------|------|-----|------------|
| macOS | `macos-release-arm64` | `Builds/macOS/ARM/Release/Matrix-Control_artefacts/Release/VST3/Matrix-Control.vst3` | `…/AU/Matrix-Control.component` | `…/Standalone/Matrix-Control.app` |
| Windows | `windows-release` | `Builds/Windows/Matrix-Control_artefacts/Release/VST3/Matrix-Control.vst3` | — | `…/Standalone/Matrix-Control.exe` |
| Linux | `linux-release` | `Builds/Linux/Release/Matrix-Control_artefacts/Release/VST3/Matrix-Control.vst3` | — | `…/Standalone/Matrix-Control` |

**Do not hard-code** without `test -f` preflight (11.1 review lesson). Prefer discovering paths from CMake output or a small `--print-paths` script flag.

### Tag vs Luthier difference

| | Luthier 10.2 | Matrix-Control 11.2 |
|---|-------------|---------------------|
| Tag format | `1.0.0-rc1` (no `v`) | `v1.0.0-rc1` (**keep existing convention**) |
| Version file | `app/version.py` | `CMakeLists.txt` `project(… VERSION …)` |
| Artefacts | Single PyInstaller zip / OS | AU + VST3 + Standalone bundles / OS |
| Signing | Out of scope | **macOS in scope** |
| Build type | N/A (Python) | CMake **Release** |

### Prerelease policy

- Tags like `v0.2.0-alpha` match historical alpha releases — treat as **prerelease** on GitHub.
- Stable public v1: tag `v1.0.0` + `MATRIX_CONTROL_PRERELEASE_SUFFIX=""` at configure time.
- Implement `is_prerelease_tag()` conservatively: any tag with suffix after `X.Y.Z` → prerelease; bare `vX.Y.Z` → stable only if suffix cache empty.

### CI workflow relationship

| Workflow | Trigger | Build | Purpose |
|----------|---------|-------|---------|
| `build-and-test.yml` (11.1) | push/PR → `main` | Debug + tests | Merge gate |
| `release.yml` (11.2) | tag `v*.*.*` | Release + tests + pack + sign | Distribution |

Do **not** merge release workflow into 11.1 — separation keeps PR CI fast (Story 11.3 optimizes 11.1 only).

### Architecture compliance

- **No Core/GUI code changes required** for basic CD — infrastructure-only story.
- **Do not** commit signing certificates or notarization passwords.
- **Do not** enable `COPY_TO_ARTEFACTS_DIR` with personal Dropbox paths in CI.
- Version displayed in About modal (`PluginVersion.cpp`) must match released tag — already driven by CMake defines.

### Previous story intelligence (11.1)

- Reuse JUCE 8.0.12 cache pattern from `build-and-test.yml`.
- Reuse Linux apt package list verbatim.
- Windows: `windows-release` preset (VS 2026) — same generator family as Debug CI.
- `fail-fast: false` on matrix; job `timeout-minutes` mandatory (Release legs are slower).
- Explicit `shell: bash` on Windows for configure/build where applicable.
- Test binary preflight: `test -f` before execute.
- 11.1 explicitly scoped **out** release workflow — do not regress Debug CI when adding `release.yml`.

### Git intelligence (recent)

- `0eaee97` — Story 7-5 review fixes (unrelated to CD; no conflict).
- `ab0a263` / PR #20 — Story 11.1 merged: `.github/workflows/build-and-test.yml`, CMake copy-flag CACHE fix.
- First CD implementation should branch from current `main` with 11.1 CI green.

### Latest tech information

- **macOS signing in CI:** Import `.p12` into ephemeral keychain; `codesign --force -s "$DEVELOPER_ID_APPLICATION" --options=runtime --timestamp` on each bundle ([Melatonin guide](https://melatonin.dev/blog/how-to-code-sign-and-notarize-macos-audio-plugins-in-ci/)).
- **Notarization:** Prefer `xcrun notarytool submit` with app-specific password + `--wait`; staple with `xcrun stapler staple`. Zip bundles before submit if using zip workflow.
- **Reference implementation:** [Pamplejuce](https://github.com/sudara/pamplejuce) GitHub Actions + `sudara/basic-macos- codesign` action patterns (adapt to Matrix-Control secret names).
- **Windows v1:** Unsigned VST3/Standalone is common for open-source MIT plugins; Authenticode via Azure Trusted Signing is optional follow-up — document in CONTRIBUTING if deferred.
- **JUCE 8.0.12:** Pin same tag as 11.1; no JUCE upgrade in this story.

### Project context reference

- Tags: annotated, `v0.0.xx-alpha[-suffix]` format (`project-context.md` § Git & Release Conventions).
- Distribution v1: **free MIT** + GitHub Releases (`brief` addendum § Distribution).
- Target release: Christmas 2026 aspirational — CD pipeline enables RC tags before v1.0.0.

### Out of scope

- Story **11.3** CI build-time optimizations (Debug PR workflow only)
- Linux VST3/Standalone **format enablement** in README (still 🔜) — CD can still build Linux targets if CMake produces them
- Windows `.msi` / macOS `.pkg` **installers** (raw zip sufficient for v1 — match brief open-source distribution)
- **`workflow_dispatch`** manual releases
- App Store / AAX / CLAP
- Automated hardware smoke tests post-release

### Open decisions (maintainer — resolve during dev-story)

1. **macOS universal vs arm64-only** for public v1 download — CI default arm64; universal requires `macos-release-universal` preset and longer build.
2. **Windows signing** — required for v1 or defer with documented unsigned build?
3. **First validation tag** — `v0.2.0-alpha` smoke vs wait for `v1.0.0-rc1` after Epic U-10.
4. **Release notes source** — git tag annotation vs `CHANGELOG.md` vs template file.

## Dev Agent Record

### Agent Model Used

Composer (Cursor)

### Debug Log References

- Release script tests: 19 passed (`python3 -m pytest Scripts/release/tests/ -q`)
- Local `Matrix-Control_Tests` regression: passed on macOS Debug binary
- `validate-tag v0.1.1-alpha` OK against current `CMakeLists.txt` (VERSION 0.1.1, suffix alpha)

### Completion Notes List

- ✅ AC1–10 implemented: tag-only `release.yml`, 3-OS Release matrix + tests, macOS sign/notarize (fail if secrets missing), `Scripts/release/prepare_release.py` with pack/finalize/publish-ci, CONTRIBUTING Releasing section, README Releases link.
- ✅ CI passes `-DMATRIX_CONTROL_PRERELEASE_SUFFIX` from tag suffix at configure time (AC8 hygiene).
- ✅ Windows signing deferred for v1 — documented unsigned fallback in CONTRIBUTING.
- ✅ macOS CI default: arm64-only (`macos-release-arm64`); universal documented as optional.
- ⏳ E2E dry-run (push test tag + confirm GitHub Release assets) pending maintainer after GitHub secrets configured — procedure in CONTRIBUTING § Releasing.

- ✅ Code review 2026-07-11: 9 patches applied (notarization per-bundle, verify_release hardening, CI release-script tests, expanded unit tests, .gitignore).

### File List

- `.github/workflows/release.yml` (added)
- `.github/workflows/build-and-test.yml` (modified — release-script-tests job)
- `Scripts/release/prepare_release.py` (added)
- `Scripts/release/templates/RELEASE_NOTES.template.md` (added)
- `Scripts/release/tests/test_prepare_release.py` (added)
- `Scripts/release/requirements-test.txt` (added)
- `CONTRIBUTING.md` (modified)
- `README.md` (modified)
- `.gitignore` (modified)
- `_bmad-output/implementation-artifacts/sprint-status.yaml` (modified)

## References

- [Source: `_bmad-output/planning-artifacts/epics.md` — Epic 11 Story 11.2]
- [Source: `_bmad-output/planning-artifacts/sprint-change-proposal-2026-07-11-ci-infrastructure.md` §4.3]
- [Source: `_bmad-output/implementation-artifacts/11-1-ci-multi-platform-build-and-tests.md`]
- [Source: `.github/workflows/build-and-test.yml`]
- [Source: `CMakeLists.txt` — `project(VERSION)`, copy flags, `juce_add_plugin`, artefact copy L614–733]
- [Source: `CMakeUserPresets.json` — Release presets]
- [Source: `Source/Shared/Definitions/PluginVersion.cpp`]
- [External: Luthier `10-2-cd-release-pipeline.md` — publish-ci pattern]
- [External: Melatonin — codesign/notarize in CI]
- [External: Pamplejuce — JUCE release workflow reference]

## Change Log

- 2026-07-11 — Story created via create-story workflow: CD release pipeline spec adapted from Luthier 10.2 for JUCE AU/VST3/Standalone + macOS signing.
- 2026-07-11 — Story 11.2 implemented: release workflow, prepare_release.py, docs, unit tests.
- 2026-07-11 — Code review: 9 patches applied; story closed.
