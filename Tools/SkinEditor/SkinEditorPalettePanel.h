#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"

class SkinEditorPalettePanel final : public juce::Component,
                                     private juce::ListBoxModel
{
public:
    SkinEditorPalettePanel();

    void setLayer(TSS::DevSkin::Layer* layer);
    void setActiveSkinId(const juce::String& skinId);
    void setReadOnly(bool readOnly);
    void refresh();

    juce::String getSelectedPastilleName() const;
    void selectPastille(const juce::String& name);

    std::function<void(const juce::String& pastilleName)> onPastilleSelected;
    std::function<void()> onAddRequested;
    std::function<void()> onDeleteRequested;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    enum class RowKind
    {
        kGap,
        kSection,
        kColour
    };

    struct Row
    {
        RowKind kind = RowKind::kColour;
        juce::String name;
        juce::uint32 argb = 0;
        bool isProduct = false;
        juce::String headerText;
        /** Second ListBox row of a visual pair (half of kColumnHeaderHeight). */
        bool isLowerListHalf = false;
    };

    int getNumRows() override;
    void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) override;
    void selectedRowsChanged(int lastRowSelected) override;

    void rebuildRows();
    /** Push upper + lower half-height ListBox rows for one visual content row. */
    void pushSplitListRow(Row row);
    int nextUpperListRow(int row) const noexcept;
    bool isRowSelectedVisually(int row) const noexcept;
    int paintWidth() const noexcept;
    void roundFlagsForRow(int row, bool& roundTop, bool& roundBottom) const noexcept;
    static bool isBlockBoundary(RowKind kind) noexcept;
    void appendPaletteGroup(const juce::String& title, std::vector<Row>& colours, bool leadGap);

    TSS::DevSkin::Layer* layer_ = nullptr;
    juce::String skinId_;
    bool readOnly_ = true;
    std::vector<Row> rows_;

    juce::Label titleLabel_;
    juce::Rectangle<int> titleTabBounds_;
    juce::ListBox list_ { "Palette", this };
    juce::TextButton addButton_ { "Add" };
    juce::TextButton deleteButton_ { "Delete" };
};
