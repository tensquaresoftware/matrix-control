#include <juce_core/juce_core.h>

#include "Core/MIDI/MidiDevicePresenceMatch.h"

class MidiDevicePresenceMatchTests : public juce::UnitTest
{
public:
    MidiDevicePresenceMatchTests() : juce::UnitTest("MidiDevicePresenceMatch") {}

    void runTest() override
    {
        testEmptyAndExactEndpoint();
        testSingleTokenDeviceUniqueId();
        testMultiTokenMatchesEndpointOrDevice();
    }

private:
    void testEmptyAndExactEndpoint()
    {
        beginTest("identifierMatchesLiveIds - empty id or empty live set");

        Core::LiveMidiIdSets ids;
        expect(! Core::identifierMatchesLiveIds({}, ids));
        expect(! Core::identifierMatchesLiveIds("123", ids));

        ids.endpointIds.add("123");
        expect(! Core::identifierMatchesLiveIds({}, ids));
        expect(Core::identifierMatchesLiveIds("123", ids));
    }

    void testSingleTokenDeviceUniqueId()
    {
        beginTest("identifierMatchesLiveIds - single token may match device UniqueID");

        Core::LiveMidiIdSets ids;
        ids.endpointIds.add("ep-1");
        ids.deviceIdsWithLiveEndpoint.add("dev-9");

        expect(Core::identifierMatchesLiveIds("dev-9", ids));
        expect(! Core::identifierMatchesLiveIds("missing", ids));
    }

    void testMultiTokenMatchesEndpointOrDevice()
    {
        beginTest("identifierMatchesLiveIds - multi-token matches endpoint or device part");

        Core::LiveMidiIdSets ids;
        ids.endpointIds.add("ep-1");
        ids.deviceIdsWithLiveEndpoint.add("dev-9");

        expect(Core::identifierMatchesLiveIds("ep-1, other", ids));
        expect(Core::identifierMatchesLiveIds("noise,dev-9", ids));
        expect(! Core::identifierMatchesLiveIds("a, b", ids));
    }
};

static MidiDevicePresenceMatchTests midiDevicePresenceMatchTests;
