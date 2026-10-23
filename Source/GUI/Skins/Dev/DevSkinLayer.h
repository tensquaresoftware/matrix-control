#pragma once

#include <functional>

#include "GUI/Skins/Dev/DevSkinTypes.h"

namespace TSS::DevSkin
{
    class Layer
    {
    public:
        Layer();

        void resetToCompiledBaseline();

        const std::vector<NamedSkin>& skins() const noexcept { return skins_; }
        const juce::String& activeSkinId() const noexcept { return activeSkinId_; }
        juce::int64 revision() const noexcept { return revision_; }

        const NamedSkin* findSkin(const juce::String& id) const;
        NamedSkin* findSkinMutable(const juce::String& id);

        bool selectSkin(const juce::String& id);
        bool isActiveReadOnly() const;

        /** Duplicate active (or named source) into a new free-text Dev skin. */
        juce::Result duplicateSkin(const juce::String& sourceId, const juce::String& newName);

        juce::Result createEmptyDevSkin(const juce::String& newName);
        juce::Result deleteDevSkin(const juce::String& id);
        juce::Result renameDevSkin(const juce::String& id, const juce::String& newName);

        juce::Result setPastilleArgb(const juce::String& skinId,
                                     const juce::String& pastilleName,
                                     juce::uint32 argb);
        juce::Result addPastille(const juce::String& skinId,
                                 const juce::String& pastilleName,
                                 juce::uint32 argb);
        /** Dev-only pastille names. Product ColourChart names cannot be deleted. */
        juce::Result deletePastille(const juce::String& skinId,
                                    const juce::String& pastilleName,
                                    bool removeReferencingMappings);

        juce::Result setMapping(const juce::String& skinId,
                                SkinColourKey key,
                                const juce::String& pastilleName);
        juce::Result bindUnbound(const juce::String& skinId,
                                 SkinColourKey key,
                                 const juce::String& pastilleName);

        std::vector<SkinColourKey> unboundKeys(const juce::String& skinId) const;
        int unboundCount(const juce::String& skinId) const;
        bool isEditorSkinExportAllowed(const juce::String& skinId) const;
        bool isProductPastilleName(const juce::String& pastilleName) const;
        int countMappingsReferencing(const juce::String& skinId,
                                     const juce::String& pastilleName) const;

        /** Append a Dev skin, reminting id/name on collision. Returns new id. */
        juce::Result appendDevSkin(NamedSkin skin, juce::String* outId = nullptr);

        PushPayload makePushPayload() const;
        PushPayload makePushPayloadForSkin(const juce::String& skinId) const;

        /** Walk derivedFrom until compiled Black/Cream (Dev-from-Dev safe). */
        juce::String resolveCompiledAncestorId(const NamedSkin& skin) const;

        void replaceAll(std::vector<NamedSkin> skins, juce::String activeId);

        std::function<void()> onChanged;

    private:
        void bump();
        juce::String makeUniqueDevId(const juce::String& name) const;
        bool isCompiledId(const juce::String& id) const;
        bool nameTaken(const juce::String& name, const juce::String& exceptId = {}) const;

        std::vector<NamedSkin> skins_;
        juce::String activeSkinId_;
        juce::int64 revision_ = 0;
    };
}
