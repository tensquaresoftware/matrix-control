#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"
#include "GUI/Looks/WidgetLooks.h"
#include "ComboBoxControlPainter.h"

namespace TSS
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

        explicit ComboBox(int width, int height, const ComboBoxLook& look, Style style = Style::Standard);
        ~ComboBox() override = default;

        void setLook(const ComboBoxLook& look);
        void setPopupMenuLook(const PopupMenuLook& look);
        void setUiScale(float uiScale);

        void paint(juce::Graphics& g) override;
        void showPopup() override;

        void mouseDown(const juce::MouseEvent& e) override;

        void focusGained(juce::Component::FocusChangeType cause) override;
        void focusLost(juce::Component::FocusChangeType cause) override;

        float getUiScale() const { return uiScale_; }
        const PopupMenuLook& getPopupMenuLook() const { return popupLook_; }
        int getBaseComponentWidth() const { return width_; }
        int getBaseComponentHeight() const { return height_; }
        
        static int getBaseWidth() { return ComboBoxControlMetrics::kDefaultWidth; }
        static int getBaseHeight() { return ComboBoxControlMetrics::kDefaultHeight; }

        static void setPopupLayoutDimensions(const PopupMenuLayoutDimensions& dimensions);
        static const PopupMenuLayoutDimensions& getPopupLayoutDimensions();

        int getScaledVerticalMargin() const;

    private:
        ComboBoxLook look_{};
        PopupMenuLook popupLook_{};
        int width_;
        int height_;
        Style style_;
        bool isPopupOpen_ = false;
        bool hasFocus_ = false;
        float uiScale_ = 1.0f;

        static PopupMenuLayoutDimensions popupLayoutDimensions_;

        juce::String getSelectedItemText() const;
        bool canShowPopup() const;
        void showPopupAsynchronously();

        friend class PopupMenuBase;
        friend class MultiColumnPopupMenu;
        friend class ScrollablePopupMenu;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBox)
    };
}
