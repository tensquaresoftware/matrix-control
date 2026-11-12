#include "Tools/SkinDesigner/SkinDesignerMappingsPanel.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "Tools/SkinDesigner/SkinDesignerChrome.h"
#include "Tools/SkinDesigner/SkinDesignerMappingModePolicy.h"
#include "Tools/SkinDesigner/SkinDesignerMappingsColumns.h"
#include "Tools/SkinDesigner/SkinDesignerUiHelpers.h"

namespace
{
    constexpr int kSwatchSize = SkinDesignerChrome::kColourSwatchSize;
    constexpr int kEdgeExtra = 2; // first/last row air
    constexpr int kFocusPad = SkinDesignerChrome::kMappingsFocusPad;
}

SkinDesignerMappingsPanel::SkinDesignerMappingsPanel()
{
    SkinDesignerChrome::stylePanelTitle(titleLabel_, "MAPPINGS");
    addAndMakeVisible(titleLabel_);

    // Same label / combo language as Skins "Product:" / "Designer:".
    objectLabel_.setFont(SkinDesignerChrome::fontRegular());
    objectLabel_.setColour(juce::Label::textColourId, SkinDesignerChrome::textMuted());
    objectLabel_.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    objectLabel_.setJustificationType(juce::Justification::centredLeft);
    SkinDesignerChrome::preventTextSquish(objectLabel_);
    addAndMakeVisible(objectLabel_);

    objectCombo_.setColour(juce::ComboBox::backgroundColourId, SkinDesignerChrome::controlFill());
    objectCombo_.setColour(juce::ComboBox::outlineColourId, SkinDesignerChrome::controlOutline());
    objectCombo_.setColour(juce::ComboBox::arrowColourId, SkinDesignerChrome::textMuted());
    objectCombo_.setColour(juce::ComboBox::textColourId, SkinDesignerChrome::textPrimary());
    objectCombo_.setColour(juce::ComboBox::buttonColourId, SkinDesignerChrome::controlFill());
    objectCombo_.onChange = [this]
    {
        if (updatingObjectCombo_)
            return;
        applyObjectComboSelection();
    };
    addAndMakeVisible(objectCombo_);

    holdColourToggle_.setClickingTogglesState(true);
    holdColourToggle_.setToggleState(false, juce::dontSendNotification);
    holdColourToggle_.setTooltip("Hold the current mapping gesture to map several targets");
    // Same muted label colour as "Object:" - checked box chrome in LookAndFeel::drawToggleButton.
    holdColourToggle_.setColour(juce::ToggleButton::textColourId, SkinDesignerChrome::textMuted());
    holdColourToggle_.onClick = [this]
    {
        if (onHoldColourChanged != nullptr)
            onHoldColourChanged();
    };
    addAndMakeVisible(holdColourToggle_);

    viewport_.setViewedComponent(&content_, false);
    viewport_.setScrollBarsShown(true, false);
    viewport_.setScrollBarThickness(SkinDesignerChrome::kScrollTrackWidth);
    viewport_.setColour(juce::ScrollBar::backgroundColourId, juce::Colours::transparentBlack);
    content_.setPaintingIsUnclipped(true);
    addAndMakeVisible(viewport_);
}

void SkinDesignerMappingsPanel::setLayer(TSS::DevSkin::Layer* layer)
{
    layer_ = layer;
}

void SkinDesignerMappingsPanel::setActiveSkinId(const juce::String& skinId)
{
    if (skinId_ == skinId)
        return;
    skinId_ = skinId;
    clearAllColumnFilters();
    objectFilterName_.clear();
    clearRowSelection();
    sortSpec_ = SkinDesignerMappingsColumnFilter::defaultMappingsSortSpec();
}

void SkinDesignerMappingsPanel::setUnboundCount(int count)
{
    unboundCount_ = juce::jmax(0, count);
}

