#include "PatchManagerActionHandlerTestSupport.h"

#include "PatchFixturePaths.h"

namespace PatchManagerActionHandlerTestSupport
{
    namespace
    {
        void applySetBankOpcode(QueueScanResult& result, const juce::uint8* data, size_t size)
        {
            result.setBank = true;
            if (size >= 6)
                result.setBankValue = data[4];
        }

        void applySinglePatchOpcode(QueueScanResult& result, const juce::uint8* data, size_t size)
        {
            result.patchData = true;
            ++result.patchSysExCount;
            if (size >= 6)
                result.patchNumber = data[4];
        }

        void applyEditBufferOpcode(QueueScanResult& result)
        {
            result.editBufferPatch = true;
            ++result.patchSysExCount;
        }

        void scanSysExMessage(QueueScanResult& result, const juce::MemoryBlock& block)
        {
            if (block.getSize() < 5)
                return;

            const auto* data = static_cast<const juce::uint8*>(block.getData());
            const auto size = block.getSize();
            const auto opcode = data[3];

            if (opcode == SysExConstants::Opcode::kSetBank)
                applySetBankOpcode(result, data, size);
            else if (opcode == SysExConstants::Opcode::kUnlockBank)
                result.unlockBank = true;
            else if (opcode == SysExConstants::Opcode::kSinglePatchData)
                applySinglePatchOpcode(result, data, size);
            else if (opcode == SysExConstants::Opcode::kSinglePatchToEditBuffer)
                applyEditBufferOpcode(result);
        }
    }

    QueueScanResult scanQueue(Core::MidiOutboundQueue& queue)
    {
        QueueScanResult result;

        while (!queue.isEmpty())
        {
            const auto msg = queue.dequeue();
            if (!msg.has_value())
                break;

            if (msg->category == Core::MidiOutboundQueue::MessageCategory::kRealtime)
            {
                if (msg->midiMessage.isProgramChange())
                {
                    ++result.programChangeCount;
                    result.lastProgramChange = msg->midiMessage.getProgramChangeNumber();
                }

                continue;
            }

            if (msg->category != Core::MidiOutboundQueue::MessageCategory::kSysEx)
                continue;

            scanSysExMessage(result, msg->sysExData);
        }

        return result;
    }

    juce::File fixturesPatchesDir()
    {
        return PatchTestFixtures::patchFixturesRoot();
    }

    juce::File createTempScanDir()
    {
        return juce::File::getSpecialLocation(juce::File::tempDirectory)
                   .getNonexistentChildFile("MatrixControlPatchManagerActionHandler", "", false);
    }

    void copyFixturePatchToDir(const juce::File& dir, const juce::String& fileName)
    {
        const auto source = PatchTestFixtures::resolvePatchFixtureFile(fileName);
        const auto destination = dir.getChildFile(fileName);
        jassert(source.existsAsFile());
        jassert(source.copyFileTo(destination));
    }

    void initializePatchManagerState(juce::ValueTree& state,
                                     int bank,
                                     int patch,
                                     bool coordinatesEstablished)
    {
        state.setProperty(BankUtility::StateProperties::kSelectedBank, bank, nullptr);
        state.setProperty(PatchManager::StateProperties::kPatchCoordinatesEstablished,
                          coordinatesEstablished,
                          nullptr);
        state.setProperty(InternalPatches::kCurrentBankNumber, bank, nullptr);
        state.setProperty(InternalPatches::kCurrentPatchNumber, patch, nullptr);
    }

    Core::PatchManagerActionHandler::Dependencies HandlerHarness::makeDependencies()
    {
        return Core::PatchManagerActionHandler::Dependencies {
            proc.apvts,
            [this]() { return limits; },
            &model,
            &mapper,
            &clipboard,
            &patchInitService,
            &patchSelectionMidiSync,
            &midiManager,
            &patchFileService,
            &patchNameSyncer,
            &dirtyPatchTracker,
            &sysExEncoder,
            [this]() { return pickFolderCallback(); },
            [this](juce::File folder, juce::String stem) {
                return pickSaveFileCallback(folder, stem);
            },
            [this](juce::String internalSanitized, juce::String fileSanitized) {
                if (pickReconciliationCallback)
                    return pickReconciliationCallback(internalSanitized, fileSanitized);
                return std::optional<Core::NameReconciliationChoice>{};
            },
            Core::kPatchNavButtonDebounceMs,
            Core::kComboboxPatchSendDebounceMs
        };
    }

    Core::ActionExecutionHooks HandlerHarness::makeHooks()
    {
        Core::ActionExecutionHooks hooks;
        hooks.setSuppressMatrixModSysEx = [this](bool suppress) { suppressMatrixModSysEx = suppress; };
        hooks.setSuppressPatchSysEx = [this](bool suppress) { suppressPatchSysEx = suppress; };
        hooks.onPatchLoaded = [state = patchLoadHookState]() { state->invoked = true; };
        hooks.confirmPatchContextChange = [state = gateState](bool includeUnsavedEditWarning)
        {
            ++state->calls;
            state->lastIncludeUnsavedEditWarning = includeUnsavedEditWarning;
            return state->allow;
        };
        hooks.isDeviceDumpAvailable = [state = dumpFakeState]() { return state->available; };
        hooks.requestDeviceDump = [state = dumpFakeState](juce::uint8 patchNumber,
                                                          Core::ActionExecutionHooks::DeviceDumpCallback onResult)
        {
            state->lastRequestedPatch = patchNumber;
            ++state->requestCount;
            if (state->deferCallback)
            {
                state->pendingCallback = std::move(onResult);
                return;
            }

            if (onResult)
                onResult(state->response);
        };
        hooks.onEditorialCheckpoint = [this]()
        {
            proc.undoManager.clearUndoHistory();
            handler.syncDirtySnapshotFromApvts();
        };
        hooks.requestNameRequiredBeforeStore = [state = nameRequiredBeforeStoreHookState]()
        {
            ++state->calls;
        };
        return hooks;
    }

