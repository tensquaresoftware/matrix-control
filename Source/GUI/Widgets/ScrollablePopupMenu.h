#pragma once

#include "PopupMenuBase.h"

namespace TSS
{
    class ScrollablePopupMenu : public PopupMenuBase
    {
    public:
        explicit ScrollablePopupMenu(ComboBox& comboBox);
        ~ScrollablePopupMenu() override;

        void paint(juce::Graphics& g) override;
        void resized() override;

        static void show(ComboBox& comboBox);

    protected:
        void handleKeyboardNavigation(const juce::KeyPress& key) override;

    private:
        class ScrollableContentComponent;
        class CustomScrollBar;

        inline constexpr static float kRightMarginFromHighlightToEdge_ = 8.0f;
        inline constexpr static float kWheelScrollFactorContent_ = 100.0f;
        inline constexpr static float kThumbInsetBase_ = 2.0f;

        float columnWidth_ = 0.0f;
        int scrollableContentHeight_ = 0;
        bool scrollbarNeeded_ = false;

        std::unique_ptr<juce::Viewport> viewport_;
        std::unique_ptr<ScrollableContentComponent> contentComponent_;
        std::unique_ptr<CustomScrollBar> customScrollBar_;

        void setupScrollableContent();
        
        juce::Rectangle<float> getItemBounds(int itemIndex) const;
        int getItemIndexAt(int x, int y) const;
        
        void drawItems(juce::Graphics& g);

        void scrollViewportBy(int deltaPixels);
        
        void navigateUp();
        void navigateDown();

        void navigateAndScroll(int delta);
        void scrollToHighlightedItem();

        friend class ScrollableContentComponent;
    };
}
