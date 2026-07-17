#include <juce_core/juce_core.h>

#include "Core/Services/DeviceMemoryLimits.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/Matrix6Or6RLimits.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

class DeviceMemoryLimitsTests : public juce::UnitTest
{
public:
    DeviceMemoryLimitsTests() : juce::UnitTest("DeviceMemoryLimits") {}

    void runTest() override
    {
        testMatrix1000Limits();
        testMatrix6Limits();
        testMatrix6RLimits();
        testUnknownDefaultsToMatrix1000();
        testMatrix6CyclicWrap();
        testMatrix1000WrapWithinBank();
        testRomGatingMatrix1000Only();
    }

private:
    void testMatrix1000Limits()
    {
        beginTest("resolve — Matrix-1000 bank concept and patch bounds");

        const auto limits = Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000);
        expect(limits.hasBankConcept());
        expectEquals(limits.minBankNumber(), Matrix1000Limits::kMinBankNumber);
        expectEquals(limits.maxBankNumber(), Matrix1000Limits::kMaxBankNumber);
        expectEquals(limits.minPatchNumber(), Matrix1000Limits::kMinPatchNumber);
        expectEquals(limits.maxPatchNumber(), Matrix1000Limits::kMaxPatchNumber);
        expect(limits.hasRomBanks());
    }

    void testMatrix6Limits()
    {
        beginTest("resolve — Matrix-6 has no bank concept");

        const auto limits = Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix6);
        expect(!limits.hasBankConcept());
        expectEquals(limits.minPatchNumber(), Matrix6Or6RLimits::kMinPatchNumber);
        expectEquals(limits.maxPatchNumber(), Matrix6Or6RLimits::kMaxPatchNumber);
        expectEquals(limits.internalPatchSlotCount(), Matrix6Or6RLimits::kInternalPatchSlotCount);
        expect(!limits.hasRomBanks());
    }

    void testMatrix6RLimits()
    {
        beginTest("resolve — Matrix-6R matches Matrix-6 memory model");

        const auto limits = Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix6R);
        expect(!limits.hasBankConcept());
        expectEquals(limits.minPatchNumber(), Matrix6Or6RLimits::kMinPatchNumber);
        expectEquals(limits.maxPatchNumber(), Matrix6Or6RLimits::kMaxPatchNumber);
        expectEquals(limits.internalPatchSlotCount(), Matrix6Or6RLimits::kInternalPatchSlotCount);
        expect(!limits.hasRomBanks());
    }

    void testUnknownDefaultsToMatrix1000()
    {
        beginTest("resolve — unknown device type defaults to Matrix-1000 limits");

        const auto limits = Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kUnknown);
        expect(limits.hasBankConcept());
        expectEquals(limits.maxBankNumber(), Matrix1000Limits::kMaxBankNumber);
    }

    void testMatrix6CyclicWrap()
    {
        beginTest("advancePatch — Matrix-6 cyclic wrap 99 to 00 and back");

        const auto limits = Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix6);

        const auto from99 = limits.advancePatch({ 0, Matrix6Or6RLimits::kMaxPatchNumber }, 1);
        expectEquals(from99.bank, 0);
        expectEquals(from99.patch, Matrix6Or6RLimits::kMinPatchNumber);

        const auto from00 = limits.advancePatch({ 0, Matrix6Or6RLimits::kMinPatchNumber }, -1);
        expectEquals(from00.bank, 0);
        expectEquals(from00.patch, Matrix6Or6RLimits::kMaxPatchNumber);
    }

    void testMatrix1000WrapWithinBank()
    {
        beginTest("advancePatch — Matrix-1000 always wraps within bank");

        const auto limits = Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000);

        const auto nextWrap = limits.advancePatch(
            { 0, Matrix1000Limits::kMaxPatchNumber },
            1);
        expectEquals(nextWrap.bank, 0);
        expectEquals(nextWrap.patch, Matrix1000Limits::kMinPatchNumber);

        const auto lowestAddressPrevWrap = limits.advancePatch(
            { Matrix1000Limits::kMinBankNumber, Matrix1000Limits::kMinPatchNumber },
            -1);
        expectEquals(lowestAddressPrevWrap.bank, Matrix1000Limits::kMinBankNumber);
        expectEquals(lowestAddressPrevWrap.patch, Matrix1000Limits::kMaxPatchNumber);

        const auto prevWrap = limits.advancePatch(
            { 1, Matrix1000Limits::kMinPatchNumber },
            -1);
        expectEquals(prevWrap.bank, 1);
        expectEquals(prevWrap.patch, Matrix1000Limits::kMaxPatchNumber);

        const auto midBankWrap = limits.advancePatch(
            { 3, Matrix1000Limits::kMaxPatchNumber },
            1);
        expectEquals(midBankWrap.bank, 3);
        expectEquals(midBankWrap.patch, Matrix1000Limits::kMinPatchNumber);
    }

    void testRomGatingMatrix1000Only()
    {
        beginTest("isPasteStoreAllowed — ROM banks Matrix-1000 only");

        const auto m1000 = Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000);
        expect(!m1000.isPasteStoreAllowed(2));
        expect(m1000.isPasteStoreAllowed(0));

        const auto m6 = Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix6);
        expect(m6.isPasteStoreAllowed(0));
        expect(m6.isPasteStoreAllowed(Matrix6Or6RLimits::kMaxPatchNumber));
    }
};

static DeviceMemoryLimitsTests deviceMemoryLimitsTests;
