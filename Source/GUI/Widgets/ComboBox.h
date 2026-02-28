#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
    class ISkin;
    class PopupMenuBase;
    class MultiColumnPopupMenu;
    class ScrollablePopupMenu;
}

namespace tss
{

    class ComboBox : public juce::ComboBox
    {
    public:
        enum class Style
        {
            Standard,
            ButtonLike
        };

        explicit ComboBox(ISkin& skin, int width, int height, Style style = Style::Standard);
        ~ComboBox() override = default;

        void setSkin(ISkin& skin);

        void paint(juce::Graphics& g) override;
        void resized() override;
        void showPopup() override;

        void mouseDown(const juce::MouseEvent& e) override;

        void focusGained(juce::Component::FocusChangeType cause) override;
        void focusLost(juce::Component::FocusChangeType cause) override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }
        static constexpr int getVerticalMargin() { return kVerticalMargin_; }

    private:
        inline constexpr static int kVerticalMargin_ = 4;
        inline constexpr static int kBackgroundHeight_ = 16;
        inline constexpr static int kLeftPadding_ = 4;
        inline constexpr static int kRightPadding_ = 4;
        inline constexpr static int kBorderThickness_ = 1;
        inline constexpr static int kBorderThicknessButtonLike_ = 2;
        inline constexpr static int kTriangleBaseSize_ = 7;
        inline constexpr static float kTriangleHeightFactor_ = 0.8660254f;

        ISkin* skin_ = nullptr;
        int width_;
        int height_;
        Style style_;
        bool isPopupOpen_ = false;
        bool hasFocus_ = false;
        
        // Image cache
        juce::Image cachedImage_;
        bool cacheValid_ = false;
        int cachedSelectedIndex_ = -1;

        // Skin cache
        juce::Colour cachedBackgroundColour_;
        juce::Colour cachedTextColour_;
        juce::Colour cachedBorderColour_;
        juce::Colour cachedFocusBorderColour_;
        juce::Font cachedFont_;

        void regenerateCache();
        void invalidateCache();
        void updateSkinCache();
        float getPixelScale() const;

        void drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled);
        void drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled);
        void drawTriangle(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled);
        void drawBorderIfNeeded(juce::Graphics& g, const juce::Rectangle<float>& bounds, const juce::Rectangle<float>& backgroundBounds, bool enabled, bool hasFocus);
        
        juce::Rectangle<float> calculateBackgroundBounds(const juce::Rectangle<float>& bounds) const;
        juce::Path createTrianglePath(float x, float y, float baseSize) const;

        juce::String getSelectedItemText() const;
        juce::Colour getTextColourForCurrentStyle(bool enabled) const;
        juce::Rectangle<float> calculateTextBounds(const juce::Rectangle<float>& bounds) const;
        void drawTextInBounds(juce::Graphics& g, const juce::String& text, const juce::Rectangle<float>& textBounds, const juce::Colour& textColour) const;

        bool canShowPopup() const;
        void showPopupAsynchronously();

        friend class PopupMenuBase;
        friend class MultiColumnPopupMenu;
        friend class ScrollablePopupMenu;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBox)
    };
}
