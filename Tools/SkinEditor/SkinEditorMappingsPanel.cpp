#include "Tools/SkinEditor/SkinEditorMappingsPanel.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"
#include "Tools/SkinEditor/SkinEditorMappingModePolicy.h"

namespace
{
    constexpr int kColPad = 6;
    // Match Palette selection-highlight width (kPaletteListPaintWidth).
    constexpr int kMappedSwatchToTableRight = SkinEditorChrome::kPaletteListPaintWidth;
    constexpr int kSwatchSize = SkinEditorChrome::kColourSwatchSize;
    constexpr int kEdgeExtra = 4; // first/last row air so edge gaps match inter-swatch 8px
    constexpr int kFocusPad = 1;  // keep 1px outside focus ring inside the viewport

    struct ColumnLayout
    {
        int propertyW = 0;
        int stateW = 0;
        int mappedX = 0;
        int mappedW = 0;
    };

    ColumnLayout columnsForWidth(int totalWidth) noexcept
    {
        const int mappedSpan = juce::jmin(kMappedSwatchToTableRight, juce::jmax(0, totalWidth));
        const int mappedX = totalWidth - mappedSpan;
        const int rest = juce::jmax(0, mappedX - kColPad);
        const int propertyW = rest / 2;
        const int stateW = rest - propertyW;
        return { propertyW, stateW, mappedX, mappedSpan };
    }
}

SkinEditorMappingsPanel::SkinEditorMappingsPanel()
{
    SkinEditorChrome::stylePanelTitle(titleLabel_, "MAPPINGS");
    addAndMakeVisible(titleLabel_);

    // Same label / combo language as Skins "Product:" / "Editor:".
    objectLabel_.setFont(SkinEditorChrome::fontRegular());
    objectLabel_.setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
    objectLabel_.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    objectLabel_.setJustificationType(juce::Justification::centredLeft);
    SkinEditorChrome::preventTextSquish(objectLabel_);
    addAndMakeVisible(objectLabel_);

    objectCombo_.setColour(juce::ComboBox::backgroundColourId, SkinEditorChrome::controlFill());
    objectCombo_.setColour(juce::ComboBox::outlineColourId, SkinEditorChrome::controlOutline());
    objectCombo_.setColour(juce::ComboBox::arrowColourId, SkinEditorChrome::textMuted());
    objectCombo_.setColour(juce::ComboBox::textColourId, SkinEditorChrome::textPrimary());
    objectCombo_.setColour(juce::ComboBox::buttonColourId, SkinEditorChrome::controlFill());
    objectCombo_.onChange = [this]
    {
        if (updatingObjectCombo_)
            return;
        setFocusedObject(objectCombo_.getSelectedItemIndex(), true);
    };
    addAndMakeVisible(objectCombo_);

    holdColourToggle_.setClickingTogglesState(true);
    holdColourToggle_.setToggleState(false, juce::dontSendNotification);
    holdColourToggle_.setTooltip("Hold the current mapping gesture to map several targets");
    // Same muted label colour as "Object:" - box chrome comes from LookAndFeel::drawToggleButton.
    holdColourToggle_.setColour(juce::ToggleButton::textColourId, SkinEditorChrome::textMuted());
    holdColourToggle_.onClick = [this]
    {
        if (onHoldColourChanged != nullptr)
            onHoldColourChanged();
    };
    addAndMakeVisible(holdColourToggle_);

    viewport_.setViewedComponent(&content_, false);
    viewport_.setScrollBarsShown(true, false);
    viewport_.setScrollBarThickness(SkinEditorChrome::kScrollTrackWidth);
    viewport_.setColour(juce::ScrollBar::backgroundColourId, juce::Colours::transparentBlack);
    content_.setPaintingIsUnclipped(true); // allow 1 px focus stroke to sit flush outside
    addAndMakeVisible(viewport_);
}

void SkinEditorMappingsPanel::setLayer(TSS::DevSkin::Layer* layer)
{
    layer_ = layer;
}

