#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

/** Left SKINS panel: Product / Editor selectors and Editor CRUD. */
class SkinEditorSkinsPanel final : public juce::Component
{
public:
    SkinEditorSkinsPanel();

    juce::ComboBox& productCombo() noexcept { return productCombo_; }
    juce::ComboBox& editorCombo() noexcept { return editorCombo_; }

    void setProductDuplicateEnabled(bool enabled);
    void setEditorActionsEnabled(bool canDuplicate, bool canRenameDelete, bool canNew);

    std::function<void()> onDuplicateProduct;
    std::function<void()> onDuplicateEditor;
    std::function<void()> onNew;
    std::function<void()> onRename;
    std::function<void()> onDelete;

    /** Preferred height at kColorPanelWidth (includes outer gutters). */
    static int preferredHeight() noexcept;
    /** Same as preferredHeight — SKINS no longer budgets a wrap row. */
    static int preferredHeightForMinWindow() noexcept;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    static int labelColumnWidth() noexcept;
    static int comboWidthForContent(int contentW, int labelColW, int trailingButtonW) noexcept;

    juce::Label titleLabel_;
    juce::Label productLabel_ { {}, "Product:" };
    juce::ComboBox productCombo_;
    juce::TextButton productDuplicateButton_ { "Duplicate" };
    juce::Label editorLabel_ { {}, "Editor:" };
    juce::ComboBox editorCombo_;
    juce::TextButton editorDuplicateButton_ { "Duplicate" };
    juce::TextButton newButton_ { "New" };
    juce::TextButton renameButton_ { "Rename" };
    juce::TextButton deleteButton_ { "Delete" };

    juce::Rectangle<int> titleTabBounds_;
    int separatorY_ = 0;
    int separatorX_ = 0;
    int separatorW_ = 0;
};
