#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"

/** Scrollable object tables; sticky "Property / State / Mapped colour" legend + gap. */
class SkinEditorMappingsPanel final : public juce::Component
{
public:
    SkinEditorMappingsPanel();

    void setLayer(TSS::DevSkin::Layer* layer);
    void setActiveSkinId(const juce::String& skinId);
    /** Whole-skin Unbound total for the Editor skin (Export gate) — not the Compare Product view. */
    void setUnboundCount(int count);
    void setReadOnly(bool readOnly);
    void refresh();

    /** When true, batch maps stay in the current gesture direction after a successful write. */
    bool isHoldColour() const noexcept { return holdColourToggle_.getToggleState(); }

    /** Hold colour toggle flipped - refresh OPERATION coaching when a bind gesture is active. */
    std::function<void()> onHoldColourChanged;

    /** Property / State cell click - main owns Idle / HeldColour / ArmedRow (bind vs re-arm). */
    std::function<void(TSS::SkinColourKey key)> onRowActivated;
    /** Mapped colour cell click on a bound row - main selects that pastille for COLOR / SV edit. */
    std::function<void(TSS::SkinColourKey key)> onMappedColourClicked;
    /** Escape while the table content has focus - clear mapping mode in main. */
    std::function<bool()> onEscapeRequested;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    enum class RowKind
    {
        kGap,
        kSection,
        kMapping
    };

    struct Row
    {
        RowKind kind = RowKind::kMapping;
        juce::String sectionTitle;
        int objectIndex = -1; // index into objects_ for section / mapping rows
        TSS::SkinColourKey key {};
        juce::String propertyLabel;
        juce::String stateLabel;
        juce::String pastilleName;
        juce::uint32 pastilleArgb = 0;
        bool pastillePending = false;
        bool unbound = false;
        int heightPx = 1;
        /** Thicken mapping row: +4px top and/or bottom (swatch stays 12px). */
        bool thickenTop = false;
        bool thickenBottom = false;
    };

    struct ObjectTable
    {
        juce::String name;
        int sectionY = 0;       // content Y of the object-header tab
        int tableTop = 0;       // first mapping row
        int tableBottom = 0;    // bottom of last mapping row
    };

    /** One paint pass over logical rows — avoids the old 1px ListBox slot tax. */
    class Content final : public juce::Component
    {
    public:
        explicit Content(SkinEditorMappingsPanel& owner) : owner_(owner)
        {
            setWantsKeyboardFocus(true);
        }
        void paint(juce::Graphics& g) override;
        void mouseUp(const juce::MouseEvent& e) override;
        bool keyPressed(const juce::KeyPress& key) override;

    private:
        SkinEditorMappingsPanel& owner_;
    };

    /** Packs paintRow arguments so the method stays within the parameter gate. */
    struct PaintRowSpec
    {
        const Row* item = nullptr;
        juce::Rectangle<float> rowArea;
        bool roundBottom = false;
        bool highlighted = false;
    };

    // Rebuild
    void rebuildRows();
    /** True when no editor skin is loaded (empty shell: no Object/Hold/legend). */
    bool isShellEmpty() const noexcept { return skinId_.isEmpty(); }
    /** Appends one object section + mapping rows; returns content Y after the table. */
    int appendObjectTable(const TSS::DevSkin::NamedSkin& skin,
                          TSS::SkinObjectType object,
                          const std::vector<TSS::SkinColourKey>& objectKeys,
                          int y);
    Row makeMappingRow(const TSS::DevSkin::NamedSkin& skin,
                       const TSS::SkinColourKey& key,
                       int objectIndex) const;
    void rebuildObjectCombo(const juce::String& preferName);

    // Paint
    void paintStickyLegend(juce::Graphics& g, juce::Rectangle<int> area) const;
    void paintRow(juce::Graphics& g, PaintRowSpec spec) const;
    void paintMappingCells(juce::Graphics& g,
                           const Row& item,
                           juce::Rectangle<int> cellArea) const;
    void paintFocusedObjectOutline(juce::Graphics& g, int tableX, int tableWidth) const;

    // Bind / focus
    void tryBindRow(int row);
    /** Bound rows only; Unbound cells are a no-op. */
    void tryEditMappedColour(int row);
    void setFocusedObject(int objectIndex, bool scrollIntoPlace);
    void scrollFocusedObjectIntoPlace();

    // Layout
    void layoutShellEmpty(juce::Rectangle<int> content);
    void layoutShellFilled(juce::Rectangle<int> content);
    void layoutContent();
    int rowIndexAtY(int y) const noexcept;
    int paintWidth() const noexcept;

    TSS::DevSkin::Layer* layer_ = nullptr;
    juce::String skinId_;
    bool readOnly_ = true;
    bool updatingObjectCombo_ = false;
    int selectedRow_ = -1;
    int focusedObject_ = -1;
    std::vector<Row> rows_;
    std::vector<ObjectTable> objects_;

    juce::Label titleLabel_;
    juce::Rectangle<int> titleTabBounds_;
    /** Sticky legend row only (gap below is empty within stickyBandBounds_). */
    juce::Rectangle<int> stickyLegendBounds_;
    juce::Rectangle<int> stickyBandBounds_;
    juce::Label objectLabel_ { {}, "Object:" };
    juce::ComboBox objectCombo_;
    juce::ToggleButton holdColourToggle_ { "Hold colour" };
    /** Whole-skin Unbound total (Export gate); painted in the Mapped colour column. */
    int unboundCount_ = 0;
    juce::Rectangle<int> unboundLabelBounds_;
    juce::Rectangle<int> unboundBadgeBounds_;
    juce::Viewport viewport_;
    Content content_ { *this };
};
