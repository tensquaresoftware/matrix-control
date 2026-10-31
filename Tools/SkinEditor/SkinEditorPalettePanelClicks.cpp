#include "Tools/SkinEditor/SkinEditorPalettePanel.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"
#include "Tools/SkinEditor/SkinEditorPalettePanelGeometry.h"

bool SkinEditorPalettePanel::isMappingCountHit(int row, int x) const noexcept
{
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return false;

    const auto& item = rows_[static_cast<size_t>(row)];
    if (item.kind != RowKind::kColour || item.mappingCount <= 0)
        return false;

    const int countLeft = SkinEditorPaletteGeometry::mappingCountLeft(paintWidth(),
                                                                     mappingCountColumnWidth_);
    return x >= countLeft && x < paintWidth();
}

bool SkinEditorPalettePanel::selectionIsMappingCountClick() const
{
    // New-row selection runs on mouseDown (button still down). Same-row re-clicks
    // skip selectedRowsChanged and are handled in listBoxItemClicked instead.
    if (! juce::ModifierKeys::getCurrentModifiersRealtime().isAnyMouseButtonDown())
        return false;

    const auto listPos = list_.getMouseXYRelative();
    const int row = list_.getRowContainingPosition(listPos.x, listPos.y);
    if (row < 0)
        return false;

    if (auto* rowComp = list_.getComponentForRowNumber(row))
        return isMappingCountHit(row, rowComp->getLocalPoint(&list_, listPos).x);

    // Row not realized yet — list X matches row paint X for this ListBox layout.
    return isMappingCountHit(row, listPos.x);
}

void SkinEditorPalettePanel::notifyMappingCountClicked(const juce::String& pastilleName)
{
    if (pastilleName.isNotEmpty() && onMappingCountClicked)
        onMappingCountClicked(pastilleName);
}

void SkinEditorPalettePanel::MappingCountHoverListener::mouseMove(const juce::MouseEvent& e)
{
    if (owner != nullptr)
        owner->updateMappingCountHover(e);
}

void SkinEditorPalettePanel::MappingCountHoverListener::mouseDrag(const juce::MouseEvent& e)
{
    if (owner != nullptr)
        owner->updateMappingCountHover(e);
}

void SkinEditorPalettePanel::MappingCountHoverListener::mouseExit(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    if (owner == nullptr)
        return;
    // Deep listener also exits nested row comps while still inside the list.
    if (! owner->list_.isMouseOver(true))
        owner->clearMappingCountHover();
    else
        owner->refreshMappingCountHoverFromPointer();
}

void SkinEditorPalettePanel::updateMappingCountHover(const juce::MouseEvent& e)
{
    updateMappingCountHoverAt(e.getEventRelativeTo(&list_).getPosition());
}

void SkinEditorPalettePanel::updateMappingCountHoverAt(juce::Point<int> listPos)
{
    const int row = list_.getRowContainingPosition(listPos.x, listPos.y);
    juce::String nextName;
    if (row >= 0 && row < static_cast<int>(rows_.size()))
    {
        int x = listPos.x;
        if (auto* rowComp = list_.getComponentForRowNumber(row))
            x = rowComp->getLocalPoint(&list_, listPos).x;
        if (isMappingCountHit(row, x))
            nextName = rows_[static_cast<size_t>(row)].name;
    }

    if (nextName == hoveredMappingCountName_)
        return;
    hoveredMappingCountName_ = nextName;
    list_.repaint();
}

void SkinEditorPalettePanel::refreshMappingCountHoverFromPointer()
{
    if (! list_.isShowing() || ! list_.isMouseOver(true))
    {
        clearMappingCountHover();
        return;
    }
    updateMappingCountHoverAt(list_.getMouseXYRelative());
}

void SkinEditorPalettePanel::clearMappingCountHover()
{
    if (hoveredMappingCountName_.isEmpty())
        return;
    hoveredMappingCountName_.clear();
    list_.repaint();
}

void SkinEditorPalettePanel::selectedRowsChanged(int)
{
    // Split rows paint a full visual line across two ListBox slots; JUCE only
    // invalidates the selected index, so force a full list repaint for highlight.
    list_.repaint();

    updateActionButtonEnablement();
    if (suppressSelectionNotify_)
        return;

    // Count digits filter Mappings without Hold / Arm / bind side effects.
    if (selectionIsMappingCountClick())
    {
        mappingCountClickHandled_ = true;
        notifyMappingCountClicked(getSelectedPastilleName());
        return;
    }

    mappingCountClickHandled_ = false;
    if (onPastilleSelected)
        onPastilleSelected(getSelectedPastilleName());
}

void SkinEditorPalettePanel::listBoxItemClicked(int row, const juce::MouseEvent& e)
{
    if (! isMappingCountHit(row, e.x))
    {
        mappingCountClickHandled_ = false;
        return;
    }

    // Selection already changed: selectedRowsChanged applied the filter.
    if (mappingCountClickHandled_)
    {
        mappingCountClickHandled_ = false;
        return;
    }

    // Same-row re-click (no selectedRowsChanged) — still update the filter.
    if (row >= 0 && row < static_cast<int>(rows_.size()))
        notifyMappingCountClicked(rows_[static_cast<size_t>(row)].name);
}

void SkinEditorPalettePanel::listBoxItemDoubleClicked(int row, const juce::MouseEvent& e)
{
    if (readOnly_ || row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    const auto& item = rows_[static_cast<size_t>(row)];
    if (item.kind != RowKind::kColour || item.isProduct)
        return;

    namespace Geo = SkinEditorPaletteGeometry;
    // Rename is name-text only — swatch and trailing count do nothing.
    const int sw = SkinEditorChrome::kColourSwatchSize;
    const int nameLeft = Geo::kSwatchX + sw + Geo::kNameGap;
    const int countLeft = Geo::mappingCountLeft(paintWidth(), mappingCountColumnWidth_);
    if (e.x < nameLeft || e.x >= countLeft - Geo::kNameCountGap)
        return;

    if (onRenamePastilleRequested)
        onRenamePastilleRequested(item.name);
}
