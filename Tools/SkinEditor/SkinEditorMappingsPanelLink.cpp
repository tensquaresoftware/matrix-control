#include "Tools/SkinEditor/SkinEditorMappingsPanel.h"

#include <algorithm>

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinEditor/SkinEditorChrome.h"
#include "Tools/SkinEditor/SkinEditorMappingsColumns.h"

void SkinEditorMappingsPanel::clearRowSelection()
{
    selectedKeys_.clear();
    hoveredLinkKey_.reset();
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

void SkinEditorMappingsPanel::selectRowKey(const TSS::SkinColourKey& key, bool toggle)
{
    if (! toggle)
    {
        selectedKeys_.clear();
        selectedKeys_.push_back(key);
        return;
    }

    const auto it = std::find(selectedKeys_.begin(), selectedKeys_.end(), key);
    if (it != selectedKeys_.end())
    {
        selectedKeys_.erase(it);
        return;
    }
    selectedKeys_.push_back(key);
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

    const auto winner = SkinEditorMappingLink::winnerPastille(skin->mappings, orderedKeys);
    if (winner.isEmpty())
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

    const auto members = skin->mappingLinkGroups[static_cast<size_t>(groupIndex)];
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
    const auto& context = selectedKeys_.back();
    const auto before = groups;
    auto next = SkinEditorMappingLink::addToContext(std::move(groups), context, key, filter);
    if (next == before)
        return;

    std::vector<TSS::SkinColourKey> ordered = selectedKeys_;
    ordered.push_back(key);
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
    SkinEditorChrome::drawLinkChainBadge(g, cellArea, kind);
}

void SkinEditorMappingsPanel::paintLinkLegend(juce::Graphics& g) const
{
    const auto linkHeader = linkHeaderState();
    const int linkChainW = SkinEditorChrome::linkChainBadgeWidth();
    const int linkTitleW = juce::jmax(0, linkLabelBounds_.getWidth()
                                             - SkinEditorChrome::kButtonGap - linkChainW);
    g.setColour(SkinEditorChrome::textTableHeader());
    g.drawText("Link", linkLabelBounds_.getX(), linkLabelBounds_.getY(), linkTitleW,
               linkLabelBounds_.getHeight(), juce::Justification::centredLeft, false);
    if (! linkChainBounds_.isEmpty())
    {
        const auto kind = (linkHeader.chrome == SkinEditorMappingLink::HeaderChrome::kYellow)
                              ? SkinEditorChrome::LinkChainKind::kYellow
                              : SkinEditorChrome::LinkChainKind::kGrey;
        SkinEditorChrome::drawLinkChainBadge(g, linkChainBounds_, kind);
    }
    if (linkHeader.badgeCount > 0 && ! linkBadgeBounds_.isEmpty())
        SkinEditorChrome::drawCountBadge(g, linkBadgeBounds_, linkHeader.badgeCount);
}
