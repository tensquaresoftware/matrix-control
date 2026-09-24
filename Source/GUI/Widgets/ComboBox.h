#pragma once

#include <functional>

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

        /** When true, ButtonLike ScrollablePopupMenu draws port-sentinel chrome on row 0
            (NO INPUT / NO OUTPUT rule + shorter hover) when the popup has more than one item.
            Consumers enable this for MIDI/audio port lists; leave false for normal first rows
            (e.g. Computer Patches). No-op for Standard / multi-column popups. Default false. */
        void setUsesPortSentinelPopupChrome(bool shouldUse) noexcept;
        [[nodiscard]] bool usesPortSentinelPopupChrome() const noexcept { return usesPortSentinelPopupChrome_; }

        void paint(juce::Graphics& g) override;
        void showPopup() override;
        /** Programmatic reopen after live item rebuild — skips onAboutToShowPopup. */
        void showPopupAfterItemRebuild();

        void mouseDown(const juce::MouseEvent& e) override;

        void focusGained(juce::Component::FocusChangeType cause) override;
        void focusLost(juce::Component::FocusChangeType cause) override;

        /** Invoked on the message thread immediately before a popup open is scheduled. */
        std::function<void()> onAboutToShowPopup;

        float getUiScale() const override { return uiScale_; }
        const PopupMenuLook& getPopupMenuLook() const override { return popupLook_; }
        juce::Component& asHostComponent() override { return *this; }
        const juce::Component& asHostComponent() const override { return *this; }
        int getBaseComponentWidth() const override { return width_; }
        int getBaseComponentHeight() const { return height_; }
        int getScaledVerticalMargin() const override;

        void notifyPopupOpened() override;
        void notifyPopupClosed() override;
        void suppressNextPopupOpen() override;

        [[nodiscard]] bool isPopupOpen() const noexcept { return isPopupOpen_; }
        /** Dismiss the modal popup without selecting an item (programmatic live refresh). */
        void dismissPopup();
        /** Called by Scrollable/MultiColumn show after constructing the modal popup. */
        void attachOpenPopup(juce::Component& popup);

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
        bool suppressNextPopupOpen_ = false;
        bool usesPortSentinelPopupChrome_ = false;
        float uiScale_ = 1.0f;
        PopupVerticalPlacement popupVerticalPlacement_ = PopupVerticalPlacement::Auto;
        juce::Component::SafePointer<juce::Component> activePopup_;
        uint32_t popupShowGeneration_ = 0;

        static PopupMenuLayoutDimensions popupLayoutDimensions_;

        juce::String getSelectedItemText() const;
        bool canShowPopup() const;
        void showPopupInternal(bool invokeAboutToShow);
        void showPopupAsynchronously();

        friend class MultiColumnPopupMenu;
        friend class ScrollablePopupMenu;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBox)
    };
}
