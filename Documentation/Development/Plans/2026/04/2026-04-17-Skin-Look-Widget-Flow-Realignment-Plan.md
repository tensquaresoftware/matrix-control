---
name: Skin-Look-Dependency-Realignment
overview: Refactor the typography/looks flow so Skin remains the source theme, LookBuilders assemble widget looks, and TypographyStyles no longer depends directly on ISkin.
todos:
  - id: archive-plan-in-project
    content: Copy initial and final Cursor plan markdown into Documentation/Development/Plans/2026/04 with project naming convention
    status: completed
  - id: decouple-typography-from-skin
    content: Refactor TypographyStyles API to remove direct ISkin dependency and expose neutral typography descriptors
    status: completed
  - id: move-font-composition-to-lookbuilders
    content: Implement Skin + Typography composition helper in LookBuilders and migrate all typography uses
    status: completed
  - id: validate-build-and-lints
    content: Run build and lint checks, then verify typography rendering parity
    status: completed
isProject: false
---

# Realign Skin-Look-Widget Flow

## Goal
Make the code match your target flow:
- Skin provides theme data
- LookBuilders construct widget looks
- Widgets only consume looks

## Target Design
- `TypographyStyles` becomes independent from `ISkin`.
- `TypographyStyles` only defines typography variants (size + weight intent).
- `LookBuilders` becomes the only place that combines `Skin` + `TypographyStyles` into final `juce::Font` inside each `*Look`.

## Planned Changes
- Update typography API in:
  - [/Volumes/Guillaume/Dev/SDKs/JUCE/Projects/Matrix-Control/Source/GUI/Looks/TypographyStyles.h](/Volumes/Guillaume/Dev/SDKs/JUCE/Projects/Matrix-Control/Source/GUI/Looks/TypographyStyles.h)
  - [/Volumes/Guillaume/Dev/SDKs/JUCE/Projects/Matrix-Control/Source/GUI/Looks/TypographyStyles.cpp](/Volumes/Guillaume/Dev/SDKs/JUCE/Projects/Matrix-Control/Source/GUI/Looks/TypographyStyles.cpp)
- Keep only typography description there (style id + numeric size + bold flag).
- Move Skin-dependent font creation into:
  - [/Volumes/Guillaume/Dev/SDKs/JUCE/Projects/Matrix-Control/Source/GUI/Looks/LookBuilders.cpp](/Volumes/Guillaume/Dev/SDKs/JUCE/Projects/Matrix-Control/Source/GUI/Looks/LookBuilders.cpp)
- Add a small helper in `LookBuilders.cpp` that:
  - selects base family from skin (`getBaseFont()` or `getBaseFontBold()`)
  - applies typography style height
  - returns final `juce::Font`
- Replace direct `typographyStyleFromSkin(...)` calls with that helper.

## Validation
- Build the project in `Builds/macOS`.
- Run lint checks on modified files.
- Quick sanity pass to ensure visual font behavior is unchanged (same sizes/styles as before).

## Expected Outcome
- Dependency direction becomes clearer and aligned with your vision.
- `TypographyStyles` is reusable and neutral.
- `LookBuilders` is the explicit composition point from theme to widget look.
