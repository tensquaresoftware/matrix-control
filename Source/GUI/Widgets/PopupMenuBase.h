#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "PopupMenuRenderer.h"

namespace TSS
{
    class ComboBox;

    class PopupMenuBase : public juce::Component
    {
    public:
        virtual ~PopupMenuBase() override;

    protected:
        inline constexpr static float kBorderThickness_ = 1.0f;
        inline constexpr static float kHighlightGap_ = 1.0f;

        PopupMenuBase(ComboBox& comboBox, bool isButtonLike);

        void mouseExit(const juce::MouseEvent& e) override;
        void inputAttemptWhenModal() override;
        bool keyPressed(const juce::KeyPress& key) override;

        void updateHighlightedItem(int itemIndex);
        void selectItem(int itemIndex);
        
        bool isValidItemIndex(int itemIndex) const;
        
        virtual void handleKeyboardNavigation(const juce::KeyPress& key) = 0;

        ComboBox& comboBox_;
        bool isButtonLike_ = false;
        int highlightedItemIndex_ = -1;
        float uiScale_ = 1.0f;
        juce::Font cachedFont_;
        PopupMenuRenderer renderer_;

        int getItemHeightDesign() const;
        float getBorderThicknessDesign() const;

    private:
        void closePopup();

        friend class MultiColumnPopupMenu;
        friend class ScrollablePopupMenu;
    };
}