void SkinDesignerMappingsPanel::setReadOnly(bool readOnly)
{
    readOnly_ = readOnly;
    holdColourToggle_.setEnabled(! readOnly);
}

void SkinDesignerMappingsPanel::refresh()
{
    hoveredLinkKey_.reset();
    hoveredPropertyBindKey_.reset();
    const auto scrollAnchor = captureViewScrollAnchor();
    rebuildRows();
    rebuildObjectCombo();
    resized();
    layoutContent();
    // Object-relative restore: absolute Y jumps when filters rewrite groups above the view.
    restoreViewScrollAnchor(scrollAnchor);
    // Unbound witness is painted on the panel (not Content) — must damage both.
    content_.repaint();
    repaint();
}

// Paint (paintStickyLegend lives in SkinDesignerMappingsPanelFilters.cpp)

void SkinDesignerMappingsPanel::paint(juce::Graphics& g)
{
    SkinDesignerChrome::paintPanelFrame(g, getLocalBounds(), titleTabBounds_, 0);
    SkinDesignerChrome::paintRoundedWell(g, viewport_.getBounds(), SkinDesignerChrome::surfacePanel());
    if (! stickyLegendBounds_.isEmpty())
        paintStickyLegend(g, stickyLegendBounds_);

    if (! unboundLabelBounds_.isEmpty())
    {
        // Amber when Export gate is blocked; muted chrome when Unbound = 0.
        const auto unboundColour = (unboundCount_ > 0) ? SkinDesignerChrome::attention()
                                                       : SkinDesignerChrome::textMuted();
        g.setFont(SkinDesignerChrome::fontRegular());
        g.setColour(unboundColour);
        g.drawText("Unbound", unboundLabelBounds_, juce::Justification::centredLeft, false);
        SkinDesignerChrome::drawCountBadge(g, { unboundBadgeBounds_, unboundCount_, unboundColour,
                                              0, false });
    }

    if (! objectClearBounds_.isEmpty())
        SkinDesignerChrome::drawClearBadge(g, objectClearBounds_);

    if (! globalClearBounds_.isEmpty())
        SkinDesignerChrome::drawClearBadge(g, globalClearBounds_);
}

juce::Colour SkinDesignerMappingsPanel::propertyBindLabelColour(const Row& item) const noexcept
{
    const bool propertyHover = propertyBindHoverEnabled_
                               && hoveredPropertyBindKey_.has_value()
                               && *hoveredPropertyBindKey_ == item.key
                               && ! propertyBindHoverColour_.isTransparent();
    if (propertyHover
        && SkinDesignerChrome::propertyBindHoverNeedsDarkText(propertyBindHoverColour_))
        return SkinDesignerChrome::textOnLightPropertyBindHover();
    return SkinDesignerChrome::textTableBody();
}

void SkinDesignerMappingsPanel::paintMappedColourCell(juce::Graphics& g,
                                                    const Row& item,
                                                    juce::Rectangle<int> area) const
{
    if (item.unbound)
    {
        g.setColour(SkinDesignerChrome::attention());
        g.drawText("Unbound", area, juce::Justification::centredLeft, false);
        return;
    }

    const int swY = area.getY() + (area.getHeight() - kSwatchSize) / 2;
    const auto swatch = juce::Rectangle<int>(area.getX(), swY, kSwatchSize, kSwatchSize);
    if (item.pastillePending)
        SkinDesignerChrome::paintPendingColourSwatch(g, swatch);
    else
        SkinDesignerChrome::paintColourSwatch(g, swatch, juce::Colour(item.pastilleArgb));
    g.setColour(SkinDesignerChrome::textTableBody());
    g.drawText(SkinDesignerUi::displayPastilleName(item.pastilleName),
               area.getX() + kSwatchSize + SkinDesignerChrome::kMappingsColumnPad,
               area.getY(),
               juce::jmax(0, area.getWidth() - kSwatchSize - SkinDesignerChrome::kMappingsColumnPad),
               area.getHeight(),
               juce::Justification::centredLeft,
               false);
}