void SkinEditorMappingsPanel::setActiveSkinId(const juce::String& skinId)
{
    skinId_ = skinId;
}

void SkinEditorMappingsPanel::setReadOnly(bool readOnly)
{
    readOnly_ = readOnly;
    holdColourToggle_.setEnabled(! readOnly);
}

void SkinEditorMappingsPanel::refresh()
{
    selectedRow_ = -1;
    const juce::String keep = (focusedObject_ >= 0
                               && focusedObject_ < static_cast<int>(objects_.size()))
                                  ? objects_[static_cast<size_t>(focusedObject_)].name
                                  : juce::String();
    rebuildRows();
    rebuildObjectCombo(keep);
    resized();
    layoutContent();
    content_.repaint();
}

// Rebuild (row builders live in SkinEditorMappingsPanelRows.cpp)

void SkinEditorMappingsPanel::rebuildObjectCombo(const juce::String& preferName)
{
    updatingObjectCombo_ = true;
    objectCombo_.clear(juce::dontSendNotification);

    int select = 0;
    for (int i = 0; i < static_cast<int>(objects_.size()); ++i)
    {
        objectCombo_.addItem(objects_[static_cast<size_t>(i)].name, i + 1);
        if (preferName.isNotEmpty() && objects_[static_cast<size_t>(i)].name == preferName)
            select = i;
    }

    focusedObject_ = -1;
    if (! objects_.empty())
    {
        objectCombo_.setSelectedItemIndex(select, juce::dontSendNotification);
        focusedObject_ = select;
    }
    objectCombo_.setEnabled(! objects_.empty());
    updatingObjectCombo_ = false;
}

// Paint

void SkinEditorMappingsPanel::paintStickyLegend(juce::Graphics& g, juce::Rectangle<int> area) const
{
    SkinEditorChrome::fillTableRow(g, { area.toFloat(), SkinEditorChrome::surfaceSection(),
                                        true, true });
    const auto cols = columnsForWidth(area.getWidth());
    g.setColour(SkinEditorChrome::textTableHeader());
    g.setFont(SkinEditorChrome::fontBold());
    g.drawText("Property",
               area.getX() + kColPad, area.getY(), juce::jmax(0, cols.propertyW - 4), area.getHeight(),
               juce::Justification::centredLeft, false);
    g.drawText("State",
               area.getX() + kColPad + cols.propertyW, area.getY(),
               juce::jmax(0, cols.stateW - 4), area.getHeight(),
               juce::Justification::centredLeft, false);
    g.drawText("Mapped colour",
               area.getX() + cols.mappedX, area.getY(), cols.mappedW, area.getHeight(),
               juce::Justification::centredLeft, false);
}

void SkinEditorMappingsPanel::paint(juce::Graphics& g)
{
    SkinEditorChrome::paintPanelFrame(g, getLocalBounds(), titleTabBounds_, 0);
    SkinEditorChrome::paintRoundedWell(g, viewport_.getBounds(), SkinEditorChrome::surfacePanel());
    if (! stickyLegendBounds_.isEmpty())
        paintStickyLegend(g, stickyLegendBounds_);
}

