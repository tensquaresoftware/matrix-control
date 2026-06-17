#include <cstring>

#include <juce_core/juce_core.h>

#include "Core/Init/InitDefaults.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"

class InitDefaultsTests : public juce::UnitTest
{
public:
    InitDefaultsTests() : juce::UnitTest("InitDefaults Tests") {}

    void runTest() override
    {
        runBufferSizes();
        runPatchFixtureIdentity();
        runMasterFixtureIdentity();
        runMatrixModCleared();
        runSysExEncodable();
    }

private:
    static juce::File fixturesInitDir()
    {
        return juce::File(MATRIX_TEST_FIXTURES_DIR).getChildFile("Init");
    }

    void runBufferSizes()
    {
        beginTest("Buffer sizes match SysExConstants");

        expectEquals(static_cast<int>(Core::InitDefaults::kPatchSize),
                     static_cast<int>(SysExConstants::kPatchPackedDataSize));
        expectEquals(static_cast<int>(Core::InitDefaults::kMasterSize),
                     static_cast<int>(SysExConstants::kMasterPackedDataSize));
    }

    void runPatchFixtureIdentity()
    {
        beginTest("Patch bytes match decoded PatchInit.syx");

        expect(! Core::InitDefaults::kPatchDefaultsArePlaceholder,
               "Patch defaults should be grounded in committed PatchInit.syx");

        SysExParser parser;
        SysExDecoder decoder(parser);

        juce::MemoryBlock original;
        const bool loaded = fixturesInitDir().getChildFile("PatchInit.syx").loadFileAsData(original);
        expect(loaded, "PatchInit.syx fixture should load from disk");
        expectEquals(static_cast<int>(original.getSize()),
                     static_cast<int>(SysExConstants::kPatchMessageLength));

        juce::uint8 decoded[SysExConstants::kPatchPackedDataSize] = {};
        expect(decoder.decodePatchSysEx(original, decoded), "Decoder should accept PatchInit.syx");

        expect(std::memcmp(Core::InitDefaults::patchData(), decoded, Core::InitDefaults::kPatchSize) == 0,
               "InitDefaults patch buffer must match decoded PatchInit.syx");
    }

    void runMasterFixtureIdentity()
    {
        beginTest("Master bytes match decoded MasterInit.syx");

        SysExParser parser;
        SysExDecoder decoder(parser);

        juce::MemoryBlock original;
        const bool loaded = fixturesInitDir().getChildFile("MasterInit.syx").loadFileAsData(original);
        expect(loaded, "MasterInit.syx fixture should load from disk");
        expectEquals(static_cast<int>(original.getSize()),
                     static_cast<int>(SysExConstants::kMasterMessageLength));

        juce::uint8 decoded[SysExConstants::kMasterPackedDataSize] = {};
        expect(decoder.decodeMasterSysEx(original, decoded), "Decoder should accept MasterInit.syx");

        expect(std::memcmp(Core::InitDefaults::masterData(), decoded, Core::InitDefaults::kMasterSize) == 0,
               "InitDefaults master buffer must match decoded MasterInit.syx");
    }

    void runMatrixModCleared()
    {
        beginTest("Matrix Mod region cleared on patch buffer");

        constexpr int kMatrixModStart = 104;
        constexpr int kBytesPerBus = 3;
        constexpr int kBusCount = Matrix1000Limits::kModulationBusCount;

        for (int bus = 0; bus < kBusCount; ++bus)
        {
            const int base = kMatrixModStart + bus * kBytesPerBus;
            expectEquals(static_cast<int>(Core::InitDefaults::patchData()[base]), 0,
                         "Matrix Mod source should be NONE (0)");
            expectEquals(static_cast<int>(Core::InitDefaults::patchData()[base + 1]), 0,
                         "Matrix Mod amount should be 0");
            expectEquals(static_cast<int>(Core::InitDefaults::patchData()[base + 2]), 0,
                         "Matrix Mod destination should be NONE (0)");
        }
    }

    void runSysExEncodable()
    {
        beginTest("Buffers re-encode through SysExEncoder");

        SysExEncoder encoder;

        const auto patchEncoded = encoder.encodePatchSysEx(0, Core::InitDefaults::patchData());
        expect(patchEncoded.getSize() == SysExConstants::kPatchMessageLength,
               "Patch init should encode to 275-byte SysEx");

        const auto masterEncoded = encoder.encodeMasterSysEx(0x03, Core::InitDefaults::masterData());
        expect(masterEncoded.getSize() == SysExConstants::kMasterMessageLength,
               "Master init should encode to 351-byte SysEx");
    }
};

static InitDefaultsTests initDefaultsTests;
