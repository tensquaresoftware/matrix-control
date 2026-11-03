#pragma once

#include <algorithm>
#include <map>
#include <optional>
#include <set>
#include <vector>

#include <juce_core/juce_core.h>

#include "GUI/Skins/SkinValues.h"

/** Pure Link-group rules for Skin Editor Mappings (no Component deps). */
namespace SkinEditorMappingLink
{
    using Group = std::vector<TSS::SkinColourKey>;
    using Groups = std::vector<Group>;

    enum class HeaderChrome
    {
        kInactiveGrey,
        kActiveGrey,
        kYellow
    };

    enum class HeaderAction
    {
        kNone,
        kUnlinkAll,
        kCreateOrMerge
    };

    struct HeaderState
    {
        HeaderChrome chrome = HeaderChrome::kInactiveGrey;
        HeaderAction action = HeaderAction::kNone;
        int badgeCount = 0;
    };

    struct SelectionBreakdown
    {
        std::set<int> groupIndexes;
        int freeCount = 0;
    };

    inline int findGroupIndex(const Groups& groups, const TSS::SkinColourKey& key) noexcept
    {
        for (int i = 0; i < static_cast<int>(groups.size()); ++i)
            for (const auto& member : groups[static_cast<size_t>(i)])
                if (member == key)
                    return i;
        return -1;
    }

    inline bool groupContainsForeignObject(const Group& group,
                                           TSS::SkinObjectType object) noexcept
    {
        for (const auto& key : group)
            if (key.object != object)
                return true;
        return false;
    }

    /** When Object macro-filter is active, reject mutate ops that span other objects. */
    inline bool groupEditableUnderObjectFilter(
        const Group& group,
        std::optional<TSS::SkinObjectType> objectFilter) noexcept
    {
        if (! objectFilter.has_value())
            return true;
        return ! groupContainsForeignObject(group, *objectFilter);
    }

    inline SelectionBreakdown breakDownSelection(
        const Groups& groups,
        const std::vector<TSS::SkinColourKey>& selectedOrdered)
    {
        SelectionBreakdown out;
        for (const auto& key : selectedOrdered)
        {
            const int index = findGroupIndex(groups, key);
            if (index < 0)
                ++out.freeCount;
            else
                out.groupIndexes.insert(index);
        }
        return out;
    }

    inline Groups removeMember(Groups groups, const TSS::SkinColourKey& key)
    {
        const int index = findGroupIndex(groups, key);
        if (index < 0)
            return groups;

        auto& group = groups[static_cast<size_t>(index)];
        group.erase(std::remove(group.begin(), group.end(), key), group.end());
        if (group.size() < 2)
            groups.erase(groups.begin() + index);
        return groups;
    }

    /** Drop every listed key from its group; dissolve undersized leftovers. */
    inline Groups unlinkKeys(Groups groups, const std::vector<TSS::SkinColourKey>& keys)
    {
        for (const auto& key : keys)
            groups = removeMember(std::move(groups), key);
        return groups;
    }

    inline std::vector<TSS::SkinColourKey> keysForObjectFilter(
        const std::vector<TSS::SkinColourKey>& selectedOrdered,
        std::optional<TSS::SkinObjectType> objectFilter)
    {
        std::vector<TSS::SkinColourKey> keys;
        keys.reserve(selectedOrdered.size());
        for (const auto& key : selectedOrdered)
        {
            if (objectFilter.has_value() && key.object != *objectFilter)
                continue;
            keys.push_back(key);
        }
        return keys;
    }

    inline bool collectEditableTouchedIndexes(
        const Groups& groups,
        const std::vector<TSS::SkinColourKey>& keys,
        std::optional<TSS::SkinObjectType> objectFilter,
        std::set<int>& touched)
    {
        for (const auto& key : keys)
        {
            const int index = findGroupIndex(groups, key);
            if (index < 0)
                continue;
            if (! groupEditableUnderObjectFilter(groups[static_cast<size_t>(index)], objectFilter))
                return false;
            touched.insert(index);
        }
        return true;
    }

    inline Group buildMergedGroup(const Groups& groups,
                                  const std::set<int>& touched,
                                  const std::vector<TSS::SkinColourKey>& keys)
    {
        Group merged;
        std::set<TSS::SkinColourKey> seen;
        auto take = [&](const TSS::SkinColourKey& key)
        {
            if (seen.insert(key).second)
                merged.push_back(key);
        };
        for (int index : touched)
            for (const auto& key : groups[static_cast<size_t>(index)])
                take(key);
        for (const auto& key : keys)
            take(key);
        return merged;
    }

    /**
     * Create one group from free keys, or merge every group touched by selection.
     * Keys outside objectFilter (when set) are ignored. Groups that already contain
     * a foreign object while filtered cannot be extended or merged.
     */
    inline Groups linkOrMerge(Groups groups,
                              const std::vector<TSS::SkinColourKey>& selectedOrdered,
                              std::optional<TSS::SkinObjectType> objectFilter)
    {
        const auto keys = keysForObjectFilter(selectedOrdered, objectFilter);
        if (keys.size() < 2)
            return groups;

        std::set<int> touched;
        if (! collectEditableTouchedIndexes(groups, keys, objectFilter, touched))
            return groups;

        auto merged = buildMergedGroup(groups, touched, keys);
        if (merged.size() < 2)
            return groups;

        Groups next;
        for (int i = 0; i < static_cast<int>(groups.size()); ++i)
            if (touched.find(i) == touched.end())
                next.push_back(groups[static_cast<size_t>(i)]);
        next.push_back(std::move(merged));
        return next;
    }