void SkinEditorMappingsPanel::paintMappingCells(juce::Graphics& g,
                                                const Row& item,
                                                juce::Rectangle<int> cellArea) const
{
    const int x0 = cellArea.getX();
    const int contentY = cellArea.getY();
    const int contentH = cellArea.getHeight();
    const auto cols = columnsForWidth(cellArea.getWidth());
    g.setFont(SkinEditorChrome::fontRegular());
    g.setColour(SkinEditorChrome::textTableBody());
    g.drawText(item.propertyLabel, x0 + kColPad, contentY, juce::jmax(0, cols.propertyW - 4), contentH,
               juce::Justification::centredLeft, false);
    g.drawText(item.stateLabel, x0 + kColPad + cols.propertyW, contentY, juce::jmax(0, cols.stateW - 4),
               contentH, juce::Justification::centredLeft, false);

    if (item.unbound)
    {
        g.setColour(SkinEditorChrome::attention());
        g.drawText("Unbound", x0 + cols.mappedX, contentY, cols.mappedW, contentH,
                   juce::Justification::centredLeft, false);
        return;
    }

    const int swY = contentY + (contentH - kSwatchSize) / 2;
    const auto swatch = juce::Rectangle<int>(x0 + cols.mappedX, swY, kSwatchSize, kSwatchSize);
    if (item.pastillePending)
        SkinEditorChrome::paintPendingColourSwatch(g, swatch);
    else
        SkinEditorChrome::paintColourSwatch(g, swatch, juce::Colour(item.pastilleArgb));
    g.setColour(SkinEditorChrome::textTableBody());
    g.drawText(item.pastilleName,
               x0 + cols.mappedX + kSwatchSize + 6, contentY,
               juce::jmax(0, cols.mappedW - kSwatchSize - 8), contentH,
               juce::Justification::centredLeft, false);
}

void SkinEditorMappingsPanel::paintRow(juce::Graphics& g, PaintRowSpec spec) const
{
    const auto& item = *spec.item;

    if (item.kind == RowKind::kSection)
    {
        SkinEditorChrome::paintTableSectionHeader(g, spec.rowArea, item.sectionTitle);
        return;
    }

    SkinEditorChrome::fillTableRow(g, { spec.rowArea, SkinEditorChrome::surfaceTable(),
                                        false, spec.roundBottom });

    // Highlight matches mid-row height only — first/last thicken padding stays unhighlighted.
    const float contentY = spec.rowArea.getY() + (item.thickenTop ? (float) kEdgeExtra : 0.0f);
    const auto contentArea = juce::Rectangle<float>(spec.rowArea.getX(), contentY,
                                                    spec.rowArea.getWidth(),
                                                    (float) SkinEditorChrome::kColumnHeaderHeight);
    if (spec.highlighted)
        SkinEditorChrome::fillTableRow(g, { contentArea, SkinEditorChrome::surfaceTableRowHighlight(),
                                            false, false });

    // Table-local cell origin (x = 0): fills use rowArea (inset by kFocusPad); text stays aligned to columns.
    paintMappingCells(g, item, { 0, (int) contentY, (int) spec.rowArea.getWidth(),
                                 SkinEditorChrome::kColumnHeaderHeight });
}

void SkinEditorMappingsPanel::paintFocusedObjectOutline(juce::Graphics& g,
                                                        int tableX,
                                                        int tableWidth) const
{
    if (focusedObject_ < 0 || focusedObject_ >= static_cast<int>(objects_.size()))
        return;

    const auto& table = objects_[static_cast<size_t>(focusedObject_)];
    if (table.tableBottom <= table.sectionY)
        return;

    const auto sectionRow = juce::Rectangle<float>(
        (float) tableX, (float) table.sectionY, (float) tableWidth,
        (float) SkinEditorChrome::kColumnHeaderHeight);
    SkinEditorChrome::strokeObjectTableFocusOutside(g, sectionRow,
                                                    (float) table.tableBottom,
                                                    table.name);
}

void SkinEditorMappingsPanel::Content::paint(juce::Graphics& g)
{
    const int width = owner_.paintWidth();
    const int x0 = kFocusPad;
    const auto clip = g.getClipBounds();
    int y = 0;
    for (int i = 0; i < static_cast<int>(owner_.rows_.size()); ++i)
    {
        const auto& item = owner_.rows_[static_cast<size_t>(i)];
        const int nextY = y + item.heightPx;
        if (nextY <= clip.getY())
        {
            y = nextY;
            continue;
        }
        if (y >= clip.getBottom())
            break;

        if (item.kind != RowKind::kGap)
        {
            const bool roundBottom = (item.kind == RowKind::kMapping)
                                     && (i + 1 >= static_cast<int>(owner_.rows_.size())
                                         || owner_.rows_[static_cast<size_t>(i + 1)].kind != RowKind::kMapping);
            const auto rowArea = juce::Rectangle<float>((float) x0, (float) y, (float) width,
                                                        (float) item.heightPx);
            owner_.paintRow(g, { &item, rowArea, roundBottom, i == owner_.selectedRow_ });
        }
        y = nextY;
    }

    owner_.paintFocusedObjectOutline(g, x0, width);
}

