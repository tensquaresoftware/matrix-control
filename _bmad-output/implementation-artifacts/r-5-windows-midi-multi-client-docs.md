---
organization: Ten Square Software
project: Matrix-Control
title: Story R-5 — Windows MIDI Multi-Client Documentation
author: BMad Agent
status: done
baseline_commit: 2e9f6ff
sources:
  - planning-artifacts/sprint-change-proposal-2026-06-06.md
  - planning-artifacts/epics.md
  - implementation-artifacts/r-3-midi-port-open-error-feedback.md
  - implementation-artifacts/r-4-midioutboundqueue-mpsc-audit.md
  - Documentation/hardware-latency.md
  - project-context.md
created: 2026-06-06
updated: 2026-06-06
---

# Story R-5: Windows MIDI Multi-Client Documentation

Status: done

<!-- NFR-6. Sprint Change Proposal 2026-06-06. Last story Epic R. Depends on R-3 (footer port-open errors). Closes Epic R when done. -->

## Story

As a Windows user,
I want setup documentation for MIDI port conflicts,
so that I can use loopMIDI or rtpMIDI when winMM blocks shared access (NFR-6).

## Acceptance Criteria

### AC 1 — Windows MIDI setup guide in `Documentation/`

1. New file **`Documentation/windows-midi-multi-client.md`** (kebab-case per project-context).
2. Guide explains:
   - **winMM exclusive access:** on Windows, legacy WinMM MIDI ports are typically **single-client** — when a DAW (e.g. Ableton Live 12) opens a hardware MIDI port, other apps (including Matrix-Control standalone or a second instance) often get `openDevice` failure (`nullptr`).
   - **JUCE context:** Matrix-Control uses JUCE MIDI I/O, which on Windows targets WinMM — same limitation applies to **MIDI From** / **MIDI To** header ports.
   - **Symptom link to R-3:** footer error `"could not open (port may be in use…)"` on Windows indicates this conflict.
