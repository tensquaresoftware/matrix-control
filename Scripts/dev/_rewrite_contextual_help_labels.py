#!/usr/bin/env python3
"""Rewrite ContextualHelp strings to include on-screen control labels.

Dry-run by default. Pass --write to apply to PluginDisplayNames.h.
"""

from __future__ import annotations

import argparse
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HEADER = ROOT / "Source/Shared/Definitions/PluginDisplayNames.h"

# Help keys that describe a module/section band — keep module prefix only (no duplicate name).
SKIP_KEYS = {
    "kTitle",
    "kSectionTitle",
    "kCentreBand",  # zone, not a named control face
    "kDevice",  # DEVICE already = module prefix
}

# Header / logo keys → explicit face labels (no ParameterWidgets sibling).
MANUAL_LABELS: dict[tuple[str, ...], dict[str, str]] = {
    ("PluginDisplayNames", "HeaderPanel", "ContextualHelp"): {
        "kEditBadge": "EDIT",
        "kMidiBadge": "MIDI",
        "kAudioBadge": "AUDIO",
        "kInputGain": "INPUT GAIN",
        "kUndo": "UNDO",
        "kRedo": "REDO",
        "kPanic": "PANIC",
        "kLogo": "LOGO",
        "kSettings": "SETTINGS...",
        "kAbout": "ABOUT...",
        "kSkin": "SKIN",
        "kUiScale": "UI SCALE",
        "kFromKeyboardActivityLed": "FROM KEYBOARD",
        "kFromSynthActivityLed": "FROM SYNTH",
        "kToSynthActivityLed": "TO SYNTH",
        "kAudioPeakIndicator": "AUDIO PEAK",
    },
    ("PluginDisplayNames", "FooterPanel", "ContextualHelp"): {
        "kCentreBand": None,  # skip
        "kDevice": None,  # skip — DEVICE: already
    },
}

# Display-name overrides when the widget label is a short letter but the face is clearer,
# or when help key has no twin in ParameterWidgets (curve displays, groups, etc.).
KEY_LABEL_FALLBACK: dict[str, str] = {
    "kInit": "I",
    "kCopy": "C",
    "kPaste": "P",
    "kSectionInit": "I",
    "kSectionCopy": "C",
    "kSectionPaste": "P",
    "kBusInit": "I",
    "kBusHandle": "#",
    "kColumnHeader": "# / SOURCE / AMOUNT / DESTINATION",
    "kBalance": "DCO 2 | DCO 1 MIX",
    "kTrackGeneratorDisplay": "TRACK GENERATOR",
    "kEnvelopeDisplay": "ENVELOPE",
    "kBrowser": "BROWSER",
    "kMemory": "MEMORY",
    "kStorage": "STORAGE",
    "kCompare": "C",
    "kDelete": "D",
    "kFlush": "F",
    "kExport": "E",
    "kHistoryPrevious": "<",
    "kHistoryNext": ">",
    "kHistoryPrev": "<",
    "kPrevious": "<",
    "kPrev": "<",
    "kNext": ">",
    "kPatchNameDisplay": "PATCH NAME",
    # Mutator recipe toggles (also resolved via StandaloneWidgets when present)
    "kEnableDco1": "D1",
    "kEnableDco2": "D2",
    "kEnableVcfVca": "F/A",
    "kEnableFmTrack": "F/T",
    "kEnableRampPortamento": "R/P",
    "kEnableEnvelope1": "E1",
    "kEnableEnvelope2": "E2",
    "kEnableEnvelope3": "E3",
    "kEnableLfo1": "L1",
    "kEnableLfo2": "L2",
    "kEnableMatrixMod": "MM",
    # Misc help keys vs ParameterWidgets naming
    "kUnison": "UNISON",
    "kVolumeInvert": "VOLUME INVERT",
    "kBankLock": "BANK LOCK",
    "kMemoryProtect": "MEMORY PROTECT",
    # Bank Utility help keys vs StandaloneWidgets naming
    "kBank0": "0",
    "kBank1": "1",
    "kBank2": "2",
    "kBank3": "3",
    "kBank4": "4",
    "kBank5": "5",
    "kBank6": "6",
    "kBank7": "7",
    "kBank8": "8",
    "kBank9": "9",
    "kImport": "IMPORT",
    # Internal / Computer patches
    "kInitPatch": "INIT",
    "kStore": "STORE",
    "kOpen": "OPEN",
    "kSave": "SAVE",
    "kSaveAs": "SAVE AS",
    "kSelectPatch": "SELECT A PATCH",
    "kCurrentBank": "CURRENT BANK",
    "kCurrentPatch": "CURRENT PATCH",
}

