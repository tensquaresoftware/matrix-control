#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"

class SkinEditorUnboundPanel final : public juce::Component,
                                     private juce::ListBoxModel
{
public:
    SkinEditorUnboundPanel();

    void setLayer(TSS::DevSkin::Layer* layer);
    void setActiveSkinId(const juce::String& skinId);
    void setReadOnly(bool readOnly);
    /** When Compare shows Product, Unbound must be empty. */
    void setForcedEmpty(bool forcedEmpty);
    void refresh();

    std::function<void(TSS::SkinColourKey key, const juce::String& pastilleName)> onBindRequested;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    int getNumRows() override;
    void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override;

    void promptBind(int row);

    TSS::DevSkin::Layer* layer_ = nullptr;
    juce::String skinId_;
    bool readOnly_ = true;
    bool forcedEmpty_ = false;
    std::vector<TSS::SkinColourKey> keys_;

    juce::ListBox list_ { "Unbound", this };
    juce::TextButton bindButton_ { "Bind..." };
};