    /**
     * Add free candidate into the group of contextKey (or create a pair).
     * Returns unchanged groups when Object≠All blocks the edit.
     */
    inline Groups addToContext(Groups groups,
                               const TSS::SkinColourKey& contextKey,
                               const TSS::SkinColourKey& candidate,
                               std::optional<TSS::SkinObjectType> objectFilter)
    {
        if (contextKey == candidate)
            return groups;
        if (objectFilter.has_value()
            && (contextKey.object != *objectFilter || candidate.object != *objectFilter))
            return groups;

        const int contextIndex = findGroupIndex(groups, contextKey);
        if (contextIndex >= 0
            && ! groupEditableUnderObjectFilter(groups[static_cast<size_t>(contextIndex)],
                                                objectFilter))
            return groups;

        const int candidateIndex = findGroupIndex(groups, candidate);
        if (candidateIndex >= 0
            && ! groupEditableUnderObjectFilter(groups[static_cast<size_t>(candidateIndex)],
                                                objectFilter))
            return groups;

        return linkOrMerge(std::move(groups), { contextKey, candidate }, objectFilter);
    }

    /** Mapped pastille wins over Unbound; among mapped conflicts, last key in order wins. */
    inline juce::String winnerPastille(
        const std::map<TSS::SkinColourKey, juce::String>& mappings,
        const std::vector<TSS::SkinColourKey>& orderedKeys)
    {
        juce::String winner;
        for (const auto& key : orderedKeys)
        {
            const auto it = mappings.find(key);
            if (it == mappings.end() || it->second.isEmpty())
                continue;
            winner = it->second;
        }
        return winner;
    }

    inline std::set<TSS::SkinColourKey> revealedKeys(
        const Groups& groups,
        const std::vector<TSS::SkinColourKey>& selectedOrdered)
    {
        std::set<TSS::SkinColourKey> revealed;
        for (const auto& key : selectedOrdered)
        {
            const int index = findGroupIndex(groups, key);
            if (index < 0)
            {
                revealed.insert(key);
                continue;
            }
            for (const auto& member : groups[static_cast<size_t>(index)])
                revealed.insert(member);
        }
        return revealed;
    }

    inline int badgeCountForSelection(const Groups& groups,
                                      const std::vector<TSS::SkinColourKey>& selectedOrdered)
    {
        if (selectedOrdered.empty())
            return 0;

        const auto parts = breakDownSelection(groups, selectedOrdered);
        if (parts.groupIndexes.size() == 1 && parts.freeCount == 0)
            return static_cast<int>(groups[static_cast<size_t>(*parts.groupIndexes.begin())].size());
        if (parts.groupIndexes.empty())
            return parts.freeCount;
        return static_cast<int>(selectedOrdered.size());
    }

    inline HeaderState headerState(const Groups& groups,
                                   const std::vector<TSS::SkinColourKey>& selectedOrdered)
    {
        HeaderState state;
        state.badgeCount = badgeCountForSelection(groups, selectedOrdered);
        if (selectedOrdered.empty())
            return state;

        const auto parts = breakDownSelection(groups, selectedOrdered);
        const bool singleFree = selectedOrdered.size() == 1 && parts.freeCount == 1;
        if (singleFree)
            return state;

        const bool oneGroupOnly = parts.groupIndexes.size() == 1 && parts.freeCount == 0;
        if (oneGroupOnly || selectedOrdered.size() >= 2)
        {
            state.chrome = oneGroupOnly ? HeaderChrome::kYellow : HeaderChrome::kActiveGrey;
            state.action = oneGroupOnly ? HeaderAction::kUnlinkAll : HeaderAction::kCreateOrMerge;
            return state;
        }

        return state;
    }

    inline bool selectionCanLinkCandidate(
        const Groups& groups,
        const std::vector<TSS::SkinColourKey>& selectedOrdered,
        std::optional<TSS::SkinObjectType> objectFilter) noexcept
    {
        for (const auto& key : selectedOrdered)
        {
            if (objectFilter.has_value() && key.object != *objectFilter)
                continue;
            const int index = findGroupIndex(groups, key);
            if (index >= 0
                && ! groupEditableUnderObjectFilter(groups[static_cast<size_t>(index)],
                                                    objectFilter))
                continue;
            return true;
        }
        return false;
    }

    /** Grey hover chain when selection provides a linkable context and candidate is free or other. */
    inline bool showsHoverChain(const Groups& groups,
                                const std::vector<TSS::SkinColourKey>& selectedOrdered,
                                const TSS::SkinColourKey& candidate,
                                std::optional<TSS::SkinObjectType> objectFilter) noexcept
    {
        if (selectedOrdered.empty())
            return false;
        if (objectFilter.has_value() && candidate.object != *objectFilter)
            return false;
        if (std::find(selectedOrdered.begin(), selectedOrdered.end(), candidate)
            != selectedOrdered.end())
            return false;

        if (findGroupIndex(groups, candidate) >= 0)
        {
            const auto revealed = revealedKeys(groups, selectedOrdered);
            if (revealed.find(candidate) != revealed.end())
                return false;
        }

        return selectionCanLinkCandidate(groups, selectedOrdered, objectFilter);
    }
}