// Bind / focus

void SkinEditorMappingsPanel::tryBindRow(int row)
{
    if (readOnly_ || onRowActivated == nullptr)
        return;
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;
    if (rows_[static_cast<size_t>(row)].kind != RowKind::kMapping)
        return;

    onRowActivated(rows_[static_cast<size_t>(row)].key);
}

void SkinEditorMappingsPanel::tryEditMappedColour(int row)
{
    if (onMappedColourClicked == nullptr)
        return;
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    const auto& item = rows_[static_cast<size_t>(row)];
    if (item.kind != RowKind::kMapping || item.unbound)
        return;

    onMappedColourClicked(item.key);
}

void SkinEditorMappingsPanel::setFocusedObject(int objectIndex, bool scrollIntoPlace)
{
    if (objectIndex < 0 || objectIndex >= static_cast<int>(objects_.size()))
        return;

    focusedObject_ = objectIndex;

    updatingObjectCombo_ = true;
    objectCombo_.setSelectedItemIndex(objectIndex, juce::dontSendNotification);
    updatingObjectCombo_ = false;

    if (scrollIntoPlace)
        scrollFocusedObjectIntoPlace();

    content_.repaint();
}

void SkinEditorMappingsPanel::scrollFocusedObjectIntoPlace()
{
    if (focusedObject_ < 0 || focusedObject_ >= static_cast<int>(objects_.size()))
        return;

    // Pin tab under sticky legend + gap, keeping the 1 px focus ring above the tab visible.
    const int targetY = juce::jmax(0, objects_[static_cast<size_t>(focusedObject_)].sectionY
                                          - kFocusPad);
    const int maxY = juce::jmax(0, content_.getHeight() - viewport_.getHeight());
    viewport_.setViewPosition(0, juce::jlimit(0, maxY, targetY));
}

void SkinEditorMappingsPanel::Content::mouseUp(const juce::MouseEvent& e)
{
    const int row = owner_.rowIndexAtY(e.y);
    if (row < 0)
        return;

    const auto& item = owner_.rows_[static_cast<size_t>(row)];
    if (item.kind == RowKind::kGap)
        return;

    if (item.objectIndex >= 0)
        owner_.setFocusedObject(item.objectIndex, false);

    if (item.kind != RowKind::kMapping)
        return;

    owner_.selectedRow_ = row;
    grabKeyboardFocus();
    repaint();

    const int mappedX = columnsForWidth(owner_.paintWidth()).mappedX;
    const bool releaseInMapped = SkinEditorMappingPolicy::isMappedColourColumn(e.x, mappedX);
    const bool pressInMapped = SkinEditorMappingPolicy::isMappedColourColumn(e.getMouseDownX(), mappedX);
    if (releaseInMapped != pressInMapped)
        return; // press and release in different columns: highlight/focus only

    if (releaseInMapped)
        owner_.tryEditMappedColour(row);
    else
        owner_.tryBindRow(row);
}

bool SkinEditorMappingsPanel::Content::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey && owner_.onEscapeRequested != nullptr)
        return owner_.onEscapeRequested();
    return false;
}

// Layout

void SkinEditorMappingsPanel::resized()
{
    const auto chrome = SkinEditorChrome::layoutPanel(getLocalBounds(),
                                                      SkinEditorChrome::titleTabWidth("MAPPINGS"),
                                                      0);
    titleTabBounds_ = chrome.tab;
    titleLabel_.setBounds(chrome.tab);
    if (isShellEmpty())
        layoutShellEmpty(chrome.content);
    else
        layoutShellFilled(chrome.content);
}

