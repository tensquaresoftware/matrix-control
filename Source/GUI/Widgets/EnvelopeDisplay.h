#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
    class ISkin;

    class EnvelopeDisplay : public juce::Component
    {
    public:
        using ValueChangedCallback = std::function<void(int paramIndex, int newValue)>;
        
        explicit EnvelopeDisplay(ISkin& skin, int width, int height);
        ~EnvelopeDisplay() override = default;

        void setSkin(ISkin& skin);
        
        void setDelay(int value);
        void setAttack(int value);
        void setDecay(int value);
        void setSustain(int value);
        void setRelease(int value);
        
        void setDelay(int value, bool notify);
        void setAttack(int value, bool notify);
        void setDecay(int value, bool notify);
        void setSustain(int value, bool notify);
        void setRelease(int value, bool notify);
        
        int getDelay() const { return delay_; }
        int getAttack() const { return attack_; }
        int getDecay() const { return decay_; }
        int getSustain() const { return sustain_; }
        int getRelease() const { return release_; }
        
        void setOnValueChanged(ValueChangedCallback callback);
        
        void paint(juce::Graphics& g) override;
        
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
        
        inline constexpr static int kParameterCount_ = 5;
        inline constexpr static int kPointMinValue_ = 0;
        inline constexpr static int kPointMaxValue_ = 63;

        inline constexpr static int kCurvePointCount_ = 6;
        inline constexpr static float kCurvePadding_ = 5.0f;
        inline constexpr static float kCurvePointRadius_ = 3.0f;
        inline constexpr static float kCurveLineThickness_ = 1.0f;
        inline constexpr static float kMinCurveSegmentWidth_ = 1.0f;
        inline constexpr static float kPointHitZoneRadius_ = 10.0f;
        inline constexpr static float kSustainSegmentHitZone_ = 10.0f;

        ISkin* skin_ = nullptr;
        int width_;
        int height_;
        
        int delay_ {0};
        int attack_ {0};
        int decay_ {10};
        int sustain_ {50};
        int release_ {10};
        
        int draggedPointIndex_ = -1;
        bool draggingSustainSegment_ = false;
        
        juce::Image cachedImage_;
        bool cacheValid_ = false;
        int cachedDelay_ {-1};
        int cachedAttack_ {-1};
        int cachedDecay_ {-1};
        int cachedSustain_ {-1};
        int cachedRelease_ {-1};
        
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
        void drawEnvelope(juce::Graphics& g, const juce::Rectangle<float>& innerBounds);
        
        struct EnvelopePoints
        {
            juce::Point<float> points[kCurvePointCount_];
            float segmentMaxWidth;
        };
        
        EnvelopePoints calculateEnvelopePoints(const juce::Rectangle<float>& centerBounds) const;
        int findPointAtPosition(const juce::Point<float>& position, const juce::Rectangle<float>& innerBounds) const;
        bool findSustainSegmentAtPosition(const juce::Point<float>& position, const juce::Rectangle<float>& innerBounds) const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopeDisplay)
    };
}
