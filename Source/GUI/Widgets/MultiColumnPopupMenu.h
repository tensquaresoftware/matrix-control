#pragma once

#include "PopupMenuBase.h"

namespace TSS
{
    class MultiColumnPopupMenu : public PopupMenuBase
    {
    public:
        explicit MultiColumnPopupMenu(ComboBox& comboBox);
        ~MultiColumnPopupMenu() override = default;

        void paint(juce::Graphics& g) override;
        void mouseMove(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent& e) override;

        static void show(ComboBox& comboBox);

    protected:
        void handleKeyboardNavigation(const juce::KeyPress& key) override;

    private:
        inline constexpr static int kSeparatorWidth_ = 1;
        inline constexpr static int kColumnThreshold_ = 10;

        int columnCount_ = 1;
        int itemsPerColumn_ = 0;
        float columnWidth_ = 0.0f;

        void calculateLayout();
        int calculateColumnCount(int totalItems) const;
        int calculateItemsPerColumn(int totalItems, int columnCount) const;
        float getSeparatorWidth() const { return 1.0f * uiScale_; }
        float getActualColumnWidth(float contentWidth) const;

        juce::Rectangle<float> getItemBounds(int itemIndex) const;
        int getItemIndexAt(int x, int y) const;
        int getColumnFromX(int x) const;
        int getRowFromY(int y) const;
        int getItemIndexFromColumnAndRow(int column, int row) const;
        
        void drawItems(juce::Graphics& g);
        
        void navigateUp();
        void navigateDown();
        void navigateLeft();
        void navigateRight();

        void navigateInSingleColumn(int delta);
        void navigateInMultiColumn(int columnDelta, int rowDelta);
        bool tryNavigateToItemIndex(int itemIndex);
    };
}
