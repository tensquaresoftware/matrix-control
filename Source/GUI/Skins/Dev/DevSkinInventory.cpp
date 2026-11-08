#include "GUI/Skins/Dev/DevSkinInventory.h"

#include "GUI/Skins/Dev/DevSkinColourElementList.h"

namespace TSS::DevSkin
{
    std::vector<SkinColourKey> allStructuredKeys()
    {
        std::vector<SkinColourKey> keys;
        keys.reserve(compiledColourElements().size());
        forEachCompiledColourElement([&](const SkinColours::ColourElement& element)
        {
            keys.push_back(element.key);
        });
        return keys;
    }
}
