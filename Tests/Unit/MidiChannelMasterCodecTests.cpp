#include <array>

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/Models/MidiChannelMasterCodec.h"

class MidiChannelMasterCodecTests : public juce::UnitTest
{
public:
    MidiChannelMasterCodecTests() : juce::UnitTest("MidiChannelMasterCodec Tests") {}

    void runTest() override
    {
        runEncodeCases();
        runDecodeCases();
        runMonoChannelAboveNineDisplayClamp();
    }

private:
    using MasterBuffer = std::array<juce::uint8, SysExConstants::kMasterPackedDataSize>;

    static juce::uint8 basic(const MasterBuffer& buf)
    {
        return buf[static_cast<size_t>(Core::MidiChannelMasterCodec::kBasicChannelOffset)];
    }

    static juce::uint8 omni(const MasterBuffer& buf)
    {
        return buf[static_cast<size_t>(Core::MidiChannelMasterCodec::kOmniOffset)];
    }

    static juce::uint8 mono(const MasterBuffer& buf)
    {
        return buf[static_cast<size_t>(Core::MidiChannelMasterCodec::kMonoOffset)];
    }

    void expectWire(const MasterBuffer& buf, juce::uint8 expectedBasic,
                    juce::uint8 expectedOmni, juce::uint8 expectedMono)
    {
        expectEquals(static_cast<int>(basic(buf)), static_cast<int>(expectedBasic));
        expectEquals(static_cast<int>(omni(buf)), static_cast<int>(expectedOmni));
        expectEquals(static_cast<int>(mono(buf)), static_cast<int>(expectedMono));
    }

    void runEncodeCases()
    {
        beginTest("Encode OMNI / CHANNEL 1 / CHANNEL 16 / MONO G1 / MONO G9");

        MasterBuffer buf {};

        Core::MidiChannelMasterCodec::applyComboIndex(
            buf.data(), buf.size(), Core::MidiChannelMasterCodec::kOmniComboIndex);
        expectWire(buf, 0, 1, 0);

        Core::MidiChannelMasterCodec::applyComboIndex(buf.data(), buf.size(), 1); // CHANNEL 1
        expectWire(buf, 0, 0, 0);

        Core::MidiChannelMasterCodec::applyComboIndex(buf.data(), buf.size(), 16); // CHANNEL 16
        expectWire(buf, 15, 0, 0);

        Core::MidiChannelMasterCodec::applyComboIndex(
            buf.data(), buf.size(), Core::MidiChannelMasterCodec::kFirstMonoIndex); // MONO G1
        expectWire(buf, 0, 0, 1);

        Core::MidiChannelMasterCodec::applyComboIndex(
            buf.data(), buf.size(), Core::MidiChannelMasterCodec::kLastMonoIndex); // MONO G9
        expectWire(buf, 8, 0, 1);

        // Acceptance: CHANNEL 4 → byte11=3, Omni=0, Mono=0
        Core::MidiChannelMasterCodec::applyComboIndex(buf.data(), buf.size(), 4);
        expectWire(buf, 3, 0, 0);

        // Acceptance: MONO G3 → byte11=2, Omni=0, Mono=1
        Core::MidiChannelMasterCodec::applyComboIndex(buf.data(), buf.size(), 19);
        expectWire(buf, 2, 0, 1);
    }

    void runDecodeCases()
    {
        beginTest("Decode buffer patterns to combo indices");

        MasterBuffer buf {};

        buf[11] = 7;
        buf[12] = 1;
        buf[35] = 0;
        expectEquals(Core::MidiChannelMasterCodec::readComboIndex(buf.data(), buf.size()), 0,
                     "Omni=1 → OMNI (ignore stale channel)");

        buf[11] = 0;
        buf[12] = 0;
        buf[35] = 0;
        expectEquals(Core::MidiChannelMasterCodec::readComboIndex(buf.data(), buf.size()), 1,
                     "Poly channel 1");

        buf[11] = 15;
        expectEquals(Core::MidiChannelMasterCodec::readComboIndex(buf.data(), buf.size()), 16,
                     "Poly channel 16");

        buf[11] = 0;
        buf[35] = 1;
        expectEquals(Core::MidiChannelMasterCodec::readComboIndex(buf.data(), buf.size()), 17,
                     "MONO G1");

        buf[11] = 8;
        expectEquals(Core::MidiChannelMasterCodec::readComboIndex(buf.data(), buf.size()), 25,
                     "MONO G9");

        buf[11] = 2;
        buf[12] = 1;
        buf[35] = 1;
        expectEquals(Core::MidiChannelMasterCodec::readComboIndex(buf.data(), buf.size()), 19,
                     "Mono=1 and Omni=1 → MONO G3 (Mono wins)");
    }

    void runMonoChannelAboveNineDisplayClamp()
    {
        beginTest("Mono with basic channel > 9 clamps display to MONO G9");

        MasterBuffer buf {};
        buf[11] = 15; // channel 16 on wire
        buf[12] = 0;
        buf[35] = 1;

        expectEquals(Core::MidiChannelMasterCodec::readComboIndex(buf.data(), buf.size()), 25,
                     "Odd Mono channel>9 → MONO G9");
    }
};

static MidiChannelMasterCodecTests midiChannelMasterCodecTests;
