# TrackGeneratorDisplay Widget Implementation

**Status:** Completed (2026-02-06)

## Overview

Complete the TrackGeneratorDisplay widget implementation with curve drawing, value setters, and mouse interaction for editing the 5-point transfer function curve. The widget displays 5 points connected by line segments, where points are equidistant on X-axis and only Y values are adjustable (range 0-63).

## Architecture

The widget follows the existing pattern established in the codebase:

- Inherits from `juce::Component` (not `juce::Slider`)
- Uses image caching for performance (like `Slider` and `NumberBox`)
- Integrates with APVTS indirectly via value setters
- Handles mouse interaction for direct manipulation

## Implementation Steps (Completed)

### 1. Add Data Members and Constants

- Constants: `kPointCount_`, `kPadding_`, `kPointRadius_`, `kHitZoneRadius_`, `kLineThickness_`, `kMinValue_`, `kMaxValue_`
- Data members: `pointValues_`, `draggedPointIndex_`, cache (`cachedImage_`, `cacheValid_`, `cachedPointValues_`), `cachedCurveColour_`, `onValueChanged_`

### 2. Add Public Setters

- `setTrackPoint1` … `setTrackPoint5` / `getTrackPoint1` … `getTrackPoint5`
- `setOnValueChanged(ValueChangedCallback)`

### 3. Implement Cache System

- `regenerateCache()`, `invalidateCache()`, `updateSkinCache()`
- Cache drawn at widget dimensions (no double scaling)

### 4. Implement Curve Drawing

- `drawCurve()`, `calculatePointPosition()`
- Draw area: `bounds.reduced(kPadding_)`
- Points X: equidistant; Y: value 0–63 mapped to height (top=63, bottom=0)
- Colour: `ColourChart::kGreen4`, filled circles for points

### 5. Implement Mouse Interaction

- `mouseDown` / `mouseDrag` / `mouseUp`
- `findPointAtPosition()` with `kHitZoneRadius_`
- Vertical drag only on points; callback on value change

### 6–8. paint(), setSkin(), Setters

- `paint()` uses cached curve image
- `setSkin()` invalidates cache
- All setters clamp to 0–63 and invalidate cache on change

## Key Design Decisions

1. **No base class:** TrackGeneratorDisplay and EnvelopeDisplay implemented separately first.
2. **Image caching:** For performance; invalidated on value or skin change.
3. **Indirect APVTS:** Setters + callback; parent/PluginProcessor handle APVTS sync.
4. **Value range:** 0–63 (Matrix-1000 Track Generator).
5. **Mouse:** Vertical drag on points only; 10px hit zone.

## Files Modified

- `Source/GUI/Widgets/TrackGeneratorDisplay.h`
- `Source/GUI/Widgets/TrackGeneratorDisplay.cpp`
