#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
    class ISkin;

    class NumberBox : public juce::Component
    {
    public:
        using ValueChangedCallback = std::function<void(int)>;
        
        explicit NumberBox(ISkin& skin, int width, bool editable, int minValue, int maxValue);
        ~NumberBox() override = default;

        void setSkin(ISkin& skin);

        void setValue(int newValue);
        int getValue() const { return currentValue_; }
        
        void setOnValueChanged(ValueChangedCallback callback);

        void setShowDot(bool show);
        bool getShowDot() const { return showDot_; }

        void paint(juce::Graphics& g) override;
        void resized() override;
        void mouseDoubleClick(const juce::MouseEvent& e) override;

        static constexpr int getHeight() { return kHeight_; }

    private:
        inline constexpr static int kHeight_ = 20;
        inline constexpr static int kBorderThickness_ = 2;
        inline constexpr static float kDotRadius_ = 1.5f;
        inline constexpr static float kDotXOffset_ = 3.0f;
        inline constexpr static float kEditorFontSizeIncrease_ = 4.0f;

        ISkin* skin_ = nullptr;
        int currentValue_ = 0;
        int minValue_ = 0;
        int maxValue_ = 99;
        bool editable_ = false;
        bool showDot_ = false;
        std::unique_ptr<juce::TextEditor> editor_;
        ValueChangedCallback onValueChanged_;

        // Image cache
        juce::Image cachedImage_;
        bool cacheValid_ = false;

        // Skin cache
        juce::Colour cachedBackgroundColour_;
        juce::Colour cachedBorderColour_;
        juce::Colour cachedTextColour_;
        juce::Colour cachedDotColour_;
        juce::Font cachedFont_;
        
        // Cached text width (expensive GlyphArrangement)
        float cachedTextWidth_ = 0.0f;
        juce::String cachedValueText_;

        void regenerateCache();
        void invalidateCache();
        void updateSkinCache();
        void updateTextWidthCache();
        float getPixelScale() const;

        juce::Colour getBorderColour() const;
        juce::Point<float> calculateDotPosition(const juce::Rectangle<float>& bounds, float textWidth) const;
        
        void showEditor();
        void hideEditor();
        void handleEditorReturn();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NumberBox)
    };
}
