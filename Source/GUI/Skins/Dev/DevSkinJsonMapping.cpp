#include "GUI/Skins/Dev/DevSkinJsonMapping.h"

#include "GUI/Skins/Dev/DevSkinKeyNames.h"

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
        if (! parseObjectType(objectValue->getProperty("object").toString(), object)
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
        if (colourSet != ColourSet::k1 && ! supportsMultipleColourSets(object))
        {
            error = "Mapping uses a higher colourSet for a k1-only object";
            return false;
        }

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
                auto expandedKey = key;
                expandedKey.colourSet = colourSet;
                skin.mappings.try_emplace(expandedKey, paletteName);
            }
        }
    }
}
