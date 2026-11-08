---
id: SPEC-skin-editor
companions:
  - thin-bridge.md
  - dev-runtime-layer.md
  - json-bounds.md
  - cmake-packaging.md
  - editor-surfaces.md
  - code-map.md
  - ../spec-skins-colour-model-ssot/SPEC.md
  - ../spec-skins-colour-model-ssot/target-model.md
  - ../../../CONVENTIONS.md
  - ../../project-context.md
sources:
  - ../../forge/skin-editor/forged-idea.md
  - ../../forge/skin-editor/.memlog.md
  - ../../forge/mode-dev-skins/forged-idea.md
---

> **Canonical contract.** This SPEC and the files in `companions:` are the complete, preservation-validated contract for what to build, test, and validate. Source documents listed in frontmatter are for traceability — consult them only if you need narrative rationale or prose color this contract intentionally omits.

# Matrix-Control Skin Editor (V1 author tool)

## Why

Skin SSOT (closed palette, structured object × property × state mappings, Black/Cream, Looks) is delivered; inventing and exploring skins still means editing C++ by hand. This vision realizes a **Dev author tool** — a separate JUCE Skin Editor that pushes a runtime Dev layer to Standalone Dev so truth stays real product paint — without reopening the model gate, without a web shell, and without holding multi-OS alpha hostage. Christmas 2026 stays adaptive: editor is the preferred invent path; Black/Cream Skin SSOT alone remain the alpha safety net.

## Capabilities

- **CAP-1**
  - **intent:** An operator can run **Matrix-Control Skin Editor** as a separate Standalone JUCE process beside Standalone Dev and connect a thin one-way push bridge so model changes reach the product paint surface.
  - **success:** With both processes up, a push from the editor is acknowledged; Standalone Dev applies the payload and repaints. With Standalone down, the editor still edits the Dev layer offline and shows disconnected status (no fake product preview).

- **CAP-2**
  - **intent:** The operator can load compiled Skin SSOT into a Dev runtime layer, **Duplicate** Black/Cream into a **named Dev skin**, and select/create/delete Dev skins without mutating compiled product skins in place.
  - **success:** Black/Cream remain read-only; Duplicate yields an editable named Dev skin; select switches the active Dev (or read-only BC) identity; delete removes only Dev-layer skins. Compiled C++ tables are unchanged.

- **CAP-3**
  - **intent:** On a Dev skin, the operator can edit palette pastilles and structured mappings and push them so Standalone Dev live-paints from that layer.
  - **success:** Changing a Dev mapping or pastille and pushing updates Standalone Dev appearance for the affected slots without rebuilding; judging fidelity uses Standalone paint only.

- **CAP-4**
  - **intent:** The operator can see **Unbound** structured keys as text in the editor (object × property × state names) and bind them to palette entries.
  - **success:** Unbound list reflects missing mappings for the active Dev skin; binding creates a mapping, clears that row from Unbound, and a subsequent push updates Standalone Dev. No Unbound overlay appears on Matrix-Control paint.

- **CAP-5**
  - **intent:** The operator can export/import the Dev runtime layer as Dev-only JSON for session portability and handoff to a later C++ integration step.
  - **success:** Export then import into a fresh editor session restores palette + mappings + Dev skin names (round-trip). **Mandatory** for Skin Editor V1 done (`json-bounds.md`).

- **CAP-6**
  - **intent:** Release user builds stay free of the Skin Editor binary and of the Dev receiver; authoring stays same-repo Dev CMake targets.
  - **success:** Release packaging omits the editor; AU/VST3 and Release Standalone lack the author receiver; Dev preset builds Skin Editor + Standalone Dev receiver only.

## Constraints

