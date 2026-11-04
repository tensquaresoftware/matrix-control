#include "Tools/SkinEditor/SkinEditorMappingsPanel.h"

#include <algorithm>

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinEditor/SkinEditorChrome.h"
#include "Tools/SkinEditor/SkinEditorMappingsColumns.h"

void SkinEditorMappingsPanel::clearSelectionAndHovers()
{
    selectedKeys_.clear();
    selectionAnchorKey_.reset();
    hoveredLinkKey_.reset();
    hoveredPropertyBindKey_.reset();
}

void SkinEditorMappingsPanel::clearRowSelection()
{
    const bool hadPropertyHover = hoveredPropertyBindKey_.has_value();
    if (selectedKeys_.empty() && ! selectionAnchorKey_.has_value())
    {
        hoveredLinkKey_.reset();
        hoveredPropertyBindKey_.reset();
        if (hadPropertyHover)
        {
            content_.repaint();
            repaint();
        }
        return;
    }
    clearSelectionAndHovers();
    content_.repaint();
    repaint();
}

void SkinEditorMappingsPanel::clearTableRowSelection()
{
    clearRowSelection();
}

bool SkinEditorMappingsPanel::hitActionRowChrome(juce::Point<int> pos) const noexcept
{
    return objectCombo_.getBounds().contains(pos) || holdColourToggle_.getBounds().contains(pos)
           || objectLabel_.getBounds().contains(pos) || unboundLabelBounds_.contains(pos)
           || unboundBadgeBounds_.contains(pos) || objectClearBounds_.contains(pos);
}

bool SkinEditorMappingsPanel::preservesRowSelectionForEvent(const juce::MouseEvent& e) const
{
    auto* origin = e.originalComponent;
    if (origin == nullptr || (origin != this && ! isParentOf(origin)))
        return false;

    const auto pos = e.getEventRelativeTo(const_cast<SkinEditorMappingsPanel*>(this)).getPosition();
    if (stickyBandBounds_.contains(pos) || stickyLegendBounds_.contains(pos)
        || hitActionRowChrome(pos))
        return true;
    if (! viewport_.getBounds().contains(pos))
        return false;

    const auto contentPos = e.getEventRelativeTo(const_cast<Content*>(&content_)).getPosition();
    const int row = rowIndexAtY(contentPos.y);
    return row >= 0 && rows_[static_cast<size_t>(row)].kind != RowKind::kGap;
}

bool SkinEditorMappingsPanel::isKeySelected(const TSS::SkinColourKey& key) const noexcept
{
    return std::find(selectedKeys_.begin(), selectedKeys_.end(), key) != selectedKeys_.end();
}

SkinEditorMappingLink::Groups SkinEditorMappingsPanel::currentLinkGroups() const
{
    const auto* skin = (layer_ != nullptr && skinId_.isNotEmpty())
                           ? layer_->findSkin(skinId_)
                           : nullptr;
    if (skin == nullptr)
        return {};
    return skin->mappingLinkGroups;
}

std::optional<TSS::SkinObjectType> SkinEditorMappingsPanel::objectFilterType() const
{
    if (objectFilterName_.isEmpty())
        return std::nullopt;

    for (const auto object : SkinEditorMappingsColumnFilter::mappingsObjectDisplayOrder())
        if (TSS::DevSkin::toString(object) == objectFilterName_)
            return object;
    return std::nullopt;
}

bool SkinEditorMappingsPanel::isKeyGroupRevealed(const TSS::SkinColourKey& key) const
{
    const auto revealed = SkinEditorMappingLink::revealedKeys(currentLinkGroups(), selectedKeys_);
    return revealed.find(key) != revealed.end();
}

void SkinEditorMappingsPanel::selectRowKeyRange(const TSS::SkinColourKey& endKey)
{
    if (! selectionAnchorKey_.has_value())
    {
        selectRowKey(endKey, false, false);
        return;
    }

    std::vector<TSS::SkinColourKey> visible;
    visible.reserve(rows_.size());
    for (const auto& row : rows_)
        if (row.kind == RowKind::kMapping)
            visible.push_back(row.key);

    const auto findIndex = [&](const TSS::SkinColourKey& key) -> int
    {
        for (int i = 0; i < static_cast<int>(visible.size()); ++i)
            if (visible[static_cast<size_t>(i)] == key)
                return i;
        return -1;
    };

    const int anchorIndex = findIndex(*selectionAnchorKey_);
    const int endIndex = findIndex(endKey);
    if (anchorIndex < 0 || endIndex < 0)
    {
        selectRowKey(endKey, false, false);
        return;
    }

    const int lo = juce::jmin(anchorIndex, endIndex);
    const int hi = juce::jmax(anchorIndex, endIndex);
    selectedKeys_.clear();
    selectedKeys_.reserve(static_cast<size_t>(hi - lo + 1));
    for (int i = lo; i <= hi; ++i)
        selectedKeys_.push_back(visible[static_cast<size_t>(i)]);

    // Clicked end stays the link pastille winner (vector back).
    // Keep selectionAnchorKey_ so successive Shift+clicks still originate there.
    if (selectedKeys_.back() != endKey)
    {
        selectedKeys_.erase(std::remove(selectedKeys_.begin(), selectedKeys_.end(), endKey),
                            selectedKeys_.end());
        selectedKeys_.push_back(endKey);
    }
}

