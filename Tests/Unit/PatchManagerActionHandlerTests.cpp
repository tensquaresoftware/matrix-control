#include <functional>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/Init/InitTemplateLoader.h"
#include "Core/Init/PatchInitService.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/PatchSelectionMidiSync.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/ClipboardService.h"
#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/PatchFileService.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace PatchManager = PluginIDs::PatchManagerSection;
namespace InternalPatches = PatchManager::InternalPatchesModule::StandaloneWidgets;
namespace BankUtility = PatchManager::BankUtilityModule;
namespace ComputerPatches = PatchManager::ComputerPatchesModule;
namespace FooterMessages = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages;

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
    bool sysExMessageHasOpcode(const juce::MemoryBlock& block, juce::uint8 opcode, int expectedDataByte = -1)
    {
        if (block.getSize() < 5)
            return false;

        const auto* data = static_cast<const juce::uint8*>(block.getData());
        if (data[0] != SysExConstants::kSysExStart
            || data[1] != SysExConstants::kManufacturerIdOberheim
            || data[2] != SysExConstants::kDeviceIdMatrix1000
            || data[3] != opcode)
        {
            return false;
        }

        if (expectedDataByte >= 0)
        {
            if (block.getSize() < 6 || data[4] != static_cast<juce::uint8>(expectedDataByte))
                return false;
        }

        return data[block.getSize() - 1] == SysExConstants::kSysExEnd;
    }

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
        std::function<juce::File()> pickFolderCallback { []() { return juce::File(); } };
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
            , handler(proc.apvts,
                      [this]() { return limits; },
                      &model,
                      &mapper,
                      &clipboard,
                      &patchInitService,
                      &patchSelectionMidiSync,
                      &midiManager,
                      &patchFileService,
                      [this]() { return pickFolderCallback(); },
                      Core::ActionExecutionHooks{
                          [this](bool suppress) { suppressMatrixModSysEx = suppress; },
                          nullptr,
                          [this](bool suppress) { suppressPatchSysEx = suppress; } })
        {
            initializePatchManagerState(proc.apvts.state, 0, 0, false);
            patchSelectionMidiSync.resetLastSyncedBank(0);
        }
    };

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
};

static PatchManagerActionHandlerTests patchManagerActionHandlerTests;
