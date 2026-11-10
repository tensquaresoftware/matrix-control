---
title: 'Restore CI green for Skin Editor colour-picker and session JSON tests'
type: 'bugfix'
created: '2026-11-10'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** GitHub Actions `Build and Test` has been red on all platforms because unit tests disagree with intentional Skin Editor behaviour: the colour-picker layout gap under #/RGB, and Linux session JSON round-trip when Documents is missing.

**Approach:** Align the two failing unit tests with the existing product behaviour (tight 5px pad under #/RGB; import only keeps directories that exist). Do not change picker layout math or session path validation.

</frozen-after-approval>

## Implementation Notes

- Aligned `SkinEditorColourPickerTests` RGB→Saturation gap expectation with layout tight pad (`kPanelContentPad - 5`); product layout unchanged.
- Seeded `DevSkinLayerJson` session round-trip `lastSkinJsonDirectory` from a unique temp directory via `getNonexistentChildFile` (import drops non-existing paths); early-return if create fails; delete temp dir after asserts.
- Verified locally: `SkinEditorColourPicker` and `DevSkinLayerJson` categories → 0 failures; `lint_touched.py --base HEAD` OK.
- Unrelated dirty WIP left untouched: Slider `editorCaret` retarget in LookBuildersMultiSet + SliderZoneSeedTests pin (out of this oneshot Intent).

## Review Triage Log

- Blind Hunter: Slider caret / LookBuilders in worktree undocumented — **false** (unrelated local WIP; not part of this Intent).
- Blind Hunter: closes deferred caret decision without deferred-work update — **false** (same out-of-scope WIP).
- Blind Hunter: no negative test for missing Documents path drop — **low**, deferred (CI green covered by happy-path seed; negative case is documentation of existing import rule).
- Blind Hunter: no RAII for temp dir cleanup — **low**, rejected (CI runners clean temp; early-return added for setup failure).
- Blind Hunter: fixed temp dir name collision — **medium**, patched (`getNonexistentChildFile`).
- Blind Hunter: soft setup expect continues on failure — **medium**, patched (early return when dir not ready).
- Blind Hunter: spec status / SliderZoneSeed verification under-documented — **false** (status set done; SliderZoneSeed not in Intent).
- Blind Hunter: duplicated magic `5` pad vs product — **low**, deferred (pre-existing product duplication; extracting shared constant is out of oneshot footprint).
