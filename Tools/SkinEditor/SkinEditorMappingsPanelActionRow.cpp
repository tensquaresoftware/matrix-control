#include "Tools/SkinEditor/SkinEditorMappingsPanel.h"

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinEditor/SkinEditorChrome.h"
#include "Tools/SkinEditor/SkinEditorMappingsColumnFilter.h"
#include "Tools/SkinEditor/SkinEditorMappingsColumns.h"

void SkinEditorMappingsPanel::applyObjectComboSelection()
{
    objectFilterName_ = SkinEditorMappingsColumnFilter::objectFilterNameFromComboSelection(
        objectCombo_.getSelectedId(), objectCombo_.getText());
    focusedObject_ = -1;
    refresh();
}

void SkinEditorMappingsPanel::clearObjectMacroFilter()
{
    if (objectFilterName_.isEmpty())
        return;
    objectFilterName_.clear();
    focusedObject_ = -1;
    refresh();
}

void SkinEditorMappingsPanel::rebuildObjectCombo()
{
    updatingObjectCombo_ = true;
    objectCombo_.clear(juce::dontSendNotification);
    objectCombo_.addItem(SkinEditorChrome::kAllObjectsComboText, 1);

    juce::StringArray names;
    for (const auto object : SkinEditorMappingsColumnFilter::mappingsObjectDisplayOrder())
        names.addIfNotAlreadyThere(TSS::DevSkin::toString(object));
    names.sort(true); // A→Z; "- All objects -" stays id 1 above this list.

    int nextId = 2;
    int selectedId = 1;
    for (const auto& name : names)
    {
        objectCombo_.addItem(name, nextId);
        if (objectFilterName_.isNotEmpty() && name == objectFilterName_)
            selectedId = nextId;
        ++nextId;
    }

    if (objectFilterName_.isNotEmpty() && selectedId == 1)
        objectFilterName_.clear();

    objectCombo_.setSelectedId(selectedId, juce::dontSendNotification);
    objectCombo_.setEnabled(objectCombo_.getNumItems() > 1);
    updatingObjectCombo_ = false;
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

void SkinEditorMappingsPanel::setPropertyBindHoverEnabled(bool enabled, juce::Colour preview)
{
    if (! enabled)
        preview = {};
    if (propertyBindHoverEnabled_ == enabled && propertyBindHoverColour_ == preview)
        return;
    propertyBindHoverEnabled_ = enabled;
    propertyBindHoverColour_ = preview;
    if (! enabled)
        hoveredPropertyBindKey_.reset();
    content_.repaint();
}

void SkinEditorMappingsPanel::revealPastilleMappings(const juce::String& pastilleName)
{
    const bool objectWasOn = objectFilterName_.isNotEmpty();
    objectFilterName_.clear();
    focusedObject_ = -1;
    if (mappedColourFilter_ == pastilleName)
    {
        if (objectWasOn)
            refresh();
        return;
    }
    mappedColourFilter_ = pastilleName;
    refresh();
}

void SkinEditorMappingsPanel::layoutActionRow(juce::Rectangle<int> actions, int tableX, int tableW)
{
    objectClearBounds_ = {};
    auto actionRow = juce::Rectangle<int>(tableX, actions.getY(), tableW, actions.getHeight());
    const int labelW = SkinEditorChrome::measureLabelWidth(objectLabel_.getText(),
                                                          SkinEditorChrome::fontRegular());
    objectLabel_.setBounds(actionRow.removeFromLeft(labelW));

    const auto cols = SkinEditorMappingsColumns::forWidth(tableW);
    // Unbound + badge: same content origin as "Mapped colour" header (column + cell pad).
    constexpr int kUnboundBadgeGap = 6;
    const auto unboundFont = SkinEditorChrome::fontRegular();
    const int unboundLabelW = SkinEditorChrome::measureTextWidth(unboundFont, "Unbound");
    const int badgeW = SkinEditorChrome::linkCountBadgeWidth();
    const int unboundLeft = tableX + cols.contentLeft(cols.mappedX);
    unboundLabelBounds_ = { unboundLeft, actions.getY(), unboundLabelW, actions.getHeight() };
    unboundBadgeBounds_ = { unboundLeft + unboundLabelW + kUnboundBadgeGap,
                            actions.getY(), badgeW, actions.getHeight() };

    const bool objectFiltered = isObjectMacroFilterActive();
    const int clearW = objectFiltered ? SkinEditorChrome::clearBadgeWidth() : 0;
    const int clearGap = objectFiltered ? SkinEditorChrome::kButtonGap : 0;

    // Object combo fixed width, then optional yellow clear, then Hold gap.
    const int comboMax = juce::jmax(0, unboundLeft - SkinEditorChrome::kButtonGap
                                           - clearW - clearGap - actionRow.getX());
    const int comboW = juce::jmin(SkinEditorChrome::kObjectComboWidth, comboMax);
    objectCombo_.setBounds(actionRow.getX(), actions.getY(), comboW, actions.getHeight());

    int holdAnchorLeft = objectCombo_.getRight();
    if (objectFiltered)
    {
        objectClearBounds_ = { objectCombo_.getRight() + clearGap,
                               actions.getY(),
                               clearW,
                               actions.getHeight() };
        holdAnchorLeft = objectClearBounds_.getRight();
    }

    const int holdW = SkinEditorChrome::measureTextWidth(SkinEditorChrome::fontRegular(),
                                                        holdColourToggle_.getButtonText())
                      + 16 + 6; // tick box + gap to label
    // Tick box: Colour set content origin, then 6 px left (toward column boundary).
    const int holdAlignX = tableX + cols.contentLeft(cols.colourSetLeft()) - 6;
    const int holdLeft = SkinEditorChrome::mappingsHoldLeft(holdAnchorLeft, holdAlignX);
    const int holdMaxRight = unboundLeft - SkinEditorChrome::kButtonGap;
    holdColourToggle_.setBounds(holdLeft, actions.getY(),
                                juce::jmin(holdW, juce::jmax(0, holdMaxRight - holdLeft)),
                                actions.getHeight());
}
