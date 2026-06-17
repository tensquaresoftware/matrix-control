#include <juce_core/juce_core.h>

#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/Matrix6Or6RLimits.h"

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
        testMatrix1000InterBankWrapUnlocked();
        testMatrix1000LockedWrapWithinBank();
        testRomGatingMatrix1000Only();
        testDeviceTypeRegistryMemberBytes();
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

        const auto from99 = limits.advancePatch({ 0, Matrix6Or6RLimits::kMaxPatchNumber }, 1, false);
        expectEquals(from99.bank, 0);
        expectEquals(from99.patch, Matrix6Or6RLimits::kMinPatchNumber);

        const auto from00 = limits.advancePatch({ 0, Matrix6Or6RLimits::kMinPatchNumber }, -1, false);
        expectEquals(from00.bank, 0);
        expectEquals(from00.patch, Matrix6Or6RLimits::kMaxPatchNumber);
    }

    void testMatrix1000InterBankWrapUnlocked()
    {
        beginTest("advancePatch — Matrix-1000 inter-bank wrap when unlocked");

        const auto limits = Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000);

        const auto nextBank = limits.advancePatch(
            { 0, Matrix1000Limits::kMaxPatchNumber },
            1,
            false);
        expectEquals(nextBank.bank, 1);
        expectEquals(nextBank.patch, Matrix1000Limits::kMinPatchNumber);

        const auto prevBank = limits.advancePatch(
            { 1, Matrix1000Limits::kMinPatchNumber },
            -1,
            false);
        expectEquals(prevBank.bank, 0);
        expectEquals(prevBank.patch, Matrix1000Limits::kMaxPatchNumber);

        const auto wrapToLastBank = limits.advancePatch(
            { Matrix1000Limits::kMinBankNumber, Matrix1000Limits::kMinPatchNumber },
            -1,
            false);
        expectEquals(wrapToLastBank.bank, Matrix1000Limits::kMaxBankNumber);
        expectEquals(wrapToLastBank.patch, Matrix1000Limits::kMaxPatchNumber);
    }

    void testMatrix1000LockedWrapWithinBank()
    {
        beginTest("advancePatch — Matrix-1000 locked wraps within bank only");

        const auto limits = Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000);

        const auto wrapped = limits.advancePatch(
            { 3, Matrix1000Limits::kMaxPatchNumber },
            1,
            true);
        expectEquals(wrapped.bank, 3);
        expectEquals(wrapped.patch, Matrix1000Limits::kMinPatchNumber);
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

    void testDeviceTypeRegistryMemberBytes()
    {
        beginTest("DeviceTypeRegistry — member byte mapping");

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
    }
};

static DeviceMemoryLimitsTests deviceMemoryLimitsTests;
