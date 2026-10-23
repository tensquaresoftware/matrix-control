#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"

class SkinEditorMappingsPanel final : public juce::Component,
                                      private juce::ListBoxModel,
                                      private juce::ComboBox::Listener
{
public:
    SkinEditorMappingsPanel();

    void setLayer(TSS::DevSkin::Layer* layer);
    void setActiveSkinId(const juce::String& skinId);
    void setReadOnly(bool readOnly);
    void refresh();

    std::function<void(TSS::SkinColourKey key, const juce::String& pastilleName)> onBindRequested;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    int getNumRows() override;
    void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override;
    void comboBoxChanged(juce::ComboBox*) override;

    void rebuildRows();
    void promptBind(int row);

    struct Row
    {
        TSS::SkinColourKey key {};
        juce::String propertyLabel;
        juce::String pastilleName;
    };

    TSS::DevSkin::Layer* layer_ = nullptr;
    juce::String skinId_;
    bool readOnly_ = true;
    std::vector<Row> rows_;

    juce::ComboBox objectFilter_;
    juce::ComboBox stateFilter_;
    juce::ListBox list_ { "Mappings", this };
    juce::TextButton bindButton_ { "Bind..." };
};
