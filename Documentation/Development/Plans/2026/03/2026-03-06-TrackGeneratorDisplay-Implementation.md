---
name: TrackGeneratorDisplay Implementation
overview: Complete the TrackGeneratorDisplay widget implementation with curve drawing, value setters, and mouse interaction for editing the 5-point transfer function curve.
todos:
  - id: add_constants
    content: Add constants (kPointCount_, kPadding_, kPointRadius_, etc.) to TrackGeneratorDisplay.h
    status: completed
  - id: add_members
    content: Add data members (pointValues_, draggedPointIndex_, cache members) to TrackGeneratorDisplay.h
    status: completed
  - id: add_setters
    content: Add public setters/getters and callback mechanism to TrackGeneratorDisplay.h
    status: completed
  - id: add_private_methods
    content: Add private methods declarations (cache, drawing, mouse helpers) to TrackGeneratorDisplay.h
    status: completed
  - id: implement_cache
    content: Implement cache system (regenerateCache, invalidateCache, updateSkinCache) in TrackGeneratorDisplay.cpp
    status: completed
  - id: implement_drawing
    content: Implement curve drawing (drawCurve, calculatePointPosition) in TrackGeneratorDisplay.cpp
    status: completed
  - id: implement_mouse
    content: Implement mouse interaction (mouseDown, mouseDrag, mouseUp, findPointAtPosition) in TrackGeneratorDisplay.cpp
    status: completed
  - id: update_paint
    content: Update paint() method to use cached curve in TrackGeneratorDisplay.cpp
    status: completed
  - id: update_setskin
    content: Update setSkin() to invalidate cache in TrackGeneratorDisplay.cpp
    status: completed
  - id: implement_setters
    content: Implement all 5 setters (setTrackPoint1-5) in TrackGeneratorDisplay.cpp
    status: completed
isProject: false
---

# TrackGeneratorDisplay Widget Implementation

## Overview

Implement the complete TrackGeneratorDisplay widget that visualizes and allows interactive editing of the Matrix-1000's Track Generator transfer function. The widget displays 5 points connected by line segments, where points are equidistant on X-axis and only Y values are adjustable (range 0-63).

## Architecture

The widget follows the existing pattern established in the codebase:

- Inherits from `juce::Component` (not `juce::Slider`)
- Uses image caching for performance (like `Slider` and `NumberBox`)
- Integrates with APVTS indirectly via value setters
- Handles mouse interaction for direct manipulation

## Implementation Steps

### 1. Add Data Members and Constants

Update `[Source/GUI/Widgets/TrackGeneratorDisplay.h](Source/GUI/Widgets/TrackGeneratorDisplay.h)`:

**Add constants:**

```cpp
inline constexpr static int kPointCount_ = 5;
inline constexpr static float kPadding_ = 5.0f;
inline constexpr static float kPointRadius_ = 5.0f;
inline constexpr static float kHitZoneRadius_ = 10.0f;
inline constexpr static float kLineThickness_ = 2.0f;
inline constexpr static int kMinValue_ = 0;
inline constexpr static int kMaxValue_ = 63;
```

**Add data members:**

```cpp
std::array<int, kPointCount_> pointValues_ {0, 15, 31, 47, 63}; // Default linear
int draggedPointIndex_ = -1; // -1 means no point being dragged

// Image cache
juce::Image cachedImage_;
bool cacheValid_ = false;
std::array<int, kPointCount_> cachedPointValues_;

// Skin cache
juce::Colour cachedCurveColour_;
```

### 2. Add Public Setters

Add value setters that will be called when APVTS parameters change:

```cpp
void setTrackPoint1(int value);
void setTrackPoint2(int value);
void setTrackPoint3(int value);
void setTrackPoint4(int value);
void setTrackPoint5(int value);

int getTrackPoint1() const { return pointValues_[0]; }
int getTrackPoint2() const { return pointValues_[1]; }
int getTrackPoint3() const { return pointValues_[2]; }
int getTrackPoint4() const { return pointValues_[3]; }
int getTrackPoint5() const { return pointValues_[4]; }
```

Add callback mechanism for notifying value changes:

```cpp
using ValueChangedCallback = std::function<void(int pointIndex, int newValue)>;
void setOnValueChanged(ValueChangedCallback callback);

private:
ValueChangedCallback onValueChanged_;
```

### 3. Implement Cache System

Add private methods for cache management:

```cpp
void regenerateCache();
void invalidateCache();
void updateSkinCache();
float getPixelScale() const;
```

Implementation pattern similar to `Slider`:

- `invalidateCache()`: marks `cacheValid_ = false` and calls `repaint()`
- `regenerateCache()`: creates offscreen image, draws curve, updates cache
- Check if values changed before regenerating