# Help kName → widget kName when they differ but share a parent module.
KEY_ALIASES: dict[str, tuple[str, ...]] = {
    "kPrevious": ("kLoadPreviousPatch", "kLoadPreviousPatchFile", "kHistoryPrevious", "kPrev"),
    "kNext": ("kLoadNextPatch", "kLoadNextPatchFile", "kHistoryNext", "kNext"),
    "kCopy": ("kCopyBank", "kCopyPatch", "kCopy"),
    "kPaste": ("kPasteBank", "kPastePatch", "kPaste"),
    "kExport": ("kExportBank", "kExport"),
    "kImport": ("kImportBank", "kImport"),
    "kInit": ("kInitPatch", "kInit"),
    "kStore": ("kStorePatch", "kStore"),
    "kFlush": ("kClear", "kFlush"),
    "kHistoryPrevious": ("kHistoryPrevious",),
    "kHistoryNext": ("kHistoryNext",),
    "kUnison": ("kUnisonEnable", "kUnison"),
    "kBankLock": ("kBankLockEnable", "kBankLock"),
    "kVolumeInvert": ("kVolumeInvertEnable", "kVolumeInvert"),
    "kMemoryProtect": ("kMemoryProtectEnable", "kMemoryProtect"),
    "kCurrentBank": ("kCurrentBankNumber", "kCurrentBank"),
    "kCurrentPatch": ("kCurrentPatchNumber", "kCurrentPatch"),
    "kSelectPatch": ("kSelectPatchFile", "kSelectAPatch", "kSelectPatch"),
    "kOpen": ("kOpenPatchFolder", "kOpenFolder", "kOpen"),
    "kSaveAs": ("kSavePatchAsFile", "kSaveAs"),
    "kSave": ("kSavePatchFile", "kSave"),
    "kBank0": ("kSelectBank0",),
    "kBank1": ("kSelectBank1",),
    "kBank2": ("kSelectBank2",),
    "kBank3": ("kSelectBank3",),
    "kBank4": ("kSelectBank4",),
    "kBank5": ("kSelectBank5",),
    "kBank6": ("kSelectBank6",),
    "kBank7": ("kSelectBank7",),
    "kBank8": ("kSelectBank8",),
    "kBank9": ("kSelectBank9",),
}


def unescape_c_string(s: str) -> str:
    return bytes(s, "utf-8").decode("unicode_escape") if "\\" in s else s


def parse_string_literal_concat(text: str, start: int) -> tuple[str, int] | None:
    """Parse `"a" "b"` concat starting at first quote; return (value, index after ';')."""
    if start >= len(text) or text[start] != '"':
        return None
    parts: list[str] = []
    p = start
    while p < len(text) and text[p] == '"':
        m = re.match(r'"(.*?)"\s*', text[p:], re.S)
        if not m:
            return None
        parts.append(m.group(1))
        p += m.end()
    if p >= len(text) or text[p] != ";":
        return None
    return ("".join(parts), p + 1)


