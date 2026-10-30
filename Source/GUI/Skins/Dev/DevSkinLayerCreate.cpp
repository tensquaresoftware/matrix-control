#include "GUI/Skins/Dev/DevSkinLayer.h"

namespace TSS::DevSkin
{
    juce::Result Layer::createEditorSkinWithUnboundMappings(
        const juce::String& sourceId,
        const juce::String& newName,
        const juce::String& mergeCustomPastillesFromId)
    {
        const auto created = duplicateSkin(sourceId, newName, false);
        if (created.failed())
            return created;

        if (mergeCustomPastillesFromId.isEmpty()
            || mergeCustomPastillesFromId == activeSkinId_)
            return juce::Result::ok();

        const auto* donor = findSkin(mergeCustomPastillesFromId);
        auto* dest = findSkinMutable(activeSkinId_);
        if (donor == nullptr || dest == nullptr)
            return juce::Result::ok();

        appendCustomPastillesFrom(*dest, *donor);
        bump();
        return juce::Result::ok();
    }

    void Layer::appendCustomPastillesFrom(NamedSkin& dest, const NamedSkin& source) const
    {
        for (const auto& pastille : source.palette)
        {
            if (isProductPastilleName(pastille.name))
                continue;

            bool exists = false;
            for (const auto& existing : dest.palette)
            {
                if (existing.name.equalsIgnoreCase(pastille.name))
                {
                    exists = true;
                    break;
                }
            }
            if (! exists)
                dest.palette.push_back(pastille);
        }
    }
}
