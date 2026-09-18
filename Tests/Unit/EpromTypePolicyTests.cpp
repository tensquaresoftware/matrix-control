#include <juce_core/juce_core.h>

#include "Core/MIDI/Queue/SysExDelayProfile.h"
#include "Core/Services/EpromTypePolicy.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginIDs.h"

class EpromTypePolicyTests : public juce::UnitTest
{
public:
    EpromTypePolicyTests() : juce::UnitTest("EpromTypePolicy") {}

    void runTest() override
    {
        testNormalizeClampsInvalid();
        testToEpromClassStockAndOptimised();
        testMatrix6FamilyRejectsGligli();
        testInquirySuggestionMatrix1000();
        testInquirySuggestionMatrix6();
        testInquirySuggestionAmbiguous();
        testPreferredForPrompt();
        testForEachValidItemSizesAndContents();
        testDeviceFamilyFromType();
    }

private:
    void testNormalizeClampsInvalid()
    {
        beginTest("normalize — invalid ids fall back to UNKNOWN");

        expectEquals(Core::EpromTypePolicy::normalize(0),
                     PluginIDs::Settings::EpromType::kUnknown);
        expectEquals(Core::EpromTypePolicy::normalize(99),
                     PluginIDs::Settings::EpromType::kUnknown);
        expectEquals(Core::EpromTypePolicy::normalize(PluginIDs::Settings::EpromType::kTauntek),
                     PluginIDs::Settings::EpromType::kTauntek);
    }

    void testToEpromClassStockAndOptimised()
    {
        beginTest("toEpromClass — FACTORY/UNKNOWN stock; GLIGLI/TAUNTEK/UNTERGEEK optimised");

        using namespace PluginIDs::Settings::EpromType;

        expectEquals(static_cast<int>(Core::EpromTypePolicy::toEpromClass(kFactory)),
                     static_cast<int>(Core::EpromClass::kStock));
        expectEquals(static_cast<int>(Core::EpromTypePolicy::toEpromClass(kUnknown)),
                     static_cast<int>(Core::EpromClass::kStock));
        expectEquals(static_cast<int>(Core::EpromTypePolicy::toEpromClass(kGligli)),
                     static_cast<int>(Core::EpromClass::kOptimised));
        expectEquals(static_cast<int>(Core::EpromTypePolicy::toEpromClass(kTauntek)),
                     static_cast<int>(Core::EpromClass::kOptimised));
        expectEquals(static_cast<int>(Core::EpromTypePolicy::toEpromClass(kUntergeek)),
                     static_cast<int>(Core::EpromClass::kOptimised));
    }

    void testMatrix6FamilyRejectsGligli()
    {
        beginTest("Matrix-6 family — GLIGLI coerced to UNKNOWN");

        using namespace PluginIDs::Settings::EpromType;
        const auto family = Core::MatrixDeviceFamily::kMatrix6Or6R;

        expect(! Core::EpromTypePolicy::isValidForDeviceFamily(kGligli, family));
        expectEquals(Core::EpromTypePolicy::coerceForDeviceFamily(kGligli, family), kUnknown);
        expectEquals(Core::EpromTypePolicy::coerceForDeviceFamily(kTauntek, family), kTauntek);
    }

    void testInquirySuggestionMatrix1000()
    {
        beginTest("Inquiry suggestion — Matrix-1000 version map");

        using namespace PluginIDs::Settings::EpromType;
        const auto family = Core::MatrixDeviceFamily::kMatrix1000;

        expectEquals(Core::EpromTypePolicy::suggestFromInquiryVersion("1.16", family), kGligli);
        expectEquals(Core::EpromTypePolicy::suggestFromInquiryVersion("1.20", family), kTauntek);
        expectEquals(Core::EpromTypePolicy::suggestFromInquiryVersion("1.21", family), kUntergeek);
        expectEquals(Core::EpromTypePolicy::suggestFromInquiryVersion("1.11", family), kFactory);
        expectEquals(Core::EpromTypePolicy::suggestFromInquiryVersion("1.09", family), kFactory);
        expectEquals(Core::EpromTypePolicy::suggestFromInquiryVersion(" 1.20 ", family), kTauntek);
    }

