#include <functional>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/Init/InitDefaults.h"
#include "Core/Init/InitTemplateLoader.h"
#include "Core/Init/PatchInitService.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/PatchSelectionMidiSync.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/ClipboardService.h"
#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchFileNameReconciler.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace PatchManager = PluginIDs::PatchManagerSection;
namespace InternalPatches = PatchManager::InternalPatchesModule::StandaloneWidgets;
namespace BankUtility = PatchManager::BankUtilityModule;
namespace ComputerPatches = PatchManager::ComputerPatchesModule;
namespace PatchNameIds = PluginIDs::PatchEditSection::PatchNameModule;
namespace FooterMessages = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages;
namespace Policy = PluginIDs::Settings::NameReconciliationPolicy;

class TestAudioProcessorPatchManager : public juce::AudioProcessor
{
public:
    TestAudioProcessorPatchManager()
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, nullptr, "P", {})
    {
    }

    juce::AudioProcessorValueTreeState apvts;

    const juce::String getName() const override { return "Test"; }
    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}
};

namespace
{
    struct QueueScanResult
    {
        bool setBank = false;
        bool unlockBank = false;
        bool patchData = false;
        int setBankValue = -1;
    };

    QueueScanResult scanQueue(Core::MidiOutboundQueue& queue)
    {
        QueueScanResult result;

        while (!queue.isEmpty())
        {
            const auto msg = queue.dequeue();
            if (!msg.has_value())
                break;

            if (msg->category != Core::MidiOutboundQueue::MessageCategory::kSysEx)
                continue;

            const auto& block = msg->sysExData;
            if (block.getSize() < 5)
                continue;

            const auto* data = static_cast<const juce::uint8*>(block.getData());

            if (data[3] == SysExConstants::Opcode::kSetBank && block.getSize() >= 6)
            {
                result.setBank = true;
                result.setBankValue = data[4];
            }
            else if (data[3] == SysExConstants::Opcode::kUnlockBank)
            {
                result.unlockBank = true;
            }
            else if (data[3] == SysExConstants::Opcode::kSinglePatchData)
            {
                result.patchData = true;
            }
        }

        return result;
    }

    juce::File fixturesPatchesDir()
    {
        return juce::File(MATRIX_TEST_FIXTURES_DIR).getChildFile("Patches");
    }

    juce::File createTempScanDir()
    {
        return juce::File::getSpecialLocation(juce::File::tempDirectory)
                   .getNonexistentChildFile("MatrixControlPatchManagerActionHandler", "", false);
    }

    void copyFixturePatchToDir(const juce::File& dir, const juce::String& fileName)
    {
        const auto source = fixturesPatchesDir().getChildFile(fileName);
        jassert(source.existsAsFile());
        jassert(source.copyFileTo(dir.getChildFile(fileName)));
    }

    void initializePatchManagerState(juce::ValueTree& state, int bank, int patch, bool bankLocked = false)
    {
        state.setProperty(BankUtility::StateProperties::kSelectedBank, bank, nullptr);
        state.setProperty(BankUtility::StateProperties::kBanksLocked, bankLocked, nullptr);
        state.setProperty(InternalPatches::kCurrentBankNumber, bank, nullptr);
        state.setProperty(InternalPatches::kCurrentPatchNumber, patch, nullptr);
    }
}

class PatchManagerActionHandlerTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerTests() : juce::UnitTest("PatchManagerActionHandler") {}

    void runTest() override
    {
        testPasteRomBankBlocked();
        testPasteRamBankSuccess();
        testStoreRomBankBlocked();
        testStoreRamBankSuccess();
        testInitLoadsTemplateAndBufferToApvts();
        testBankSelectMatrix1000SetBank();
        testBankSelectMatrix6NoSetBank();
        testNavigationWrapsAcrossBanks();
        testNavigationLockedWithinBankNoSetBank();
        testColdStart_firstNavigation_sendsSetBank();
        testNavigationStaleSyncedBank_sendsSetBankOnFirstClick();
        testUnlockBankSends0CHOnly();
        testBankSelectSetsBanksLockedTrue();
        testUnlockBankMatrix6NoOp();
        testOpenPersistsFolderPath();
        testOpenCancelledDoesNotPersist();
        testRescanPersistedFolderScansOnStartup();
        testRescanPersistedFolderMissingPathWarningFooter();
        testRescanPersistedFolderEmptyPathNoOp();
        testRescanPersistedFolderEmptyPathClearsStaleCache();
        testRescanPersistedFolderNoSysEx();
        testFolderPathSessionXmlRoundTrip();
        testSaveAs_writesAndRescans();
        testSave_overwritesSelectedFile();
        testSave_selectSentinelNoOp();
        testSave_unusableFolderNoOp();
        testSaveAs_cancelledNoWrite();
        testSave_noSysEx();
        testSaveAs_noSysEx();
        testSave_syncsPatchEditName();
        testLoadSelected_enqueuesSysEx();
        testLoadSelected_sentinelNoOp();
        testLoadSelected_staleScanNoOp();
        testLoadSelected_outOfRangeWarning();
        testLoadSelected_invalidFileWarning();
        testLoad_askOnceCancelRestoresModel();
        testLoad_mismatch_preferInternalFooter();
        testLoad_mismatch_preferFilenameFooter();
        testLoadAdjacent_nextWraps();
        testLoadAdjacent_previousWraps();
        testLoadAdjacent_singleFileReloads();
        testLoadAdjacent_sentinelNoOp();
        testLoadAdjacent_enqueuesSysEx();
        testLoadAdjacent_staleScanNoOp();
        testLoadAdjacent_emptyListNoOp();
    }

