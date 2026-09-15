# ASCII display strings (Matrix-Control) — SSOT for agents

**Scope:** Any user-visible copy painted in the plugin UI (footer status, contextual help, confirmation dialogs, progress text) stored in `PluginDisplayNames` or written to APVTS `uiMessageText`.

**Why:** The footer / UI font path (PT Sans Narrow) does not reliably render UTF-8 punctuation. Em dashes and similar glyphs show as mojibake (e.g. `a€`). See `_bmad-output/implementation-artifacts/spec-user-message-ascii-encoding.md`.

## Mandatory for new copy

| Use | Not in footer/status/dialog copy |
|-----|----------------------------------|
| ASCII hyphen `-` or spaced ` - ` for clause breaks | Em dash `—` (U+2014), en dash `–` (U+2013) |
| ASCII ellipsis `...` (or paint-time truncation helper) | Unicode ellipsis `…` |
| Straight ASCII quotes when needed | Curly quotes `“”‘’` |

## Approved exceptions (do not “fix” without a story)

- **History root sentinel** — em dash via `juce::String::fromUTF8("\xe2\x80\x94")` in the History combobox only (`kHistoryRootSentinel`).
- **PITCH octave labels** — `±` in MODE/PITCH combo faces where PT Sans Narrow is validated (see playable-calibration spec).
- **Non-UI** — comments, BMad chat/spec prose, user manual Markdown.

## Agent checklist (Build / Code Review)

1. New strings go in `PluginDisplayNames` (English), not inline in panels.
2. Before finishing: grep touched files for `—`, `–`, `…` inside string literals bound for display.
3. BMad spec **frozen copy tables** for footer/help must use ASCII punctuation — never paste from Word/Chat with typographic dashes.
4. Prefer ` - ` (space-hyphen-space) where Compare/footer guidance already does.

## Reference

- Constitution: `_bmad-output/project-context.md` (User-visible string encoding)
- Prior bugfix: `spec-user-message-ascii-encoding.md`
