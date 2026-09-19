---
title: 'Artefacts standalone no audio'
type: 'bugfix'
created: '2026-09-19'
status: 'done'
route: 'dispatch'
baseline_commit: 'a9f07d8902087bfdec2dee1de0cc859012160157'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-standalone-audio-input-passthrough.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The standalone Matrix-Control app plays synth monitor audio (AUDIO FROM) when launched from the local Builds tree, but is silent when the same app is launched from the Dropbox Artefacts copy. Observed: Scarlett 6i6 hardware input 1 meters active, AUDIO FROM correctly set to that input (same as Cursor launch), yet Matrix-Control peak meter stays dead and speakers stay silent.

**Approach:** Explain-and-wait — no code or Artefacts-path change in this pass. Keep Artefacts on Dropbox for multi-machine centralization. Guillaume notes the finding in personal docs and retests next week (including a Desktop copy of the Dropbox `.app` as a differential). Reopen only if the home retest confirms a product/packaging fix (e.g. mic usage plist) rather than path/TCC alone.

**Decisions:**
- Deliverable: Explain-and-wait (no repo code change; personal notes only).
- Symptom class: Peak-dead-passthrough-silent with AUDIO FROM correctly selected and hardware input active.
- Artefacts stay on Dropbox; Desktop copy is a temporary diagnostic only.

## Boundaries & Constraints

**Always:**
- Keep hosted VST3/AU without an input bus (AD-11 / Story R-1).
- Keep shared Application Support prefs (`Matrix-Control.settings`); do not make audio settings path-dependent.
- Prefer evidence from binary identity, signing, xattrs, and JUCE mute/passthrough plumbing over speculative rewrites of `StandalonePluginHolder`.
- Leave `ARTEFACTS_DIR_MACOS` on Dropbox unless Guillaume later asks to relocate.

**Never:**
- Rewrite JUCE `AudioDeviceManager` / `StandalonePluginHolder` plumbing to “fix Dropbox”.
- Change `audioFromSourceId` format or mute-unmute product behavior without a confirmed product bug.
- Assume Builds vs Artefacts are different builds without re-checking SHA-256 after the next copy.
- Harden mic plist or relocate Artefacts in this pass.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Local Builds launch | Same binary, local path, TCC previously allowed | Peak + passthrough when AUDIO FROM set and inputs enabled | N/A |
| Dropbox Artefacts launch | Same binary, File Provider path, possibly separate TCC | Peak dead + speakers silent while AUDIO FROM shows input 1 and hardware meters | Grant Mic/audio input for that path, or launch Desktop copy as differential |
| Desktop copy of Artefacts | Same binary copied off Dropbox | If audio returns: Dropbox path / TCC differential confirmed | If still silent: revisit mute prefs / missing mic usage string |
| Shared prefs | Same `audioSetup` / `audioFromSourceId` for both paths | Product routing identical; silence is launch-path or OS privacy | Do not invent per-path settings |

</frozen-after-approval>

## Code Map

- `CMakeLists.txt` (`COPY_TO_ARTEFACTS_DIR`, `copy_directory` Standalone `.app` → Dropbox Artefacts) — packaging only; binary identical to Builds after copy (SHA-256 match verified 2026-09-19).
- `Source/Standalone/MatrixControlStandaloneApp.cpp` — `appProperties` via `ProjectPaths::makeProductPropertiesFileOptions` → shared `~/Library/Application Support/Ten Square Software/Matrix-Control/`.
- `Source/Core/Audio/StandaloneAudioInputRouterStandalone.cpp` — `enableInputMonitoring` / `disableInputMonitoring` toggle JUCE `StandalonePluginHolder::getMuteInputValue()`.
- `Source/Core/PluginProcessorAudio.cpp` — `syncAudioPassthroughFromSourceId`; unmutes when `audioFromSourceId` non-empty.
- `Source/Core/Audio/AudioInputSourceCatalog.cpp` — catalog from active `inputChannels` bitset (compacted bus), not raw hardware indices.
- `juce_add_plugin` in `CMakeLists.txt` — no `MICROPHONE_PERMISSION_ENABLED` / `MICROPHONE_PERMISSION_TEXT` today; neither Builds nor Artefacts Info.plist has `NSMicrophoneUsageDescription`; no entitlements; adhoc linker-signed.
- Artefacts xattrs (Dropbox) — `com.apple.fileprovider.dir#N`, `com.dropbox.attrs`; Builds copy has no Dropbox File Provider attrs.
- Prior related fix — `_bmad-output/implementation-artifacts/spec-standalone-audio-input-passthrough.md` (muteInput / compacted bus); applies equally to both launch paths if prefs shared.

## Tasks & Acceptance

**Execution:**
- [x] Close this pass with no source/CMake change (Explain-and-wait).
- [x] Leave Implementation Notes with the home retest checklist for next week (Desktop copy + Privacy settings comparison).