void SkinEditorMappingsPanel::selectRowKey(const TSS::SkinColourKey& key, bool toggle, bool range)
{
    if (range)
    {
        selectRowKeyRange(key);
        return;
    }

    if (! toggle)
    {
        selectedKeys_.clear();
        selectedKeys_.push_back(key);
        selectionAnchorKey_ = key;
        return;
    }

    const auto it = std::find(selectedKeys_.begin(), selectedKeys_.end(), key);
    if (it != selectedKeys_.end())
        selectedKeys_.erase(it);
    else
        selectedKeys_.push_back(key);

    if (selectedKeys_.empty())
    {
        selectionAnchorKey_.reset();
        return;
    }
    // Keep a plain-click origin when still selected; otherwise seed so Shift-range
    // works after a Cmd/Ctrl-only multi-select (or after the origin was toggled off).
    if (! selectionAnchorKey_.has_value()
        || std::find(selectedKeys_.begin(), selectedKeys_.end(), *selectionAnchorKey_)
               == selectedKeys_.end())
        selectionAnchorKey_ = selectedKeys_.back();
}

SkinEditorMappingLink::HeaderState SkinEditorMappingsPanel::linkHeaderState() const
{
    return SkinEditorMappingLink::headerState(
        currentLinkGroups(),
        SkinEditorMappingLink::keysForObjectFilter(selectedKeys_, objectFilterType()));
}

bool SkinEditorMappingsPanel::commitLinkGroups(SkinEditorMappingLink::Groups groups)
{
    if (readOnly_ || layer_ == nullptr || skinId_.isEmpty())
        return false;
    const auto result = layer_->setMappingLinkGroups(skinId_, std::move(groups));
    if (result.failed())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Link groups",
                                               result.getErrorMessage());
        return false;
    }
    return true;
}

bool SkinEditorMappingsPanel::writePastilleToKey(const TSS::SkinColourKey& key,
                                                 const juce::String& pastilleName)
{
    const auto* skin = layer_->findSkin(skinId_);
    if (skin == nullptr)
        return false;
    const auto it = skin->mappings.find(key);
    if (it != skin->mappings.end() && it->second == pastilleName)
        return true;
    const bool unbound = (it == skin->mappings.end());
    const auto result = unbound ? layer_->bindUnbound(skinId_, key, pastilleName)
                                : layer_->setMapping(skinId_, key, pastilleName);
    if (! result.failed())
        return true;
    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                           "Link mapping",
                                           result.getErrorMessage());
    return false;
}

void SkinEditorMappingsPanel::alignGroupPastilles(const std::vector<TSS::SkinColourKey>& orderedKeys)
{
    if (readOnly_ || layer_ == nullptr || skinId_.isEmpty() || orderedKeys.empty())
        return;

    const auto* skin = layer_->findSkin(skinId_);
    if (skin == nullptr)
        return;

    int groupIndex = -1;
    for (const auto& key : orderedKeys)
    {
        groupIndex = SkinEditorMappingLink::findGroupIndex(skin->mappingLinkGroups, key);
        if (groupIndex >= 0)
            break;
    }
    if (groupIndex < 0)
        return;

    const auto& members = skin->mappingLinkGroups[static_cast<size_t>(groupIndex)];
    const auto winner = SkinEditorMappingLink::winnerPastilleForGroup(skin->mappings, members,
                                                                      orderedKeys);
    if (winner.isEmpty())
        return;

    for (const auto& key : members)
        if (! writePastilleToKey(key, winner))
            return;
}

void SkinEditorMappingsPanel::handleLinkChainClick(const TSS::SkinColourKey& key)
{
    if (readOnly_)
        return;

    auto groups = currentLinkGroups();
    const int groupIndex = SkinEditorMappingLink::findGroupIndex(groups, key);
    if (groupIndex >= 0 && isKeyGroupRevealed(key))
    {
        if (commitLinkGroups(SkinEditorMappingLink::removeMember(std::move(groups), key)))
            refresh();
        return;
    }

    if (selectedKeys_.empty())
        return;

    const auto filter = objectFilterType();
    const auto before = groups;
    SkinEditorMappingLink::Groups next;
    std::vector<TSS::SkinColourKey> ordered = selectedKeys_;
    if (std::find(ordered.begin(), ordered.end(), key) == ordered.end())
        ordered.push_back(key);

    if (selectedKeys_.size() >= 2)
        next = SkinEditorMappingLink::linkOrMerge(std::move(groups), ordered, filter);
    else
        next = SkinEditorMappingLink::addToContext(std::move(groups), selectedKeys_.back(), key,
                                                   filter);

    if (next == before)
        return;
    if (! commitLinkGroups(std::move(next)))
        return;
    alignGroupPastilles(ordered);
    if (! isKeySelected(key))
        selectedKeys_.push_back(key);
    refresh();
}

