#include "GUI/Skins/Dev/DevSkinJsonMapping.h"

#include "GUI/Skins/Dev/DevSkinKeyNames.h"

namespace
{
    bool isLegacyButtonLikeObjectName(const juce::String& objectText)
    {
        return objectText.equalsIgnoreCase("SliderButtonLike")
            || objectText.equalsIgnoreCase("ComboBoxButtonLike")
            || objectText.equalsIgnoreCase("PopupMenuButtonLike");
    }
}

namespace TSS::DevSkin
{
    juce::var mappingToVar(const SkinColourKey& key, const juce::String& paletteName)
    {
        auto* object = new juce::DynamicObject();
        object->setProperty("object", toString(key.object));
        object->setProperty("property", toString(key.property));
        object->setProperty("state", toString(key.state));
        object->setProperty("colourSet", toString(key.colourSet));
        object->setProperty("paletteName", paletteName);
        return juce::var(object);
    }

    bool mappingFromVar(const juce::var& value,
                        SkinColourKey& key,
                        juce::String& paletteName,
                        juce::String& error)
    {
        auto* objectValue = value.getDynamicObject();
        if (objectValue == nullptr)
        {
            error = "Mapping entry must be an object";
            return false;
        }

        SkinObjectType object {};
        SkinColourProperty property {};
        SkinColourState state {};
        ColourSet colourSet = ColourSet::k1;
        const auto objectText = objectValue->getProperty("object").toString();
        if (! parseObjectType(objectText, object)
            || ! parseProperty(objectValue->getProperty("property").toString(), property)
            || ! parseState(objectValue->getProperty("state").toString(), state))
        {
            error = "Mapping has unknown object/property/state";
            return false;
        }

        const auto setValue = objectValue->getProperty("colourSet");
        if (! setValue.isVoid() && ! parseColourSet(setValue.toString(), colourSet))
        {
            error = "Mapping has unknown colourSet";
            return false;
        }

        // Legacy *ButtonLike rows carried mono greys. Always force the object's last (mono)
        // colour set after object remap (whatever colourSet the row carried) so those greys
        // cannot overwrite the zone accents on lower sets.
        if (isLegacyButtonLikeObjectName(objectText))
            colourSet = static_cast<ColourSet>(colourSetCount(object) - 1);

        paletteName = objectValue->getProperty("paletteName").toString();
        if (paletteName.isEmpty())
        {
            error = "Mapping requires paletteName";
            return false;
        }

        key = skinColourKey(object, property, state, colourSet);
        return true;
    }

    void expandLegacyColourSets(NamedSkin& skin)
    {
        const auto legacyMappings = skin.mappings;
        for (const auto& [key, paletteName] : legacyMappings)
        {
            if (key.colourSet != ColourSet::k1 || ! supportsMultipleColourSets(key.object))
                continue;

            for (const auto colourSet : kAllColourSets)
            {
                if (! supportsColourSet(key.object, colourSet))
                    continue;
                auto expandedKey = key;
                expandedKey.colourSet = colourSet;
                skin.mappings.try_emplace(expandedKey, paletteName);
            }
        }
    }

    juce::var colourSetLabelsToVar(const NamedSkin& skin)
    {
        auto* labels = new juce::DynamicObject();
        bool any = false;
        for (const auto colourSet : kAllColourSets)
        {
            const auto& text = skin.colourSetLabels[static_cast<size_t>(colourSet)];
            if (text.isEmpty())
                continue;
            labels->setProperty(toString(colourSet), text);
            any = true;
        }
        if (! any)
        {
            delete labels;
            return {};
        }
        return juce::var(labels);
    }

    void colourSetLabelsFromVar(const juce::var& labelsVar, NamedSkin& out)
    {
        auto* labelsObj = labelsVar.getDynamicObject();
        if (labelsObj == nullptr)
            return;

        for (const auto colourSet : kAllColourSets)
        {
            const auto key = toString(colourSet);
            if (! labelsObj->hasProperty(key))
                continue;
            out.colourSetLabels[static_cast<size_t>(colourSet)] =
                sanitizeColourSetLabel(labelsObj->getProperty(key).toString());
        }
    }

    juce::var mappingKeyToVar(const SkinColourKey& key)
    {
        auto* object = new juce::DynamicObject();
        object->setProperty("object", toString(key.object));
        object->setProperty("property", toString(key.property));
        object->setProperty("state", toString(key.state));
        object->setProperty("colourSet", toString(key.colourSet));
        return juce::var(object);
    }

    bool mappingKeyFromVar(const juce::var& value, SkinColourKey& key)
    {
        auto* objectValue = value.getDynamicObject();
        if (objectValue == nullptr)
            return false;

        SkinObjectType object {};
        SkinColourProperty property {};
        SkinColourState state {};
        ColourSet colourSet = ColourSet::k1;
        if (! parseObjectType(objectValue->getProperty("object").toString(), object)
            || ! parseProperty(objectValue->getProperty("property").toString(), property)
            || ! parseState(objectValue->getProperty("state").toString(), state))
            return false;

        const auto setValue = objectValue->getProperty("colourSet");
        if (! setValue.isVoid() && ! parseColourSet(setValue.toString(), colourSet))
            return false;

        if (! supportsColourSet(object, colourSet))
            return false;

        key = skinColourKey(object, property, state, colourSet);
        return true;
    }

    juce::var mappingLinkGroupsToVar(const NamedSkin& skin)
    {
        if (skin.mappingLinkGroups.empty())
            return {};

        juce::Array<juce::var> groups;
        for (const auto& group : skin.mappingLinkGroups)
        {
            if (group.size() < 2)
                continue;
            juce::Array<juce::var> members;
            for (const auto& key : group)
                members.add(mappingKeyToVar(key));
            if (members.size() >= 2)
                groups.add(juce::var(members));
        }
        if (groups.isEmpty())
            return {};
        return juce::var(groups);
    }

    std::vector<std::vector<SkinColourKey>> sanitizeMappingLinkGroups(
        std::vector<std::vector<SkinColourKey>> groups,
        const std::set<SkinColourKey>& validKeys)
    {
        std::vector<std::vector<SkinColourKey>> cleaned;
        std::set<SkinColourKey> claimed;
        for (auto& group : groups)
        {
            std::vector<SkinColourKey> kept;
            for (const auto& key : group)
            {
                if (! validKeys.empty() && validKeys.find(key) == validKeys.end())
                    continue;
                if (! claimed.insert(key).second)
                    continue;
                kept.push_back(key);
            }
            if (kept.size() >= 2)
                cleaned.push_back(std::move(kept));
        }
        return cleaned;
    }

    void mappingLinkGroupsFromVar(const juce::var& groupsVar,
                                  NamedSkin& out,
                                  const std::set<SkinColourKey>& validKeys)
    {
        out.mappingLinkGroups.clear();
        auto* groupsArr = groupsVar.getArray();
        if (groupsArr == nullptr)
            return;

        std::vector<std::vector<SkinColourKey>> parsed;
        for (const auto& groupVar : *groupsArr)
        {
            auto* membersArr = groupVar.getArray();
            if (membersArr == nullptr)
                continue;

            std::vector<SkinColourKey> members;
            for (const auto& memberVar : *membersArr)
            {
                SkinColourKey key;
                if (! mappingKeyFromVar(memberVar, key))
                    continue;
                members.push_back(key);
            }
            parsed.push_back(std::move(members));
        }
        out.mappingLinkGroups = sanitizeMappingLinkGroups(std::move(parsed), validKeys);
    }
}