void SkinDesignerMappingsPanel::paintMappingCells(juce::Graphics& g,
                                                const Row& item,
                                                juce::Rectangle<int> cellArea) const
{
    const int x0 = cellArea.getX();
    const int contentY = cellArea.getY();
    const int contentH = cellArea.getHeight();
    const auto cols = SkinDesignerMappingsColumns::forWidth(cellArea.getWidth());
    g.setFont(SkinDesignerChrome::fontRegular());
    g.setColour(propertyBindLabelColour(item));
    g.drawText(item.propertyLabel,
               x0 + cols.contentLeft(cols.propertyLeft()), contentY,
               cols.contentWidth(cols.propertyW), contentH,
               juce::Justification::centredLeft, false);
    g.setColour(SkinDesignerChrome::textTableBody());
    g.drawText(item.stateLabel,
               x0 + cols.contentLeft(cols.stateLeft()), contentY,
               cols.contentWidth(cols.stateW), contentH,
               juce::Justification::centredLeft, false);

    paintColourSetCell(g, item,
                       { x0 + cols.contentLeft(cols.colourSetLeft()),
                         contentY,
                         cols.contentWidth(cols.colourSetW),
                         contentH });
    paintLinkCell(g, item, { x0 + cols.linkLeft(), contentY, cols.linkW, contentH });
    paintMappedColourCell(g, item,
                          { x0 + cols.contentLeft(cols.mappedX), contentY,
                            cols.contentWidth(cols.mappedW), contentH });
}

void SkinDesignerMappingsPanel::paintRow(juce::Graphics& g, PaintRowSpec spec) const
{
    const auto& item = *spec.item;

    if (item.kind == RowKind::kSection)
        return; // Tab + continuity rail painted in Content::paint.

    // Row fill comes from continuous cut column slabs (Content::paint).
    const float contentY = spec.rowArea.getY() + (item.thickenTop ? (float) kEdgeExtra : 0.0f);
    const auto cellArea = juce::Rectangle<int>((int) spec.rowArea.getX(),
                                               (int) contentY,
                                               (int) spec.rowArea.getWidth(),
                                               SkinDesignerChrome::kColumnHeaderHeight);
    const auto cols = SkinDesignerMappingsColumns::forWidth(cellArea.getWidth());
    constexpr int kInsetX = SkinDesignerChrome::kMappingsHighlightInsetX;
    constexpr int kInsetY = SkinDesignerChrome::kMappingsHighlightInsetY;
    constexpr float kCorner = SkinDesignerChrome::kMappingsHighlightCorner;
    if (spec.highlighted)
        SkinDesignerMappingsColumns::fillCutColumnSlabs(
            g, cols, { cellArea, SkinDesignerChrome::surfaceTableRowHighlight(),
                       true, true, 0, 4, kInsetX, kInsetY, kCorner });

    const bool propertyHover = propertyBindHoverEnabled_
                               && hoveredPropertyBindKey_.has_value()
                               && *hoveredPropertyBindKey_ == item.key;
    if (propertyHover)
    {
        // Exact Palette pastille (not a blend) — pending keeps the grey fallback.
        const auto hoverFill = propertyBindHoverColour_.isTransparent()
                                   ? SkinDesignerChrome::surfaceTablePropertyBindHover()
                                   : propertyBindHoverColour_;
        SkinDesignerMappingsColumns::fillCutColumnSlabs(
            g, cols, { cellArea, hoverFill, true, true, 0, 0, kInsetX, kInsetY, kCorner });
    }
    paintMappingCells(g, item, cellArea);
}