def walk_header(text: str) -> tuple[list[dict], dict[tuple, str], dict[str, str]]:
    """Return help entries, path-scoped labels, and global kName→literal for alias resolve."""
    help_entries: list[dict] = []
    labels: dict[tuple, str] = {}
    global_literals: dict[str, str] = {}  # last-seen kName → "literal" (for ShortLabels etc.)
    aliases: dict[tuple, str] = {}  # (path, kName) → qualified ref like ShortLabels::kInit

    ns_stack: list[str] = []
    brace_stack: list[str] = []  # 'ns' or 'block'
    pos = 0
    n = len(text)

    while pos < n:
        # Skip // comments
        if text.startswith("//", pos):
            nl = text.find("\n", pos)
            pos = n if nl < 0 else nl + 1
            continue
        # Skip /* */ comments
        if text.startswith("/*", pos):
            end = text.find("*/", pos + 2)
            pos = n if end < 0 else end + 2
            continue

        if text.startswith("namespace ", pos):
            m = re.match(r"namespace (\w+)\s*\{", text[pos:])
            if m:
                name = m.group(1)
                ns_stack.append(name)
                brace_stack.append("ns")
                pos += m.end()
                continue

        if text[pos] == "{":
            brace_stack.append("block")
            pos += 1
            continue

        if text[pos] == "}":
            if brace_stack:
                kind = brace_stack.pop()
                if kind == "ns" and ns_stack:
                    ns_stack.pop()
            pos += 1
            continue

        m = re.match(r"constexpr const char\* (k\w+)\s*=\s*", text[pos:])
        if m:
            kname = m.group(1)
            lit_start = pos + m.end()
            parsed = parse_string_literal_concat(text, lit_start)
            path = tuple(ns_stack)
            span_start = pos
            if parsed is not None:
                value, end = parsed
                if path and path[-1] == "ContextualHelp":
                    help_entries.append(
                        {
                            "path": path,
                            "kname": kname,
                            "value": value,
                            "start": span_start,
                            "end": end,
                        }
                    )
                else:
                    # Face / widget literals only — never let HELP copy pollute alias resolve.
                    labels[(path, kname)] = value
                    global_literals[kname] = value
                pos = end
                continue

            # Alias: ShortLabels::kInit / ChoiceLists::...::kDco1Dco2Mix / kPatchNameDisplay
            am = re.match(r"([A-Za-z0-9_:]+)\s*;", text[lit_start:])
            if am:
                ref = am.group(1)
                end = lit_start + am.end()
                if path and path[-1] == "ContextualHelp":
                    # e.g. kTitle = kPatchNameDisplay — not a unique string span to rewrite
                    pos = end
                    continue
                aliases[(path, kname)] = ref
                pos = end
                continue

            pos += m.end()
            continue

        pos += 1

    # Resolve widget aliases into labels map
    def resolve_ref(ref: str) -> str | None:
        leaf = ref.split("::")[-1]
        if leaf in global_literals:
            return global_literals[leaf]
        return None

    for (path, kname), ref in aliases.items():
        resolved = resolve_ref(ref)
        if resolved is not None:
            labels[(path, kname)] = resolved

    return help_entries, labels, global_literals


def lookup_label_in_parent(
    parent: tuple[str, ...],
    knames: tuple[str, ...],
    labels: dict[tuple, str],
) -> str | None:
    preferred_leaf = (
        "ParameterWidgets",
        "StandaloneWidgets",
        "ActionWidgets",
        "GroupLabels",
        "Buttons",
        "Widgets",
        "Header",
    )
    for kname in knames:
        preferred: str | None = None
        fallback: str | None = None
        for (lp, ln), lv in labels.items():
            if ln != kname or "ContextualHelp" in lp:
                continue
            if lp[: len(parent)] != parent:
                continue
            fallback = lv
            if len(lp) > len(parent) and lp[len(parent)] in preferred_leaf:
                preferred = lv
                break
        if preferred is not None:
            return preferred
        if fallback is not None:
            return fallback
    return None


def resolve_label(
    path: tuple[str, ...],
    kname: str,
    labels: dict[tuple, str],
) -> str | None:
    manual = MANUAL_LABELS.get(path)
    if manual is not None and kname in manual:
        return manual[kname]

    if kname in SKIP_KEYS:
        return None

    parent = tuple(p for p in path if p != "ContextualHelp")
    knames = KEY_ALIASES.get(kname, ()) + (kname,)
    found = lookup_label_in_parent(parent, knames, labels)
    if found is not None:
        return found

    return KEY_LABEL_FALLBACK.get(kname)


