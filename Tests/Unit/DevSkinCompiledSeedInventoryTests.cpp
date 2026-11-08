#include <juce_core/juce_core.h>

#include <map>

#include "GUI/Skins/Dev/DevSkinSsotSeed.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinValues.h"

class DevSkinCompiledSeedInventoryTests : public juce::UnitTest
{
public:
    DevSkinCompiledSeedInventoryTests()
        : juce::UnitTest("DevSkinCompiledSeedInventory")
    {
    }

    void runTest() override
    {
        beginTest("compiledSeed_colourSetsMatchFrozenInventory");
        assertCompiledSeedInventory(TSS::DevSkin::SkinKind::kCompiledBlack);
        assertCompiledSeedInventory(TSS::DevSkin::SkinKind::kCompiledBlackDuo);
        assertCompiledSeedInventory(TSS::DevSkin::SkinKind::kCompiledCream);

        beginTest("compiledSkin_everyMcsK1KeyHasOwnEntryAtHigherSets");
        assertHigherSetsMirrorK1Keys(TSS::Skin::ColourVariant::Black);
        assertHigherSetsMirrorK1Keys(TSS::Skin::ColourVariant::BlackDuo);
        assertHigherSetsMirrorK1Keys(TSS::Skin::ColourVariant::Cream);

        beginTest("compiledBlackSeed_monoMenusAndProgressBarGreys");
        assertCompiledBlackMonoGreys();
    }

private:
    void assertCompiledSeedInventory(TSS::DevSkin::SkinKind kind)
    {
        using TSS::SkinObjectType;

        const auto skin = TSS::DevSkin::makeCompiledSkin(kind);
        std::map<SkinObjectType, size_t> highestSetCount;
        for (const auto& [key, paletteName] : skin.mappings)
        {
            expect(TSS::supportsColourSet(key.object, key.colourSet));
            auto& count = highestSetCount[key.object];
            count = std::max(count, static_cast<size_t>(key.colourSet) + 1);
        }

        expectEquals(static_cast<int>(highestSetCount[SkinObjectType::kSlider]), 4);
        expectEquals(static_cast<int>(highestSetCount[SkinObjectType::kComboBox]), 3);
        expectEquals(static_cast<int>(highestSetCount[SkinObjectType::kPopupMenu]), 3);
        expectEquals(static_cast<int>(highestSetCount[SkinObjectType::kLed]), 2);
        expectEquals(static_cast<int>(highestSetCount[SkinObjectType::kPeakIndicator]), 2);
        for (const auto object : { SkinObjectType::kButton,
                                   SkinObjectType::kToggle,
                                   SkinObjectType::kNumberBox,
                                   SkinObjectType::kReadOnlyValueField,
                                   SkinObjectType::kProgressBar })
            expectEquals(static_cast<int>(highestSetCount[object]), 1);
    }

    // Membership probe: setColour on a mapped key keeps the count (probe skin values are discarded).
    static bool isMapped(TSS::Skin& probe, TSS::SkinColourKey key)
    {
        const auto before = probe.getMappedColourCount();
        probe.setColour(key, juce::Colours::transparentBlack);
        return probe.getMappedColourCount() == before;
    }

    void expectHigherSetsOwnEntry(TSS::Skin& probe, TSS::Skin& skin, TSS::SkinColourKey k1Key)
    {
        const auto sentinel = juce::Colour(0x01020304);
        skin.setColour(k1Key, sentinel);
        for (const auto colourSet : TSS::kAllColourSets)
        {
            if (colourSet == TSS::ColourSet::k1 || ! TSS::supportsColourSet(k1Key.object, colourSet))
                continue;
            auto higherKey = k1Key;
            higherKey.colourSet = colourSet;
            const bool mapped = isMapped(probe, higherKey);
            expect(mapped);
            if (mapped)
                expect(skin.getColour(higherKey) != sentinel);
        }
    }

    void assertHigherSetsMirrorK1Keys(TSS::Skin::ColourVariant variant)
    {
        constexpr int kPropertyCount = static_cast<int>(TSS::SkinColourProperty::kCaret) + 1;
        constexpr int kStateCount = static_cast<int>(TSS::SkinColourState::kError) + 1;

        const auto probe = TSS::Skin::create(variant);
        const auto skin = TSS::Skin::create(variant);
        for (const auto object : TSS::kMultiSetSkinObjects)
        {
            int mappedK1Keys = 0;
            for (int property = 0; property < kPropertyCount; ++property)
                for (int state = 0; state < kStateCount; ++state)
                {
                    const auto k1Key = TSS::skinColourKey(object,
                                                          static_cast<TSS::SkinColourProperty>(property),
                                                          static_cast<TSS::SkinColourState>(state));
                    if (! isMapped(*probe, k1Key))
                        continue;
                    ++mappedK1Keys;
                    expectHigherSetsOwnEntry(*probe, *skin, k1Key);
                }
            expect(mappedK1Keys > 0);
        }
    }

    static TSS::SkinColourKey keyAt(TSS::SkinObjectType object,
                                    TSS::SkinColourProperty property,
                                    TSS::ColourSet colourSet)
    {
        return TSS::skinColourKey(object, property, TSS::SkinColourState::kDefault, colourSet);
    }

    void assertCompiledBlackMonoGreys()
    {
        using TSS::ColourSet;
        using TSS::SkinColourProperty;
        using TSS::SkinObjectType;

        const auto skin = TSS::DevSkin::makeCompiledSkin(TSS::DevSkin::SkinKind::kCompiledBlackDuo);
        const auto mapping = [&skin](SkinObjectType object, SkinColourProperty property, ColourSet colourSet)
        {
            return skin.mappings.at(keyAt(object, property, colourSet));
        };

        expectEquals(mapping(SkinObjectType::kComboBox, SkinColourProperty::kBackground, ColourSet::k3),
                     juce::String("kBlack"));
        expectEquals(mapping(SkinObjectType::kPopupMenu, SkinColourProperty::kBorder, ColourSet::k3),
                     juce::String("kGrey600"));
        expectEquals(mapping(SkinObjectType::kSlider, SkinColourProperty::kTrack, ColourSet::k4),
                     juce::String("kBlack"));
        expectEquals(mapping(SkinObjectType::kComboBox, SkinColourProperty::kBackground, ColourSet::k1),
                     juce::String("kBlue950"));

        expectEquals(mapping(SkinObjectType::kProgressBar, SkinColourProperty::kTrack, ColourSet::k1),
                     juce::String("kBlack"));
        expectEquals(mapping(SkinObjectType::kProgressBar, SkinColourProperty::kValueBar, ColourSet::k1),
                     mapping(SkinObjectType::kSlider, SkinColourProperty::kValueBar, ColourSet::k4));
        expectEquals(mapping(SkinObjectType::kProgressBar, SkinColourProperty::kText, ColourSet::k1),
                     mapping(SkinObjectType::kSlider, SkinColourProperty::kText, ColourSet::k4));
    }
};

static DevSkinCompiledSeedInventoryTests devSkinCompiledSeedInventoryTests;
