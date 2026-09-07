---
title: 'Align stale EDITOR MIDI FROM strings with MIDI FROM'
type: 'bugfix'
created: '2026-09-07'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Help and guidance text still says `EDITOR MIDI FROM` (or `Editor MIDI From`) while the header control label was shortened to `MIDI FROM`, so users without hardware (or reading the manual) see a name that no longer matches the UI.

**Approach:** Replace those stale user-facing strings with the current product wording (`MIDI FROM` / title-case `MIDI From` where sibling footer prose already uses that style), and leave historical BMad artifacts and non-display symbol names alone.

</frozen-after-approval>

## Implementation Notes

- Canonical header label: `HeaderPanel::kEditorMidiFromLabel` = `"MIDI FROM"` (constant name may keep `Editor`; do not rename symbols).
- Stale hits fixed:
  - `PluginDisplayNames.h` `kDeviceDumpFailedFooter` — `EDITOR MIDI FROM` → `MIDI FROM`
  - `PluginDisplayNames.h` `kDeviceLockGuidance` — `Editor MIDI From` → `MIDI From` (match `kUnsupportedMatrixDeviceFooter` title case)
  - `Documentation/User/manuel-utilisateur.md` troubleshooting row — `` `EDITOR MIDI FROM` `` → `` `MIDI FROM` ``
- Review patch: synced the §5 blockquote under “Tant que le synthé n’est pas détecté” to the live `kDeviceLockGuidance` sentence (pre-existing paraphrase drift exposed while editing that string).
- Out of scope: `_bmad-output/**` historical stories/UAT; C++ identifier names (`kEditorMidiFromLabel`, layout width symbols).
- Grep: no remaining `EDITOR MIDI` / `Editor MIDI` under `Source/` or `Documentation/User/`.
- Casing: ALL-CAPS when naming the header control; title-case in sentence-style footer prose (same split as existing footers).

## Review Triage Log

- Blind Hunter: manual §5 blockquote out of sync with `kDeviceLockGuidance` — **medium** → **patch** (aligned quote to live string).
- Blind Hunter: spec omitted that quote as a verify target — **low** rejected (oneshot notes; fixed via patch above).
- Blind Hunter: spec still `in-progress` while notes say applied — **false** (finalize sets `done`).
- Blind Hunter: frozen Intent still present-tense after fix — **false** (frozen block must not be rewritten by agent).
- Blind Hunter: mixed `MIDI FROM` / `MIDI From` casing — **false** (intentional: control ALL-CAPS vs footer sentence style).
- Blind Hunter: grep method / residual symbol names not recorded — **low** rejected (notes now mention grep scope; identifiers intentionally out of scope).
- Blind Hunter: no acceptance checklist in oneshot spec — **false** (oneshot template omits Tasks & AC by design).
- Blind Hunter: empty `context: []` / missing rename provenance — **low** rejected (not needed for this string fix).
- Blind Hunter: incomplete `PluginDisplayNames::` qualification — **low** rejected (cosmetic for this change).
- Blind Hunter: missing golden final sentences in spec — **low** rejected (diff is the source of truth; three-line change).
