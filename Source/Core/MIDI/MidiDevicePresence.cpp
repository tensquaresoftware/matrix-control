#include "Core/MIDI/MidiDevicePresence.h"
#include "Core/MIDI/MidiDevicePresenceMatch.h"

#if JUCE_MAC || JUCE_IOS

#include <CoreMIDI/CoreMIDI.h>

namespace Core
{
    namespace
    {
        bool midiObjectReportsOffline(MIDIObjectRef object) noexcept
        {
            if (object == 0)
                return false;

            SInt32 offline = 0;
            if (MIDIObjectGetIntegerProperty(object, kMIDIPropertyOffline, &offline) != noErr)
                return false;

            return offline != 0;
        }

        bool endpointTreeIsOffline(MIDIEndpointRef endpoint) noexcept
        {
            if (endpoint == 0)
                return true;

            if (midiObjectReportsOffline(endpoint))
                return true;

            MIDIEntityRef entity = 0;
            if (MIDIEndpointGetEntity(endpoint, &entity) == noErr && entity != 0)
            {
                if (midiObjectReportsOffline(entity))
                    return true;

                MIDIDeviceRef device = 0;
                if (MIDIEntityGetDevice(entity, &device) == noErr && device != 0
                    && midiObjectReportsOffline(device))
                {
                    return true;
                }
            }

            return false;
        }

        void appendUniqueIdToken(juce::StringArray& tokens, MIDIObjectRef object)
        {
            if (object == 0)
                return;

            SInt32 objectId = 0;
            if (MIDIObjectGetIntegerProperty(object, kMIDIPropertyUniqueID, &objectId) == noErr)
                tokens.addIfNotAlreadyThere(juce::String(objectId));
        }

        void appendLiveDeviceIdForEndpoint(juce::StringArray& deviceIds, MIDIEndpointRef endpoint)
        {
            MIDIEntityRef entity = 0;
            if (MIDIEndpointGetEntity(endpoint, &entity) != noErr || entity == 0)
                return;

            MIDIDeviceRef device = 0;
            if (MIDIEntityGetDevice(entity, &device) == noErr && device != 0)
                appendUniqueIdToken(deviceIds, device);
        }

        LiveMidiIdSets collectLiveMidiIdSets(bool isInput)
        {
            LiveMidiIdSets ids;
            const ItemCount count = isInput ? MIDIGetNumberOfSources()
                                            : MIDIGetNumberOfDestinations();

            for (ItemCount i = 0; i < count; ++i)
            {
                const MIDIEndpointRef endpoint = isInput ? MIDIGetSource(i)
                                                         : MIDIGetDestination(i);
                if (endpoint == 0 || endpointTreeIsOffline(endpoint))
                    continue;

                appendUniqueIdToken(ids.endpointIds, endpoint);
                appendLiveDeviceIdForEndpoint(ids.deviceIdsWithLiveEndpoint, endpoint);
            }

            return ids;
        }

        bool isMidiIdentifierLive(const juce::String& identifier, bool isInput)
        {
            return identifierMatchesLiveIds(identifier, collectLiveMidiIdSets(isInput));
        }
    }

    bool isMidiInputIdentifierLive(const juce::String& identifier)
    {
        return isMidiIdentifierLive(identifier, true);
    }

    bool isMidiOutputIdentifierLive(const juce::String& identifier)
    {
        return isMidiIdentifierLive(identifier, false);
    }

    juce::Array<juce::MidiDeviceInfo> filterLiveMidiInputs(
        const juce::Array<juce::MidiDeviceInfo>& devices)
    {
        const auto liveIds = collectLiveMidiIdSets(true);
        juce::Array<juce::MidiDeviceInfo> live;

        for (const auto& device : devices)
        {
            if (identifierMatchesLiveIds(device.identifier, liveIds))
                live.add(device);
        }

        return live;
    }

    juce::Array<juce::MidiDeviceInfo> filterLiveMidiOutputs(
        const juce::Array<juce::MidiDeviceInfo>& devices)
    {
        const auto liveIds = collectLiveMidiIdSets(false);
        juce::Array<juce::MidiDeviceInfo> live;

        for (const auto& device : devices)
        {
            if (identifierMatchesLiveIds(device.identifier, liveIds))
                live.add(device);
        }

        return live;
    }
}

#else

namespace Core
{
    // Non-Apple: no reliable offline bit without exclusive openDevice probes. Fail-open —
    // keep OS-enumerated devices and treat non-empty ids as live so we never steal or hide
    // a port the session already holds.
    bool isMidiInputIdentifierLive(const juce::String& identifier)
    {
        return identifier.isNotEmpty();
    }

    bool isMidiOutputIdentifierLive(const juce::String& identifier)
    {
        return identifier.isNotEmpty();
    }

    juce::Array<juce::MidiDeviceInfo> filterLiveMidiInputs(
        const juce::Array<juce::MidiDeviceInfo>& devices)
    {
        return devices;
    }

    juce::Array<juce::MidiDeviceInfo> filterLiveMidiOutputs(
        const juce::Array<juce::MidiDeviceInfo>& devices)
    {
        return devices;
    }
}

#endif
