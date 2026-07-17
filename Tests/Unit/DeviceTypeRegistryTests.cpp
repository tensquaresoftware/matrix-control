#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

class DeviceTypeRegistryTests : public juce::UnitTest
{
public:
    DeviceTypeRegistryTests() : juce::UnitTest("DeviceTypeRegistry") {}

    void runTest() override
    {
        testD080MemberConstants();
        testFromMemberBytes();
        testFromDeviceInquiryAcceptReject();
        testDecodeDeviceIdGoldenBytes();
    }

private:
    static DeviceIdInfo makeInquiryInfo(juce::uint8 manufacturer,
                                        juce::uint8 familyLow,
                                        juce::uint8 familyHigh,
                                        juce::uint8 memberLow,
                                        juce::uint8 memberHigh,
                                        bool isValid = true)
    {
        DeviceIdInfo info {};
        info.manufacturerId = manufacturer;
        info.familyLow = familyLow;
        info.familyHigh = familyHigh;
        info.memberLow = memberLow;
        info.memberHigh = memberHigh;
        info.version = "1.11";
        info.isValid = isValid;
        return info;
    }

    void testD080MemberConstants()
    {
        beginTest("D-080 — Matrix-1000 member constants match Oberheim 02H/00H");

        // Named Oberheim Device ID literals (memb-lo=2H, memb-hi=0H) — lock against swap regression.
        constexpr juce::uint8 kOberheimMatrix1000MemberLow = 0x02;
        constexpr juce::uint8 kOberheimMatrix1000MemberHigh = 0x00;

        expectEquals(static_cast<int>(SysExConstants::DeviceInquiry::kExpectedMemberLow),
                     static_cast<int>(kOberheimMatrix1000MemberLow));
        expectEquals(static_cast<int>(SysExConstants::DeviceInquiry::kExpectedMemberHigh),
                     static_cast<int>(kOberheimMatrix1000MemberHigh));
        expectEquals(static_cast<int>(SysExConstants::DeviceInquiry::kExpectedFamilyHigh),
                     0x00);
    }

    void testFromMemberBytes()
    {
        beginTest("fromMemberBytes — M-1000, provisional M-6, unknown");

        expectEquals(
            static_cast<int>(Core::DeviceTypeRegistry::fromMemberBytes(
                SysExConstants::DeviceInquiry::kExpectedMemberLow,
                SysExConstants::DeviceInquiry::kExpectedMemberHigh)),
            static_cast<int>(MatrixDeviceTypes::Type::kMatrix1000));

        expectEquals(
            static_cast<int>(Core::DeviceTypeRegistry::fromMemberBytes(
                SysExConstants::DeviceInquiry::kMatrix6MemberLow,
                SysExConstants::DeviceInquiry::kMatrix6MemberHigh)),
            static_cast<int>(MatrixDeviceTypes::Type::kMatrix6));

        expectEquals(
            static_cast<int>(Core::DeviceTypeRegistry::fromMemberBytes(0xFF, 0xFF)),
            static_cast<int>(MatrixDeviceTypes::Type::kUnknown));

        // Swapped pre-D-080 pattern must not map to Matrix-1000.
        expectEquals(
            static_cast<int>(Core::DeviceTypeRegistry::fromMemberBytes(0x00, 0x02)),
            static_cast<int>(MatrixDeviceTypes::Type::kUnknown));

        // Inquiry must not invent Matrix-6R from member bytes.
        expect(Core::DeviceTypeRegistry::fromMemberBytes(
                   SysExConstants::DeviceInquiry::kMatrix6MemberLow,
                   SysExConstants::DeviceInquiry::kMatrix6MemberHigh)
               != MatrixDeviceTypes::Type::kMatrix6R);
    }

