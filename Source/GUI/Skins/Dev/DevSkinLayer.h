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

        /** Duplicate source into a new free-text Dev skin (palette + optional mappings). */
        juce::Result duplicateSkin(const juce::String& sourceId,
                                   const juce::String& newName,
                                   bool copyMappings = true);

        /**
         * New Designer skin: Product palette / ancestry, mappings cleared (full Unbound).
         * Optionally appends non-product pastilles from another Designer skin (no name dupes).
         */
        juce::Result createDesignerSkinWithUnboundMappings(
            const juce::String& sourceId,
            const juce::String& newName,
            const juce::String& mergeCustomPastillesFromId = {});

        /** Same as createDesignerSkinWithUnboundMappings from compiled Cream. */
        juce::Result createEmptyDevSkin(const juce::String& newName)
        {
            return createDesignerSkinWithUnboundMappings(kCompiledCreamId, newName);
        }
        juce::Result deleteDevSkin(const juce::String& id);
        juce::Result renameDevSkin(const juce::String& id, const juce::String& newName);

        juce::Result setPastilleArgb(const juce::String& skinId,
                                     const juce::String& pastilleName,
                                     juce::uint32 argb);
        juce::Result addPastille(const juce::String& skinId,
                                 const juce::String& pastilleName,
                                 juce::uint32 argb,
                                 bool pending = false);
        /** Rename Designer pastille; rewrites mapping string refs. Compiled skins blocked. */
        juce::Result renamePastille(const juce::String& skinId,
                                    const juce::String& pastilleName,
                                    const juce::String& newName);
        /** Delete pastille on Dev skins; mapped rows need removeReferencingMappings. */
        juce::Result deletePastille(const juce::String& skinId,
                                    const juce::String& pastilleName,
                                    bool removeReferencingMappings);

        juce::Result setMapping(const juce::String& skinId,
                                SkinColourKey key,
                                const juce::String& pastilleName);
        juce::Result bindUnbound(const juce::String& skinId,
                                 SkinColourKey key,
                                 const juce::String& pastilleName);

        /**
         * Designer-only Colour set help label for one index (cs1..cs4).
         * Empty clears the label. Does not affect runtime ColourSet identity.
         */
        juce::Result setColourSetLabel(const juce::String& skinId,
                                       ColourSet colourSet,
                                       const juce::String& label);

        /**
         * Replace designer-only mapping link groups (sanitized: orphans dropped, size >= 2).
         * Does not affect runtime / PushPayload.
         */
        juce::Result setMappingLinkGroups(const juce::String& skinId,
                                          std::vector<std::vector<SkinColourKey>> groups);

        /**
         * Replace per-skin MCS zone map (used cells only; validates occupancy + colourSetCount).
         * Empty map restores registry-default geography for that skin on push.
         */
        juce::Result setZoneMap(const juce::String& skinId, SkinZoneMap map);

        /**
         * Apply pastilleName to every peer in writerKey's link group (skips writer).
         * No-op when writer is ungrouped. Hold / mode policy stay with the caller.
         */
        juce::Result propagateMappingToLinkPeers(const juce::String& skinId,
                                                 SkinColourKey writerKey,
                                                 const juce::String& pastilleName);

        std::vector<SkinColourKey> unboundKeys(const juce::String& skinId) const;
        int unboundCount(const juce::String& skinId) const;
        bool isDesignerSkinExportAllowed(const juce::String& skinId) const;
        /** Always false — Dev palettes are author-owned (API kept for callers). */
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
        /** Append non-product pastilles from source when dest has no same name (ignore case). */
        void appendCustomPastillesFrom(NamedSkin& dest, const NamedSkin& source) const;

        std::vector<NamedSkin> skins_;
        juce::String activeSkinId_;
        juce::int64 revision_ = 0;
    };
}
