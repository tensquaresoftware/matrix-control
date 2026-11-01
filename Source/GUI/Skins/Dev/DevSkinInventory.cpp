#include "GUI/Skins/Dev/DevSkinInventory.h"

#include "GUI/Skins/Dev/DevSkinColourElementList.h"

namespace TSS::DevSkin
{
    std::vector<SkinColourKey> allStructuredKeys()
    {
        std::vector<SkinColourKey> keys;
        keys.reserve(compiledColourElements().size() * kAllColourSets.size());
        forEachCompiledColourElement([&](const SkinColours::ColourElement& element)
        {
            if (! supportsMultipleColourSets(element.key.object))
            {
                keys.push_back(element.key);
                return;
            }

            for (const auto colourSet : kAllColourSets)
            {
                auto key = element.key;
                key.colourSet = colourSet;
                keys.push_back(key);
            }
        });
        return keys;
    }
}