3. **Recommended workaround — loopMIDI:**
   - Recommend **loopMIDI 1.0.16 or newer** ([Tobias Erichsen](http://www.tobias-erichsen.de/software/loopmidi.html)) for virtual loopback ports.
   - Document typical routing: hardware interface → (optional MIDI-OX or DAW routing) → loopMIDI virtual port(s) → Matrix-Control **MIDI From/To** + DAW each on their own virtual port.
   - Note loopMIDI ports exist while loopMIDI is running (tray); autostart optional.
4. **Verified alternative — rtpMIDI:**
   - Brief section on **rtpMIDI** (network MIDI session) as alternative when Wi‑Fi/Ethernet routing is acceptable — not a substitute for local hardware fan-out but useful for multi-app setups on LAN.
5. **Matrix-Control workflow section:**
   - Standalone: set **MIDI To** to synth path; **MIDI From** for Device Inquiry inbound SysEx.
   - Hosted plugin: DAW owns hardware MIDI; plugin uses host-routed virtual ports — same winMM conflict if DAW and standalone both target one physical port.
6. **Future note (short):** Windows MIDI Services (multi-client) is emerging but DAWs/JUCE still largely use WinMM — do not promise native multi-client today.
7. English prose; same tone/structure as **`Documentation/hardware-latency.md`** (headings, tables where useful, practical steps).

### AC 2 — Documentation index

1. Update **`Documentation/README.md`** table with row for `windows-midi-multi-client.md` (Windows MIDI port conflicts / loopMIDI setup).

### AC 3 — In-app link from R-3 footer message

1. On **`JUCE_WINDOWS`** only, when **`formatFooterMessage`** builds **`kOpenRejected`** text, append setup guide reference after existing hint (do **not** change macOS/Linux strings).
2. Append text (exact suffix, after closing `).` of current message):
   - `" See Documentation/windows-midi-multi-client.md for setup help."`
3. Keep message single-line plain text — **`FooterPanel`** renders `uiMessageText` as text only (no clickable URL in this story; path is the link target for users browsing the repo or release bundle).
4. **Optional enhancement (only if trivial):** when `ProjectPaths::getProjectRoot()` resolves a repo root **and** the doc file exists, append absolute path in parentheses — e.g. `(…/Documentation/windows-midi-multi-client.md)`. Skip if fallback root or file missing — relative path suffix alone satisfies AC.
5. Do **not** change `propagateOpenFailure`, `ExceptionPropagator`, or `FooterPanel` behaviour beyond message string content.

### AC 4 — Unit tests

1. Update **`Tests/Unit/MidiPortOpenFeedbackTests.cpp`**:
   - Windows branch (`#if JUCE_WINDOWS`): `kOpenRejected` message contains `"Documentation/windows-midi-multi-client.md"`.
   - Non-Windows: message does **not** contain that path.
   - `kNotFound` messages unchanged on all platforms.
2. All existing unit tests pass.

### AC 5 — Sprint hygiene (Epic R closure)

1. When complete: `sprint-status.yaml` → `r-5-windows-midi-multi-client-docs: done`.
2. Set **`epic-r: done`** — R-5 is the last Epic R story.
3. No CMake change unless new test sources added (not expected).

## Tasks / Subtasks

- [x] **Author documentation** (AC: #1, #2)
  - [x] Create `Documentation/windows-midi-multi-client.md`
  - [x] Update `Documentation/README.md` index

- [x] **Footer message link** (AC: #3)
  - [x] Extend `MidiPortOpenFeedback::formatFooterMessage` Windows `kOpenRejected` suffix
  - [x] Optional absolute path via `ProjectPaths` when doc exists

- [x] **Tests + verification** (AC: #4, #5)
  - [x] Update `MidiPortOpenFeedbackTests.cpp`
  - [x] Run `Matrix-Control_Tests`
  - [x] Update `sprint-status.yaml` (R-5 done, epic-r done)

## Dev Notes

### Problem / context

Story **R-3** added footer feedback when `MidiInputPort` / `MidiOutputPort` open fails. Windows users frequently hit **exclusive access** when Ableton (or another app) already holds the hardware MIDI port. R-3 intentionally deferred the setup guide and doc link to **R-5**.

This story is **documentation-first** with a **minimal code touch** (footer suffix + tests). No MIDI stack changes.

### Existing code to extend (read before editing)

| File | Current state | R-5 change |
|---|---|---|
| `Source/Core/MIDI/MidiPortOpenFeedback.cpp` | Windows `kOpenRejected` mentions loopMIDI inline | Append doc path suffix; keep loopMIDI hint |
| `Tests/Unit/MidiPortOpenFeedbackTests.cpp` | Asserts loopMIDI on Windows | Add doc path assertion |
| `Documentation/README.md` | Index of public docs | Add row |
| `Documentation/hardware-latency.md` | Reference doc style (R-2) | Match tone/structure |

Current Windows open-rejected message (R-3):

```cpp
// MidiPortOpenFeedback.cpp — kOpenRejected branch
message += ", e.g. your DAW. Try closing other apps or use a virtual MIDI port such as loopMIDI";
message += ").";
```

### Doc content outline (minimum sections)

1. **Why ports fail on Windows** — winMM single-client; DAW vs Matrix-Control.
2. **Quick fix** — close other app using the port; retry header **MIDI From/To**.
3. **loopMIDI setup (recommended)** — install 1.0.16+, create ports, route hardware→virtual, assign Matrix-Control ports.
4. **rtpMIDI alternative** — when network MIDI is OK.
5. **Ableton Live 12 notes** — instrument track MIDI routing + separate audio track (cross-ref `hardware-latency.md` / AD-11).
6. **Related footer errors** — quote R-3 message pattern.

### What NOT to do

- Do **not** rewrite R-3 propagation logic or add `FooterPanel` click handlers.
- Do **not** bundle loopMIDI installer — link to vendor site only.
- Do **not** add French translation in this story (NFR-6 user manual EN+FR is a later epic scope).
- Do **not** start Epic 3 or modify `MidiOutboundQueue` (R-4 closed).

### Previous story intelligence

**R-3 (done):**
- `Core::MidiPortOpenFeedback` — SSOT for footer port-open strings.
- AC explicitly deferred doc link to R-5.
- Tests use `#if JUCE_WINDOWS` for platform-specific expectations.

**R-4 (done):**
- Build/test: `cmake --preset macOS-ARM-Debug && cmake --build Builds/macOS-ARM --target Matrix-Control_Tests`.
- Full suite was green at R-4 completion (~1007 tests).

### References

- [sprint-change-proposal-2026-06-06.md § Story R-5]
- [epics.md Epic R, Story R-5]
- [r-3-midi-port-open-error-feedback.md]
- [Documentation/hardware-latency.md] — doc style reference
- [Source/Core/MIDI/MidiPortOpenFeedback.cpp]
- [Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp] — plain-text footer renderer

## Dev Agent Record

### Agent Model Used

Composer 2.5

### Debug Log References

- Wrapped `windowsMidiSetupDocSuffix` in `#if JUCE_WINDOWS` to avoid unused-function warning on macOS CI.

### Completion Notes List

- Added `Documentation/windows-midi-multi-client.md` — winMM exclusive access, loopMIDI 1.0.16+, rtpMIDI, Ableton workflow.
- Updated `Documentation/README.md` index.
- Windows `kOpenRejected` footer suffix links to doc; absolute path appended when `ProjectPaths` resolves repo root and file exists.
- Unit tests updated; full suite green on macOS ARM Debug.

### File List

- Documentation/windows-midi-multi-client.md (new)
- Documentation/README.md (updated)
- Source/Core/MIDI/MidiPortOpenFeedback.cpp (updated)
- Tests/Unit/MidiPortOpenFeedbackTests.cpp (updated)
- _bmad-output/implementation-artifacts/r-5-windows-midi-multi-client-docs.md (updated)
- _bmad-output/implementation-artifacts/sprint-status.yaml (updated)

## Change Log

- 2026-06-06: Story created (ready-for-dev) — Windows MIDI multi-client docs + R-3 footer link; last Epic R story.
- 2026-06-06: Implementation complete — doc, footer suffix, tests; Epic R closed.