void SkinDesignerMappingsPanel::Content::paint(juce::Graphics& g)
{
    const int width = owner_.paintWidth();
    const int x0 = kFocusPad;
    const auto clip = g.getClipBounds();
    const auto cols = SkinDesignerMappingsColumns::forWidth(width);

    for (const auto& table : owner_.objects_)
    {
        if (table.tableBottom <= clip.getY() || table.sectionY >= clip.getBottom())
            continue;
        owner_.paintObjectTableChrome(g, { &table, &cols, x0, width });
    }

    // Match rebuildRows / ObjectTable metrics: top inset leaves room for the outside focus stroke.
    int y = kFocusPad;
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
            const auto rowArea = juce::Rectangle<float>((float) x0, (float) y, (float) width,
                                                        (float) item.heightPx);
            const bool highlighted = item.kind == RowKind::kMapping
                                     && owner_.isKeyGroupRevealed(item.key);
            owner_.paintRow(g, { &item, rowArea, false, highlighted });
        }
        y = nextY;
    }
}

void SkinDesignerMappingsPanel::Content::mouseUp(const juce::MouseEvent& e)
{
    if (! e.mouseWasClicked())
        return;

    const int row = owner_.rowIndexAtY(e.y);
    // Empty list air (below / above tables) or inter-table gap — drop row selection.
    if (row < 0 || owner_.rows_[static_cast<size_t>(row)].kind == RowKind::kGap)
    {
        owner_.clearRowSelection();
        return;
    }

    const auto& item = owner_.rows_[static_cast<size_t>(row)];
    if (item.objectIndex >= 0)
        owner_.setFocusedObject(item.objectIndex);
    if (item.kind != RowKind::kMapping)
        return;

    owner_.handleMappingRowMouseUp(row, e);
    grabKeyboardFocus();
    repaint();
    owner_.repaint();
}

void SkinDesignerMappingsPanel::Content::mouseMove(const juce::MouseEvent& e)
{
    const int row = owner_.rowIndexAtY(e.y);
    std::optional<TSS::SkinColourKey> nextLink;
    std::optional<TSS::SkinColourKey> nextPropertyBind;
    if (row >= 0)
    {
        const auto& item = owner_.rows_[static_cast<size_t>(row)];
        if (item.kind == RowKind::kMapping)
        {
            const auto cols = SkinDesignerMappingsColumns::forWidth(owner_.paintWidth());
            const int x = e.x - kFocusPad;
            // Grey Link chain only while the cursor is in the Link column.
            if (SkinDesignerMappingsColumns::isLinkColumnX(x, cols)
                && SkinDesignerMappingLink::showsHoverChain(owner_.currentLinkGroups(),
                                                          owner_.selectedKeys_,
                                                          item.key,
                                                          owner_.objectFilterType()))
                nextLink = item.key;
            if (owner_.propertyBindHoverEnabled_
                && SkinDesignerMappingsColumns::isPropertyColumnX(x, cols))
                nextPropertyBind = item.key;
        }
    }
    if (nextLink == owner_.hoveredLinkKey_
        && nextPropertyBind == owner_.hoveredPropertyBindKey_)
        return;
    owner_.hoveredLinkKey_ = nextLink;
    owner_.hoveredPropertyBindKey_ = nextPropertyBind;
    repaint();
}

void SkinDesignerMappingsPanel::Content::mouseExit(const juce::MouseEvent&)
{
    const bool hadHover = owner_.hoveredLinkKey_.has_value()
                          || owner_.hoveredPropertyBindKey_.has_value();
    owner_.hoveredLinkKey_.reset();
    owner_.hoveredPropertyBindKey_.reset();
    if (hadHover)
        repaint();
}

void SkinDesignerMappingsPanel::Content::mouseDoubleClick(const juce::MouseEvent& e)
{
    const int row = owner_.rowIndexAtY(e.y);
    if (row < 0)
        return;

    const auto& item = owner_.rows_[static_cast<size_t>(row)];
    if (item.kind != RowKind::kMapping)
        return;

    const auto cols = SkinDesignerMappingsColumns::forWidth(owner_.paintWidth());
    if (! SkinDesignerMappingsColumns::isColourSetColumnX(e.x - kFocusPad, cols))
        return;

    owner_.tryEditColourSetLabel(row);
}

