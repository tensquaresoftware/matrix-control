#include <vector>

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>

#include "GUI/Helpers/AudioFromComboItemSet.h"
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
        testIdentifiersMatchMidiDevices();
        testSortMidiDevicesByName();
        testMidiPortItemSetUnchangedLabelsAndCount();
        testAudioFromItemSetUnchanged();
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

    void testIdentifiersMatchMidiDevices()
    {
        beginTest("identifiersMatchMidiDevices - order-sensitive equality");

        using namespace TSS::MidiPortComboPopulation;

        juce::Array<juce::MidiDeviceInfo> devices;
        devices.add({ "A", "dev-a" });
        devices.add({ "B", "dev-b" });

        const std::vector<juce::String> matching { "dev-a", "dev-b" };
        const std::vector<juce::String> reordered { "dev-b", "dev-a" };
        const std::vector<juce::String> shorter { "dev-a" };

        expect(identifiersMatchMidiDevices(matching, devices));
        expect(! identifiersMatchMidiDevices(reordered, devices));
        expect(! identifiersMatchMidiDevices(shorter, devices));
        expect(identifiersMatchMidiDevices({}, {}));
    }

    void testSortMidiDevicesByName()
    {
        beginTest("sortMidiDevicesByName - case-insensitive alpha; sentinel stays outside sort");

        using namespace TSS::MidiPortComboPopulation;

        juce::Array<juce::MidiDeviceInfo> devices;
        devices.add({ "Zebra", "id-z" });
        devices.add({ "oxygen 61", "id-o" });
        devices.add({ "AMT8", "id-a" });
        sortMidiDevicesByName(devices);

        expectEquals(devices.size(), 3);
        expectEquals(devices[0].name, juce::String("AMT8"));
        expectEquals(devices[1].name, juce::String("oxygen 61"));
        expectEquals(devices[2].name, juce::String("Zebra"));
        expectEquals(kPortSentinelItemId, 1);
        expect(kFirstDeviceItemId > kPortSentinelItemId);
    }

    void testMidiPortItemSetUnchangedLabelsAndCount()
    {
        beginTest("midiPortItemSetUnchanged - matching ids+labels+count");

        using namespace TSS::MidiPortComboPopulation;

        juce::Array<juce::MidiDeviceInfo> devices;
        devices.add({ "Oxygen", "id-o" });
        devices.add({ "MT4", "id-m" });
        const std::vector<juce::String> ids { "id-o", "id-m" };
        const std::vector<juce::String> texts { "NO INPUT", "OXYGEN", "MT4" };

        expect(midiPortItemSetUnchanged(ids, devices, 3, texts));

        beginTest("midiPortItemSetUnchanged - same ids but stale label forces rebuild");
        auto stale = texts;
        stale[1] = "OLD NAME";
        expect(! midiPortItemSetUnchanged(ids, devices, 3, stale));

        beginTest("midiPortItemSetUnchanged - count desync forces rebuild");
        expect(! midiPortItemSetUnchanged(ids, devices, 2, texts));
    }

    void testAudioFromItemSetUnchanged()
    {
        beginTest("AudioFromComboItemSet - matching ids+labels");

        const std::vector<juce::String> ids { "ch-1", "ch-2" };
        juce::StringArray names;
        names.add("Input 1");
        names.add("Input 2");
        const std::vector<juce::String> texts { "NO INPUT", "INPUT 1", "INPUT 2" };
        const TSS::AudioFromComboItemSet::ComboTexts combo { 3, texts };

        expect(TSS::AudioFromComboItemSet::itemSetUnchanged(ids, ids, names, combo));

        beginTest("AudioFromComboItemSet - extra channelNames beyond nextIds does not overrun");
        names.add("Input 3");
        expect(TSS::AudioFromComboItemSet::itemSetUnchanged(ids, ids, names, combo));

        beginTest("AudioFromComboItemSet - shorter channelNames than nextIds is changed");
        juce::StringArray shortNames;
        shortNames.add("Input 1");
        expect(! TSS::AudioFromComboItemSet::itemSetUnchanged(ids, ids, shortNames, combo));
    }
};

static MidiPortComboPopulationTests midiPortComboPopulationTests;
