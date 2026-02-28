# Matrix-Control / GUI Implementation Details

---

- **Author:** Guillaume DUPONT
- **Organization:** Ten Square Software
- **Revision date:** 2025-08-08

---

This annex documents key technical choices and component specifications for the MatrixControl GUI. All implementation must comply with JUCE 8.0.12 standards and latest API conventions.

## 1. Scalability and Display

- **Vector graphics only:** All components are drawn using JUCE vector primitives (Paths, Shapes, etc.).
- **Scalable design:** The GUI responds to window resizing and user-selected GUI scale (presets: 50%, 75%, 90%, 100%, 125%, 150%, 200%, 250%, 300%, 400%).
- **High DPI support:** Retina/4K displays are natively supported by scaling all dimensions.

### ScalableComponentMixin (Utility Class)

Defines a reusable mixin for scaling all coordinates, sizes, line thicknesses, and font sizes:

```cpp
class ScalableComponentMixin
{
public:
    void setScaleFactor(float newScale);
    float getScaleFactor() const;
    int scaleDimension(int base) const;
    float scaleThickness(float base) const;
};
```

## 2. LookAndFeel Architecture

- **MatrixLookAndFeelBase:** Base theme derived from `juce::LookAndFeel_V4`.
- **MatrixLookAndFeelBlack / Cream:** Specializations for dark/light themes.
- **Centralized management:** Theme is switched globally via a settings menu, propagating to all components instantly.

## 3. Fonts & Typography

- All fonts use the new JUCE 8 API: `juce::FontOptions().withHeight(...).withTypefaceStyle(...)`.
- Typography is always centered and sized proportionally via the current scale factor.
- Recommended font: `"PT Sans Narrow"`, fallback to system font if unavailable.

## 4. Main Custom Components

### MatrixSlider

- **Drawing:** Pure vector, with focus border and value bar.
- **Standard size:** 120×36 px (Retina), scalable.
- **Color codes:**  
  - Background: `#022d10`
  - Focus border: `#0b461e`
  - Value bar: `#0f775e`
  - Value text: `#24dcae`
- **Interaction:** Value changes by vertical drag or mouse wheel; border color indicates focus.
- **Calibration:** Configurable (integer/float), min/max/default/step.

### MatrixEnvelope

- **Drawing:** Editable ADSR envelope, multi-segment.
- **Handles:** Interactive points for Delay, Attack, Decay, Sustain, Release.
- **Feedback:** Parameter value shown while dragging.
- **Grid:** Optional for precise editing.

### MatrixButton, MatrixComboBox, MatrixNumber, MatrixLed, etc.

- All components follow the same scaling logic and theme rules.
- LED components always visible, independent from selected tab.

## 5. Theme and Scale Management

- **ThemeManager:** Singleton class, exposes current theme and handles switching.
- **Scale control:** Exposed in GUI preferences; affects all components instantly via `AffineTransform::scale()`.

## 6. Implementation Recommendations

- **Performance:** Use `juce::CachedComponentImage` for complex drawing.
- **Responsiveness:** Avoid blocking operations on the GUI thread.
- **Testing:** All components must be tested in a standalone JUCE app before DAW integration.

## 7. Example Code Snippets

**Font Scaling:**

```cpp
juce::Font getScaledFont(float baseHeight) const
{
    return juce::Font(juce::FontOptions().withHeight(baseHeight * getScaleFactor()).withTypefaceStyle("Regular"));
}
```

**MatrixSlider Paint Example:**

```cpp
void MatrixSlider::paint(juce::Graphics& g)
{
    auto scale = getScaleFactor();
    // Draw background, focus border, value bar, and centered value text using scaled dimensions
}
```

## 8. To Be Defined / Proposed

- Accessibility options (keyboard navigation, colorblind-friendly themes)
- Touch/trackpad gesture support for controls
- Internationalization plan (if needed)
- Automated GUI tests

---

Copyright © 2025 Ten Square Software. All rights reserved.