    void testFromDeviceInquiryAcceptReject()
    {
        beginTest("fromDeviceInquiry — accept M-1000 / provisional M-6; reject wrong manuf/family");

        const auto m1000 = makeInquiryInfo(
            SysExConstants::DeviceInquiry::kExpectedManufacturer,
            SysExConstants::DeviceInquiry::kExpectedFamily,
            SysExConstants::DeviceInquiry::kExpectedFamilyHigh,
            SysExConstants::DeviceInquiry::kExpectedMemberLow,
            SysExConstants::DeviceInquiry::kExpectedMemberHigh);
        expectEquals(static_cast<int>(Core::DeviceTypeRegistry::fromDeviceInquiry(m1000)),
                     static_cast<int>(MatrixDeviceTypes::Type::kMatrix1000));

        const auto m6 = makeInquiryInfo(
            SysExConstants::DeviceInquiry::kExpectedManufacturer,
            SysExConstants::DeviceInquiry::kExpectedFamily,
            SysExConstants::DeviceInquiry::kExpectedFamilyHigh,
            SysExConstants::DeviceInquiry::kMatrix6MemberLow,
            SysExConstants::DeviceInquiry::kMatrix6MemberHigh);
        expectEquals(static_cast<int>(Core::DeviceTypeRegistry::fromDeviceInquiry(m6)),
                     static_cast<int>(MatrixDeviceTypes::Type::kMatrix6));

        auto wrongManuf = m1000;
        wrongManuf.manufacturerId = 0x41;
        expectEquals(static_cast<int>(Core::DeviceTypeRegistry::fromDeviceInquiry(wrongManuf)),
                     static_cast<int>(MatrixDeviceTypes::Type::kUnknown));

        auto wrongFamily = m1000;
        wrongFamily.familyLow = 0x07;
        expectEquals(static_cast<int>(Core::DeviceTypeRegistry::fromDeviceInquiry(wrongFamily)),
                     static_cast<int>(MatrixDeviceTypes::Type::kUnknown));

        auto wrongFamilyHigh = m1000;
        wrongFamilyHigh.familyHigh = 0x01;
        expectEquals(static_cast<int>(Core::DeviceTypeRegistry::fromDeviceInquiry(wrongFamilyHigh)),
                     static_cast<int>(MatrixDeviceTypes::Type::kUnknown));

        auto invalidFlag = m1000;
        invalidFlag.isValid = false;
        expectEquals(static_cast<int>(Core::DeviceTypeRegistry::fromDeviceInquiry(invalidFlag)),
                     static_cast<int>(MatrixDeviceTypes::Type::kUnknown));
    }

    void testDecodeDeviceIdGoldenBytes()
    {
        beginTest("SysExDecoder::decodeDeviceId — M-1000 and provisional M-6 golden replies");

        SysExParser parser;
        SysExDecoder decoder(parser);

        // F0 7E <chan> 06 02 10 06 00 02 00 '1' '.' '1' '1' F7
        const juce::uint8 m1000Reply[] = {
            0xF0, 0x7E, 0x00, 0x06, 0x02,
            0x10, 0x06, 0x00, 0x02, 0x00,
            '1', '.', '1', '1',
            0xF7
        };
        const auto m1000Info = decoder.decodeDeviceId(
            juce::MemoryBlock(m1000Reply, sizeof(m1000Reply)));
        expect(m1000Info.isValid);
        expectEquals(static_cast<int>(m1000Info.memberLow), 0x02);
        expectEquals(static_cast<int>(m1000Info.memberHigh), 0x00);
        expectEquals(static_cast<int>(Core::DeviceTypeRegistry::fromDeviceInquiry(m1000Info)),
                     static_cast<int>(MatrixDeviceTypes::Type::kMatrix1000));

        const juce::uint8 m6Reply[] = {
            0xF0, 0x7E, 0x00, 0x06, 0x02,
            0x10, 0x06, 0x00, 0x01, 0x00,
            '1', '.', '1', '1',
            0xF7
        };
        const auto m6Info = decoder.decodeDeviceId(
            juce::MemoryBlock(m6Reply, sizeof(m6Reply)));
        expect(m6Info.isValid);
        expectEquals(static_cast<int>(m6Info.memberLow), 0x01);
        expectEquals(static_cast<int>(m6Info.memberHigh), 0x00);
        expectEquals(static_cast<int>(Core::DeviceTypeRegistry::fromDeviceInquiry(m6Info)),
                     static_cast<int>(MatrixDeviceTypes::Type::kMatrix6));

        const juce::uint8 badFamilyHigh[] = {
            0xF0, 0x7E, 0x00, 0x06, 0x02,
            0x10, 0x06, 0x01, 0x02, 0x00,
            '1', '.', '1', '1',
            0xF7
        };
        const auto rejected = decoder.decodeDeviceId(
            juce::MemoryBlock(badFamilyHigh, sizeof(badFamilyHigh)));
        expect(!rejected.isValid);
    }
};

static DeviceTypeRegistryTests deviceTypeRegistryTests;
