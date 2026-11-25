---
title: 'Logo Alt/Option click opens Audio/MIDI'
type: 'feature'
created: '2026-07-25'
status: 'done'
route: 'one-shot'
---

# Logo Alt/Option click opens Audio/MIDI

## Intent

**Problem:** Opening Audio/MIDI settings in standalone required digging into the MATRIX-CONTROL logo menu, which is slower than needed for a frequent setup action.

**Approach:** Add Alt+click (Windows/Linux) / Option+click (macOS) on the logo to open the same Audio/MIDI dialog as the menu item, standalone-only, without breaking existing logo gestures.

## Suggested Review Order

**Logo gesture dispatch**

- Defer Alt/Option via the click/double-click timer so double-click only resets scale.
  [`Logo.cpp:77`](../../Source/GUI/Widgets/Logo.cpp#L77)

- Fire Audio/MIDI from the timer when Alt was held; otherwise open the popup.
  [`Logo.cpp:92`](../../Source/GUI/Widgets/Logo.cpp#L92)

- New callback + pending flag on Logo.
  [`Logo.h:23`](../../Source/GUI/Widgets/Logo.h#L23)

**Standalone-only wiring**

- Guard plugin mode before forwarding to the existing Audio/MIDI path.
  [`HeaderPanel.cpp:99`](../../Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp#L99)

**Docs**

- Document logo shortcuts including Option/Alt + click.
  [`manuel-utilisateur.md:452`](../../Documentation/Users/manuel-utilisateur.md#L452)
