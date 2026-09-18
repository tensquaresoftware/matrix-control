---
title: 'Settings DEVICE section and EPROM TYPE'
type: 'feature'
created: '2026-09-18'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
context: []
baseline_commit: 'c496c035e1391771c21b3ce53facfa2e423fe157'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** HARDWARE LATENCY sits under MASTER even though it is about the host/device path, not Master Edit. Firmware capability (Unison Detune later, SysEx delay now) cannot be inferred reliably from the 4-character Device Inquiry version string.

**Approach:** Add a DEVICE Settings section (HARDWARE LATENCY + EPROM TYPE), reorder sections to DEVICE → PATCH → PATCH MUTATOR → MASTER, compact vertical spacing so the modal stays non-scrolling, and make the user-chosen EPROM TYPE the SSOT for optimised vs stock SysEx delay. On the first successful Device Inquiry only, show an explanatory modal (not a silent write) so the user confirms or picks EPROM TYPE.

## Boundaries & Constraints

**Always:**
- Section order for this chantier: DEVICE → PATCH → PATCH MUTATOR → MASTER (INTERFACE section is deferred).
- EPROM TYPE items: FACTORY, GLIGLI, TAUNTEK, UNTERGEEK, UNKNOWN.
- Default and capability-safe fallback: UNKNOWN behaves like FACTORY (stock delay; no future advanced features).
- TAUNTEK and UNTERGEEK share the same capability wiring for now (both → optimised delay).
- Matrix-1000 combo: all five items. Matrix-6/6R combo: FACTORY, TAUNTEK, UNTERGEEK, UNKNOWN (no GLIGLI).
- First successful Device Inquiry with prompt flag unset → show a one-time explanatory modal (message thread / editor). Never silently write EPROM TYPE from Inquiry.
- Modal contains: short ASCII explanation, device-aware EPROM TYPE combo preselected to the Inquiry suggestion when mapped (else current value / UNKNOWN), buttons **OK** (apply the combo selection) and **LATER** (dismiss without changing the stored EPROM TYPE — usually still UNKNOWN). Closing either way sets the prompt-done flag so the dialog never returns.
- Inquiry version may only suggest a combo default inside that modal; it is never authoritative afterward.
- HARDWARE LATENCY remains plugin-only; standalone DEVICE may show only EPROM TYPE.
- Persist Settings via APVTS `state` int properties (same pattern as other Settings combos). Keep existing Settings persistence string keys stable.
- Display strings ASCII-only in `PluginDisplayNames`.
- Compact layout: reduce row gap and section-title-to-separator gap; recompute `kDesignHeight`; no general modal scroll; no vertical tabs in this chantier.