    void HandlerHarness::applyInitialHarnessState()
    {
        dumpFakeState->response.assign(
            Core::InitDefaults::patchData(),
            Core::InitDefaults::patchData() + Core::PatchModel::kBufferSize);
        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty("deviceType", "Matrix-1000", nullptr);
        initializePatchManagerState(proc.apvts.state, 0, 0, false);
        patchSelectionMidiSync.clearSyncedBankState();
    }

    HandlerHarness::HandlerHarness(Core::DeviceMemoryLimits limitsIn)
        : patchLoadHookState(std::make_shared<PatchLoadHookState>())
        , gateState(std::make_shared<GateState>())
        , dumpFakeState(std::make_shared<DumpFakeState>())
        , nameRequiredBeforeStoreHookState(std::make_shared<NameRequiredBeforeStoreHookState>())
        , mapper(proc.apvts, model)
        , decoder(parser)
        , initLoader(decoder)
        , patchInitService(model, initLoader, [this]() { return initTemplatesFolder; })
        , midiManager(proc.apvts, queue, tracker)
        , patchSelectionMidiSync(&midiManager)
        , limits(std::move(limitsIn))
        , patchFileService(decoder)
        , patchNameSyncer(proc.apvts, model)
        , handler(makeDependencies(), makeHooks())
    {
        applyInitialHarnessState();
    }

    void HandlerHarness::fireDeferredDump()
    {
        if (dumpFakeState->pendingCallback)
        {
            auto callback = std::move(dumpFakeState->pendingCallback);
            dumpFakeState->pendingCallback = nullptr;
            callback(dumpFakeState->response);
        }
    }

    void HandlerHarness::useSuccessfulDeviceDump()
    {
        dumpFakeState->available = true;
        dumpFakeState->deferCallback = false;
        if (dumpFakeState->response.size() != static_cast<size_t>(Core::PatchModel::kBufferSize))
        {
            dumpFakeState->response.assign(
                Core::InitDefaults::patchData(),
                Core::InitDefaults::patchData() + Core::PatchModel::kBufferSize);
        }
    }

    void HandlerHarness::triggerEditorialCheckpoint()
    {
        proc.undoManager.clearUndoHistory();
        handler.syncDirtySnapshotFromApvts();
    }

    void simulateSelectPatchFileDispatch(HandlerHarness& harness)
    {
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());
        harness.handler.flushComputerSelectDebouncerForTests();
    }

    void fireAdjacentNavigation(HandlerHarness& harness, const juce::String& adjacentPropertyId)
    {
        harness.handler.handleAction(adjacentPropertyId, juce::var());
        harness.handler.flushPatchNavDebouncerForTests();
    }

    void fireInternalPatchNavigation(HandlerHarness& harness, const juce::String& navigationPropertyId)
    {
        harness.handler.handleAction(navigationPropertyId, juce::var());
        harness.handler.flushPatchNavDebouncerForTests();
    }

    void fireOpenAndDispatchLoad(HandlerHarness& harness,
                                 std::optional<juce::String>* midFooterBeforeLoad)
    {
        const int beforeId = static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            0));
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kOpenPatchFolder, juce::var());
        const int afterId = static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            0));

        if (afterId != beforeId)
        {
            if (midFooterBeforeLoad != nullptr)
                *midFooterBeforeLoad = harness.proc.apvts.state.getProperty("uiMessageText").toString();
            simulateSelectPatchFileDispatch(harness);
            return;
        }

        harness.handler.flushComputerSelectDebouncerForTests();
    }

    void setupComputerPatchesScan(HandlerHarness& harness, const juce::File& tempDir)
    {
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
    }

    bool matchesCombinedFirstLoadFooter(const juce::String& text,
                                        int validCount,
                                        int invalidCount,
                                        const juce::File& file)
    {
        const auto location = FooterMessages::formatReadablePatchLocation(file);
        const auto loadedPlain = FooterMessages::formatLoadSuccess(location);
        const auto loadedReconFalse = FooterMessages::formatReconciliationNotice(location, false);
        const auto loadedReconTrue = FooterMessages::formatReconciliationNotice(location, true);
        return text == FooterMessages::formatFirstLoadAfterScanMessage(validCount, invalidCount, loadedPlain)
            || text == FooterMessages::formatFirstLoadAfterScanMessage(validCount, invalidCount, loadedReconFalse)
            || text == FooterMessages::formatFirstLoadAfterScanMessage(validCount, invalidCount, loadedReconTrue);
    }

    SelectPatchFileLoadDispatcher::SelectPatchFileLoadDispatcher(HandlerHarness& harnessIn)
        : harness(harnessIn)
    {
        harness.proc.apvts.state.addListener(this);
    }

    SelectPatchFileLoadDispatcher::~SelectPatchFileLoadDispatcher()
    {
        harness.proc.apvts.state.removeListener(this);
    }

    void SelectPatchFileLoadDispatcher::valueTreePropertyChanged(juce::ValueTree&,
                                                                 const juce::Identifier& property)
    {
        if (! armed)
            return;

        if (property.toString() != ComputerPatches::StandaloneWidgets::kSelectPatchFile)
            return;

        armed = false;
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());
        harness.handler.flushComputerSelectDebouncerForTests();
        armed = true;
    }
}
