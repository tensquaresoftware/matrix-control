#include "PatchManagerActionHandlerTestSupport.h"

#include "Core/Init/InitDefaults.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerHeaderClickTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerHeaderClickTests()
        : juce::UnitTest("PatchManagerActionHandlerHeaderClick")
    {
    }

    void runTest() override
    {
        testComputerHeader_focusOnlyWhenNoSelection();
        testComputerHeader_reclaimAndReloadWhenLoadable();
        testComputerHeader_alreadyFocusedStillReloads();
        testInternalHeader_focusOnlyWhenCoordinatesUndefined();
        testInternalHeader_reclaimAndReloadWhenEstablished();
        testInternalHeader_alreadyFocusedStillReloads();
        testInternalHeader_reclaimSendsProgramChangeBeforeDump();
        testComputerHeader_gateCancelKeepsFocusNoLoad();
        testInternalHeader_gateCancelKeepsFocusNoDump();
    }

private:
    static int navigationFocus(const HandlerHarness& harness)
    {
        return static_cast<int>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kDefault));
    }

    static bool writeValidPatchSyx(const juce::File& dir, const juce::String& fileName)
    {
        if (! dir.isDirectory() && ! dir.createDirectory())
            return false;

        Core::PatchModel model;
        model.loadFrom(Core::InitDefaults::patchData());
        SysExEncoder encoder;
        const auto syx = encoder.encodePatchSysEx(0, model.data());
        return dir.getChildFile(fileName).replaceWithData(syx.getData(), syx.getSize());
    }

    void testComputerHeader_focusOnlyWhenNoSelection()
    {
        beginTest("computerHeader_focusOnly_whenNoSelection");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 5, true);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kInternal,
            nullptr);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            0,
            nullptr);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kHeaderClick, juce::var());

        expectEquals(navigationFocus(harness), PatchManager::NavigationFocus::kComputer);
        expect(harness.queue.isEmpty());
        expectEquals(harness.gateState->calls, 0);
    }

    void testComputerHeader_reclaimAndReloadWhenLoadable()
    {
        beginTest("computerHeader_reclaimAndReload_whenLoadable");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 5, true);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kInternal,
            nullptr);

        const auto tempDir = createTempScanDir();
        expect(writeValidPatchSyx(tempDir, "HeaderReload.syx"));
        setupComputerPatchesScan(harness, tempDir);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kHeaderClick, juce::var());

        expectEquals(navigationFocus(harness), PatchManager::NavigationFocus::kComputer);
        expect(harness.gateState->calls >= 1);
        const auto queued = scanQueue(harness.queue);
        expect(queued.editBufferPatch || queued.patchData);

        tempDir.deleteRecursively();
    }

    void testComputerHeader_alreadyFocusedStillReloads()
    {
        beginTest("computerHeader_alreadyFocused_stillReloads");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 5, true);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kComputer,
            nullptr);

        const auto tempDir = createTempScanDir();
        expect(writeValidPatchSyx(tempDir, "HeaderReload.syx"));
        setupComputerPatchesScan(harness, tempDir);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kHeaderClick, juce::var());

        expectEquals(navigationFocus(harness), PatchManager::NavigationFocus::kComputer);
        expect(harness.gateState->calls >= 1);
        const auto queued = scanQueue(harness.queue);
        expect(queued.editBufferPatch || queued.patchData);

        tempDir.deleteRecursively();
    }

    void testInternalHeader_focusOnlyWhenCoordinatesUndefined()
    {
        beginTest("internalHeader_focusOnly_whenCoordinatesUndefined");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 0, false);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kComputer,
            nullptr);

        harness.handler.handleAction(InternalPatches::kHeaderClick, juce::var());

        expectEquals(navigationFocus(harness), PatchManager::NavigationFocus::kInternal);
        expectEquals(harness.dumpFakeState->requestCount, 0);
        expectEquals(harness.gateState->calls, 0);
    }

    void testInternalHeader_reclaimAndReloadWhenEstablished()
    {
        beginTest("internalHeader_reclaimAndReload_whenEstablished");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 2, 17, true);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kComputer,
            nullptr);
        harness.useSuccessfulDeviceDump();
        harness.dumpFakeState->deferCallback = true;

        harness.handler.handleAction(InternalPatches::kHeaderClick, juce::var());

        expectEquals(navigationFocus(harness), PatchManager::NavigationFocus::kInternal);
        expectEquals(harness.gateState->calls, 1);
        expectEquals(harness.dumpFakeState->requestCount, 1);
        expectEquals(static_cast<int>(harness.dumpFakeState->lastRequestedPatch), 17);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 2);
        expect(queued.programChangeCount >= 1);
        expectEquals(queued.lastProgramChange, 17);

        harness.fireDeferredDump();
        expectEquals(navigationFocus(harness), PatchManager::NavigationFocus::kInternal);
    }

    void testInternalHeader_alreadyFocusedStillReloads()
    {
        beginTest("internalHeader_alreadyFocused_stillReloads");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 1, 8, true);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kInternal,
            nullptr);
        harness.useSuccessfulDeviceDump();
        harness.dumpFakeState->deferCallback = true;

        harness.handler.handleAction(InternalPatches::kHeaderClick, juce::var());

        expectEquals(navigationFocus(harness), PatchManager::NavigationFocus::kInternal);
        expectEquals(harness.dumpFakeState->requestCount, 1);
        expectEquals(static_cast<int>(harness.dumpFakeState->lastRequestedPatch), 8);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 1);
        expect(queued.programChangeCount >= 1);
        expectEquals(queued.lastProgramChange, 8);
    }

    void testInternalHeader_reclaimSendsProgramChangeBeforeDump()
    {
        beginTest("internalHeader_reclaim_sendsProgramChange_beforeDump");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 3, 12, true);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kComputer,
            nullptr);
        harness.patchSelectionMidiSync.resetLastSyncedBank(3);
        harness.useSuccessfulDeviceDump();
        harness.dumpFakeState->deferCallback = true;

        harness.handler.handleAction(InternalPatches::kHeaderClick, juce::var());

        expectEquals(harness.dumpFakeState->requestCount, 1);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 3);
        expect(queued.programChangeCount >= 1);
        expectEquals(queued.lastProgramChange, 12);
        expect(!queued.editBufferPatch);
        expect(!queued.patchData);
    }

    void testComputerHeader_gateCancelKeepsFocusNoLoad()
    {
        beginTest("computerHeader_gateCancel_keepsFocus_noLoad");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 5, true);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kInternal,
            nullptr);
        harness.gateState->allow = false;

        const auto tempDir = createTempScanDir();
        expect(writeValidPatchSyx(tempDir, "HeaderReload.syx"));
        setupComputerPatchesScan(harness, tempDir);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kHeaderClick, juce::var());

        expectEquals(navigationFocus(harness), PatchManager::NavigationFocus::kComputer);
        expect(harness.gateState->calls >= 1);
        expect(harness.queue.isEmpty());

        tempDir.deleteRecursively();
    }

    void testInternalHeader_gateCancelKeepsFocusNoDump()
    {
        beginTest("internalHeader_gateCancel_keepsFocus_noDump");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 2, 17, true);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kComputer,
            nullptr);
        harness.gateState->allow = false;

        harness.handler.handleAction(InternalPatches::kHeaderClick, juce::var());

        expectEquals(navigationFocus(harness), PatchManager::NavigationFocus::kInternal);
        expectEquals(harness.gateState->calls, 1);
        expectEquals(harness.dumpFakeState->requestCount, 0);
        expect(harness.queue.isEmpty());
    }
};

static PatchManagerActionHandlerHeaderClickTests patchManagerActionHandlerHeaderClickTests;
