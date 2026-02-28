#pragma once

#include <array>
#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
    class ISkin;

    class TrackGeneratorDisplay : public juce::Component
    {
    public:
        using ValueChangedCallback = std::function<void(int pointIndex, int newValue)>;
        
        explicit TrackGeneratorDisplay(ISkin& skin, int width, int height);
        ~TrackGeneratorDisplay() override = default;

        void setSkin(ISkin& skin);
        
        void setTrackPoint1(int value);
        void setTrackPoint2(int value);
        void setTrackPoint3(int value);
        void setTrackPoint4(int value);
        void setTrackPoint5(int value);
        
        void setTrackPoint1(int value, bool notify);
        void setTrackPoint2(int value, bool notify);
        void setTrackPoint3(int value, bool notify);
        void setTrackPoint4(int value, bool notify);
        void setTrackPoint5(int value, bool notify);
        
        int getTrackPoint1() const { return pointValues_[0]; }
        int getTrackPoint2() const { return pointValues_[1]; }
        int getTrackPoint3() const { return pointValues_[2]; }
        int getTrackPoint4() const { return pointValues_[3]; }
        int getTrackPoint5() const { return pointValues_[4]; }
        
        void setOnValueChanged(ValueChangedCallback callback);
        
        void paint(juce::Graphics& g) override;
        void resized() override;

        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent& e) override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        inline constexpr static int kWidgetBorderThickness_ = 2;
        inline constexpr static int kWidgetPaddingTop_ = 12;
        inline constexpr static int kWidgetPaddingBottom_ = 10;
        inline constexpr static float kWidgetTriangleBase_ = 10.0f;
        
        inline constexpr static int kPointMinValue_ = 0;
        inline constexpr static int kPointMaxValue_ = 63;
        
        inline constexpr static int kCurvePointCount_ = 5;
        inline constexpr static float kCurvePadding_ = 5.0f;
        inline constexpr static float kCurvePointRadius_ = 3.0f;
        inline constexpr static float kCurveLineThickness_ = 1.0f;
        inline constexpr static float kPointHitZoneRadius_ = 10.0f;
        

        ISkin* skin_ = nullptr;
        int width_;
        int height_;
        
        std::array<int, kCurvePointCount_> pointValues_ {0, 15, 31, 47, 63};
        int draggedPointIndex_ = -1;
        
        juce::Image cachedImage_;
        bool cacheValid_ = false;
        std::array<int, kCurvePointCount_> cachedPointValues_;
        
        juce::Colour cachedCurveColour_;
        
        ValueChangedCallback onValueChanged_;

        void drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds);
        void drawBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds);
        void drawTriangle(juce::Graphics& g, const juce::Rectangle<float>& bounds);
        
        void regenerateCache();
        void invalidateCache();
        void updateSkinCache();
        float getPixelScale() const;
        
        juce::Rectangle<float> getCurveCenterBounds(const juce::Rectangle<float>& innerBounds) const;
        void drawCurve(juce::Graphics& g, const juce::Rectangle<float>& innerBounds);
        float computePointX(int pointIndex, const juce::Rectangle<float>& centerBounds) const;
        juce::Point<float> calculatePointPosition(int pointIndex, const juce::Rectangle<float>& centerBounds) const;

        int findPointAtPosition(const juce::Point<float>& position, const juce::Rectangle<float>& innerBounds) const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackGeneratorDisplay)
    };
}
