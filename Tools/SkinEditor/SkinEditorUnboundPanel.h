#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"

class SkinEditorUnboundPanel final : public juce::Component,
                                     private juce::ListBoxModel
{
public:
    SkinEditorUnboundPanel();
    ~SkinEditorUnboundPanel() override;

    void setLayer(TSS::DevSkin::Layer* layer);
    void setActiveSkinId(const juce::String& skinId);
    void setReadOnly(bool readOnly);
    /** When Compare shows Product, Unbound must show an empty list. */
    void setEmptyForCompareProduct(bool empty);
    void refresh();

    /** Row click - main owns Idle / HeldColour / ArmedRow (bind vs re-arm). */
    std::function<void(TSS::SkinColourKey key)> onRowActivated;
    /** Escape while the list has focus - clear mapping mode in main. */
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

    int getNumRows() override;
    void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent&) override;

    void tryBindRow(int row);

    TSS::DevSkin::Layer* layer_ = nullptr;
    juce::String skinId_;
    bool readOnly_ = true;
    bool emptyForCompareProduct_ = false;
    std::vector<TSS::SkinColourKey> keys_;
    EscapeKeyForwarder escapeForwarder_;

    juce::Label titleLabel_;
    juce::ListBox list_ { "Unbound", this };
    juce::Rectangle<int> titleTabBounds_;
    juce::Rectangle<int> badgeBounds_;
};
