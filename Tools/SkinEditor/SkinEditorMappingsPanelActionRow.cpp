#include "Tools/SkinEditor/SkinEditorMappingsPanel.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"

namespace
{
    struct ColumnLayout
    {
        int propertyW = 0;
        int stateW = 0;
        int colourSetW = 0;
        int mappedX = 0;
        int mappedW = 0;
    };

    ColumnLayout columnsForWidth(int totalWidth) noexcept
    {
        const int mappedSpan = juce::jmin(SkinEditorChrome::kPaletteListPaintWidth,
                                          juce::jmax(0, totalWidth));
        return { SkinEditorChrome::kMappingsPropertyColumnWidth,
                 SkinEditorChrome::kMappingsStateColumnWidth,
                 SkinEditorChrome::kMappingsColourSetColumnWidth,
                 totalWidth - mappedSpan,
                 mappedSpan };
    }
}

void SkinEditorMappingsPanel::clearAllColumnFilters()
{
    propertyFilter_.clear();
    stateFilter_.clear();
    colourSetFilter_.clear();
    mappedColourFilter_.clear();
}

bool SkinEditorMappingsPanel::anyColumnFilterActive() const noexcept
{
    return propertyFilter_.isNotEmpty()
        || stateFilter_.isNotEmpty()
        || colourSetFilter_.isNotEmpty()
        || mappedColourFilter_.isNotEmpty();
}

void SkinEditorMappingsPanel::clearColumnFilter(FilterColumn column)
{
    filterValue(column).clear();
    refresh();
}

void SkinEditorMappingsPanel::setColumnFilter(FilterColumn column, const juce::String& value)
{
    auto& slot = filterValue(column);
    if (slot == value)
        return;
    slot = value;
    refresh();
}

void SkinEditorMappingsPanel::setMappedColourFilter(const juce::String& pastilleName)
{
    setColumnFilter(FilterColumn::kMappedColour, pastilleName);
}

void SkinEditorMappingsPanel::layoutActionRow(juce::Rectangle<int> actions, int tableX, int tableW)
{
    auto actionRow = juce::Rectangle<int>(tableX, actions.getY(), tableW, actions.getHeight());
    const int labelW = SkinEditorChrome::measureLabelWidth(objectLabel_.getText(),
                                                          SkinEditorChrome::fontRegular());
    objectLabel_.setBounds(actionRow.removeFromLeft(labelW));

    const auto cols = columnsForWidth(tableW);
    // Unbound + badge share Mapped colour column left; optical +1 matches table cells.
    constexpr int kUnboundBadgeGap = 6;
    const auto unboundFont = SkinEditorChrome::fontRegular();
    const int unboundLabelW = SkinEditorChrome::measureTextWidth(unboundFont, "Unbound");
    const int badgeW = SkinEditorChrome::countBadgeWidth(unboundCount_);
    const int mappedLeft = tableX + cols.mappedX;
    const int unboundLeft = mappedLeft - 1;
    unboundLabelBounds_ = { unboundLeft, actions.getY(), unboundLabelW, actions.getHeight() };
    unboundBadgeBounds_ = { unboundLeft + unboundLabelW + kUnboundBadgeGap,
                            actions.getY(), badgeW, actions.getHeight() };

    // Global clear: right edge flush with sticky legend / dark header (tableX + tableW).
    globalClearBounds_ = {};
    if (anyColumnFilterActive())
    {
        const int clearW = SkinEditorChrome::clearBadgeWidth();
        const int clearX = tableX + tableW - clearW;
        if (clearX >= unboundBadgeBounds_.getRight() + SkinEditorChrome::kButtonGap)
            globalClearBounds_ = { clearX, actions.getY(), clearW, actions.getHeight() };
    }

    // Object combo fixed width, then gap, then Hold — Unbound stays on Mapped colour.
    const int comboMax = juce::jmax(0, unboundLeft - SkinEditorChrome::kButtonGap - actionRow.getX());
    const int comboW = juce::jmin(SkinEditorChrome::kObjectComboWidth, comboMax);
    objectCombo_.setBounds(actionRow.getX(), actions.getY(), comboW, actions.getHeight());

    const int holdW = SkinEditorChrome::measureTextWidth(SkinEditorChrome::fontRegular(),
                                                        holdColourToggle_.getButtonText())
                      + 16 + 6; // tick box + gap to label
    const int holdLeft = objectCombo_.getRight() + SkinEditorChrome::kMappingsObjectToHoldGap;
    const int holdMaxRight = unboundLeft - SkinEditorChrome::kButtonGap;
    holdColourToggle_.setBounds(holdLeft, actions.getY(),
                                juce::jmin(holdW, juce::jmax(0, holdMaxRight - holdLeft)),
                                actions.getHeight());
}
