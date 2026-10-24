#pragma once

#include <functional>
#include <optional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"

/** Scrollable object tables; sticky "Property / State / Mapped colour" legend + gap. */
class SkinEditorMappingsPanel final : public juce::Component
{
public:
    SkinEditorMappingsPanel();

    void setLayer(TSS::DevSkin::Layer* layer);
    void setActiveSkinId(const juce::String& skinId);
    void setReadOnly(bool readOnly);
    void setSelectedPastilleName(const juce::String& name);
    void refresh();

    /** Bind using the Palette selection (pastilleName may be empty → ignored). */
    std::function<void(TSS::SkinColourKey key, const juce::String& pastilleName)> onBindRequested;

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
        bool unbound = false;
        int heightPx = 1;
        /** Thicken mapping row: +4px top and/or bottom (swatch stays 14px). */
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
        explicit Content(SkinEditorMappingsPanel& owner) : owner_(owner) {}
        void paint(juce::Graphics& g) override;
        void mouseUp(const juce::MouseEvent& e) override;

    private:
        SkinEditorMappingsPanel& owner_;
    };

    void rebuildRows();
    void rebuildObjectCombo(const juce::String& preferName);
    void tryBindRow(int row);
    void setFocusedObject(int objectIndex, bool scrollIntoPlace);
    void scrollFocusedObjectIntoPlace();
    void paintStickyLegend(juce::Graphics& g, juce::Rectangle<int> area) const;
    void paintRow(juce::Graphics& g, const Row& item, juce::Rectangle<float> rowArea,
                  bool roundBottom, bool highlighted) const;
    void layoutContent();
    int rowIndexAtY(int y) const noexcept;
    int paintWidth() const noexcept;

    TSS::DevSkin::Layer* layer_ = nullptr;
    juce::String skinId_;
    juce::String selectedPastille_;
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
    juce::Viewport viewport_;
    Content content_ { *this };
};
