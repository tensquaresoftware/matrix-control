#include <juce_core/juce_core.h>

#include "Core/MIDI/Queue/SysExDelayProfile.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Shared/Definitions/PluginIDs.h"

class SysExDelayProfileTests : public juce::UnitTest
{
public:
    SysExDelayProfileTests() : juce::UnitTest("SysExDelayProfile") {}

    void runTest() override
    {
        testStockDefaultMatrix1000();
        testStockMatrix1000FromSettings();
        testStockMatrix6FromSettings();
        testOptimisedFromSettingsTypes();
        testUnknownEpromUsesStock();
        testUnknownMemberBytesFallbackStockM1000();
        testFromDeviceInquiryMatrix6MemberBytes();
        testGligliOnMatrix6FamilyYieldsStock();
        testOptimisedDelayConstants();
        testFromDeviceInquiryUsesSettingsNotVersion();
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

    void testStockMatrix1000FromSettings()
    {
        beginTest("fromSettings — FACTORY M-1000, 10 ms");

        const auto profile = Core::SysExDelayProfile::fromSettings(
            PluginIDs::Settings::EpromType::kFactory,
            Core::MatrixDeviceFamily::kMatrix1000);
        expectEquals(profile.getDelayMs(), 10);
        expectEquals(static_cast<int>(profile.getDeviceFamily()),
                     static_cast<int>(Core::MatrixDeviceFamily::kMatrix1000));
    }

    void testStockMatrix6FromSettings()
    {
        beginTest("fromSettings — FACTORY M-6, 20 ms stock");

        const auto profile = Core::SysExDelayProfile::fromSettings(
            PluginIDs::Settings::EpromType::kFactory,
            Core::MatrixDeviceFamily::kMatrix6Or6R);
        expectEquals(profile.getDelayMs(), Core::SysExDelayProfile::kStockDelayMsMatrix6);
        expectEquals(static_cast<int>(profile.getDeviceFamily()),
                     static_cast<int>(Core::MatrixDeviceFamily::kMatrix6Or6R));
    }

    void testOptimisedFromSettingsTypes()
    {
        beginTest("optimised EPROM — GLIGLI / TAUNTEK / UNTERGEEK from Settings");

        using namespace PluginIDs::Settings::EpromType;

        for (const int id : { kGligli, kTauntek, kUntergeek })
        {
            const auto profile = Core::SysExDelayProfile::fromSettings(
                id, Core::MatrixDeviceFamily::kMatrix1000);
            expectEquals(static_cast<int>(profile.getEpromClass()),
                         static_cast<int>(Core::EpromClass::kOptimised));
        }
    }

    void testUnknownEpromUsesStock()
    {
        beginTest("UNKNOWN EPROM TYPE — stock class");

        const auto profile = Core::SysExDelayProfile::fromSettings(
            PluginIDs::Settings::EpromType::kUnknown,
            Core::MatrixDeviceFamily::kMatrix1000);
        expectEquals(static_cast<int>(profile.getEpromClass()),
                     static_cast<int>(Core::EpromClass::kStock));
    }

    void testUnknownMemberBytesFallbackStockM1000()
    {
        beginTest("fromDeviceInquiry — unknown member bytes fall back to M-1000 family");

        const auto info = makeDeviceInfo(0xFF, 0xFF, "1.11");
        const auto profile = Core::SysExDelayProfile::fromDeviceInquiry(
            info, PluginIDs::Settings::EpromType::kFactory);
        expectEquals(profile.getDelayMs(), Core::SysExDelayProfile::kStockDelayMsMatrix1000);
        expectEquals(static_cast<int>(profile.getDeviceFamily()),
                     static_cast<int>(Core::MatrixDeviceFamily::kMatrix1000));
    }

    void testFromDeviceInquiryMatrix6MemberBytes()
    {
        beginTest("fromDeviceInquiry — provisional M-6 member bytes 0x01/0x00");

        const auto profile = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x01, 0x00, "1.11"),
            PluginIDs::Settings::EpromType::kFactory);
        expectEquals(profile.getDelayMs(), Core::SysExDelayProfile::kStockDelayMsMatrix6);
        expectEquals(static_cast<int>(profile.getDeviceFamily()),
                     static_cast<int>(Core::MatrixDeviceFamily::kMatrix6Or6R));
    }

    void testGligliOnMatrix6FamilyYieldsStock()
    {
        beginTest("fromSettings — GLIGLI on Matrix-6 family coerces to stock");

        const auto profile = Core::SysExDelayProfile::fromSettings(
            PluginIDs::Settings::EpromType::kGligli,
            Core::MatrixDeviceFamily::kMatrix6Or6R);
        expectEquals(static_cast<int>(profile.getEpromClass()),
                     static_cast<int>(Core::EpromClass::kStock));
        expectEquals(profile.getDelayMs(), Core::SysExDelayProfile::kStockDelayMsMatrix6);
    }

    void testOptimisedDelayConstants()
    {
        beginTest("optimised EPROM — absolute delay constants per device family");

        const auto optimisedM1000 = Core::SysExDelayProfile::fromSettings(
            PluginIDs::Settings::EpromType::kTauntek,
            Core::MatrixDeviceFamily::kMatrix1000);
        expectEquals(optimisedM1000.getDelayMs(),
                     Core::SysExDelayProfile::kOptimisedDelayMsMatrix1000);

        const auto optimisedM6 = Core::SysExDelayProfile::fromSettings(
            PluginIDs::Settings::EpromType::kTauntek,
            Core::MatrixDeviceFamily::kMatrix6Or6R);
        expectEquals(optimisedM6.getDelayMs(),
                     Core::SysExDelayProfile::kOptimisedDelayMsMatrix6);
    }

    void testFromDeviceInquiryUsesSettingsNotVersion()
    {
        beginTest("fromDeviceInquiry — Settings SSOT, version string ignored for class");

        const auto stockDespiteTauntekVersion = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x02, 0x00, "TAUNTEK"),
            PluginIDs::Settings::EpromType::kFactory);
        expectEquals(static_cast<int>(stockDespiteTauntekVersion.getEpromClass()),
                     static_cast<int>(Core::EpromClass::kStock));

        const auto optimisedDespiteFactoryVersion = Core::SysExDelayProfile::fromDeviceInquiry(
            makeDeviceInfo(0x02, 0x00, "1.11"),
            PluginIDs::Settings::EpromType::kGligli);
        expectEquals(static_cast<int>(optimisedDespiteFactoryVersion.getEpromClass()),
                     static_cast<int>(Core::EpromClass::kOptimised));
    }

    void testOptimisedShorterThanStockSameFamily()
    {
        beginTest("optimised delay strictly less than stock per device family");

        const auto stockM1000 = Core::SysExDelayProfile::fromSettings(
            PluginIDs::Settings::EpromType::kFactory,
            Core::MatrixDeviceFamily::kMatrix1000);
        const auto optimisedM1000 = Core::SysExDelayProfile::fromSettings(
            PluginIDs::Settings::EpromType::kTauntek,
            Core::MatrixDeviceFamily::kMatrix1000);

        expect(stockM1000.getDelayMs() > optimisedM1000.getDelayMs());

        const auto stockM6 = Core::SysExDelayProfile::fromSettings(
            PluginIDs::Settings::EpromType::kUnknown,
            Core::MatrixDeviceFamily::kMatrix6Or6R);
        const auto optimisedM6 = Core::SysExDelayProfile::fromSettings(
            PluginIDs::Settings::EpromType::kUntergeek,
            Core::MatrixDeviceFamily::kMatrix6Or6R);

        expect(stockM6.getDelayMs() > optimisedM6.getDelayMs());
    }
};

static SysExDelayProfileTests sysExDelayProfileTests;
