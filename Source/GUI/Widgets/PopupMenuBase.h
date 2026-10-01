#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

#include "IPopupMenuHost.h"
#include "PopupMenuRenderer.h"

namespace TSS
{
    class ComboBox;
    class PopupMenuOutsideDismissWatcher;

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

        IPopupMenuHost& host_;
        ComboBox& comboBox_;
        bool isButtonLike_ = false;
        int highlightedItemIndex_ = -1;
        float uiScale_ = 1.0f;
        juce::Font cachedFont_;
        PopupMenuRenderer renderer_;

        int getItemHeightDesign() const;
        float getBorderThicknessDesign() const;
        void armOutsideDismissWatcher();

    private:
        void closePopup();

        std::unique_ptr<PopupMenuOutsideDismissWatcher> outsideDismissWatcher_;

        friend class MultiColumnPopupMenu;
        friend class ScrollablePopupMenu;
    };
}
