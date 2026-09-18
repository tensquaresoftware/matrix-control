#include <vector>

#include <juce_core/juce_core.h>

#include "GUI/Helpers/MidiPortComboPopulation.h"

class MidiPortComboPopulationTests : public juce::UnitTest
{
public:
    MidiPortComboPopulationTests() : juce::UnitTest("MidiPortComboPopulation") {}

    void runTest() override
    {
        testFindItemIdEmptyAndMissing();
        testFindItemIdMatchesDevice();
        testGetPortIdentifierSentinelAndBounds();
    }

private:
    void testFindItemIdEmptyAndMissing()
    {
        beginTest("findItemIdForPortIdentifier - empty or unknown -> sentinel");

        using namespace TSS::MidiPortComboPopulation;
        const std::vector<juce::String> ids { "dev-a", "dev-b" };

        expectEquals(findItemIdForPortIdentifier(ids, {}), kPortSentinelItemId);
        expectEquals(findItemIdForPortIdentifier(ids, "missing"), kPortSentinelItemId);
    }

    void testFindItemIdMatchesDevice()
    {
        beginTest("findItemIdForPortIdentifier - device index maps to item id");

        using namespace TSS::MidiPortComboPopulation;
        const std::vector<juce::String> ids { "dev-a", "dev-b" };

        expectEquals(findItemIdForPortIdentifier(ids, "dev-a"), kFirstDeviceItemId);
        expectEquals(findItemIdForPortIdentifier(ids, "dev-b"), kFirstDeviceItemId + 1);
    }

    void testGetPortIdentifierSentinelAndBounds()
    {
        beginTest("getPortIdentifierForItemId - sentinel/out-of-range empty; device id returns");

        using namespace TSS::MidiPortComboPopulation;
        const std::vector<juce::String> ids { "dev-a", "dev-b" };

        expect(getPortIdentifierForItemId(ids, kPortSentinelItemId).isEmpty());
        expect(getPortIdentifierForItemId(ids, kFirstDeviceItemId + 99).isEmpty());
        expectEquals(getPortIdentifierForItemId(ids, kFirstDeviceItemId), juce::String("dev-a"));
        expectEquals(getPortIdentifierForItemId(ids, kFirstDeviceItemId + 1), juce::String("dev-b"));
    }
};

static MidiPortComboPopulationTests midiPortComboPopulationTests;
