---
title: 'Skin Editor external JSON reload prompt'
type: 'feature'
created: '2026-11-08'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** While a JSON file is loaded via Import, external editors can change it on disk with no Skin Editor warning, so the on-screen skins silently diverge from the file Guillaume thinks he is using.

**Approach:** After a successful Import, watch that file. When its contents change on disk, show a native modal (English, ASCII) offering Reload (replace editor Dev skins from the file via `importFromJson`) or Discard (keep in-memory skins and re-baseline the watch so the prompt does not repeat until the next external change).

</frozen-after-approval>

## Implementation Notes

- Watch last successful Import path only (not Export-only paths unless they are that same file after a write we own).
- Poll ~1s with a dedicated timer; ignore our own Export writes to the watched path by refreshing the baseline after write.
- Dialog copy: title `JSON File Changed`; body explains Reload replaces Editor skins from that file; adds unsaved-loss note when `editorDirty_`; buttons `Reload`, `Discard`.
- Reload uses `TSS::DevSkin::importFromJson` (compiled + file skins), then refresh UI / optional push; Discard only updates the watched fingerprint.
- Guard against re-entrant dialogs while one is open.
- Blind Hunter review: patched dirty warning, content fingerprint (`hashCode64`), 2-poll debounce, `File::operator==` for Export suppress, clear watch on session discard. Deferred: unit tests; permanent missing-file UX; append-vs-replace product nuance (Reload intentionally replace-all like a document open).
- `importFromJson` only `replaceAll` after a full successful parse — failed Reload leaves the layer intact.

## Review Triage Log

- Append Import vs replace-all Reload — medium, deferred: intentional document Reload via `importFromJson`; dialog now states it replaces Editor skins.
- Dirty Reload loses edits — medium, patched: dialog warns when `editorDirty_`.
- Failed Reload corrupts layer — false: `replaceAll` only after successful parse/append.
- Missing watched file silent — medium, deferred: transient rename-save ignored; permanent-delete UX later.
- Watch not cleared on session discard — medium, patched: `clearWatchedImportJson` in discard path.
- Weak fingerprint — medium, patched: content length + `hashCode64`.
- Half-written JSON race — medium, patched: 2-poll stable debounce.
- No unit tests — medium, deferred.
- Export path string match — low, patched: `File::operator==`.
