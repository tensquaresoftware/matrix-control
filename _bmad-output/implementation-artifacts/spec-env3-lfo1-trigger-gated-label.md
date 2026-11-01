---
title: 'ENV 3 LFO 1 TRIGGER gated label matches ENV 1/2'
type: 'bugfix'
created: '2026-11-01'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** On ENV 3, the LFO 1 TRIGGER combo shows a long gated item ("GATED LFO 1 TRIGGER") that truncates in the control, while ENV 1 and ENV 2 already show the short label "G-LFO 1" for the same choice.

**Approach:** Point ENV 3's gated choice at the same short display constant ENV 1/2 already use, and remove the unused long-label constant.

</frozen-after-approval>

## Implementation Notes

- ENV 3 LFO 1 TRIGGER gated choice now uses `ChoiceLists::Lfo1Trigger::kGLfo1` ("G-LFO 1"), matching ENV 1/2.
- Removed unused `kGatedLfo1Trigger` ("GATED LFO 1 TRIGGER") from `PluginDisplayNames.h`.
- `lint_touched.py` passed on both touched files.
- Skipped Acceptance Auditor / Edge Case Hunter (oneshot path lists Blind Hunter only).

## Review Triage Log

- Blind: Spec still `in-progress` while notes claim done — **false** — mid-oneshot snapshot; status set to `done` at finalize.
- Blind: Oneshot spec lacks acceptance / manual verification steps — **false** — oneshot route deliberately omits those sections; smoke offered in presentation.
- Blind: Truncation site/scale not recorded — **low (rejected)** — user report + long label already explain the defect; recording repro coords would not change the fix.
- Blind: Contextual help still omits NORMAL / LFO 1 / G-LFO 1 meanings — **medium (defer)** — pre-existing copy gap, not introduced by this label alignment.
- Blind: No test asserting ENV1/2/3 LFO 1 TRIGGER choice-string parity — **medium (defer)** — real regression gap; out of oneshot footprint.
- Blind: Lint claim lacks command evidence — **low (rejected)** — `lint_touched.py` exit 0 already observed this session; expanding notes adds noise.
- Blind: Add comment/alias so editors do not reintroduce a long gated label — **low (rejected)** — `kGLfo1` / `G-LFO 1` already match ENV 1/2 SSOT; comment would not prevent a wrong constant choice.
- Blind: Contextual-help inventory not updated — **false** — inventory mirrors help strings unchanged by this pass; no label inventory duty for combo choices.

