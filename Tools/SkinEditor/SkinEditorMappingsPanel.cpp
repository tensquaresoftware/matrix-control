#include "Tools/SkinEditor/SkinEditorMappingsPanel.h"

#include <map>

#include "GUI/Skins/Dev/DevSkinInventory.h"
#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinEditor/SkinEditorChrome.h"

namespace
{
    constexpr int kColPad = 6;
    /** Same as Palette: swatch left (x=4) → panel right = panelW - contentPad - 4. */
    constexpr int kMappedSwatchToTableRight = SkinEditorChrome::kPalettePanelWidth
                                             - SkinEditorChrome::kPanelContentPad
                                             - 4;
    /** Original swatch size in a 22px row with 4px insets. */
    constexpr int kSwatchSize = 14;
    constexpr int kMidInset = 4;
    /** Extra air on first/last mapping rows so edge gaps match the 8px between swatches. */
    constexpr int kEdgeExtra = 4;
    /**
     * Inset around each table so the 1 px outside focus ring is drawn inside the
     * viewport (verticals were clipped at x = 0 / right edge).
     */
    constexpr int kFocusPad = 1;

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
}

void SkinEditorMappingsPanel::setSelectedPastilleName(const juce::String& name)
{
    selectedPastille_ = name;
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
    layoutContent();
    content_.repaint();
}

void SkinEditorMappingsPanel::paintStickyLegend(juce::Graphics& g, juce::Rectangle<int> area) const
{
    SkinEditorChrome::fillTableRow(g, area.toFloat(), SkinEditorChrome::surfaceSection(),
                                   true, true);
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
    SkinEditorChrome::paintPanelFrame(g, getLocalBounds(), titleTabBounds_);
    SkinEditorChrome::paintRoundedWell(g, viewport_.getBounds(), SkinEditorChrome::surfacePanel());
    if (! stickyLegendBounds_.isEmpty())
        paintStickyLegend(g, stickyLegendBounds_);
}

void SkinEditorMappingsPanel::resized()
{
    const auto chrome = SkinEditorChrome::layoutPanel(getLocalBounds(),
                                                      SkinEditorChrome::titleTabWidth("MAPPINGS"));
    titleTabBounds_ = chrome.tab;
    titleLabel_.setBounds(chrome.tab);
    auto bounds = chrome.content;

    // Top: Object bar, then content pad (same rhythm as Palette).
    auto actions = bounds.removeFromTop(SkinEditorChrome::kButtonHeight);
    bounds.removeFromTop(SkinEditorChrome::kPanelContentPad);

    stickyBandBounds_ = bounds.removeFromTop(SkinEditorChrome::kColumnHeaderHeight
                                             + SkinEditorChrome::kColumnHeaderGap);
    stickyLegendBounds_ = stickyBandBounds_.removeFromTop(SkinEditorChrome::kColumnHeaderHeight);

    auto listBounds = bounds;
    listBounds.setRight(listBounds.getRight() + SkinEditorChrome::kPanelContentPad);
    viewport_.setBounds(listBounds);
    layoutContent();

    // Table / legend / Object controls share the inset paint width (room for focus ring).
    const int tableW = paintWidth();
    const int tableX = viewport_.getX() + kFocusPad;
    stickyLegendBounds_.setX(tableX);
    stickyLegendBounds_.setWidth(tableW);

    // Gap after "Object:" = measureLabelWidth's +8, same as Product: / Editor: (no extra kButtonGap).
    auto actionRow = juce::Rectangle<int>(tableX, actions.getY(), tableW, actions.getHeight());
    const int labelW = SkinEditorChrome::measureLabelWidth(objectLabel_.getText(),
                                                          SkinEditorChrome::fontRegular());
    objectLabel_.setBounds(actionRow.removeFromLeft(labelW));
    objectCombo_.setBounds(actionRow);
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

void SkinEditorMappingsPanel::paintRow(juce::Graphics& g,
                                       const Row& item,
                                       juce::Rectangle<float> rowArea,
                                       bool roundBottom,
                                       bool highlighted) const
{
    const int y = (int) rowArea.getY();
    const int width = (int) rowArea.getWidth();

    if (item.kind == RowKind::kSection)
    {
        SkinEditorChrome::paintTableSectionHeader(g, rowArea, item.sectionTitle);
        return;
    }

    SkinEditorChrome::fillTableRow(g, rowArea, SkinEditorChrome::surfaceTable(),
                                   false, roundBottom);

    if (highlighted)
        SkinEditorChrome::fillTableRow(g, rowArea, SkinEditorChrome::surfaceTableRowHighlight(),
                                       false, roundBottom);

    // Text + swatch share the 22 px content band (edge extras sit outside it).
    const int contentY = y + (item.thickenTop ? kEdgeExtra : 0);
    const int contentH = SkinEditorChrome::kColumnHeaderHeight;
    const auto cols = columnsForWidth(width);
    g.setFont(SkinEditorChrome::fontRegular());
    g.setColour(SkinEditorChrome::textTableBody());
    g.drawText(item.propertyLabel, kColPad, contentY, juce::jmax(0, cols.propertyW - 4), contentH,
               juce::Justification::centredLeft, false);
    g.drawText(item.stateLabel, kColPad + cols.propertyW, contentY, juce::jmax(0, cols.stateW - 4),
               contentH, juce::Justification::centredLeft, false);

    if (item.unbound)
    {
        g.setColour(juce::Colour(0xffe0a020));
        g.drawText("Unbound", cols.mappedX, contentY, cols.mappedW, contentH,
                   juce::Justification::centredLeft, false);
    }
    else
    {
        SkinEditorChrome::paintColourSwatch(g,
                                            juce::Rectangle<int>(cols.mappedX, contentY + kMidInset,
                                                                 kSwatchSize, kSwatchSize),
                                            juce::Colour(item.pastilleArgb));
        g.setColour(SkinEditorChrome::textTableBody());
        g.drawText(item.pastilleName,
                   cols.mappedX + kSwatchSize + 6, contentY,
                   juce::jmax(0, cols.mappedW - kSwatchSize - 8), contentH,
                   juce::Justification::centredLeft, false);
    }
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
            owner_.paintRow(g, item, rowArea, roundBottom, i == owner_.selectedRow_);
        }
        y = nextY;
    }

    if (owner_.focusedObject_ >= 0
        && owner_.focusedObject_ < static_cast<int>(owner_.objects_.size()))
    {
        const auto& table = owner_.objects_[static_cast<size_t>(owner_.focusedObject_)];
        if (table.tableBottom > table.sectionY)
        {
            const auto sectionRow = juce::Rectangle<float>(
                (float) x0, (float) table.sectionY, (float) width,
                (float) SkinEditorChrome::kColumnHeaderHeight);
            SkinEditorChrome::strokeObjectTableFocusOutside(g, sectionRow,
                                                            (float) table.tableBottom,
                                                            table.name);
        }
    }
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
    repaint();

    // Map click x into table-local coordinates (content is inset by kFocusPad).
    if (e.x - kFocusPad < columnsForWidth(owner_.paintWidth()).mappedX)
        return;

    owner_.tryBindRow(row);
}

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

    if (! objects_.empty())
    {
        objectCombo_.setSelectedItemIndex(select, juce::dontSendNotification);
        focusedObject_ = select;
    }
    else
    {
        focusedObject_ = -1;
    }

    updatingObjectCombo_ = false;
}

