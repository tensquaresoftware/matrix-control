#pragma once

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
}
