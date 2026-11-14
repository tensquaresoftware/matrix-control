#pragma once

#include <set>

#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinTypes.h"

namespace TSS::DevSkin
{
    juce::var mappingToVar(const SkinColourKey& key, const juce::String& paletteName);
    /** Parses one row; colour sets beyond the object's inventory parse OK and callers drop them. */
    bool mappingFromVar(const juce::var& value,
                        SkinColourKey& key,
                        juce::String& paletteName,
                        juce::String& error);
    void expandLegacyColourSets(NamedSkin& skin);

    /** Designer-only colourSetLabels object, or void when all labels empty. */
    juce::var colourSetLabelsToVar(const NamedSkin& skin);
    /** Reads optional colourSetLabels; unknown / invalid keys ignored. */
    void colourSetLabelsFromVar(const juce::var& labelsVar, NamedSkin& out);

    /** SkinColourKey identity object (no paletteName) for mappingLinkGroups. */
    juce::var mappingKeyToVar(const SkinColourKey& key);
    /** Parses a mapping-key object; returns false on unknown fields. */
    bool mappingKeyFromVar(const juce::var& value, SkinColourKey& key);

    /** Designer-only mappingLinkGroups array, or void when empty. */
    juce::var mappingLinkGroupsToVar(const NamedSkin& skin);
    /**
     * Reads optional mappingLinkGroups; drops orphans / invalid / undersized groups.
     * validKeys = inventory; empty set accepts any parseable key (tests only).
     */
    void mappingLinkGroupsFromVar(const juce::var& groupsVar,
                                  NamedSkin& out,
                                  const std::set<SkinColourKey>& validKeys);
    /** Drop orphans / duplicates / undersized groups; one membership per key. */
    std::vector<std::vector<SkinColourKey>> sanitizeMappingLinkGroups(
        std::vector<std::vector<SkinColourKey>> groups,
        const std::set<SkinColourKey>& validKeys);

    juce::var zoneMapEntryToVar(const SkinZoneMapKey& key, ColourSet colourSet);
    /** Missing / void zoneMap => empty map (registry defaults). */
    bool zoneMapFromVar(const juce::var& zoneMapVar, SkinZoneMap& out, juce::String& error);
}