void SkinEditorMappingsPanel::rebuildRows()
{
    rows_.clear();
    objects_.clear();
    if (layer_ == nullptr)
        return;

    const auto* skin = layer_->findSkin(skinId_);
    if (skin == nullptr)
        return;

    const auto keys = TSS::DevSkin::allStructuredKeys();
    std::vector<TSS::SkinObjectType> objectOrder;
    std::map<TSS::SkinObjectType, std::vector<TSS::SkinColourKey>> keysByObject;

    for (const auto& key : keys)
    {
        auto& bucket = keysByObject[key.object];
        if (bucket.empty())
            objectOrder.push_back(key.object);
        bucket.push_back(key);
    }

    bool firstObject = true;
    int y = kFocusPad; // leave room for the outside focus ring above the first table
    for (const auto object : objectOrder)
    {
        if (! firstObject)
        {
            Row gap;
            gap.kind = RowKind::kGap;
            gap.heightPx = SkinEditorChrome::kTableGapHeight;
            rows_.push_back(gap);
            y += gap.heightPx;
        }
        firstObject = false;

        const int objectIndex = static_cast<int>(objects_.size());
        ObjectTable table;
        table.name = TSS::DevSkin::toString(object);
        table.sectionY = y;

        Row section;
        section.kind = RowKind::kSection;
        section.sectionTitle = table.name;
        section.objectIndex = objectIndex;
        section.heightPx = SkinEditorChrome::kColumnHeaderHeight;
        rows_.push_back(section);
        y += section.heightPx;

        table.tableTop = y;
        const auto& objectKeys = keysByObject[object];
        for (size_t i = 0; i < objectKeys.size(); ++i)
        {
            const auto& key = objectKeys[i];
            Row row;
            row.kind = RowKind::kMapping;
            row.objectIndex = objectIndex;
            row.key = key;
            row.propertyLabel = TSS::DevSkin::toString(key.property);
            row.stateLabel = TSS::DevSkin::toString(key.state);
            row.thickenTop = (i == 0);
            row.thickenBottom = (i + 1 == objectKeys.size());
            row.heightPx = SkinEditorChrome::kColumnHeaderHeight
                           + (row.thickenTop ? kEdgeExtra : 0)
                           + (row.thickenBottom ? kEdgeExtra : 0);

            const auto it = skin->mappings.find(key);
            if (it == skin->mappings.end() || it->second.isEmpty())
            {
                row.unbound = true;
            }
            else
            {
                row.pastilleName = it->second;
                for (const auto& pastille : skin->palette)
                {
                    if (pastille.name == row.pastilleName)
                    {
                        row.pastilleArgb = pastille.argb;
                        break;
                    }
                }
            }

            rows_.push_back(row);
            y += row.heightPx;
        }
        table.tableBottom = y;
        objects_.push_back(table);
    }
}

void SkinEditorMappingsPanel::tryBindRow(int row)
{
    if (readOnly_ || selectedPastille_.isEmpty() || onBindRequested == nullptr)
        return;
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;
    if (rows_[static_cast<size_t>(row)].kind != RowKind::kMapping)
        return;

    onBindRequested(rows_[static_cast<size_t>(row)].key, selectedPastille_);
}
