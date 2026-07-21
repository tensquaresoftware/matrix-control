#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"
#include "GUI/Looks/WidgetLooks.h"
#include "ComboBoxControlPainter.h"
#include "IPopupMenuHost.h"
#include "PopupMenuPositioner.h"

namespace TSS
{
    class MultiColumnPopupMenu;
    class ScrollablePopupMenu;

    class ComboBox : public juce::ComboBox, public IPopupMenuHost
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
        void setPopupVerticalPlacement(PopupVerticalPlacement placement);
        PopupVerticalPlacement getPopupVerticalPlacement() const override { return popupVerticalPlacement_; }

        void paint(juce::Graphics& g) override;
        void showPopup() override;

        void mouseDown(const juce::MouseEvent& e) override;

        void focusGained(juce::Component::FocusChangeType cause) override;
        void focusLost(juce::Component::FocusChangeType cause) override;

        float getUiScale() const override { return uiScale_; }
        const PopupMenuLook& getPopupMenuLook() const override { return popupLook_; }
        juce::Component& asHostComponent() override { return *this; }
        const juce::Component& asHostComponent() const override { return *this; }
        int getBaseComponentWidth() const override { return width_; }
        int getBaseComponentHeight() const { return height_; }
        int getScaledVerticalMargin() const override;

        void notifyPopupOpened() override;
        void notifyPopupClosed() override;

        static int getBaseWidth() { return ComboBoxControlMetrics::kDefaultWidth; }
        static int getBaseHeight() { return ComboBoxControlMetrics::kDefaultHeight; }

        static void setPopupLayoutDimensions(const PopupMenuLayoutDimensions& dimensions);
        static const PopupMenuLayoutDimensions& getPopupLayoutDimensions();

    private:
        ComboBoxLook look_{};
        PopupMenuLook popupLook_{};
        int width_;
        int height_;
        Style style_;
        bool isPopupOpen_ = false;
        bool hasFocus_ = false;
        float uiScale_ = 1.0f;
        PopupVerticalPlacement popupVerticalPlacement_ = PopupVerticalPlacement::Auto;

        static PopupMenuLayoutDimensions popupLayoutDimensions_;

        juce::String getSelectedItemText() const;
        bool canShowPopup() const;
        void showPopupAsynchronously();

        friend class MultiColumnPopupMenu;
        friend class ScrollablePopupMenu;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBox)
    };
}
