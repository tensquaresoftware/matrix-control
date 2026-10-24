#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"

class SkinEditorPalettePanel final : public juce::Component,
                                     private juce::ListBoxModel
{
public:
    SkinEditorPalettePanel();
    ~SkinEditorPalettePanel() override;

    void setLayer(TSS::DevSkin::Layer* layer);
    void setActiveSkinId(const juce::String& skinId);
    void setReadOnly(bool readOnly);
    void refresh();

    juce::String getSelectedPastilleName() const;
    void selectPastille(const juce::String& name);
    void clearSelection();

    std::function<void(const juce::String& pastilleName)> onPastilleSelected;
    std::function<void()> onAddRequested;
    std::function<void()> onDeleteRequested;
    /** Escape while the palette list has focus - clear mapping mode in main. */
    std::function<bool()> onEscapeRequested;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    struct EscapeKeyForwarder final : public juce::KeyListener
    {
        std::function<bool()>* handler = nullptr;
        bool keyPressed(const juce::KeyPress& key, juce::Component*) override
        {
            if (key == juce::KeyPress::escapeKey && handler != nullptr && *handler != nullptr)
                return (*handler)();
            return false;
        }
    };

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
    EscapeKeyForwarder escapeForwarder_;

    juce::Label titleLabel_;
    juce::Rectangle<int> titleTabBounds_;
    juce::ListBox list_ { "Palette", this };
    juce::TextButton addButton_ { "Add" };
    juce::TextButton deleteButton_ { "Delete" };
};