_KNOWN_VERB_STARTS = (
    "sets ",
    "set ",
    "selects ",
    "select ",
    "chooses ",
    "choose ",
    "enables ",
    "enable ",
    "disables ",
    "tracks ",
    "track ",
    "copies ",
    "copy ",
    "pastes ",
    "paste ",
    "resets ",
    "reset ",
    "sends ",
    "send ",
    "shows ",
    "show ",
    "opens ",
    "open ",
    "saves ",
    "save ",
    "loads ",
    "load ",
    "creates ",
    "create ",
    "deletes ",
    "delete ",
    "flushes ",
    "flush ",
    "exports ",
    "export ",
    "imports ",
    "import ",
    "stores ",
    "store ",
    "recalls ",
    "rolls ",
    "compares ",
    "steps ",
    "includes ",
    "include ",
    "mixes ",
    "mix ",
    "adds ",
    "add ",
    "hard-syncs ",
    "smooths ",
    "lets ",
    "light ",
    "lights ",
    "monitors ",
    "monitor ",
    "undoes ",
    "redoes ",
    "inverts ",
    "locks ",
    "protects ",
    "transposes ",
    "fine-tunes ",
    "balances ",
    "controls ",
    "mutates ",
)


def sentence_label(label: str) -> str:
    """Face label for mid-sentence use (drop trailing menu ellipsis)."""
    return label[:-3] if label.endswith("...") else label


def rewrite_message(module_msg: str, label: str) -> str:
    """MODULE: Rest → MODULE: LABEL rest (sentence-case → lower first letter)."""
    if ": " not in module_msg:
        return module_msg
    prefix, rest = module_msg.split(": ", 1)
    face = sentence_label(label)
    if face.upper() == prefix.upper():
        return module_msg  # avoid "MIDI: MIDI ..."
    # Idempotent: already rewritten
    if rest == face or rest.startswith(face + " "):
        return module_msg
    if rest and rest[0].isupper() and len(rest) > 1 and rest[1].islower():
        rest = rest[0].lower() + rest[1:]
    # Grammar polish when the old sentence started with a bare verb/noun phrase
    if rest.startswith("include "):
        rest = "includes " + rest[len("include ") :]
    elif rest.startswith("how much "):
        rest = "sets how much " + rest[len("how much ") :]
    elif rest.startswith("drag "):
        rest = "lets you drag " + rest[len("drag ") :]
    elif rest.startswith("fine detune of "):
        rest = "fine-tunes " + rest[len("fine detune of ") :]
    elif rest.startswith("fine detune "):
        rest = "fine-tunes " + rest[len("fine detune ") :]
    elif rest.startswith("delay before "):
        rest = "sets the delay before " + rest[len("delay before ") :]
    elif rest.startswith("time to "):
        rest = "sets the time to " + rest[len("time to ") :]
    elif rest.startswith("level held "):
        rest = "sets the level held " + rest[len("level held ") :]
    elif rest.startswith("overall envelope "):
        rest = "sets overall envelope " + rest[len("overall envelope ") :]
    elif rest.startswith("phase point "):
        rest = "sets the phase point " + rest[len("phase point ") :]
    elif rest.startswith("open a folder and save "):
        rest = "opens a folder and saves " + rest[len("open a folder and save ") :]
    elif rest.startswith("open a "):
        rest = "opens a " + rest[len("open a ") :]
    elif rest.startswith("open "):
        rest = "opens " + rest[len("open ") :]
    elif rest.startswith("init, copy, paste, and store "):
        rest = "groups init, copy, paste, and store " + rest[len("init, copy, paste, and store ") :]
    elif not any(rest.startswith(v) for v in _KNOWN_VERB_STARTS):
        # Noun / noun-phrase openers (group labels, etc.)
        rest = "is the " + rest
    return f"{prefix}: {face} {rest}"


def escape_for_cpp(s: str) -> str:
    return s.replace("\\", "\\\\").replace('"', '\\"')