### 4. Implement Curve Drawing

Add private drawing methods in `[Source/GUI/Widgets/TrackGeneratorDisplay.cpp](Source/GUI/Widgets/TrackGeneratorDisplay.cpp)`:

```cpp
void drawCurve(juce::Graphics& g, const juce::Rectangle<float>& bounds);
juce::Point<float> calculatePointPosition(int pointIndex, const juce::Rectangle<float>& bounds) const;
```

**Drawing logic:**

- Use `bounds.reduced(kPadding_)` for drawable area
- Points X positions: equidistant (0%, 25%, 50%, 75%, 100% of width)
- Points Y positions: map value (0-63) to height (top=63, bottom=0)
- Draw line segments with `ColourChart::kGreen4` and `kLineThickness_`
- Draw filled circles at each point with `ColourChart::kGreen4` and `kPointRadius_`

### 5. Implement Mouse Interaction

Add mouse event handlers:

```cpp
void mouseDown(const juce::MouseEvent& e) override;
void mouseDrag(const juce::MouseEvent& e) override;
void mouseUp(const juce::MouseEvent& e) override;
```

**Mouse interaction logic:**

**mouseDown:**

- Find nearest point within `kHitZoneRadius_`
- Store index in `draggedPointIndex_`
- If no point found, `draggedPointIndex_ = -1`

**mouseDrag:**

- If `draggedPointIndex_ >= 0`:
  - Convert mouse Y to value (0-63, inverted: top=63, bottom=0)
  - Clamp to `[kMinValue_, kMaxValue_]`
  - Update `pointValues_[draggedPointIndex_]`
  - Invalidate cache
  - Notify via callback: `if (onValueChanged_) onValueChanged_(draggedPointIndex_, newValue);`

**mouseUp:**

- Reset `draggedPointIndex_ = -1`

Add helper method:

```cpp
int findPointAtPosition(const juce::Point<float>& position, const juce::Rectangle<float>& bounds) const;
```

### 6. Update paint() Method

Modify `paint()` to use cache:

```cpp
void TrackGeneratorDisplay::paint(juce::Graphics& g)
{
    if (skin_ == nullptr)
        return;

    const auto bounds = getLocalBounds().toFloat();
    const auto contentBounds = bounds.reduced(0.0f, static_cast<float>(kVerticalPadding_));

    // Draw background and border (existing code)
    drawBackground(g, contentBounds);
    drawBorder(g, contentBounds);
    drawTriangle(g, contentBounds);
    
    // Draw cached curve
    if (!cacheValid_)
        regenerateCache();
    
    g.drawImageAt(cachedImage_, 0, kVerticalPadding_);
}
```

### 7. Update setSkin() Method

Invalidate cache when skin changes:

```cpp
void TrackGeneratorDisplay::setSkin(tss::Skin& skin)
{
    skin_ = &skin;
    updateSkinCache();
    invalidateCache();
}
```

### 8. Implement Setters

Each setter follows the same pattern:

```cpp
void TrackGeneratorDisplay::setTrackPoint1(int value)
{
    const int clampedValue = juce::jlimit(kMinValue_, kMaxValue_, value);
    
    if (pointValues_[0] != clampedValue)
    {
        pointValues_[0] = clampedValue;
        invalidateCache();
    }
}
```

## Key Design Decisions

1. **No base class**: TrackGeneratorDisplay and EnvelopeDisplay will be implemented separately first, then refactored if needed after observing duplication patterns.
2. **Image caching**: Essential for performance, especially with mouse dragging. Cache is invalidated only when values or skin change.
3. **Indirect APVTS integration**: The widget exposes setters and callbacks. The actual APVTS synchronization will be handled by parent panels or PluginProcessor (similar to NumberBox pattern).
4. **Value range**: Fixed to 0-63 as per Matrix-1000 specification for Track Generator points.
5. **Mouse interaction**: Vertical drag only on points (not segments), with generous hit zone (10px radius) for easier manipulation.

## Testing Strategy

After implementation:

1. Verify curve draws correctly with default values (linear 0, 15, 31, 47, 63)
2. Test mouse drag on each point
3. Verify callback notifications
4. Test extreme values (all 0, all 63, random patterns)
5. Verify cache invalidation works correctly
6. Test skin switching

## Files Modified

- `[Source/GUI/Widgets/TrackGeneratorDisplay.h](Source/GUI/Widgets/TrackGeneratorDisplay.h)` - Add members, constants, methods
- `[Source/GUI/Widgets/TrackGeneratorDisplay.cpp](Source/GUI/Widgets/TrackGeneratorDisplay.cpp)` - Implement curve drawing and interaction