- Branches onto delivered **Skin SSOT**; does not invent a third colour system or reopen the model gate unless a hard contradiction appears.
- **Form:** separate Standalone JUCE app (*Matrix-Control Skin Editor*), native widgets; workflow assumes Matrix-Control fullscreen + editor on an external display.
- **Contract:** one-way push of palette + mappings + active skin choice → Standalone Dev applies/repaints (`thin-bridge.md`). No product GUI preview in the editor. No bidirectional IDE / shared undo in V1.
- **Truth surface:** Standalone Dev only — no author IPC to plugin-in-DAW in V1.
- **Repo:** same Matrix-Control repo; separate CMake Dev target (`cmake-packaging.md`). No separate repo V1.
- **Data truth:** C++ = product SSOT; editor = Dev runtime layer; JSON = save/load of that layer only (`dev-runtime-layer.md`, `json-bounds.md`).
- **Product integration handoff:** promoting an invented Dev skin (new pastilles, mappings, display name) into shipped Matrix-Control is a **separate BMad-assisted C++ source update** — not automatic JSON→Release, not in-editor codegen. Name polish may happen at that last C++ step.
- Dev skins may **add new pastille names** beyond compiled ColourChart (plus remap existing); those names exist in the Dev layer until a later C++ palette update lands them in product SSOT.
- Dev skin **names are free-text**; operator should prefer definitive display names intended for later Matrix-Control UI (no fixed slot list / no hard concurrent cap in V1).
- Compiled **Black/Cream** are read-only; edits require **Duplicate** to a Dev skin first.
- **Unbound** is central and **editor-text only** — no Unbound overlay on product paint.
- **Ship threshold (editor V1 done)** = Dev skin CRUD + live paint + actionable Unbound + **JSON export/import round-trip** (`json-bounds.md`). No editor-V1-done without JSON I/O.
- **Alpha net:** multi-OS alpha proceeds with current Black/Cream Skin SSOT; editor (including JSON) desirable, **non-blocking** for alpha.
- Skin Studio HTML remains **frozen** V1 — no Lot C, no web shell for this editor.
- English-only source and UI strings; JUCE 9.0.3; quality via `Scripts/quality/lint_touched.py` on touched C++; follow `CONVENTIONS.md` / `project-context.md`.
- Orbitron = brand font only; PatchNameDisplay stays on its Skin SSOT / typography rules — do not conflate.
- No local shrink/hex shortcuts on the product user GUI path introduced “for the editor”.

## Non-goals

- Implementing this editor or changing Skin SSOT model tables in the spec chat itself.
- Skin Studio Lot C / pixel-near HTML GUI / web shell for the editor.
- Proud aesthetic recalibration of shipped Black/Cream as a requirement of this tool contract (tool enables invent; C++ integration is later).
- Automatic promotion of Dev layer / JSON into Release C++ SSOT inside the editor (the handoff tool is a later BMad C++ change, not the editor).
- External contributors, Dev build distribution, or five shipped skins as V1 editor requirements.
- JSON as Release product SSOT; file skins on the user Release path.
- In-layout edit panel mixed into Release MainComponent; in-place edit of compiled Black/Cream.
- Rich sync / shared undo / live selection IDE between editor and product; Unbound highlight on Matrix-Control paint.
- Author telecommand into a DAW-hosted plugin instance.
- Holding multi-OS alpha or Christmas plan hostage to editor completion.
- Running `bmad-build`, commits, push, or merge from this spec alone.

## Success signal

On a Dev build pair (Skin Editor + Standalone Dev), an operator Duplicates Cream (or Black) to a named Dev skin, rebinds at least one mapping (and clears an Unbound row if present), pushes and sees Standalone Dev repaint live, then **exports JSON, restarts the editor, imports that file**, and recovers the same Dev layer (push again if connected) — without a product preview inside the editor and without touching compiled SSOT. Alpha can still ship Black/Cream without this tool.

## Assumptions

- Skin SSOT model gate is available on the branch/main this editor will target (PR #34 / merge state as of forge).
- Thin-bridge transport is an implementer choice within `thin-bridge.md` (local, one-way, minimal ack/errors).
- Unbound inventory = structured keys expected by product Skin SSOT for which the active Dev skin has no mapping (or an explicit unbound marker).
- Calendar phrases like “a few days” are aspiration only; the **alpha non-blocking** net stays in force (independent of editor/JSON).
- JSON is the expected portable handoff into the later BMad C++ integration step; it still does not auto-write Release SSOT.

## Next (for `/bmad-build`)

**Ready** to feed **`/bmad-build` on this spec alone** — open questions locked; JSON mandatory for editor V1 done. Do **not** reopen the Skin SSOT model forge/spec or Skin Studio Lot C. Suggested review order: `code-map.md`. Story breakdown optional if Guillaume wants sliced CAP delivery.
