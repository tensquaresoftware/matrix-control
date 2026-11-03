#pragma once

#include <algorithm>
#include <map>
#include <vector>

#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinInventory.h"
#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "GUI/Skins/Dev/DevSkinTypes.h"

/** Pure Mappings column-filter predicates (juce_core + DevSkin; no Component). */
namespace SkinEditorMappingsColumnFilter
{
    inline constexpr const char* kUnboundMenuValue = "Unbound";

    enum class Column
    {
        kProperty,
        kState,
        kColourSet,
        kMappedColour
    };

    /** Panel-wide sticky-legend sort (one active column). */
    enum class SortColumn
    {
        kNone,
        kProperty,
        kState,
        kColourSet,
        kMappedColour
    };

    /** Sticky-legend header click: left = sort, popup/right = filter menu. */
    enum class HeaderGesture
    {
        kSort,
        kOpenFilter
    };

    inline HeaderGesture headerGestureForClick(bool isPopupMenu) noexcept
    {
        return isPopupMenu ? HeaderGesture::kOpenFilter : HeaderGesture::kSort;
    }

    /**
     * Object combo macro-filter: empty name = "- All objects -" (every group visible).
     * Non-empty = show only that SkinObjectType display name.
     */
    inline bool objectMacroFilterShowsGroup(const juce::String& objectFilterName,
                                            TSS::SkinObjectType object) noexcept
    {
        return objectFilterName.isEmpty()
            || TSS::DevSkin::toString(object) == objectFilterName;
    }

    /** Object combo selectedId 1 (or less) = All objects; else the item text is the filter name. */
    inline juce::String objectFilterNameFromComboSelection(int selectedId,
                                                           const juce::String& itemText)
    {
        if (selectedId <= 1)
            return {};
        return itemText;
    }

    struct SortSpec
    {
        SortColumn column = SortColumn::kNone;
        bool ascending = true; // true = A->Z
    };

    /** Launch / skin-switch default: Property A->Z (yellow down picto). */
    inline SortSpec defaultMappingsSortSpec() noexcept
    {
        return { SortColumn::kProperty, true };
    }

    struct ActiveFilters
    {
        juce::String property;
        juce::String state;
        juce::String colourSet;
        juce::String mappedColour;
    };

    struct ObjectKeyGroup
    {
        TSS::SkinObjectType object {};
        std::vector<TSS::SkinColourKey> keys;
    };

    /**
     * Editor-only Mappings table order (descending GUI reading).
     * Independent of SkinObjectType enum ordinals and inventory push order.
     */
    inline const std::vector<TSS::SkinObjectType>& mappingsObjectDisplayOrder()
    {
        static const std::vector<TSS::SkinObjectType> order {
            TSS::SkinObjectType::kWindowSurface,
            TSS::SkinObjectType::kDialog,
            TSS::SkinObjectType::kChromeText,
            TSS::SkinObjectType::kHeaderPanel,
            TSS::SkinObjectType::kBodyPanel,
            TSS::SkinObjectType::kFooterPanel,
            TSS::SkinObjectType::kFooterMessage,
            TSS::SkinObjectType::kLed,
            TSS::SkinObjectType::kPeakIndicator,
            TSS::SkinObjectType::kEnvelopeDisplay,
            TSS::SkinObjectType::kTrackGeneratorDisplay,
            TSS::SkinObjectType::kPatchNameDisplay,
            TSS::SkinObjectType::kSectionHeader,
            TSS::SkinObjectType::kModuleHeader,
            TSS::SkinObjectType::kGroupLabel,
            TSS::SkinObjectType::kLabel,
            TSS::SkinObjectType::kSlider,
            TSS::SkinObjectType::kProgressBar,
            TSS::SkinObjectType::kComboBox,
            TSS::SkinObjectType::kPopupMenu,
            TSS::SkinObjectType::kNumberBox,
            TSS::SkinObjectType::kReadOnlyValueField,
            TSS::SkinObjectType::kButton,
            TSS::SkinObjectType::kToggle,
            TSS::SkinObjectType::kHorizontalSeparator,
        };
        return order;
    }

    inline bool keyMatches(const TSS::DevSkin::NamedSkin& skin,
                           const TSS::SkinColourKey& key,
                           const ActiveFilters& filters)
    {
        if (filters.property.isNotEmpty()
            && TSS::DevSkin::toString(key.property) != filters.property)
            return false;
        if (filters.state.isNotEmpty()
            && TSS::DevSkin::toString(key.state) != filters.state)
            return false;
        if (filters.colourSet.isNotEmpty()
            && TSS::DevSkin::toString(key.colourSet) != filters.colourSet)
            return false;
        if (filters.mappedColour.isEmpty())
            return true;

        const auto it = skin.mappings.find(key);
        const bool unbound = (it == skin.mappings.end() || it->second.isEmpty());
        if (filters.mappedColour == kUnboundMenuValue)
            return unbound;
        return ! unbound && it->second == filters.mappedColour;
    }

