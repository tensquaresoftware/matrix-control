# PRD Addendum — Matrix-Control

Technical detail that supports the PRD but does not belong in the main FR narrative.

---

## Interactive display geometry — EnvelopeDisplay

Source: Guillaume, Phase 0 zone 3 workshop (2026-05-27). Values 0–63 match Matrix-1000 SysEx (6-bit positive parameters).

### Layout

- Light padding around the panel so control points do not touch edges.
- Start point: fixed bottom-left (Note On start).

### Segments and drag behaviour

| Parameter | Horizontal | Vertical | Max X span (value 63) | Max Y span (value 63) | Notes |
|---|---|---|---|---|---|
| Delay | Draggable (duration) | Fixed at bottom | ~20% panel width | — | Segment always horizontal at bottom |
| Attack | Draggable (duration) | Fixed at top | ~20% panel width | — | At value 0, segment intentionally not fully vertical |
| Decay | Draggable (duration) | Follows Sustain level | ~20% panel width | ~100% panel height | At value 0, segment intentionally not fully vertical; **no direct vertical drag on Decay point** (avoids simultaneous Decay + Sustain SysEx) |
| Sustain | Fixed segment length | Draggable via sustain point/segment | ~20% panel width (fixed) | ~100% panel height | Horizontal segment translates vertically |
| Release | Draggable (duration) | Fixed at bottom | ~20% panel width | — | At value 0, segment intentionally not fully vertical |

### Visual markers (D-017)

- Replace filled/hollow circles with **small filled squares** at junctions (80's rectangular GUI aesthetic).

---

## Interactive display geometry — TrackGeneratorDisplay

- Same outer padding as envelope displays.
- Five points: **vertical drag only** (Y maps 0–63 to ~100% panel height).
- Points spaced evenly on X; each segment ~25% panel width.
- Markers: small filled squares (same as envelope displays).

---

## Target sync architecture (D-018)

**Single source of truth:** APVTS `AudioParameter` values (0–63 normalised).

**Slider → curve:** existing `SliderParameterAttachment` updates APVTS; display listens to parameter change and repaints (`notify = false` on display setters to avoid feedback loops).

**Curve → slider:** display writes `parameter.setValueNotifyingHost(normalised)` (with `beginChangeGesture` / `endChangeGesture` during mouse drag); JUCE attachments update sliders automatically — **no** manual `slider.setValue()` from the display.

**External updates** (preset load, DAW automation, MIDI echo from synth): same parameter listeners refresh both sliders (via attachments) and displays.

**PluginProcessor:** reacts to parameter changes for SysEx / business logic — not for UI resync.

**Refactor scope:** remove Display→Slider bridge in `PatchEditPanel`; replace `PatchEditDisplaysPanel` paramId if-chain with descriptor-driven mapping table.