**Never:**
- INTERFACE section / CONTEXTUAL HELP (deferred).
- Unison Detune / CC 94 behavior (deferred).
- Silent auto-set of EPROM TYPE from Device Inquiry.
- Treat Device Inquiry version as authoritative EPROM identity after the one-time prompt.
- Guess EPROM class for live SysEx delay from the 4-char version substring (replace with Settings SSOT).
- Hide EPROM TYPE entirely on Matrix-6/6R.
- Move Settings prefs to ApplicationProperties or new AudioParameters.
- French in source/UI strings.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Fresh install | No EPROM property | Default UNKNOWN; stock SysEx delay; prompt flag false | N/A |
| User picks TAUNTEK in Settings | Combo change | Persist id; optimised delay for current device family | Clamp invalid ids on restore |
| User picks UNKNOWN | Combo = UNKNOWN | Stock delay (same as FACTORY) | N/A |
| First Inquiry M-1000 `1.20` | Prompt flag false | Modal opens; combo defaults to TAUNTEK; OK applies selection; LATER leaves stored value unchanged; flag set either way | Ambiguous version → combo defaults UNKNOWN |
| First Inquiry M-1000 `1.21` | Prompt flag false | Modal; default UNTERGEEK | N/A |
| First Inquiry M-1000 `1.16` | Prompt flag false | Modal; default GLIGLI | N/A |
| First Inquiry M-1000 `1.11` / `1.09` | Prompt flag false | Modal; default FACTORY | N/A |
| First Inquiry M-6/6R `2.15` | Prompt flag false | Modal; default TAUNTEK (no GLIGLI item) | N/A |
| Second Inquiry / reconnect | Prompt flag true | No modal; EPROM TYPE unchanged | N/A |
| User already set type in Settings before first Inquiry | Prompt flag still false | Still show one-time modal; combo prefers Inquiry suggestion when mapped, else current stored value; OK/LATER as above | Always once when flag false |
| Device type → Matrix-6 | Settings open / type change | Rebuild EPROM items without GLIGLI; if current was GLIGLI, coerce to UNKNOWN | Refresh combo without crash |
| Device type → Matrix-1000 | Was on 6/6R | Rebuild full item list; keep stored id if still valid | N/A |
| Plugin mode | DEVICE section | HARDWARE LATENCY + EPROM TYPE | N/A |
| Standalone | DEVICE section | EPROM TYPE only (no empty latency row) | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Settings/SettingsPanel.h` / `.cpp` / `SettingsPanelSetup.cpp` — sections, layout metrics (`kRowGap_` 12, `kSectionTitleGap_` 4, `kDesignHeight` 456), MASTER owns latency today; add DEVICE + eprom combo; compact gaps; reorder `layoutContent`.
- `Source/GUI/PluginEditorSettings.cpp` — restore/wire Settings combos + latency; extend for EPROM TYPE + device-type item refresh.
- Existing project confirmation / system-style modal pattern (reuse Look + always-on-top plugin rules) — one-time EPROM TYPE prompt UI owned by editor, not Core.
- `Source/Shared/Definitions/PluginDisplayNames.h` — DEVICE / EPROM TYPE strings, modal title/body/buttons (ASCII).
- `Source/Shared/Definitions/PluginIDs.h` — `kEpromType` + item ints + `kEpromTypePromptDone` (bool/int flag).
- `Source/Core/PluginProcessor*.cpp` — initialize new APVTS properties.
- `Source/Core/MIDI/Queue/SysExDelayProfile.{h,cpp}` — stop live delay from version substring; build profile from Settings EPROM class + device family.
- `Source/Core/MIDI/MidiManagerDeviceInquiry.cpp` — on success: apply delay from Settings SSOT; signal “prompt needed” when flag unset (do not write EPROM TYPE in Core).
- New small Core helper — map type→`EpromClass`, Inquiry version→suggested type, valid items per device family; unit-tested.
- `Tests/Unit/SysExDelayProfileTests.cpp` (+ policy tests) — Settings/policy-driven delay; keep family delay ms coverage.
- Reuse: APVTS Settings combo pattern; `DeviceTypeRegistry::fromApvtsProperty`; existing confirmation modal chrome.
- Do not change: existing Settings persistence key strings; HardwareLatency→host `setLatencySamples`; footer `deviceVersion` display; Defrag placeholder.

## Tasks & Acceptance

**Execution:**
- [x] `PluginIDs.h` + `PluginDisplayNames.h` — DEVICE / EPROM TYPE ids and ASCII strings; modal title, body, OK / LATER.
- [x] New Core policy (+ tests) — capability map, device-family item sets, Inquiry suggestion table (`1.16`→GLIGLI, `1.20`→TAUNTEK, `1.21`→UNTERGEEK, `1.11`/`1.09`→FACTORY, M-6/6R `2.15`→TAUNTEK; else none).
- [x] `SysExDelayProfile` + `MidiManagerDeviceInquiry` — delay SSOT from Settings; on first success with flag unset, request editor prompt (no silent property write).
- [x] Editor one-time EPROM TYPE modal — combo + OK / LATER; set prompt-done; persist choice; refresh delay profile.
- [x] `SettingsPanel*` + `PluginEditorSettings.cpp` + processor init — DEVICE section, move latency, device-aware combo, compact metrics, recompute `kDesignHeight`.
- [x] Unit tests + CMake for policy / delay; lint touched C++.

**Acceptance Criteria:**
- Given Settings open, when viewing sections, then order is DEVICE → PATCH → PATCH MUTATOR → MASTER with no INTERFACE yet.
- Given plugin mode, when DEVICE is shown, then HARDWARE LATENCY is under DEVICE (not MASTER) and still updates host latency.
- Given standalone, when DEVICE is shown, then only EPROM TYPE appears in that section.
- Given Matrix-1000, when opening EPROM TYPE, then items are FACTORY / GLIGLI / TAUNTEK / UNTERGEEK / UNKNOWN.
- Given Matrix-6 or 6R, when opening EPROM TYPE, then GLIGLI is absent.
- Given UNKNOWN or FACTORY, when SysEx is queued, then stock delay for the device family is used.
- Given GLIGLI, TAUNTEK, or UNTERGEEK, when SysEx is queued, then optimised delay for the device family is used.
- Given first successful Inquiry with prompt flag unset, when detection completes, then the EPROM TYPE modal appears once with suggested default when mapped.
- Given the user clicks OK, when the modal closes, then the combo selection is persisted, delay updates, and the prompt flag is set.
- Given the user clicks LATER, when the modal closes, then the stored EPROM TYPE is unchanged, the prompt flag is set, and reconnect does not re-prompt.
- Given prompt flag already set, when Inquiry succeeds again, then no modal and EPROM TYPE is unchanged.
- Given compacted layout, when Settings opens, then content fits without a general scroll at design scale.

## Review Triage Log

- Blind: live delay skips coerceForDeviceFamily (GLIGLI on M-6 stays optimised) — **high** — verified in `SysExDelayProfile::fromSettings` (no coerce). Route: patch.
- Blind: device-type change coerces only when Settings open — **medium** — verified in `PluginEditorAudio.cpp` deviceType branch. Route: patch.
- Blind: `applyRestoredPluginState` never refreshes SysEx delay — **medium** — verified; only `syncHardwareLatencyFromState`. Route: patch.
- Blind: `kEpromTypePromptPending` not stripped in `getStateInformation` — **medium** — verified (only feedback + unresponsive stripped). Route: patch.
- Blind: open clears pending before answer; destroy can lose re-prompt — **medium** — verified `openEpromTypePromptDialog` clears pending first. Route: patch (guard + clear pending only with promptDone).
- Blind: overlapping callAsync can re-open dialog — **medium** — verified no promptDone/visible guard. Route: patch (same open guard).
- Blind: Settings restore coerces without delay refresh — **medium** — verified `restoreSettingsPanelFromState`. Route: patch.
- Blind: HARDWARE LATENCY contextual help still says inter-SysEx delay — **medium** — verified `PluginDisplayNames` help string. Route: patch.
- Blind: kDesignHeight 460 too small — **false** — content math at scale 1 is 428 + 32 padding = 460 for plugin layout.
- Blind: missing live GLIGLI+M-6 / forEachValidItem tests — **medium** — verified gap. Route: patch (with verification-gap items).
- Blind: CMake indentation drift — **low** — reject (cosmetic; everyday harm unlikely; fix not required for this story).
- Blind: spec status/notes empty / Code Map stale — **false** — process artifact, not a runtime defect; notes filled at present step.
- Blind: NORDCORE optimised path removed without migration note — **medium** (unverified product harm) — **defer** — document in deferred-work; no Settings item by intentional taxonomy.
- Blind: EpromTypePolicy includes SysExDelayProfile for EpromClass — **low** — reject (no named caller divergence yet).
- Blind: suggestion exact-token only — **false** — matches frozen suggestion table; variants intentionally UNKNOWN.
- Edge: prompt reappears after LATER if second Inquiry sets pending while open — **high** — same root as open guard (promptDone). Route: patch.
- Edge: restore coerce without delay refresh — **medium** — duplicate of Blind restore. Route: patch (grouped).
- Edge: fromSettings no coerce — **high** — duplicate of Blind live delay. Route: patch (grouped).
- Edge: open ignores promptDone — **high** — claim kind; duplicate open guard. Route: patch (grouped).
- VG: M-6 member bytes via fromDeviceInquiry untested — **medium** — pre-verified gap. Route: patch.
- VG: refreshSysExDelayFromSettings untested — **medium** — pre-verified gap. Route: patch.
- VG: prompt-pending Core contract untested — **defer** — needs inquiry harness; manual checks cover this chantier.
- VG: preferredEpromTypeForPrompt untested — **medium** — pre-verified gap. Route: patch (lift into policy + test).
- VG: forEachValidItem / deviceFamilyFromType untested — **medium** — pre-verified gap. Route: patch.
- VG other: live path no coerce — **high** — duplicate. Route: patch (grouped).

## Implementation Notes


**Modal vs silent:** Human chose explanatory modal (option 2). No silent write of EPROM TYPE from Inquiry.

**Modal copy (ASCII, agent default — tune in implementation if needed):**
- Title: `EPROM TYPE`
- Body: `Select the EPROM type for your synth. This affects MIDI timing and future features. A suggestion may be shown from the reported firmware version; confirm or change it.`
- Buttons: `OK` | `LATER`

**Prompt vs identity:** Footer keeps Inquiry `deviceVersion`. Settings / modal EPROM TYPE is the capability declaration.

**SysEx delay SSOT:** Replace broken substring match on 4-char versions; Settings drives `EpromClass`; Inquiry still drives `MatrixDeviceFamily`.

**Compact targets:** `kRowGap_` 12→8; `kSectionTitleGap_` 4→2; recompute `kDesignHeight`. No scroll.

**Suggested version map (modal combo default only):**
| Version (trimmed/upper) | Device family | Suggestion |
|-------------------------|---------------|------------|
| 1.16 | M-1000 | GLIGLI |
| 1.20 | M-1000 | TAUNTEK |
| 1.21 | M-1000 | UNTERGEEK |
| 1.11, 1.09 | M-1000 | FACTORY |
| 2.15 | M-6/6R | TAUNTEK |
| other | any | UNKNOWN |

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- expected: SysExDelayProfile + policy tests green
- `python3 Scripts/quality/lint_touched.py` -- expected: no findings on touched C++

**Manual checks:**
- Open Settings in plugin and standalone; confirm DEVICE contents and section order.
- Switch device type M-1000 ↔ 6/6R; confirm EPROM item lists.
- First connect: modal once with suggestion; OK persists; reconnect does not re-prompt.
- LATER path: stays UNKNOWN; no second prompt.
