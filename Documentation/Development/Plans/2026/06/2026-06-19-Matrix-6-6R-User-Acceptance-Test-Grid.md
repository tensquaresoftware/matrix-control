# Matrix-6 / Matrix-6R — User Acceptance Test Grid

Living document for structured manual UAT on Matrix-6 and Matrix-6R hardware (or controlled simulation).  
Update **Session log** and row **Result** columns after each test pass.

**Related stories:** 8.5 (memory limits), 7.3 / 7.3b (Patch Manager handler), 7.5 (Bank Utility UI), 8.1–8.2 (device inquiry / type).

---

## How to use this document

1. Pick one **Session** block below (or add a new dated session).
2. Fill **Environment** before testing.
3. Work through sections in order; mark each row `PASS` / `FAIL` / `N/A` / `BLOCKED`.
4. Capture MIDI evidence (monitor log or screenshot) for `FAIL` rows.
5. Append a one-line summary to **Session log** when done.

**Simulation without hardware (partial):** see [Appendix A — APVTS device-type override](#appendix-a--apvts-device-type-override).  
Full UAT still requires real Matrix-6/6R for inquiry, SysEx, and display behaviour.

---

## Session template (copy for each run)

### Session — YYYY-MM-DD

| Field | Value |
|-------|-------|
| **Tester** | |
| **Build** | Standalone Debug / VST3 — commit `________` |
| **Synth** | Matrix-6 / Matrix-6R — firmware `________` |
| **MIDI** | Input / Output ports |
| **Host** (if VST3) | |
| **deviceType (APVTS)** | `Matrix-6` / `Matrix-6R` / inquiry auto |
| **deviceDetected** | true / false |

---

## Section 1 — Device identity and limits

| ID | Test | Steps | Expected | Result | Notes |
|----|------|-------|----------|--------|-------|
| M6-1.1 | Device inquiry | Connect MIDI; open plugin; wait for inquiry | Footer or log: synth detected as Matrix-6 or Matrix-6R; `deviceType` APVTS matches | | |
| M6-1.2 | No bank concept | Inspect Internal Patches bank NumberBox | Bank field read-only or fixed; range 0 only; no bank 1–9 semantics | | |
| M6-1.3 | Patch range | Set patch NumberBox to 0 and 99 | Accepted; clamp at boundaries | | |
| M6-1.4 | Patch range OOB | Attempt 100 or −1 (if UI allows typing) | Clamped to 0–99 | | |
| M6-1.5 | Reconnect | Power-cycle synth; reconnect MIDI | `deviceType` re-resolved; limits unchanged | | |

---

## Section 2 — Bank Utility panel (must be inactive)

| ID | Test | Steps | Expected | Result | Notes |
|----|------|-------|----------|--------|-------|
| M6-2.1 | Visual graying | With `deviceDetected` true and Matrix-6 type | Buttons 0–9 and **UNLOCK** at 50 % alpha (grayed) | | |
| M6-2.2 | Bank button click | Click bank **3** | No Set Bank SysEx (`F0 10 06 0A …`); APVTS bank unchanged | | |
| M6-2.3 | UNLOCK click | Click **UNLOCK** | No Unlock SysEx (`F0 10 06 0C …`); `patchManagerBanksLocked` unchanged | | |
| M6-2.4 | Footer on grayed click | Click grayed bank (if clickable) | Optional info footer (Matrix-1000-only message) — document observed text | | |
| M6-2.5 | No device detected | Disconnect / inquiry fails | Graying **off** (buttons normal brightness) but handler still no-ops if type is M-6 — note UX gap | | |

---

## Section 3 — Internal Patches navigation

| ID | Test | Steps | Expected | Result | Notes |
|----|------|-------|----------|--------|-------|
| M6-3.1 | Previous / Next | Patch 50 → `<` / `>` | Program Change only; patch 49 / 51; **no** Set Bank | | |
| M6-3.2 | Wrap high | Patch 99 → `>` | Patch 0; same “bank”; PC only | | |
| M6-3.3 | Wrap low | Patch 0 → `<` | Patch 99; PC only | | |
| M6-3.4 | NumberBox patch edit | Type patch 42 | PC to hardware; no Set Bank | | |
| M6-3.5 | Rapid navigation | Ten `<` clicks from patch 10 | Monotonic wrap; no SysEx except PC | | |

---

## Section 4 — INIT / COPY / PASTE / STORE

| ID | Test | Steps | Expected | Result | Notes |
|----|------|-------|----------|--------|-------|
| M6-4.1 | INIT | Click **INIT** | Editor resets; footer info; no bank SysEx | | |
| M6-4.2 | COPY → PASTE | COPY patch; PASTE | Patch data SysEx (`0x01` single patch); no Set Bank | | |
| M6-4.3 | STORE | Edit patch; **STORE** | Patch data SysEx to current patch slot; no Set Bank | | |
| M6-4.4 | PASTE enabled | All patch slots 0–99 | PASTE/STORE enabled (no ROM banks on M-6) | | |

---

## Section 5 — Cross-regression (Matrix-1000 must not leak)

| ID | Test | Steps | Expected | Result | Notes |
|----|------|-------|----------|--------|-------|
| M6-5.1 | No 0x0A ever | Full session MIDI log review | Zero `0A` opcodes on M-6 session | | |
| M6-5.2 | No 0x0C ever | Click UNLOCK (if reachable) | Zero `0C` opcodes | | |
| M6-5.3 | banksLocked property | After navigation + STORE | `patchManagerBanksLocked` not written by M-6 paths (or irrelevant / unchanged) | | |

---

## Section 6 — Matrix-6R-specific (if hardware available)

| ID | Test | Steps | Expected | Result | Notes |
|----|------|-------|----------|--------|-------|
| M6R-6.1 | Inquiry identity | Connect 6R | `deviceType` = `Matrix-6R` | | |
| M6R-6.2 | Limits parity | Repeat M6-3.1 – M6-4.3 | Same as Matrix-6 | | |

---

## Section 7 — Switching device type (brownfield)

| ID | Test | Steps | Expected | Result | Notes |
|----|------|-------|----------|--------|-------|
| M6-7.1 | M-1000 → M-6 | Start M-1000; switch type to M-6 (inquiry or override) | Bank Utility grays; coordinates reconciled | | |
| M6-7.2 | M-6 → M-1000 | Switch back | Bank Utility active; bank 0–9 usable | | |
| M6-7.3 | Stale lock flag | M-1000 locked → switch to M-6 | Document `patchManagerBanksLocked` behaviour (known defer) | | |

---

## Session log

| Date | Tester | Scope | Pass | Fail | Blocked | Summary |
|------|--------|-------|------|------|---------|---------|
| 2026-06-19 | Guillaume | — | — | — | — | Grid created; session not started. Matrix-1000 smoke 7.3b validated separately. |
| 2026-07-11 | Guillaume | Story 7.5 Bank Utility UI | 5 | 0 | 1 | M-1000 smoke PASS (Tauntek v1.20). M6-2.x graying **BLOCKED** — no Matrix-6/6R hardware. Handler paths for M-6 covered in CI only. |

---

## Appendix A — APVTS device-type override

Use only for **UI / handler dry-runs** while Matrix-1000 is connected. Do not rely on this for hardware SysEx validation.

**Requirements for Bank Utility graying:** `deviceDetected == true` **and** `deviceType` ∈ `{ Matrix-6, Matrix-6R }`.

1. Connect synth (or set `deviceDetected` true via inquiry).
2. Set root APVTS property `deviceType` to `"Matrix-6"` or `"Matrix-6R"`.
3. Verify Section 2 visually; Section 3–4 handler paths via MIDI monitor.
4. Restore `deviceType` to `"Matrix-1000"` before normal editing.

**Automated coverage (CI):** `PatchManagerActionHandlerTests` — `bankSelect_matrix6_noSetBank`, `unlockBank_matrix6_noOp`.

---

## Appendix C — Device-type simulation harness (backlog)

**Problem:** Guillaume's lab has **Matrix-1000 only** (Tauntek EPROM v1.20). Many features (Bank Utility graying, memory limits, inquiry identity) need Matrix-6 / Matrix-6R behaviour that cannot be validated on real hardware today.

**Current workarounds (insufficient for full UAT):**

| Approach | Covers | Gaps |
|----------|--------|------|
| Appendix A — manual APVTS `deviceType` override | UI gating dry-runs with `deviceDetected == true` | No dev UI; easy to forget restore; no inquiry/SysEx fidelity |
| CI unit tests (`PatchManagerActionHandlerTests`, `DeviceMemoryLimits`) | Handler no-op / limit branches | No GUI, no visual graying sign-off |

**Target (when Epic 8 or a utility story prioritizes it):**

1. **Dev-only control** (Settings or TestComponent) to set `deviceType` + `deviceDetected` without MIDI inquiry — three presets: Matrix-1000, Matrix-6, Matrix-6R.
2. **Optional MIDI mock** for inquiry response bytes (Epic 8.2+) so footer identity and auto-detection paths are testable without hardware.
3. **Document** the override in this grid and in Standalone smoke checklists.

**Stories likely to need this:** 7.5 (partial — M-6 graying untested), 7.6, 8.1–8.3, U-8 / U-9 panel audits.

Until the harness exists: mark Matrix-6/6R UAT rows `BLOCKED` or `N/A`; rely on CI + Appendix A for best-effort UI checks.

---

## Appendix B — Evidence checklist

- [ ] MIDI monitor capture attached / path noted
- [ ] Footer screenshot for INIT / errors
- [ ] APVTS snapshot (optional) for failed rows
- [ ] Commit hash under test recorded in session header
