# Thin bridge — Skin Editor → Standalone Dev

Load-bearing contract for the one-way telecommand. Visual truth remains Standalone Dev paint.

## Roles

| Process | Role |
|---------|------|
| **Matrix-Control Skin Editor** | Author UI; owns Dev runtime layer edits; **pushes** model only |
| **Matrix-Control Standalone (Dev build)** | Receives push; applies layer; **repaints** product GUI |

No product GUI preview, reconstituted chrome, or Unbound overlay inside the editor. No author path to a plugin hosted in a DAW in V1.

## Payload (push)

Each successful push carries enough for Standalone Dev to replace its active Dev presentation:

1. **Active skin identity** — which named Dev skin (or compiled Black/Cream read-only view) is selected.
2. **Palette** — named pastilles for that skin’s Dev layer (see `dev-runtime-layer.md`).
3. **Mappings** — structured `(ObjectType, Property, State)` → palette entry for that skin.

Partial pushes (palette-only, mappings-only, or selection-only) are allowed if the receiver can apply them without inventing missing state; full replace of the active Dev skin is the safe default.

## Direction and richness

- **One-way:** Editor → Standalone Dev.
- **Not in V1:** shared undo, selection sync, continuous bidirectional IDE, live widget pickers on the product paint, Unbound highlight on Matrix-Control.

## Ack / errors (minimal)

Receiver replies with a small status only:

- **ack** — applied (optionally include applied skin id / revision).
- **error** — not applied; short machine-readable code + human message (e.g. unknown skin, malformed payload, receiver not in Dev mode, apply failed).

No streaming diagnostics channel. Editor shows connection/apply status in plain UI text.

## Lifecycle (two processes)

1. Operator launches **Standalone Dev** (truth surface) and **Skin Editor** (author).
2. Editor discovers / connects to the local Standalone Dev receiver.
3. If Standalone Dev is absent: editor remains usable for offline Dev-layer edit; live paint is unavailable until connect (no fake preview).
4. On connect: editor may push current active Dev skin (or wait for first explicit edit — implementer choice; prefer push-on-connect of active skin so surfaces match).
5. Disconnect / Standalone quit: editor keeps Dev layer in memory; Standalone falls back to compiled Skin SSOT (Black/Cream) — Dev overlay is not product SSOT.
6. Editor quit does not leave a durable Release path; session Dev layer is lost unless JSON lot saved it.

## Transport

Mechanism is an **implementation choice** (e.g. JUCE `InterprocessConnection`, localhost socket) provided it stays:

- local machine only;
- thin push + minimal ack/errors;
- gated behind Dev Standalone receiver (see `cmake-packaging.md`).

Do not invent a second paint stack or a web shell to avoid IPC.

## Forbidden on this bridge

- Shipping receiver into Release user plugins / Release Standalone packaging.
- Authoring toward in-DAW plugin instances.
- Treating JSON files on disk as the live bridge (JSON is optional save/load of the Dev layer, not the paint pipe).