void SkinEditorMappingsPanel::layoutShellEmpty(juce::Rectangle<int> content)
{
    objectLabel_.setVisible(false);
    objectCombo_.setVisible(false);
    holdColourToggle_.setVisible(false);
    objectLabel_.setBounds({});
    objectCombo_.setBounds({});
    holdColourToggle_.setBounds({});
    stickyBandBounds_ = {};
    stickyLegendBounds_ = {};
    auto listBounds = content;
    listBounds.setRight(listBounds.getRight() + SkinEditorChrome::kPanelContentPad);
    viewport_.setBounds(listBounds);
    layoutContent();
}

void SkinEditorMappingsPanel::layoutShellFilled(juce::Rectangle<int> content)
{
    objectLabel_.setVisible(true);
    objectCombo_.setVisible(true);
    holdColourToggle_.setVisible(true);

    auto actions = content.removeFromTop(SkinEditorChrome::kButtonHeight);
    content.removeFromTop(SkinEditorChrome::kPanelContentPad);

    stickyBandBounds_ = content.removeFromTop(SkinEditorChrome::kColumnHeaderHeight
                                              + SkinEditorChrome::kColumnHeaderGap);
    stickyLegendBounds_ = stickyBandBounds_.removeFromTop(SkinEditorChrome::kColumnHeaderHeight);

    auto listBounds = content;
    listBounds.setRight(listBounds.getRight() + SkinEditorChrome::kPanelContentPad);
    viewport_.setBounds(listBounds);
    layoutContent();

    const int tableW = paintWidth();
    const int tableX = viewport_.getX() + kFocusPad;
    stickyLegendBounds_.setX(tableX);
    stickyLegendBounds_.setWidth(tableW);

    auto actionRow = juce::Rectangle<int>(tableX, actions.getY(), tableW, actions.getHeight());
    const int labelW = SkinEditorChrome::measureLabelWidth(objectLabel_.getText(),
                                                          SkinEditorChrome::fontRegular());
    objectLabel_.setBounds(actionRow.removeFromLeft(labelW));

    const auto cols = columnsForWidth(tableW);
    const int holdLeft = tableX + cols.mappedX;
    const int holdW = SkinEditorChrome::measureTextWidth(SkinEditorChrome::fontRegular(),
                                                        holdColourToggle_.getButtonText())
                      + 16 + 6; // button-styled tick box + gap to label
    holdColourToggle_.setBounds(holdLeft, actions.getY(),
                                juce::jmin(holdW, tableX + tableW - holdLeft),
                                actions.getHeight());

    const int comboRight = holdLeft - SkinEditorChrome::kButtonGap;
    objectCombo_.setBounds(actionRow.getX(), actions.getY(),
                           juce::jmax(0, comboRight - actionRow.getX()),
                           actions.getHeight());
}

int SkinEditorMappingsPanel::paintWidth() const noexcept
{
    // Leave kFocusPad on both sides so the outside focus stroke is not clipped.
    return juce::jmax(1, viewport_.getWidth() - SkinEditorChrome::kScrollTrackWidth
                                             - 2 * kFocusPad);
}

void SkinEditorMappingsPanel::layoutContent()
{
    int totalH = kFocusPad; // bottom pad mirrors the top inset from rebuildRows
    for (const auto& row : rows_)
        totalH += row.heightPx;

    const int w = juce::jmax(1, viewport_.getWidth());
    content_.setSize(w, juce::jmax(viewport_.getHeight(), totalH));
}

int SkinEditorMappingsPanel::rowIndexAtY(int y) const noexcept
{
    int cursor = 0;
    for (int i = 0; i < static_cast<int>(rows_.size()); ++i)
    {
        const int next = cursor + rows_[static_cast<size_t>(i)].heightPx;
        if (y >= cursor && y < next)
            return i;
        cursor = next;
    }
    return -1;
}