def format_cpp_string_literal(value: str, continuation_indent: str) -> str:
    """Format value as one or more adjacent C string literals; spaces preserved at joins."""
    esc = escape_for_cpp(value)
    max_len = 100
    if len(esc) <= max_len:
        return f'"{esc}"'

    parts: list[str] = []
    remaining = esc
    while remaining:
        if len(remaining) <= max_len:
            parts.append(remaining)
            break
        cut = remaining.rfind(" ", 0, max_len)
        if cut < 40:
            cut = max_len
        # Keep the separating space on the *next* fragment so joins stay correct.
        parts.append(remaining[:cut])
        remaining = remaining[cut:]
        if remaining.startswith(" "):
            # move space to start of next part (already there)
            pass

    lines = [f'"{parts[0]}"']
    for part in parts[1:]:
        lines.append(f'{continuation_indent}"{part}"')
    return "\n".join(lines)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--write", action="store_true")
    args = ap.parse_args()

    text = HEADER.read_text()
    help_entries, labels, _globals = walk_header(text)

    module_style = [e for e in help_entries if re.match(r"^[A-Z0-9/].*: ", e["value"])]
    changes: list[tuple[dict, str, str | None]] = []
    for e in module_style:
        label = resolve_label(e["path"], e["kname"], labels)
        if label is None:
            changes.append((e, e["value"], None))
            continue
        new = rewrite_message(e["value"], label)
        changes.append((e, new, label))

    rewritten = [(e, new, lab) for e, new, lab in changes if lab is not None and new != e["value"]]
    skipped = [(e, lab) for e, new, lab in changes if lab is None]
    unchanged = [
        (e, lab) for e, new, lab in changes if lab is not None and new == e["value"]
    ]

    print(f"module-style help: {len(module_style)}")
    print(f"rewritten: {len(rewritten)}")
    print(f"skipped (no insert): {len(skipped)}")
    print(f"unchanged (label==prefix): {len(unchanged)}")

    lengths = sorted((len(new), new) for _, new, _ in rewritten)
    if lengths:
        print(f"longest rewritten ({lengths[-1][0]}): {lengths[-1][1]}")
        print(f"top 5:")
        for ln, msg in lengths[-5:][::-1]:
            print(f"  {ln}: {msg}")

    print("\nSamples:")
    for e, new, lab in rewritten[:8]:
        print(f"  {e['kname']} [{lab}]")
        print(f"    OLD: {e['value']}")
        print(f"    NEW: {new}")

    if skipped:
        print("\nSkipped keys:")
        for e, _ in skipped[:30]:
            print(f"  {'.'.join(e['path'][-3:])}.{e['kname']}: {e['value'][:70]}")

    if not args.write:
        print("\nDry-run only. Pass --write to apply.")
        return

    # Replace only the string-literal region (from first " to the char before ';'),
    # preserving `constexpr const char* kName =` and surrounding indentation.
    rewritten_sorted = sorted(rewritten, key=lambda t: t[0]["start"], reverse=True)
    out = text
    for e, new, _lab in rewritten_sorted:
        m = re.match(r"constexpr const char\* (k\w+)\s*=\s*", out[e["start"] :])
        if not m:
            raise RuntimeError(f"Cannot re-match assignment for {e['kname']}")
        lit_start = e["start"] + m.end()
        lit_end = e["end"] - 1  # position of ';'
        if out[lit_end] != ";":
            raise RuntimeError(f"Expected ';' at end of {e['kname']}")

        # Prefer matching the common style: newline after '=', then indented quotes
        # aligned with the first quote column (project convention).
        first_quote = out.find('"', lit_start, lit_end + 1)
        if first_quote < 0:
            raise RuntimeError(f"No quote for {e['kname']}")
        quote_line_start = out.rfind("\n", 0, first_quote) + 1
        cont = " " * (first_quote - quote_line_start)
        replacement = format_cpp_string_literal(new, cont)
        out = out[:first_quote] + replacement + out[lit_end:]

    HEADER.write_text(out)
    print(f"\nWrote {HEADER}")


if __name__ == "__main__":
    main()