void SkinEditorMappingsPanel::handleLinkHeaderClick()
{
    if (readOnly_)
        return;

    const auto state = linkHeaderState();
    auto groups = currentLinkGroups();
    const auto filter = objectFilterType();

    if (state.action == SkinEditorMappingLink::HeaderAction::kUnlinkAll)
    {
        const auto revealed = SkinEditorMappingLink::revealedKeys(groups, selectedKeys_);
        std::vector<TSS::SkinColourKey> toUnlink;
        for (const auto& key : revealed)
        {
            if (filter.has_value() && key.object != *filter)
                continue;
            toUnlink.push_back(key);
        }
        if (commitLinkGroups(SkinEditorMappingLink::unlinkKeys(std::move(groups), toUnlink)))
            refresh();
        return;
    }

    if (state.action != SkinEditorMappingLink::HeaderAction::kCreateOrMerge)
        return;

    const auto before = groups;
    auto next = SkinEditorMappingLink::linkOrMerge(std::move(groups), selectedKeys_, filter);
    if (next == before)
        return;
    if (! commitLinkGroups(std::move(next)))
        return;
    alignGroupPastilles(selectedKeys_);
    refresh();
}

bool SkinEditorMappingsPanel::handleLinkLegendClick(juce::Point<int> pos)
{
    if (linkChainBounds_.contains(pos) || linkLabelBounds_.contains(pos)
        || linkBadgeBounds_.contains(pos))
    {
        handleLinkHeaderClick();
        return true;
    }
    return false;
}

void SkinEditorMappingsPanel::paintLinkCell(juce::Graphics& g,
                                            const Row& item,
                                            juce::Rectangle<int> cellArea) const
{
    if (selectedKeys_.empty())
        return;

    const auto groups = currentLinkGroups();
    const bool yellow = isKeyGroupRevealed(item.key)
                        && SkinEditorMappingLink::findGroupIndex(groups, item.key) >= 0;
    const bool greyHover = ! yellow
                           && hoveredLinkKey_.has_value()
                           && *hoveredLinkKey_ == item.key
                           && SkinEditorMappingLink::showsHoverChain(groups,
                                                                    selectedKeys_,
                                                                    item.key,
                                                                    objectFilterType());
    if (! yellow && ! greyHover)
        return;

    const auto kind = yellow ? SkinEditorChrome::LinkChainKind::kYellow
                             : SkinEditorChrome::LinkChainKind::kGrey;
    // Match sticky-legend chain X; content table origin is kFocusPad.
    constexpr int kRowChainNudgeX = 1;
    const int tableOriginX = SkinEditorChrome::kMappingsFocusPad;
    const int tableLocalChainX = (! linkChainBounds_.isEmpty() && ! stickyLegendBounds_.isEmpty())
                                     ? (linkChainBounds_.getX() - stickyLegendBounds_.getX())
                                     : (cellArea.getX() - tableOriginX
                                        + SkinEditorChrome::kMappingsColumnPad);
    const int chainX = tableOriginX + tableLocalChainX + kRowChainNudgeX;
    const auto chainArea = juce::Rectangle<int>(chainX,
                                                cellArea.getY(),
                                                SkinEditorChrome::linkChainBadgeWidth(),
                                                cellArea.getHeight());
    SkinEditorChrome::drawLinkChainBadge(g, chainArea, kind);
}

void SkinEditorMappingsPanel::paintLinkLegend(juce::Graphics& g) const
{
    const auto linkHeader = linkHeaderState();
    if (! linkChainBounds_.isEmpty())
    {
        const auto kind = (linkHeader.chrome == SkinEditorMappingLink::HeaderChrome::kYellow)
                              ? SkinEditorChrome::LinkChainKind::kYellow
                              : SkinEditorChrome::LinkChainKind::kGrey;
        SkinEditorChrome::drawLinkChainBadge(g, linkChainBounds_, kind);
    }
    if (linkBadgeBounds_.isEmpty())
        return;

    // Idle / empty selection: same grey as disabled Link chain (#323232).
    // Yellow = active link group; muted grey = free multi-select create/merge.
    juce::Colour badgeFill = SkinEditorChrome::surfaceTab();
    if (linkHeader.badgeCount > 0)
    {
        badgeFill = (linkHeader.chrome == SkinEditorMappingLink::HeaderChrome::kYellow)
                        ? SkinEditorChrome::attention()
                        : SkinEditorChrome::textMuted();
    }
    SkinEditorChrome::drawLinkCountBadge(g, linkBadgeBounds_, linkHeader.badgeCount, badgeFill);
}
