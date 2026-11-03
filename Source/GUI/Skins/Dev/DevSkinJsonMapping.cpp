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

        // Legacy *ButtonLike rows carried mono greys. Always force ColourSet k4 after object
        // remap — including when colourSet is missing/defaulted to k1 or explicitly non-k4 —
        // so those greys cannot overwrite classic greens on k1–k3.
        if (isLegacyButtonLikeObjectName(objectText))
            colourSet = ColourSet::k4;

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
            auto text = labelsObj->getProperty(key).toString().trim();
            if (text.length() > kColourSetLabelMaxChars)
                text = text.substring(0, kColourSetLabelMaxChars);
            const auto& allowed = colourSetLabelAllowedChars();
            juce::String asciiOnly;
            for (int i = 0; i < text.length(); ++i)
            {
                const auto ch = text[i];
                if (allowed.containsChar(ch))
                    asciiOnly += ch;
            }
            out.colourSetLabels[static_cast<size_t>(colourSet)] = asciiOnly;
        }
    }
}
