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
    int getNumRows() override;
    void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) override;
    void selectedRowsChanged(int lastRowSelected) override;

    struct Row
    {
        juce::String name;
        juce::uint32 argb = 0;
        bool product = false;
        bool sectionHeader = false;
        juce::String headerText;
    };

    void rebuildRows();

    TSS::DevSkin::Layer* layer_ = nullptr;
    juce::String skinId_;
    bool readOnly_ = true;
    std::vector<Row> rows_;

    juce::ListBox list_ { "Palette", this };
    juce::TextButton addButton_ { "+ Add" };
    juce::TextButton deleteButton_ { "Delete..." };
};
