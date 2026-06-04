#include <juce_core/juce_core.h>

#include "Core/MIDI/Queue/SysExDelayProfile.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"

class SysExDelayProfileTests : public juce::UnitTest
{
public:
    SysExDelayProfileTests() : juce::UnitTest("SysExDelayProfile") {}

    void runTest() override
    {
        testStockDefaultMatrix1000();
        testStockMatrix1000FromMemberBytes();
        testStockMatrix6FromMemberBytes();
        testOptimisedFirmwareStrings();
        testUnknownVersionUsesStock();
        testUnknownMemberBytesFallbackStockM1000();
        testOptimisedDelayConstants();
        testTaunVersionWithoutTekTokenStaysStock();
        testOptimisedShorterThanStockSameFamily();
    }

private:
    static DeviceIdInfo makeDeviceInfo(juce::uint8 memberLow,
                                       juce::uint8 memberHigh,
                                       const juce::String& version)
    {
        DeviceIdInfo info {};
        info.memberLow = memberLow;
        info.memberHigh = memberHigh;
        info.version = version;
        info.isValid = true;
        return info;
    }

    void testStockDefaultMatrix1000()
    {
        beginTest("stockDefault — stock M-1000, 10 ms");

        const auto profile = Core::SysExDelayProfile::stockDefault();
        expectEquals(profile.getDelayMs(), Core::SysExDelayProfile::kStockDelayMsMatrix1000);
        expectEquals(static_cast<int>(profile.getEpromClass()),
                     static_cast<int>(Core::EpromClass::kStock));
        expectEquals(static_cast<int>(profile.getDeviceFamily()),
                     static_cast<int>(Core::MatrixDeviceFamily::kMatrix1000));
    }

    void testStockMatrix1000FromMemberBytes()
    {
        beginTest("fromDeviceInquiry — explicit M-1000 member bytes 0x02/0x00");

        const auto info = makeDeviceInfo(0x02, 0x00, "1.11");
        const auto profile = Core::SysExDelayProfile::fromDeviceInquiry(info);
        expectEquals(profile.getDelayMs(), 10);
        expectEquals(static_cast<int>(profile.getDeviceFamily()),
                     static_cast<int>(Core::MatrixDeviceFamily::kMatrix1000));
    }

    void testStockMatrix6FromMemberBytes()
    {
        beginTest("fromDeviceInquiry — provisional M-6 member bytes 0x01/0x00, 20 ms stock");

        const auto info = makeDeviceInfo(0x01, 0x00, "1.11");
        const auto profile = Core::SysExDelayProfile::fromDeviceInquiry(info);
        expectEquals(profile.getDelayMs(), Core::SysExDelayProfile::kStockDelayMsMatrix6);
        expectEquals(static_cast<int>(profile.getDeviceFamily()),
                     static_cast<int>(Core::MatrixDeviceFamily::kMatrix6Or6R));
    }

    void testOptimisedFirmwareStrings()
    {
        beginTest("optimised EPROM — TAUNTEK / GLIGLI / NORDCORE case-insensitive");

        const auto tauntek = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x02, 0x00, "TAUNTEK 2.0"));
        expectEquals(static_cast<int>(tauntek.getEpromClass()),
                     static_cast<int>(Core::EpromClass::kOptimised));

        const auto gligli = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x02, 0x00, "gligli"));
        expectEquals(static_cast<int>(gligli.getEpromClass()),
                     static_cast<int>(Core::EpromClass::kOptimised));

        const auto nordcore = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x02, 0x00, "NORDCORE 1.0"));
        expectEquals(static_cast<int>(nordcore.getEpromClass()),
                     static_cast<int>(Core::EpromClass::kOptimised));
    }

    void testUnknownVersionUsesStock()
    {
        beginTest("empty / factory version — stock class");

        const auto emptyVersion = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x02, 0x00, {}));
        expectEquals(static_cast<int>(emptyVersion.getEpromClass()),
                     static_cast<int>(Core::EpromClass::kStock));

        const auto factory = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x02, 0x00, " 110"));
        expectEquals(static_cast<int>(factory.getEpromClass()),
                     static_cast<int>(Core::EpromClass::kStock));
    }

    void testUnknownMemberBytesFallbackStockM1000()
    {
        beginTest("fromDeviceInquiry — unknown member bytes fall back to stock M-1000, 10 ms");

        const auto info = makeDeviceInfo(0xFF, 0xFF, "1.11");
        const auto profile = Core::SysExDelayProfile::fromDeviceInquiry(info);
        expectEquals(profile.getDelayMs(), Core::SysExDelayProfile::kStockDelayMsMatrix1000);
        expectEquals(static_cast<int>(profile.getEpromClass()),
                     static_cast<int>(Core::EpromClass::kStock));
        expectEquals(static_cast<int>(profile.getDeviceFamily()),
                     static_cast<int>(Core::MatrixDeviceFamily::kMatrix1000));
    }

    void testOptimisedDelayConstants()
    {
        beginTest("optimised EPROM — absolute delay constants per device family");

        const auto optimisedM1000 = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x02, 0x00, "TAUNTEK"));
        expectEquals(optimisedM1000.getDelayMs(),
                     Core::SysExDelayProfile::kOptimisedDelayMsMatrix1000);

        const auto optimisedM6 = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x01, 0x00, "GLIGLI"));
        expectEquals(optimisedM6.getDelayMs(),
                     Core::SysExDelayProfile::kOptimisedDelayMsMatrix6);
    }

    void testTaunVersionWithoutTekTokenStaysStock()
    {
        beginTest("TAUN 2.0 — no TAUNTEK token, stock class");

        const auto profile = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x02, 0x00, "TAUN 2.0"));
        expectEquals(static_cast<int>(profile.getEpromClass()),
                     static_cast<int>(Core::EpromClass::kStock));
        expectEquals(profile.getDelayMs(), Core::SysExDelayProfile::kStockDelayMsMatrix1000);
    }

    void testOptimisedShorterThanStockSameFamily()
    {
        beginTest("optimised delay strictly less than stock per device family");

        const auto stockM1000 = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x02, 0x00, "1.11"));
        const auto optimisedM1000 = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x02, 0x00, "TAUNTEK"));

        expect(stockM1000.getDelayMs() > optimisedM1000.getDelayMs());

        const auto stockM6 = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x01, 0x00, "1.11"));
        const auto optimisedM6 = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x01, 0x00, "GLIGLI"));

        expect(stockM6.getDelayMs() > optimisedM6.getDelayMs());
    }
};

static SysExDelayProfileTests sysExDelayProfileTests;
