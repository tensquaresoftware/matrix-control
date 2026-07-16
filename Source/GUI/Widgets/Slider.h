#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace TSS
{
    struct SliderConfig
    {
        double minValue = 0.0;
        double maxValue = 100.0;
        double defaultValue = 0.0;
        double step = 1.0;
        juce::String unit;
        juce::String minimumDisplayText;
        std::function<float(double)> normalizedFill;
        std::function<juce::String(double)> formatValue;
    };

    /** Linear bar slider. Width and height are design dimensions; \p look carries full widget styling
        including the complete \p SliderLook::font (family, weight, height) as for \p Button. */
    class Slider : public juce::Slider
    {
    public:
        Slider(int width, int height, const SliderLook& look, const SliderConfig& config);
        ~Slider() override = default;

        void setLook(const SliderLook& look);
        void setUiScale(float uiScale);

        juce::String getUnit() const;

        void paint(juce::Graphics& g) override;

        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent&) override;
        void mouseDoubleClick(const juce::MouseEvent&) override;

        void focusGained(juce::Component::FocusChangeType cause) override;
        void focusLost(juce::Component::FocusChangeType cause) override;

        bool keyPressed(const juce::KeyPress& key) override;

    private:
        inline constexpr static int kValueBarPadding_ = 1;
        inline constexpr static float kFocusBorderThickness_ = 1.0f;
        inline constexpr static double kDragSensitivity_ = 0.5;
        inline constexpr static double kShiftStepMultiplier_ = 10.0;

        SliderLook look_{};
        int width_;
        int height_;
        float uiScale_ = 1.0f;
        double defaultValue_ = 0.0;
        double step_ = 1.0;
        int valueDecimalPlaces_ = 0;
        double dragStartValue_ = 0.0;
        juce::Point<int> dragStartPosition_;
        juce::String unit_;
        juce::String minimumDisplayText_;
        std::function<float(double)> normalizedFill_;
        std::function<juce::String(double)> formatValue_;
        bool hasFocus_ = false;

        void drawTrack(juce::Graphics& g, const juce::Rectangle<int>& bounds, bool enabled);
        void drawValueBar(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled);
        void drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled);
        void drawFocusBorderIfNeeded(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool hasFocus);

        juce::Rectangle<float> calculateValueBarBounds(const juce::Rectangle<int>& trackBoundsInt, int insetPerSide) const;

        static int countDecimalPlacesForStep(double step);
        double calculateStep(bool isShiftPressed) const;
        bool isIncrementKey(int keyCode) const;
        bool isDecrementKey(int keyCode) const;
        void updateValueWithStep(double step, bool increment);
        void resetToDefaultValue();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Slider)
    };
}
