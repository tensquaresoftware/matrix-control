#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "PopupMenuRenderer.h"

namespace TSS
{
    class HierarchicalComboBox;

    class HierarchicalPopupMenu : public juce::Component
    {
    public:
        explicit HierarchicalPopupMenu(HierarchicalComboBox& owner);
        ~HierarchicalPopupMenu() override;

        void paint(juce::Graphics& g) override;
        bool hitTest(int x, int y) override;
        void mouseMove(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent& e) override;
        void mouseExit(const juce::MouseEvent& e) override;
        void inputAttemptWhenModal() override;
        bool keyPressed(const juce::KeyPress& key) override;

        static void show(HierarchicalComboBox& owner);

    private:
        inline constexpr static float kHighlightGap_ = 1.0f;

        HierarchicalComboBox& owner_;
        PopupMenuRenderer renderer_;
        float uiScale_ = 1.0f;
        juce::Font cachedFont_;
        int highlightedPrimaryIndex_ = -1;
        int highlightedChildIndex_ = -1;
        float primaryColumnWidth_ = 0.0f;
        float secondaryColumnWidth_ = 0.0f;

        bool hasSecondaryColumn() const;
        int getOpenablePrimaryCount() const;
        float getItemHeight() const;
        float getBorderThicknessDesign() const;
        float getLayoutBorderThickness() const;
        float getSnappedBorderThickness() const;
        void drawPanelBorderEdges(juce::Graphics& g,
                                  juce::Rectangle<float> panel,
                                  float thickness,
                                  bool drawLeft,
                                  bool drawTop,
                                  bool drawRight,
                                  bool drawBottom) const;
        void drawStaircasePanelBorders(juce::Graphics& g,
                                       juce::Rectangle<float> primaryPanel,
                                       juce::Rectangle<float> secondaryPanel,
                                       float thickness) const;
        juce::Rectangle<float> getPrimaryPanelBounds() const;
        juce::Rectangle<float> getSecondaryPanelBounds() const;
        juce::Rectangle<float> getPrimaryContentBounds() const;
        juce::Rectangle<float> getSecondaryContentBounds() const;
        juce::Rectangle<float> getPrimaryItemBounds(int primaryIndex) const;
        juce::Rectangle<float> getSecondaryItemBounds(int childIndex) const;
        int getPrimaryIndexAt(int x, int y) const;
        int getChildIndexAt(int x, int y) const;
        void updateHighlightFromPosition(int x, int y);
        void selectPrimaryLeaf(int primaryIndex);
        void selectChild(int primaryIndex, int childIndex);
        int getHighlightedPrimaryIndex() const { return highlightedPrimaryIndex_; }
        void getPreferredContentSize(float& width, float& height) const;
        void closePopup();

        static int countOpenablePrimaries(const HierarchicalComboBox& owner);
        static int openableIndexForPrimaryId(const HierarchicalComboBox& owner, int primaryId);
        static int primaryStorageIndexForOpenableIndex(const HierarchicalComboBox& owner, int openableIndex);
    };
}
