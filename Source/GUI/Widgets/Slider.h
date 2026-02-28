#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
    class ISkin;

    class Slider : public juce::Slider
    {
    public:
        explicit Slider(ISkin& skin, int width, int height, double defaultValue = 0.0);
        ~Slider() override = default;

        void setSkin(ISkin& skin);

        void setUnit(const juce::String& unit);
        juce::String getUnit() const;

        void paint(juce::Graphics& g) override;
        void resized() override;

        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent&) override;
        void mouseDoubleClick(const juce::MouseEvent&) override;

        void focusGained(juce::Component::FocusChangeType cause) override;
        void focusLost(juce::Component::FocusChangeType cause) override;

        bool keyPressed(const juce::KeyPress& key) override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        inline constexpr static int kTrackHeight_ = 16;
        inline constexpr static double kDragSensitivity_ = 0.5;
        inline constexpr static double kShiftKeyStep_ = 10.0;

        ISkin* skin_ = nullptr;
        
        int width_;
        int height_;
        double defaultValue_ = 0.0;
        double dragStartValue_ = 0.0;
        juce::Point<int> dragStartPosition_;
        juce::String unit_;
        bool hasFocus_ = false;
        
        // Image cache
        juce::Image cachedImage_;
        bool cacheValid_ = false;
        double cachedValue_ = 0.0;

        // Skin cache
        juce::Colour cachedTrackColour_;
        juce::Colour cachedValueBarColour_;
        juce::Colour cachedTextColour_;
        juce::Colour cachedFocusBorderColour_;
        juce::Font cachedFont_;

        void regenerateCache();
        void invalidateCache();
        void updateSkinCache();
        float getPixelScale() const;

        void drawTrack(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled);
        void drawValueBar(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled);
        void drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled);
        void drawFocusBorderIfNeeded(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool hasFocus);
        
        juce::Rectangle<float> calculateTrackBounds(const juce::Rectangle<float>& bounds) const;
        juce::Rectangle<float> calculateValueBarBounds(const juce::Rectangle<float>& trackBounds, bool enabled) const;
        
        double calculateStepForRange(double rangeLength, bool isShiftPressed) const;
        bool isIncrementKey(int keyCode) const;
        bool isDecrementKey(int keyCode) const;
        void updateValueWithStep(double step, bool increment);
        void resetToDefaultValue();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Slider)
    };
}

