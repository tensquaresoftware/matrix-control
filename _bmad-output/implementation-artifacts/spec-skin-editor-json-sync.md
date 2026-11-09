---
title: 'Skin Editor — JSON Sync Off/Ask/Auto'
type: 'feature'
created: '2026-11-09'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Authors who edit the last imported JSON outside Skin Editor either get interrupted by every Reload/Discard prompt or have no way to follow disk changes automatically.

**Approach:** Add a SESSION combo labelled JSON Sync with Off / Ask / Auto. Persist the choice in the session file (default Ask on first launch or legacy sessions). Off ignores disk changes; Ask keeps the existing prompt; Auto reloads silently when the editor is clean, and falls back to Ask when dirty.

</frozen-after-approval>

## Implementation Notes

- Session field `jsonSyncMode` (`off` / `ask` / `auto`); missing key → Ask.
- SESSION layout: after Import, separator + muted `JSON Sync` label + combo.
- Import watch timer runs only when mode is Ask or Auto and a path is armed.
- Auto + dirty → same Reload/Discard prompt as Ask.
- Discard Editor work keeps the JSON Sync preference.
