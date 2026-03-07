#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace tss
{
    class PopupMenuBase;
    class MultiColumnPopupMenu;
    class ScrollablePopupMenu;

    class ComboBox : public juce::ComboBox
    {
    public:
        enum class Style
        {
            Standard,
            ButtonLike
        };

        explicit ComboBox(int width, int height, Style style = Style::Standard);
        ~ComboBox() override = default;

        void setLook(const ComboBoxLook& look);
        void setPopupMenuLook(const PopupMenuLook& look);
        void setScalingFactor(float scalingFactor);
        
        void paint(juce::Graphics& g) override;
        void showPopup() override;

        void mouseDown(const juce::MouseEvent& e) override;

        void focusGained(juce::Component::FocusChangeType cause) override;
        void focusLost(juce::Component::FocusChangeType cause) override;

        float getScalingFactor() const { return scalingFactor_; }
        const PopupMenuLook& getPopupMenuLook() const { return popupLook_; }
        int getBaseComponentWidth() const { return width_; }
        int getBaseComponentHeight() const { return height_; }
        
        static int getBaseWidth() { return kDefaultWidth_; }
        static int getBaseHeight() { return kDefaultHeight_; }
        static constexpr int getVerticalMargin() { return kVerticalMargin_; }

    private:
        inline constexpr static int kDefaultWidth_ = 100;
        inline constexpr static int kDefaultHeight_ = 20;
        inline constexpr static int kVerticalMargin_ = 4;
        inline constexpr static int kBackgroundHeight_ = 16;
        inline constexpr static int kLeftPadding_ = 4;
        inline constexpr static int kRightPadding_ = 4;
        inline constexpr static int kBorderThickness_ = 1;
        inline constexpr static int kBorderThicknessButtonLike_ = 2;
        inline constexpr static int kTriangleBaseSize_ = 7;
        inline constexpr static float kTriangleHeightFactor_ = 0.8660254f;
        inline constexpr static float kFontSize_ = 14.0f;

        ComboBoxLook look_{};
        PopupMenuLook popupLook_{};
        int width_;
        int height_;
        Style style_;
        bool isPopupOpen_ = false;
        bool hasFocus_ = false;
        float scalingFactor_ = 1.0f;

        void drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled);
        void drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled);
        void drawTriangle(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled);
        void drawBorderIfNeeded(juce::Graphics& g, const juce::Rectangle<float>& bounds, const juce::Rectangle<float>& backgroundBounds, bool enabled, bool hasFocus);
        
        juce::Rectangle<float> calculateBackgroundBounds(const juce::Rectangle<float>& bounds) const;
        juce::Path createTrianglePath(float x, float y, float baseSize) const;

        juce::String getSelectedItemText() const;
        juce::Colour getTextColourForCurrentStyle(bool enabled) const;
        juce::Colour getTriangleColourForCurrentStyle(bool enabled) const;
        juce::Colour getBackgroundColourForCurrentStyle(bool enabled) const;
        juce::Colour getBorderColourForCurrentStyle(bool enabled) const;
        juce::Colour getFocusBorderColourForCurrentStyle() const;
        juce::Rectangle<float> calculateTextBounds(const juce::Rectangle<float>& bounds) const;

        bool canShowPopup() const;
        void showPopupAsynchronously();

        friend class PopupMenuBase;
        friend class MultiColumnPopupMenu;
        friend class ScrollablePopupMenu;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBox)
    };
}
