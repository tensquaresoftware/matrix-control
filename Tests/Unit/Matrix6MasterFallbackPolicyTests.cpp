#include <juce_core/juce_core.h>

#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/MIDI/MasterPullOnConnectPolicy.h"
#include "Core/MIDI/Matrix6MasterFallbackPolicy.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

class Matrix6MasterFallbackPolicyTests : public juce::UnitTest
{
public:
    Matrix6MasterFallbackPolicyTests() : juce::UnitTest("Matrix6MasterFallbackPolicy") {}

    void runTest() override
    {
        testUndetectedTimeoutRunsFallback();
        testSoftAbortPresenceSkipsFallback();
        testPortPairForceRunsFallback();
        testMatrix1000PullOnConnectStillAllowed();
        testFallbackFamilyDoesNotArmMasterPull();
        testFallbackSuccessIdentity();
        testFallbackTimeoutFailureMessage();
        testStaleTokenCancelsFallback();
        testFallbackUnlocksEditorGate();
    }

private:
    void testUndetectedTimeoutRunsFallback()
    {
        beginTest("Inquiry timeout while undetected — Master fallback runs");

        expect(Core::shouldRunMatrix6MasterFallbackOnInquiryTimeout(false, false));
        expect(Core::shouldRunMatrix6MasterFallbackOnInquiryTimeout(false, true));
    }

    void testSoftAbortPresenceSkipsFallback()
    {
        beginTest("Already detected on same pair — soft-abort, no Master fallback");

        expect(! Core::shouldRunMatrix6MasterFallbackOnInquiryTimeout(true, false));
    }

    void testPortPairForceRunsFallback()
    {
        beginTest("Port-pair change force flag + Inquiry timeout — Master fallback runs");

        expect(Core::shouldRunMatrix6MasterFallbackOnInquiryTimeout(true, true));
    }

    void testMatrix1000PullOnConnectStillAllowed()
    {
        beginTest("Matrix-1000 Inquiry success — Master pull-on-connect may still arm");

        expect(Core::shouldPullMasterAfterDeviceInquirySuccess(
            false, MatrixDeviceTypes::Type::kUnknown, MatrixDeviceTypes::Type::kMatrix1000));
        // Inquiry success never evaluates the Inquiry-timeout fallback helper.
        expect(Core::shouldRunMatrix6MasterFallbackOnInquiryTimeout(false, false));
    }

    void testFallbackFamilyDoesNotArmMasterPull()
    {
        beginTest("Matrix-6 family after Master fallback — pull-on-connect stays off");

        expect(! Core::shouldPullMasterAfterDeviceInquirySuccess(
            false, MatrixDeviceTypes::Type::kUnknown, MatrixDeviceTypes::Type::kMatrix6));
        expect(! Core::shouldPullMasterAfterDeviceInquirySuccess(
            true,
            MatrixDeviceTypes::Type::kMatrix6,
            MatrixDeviceTypes::Type::kMatrix6,
            true));
    }

    void testFallbackSuccessIdentity()
    {
        beginTest("Master fallback success — Matrix-6 family, empty version, preserve EPROM");

        expect(Core::matrix6MasterFallbackDeviceType() == MatrixDeviceTypes::Type::kMatrix6);
        expect(juce::String(Core::matrix6MasterFallbackDeviceVersion()).isEmpty());
        expect(Core::matrix6MasterFallbackPreservesStoredEpromType());
    }

    void testFallbackTimeoutFailureMessage()
    {
        beginTest("Master fallback timeout — clear Inquiry-then-Master failure copy");

        const juce::String message(Core::matrix6MasterFallbackTimeoutFailureMessage());
        expect(message.containsIgnoreCase("Device Inquiry"));
        expect(message.containsIgnoreCase("Master"));
    }

    void testStaleTokenCancelsFallback()
    {
        beginTest("New inquiry token — stale Master fallback handlers no-op");

        expect(Core::isMatrix6MasterFallbackTokenCurrent(7, 7));
        expect(! Core::isMatrix6MasterFallbackTokenCurrent(7, 8));
    }

    void testFallbackUnlocksEditorGate()
    {
        beginTest("Detected Matrix-6 after fallback — editor SysEx gate unlocks");

        expect(! Core::isSectionLocked(
            true, MatrixDeviceTypes::Type::kMatrix6, false, false));
        expect(Core::maySendEditorProgramChange(
            true, MatrixDeviceTypes::Type::kMatrix6, false));
    }
};

static Matrix6MasterFallbackPolicyTests matrix6MasterFallbackPolicyTests;
