#include <juce_core/juce_core.h>

#include "Core/MIDI/Matrix6MasterResponseFilter.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/Transport/MidiReceiver.h"

namespace
{
juce::MemoryBlock makeFrame(size_t length, juce::uint8 opcode)
{
    juce::MemoryBlock block(length, true);
    auto* data = static_cast<juce::uint8*>(block.getData());
    data[0] = SysExConstants::kSysExStart;
    data[1] = SysExConstants::kManufacturerIdOberheim;
    data[2] = SysExConstants::kDeviceIdMatrix1000;
    data[3] = opcode;
    data[length - 1] = SysExConstants::kSysExEnd;
    return block;
}
} // namespace

class Matrix6MasterResponseFilterTests : public juce::UnitTest
{
public:
    Matrix6MasterResponseFilterTests() : juce::UnitTest("Matrix6MasterResponseFilter") {}

    void runTest() override
    {
        testAccepts478And479();
        testRejects351And477AndWrongOpcode();
        testRejectsWrongHeaderAndMissingEox();
        testFilterKeepsArmedUntilValidMaster();
    }

private:
    void testAccepts478And479()
    {
        beginTest("isMatrix6MasterParameterDump — accepts wire lengths 478 and 479");

        expect(Core::isMatrix6MasterParameterDump(
            makeFrame(SysExConstants::kMatrix6MasterMessageLengthMin,
                      SysExConstants::Opcode::kMasterParameterData)));
        expect(Core::isMatrix6MasterParameterDump(
            makeFrame(SysExConstants::kMatrix6MasterMessageLengthMax,
                      SysExConstants::Opcode::kMasterParameterData)));
    }

    void testRejects351And477AndWrongOpcode()
    {
        beginTest("isMatrix6MasterParameterDump — rejects M-1000 351, printed 477, wrong opcode");

        expect(! Core::isMatrix6MasterParameterDump(
            makeFrame(SysExConstants::kMasterMessageLength,
                      SysExConstants::Opcode::kMasterParameterData)));
        expect(! Core::isMatrix6MasterParameterDump(
            makeFrame(477, SysExConstants::Opcode::kMasterParameterData)));
        expect(! Core::isMatrix6MasterParameterDump(
            makeFrame(SysExConstants::kMatrix6MasterMessageLengthMax,
                      SysExConstants::Opcode::kRequestData)));
        expect(! Core::isMatrix6MasterParameterDump(
            makeFrame(SysExConstants::kMatrix6MasterMessageLengthMax,
                      0x0B)));
    }

    void testRejectsWrongHeaderAndMissingEox()
    {
        beginTest("isMatrix6MasterParameterDump — rejects wrong manuf/device id and missing F0/F7");

        auto badManuf = makeFrame(SysExConstants::kMatrix6MasterMessageLengthMax,
                                  SysExConstants::Opcode::kMasterParameterData);
        static_cast<juce::uint8*>(badManuf.getData())[1] = 0x41;
        expect(! Core::isMatrix6MasterParameterDump(badManuf));

        auto badDevice = makeFrame(SysExConstants::kMatrix6MasterMessageLengthMax,
                                   SysExConstants::Opcode::kMasterParameterData);
        static_cast<juce::uint8*>(badDevice.getData())[2] = 0x02;
        expect(! Core::isMatrix6MasterParameterDump(badDevice));

        auto missingStart = makeFrame(SysExConstants::kMatrix6MasterMessageLengthMax,
                                      SysExConstants::Opcode::kMasterParameterData);
        static_cast<juce::uint8*>(missingStart.getData())[0] = 0x00;
        expect(! Core::isMatrix6MasterParameterDump(missingStart));

        auto missingEox = makeFrame(SysExConstants::kMatrix6MasterMessageLengthMax,
                                    SysExConstants::Opcode::kMasterParameterData);
        static_cast<juce::uint8*>(missingEox.getData())[missingEox.getSize() - 1] = 0x00;
        expect(! Core::isMatrix6MasterParameterDump(missingEox));
    }

    void testFilterKeepsArmedUntilValidMaster()
    {
        beginTest("armOneShotSysExCapture — ignores M-1000 Master then accepts Matrix-6 length");

        MidiReceiver receiver;
        int deliveries = 0;
        juce::MemoryBlock last;

        receiver.armOneShotSysExCapture(
            [&](const juce::MemoryBlock& sysex)
            {
                ++deliveries;
                last = sysex;
            },
            [](const juce::MemoryBlock& sysex)
            {
                return Core::isMatrix6MasterParameterDump(sysex);
            });

        const auto m1000 = makeFrame(SysExConstants::kMasterMessageLength,
                                     SysExConstants::Opcode::kMasterParameterData);
        receiver.feedCompleteSysEx(m1000);
        expectEquals(deliveries, 0);

        const auto matrix6 = makeFrame(SysExConstants::kMatrix6MasterMessageLengthMax,
                                       SysExConstants::Opcode::kMasterParameterData);
        receiver.feedCompleteSysEx(matrix6);
        expectEquals(deliveries, 1);
        expect(last == matrix6);
    }
};

static Matrix6MasterResponseFilterTests matrix6MasterResponseFilterTests;