    void testInquirySuggestionMatrix6()
    {
        beginTest("Inquiry suggestion — Matrix-6/6R 2.15 → TAUNTEK");

        expectEquals(Core::EpromTypePolicy::suggestFromInquiryVersion(
                         "2.15", Core::MatrixDeviceFamily::kMatrix6Or6R),
                     PluginIDs::Settings::EpromType::kTauntek);
    }

    void testInquirySuggestionAmbiguous()
    {
        beginTest("Inquiry suggestion — unmapped version → UNKNOWN");

        expectEquals(Core::EpromTypePolicy::suggestFromInquiryVersion(
                         "9.99", Core::MatrixDeviceFamily::kMatrix1000),
                     PluginIDs::Settings::EpromType::kUnknown);
        expectEquals(Core::EpromTypePolicy::suggestFromInquiryVersion(
                         "1.16", Core::MatrixDeviceFamily::kMatrix6Or6R),
                     PluginIDs::Settings::EpromType::kUnknown);
    }

    void testPreferredForPrompt()
    {
        beginTest("preferredForPrompt — mapped suggestion wins; else stored");

        using namespace PluginIDs::Settings::EpromType;

        expectEquals(Core::EpromTypePolicy::preferredForPrompt(kTauntek, kFactory), kTauntek);
        expectEquals(Core::EpromTypePolicy::preferredForPrompt(kUnknown, kGligli), kGligli);
        expectEquals(Core::EpromTypePolicy::preferredForPrompt(kUnknown, kUnknown), kUnknown);
        expectEquals(Core::EpromTypePolicy::preferredForPrompt(99, kFactory), kFactory);
    }

    void testForEachValidItemSizesAndContents()
    {
        beginTest("forEachValidItem — Matrix-1000 has 5; Matrix-6 family has 4 without GLIGLI");

        using namespace PluginIDs::Settings::EpromType;

        juce::Array<int> m1000;
        Core::EpromTypePolicy::forEachValidItem(Core::MatrixDeviceFamily::kMatrix1000,
                                                [&m1000](int id) { m1000.add(id); });
        expectEquals(m1000.size(), 5);
        expect(m1000.contains(kFactory));
        expect(m1000.contains(kGligli));
        expect(m1000.contains(kTauntek));
        expect(m1000.contains(kUntergeek));
        expect(m1000.contains(kUnknown));

        juce::Array<int> m6;
        Core::EpromTypePolicy::forEachValidItem(Core::MatrixDeviceFamily::kMatrix6Or6R,
                                                [&m6](int id) { m6.add(id); });
        expectEquals(m6.size(), 4);
        expect(m6.contains(kFactory));
        expect(! m6.contains(kGligli));
        expect(m6.contains(kTauntek));
        expect(m6.contains(kUntergeek));
        expect(m6.contains(kUnknown));
    }

    void testDeviceFamilyFromType()
    {
        beginTest("deviceFamilyFromType — Matrix-6/6R vs Matrix-1000 / Unknown");

        expectEquals(static_cast<int>(Core::EpromTypePolicy::deviceFamilyFromType(
                         MatrixDeviceTypes::Type::kMatrix6)),
                     static_cast<int>(Core::MatrixDeviceFamily::kMatrix6Or6R));
        expectEquals(static_cast<int>(Core::EpromTypePolicy::deviceFamilyFromType(
                         MatrixDeviceTypes::Type::kMatrix6R)),
                     static_cast<int>(Core::MatrixDeviceFamily::kMatrix6Or6R));
        expectEquals(static_cast<int>(Core::EpromTypePolicy::deviceFamilyFromType(
                         MatrixDeviceTypes::Type::kMatrix1000)),
                     static_cast<int>(Core::MatrixDeviceFamily::kMatrix1000));
        expectEquals(static_cast<int>(Core::EpromTypePolicy::deviceFamilyFromType(
                         MatrixDeviceTypes::Type::kUnknown)),
                     static_cast<int>(Core::MatrixDeviceFamily::kMatrix1000));
    }
};

static EpromTypePolicyTests epromTypePolicyTests;