bool SkinDesignerMappingsPanel::Content::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey && owner_.onEscapeRequested != nullptr)
        return owner_.onEscapeRequested();
    return false;
}

// Layout

void SkinDesignerMappingsPanel::resized()
{
    const auto chrome = SkinDesignerChrome::layoutPanel(getLocalBounds(),
                                                      SkinDesignerChrome::titleTabWidth("MAPPINGS"),
                                                      0);
    titleTabBounds_ = chrome.tab;
    titleLabel_.setBounds(chrome.tab);
    if (isShellEmpty())
        layoutShellEmpty(chrome.content);
    else
        layoutShellFilled(chrome.content);
}

void SkinDesignerMappingsPanel::layoutShellEmpty(juce::Rectangle<int> content)
{
    objectLabel_.setVisible(false);
    objectCombo_.setVisible(false);
    holdColourToggle_.setVisible(false);
    objectLabel_.setBounds({});
    objectCombo_.setBounds({});
    holdColourToggle_.setBounds({});
    unboundLabelBounds_ = {};
    unboundBadgeBounds_ = {};
    objectClearBounds_ = {};
    stickyBandBounds_ = {};
    stickyLegendBounds_ = {};
    clearLegendHitTargets();
    auto listBounds = content;
    listBounds.setRight(listBounds.getRight() + SkinDesignerChrome::kPanelContentPad);
    viewport_.setBounds(listBounds);
    layoutContent();
}

void SkinDesignerMappingsPanel::layoutShellFilled(juce::Rectangle<int> content)
{
    objectLabel_.setVisible(true);
    objectCombo_.setVisible(true);
    holdColourToggle_.setVisible(true);

    auto actions = content.removeFromTop(SkinDesignerChrome::kButtonHeight);
    content.removeFromTop(SkinDesignerChrome::kPanelContentPad);

    stickyBandBounds_ = content.removeFromTop(SkinDesignerChrome::kColumnHeaderHeight
                                              + SkinDesignerChrome::kColumnHeaderGap);
    stickyLegendBounds_ = stickyBandBounds_.removeFromTop(SkinDesignerChrome::kColumnHeaderHeight);

    auto listBounds = content;
    listBounds.setRight(listBounds.getRight() + SkinDesignerChrome::kPanelContentPad);
    viewport_.setBounds(listBounds);
    layoutContent();

    const int tableW = paintWidth();
    const int tableX = viewport_.getX() + kFocusPad;
    stickyLegendBounds_.setX(tableX);
    stickyLegendBounds_.setWidth(tableW);
    layoutLegendHitTargets();
    layoutActionRow(actions, tableX, tableW);
}

int SkinDesignerMappingsPanel::paintWidth() const noexcept
{
    // Leave kFocusPad on both sides so the outside focus stroke is not clipped.
    return juce::jmax(1, viewport_.getWidth() - SkinDesignerChrome::kScrollTrackWidth
                                             - 2 * kFocusPad);
}

void SkinDesignerMappingsPanel::layoutContent()
{
    // Top + bottom insets: outside 1 px focus stroke must not clip at either scroll extreme.
    int totalH = 2 * kFocusPad;
    for (const auto& row : rows_)
        totalH += row.heightPx;

    const int w = juce::jmax(1, viewport_.getWidth());
    content_.setSize(w, juce::jmax(viewport_.getHeight(), totalH));
}

int SkinDesignerMappingsPanel::rowIndexAtY(int y) const noexcept
{
    int cursor = kFocusPad;
    for (int i = 0; i < static_cast<int>(rows_.size()); ++i)
    {
        const int next = cursor + rows_[static_cast<size_t>(i)].heightPx;
        if (y >= cursor && y < next)
            return i;
        cursor = next;
    }
    return -1;
}
