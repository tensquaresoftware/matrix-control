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
    void syncProductNavButtons();
    void syncEditorNavButtons();

    std::function<void()> onDuplicateProduct;
    std::function<void()> onDuplicateEditor;
    std::function<void()> onNew;
    std::function<void()> onRename;
    std::function<void()> onDelete;

    /** Preferred height at kColorPanelWidth (includes outer gutters). */
    static int preferredHeight() noexcept;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    struct SelectorMetrics
    {
        int labelColW = 0;
        int squareW = 0;
        int comboW = 0;
        int selectorSpanW = 0;
    };

    static int labelColumnWidth() noexcept;
    static int squareNavButtonWidth() noexcept;
    static int comboWidthForContent(int contentW, int labelColW) noexcept;

    struct ComboNavRowRefs
    {
        juce::Label& label;
        juce::ComboBox& combo;
        juce::TextButton& prev;
        juce::TextButton& next;
    };

    void cycleCombo(juce::ComboBox& combo, int delta);
    SelectorMetrics makeSelectorMetrics(int contentWidth) const noexcept;
    void layoutComboNavRow(juce::Rectangle<int>& content,
                           const SelectorMetrics& metrics,
                           const ComboNavRowRefs& row);
    void layoutProductDuplicateRow(juce::Rectangle<int>& content, int labelColW);
    void layoutEditorActionRow(juce::Rectangle<int>& content, const SelectorMetrics& metrics);

    juce::Label titleLabel_;
    juce::Label productLabel_ { {}, "Product:" };
    juce::ComboBox productCombo_;
    juce::TextButton productPrevButton_ { "<" };
    juce::TextButton productNextButton_ { ">" };
    juce::TextButton productDuplicateButton_ { "Duplicate" };

    juce::Label editorLabel_ { {}, "Editor:" };
    juce::ComboBox editorCombo_;
    juce::TextButton editorPrevButton_ { "<" };
    juce::TextButton editorNextButton_ { ">" };
    juce::TextButton editorDuplicateButton_ { "Dup" };
    juce::TextButton newButton_ { "New" };
    juce::TextButton renameButton_ { "Ren" };
    juce::TextButton deleteButton_ { "Del" };

    juce::Rectangle<int> titleTabBounds_;
    int separatorY_ = 0;
    int separatorX_ = 0;
    int separatorW_ = 0;
};
