---
title: 'Linux Standalone: enable JACK audio device type'
type: 'feature'
created: '2026-10-30'
status: 'done'
route: 'oneshot'
baseline_commit: 'c3bd9e17dacdf8de9a7d8e08433ecedf00b975a2'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-gh-42-windows-standalone-asio.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** On Linux Standalone, Settings → AUDIO never lists JACK (only ALSA by default), so studio / low-latency JACK or PipeWire-JACK setups cannot be chosen from DRIVER TYPE.

**Approach:** Turn on JUCE’s Linux JACK backend (`JUCE_JACK=1`) for Matrix-Control builds so Standalone’s AudioDeviceManager registers the JACK device type. Keep ALSA. Reuse the existing Settings AUDIO enumeration; no UI or Settings refactor.

</frozen-after-approval>

## Implementation Notes

- `CMakeLists.txt`: `if(CMAKE_SYSTEM_NAME STREQUAL "Linux")` sets `JUCE_JACK=1` PUBLIC on `Matrix-Control` and PRIVATE on `Matrix-Control_Tests`, plus FATAL_ERROR if the product `COMPILE_DEFINITIONS` lack the flag (same drift guard as GH-42 ASIO).
- Review patch: Linux configure `find_path(jack/jack.h)` with FATAL_ERROR naming `libjack-jackd2-dev` when headers are missing (clearer than a late JUCE compile failure).
- Added `Tests/Unit/LinuxJackDeviceTypeTests.cpp` — asserts `JUCE_JACK` and JACK type registration on Linux; off / no JACK type on other platforms.
- Left `JUCE_ALSA` at JUCE default (1). No Settings AUDIO UI changes.
- JUCE JACK type name is `"JACK"`; runtime uses `dlopen("libjack.so…")` — missing server/lib does not crash startup; type may still appear with empty devices, or open may fail.
- Linux configure/compile needs JACK headers (`libjack-jackd2-dev` or equivalent). CI (`build-and-test.yml` / `release.yml`) already installs that package; CONTRIBUTING already points at the workflow deps.
- Verified on Windows: `cmake --preset windows-debug -DMATRIX_BUILD_TESTS=ON`, build `Matrix-Control_Tests`, `--category LinuxJackDeviceType` and `--category WindowsAsioDeviceType` pass; `lint_touched.py --base main` OK. Linux compile/smoke deferred to CI / optional local Linux machine.

## Review Triage Log

- false — Blind: oneshot spec omits Always/Never / I/O / Code Map / AC — Oneshot route deletes those sections by design; Intent is the contract.
- defer — Blind: no Linux DRIVER TYPE smoke / CI green proof in this session — Real; deferred (see deferred-work.md).
- medium / patch — Blind: no configure-time jack.h probe — Real; fixed with `find_path` + FATAL_ERROR naming the package.
- low / rejected — Blind: README silent on JACK package — CONTRIBUTING already points at workflow deps; long docs out of scope.
- false — Blind: PUBLIC define also lands in Linux VST3 — Same shared-target pattern as GH-42 ASIO; Standalone-only define is insufficient by design.
- low / rejected — Blind: test does not assert ALSA still registered — Change does not touch `JUCE_ALSA`; default remains on.
- low / rejected — Blind: `containsIgnoreCase("JACK")` vs exact `"JACK"` — Matches GH-42 ASIO test policy; JUCE type name is JACK.
- false — Blind: no edge-case matrix for server-down / PipeWire — Oneshot + Intent already accept missing type or open failure without crash.
- false — Blind: branch has no commits yet — Mid-workflow; commit follows finalize.
- defer — Blind: no LGPL / tracking id for deferred smoke — LGPL notice deferred (see deferred-work.md); smoke deferred same.
- low / rejected — Blind: failed expect does not dump type names — Cosmetic CI diagnostics; rejected on ASIO sibling for same reason.
