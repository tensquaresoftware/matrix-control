---
title: 'Skin Editor — Product Duplicate selects skin in Matrix-Control'
type: 'bugfix'
created: '2026-11-01'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After Duplicate on a Product skin in the Skin Editor (Skins > Product), Matrix-Control sometimes shows the new Dev copy and sometimes keeps the previous look, because a bridge push can fire while the session still points at the old Editor id, then the UI refreshes without a second push. New Editor skin must not steal the plugin's current look.

**Approach:** Make Product Duplicate always push the newly selected Editor skin to Matrix-Control when the bridge is connected (after session state is updated). Keep New (and Editor Duplicate) from pushing on that action; suppress the mid-mutation `onChanged` push that currently races with a stale Editor id.

</frozen-after-approval>

## Implementation Notes

### Investigation (code map — oneshot)

- `Tools/SkinEditor/SkinEditorMainComponentActions.cpp` — `onDuplicateProduct` → `duplicateFromSkin`; `onNew` shares the same stale-id race; both set `editorSkinId` + `compareOn=false` then `refreshAll()` **without** `pushVisible()`.
- `Tools/SkinEditor/SkinEditorMainComponent.cpp` — `layer_.onChanged` → `applyColourPreviewRefresh()` → may `pushVisible()` **before** `editorSkinId` is updated.
- `Tools/SkinEditor/SkinEditorMainComponentSession.cpp` — `pushVisible()` / `visibleSkinId()` / `applyEditorComboChange`.
- `Source/GUI/Skins/Dev/DevSkinLayer.cpp` — `duplicateSkin` → `bump()` → `onChanged`.
- Do **not** change bridge protocol, Compare semantics, Product combo auto-push rules, or `applySkinFromItemId` persistence.

### Agent decisions

- **Editor Duplicate:** no intentional post-action push (Product Duplicate only). Still suppress stale mid-mutation push via shared `duplicateFromSkin`.
- **Bridge disconnected:** push only if connected after Product Duplicate; reconnect already `pushVisible()`.
- **Cancel / failed dialogs:** unchanged — no push.

### Done

- `duplicateFromSkin` / `onNew` run layer mutations under `runWithoutLayerChangeNotify`.
- Shared `adoptActiveEditorSkinAfterCreate()` updates session, cancels pending colour-preview coalesce (avoids late push after New), refreshes UI; no push inside adopt.
- `onDuplicateProduct` calls `pushVisible()` when bridge connected after adopt.
- Lint OK; `Matrix-Control_SkinEditor` builds.

### Verification (manual)

- Bridge connected + Compare on → Product Duplicate: Compare clears; Matrix-Control shows the new Dev skin.
- Bridge connected → New / Editor Duplicate: Matrix-Control look unchanged by that click.
- Cancel / fail dialogs: no push.

## Review Triage Log

- Dead `! isCompareProductView()` after adopt clears Compare — **low/patch**: removed; push gated on `bridge_.isConnected()` only.
- Pending colour coalesce / gesture can push after New — **medium/patch**: stop coalesce timer + clear gesture in `adoptActiveEditorSkinAfterCreate`.
- No unit test for push ordering — **defer** (no MainComponent harness).
- `runWithoutLayerChangeNotify` non-RAII — **defer** (pre-existing helper).
- `onDelete` stale-id race — **defer** (pre-existing; out of scope).
- Adopt comment underspecifies Product-only push — **low/patch**: comment updated.
- Spec manual cases incomplete — **low/patch**: verification cases expanded in Implementation Notes.
- Auto-push lacks distinct status cue — **false**: `pushVisible()` uses the same bridge ack / LAST PUSH path as manual Push.
