#include "PatchManagerActionHandlerTestSupport.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerStoreSentinelTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerStoreSentinelTests()
        : juce::UnitTest("PatchManagerActionHandlerStoreSentinel")
    {
    }

    void runTest() override
    {
        testStoreInitSentinel_requestsNameRequired_noSysEx();
        testStoreInitSentinel_clearedName_sendsSysEx();
        testStoreInitSentinel_reclickWhileSentinel_invokesHookAgain();
    }

private:
    void testStoreInitSentinel_requestsNameRequired_noSysEx()
    {
        beginTest("store_initSentinel_requestsNameRequired_noSysEx");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 1, 10, false);

        harness.model.setName(
            PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kInitPatchName);
        harness.patchNameSyncer.bufferToApvts();

        harness.handler.handleAction(InternalPatches::kStorePatch, juce::var());

        expectEquals(harness.nameRequiredBeforeStoreHookState->calls, 1);
        expect(harness.queue.isEmpty());
    }

    void testStoreInitSentinel_clearedName_sendsSysEx()
    {
        beginTest("store_initSentinel_clearedName_sendsSysEx");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 1, 11, false);

        harness.model.setName(
            PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kInitPatchName);
        harness.patchNameSyncer.bufferToApvts();

        harness.handler.handleAction(InternalPatches::kStorePatch, juce::var());
        expectEquals(harness.nameRequiredBeforeStoreHookState->calls, 1);
        expect(harness.queue.isEmpty());

        harness.model.setName("NEWNAMED");
        harness.patchNameSyncer.bufferToApvts();

        harness.handler.handleAction(InternalPatches::kStorePatch, juce::var());

        expectEquals(harness.nameRequiredBeforeStoreHookState->calls, 1);
        const auto queued = scanQueue(harness.queue);
        expect(queued.patchData);
        expect(!queued.editBufferPatch);
    }

    void testStoreInitSentinel_reclickWhileSentinel_invokesHookAgain()
    {
        beginTest("store_initSentinel_reclickWhileSentinel_invokesHookAgain");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 1, 12, false);

        harness.model.setName(
            PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kInitPatchName);
        harness.patchNameSyncer.bufferToApvts();

        harness.handler.handleAction(InternalPatches::kStorePatch, juce::var());
        harness.handler.handleAction(InternalPatches::kStorePatch, juce::var());

        expectEquals(harness.nameRequiredBeforeStoreHookState->calls, 2);
        expect(harness.queue.isEmpty());
    }
};

static PatchManagerActionHandlerStoreSentinelTests patchManagerActionHandlerStoreSentinelTests;
