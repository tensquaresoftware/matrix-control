#include <juce_core/juce_core.h>

#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"

class EditorOutboundGateTests : public juce::UnitTest
{
public:
    EditorOutboundGateTests() : juce::UnitTest("EditorOutboundGate") {}

    void runTest() override
    {
        beginTest("isEditorOutboundAllowed — follows deviceDetected");
        expect(! Core::isEditorOutboundAllowed(false));
        expect(Core::isEditorOutboundAllowed(true));

        beginTest("maySendEditorProgramChange — blocked when undetected");
        expect(! Core::maySendEditorProgramChange(false));
        expect(Core::maySendEditorProgramChange(true));

        beginTest("maySendEditorSysEx — blocks editor SysEx when undetected");
        juce::MemoryBlock remoteEdit;
        {
            const juce::uint8 bytes[] = { 0xF0, 0x10, 0x06, 0x06, 0x00, 0x40, 0xF7 };
            remoteEdit.append(bytes, sizeof(bytes));
        }
        expect(! Core::maySendEditorSysEx(false, remoteEdit));
        expect(Core::maySendEditorSysEx(true, remoteEdit));

        beginTest("maySendEditorSysEx — Device Inquiry allowlisted while undetected");
        const auto inquiry = SysExEncoder::encodeDeviceInquiry();
        expect(Core::isDeviceInquirySysEx(inquiry));
        expect(Core::maySendEditorSysEx(false, inquiry));
        expect(Core::maySendEditorSysEx(true, inquiry));

        beginTest("isSectionLocked — composes device detection and Compare");
        expect(Core::isSectionLocked(false, false));
        expect(Core::isSectionLocked(false, true));
        expect(Core::isSectionLocked(true, true));
        expect(! Core::isSectionLocked(true, false));
    }
};

static EditorOutboundGateTests editorOutboundGateTests;