private:
    struct HandlerHarness
    {
        TestAudioProcessorPatchManager proc;
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper;
        Core::ClipboardService clipboard;
        SysExParser parser;
        SysExDecoder decoder;
        Core::InitTemplateLoader initLoader;
        Core::PatchInitService patchInitService;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManager midiManager;
        Core::PatchSelectionMidiSync patchSelectionMidiSync;
        Core::DeviceMemoryLimits limits;
        Core::PatchFileService patchFileService;
        Core::PatchNameSyncer patchNameSyncer;
        SysExEncoder sysExEncoder;
        std::function<juce::File()> pickFolderCallback { []() { return juce::File(); } };
        std::function<juce::File(juce::File, juce::String)> pickSaveFileCallback {
            [](juce::File, juce::String) { return juce::File(); }
        };
        Core::PatchFileNameReconciler::Picker pickReconciliationCallback;
        bool suppressPatchSysEx { false };
        bool suppressMatrixModSysEx { false };
        Core::PatchManagerActionHandler handler;

        explicit HandlerHarness(Core::DeviceMemoryLimits limitsIn)
            : mapper(proc.apvts, model)
            , decoder(parser)
            , initLoader(decoder)
            , patchInitService(model, initLoader, []() { return juce::File(); })
            , midiManager(proc.apvts, queue, tracker)
            , patchSelectionMidiSync(&midiManager)
            , limits(std::move(limitsIn))
            , patchFileService(decoder)
            , patchNameSyncer(proc.apvts, model)
            , handler(proc.apvts,
                      [this]() { return limits; },
                      &model,
                      &mapper,
                      &clipboard,
                      &patchInitService,
                      &patchSelectionMidiSync,
                      &midiManager,
                      &patchFileService,
                      &patchNameSyncer,
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
                      Core::ActionExecutionHooks{
                          [this](bool suppress) { suppressMatrixModSysEx = suppress; },
                          nullptr,
                          [this](bool suppress) { suppressPatchSysEx = suppress; },
                          nullptr })
        {
            initializePatchManagerState(proc.apvts.state, 0, 0, false);
            patchSelectionMidiSync.clearSyncedBankState();
        }
    };

    void simulateSelectPatchFileDispatch(HandlerHarness& harness)
    {
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());
    }

    void fireAdjacentNavigation(HandlerHarness& harness, const juce::String& adjacentPropertyId)
    {
        const int beforeId = static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            0));
        harness.handler.handleAction(adjacentPropertyId, juce::var());
        const int afterId = static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            0));

        if (afterId != beforeId)
            simulateSelectPatchFileDispatch(harness);
    }

    void setupComputerPatchesScan(HandlerHarness& harness, const juce::File& tempDir)
    {
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
    }

    void testPasteRomBankBlocked()
    {
        beginTest("paste_romBank_blocked");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 3, 12, false);
        harness.mapper.apvtsToBuffer();
        harness.clipboard.copyFullPatch(harness.model);

        harness.handler.handleAction(InternalPatches::kPastePatch, juce::var());

        const auto footer = harness.proc.apvts.state.getProperty("uiMessageText").toString();
        expect(footer == PluginDisplayNames::PatchManagerSection::InternalPatchesModule::kRomBankPasteStoreFooterMessage);
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "warning");
        expect(harness.queue.isEmpty());
    }

    void testPasteRamBankSuccess()
    {
        beginTest("paste_ramBank_success");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 7, false);
        harness.mapper.apvtsToBuffer();
        harness.clipboard.copyFullPatch(harness.model);

        harness.handler.handleAction(InternalPatches::kPastePatch, juce::var());

        expect(harness.clipboard.canPasteFullPatch());
        const auto queued = scanQueue(harness.queue);
        expect(queued.patchData);
    }

    void testStoreRomBankBlocked()
    {
        beginTest("store_romBank_blocked");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 5, 1, false);

        harness.handler.handleAction(InternalPatches::kStorePatch, juce::var());

        const auto footer = harness.proc.apvts.state.getProperty("uiMessageText").toString();
        expect(footer == PluginDisplayNames::PatchManagerSection::InternalPatchesModule::kRomBankPasteStoreFooterMessage);
        expect(harness.queue.isEmpty());
    }

    void testStoreRamBankSuccess()
    {
        beginTest("store_ramBank_success");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 1, 42, false);

        harness.handler.handleAction(InternalPatches::kStorePatch, juce::var());

        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 1);
        expect(queued.patchData);
    }

    void testInitLoadsTemplateAndBufferToApvts()
    {
        beginTest("initFullPatch_loadsTemplate");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        expect(!harness.suppressPatchSysEx);
        expect(!harness.suppressMatrixModSysEx);
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString().isNotEmpty());
        expect(harness.queue.isEmpty());
    }

    void testBankSelectMatrix1000SetBank()
    {
        beginTest("bankSelect_matrix1000_setBank");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 4, false);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);

        harness.handler.handleAction(BankUtility::StandaloneWidgets::kSelectBank3, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 3);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 3);
    }

    void testBankSelectMatrix6NoSetBank()
    {
        beginTest("bankSelect_matrix6_noSetBank");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix6));
        initializePatchManagerState(harness.proc.apvts.state, 0, 4, false);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);

        harness.handler.handleAction(BankUtility::StandaloneWidgets::kSelectBank3, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 0);
        expect(!static_cast<bool>(harness.proc.apvts.state.getProperty(BankUtility::StateProperties::kBanksLocked)));
        expect(harness.queue.isEmpty());
    }

    void testNavigationWrapsAcrossBanks()
    {
        beginTest("navigation_wrapsAcrossBanks");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, Matrix1000Limits::kMaxPatchNumber, false);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);

        harness.handler.handleAction(InternalPatches::kLoadNextPatch, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)),
                     Matrix1000Limits::kMinPatchNumber);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 1);
    }

    void testUnlockBankSends0CHOnly()
    {
        beginTest("unlockBank_sends0CHOnly");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 2, 5, true);

        harness.handler.handleAction(BankUtility::StandaloneWidgets::kUnlockBank, juce::var());

        expect(!static_cast<bool>(harness.proc.apvts.state.getProperty(BankUtility::StateProperties::kBanksLocked)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 2);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 5);
        const auto queued = scanQueue(harness.queue);
        expect(queued.unlockBank);
        expect(!queued.setBank);
    }

    void testBankSelectSetsBanksLockedTrue()
    {
        beginTest("bankSelect_setsBanksLockedTrue");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 4, false);

        harness.handler.handleAction(BankUtility::StandaloneWidgets::kSelectBank3, juce::var());

        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(BankUtility::StateProperties::kBanksLocked)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 3);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 3);
    }

    void testNavigationLockedWithinBankNoSetBank()
    {
        beginTest("navigation_locked_withinBank_noSetBank");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 5, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);

        harness.handler.handleAction(InternalPatches::kLoadNextPatch, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 6);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(BankUtility::StateProperties::kBanksLocked)));
        const auto queued = scanQueue(harness.queue);
        expect(!queued.setBank);
    }

    void testColdStart_firstNavigation_sendsSetBank()
    {
        beginTest("coldStart_firstNavigation_sendsSetBank");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 0, false);

        harness.handler.handleAction(InternalPatches::kLoadNextPatch, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 1);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 0);
    }

    void testNavigationStaleSyncedBank_sendsSetBankOnFirstClick()
    {
        beginTest("navigation_staleSyncedBank_sendsSetBankOnFirstClick");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 9, 93, false);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);

        harness.handler.handleAction(InternalPatches::kLoadNextPatch, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 9);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 94);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 9);
    }

    void testUnlockBankMatrix6NoOp()
    {
        beginTest("unlockBank_matrix6_noOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix6));
        initializePatchManagerState(harness.proc.apvts.state, 0, 4, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);

        harness.handler.handleAction(BankUtility::StandaloneWidgets::kUnlockBank, juce::var());

        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(BankUtility::StateProperties::kBanksLocked)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 4);
        expect(harness.queue.isEmpty());
    }

    void testOpenPersistsFolderPath()
    {
        beginTest("open_persistsFolderPath");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.pickFolderCallback = [&tempDir]() { return tempDir; };

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kOpenPatchFolder, juce::var());

        expect(harness.proc.apvts.state.getProperty(ComputerPatches::StateProperties::kFolderPath).toString()
               == tempDir.getFullPathName());
        expectEquals(harness.patchFileService.getLastScanResult().validCount, 1);

        tempDir.deleteRecursively();
    }

    void testOpenCancelledDoesNotPersist()
    {
        beginTest("open_cancelled_doesNotPersist");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const juce::String keepPath = "/tmp/keep";
        harness.proc.apvts.state.setProperty(ComputerPatches::StateProperties::kFolderPath, keepPath, nullptr);
        harness.pickFolderCallback = []() { return juce::File(); };

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kOpenPatchFolder, juce::var());

        expect(harness.proc.apvts.state.getProperty(ComputerPatches::StateProperties::kFolderPath).toString()
               == keepPath);
    }

    void testRescanPersistedFolderScansOnStartup()
    {
        beginTest("rescanPersistedFolder_scansOnStartup");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);

        harness.handler.rescanPersistedComputerPatchesFolder();

        const auto& scan = harness.patchFileService.getLastScanResult();
        expect(scan.validCount > 0);
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString().isNotEmpty());
        expect(harness.proc.apvts.state.hasProperty(ComputerPatches::StateProperties::kScanRevision));

        tempDir.deleteRecursively();
    }

    void testRescanPersistedFolderMissingPathWarningFooter()
    {
        beginTest("rescanPersistedFolder_missingPath_warningFooter");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            "/nonexistent/path",
            nullptr);

        harness.handler.rescanPersistedComputerPatchesFolder();

        const auto& scan = harness.patchFileService.getLastScanResult();
        expect(!scan.folderUsable);
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String(FooterMessages::kFolderNotFound));
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "warning");
    }

    void testRescanPersistedFolderEmptyPathNoOp()
    {
        beginTest("rescanPersistedFolder_emptyPath_noOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        harness.proc.apvts.state.setProperty(ComputerPatches::StateProperties::kFolderPath, juce::String(), nullptr);

        harness.handler.rescanPersistedComputerPatchesFolder();

        expectEquals(harness.patchFileService.getLastScanResult().validCount, 0);
        expect(!harness.proc.apvts.state.hasProperty(ComputerPatches::StateProperties::kScanRevision));
    }

    void testRescanPersistedFolderEmptyPathClearsStaleCache()
    {
        beginTest("rescanPersistedFolder_emptyPath_clearsStaleCache");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        expect(harness.patchFileService.getLastScanResult().validCount > 0);

        harness.proc.apvts.state.setProperty(ComputerPatches::StateProperties::kFolderPath, juce::String(), nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();

        expectEquals(harness.patchFileService.getLastScanResult().validCount, 0);
        expect(!harness.patchFileService.getLastScanResult().folderUsable);
        expect(harness.proc.apvts.state.hasProperty(ComputerPatches::StateProperties::kScanRevision));

        tempDir.deleteRecursively();
    }

    void testRescanPersistedFolderNoSysEx()
    {
        beginTest("rescanPersisted_noSysEx");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);

        harness.handler.rescanPersistedComputerPatchesFolder();

        const auto queued = scanQueue(harness.queue);
        expect(!queued.patchData);
        expect(harness.queue.isEmpty());

        tempDir.deleteRecursively();
    }

    void testFolderPathSessionXmlRoundTrip()
    {
        beginTest("folderPath_sessionXmlRoundTrip");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const juce::String path = "/tmp/persisted/folder";
        harness.proc.apvts.state.setProperty(ComputerPatches::StateProperties::kFolderPath, path, nullptr);

        juce::MemoryBlock destData;
        if (auto xml = harness.proc.apvts.copyState().createXml())
            juce::AudioProcessor::copyXmlToBinary(*xml, destData);

        TestAudioProcessorPatchManager restored;
        if (auto xmlState = juce::AudioProcessor::getXmlFromBinary(destData.getData(),
                                                                   static_cast<int>(destData.getSize())))
            restored.apvts.replaceState(juce::ValueTree::fromXml(*xmlState));

        expect(restored.apvts.state.getProperty(ComputerPatches::StateProperties::kFolderPath).toString() == path);
    }

    void testSaveAs_writesAndRescans()
    {
        beginTest("saveAs_writesAndRescans");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();

        const auto revisionBefore = harness.proc.apvts.state.getProperty(
            ComputerPatches::StateProperties::kScanRevision);

        harness.pickSaveFileCallback = [&tempDir](juce::File, juce::String stem) {
            return tempDir.getChildFile(stem + ".syx");
        };

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchAs, juce::var());

        const auto savedFile = tempDir.getChildFile("PATCH.syx");
        expect(savedFile.existsAsFile());
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::formatSaveSuccess(savedFile.getFileName()));
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "info");
        expect(harness.proc.apvts.state.hasProperty(ComputerPatches::StateProperties::kScanRevision));
        expect(harness.proc.apvts.state.getProperty(ComputerPatches::StateProperties::kScanRevision)
               != revisionBefore);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
            1);

        juce::MemoryBlock savedSysEx;
        expect(savedFile.loadFileAsData(savedSysEx));
        expect(harness.decoder.validatePatchSysExMessage(savedSysEx));

        tempDir.deleteRecursively();
    }

    void testSave_overwritesSelectedFile()
    {
        beginTest("save_overwritesSelectedFile");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        const auto target = tempDir.getChildFile("Patch 71.syx");
        const auto sizeBefore = target.getSize();

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        expect(target.existsAsFile());
        expect(target.getSize() > 0);
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::formatSaveSuccess(target.getFileName()));
        expectEquals(static_cast<int>(harness.patchFileService.getLastScanResult().validCount), 1);
        expect(sizeBefore > 0);

        tempDir.deleteRecursively();
    }

    void testSave_selectSentinelNoOp()
    {
        beginTest("save_selectSentinelNoOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            0,
            nullptr);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        expectEquals(tempDir.getNumberOfChildFiles(juce::File::findFiles), 0);
        expect(! harness.proc.apvts.state.getProperty("uiMessageText").toString().startsWith("Saved "));

        tempDir.deleteRecursively();
    }

    void testSaveAs_cancelledNoWrite()
    {
        beginTest("saveAs_cancelledNoWrite");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();

        const auto revisionBefore = harness.proc.apvts.state.getProperty(
            ComputerPatches::StateProperties::kScanRevision);
        const auto patchNameBefore = harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString();
        const auto selectBefore = static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile));
        harness.pickSaveFileCallback = [](juce::File, juce::String) { return juce::File(); };

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchAs, juce::var());

        expectEquals(tempDir.getNumberOfChildFiles(juce::File::findFiles), 1);
        expect(harness.proc.apvts.state.getProperty(ComputerPatches::StateProperties::kScanRevision)
               == revisionBefore);
        expect(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString() == patchNameBefore);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
            selectBefore);

        tempDir.deleteRecursively();
    }

    void testSave_noSysEx()
    {
        beginTest("save_noSysEx");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        const auto queued = scanQueue(harness.queue);
        expect(! queued.patchData);
        expect(harness.queue.isEmpty());

        tempDir.deleteRecursively();
    }

    void testSave_unusableFolderNoOp()
    {
        beginTest("save_unusableFolderNoOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        expect(tempDir.deleteRecursively());

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        expect(! harness.proc.apvts.state.getProperty("uiMessageText").toString().startsWith("Saved "));
    }

    void testSaveAs_noSysEx()
    {
        beginTest("saveAs_noSysEx");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();

        harness.pickSaveFileCallback = [&tempDir](juce::File, juce::String stem) {
            return tempDir.getChildFile(stem + ".syx");
        };

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchAs, juce::var());

        const auto queued = scanQueue(harness.queue);
        expect(! queued.patchData);
        expect(harness.queue.isEmpty());

        tempDir.deleteRecursively();
    }

    void testSave_syncsPatchEditName()
    {
        beginTest("save_syncsPatchEditName");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();

        harness.pickSaveFileCallback = [&tempDir](juce::File, juce::String) {
            return tempDir.getChildFile("MY-PATCH.syx");
        };

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchAs, juce::var());

        expect(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString() == "MY-PATCH");

        tempDir.deleteRecursively();
    }

    void testLoadSelected_enqueuesSysEx()
    {
        beginTest("loadSelected_enqueuesSysEx");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());

        const auto queued = scanQueue(harness.queue);
        expect(queued.patchData);
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "info");
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::formatReconciliationNotice("BNK2 71", false));

        tempDir.deleteRecursively();
    }

    void testLoadSelected_sentinelNoOp()
    {
        beginTest("loadSelected_sentinelNoOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            0,
            nullptr);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());

        expect(harness.queue.isEmpty());
        expect(! harness.proc.apvts.state.getProperty("uiMessageText").toString().startsWith("Loaded "));

        tempDir.deleteRecursively();
    }

    void testLoadSelected_staleScanNoOp()
    {
        beginTest("loadSelected_staleScanNoOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        expect(tempDir.deleteRecursively());

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());

        expect(harness.queue.isEmpty());
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "warning");
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::kLoadSelectionStale);

        tempDir.deleteRecursively();
    }

    void testLoadSelected_outOfRangeWarning()
    {
        beginTest("loadSelected_outOfRangeWarning");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            99,
            nullptr);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());

        expect(harness.queue.isEmpty());
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "warning");
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::kLoadSelectionStale);

        tempDir.deleteRecursively();
    }

    void testLoadSelected_invalidFileWarning()
    {
        beginTest("loadSelected_invalidFileWarning");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        const auto patchNameBefore = harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString();
        const juce::uint8 garbage[] = { 0x00, 0x01, 0x02, 0x03 };
        expect(tempDir.getChildFile("Patch 71.syx").replaceWithData(garbage, sizeof(garbage)));

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());

        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "warning");
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString().isNotEmpty());
        expect(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString() == patchNameBefore);
        expect(harness.queue.isEmpty());

        tempDir.deleteRecursively();
    }

    void testLoad_askOnceCancelRestoresModel()
    {
        beginTest("load_askOnceCancelRestoresModel");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        harness.mapper.apvtsToBuffer();
        harness.model.setName("KEEP-ME");
        harness.mapper.bufferToApvts();
        const auto patchNameBefore = harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString();

        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNameReconciliationPolicy,
            Policy::kAskOncePerLoad,
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        harness.pickReconciliationCallback =
            [](juce::String, juce::String) -> std::optional<Core::NameReconciliationChoice>
            {
                return std::nullopt;
            };

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());

        expect(harness.model.getName() == "KEEP-ME");
        expect(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString() == patchNameBefore);
        expect(harness.queue.isEmpty());

        tempDir.deleteRecursively();
    }

    void testLoad_mismatch_preferInternalFooter()
    {
        beginTest("load_mismatch_preferInternalFooter");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        Core::PatchModel exportModel;
        exportModel.loadFrom(Core::InitDefaults::patchData());
        exportModel.setName("INSIDE");
        expect(harness.patchFileService.savePatchSysExFile(
            tempDir.getChildFile("OUTSIDE.syx"),
            exportModel.data(),
            harness.sysExEncoder).success);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNameReconciliationPolicy,
            Policy::kPreferInternal,
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());

        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::formatReconciliationNotice("INSIDE", false));
        expect(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString() == "INSIDE");

        tempDir.deleteRecursively();
    }

    void testLoad_mismatch_preferFilenameFooter()
    {
        beginTest("load_mismatch_preferFilenameFooter");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        Core::PatchModel exportModel;
        exportModel.loadFrom(Core::InitDefaults::patchData());
        exportModel.setName("INSIDE");
        expect(harness.patchFileService.savePatchSysExFile(
            tempDir.getChildFile("OUTSIDE.syx"),
            exportModel.data(),
            harness.sysExEncoder).success);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNameReconciliationPolicy,
            Policy::kPreferFilename,
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());

        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::formatReconciliationNotice("OUTSIDE", true));
        expect(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString() == "OUTSIDE");

        tempDir.deleteRecursively();
    }

    void testLoadAdjacent_nextWraps()
    {
        beginTest("loadAdjacent_nextWraps");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 66.syx");
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            3,
            nullptr);

        fireAdjacentNavigation(harness, ComputerPatches::StandaloneWidgets::kLoadNextPatchFile);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     1);
        expect(scanQueue(harness.queue).patchData);

        tempDir.deleteRecursively();
    }

    void testLoadAdjacent_previousWraps()
    {
        beginTest("loadAdjacent_previousWraps");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 66.syx");
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        fireAdjacentNavigation(harness, ComputerPatches::StandaloneWidgets::kLoadPreviousPatchFile);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     3);
        expect(scanQueue(harness.queue).patchData);

        tempDir.deleteRecursively();
    }

    void testLoadAdjacent_singleFileReloads()
    {
        beginTest("loadAdjacent_singleFileReloads");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        fireAdjacentNavigation(harness, ComputerPatches::StandaloneWidgets::kLoadNextPatchFile);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     1);
        expect(scanQueue(harness.queue).patchData);

        tempDir.deleteRecursively();
    }

    void testLoadAdjacent_sentinelNoOp()
    {
        beginTest("loadAdjacent_sentinelNoOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            0,
            nullptr);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kLoadNextPatchFile, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     0);
        expect(harness.queue.isEmpty());

        tempDir.deleteRecursively();
    }

    void testLoadAdjacent_enqueuesSysEx()
    {
        beginTest("loadAdjacent_enqueuesSysEx");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        fireAdjacentNavigation(harness, ComputerPatches::StandaloneWidgets::kLoadNextPatchFile);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     2);
        expect(scanQueue(harness.queue).patchData);

        tempDir.deleteRecursively();
    }

    void testLoadAdjacent_staleScanNoOp()
    {
        beginTest("loadAdjacent_staleScanNoOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        expect(tempDir.deleteRecursively());

        harness.proc.apvts.state.setProperty("uiMessageText", juce::String(), nullptr);
        harness.proc.apvts.state.setProperty("uiMessageSeverity", juce::String(), nullptr);
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kLoadNextPatchFile, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     1);
        expect(harness.queue.isEmpty());
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString().isEmpty());
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString().isEmpty());

        tempDir.deleteRecursively();
    }

    void testLoadAdjacent_emptyListNoOp()
    {
        beginTest("loadAdjacent_emptyListNoOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        setupComputerPatchesScan(harness, tempDir);
        expectEquals(harness.patchFileService.getLastScanResult().validCount, 0);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        harness.proc.apvts.state.setProperty("uiMessageText", juce::String(), nullptr);
        harness.proc.apvts.state.setProperty("uiMessageSeverity", juce::String(), nullptr);
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kLoadNextPatchFile, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     1);
        expect(harness.queue.isEmpty());
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString().isEmpty());
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString().isEmpty());

        tempDir.deleteRecursively();
    }
};

static PatchManagerActionHandlerTests patchManagerActionHandlerTests;
