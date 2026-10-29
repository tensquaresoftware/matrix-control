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
        testMasterOnlyPresenceProofPolicy();
        testUndetectedTimeoutRunsFallback();
        testMatrix1000PresenceSkipsFallback();
        testMatrix6FamilyPresenceRunsFallback();
        testPortPairForceRunsFallback();
        testSoftAbortOnPresenceMasterFailure();
        testClearDetectOnDiscoveryMasterFailure();
        testMatrix1000PullOnConnectStillAllowed();
        testFallbackFamilyDoesNotArmMasterPull();
        testFallbackSuccessIdentity();
        testFallbackTimeoutFailureMessage();
        testStaleTokenCancelsFallback();
        testFallbackUnlocksEditorGate();
    }

private:
    void testMasterOnlyPresenceProofPolicy()
    {
        beginTest("Detected Matrix-6 family — Master-only presence; others keep Inquiry-first");

        expect(Core::shouldSendMasterOnlyPresenceProof(
            true, MatrixDeviceTypes::Type::kMatrix6));
        expect(Core::shouldSendMasterOnlyPresenceProof(
            true, MatrixDeviceTypes::Type::kMatrix6R));
        expect(! Core::shouldSendMasterOnlyPresenceProof(
            true, MatrixDeviceTypes::Type::kMatrix1000));
        expect(! Core::shouldSendMasterOnlyPresenceProof(
            true, MatrixDeviceTypes::Type::kUnknown));
        expect(! Core::shouldSendMasterOnlyPresenceProof(
            false, MatrixDeviceTypes::Type::kMatrix6));
        expect(! Core::shouldSendMasterOnlyPresenceProof(
            false, MatrixDeviceTypes::Type::kUnknown));
        expect(! Core::shouldSendMasterOnlyPresenceProof(
            true, MatrixDeviceTypes::Type::kMatrix6, true),
               "Port-pair force must keep Inquiry-first");
    }

    void testUndetectedTimeoutRunsFallback()
    {
        beginTest("Inquiry timeout while undetected — Master fallback runs");

        expect(Core::shouldRunMatrix6MasterFallbackOnInquiryTimeout(
            false, false, MatrixDeviceTypes::Type::kUnknown));
        expect(Core::shouldRunMatrix6MasterFallbackOnInquiryTimeout(
            false, true, MatrixDeviceTypes::Type::kUnknown));
    }

    void testMatrix1000PresenceSkipsFallback()
    {
        beginTest("Detected Matrix-1000 on same pair — soft-abort, no Master fallback");

        expect(! Core::shouldRunMatrix6MasterFallbackOnInquiryTimeout(
            true, false, MatrixDeviceTypes::Type::kMatrix1000));
    }

    void testMatrix6FamilyPresenceRunsFallback()
    {
        beginTest("Detected Matrix-6 family on same pair — Master presence re-check runs");

        expect(Core::shouldRunMatrix6MasterFallbackOnInquiryTimeout(
            true, false, MatrixDeviceTypes::Type::kMatrix6));
        expect(Core::shouldRunMatrix6MasterFallbackOnInquiryTimeout(
            true, false, MatrixDeviceTypes::Type::kMatrix6R));
    }

    void testPortPairForceRunsFallback()
    {
        beginTest("Port-pair change force flag + Inquiry timeout — Master fallback runs");

        expect(Core::shouldRunMatrix6MasterFallbackOnInquiryTimeout(
            true, true, MatrixDeviceTypes::Type::kMatrix1000));
        expect(Core::shouldRunMatrix6MasterFallbackOnInquiryTimeout(
            true, true, MatrixDeviceTypes::Type::kMatrix6));
    }

    void testSoftAbortOnPresenceMasterFailure()
    {
        beginTest("Matrix-6 family presence Master failure — soft-abort, keep detection");

        expect(Core::shouldSoftAbortOnMatrix6MasterFallbackFailure(
            true, MatrixDeviceTypes::Type::kMatrix6, false));
        expect(Core::shouldSoftAbortOnMatrix6MasterFallbackFailure(
            true, MatrixDeviceTypes::Type::kMatrix6R, false));
        expect(! Core::shouldSoftAbortOnMatrix6MasterFallbackFailure(
            true, MatrixDeviceTypes::Type::kMatrix1000, false));
    }

    void testClearDetectOnDiscoveryMasterFailure()
    {
        beginTest("Undetected or port-pair force Master failure — clear detect, not soft-abort");

        expect(! Core::shouldSoftAbortOnMatrix6MasterFallbackFailure(
            false, MatrixDeviceTypes::Type::kUnknown, false));
        expect(! Core::shouldSoftAbortOnMatrix6MasterFallbackFailure(
            true, MatrixDeviceTypes::Type::kMatrix6, true));
    }

    void testMatrix1000PullOnConnectStillAllowed()
    {
        beginTest("Matrix-1000 Inquiry success — Master pull-on-connect may still arm");

        expect(Core::shouldPullMasterAfterDeviceInquirySuccess(
            false, MatrixDeviceTypes::Type::kUnknown, MatrixDeviceTypes::Type::kMatrix1000));
        // Inquiry success never evaluates the Inquiry-timeout fallback helper.
        expect(Core::shouldRunMatrix6MasterFallbackOnInquiryTimeout(
            false, false, MatrixDeviceTypes::Type::kUnknown));
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
