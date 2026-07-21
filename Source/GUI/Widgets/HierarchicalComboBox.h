#pragma once

#include <functional>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"
#include "ComboBoxControlPainter.h"
#include "IPopupMenuHost.h"
#include "PopupMenuPositioner.h"

namespace TSS
{
    class HierarchicalPopupMenu;

    class HierarchicalComboBox : public juce::Component, public IPopupMenuHost
    {
    public:
        explicit HierarchicalComboBox(int width, int height, const ComboBoxLook& look);
        ~HierarchicalComboBox() override;

        void setLook(const ComboBoxLook& look);
        void setPopupMenuLook(const PopupMenuLook& look);
        void setUiScale(float uiScale);
        void setInactiveAppearance(bool inactive);
        void setPopupVerticalPlacement(PopupVerticalPlacement placement);
        PopupVerticalPlacement getPopupVerticalPlacement() const override { return popupVerticalPlacement_; }

        void clear();
        void addPrimaryItem(int id, const juce::String& label, bool isSentinel = false);
        void addChildItem(int primaryId, int id, const juce::String& label);

        void setSelectedPrimaryId(int primaryId, juce::NotificationType notification = juce::sendNotification);
        void setSelectedChildId(int childId, juce::NotificationType notification = juce::sendNotification);
        void setSelectedIds(int primaryId, int childId, juce::NotificationType notification = juce::sendNotification);

        int getSelectedPrimaryId() const { return selectedPrimaryId_; }
        int getSelectedChildId() const { return selectedChildId_; }

        void setTextWhenNothingSelected(const juce::String& text);
        const juce::String& getTextWhenNothingSelected() const { return textWhenNothingSelected_; }

        std::function<void()> onChange;
        std::function<void()> onBeforeShowPopup;

        void paint(juce::Graphics& g) override;
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

        static int getBaseHeight() { return ComboBoxControlMetrics::kDefaultHeight; }

    private:
        struct ChildItem
        {
            int id = 0;
            juce::String label;
        };

        struct PrimaryItem
        {
            int id = 0;
            juce::String label;
            bool isSentinel = false;
            std::vector<ChildItem> children;
        };

        friend class HierarchicalPopupMenu;

        int getPrimaryItemCount() const { return static_cast<int>(primaryItems_.size()); }
        const PrimaryItem& getPrimaryItem(int index) const { return primaryItems_[static_cast<size_t>(index)]; }
        bool canShowPopupMenu() const;
        void commitSelectionFromPopup(int primaryId, int childId);

        ComboBoxLook look_{};
        PopupMenuLook popupLook_{};
        std::vector<PrimaryItem> primaryItems_;
        juce::String textWhenNothingSelected_ { "<EMPTY>" };
        int width_;
        int height_;
        int selectedPrimaryId_ = 0;
        int selectedChildId_ = 0;
        bool isPopupOpen_ = false;
        bool hasFocus_ = false;
        bool inactiveAppearance_ = false;
        float uiScale_ = 1.0f;
        PopupVerticalPlacement popupVerticalPlacement_ = PopupVerticalPlacement::Auto;

        void showPopup();
        void commitSelection(int primaryId, int childId, juce::NotificationType notification);
        juce::String getDisplayText() const;
        const PrimaryItem* findPrimaryItem(int primaryId) const;
        const ChildItem* findChildItem(const PrimaryItem& primary, int childId) const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HierarchicalComboBox)
    };
}