**Acceptance Criteria:**
- Given this approved intent, when the pass completes, then the repository is unchanged for this issue and the frozen decisions record Explain-and-wait + confirmed peak-dead symptom + Dropbox Artefacts kept.
- Given Guillaume’s next-week retest, when he compares Builds vs Dropbox Artefacts vs Desktop copy under the same Scarlett / AUDIO FROM setup, then the outcome (audio returns on Desktop only, or not) decides whether to reopen for mic-plist hardening vs pure TCC grant on the Dropbox path.

## Implementation Notes

- 2026-09-19: Explain-and-wait closed with **no** `Source/` or `CMakeLists.txt` changes. Spec artifact only.
- Matrix rows are OS / launch-path differentials (TCC, Dropbox File Provider, Desktop copy). No automated unit tests apply; coverage is the home retest checklist below.
- Home retest checklist (next week):
  1. Builds standalone + Scarlett input 1 + AUDIO FROM → expect peak + speakers.
  2. Dropbox Artefacts `.app` same setup → compare (historically: peak dead, speakers silent).
  3. Copy Artefacts `.app` to Desktop and launch → if audio returns, path/TCC (or Dropbox) implicated.
  4. System Settings → Privacy & Security → Microphone: note Builds vs Artefacts path allow/deny.
  5. If Desktop still silent after mic allow: reopen for `MICROPHONE_PERMISSION_ENABLED` hardening (do not relocate Artefacts off Dropbox unless asked).

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence |
|---------|---------|----------|
| Blind: empty Spec Change Log / Review Triage Log need a dated close entry | false | Spec Change Log is for bad_spec loopbacks only (template). Review Triage Log is filled by this review step; empty-before-triage is expected. Fix would be editing this build's spec. |
| Blind: status in-progress conflicts with closed tasks | false | Workflow owns status: was `in-progress` during implement, now `in-review`, then `done` in present. Diff snapshot predates `in-review`. Not a product defect. |
| Blind: I/O matrix mixes healthy vs observed bug under Expected | false | Intentional for Explain-and-wait diagnostics; Artefacts silence is documented observed state, not acceptance target. Fix would be editing this build's spec. |
| Blind: SHA-256 truncated in Design Notes | false / low reject | Cosmetic for a no-code pass; full digest was verified in-session (`ca56ce73d28269e4e95ed55398e71acb7ca1c6d8d8f9e62cc8dc950fdc3adb58`). Fix would be editing this build's spec. |
| Blind: retest checklist omits xattr / Gatekeeper compare | false | Useful optional tip but out of frozen Explain-and-wait deliverable; Code Map already records Dropbox xattrs. Fix would be editing this build's spec. |
| Blind: retest omits mute / shared-prefs confirmation | false | Frozen symptom already includes correct AUDIO FROM + live Scarlett meters vs dead Matrix peak; prefs are shared across paths. Fix would be editing this build's spec. |
| Blind: reopen criteria incomplete for all outcome branches | false | Acceptance already branches Desktop restores vs still silent → mic-plist vs TCC. Extra branches are nice-to-have. Fix would be editing this build's spec. |
| Blind: missing deferred decision for mic-plist if retest inconclusive | false | Frozen Never defers hardening this pass; Approach says reopen after retest. Tracking another deferred decision is a spec edit. |
| Blind: no Console / TCC failure evidence in Verification | false | Manual Privacy Settings check already covers the primary differential; Console capture is optional. Fix would be editing this build's spec. |
| Edge Case Hunter: (none) | false | Empty finding set `[]` — no claim to verify. |
| Verification Gap: no gaps | false | Correct for Explain-and-wait with no Source/CMake/test changes; trusted as filed. |

## Design Notes

Investigation (2026-09-19) already ruled out “different binary / incomplete copy”: Builds and Artefacts executables share SHA-256 `ca56ce73…`, same Info.plist identity `com.TenSquareSoftware.Matrix-Control`, same adhoc signature, empty entitlements, identical file trees. Path-dependent audio code was not found; `ProjectPaths` affects repo/log roots, not `AudioDeviceManager`. Strongest remaining explanations are macOS TCC keyed by executable path for adhoc apps, and/or Dropbox File Provider execution quirks. Secondary gap: missing microphone usage description hurts any new launch path more than an already-allowed Builds path. User-confirmed symptom matches peak-dead / silent speakers with correct AUDIO FROM and live Scarlett input meters.

## Verification

**Manual checks (home retest):**
- Launch Builds standalone: expect peak + speakers with AUDIO FROM = Scarlett input 1.
- Launch Dropbox Artefacts standalone: compare peak / speakers under the same Settings and AUDIO FROM.
- Copy Artefacts `.app` to Desktop and launch: if audio returns, Dropbox path / TCC is implicated; if not, check System Settings → Privacy → Microphone for that app path and consider mic usage plist next.
- System Settings → Privacy & Security → Microphone: note whether Builds and Artefacts paths appear and whether each is allowed.
