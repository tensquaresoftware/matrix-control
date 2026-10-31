#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"

class SkinEditorPalettePanel final : public juce::Component,
                                     private juce::ListBoxModel,
                                     private juce::ScrollBar::Listener
{
public:
    SkinEditorPalettePanel();
    ~SkinEditorPalettePanel() override;

    void setLayer(TSS::DevSkin::Layer* layer);
    void setActiveSkinId(const juce::String& skinId);
    void setReadOnly(bool readOnly);
    void refresh();

    juce::String getSelectedPastilleName() const;
    /** Programmatic selection — does not fire onPastilleSelected (no mapping-mode side effects). */
    void selectPastille(const juce::String& name);
    void clearSelection();

    std::function<void(const juce::String& pastilleName)> onPastilleSelected;
    /** Non-zero mapping-count digit click — filter Mappings by this pastille (no Hold/Arm). */
    std::function<void(const juce::String& pastilleName)> onMappingCountClicked;
    std::function<void()> onAddRequested;
    std::function<void()> onDuplicateRequested;
    std::function<void()> onDeleteRequested;
    std::function<void(const juce::String& pastilleName)> onRenamePastilleRequested;
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

    struct MappingCountHoverListener final : public juce::MouseListener
    {
        SkinEditorPalettePanel* owner = nullptr;
        void mouseMove(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseExit(const juce::MouseEvent& e) override;
    };

    /** Stacked sticky section titles over the list (Product, then Editor). */
    struct StickyHeadersOverlay final : public juce::Component
    {
        explicit StickyHeadersOverlay(SkinEditorPalettePanel& owner) : owner_(owner)
        {
            setInterceptsMouseClicks(true, false);
        }

        void paint(juce::Graphics& g) override { owner_.paintStickySectionHeaders(g); }
        bool hitTest(int x, int y) override { return owner_.stickyHeaderHitTest(x, y); }

        SkinEditorPalettePanel& owner_;
    };

    enum class RowKind
    {
        kGap,
        kSection,
        kColour,
        kEmptyHint
    };

    struct Row
    {
        RowKind kind = RowKind::kColour;
        juce::String name;
        juce::uint32 argb = 0;
        bool pending = false;
        bool isProduct = false;
        /** How many Mappings rows bind to this pastille (0 when unused). */
        int mappingCount = 0;
        juce::String headerText;
        /** Second ListBox row of a visual pair (half of kColumnHeaderHeight). */
        bool isLowerListHalf = false;
    };

    struct StickySection
    {
        int upperListRow = 0;
        juce::String title;
    };

    int getNumRows() override;
    void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent& e) override;
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent& e) override;
    void scrollBarMoved(juce::ScrollBar* scrollBar, double newRangeStart) override;

    bool isMappingCountHit(int row, int x) const noexcept;
    bool selectionIsMappingCountClick() const;
    void notifyMappingCountClicked(const juce::String& pastilleName);
    void updateMappingCountHover(const juce::MouseEvent& e);
    void updateMappingCountHoverAt(juce::Point<int> listPos);
    void refreshMappingCountHoverFromPointer();
    void clearMappingCountHover();

    void rebuildRows();
    void updateActionButtonEnablement();
    void layoutActionButtons(juce::Rectangle<int> actionRow);
    void layoutStickyHeadersOverlay();
    void attachScrollBarListener();
    /** True when no editor skin is loaded (empty shell: no New/Duplicate/Del). */
    bool isShellEmpty() const noexcept { return skinId_.isEmpty(); }
    /** Push upper + lower half-height ListBox rows for one visual content row. */
    void pushSplitListRow(Row row);
    int nextUpperListRow(int row) const noexcept;
    bool isRowSelectedVisually(int row) const noexcept;
    int paintWidth() const noexcept;
    void roundFlagsForRow(int row, bool& roundTop, bool& roundBottom) const noexcept;
    static bool isBlockBoundary(RowKind kind) noexcept;
    static bool isContentRow(RowKind kind) noexcept;
    void appendPaletteGroup(const juce::String& title, std::vector<Row>& colours, bool leadGap);
    void paintColourRow(juce::Graphics& g, const Row& item, int row, int yOff);
    void paintStickySectionHeaders(juce::Graphics& g);
    bool stickyHeaderHitTest(int x, int y) const noexcept;
    int stickyHeaderYInView(int sectionIndex, int scrollY) const noexcept;

    TSS::DevSkin::Layer* layer_ = nullptr;
    juce::String skinId_;
    bool readOnly_ = true;
    bool suppressSelectionNotify_ = false;
    /** selectedRowsChanged already applied a count-click filter for this mouse gesture. */
    bool mappingCountClickHandled_ = false;
    /** Pastille name whose mapping-count column is hovered (badge paint); empty = none. */
    juce::String hoveredMappingCountName_;
    std::vector<Row> rows_;
    std::vector<StickySection> stickySections_;
    /** Trailing count column width (widest digit string / badge among colour rows). */
    int mappingCountColumnWidth_ = 0;
    EscapeKeyForwarder escapeForwarder_;
    MappingCountHoverListener mappingCountHover_;
    juce::ScrollBar* listenedScrollBar_ = nullptr;

    juce::Label titleLabel_;
    juce::Rectangle<int> titleTabBounds_;
    juce::ListBox list_ { "Palette", this };
    StickyHeadersOverlay stickyHeaders_ { *this };
    juce::TextButton addButton_ { "New" };
    juce::TextButton duplicateButton_ { "Dup" };
    juce::TextButton deleteButton_ { "Del" };
};