    /**
     * Inventory keys grouped by object under ActiveFilters.
     * Group order follows mappingsObjectDisplayOrder(); unknown inventory objects
     * append in first-seen inventory order. Omits empty sections.
     */
    inline std::vector<ObjectKeyGroup> filteredObjectGroups(const TSS::DevSkin::NamedSkin& skin,
                                                            const ActiveFilters& filters)
    {
        std::map<TSS::SkinObjectType, std::vector<TSS::SkinColourKey>> keysByObject;
        for (const auto& key : TSS::DevSkin::allStructuredKeys())
            keysByObject[key.object].push_back(key);

        std::vector<ObjectKeyGroup> groups;

        const auto appendObject = [&](TSS::SkinObjectType object)
        {
            const auto it = keysByObject.find(object);
            if (it == keysByObject.end())
                return;

            ObjectKeyGroup group;
            group.object = object;
            for (const auto& key : it->second)
            {
                if (keyMatches(skin, key, filters))
                    group.keys.push_back(key);
            }
            keysByObject.erase(it);
            if (! group.keys.empty())
                groups.push_back(std::move(group));
        };

        for (const auto object : mappingsObjectDisplayOrder())
            appendObject(object);

        for (const auto& key : TSS::DevSkin::allStructuredKeys())
        {
            if (keysByObject.find(key.object) == keysByObject.end())
                continue;
            appendObject(key.object);
        }
        return groups;
    }

    /**
     * Colour set filter PopupMenu label: "k1" or "k1: Patch" when a shared help label exists.
     * Filter value / match key stays the bare token (toString(ColourSet)).
     */
    inline juce::String colourSetFilterMenuLabel(const TSS::DevSkin::NamedSkin& skin,
                                                 const juce::String& colourSetToken)
    {
        for (const auto colourSet : TSS::kAllColourSets)
        {
            if (TSS::DevSkin::toString(colourSet) != colourSetToken)
                continue;
            const auto& help = skin.colourSetLabels[static_cast<size_t>(colourSet)];
            if (help.isEmpty())
                return colourSetToken;
            return colourSetToken + ": " + help;
        }
        return colourSetToken;
    }

    /** Values present in the Editor skin inventory rows for one column. */
    inline juce::StringArray presentValues(const TSS::DevSkin::NamedSkin& skin, Column column)
    {
        juce::StringArray values;
        bool hasUnbound = false;
        for (const auto& key : TSS::DevSkin::allStructuredKeys())
        {
            if (column == Column::kProperty)
            {
                values.addIfNotAlreadyThere(TSS::DevSkin::toString(key.property));
                continue;
            }
            if (column == Column::kState)
            {
                values.addIfNotAlreadyThere(TSS::DevSkin::toString(key.state));
                continue;
            }
            if (column == Column::kColourSet)
            {
                values.addIfNotAlreadyThere(TSS::DevSkin::toString(key.colourSet));
                continue;
            }

            const auto it = skin.mappings.find(key);
            if (it == skin.mappings.end() || it->second.isEmpty())
                hasUnbound = true;
            else
                values.addIfNotAlreadyThere(it->second);
        }

        values.sort(true);
        if (column == Column::kMappedColour && hasUnbound
            && ! values.contains(kUnboundMenuValue))
            values.insert(0, kUnboundMenuValue);
        return values;
    }

    inline juce::String sortDisplayValue(const TSS::DevSkin::NamedSkin& skin,
                                         const TSS::SkinColourKey& key,
                                         SortColumn column)
    {
        if (column == SortColumn::kProperty)
            return TSS::DevSkin::toString(key.property);
        if (column == SortColumn::kState)
            return TSS::DevSkin::toString(key.state);
        if (column == SortColumn::kColourSet)
            return TSS::DevSkin::toString(key.colourSet);
        if (column != SortColumn::kMappedColour)
            return {};

        const auto it = skin.mappings.find(key);
        if (it == skin.mappings.end() || it->second.isEmpty())
            return kUnboundMenuValue;
        return it->second;
    }

    /** Stable sort of already-filtered keys by SortSpec (no-op when column is kNone). */
    inline void sortFilteredKeys(const TSS::DevSkin::NamedSkin& skin,
                                 std::vector<TSS::SkinColourKey>& keys,
                                 SortSpec sort)
    {
        if (sort.column == SortColumn::kNone || keys.size() < 2)
            return;

        std::stable_sort(keys.begin(), keys.end(),
                         [&](const TSS::SkinColourKey& a, const TSS::SkinColourKey& b)
                         {
                             const auto left = sortDisplayValue(skin, a, sort.column);
                             const auto right = sortDisplayValue(skin, b, sort.column);
                             const int cmp = left.compareIgnoreCase(right);
                             return sort.ascending ? (cmp < 0) : (cmp > 0);
                         });
    }
}
