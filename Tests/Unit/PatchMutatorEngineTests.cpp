#include <cstring>
#include <functional>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Factories/ApvtsFactory.h"
#include "Core/Init/InitDefaults.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/PatchMutator/MutationHistoryStore.h"
#include "Core/Services/PatchMutator/MutationNaming.h"
#include "Core/Services/PatchMutator/PatchMutatorEngine.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace PatchMutator = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;
namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;
namespace MutatorDisplayNames = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;

class TestAudioProcessorMutator : public juce::AudioProcessor
{
public:
    TestAudioProcessorMutator()
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, nullptr, "P", ApvtsFactory::createParameterLayout())
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

class PatchMutatorEngineTests : public juce::UnitTest
{
public:
    PatchMutatorEngineTests() : juce::UnitTest("PatchMutatorEngine") {}

    void runTest() override
    {
        mutate_emptyHistory_createsM00();
        mutate_gapAllocation();
        mutate_limitBlocks();
        mutate_noOpRecipe_blocked();
        mutate_fromAuditionedRetry();
        mutate_sendsSysExOnce();
        mutate_neverDeletesRoots();

        retry_emptyHistory_blocked();
        retry_firstRetry_createsR00();
        retry_usesParentSnapshot_notResult();
        retry_fromSelectedRetry_usesThatEntryParentSnapshot();
        retry_gapAllocation();
        retry_limitBlocks();
        retry_noOpRecipe_blocked();
        retry_sendsSysExOnce();
        retry_neverDeletesExistingRetries();
        retry_staysUnderSameRoot();

        sync_emptyHistory_emptySentinel();
        sync_afterInsertRoot_listsAndSelectsNewRoot();
        sync_sortedRoots_numericOrder();
        sync_retryListForSelectedRoot();
        sync_changingSelectedM_rebuildsRList();
        applySelectionFromApvts_drivesAudition();
        mutate_success_updatesApvtsHistory();
        retry_success_updatesApvtsHistory();

        audition_emptyHistory_noSysEx();
        audition_selectedRoot_sendsSysExOnce();
        audition_selectedRetry_sendsSysExOnce();
        audition_idempotent_skipsDuplicateSysEx();
        audition_compareActive_noSysEx();

        mutate_firstRoot_capturesInitialSnapshot();
        mutate_secondRoot_doesNotOverwriteInitialSnapshot();
        compare_emptyHistory_blocked();
        compare_enter_auditionsInitialSnapshot();
        compare_enter_setsCompareActive();
        compare_exit_restoresSelection();
        compare_exit_auditionsRestoredEntry();
        compare_auditionBlockedWhileActive();

        delete_emptyHistory_blocked();
        delete_noSelection_blocked();
        delete_retry_removesSingleEntry();
        delete_root_cascadesRetries();
        delete_retry_selectionMovesToPrevious();
        delete_retry_firstRetry_fallsBackToRootOnly();
        delete_root_selectionMovesToPreviousRoot();
        delete_lastEntry_emptyHistory_auditionsInitialSnapshot();
        delete_lastEntry_disablesCompare();
        delete_whileCompareActive_noStaleRestore();
        delete_syncsApvtsAfterSuccess();
        clear_purgesHistory_emptySentinel();
        clear_disablesCompare();
        clear_auditionsInitialSnapshot();
        clear_keepsInitialSnapshot();
    }

private:
    struct EngineHarness
    {
        TestAudioProcessorMutator proc;
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper;
        Core::PatchNameSyncer patchNameSyncer;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManager midiManager;
        bool suppressPatchSysEx { false };
        bool suppressMatrixModSysEx { false };
        int currentPatchNumber { 0 };
        Core::PatchMutatorEngine engine;

        EngineHarness()
            : mapper(proc.apvts, model)
            , patchNameSyncer(proc.apvts, model)
            , midiManager(proc.apvts, queue, tracker)
            , engine(&model,
                     &mapper,
                     &patchNameSyncer,
                     &midiManager,
                     proc.apvts,
                     Core::ActionExecutionHooks{
                         [this](bool suppress) { suppressMatrixModSysEx = suppress; },
                         nullptr,
                         [this](bool suppress) { suppressPatchSysEx = suppress; },
                         nullptr },
                     [this]() { return currentPatchNumber; })
        {
            model.loadFrom(Core::InitDefaults::patchData());
        }

        void setRecipe(int amount, int random, bool enableDco1 = true)
        {
            proc.apvts.state.setProperty(PatchMutator::kAmount, amount, nullptr);
            proc.apvts.state.setProperty(PatchMutator::kRandom, random, nullptr);
            proc.apvts.state.setProperty(PatchMutator::kEnableDco1, enableDco1, nullptr);
            proc.apvts.state.setProperty(PatchMutator::kEnableDco2, false, nullptr);
            proc.apvts.state.setProperty(PatchMutator::kEnableVcfVca, false, nullptr);
            proc.apvts.state.setProperty(PatchMutator::kEnableFmTrack, false, nullptr);
            proc.apvts.state.setProperty(PatchMutator::kEnableRampPortamento, false, nullptr);
            proc.apvts.state.setProperty(PatchMutator::kEnableEnvelope1, false, nullptr);
            proc.apvts.state.setProperty(PatchMutator::kEnableEnvelope2, false, nullptr);
            proc.apvts.state.setProperty(PatchMutator::kEnableEnvelope3, false, nullptr);
            proc.apvts.state.setProperty(PatchMutator::kEnableLfo1, false, nullptr);
            proc.apvts.state.setProperty(PatchMutator::kEnableLfo2, false, nullptr);
        }

        Core::MutationHistoryStore& store() { return engine.historyStore_; }
    };

    static int countPatchSysExMessages(Core::MidiOutboundQueue& queue)
    {
        int count = 0;

        while (! queue.isEmpty())
        {
            const auto msg = queue.dequeue();
            if (! msg.has_value())
                break;

            if (msg->category != Core::MidiOutboundQueue::MessageCategory::kSysEx)
                continue;

            const auto& block = msg->sysExData;
            if (block.getSize() < 4)
                continue;

            const auto* data = static_cast<const juce::uint8*>(block.getData());
            if (data[3] == SysExConstants::Opcode::kSinglePatchData)
                ++count;
        }

        return count;
    }

    static Core::PatchModel makeDistinctBuffer(int seed)
    {
        Core::PatchModel patch;
        const auto marker = static_cast<juce::uint8>(seed & 0xFF);
        std::memset(patch.data(), marker, Core::PatchModel::kBufferSize);
        patch.data()[8] = marker;
        Core::MutationNaming::applyPatchName(patch, 0);
        return patch;
    }

    void mutate_emptyHistory_createsM00()
    {
        beginTest("mutate_emptyHistory_createsM00");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto result = harness.engine.mutate();
        expect(result.success);
        expectEquals(harness.engine.rootCount(), 1);

        const auto entry = harness.engine.getEntry(0);
        expect(entry.has_value());
        expect(entry->result[0] == static_cast<juce::uint8>('M'));
        expect(entry->result[1] == static_cast<juce::uint8>('0'));
        expect(entry->result[2] == static_cast<juce::uint8>('0'));
    }

    void mutate_gapAllocation()
    {
        beginTest("mutate_gapAllocation");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto m00 = makeDistinctBuffer(1);
        const auto parent = makeDistinctBuffer(2);
        expect(harness.store().insertRoot(0, m00, parent));
        expect(harness.store().deleteRoot(0));
        expect(harness.store().peekNextRootIndex().value_or(-1) == 0);

        const auto result = harness.engine.mutate();
        expect(result.success);
        expect(harness.store().hasRoot(0));
    }

    void mutate_limitBlocks()
    {
        beginTest("mutate_limitBlocks");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        for (int i = 0; i < Core::MutationHistoryStore::kMaxRoots; ++i)
        {
            const auto resultPatch = makeDistinctBuffer(i + 10);
            const auto parentPatch = makeDistinctBuffer(i + 110);
            expect(harness.store().insertRoot(i, resultPatch, parentPatch));
        }

        const auto result = harness.engine.mutate();
        expect(! result.success);
        expect(result.defragModalRequested);
    }

    void mutate_noOpRecipe_blocked()
    {
        beginTest("mutate_noOpRecipe_blocked");

        EngineHarness harness;
        harness.setRecipe(0, 100, true);

        const auto result = harness.engine.mutate();
        expect(! result.success);
        expectEquals(harness.engine.rootCount(), 0);
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void mutate_fromAuditionedRetry()
    {
        beginTest("mutate_fromAuditionedRetry");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(11);
        auto m00Parent = makeDistinctBuffer(12);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto retryResult = makeDistinctBuffer(21);
        retryResult.data()[9] = static_cast<juce::uint8>(0xAB);
        Core::MutationNaming::applyPatchName(retryResult, 0, 0);
        expect(harness.store().insertRetry(0, 0, retryResult, m00Parent));

        harness.engine.setAuditionSelection(0, 0);

        const auto result = harness.engine.mutate();
        expect(result.success);
        expect(harness.store().hasRoot(1));

        const auto parentEntry = harness.store().getEntry(1);
        expect(parentEntry.has_value());
        expectEquals(static_cast<int>(parentEntry->parentSnapshot[9]), static_cast<int>(retryResult.data()[9]));
    }

    void mutate_sendsSysExOnce()
    {
        beginTest("mutate_sendsSysExOnce");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto result = harness.engine.mutate();
        expect(result.success);
        expectEquals(countPatchSysExMessages(harness.queue), 1);
    }

    void mutate_neverDeletesRoots()
    {
        beginTest("mutate_neverDeletesRoots");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto m00 = makeDistinctBuffer(31);
        const auto m02 = makeDistinctBuffer(32);
        const auto parent = makeDistinctBuffer(33);
        expect(harness.store().insertRoot(0, m00, parent));
        expect(harness.store().insertRoot(2, m02, parent));

        const auto result = harness.engine.mutate();
        expect(result.success);
        expect(harness.store().hasRoot(0));
        expect(harness.store().hasRoot(2));
    }

    void retry_emptyHistory_blocked()
    {
        beginTest("retry_emptyHistory_blocked");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto result = harness.engine.retry();
        expect(! result.success);
        expectEquals(harness.engine.rootCount(), 0);
        expectEquals(harness.store().retryCount(0), 0);
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void retry_firstRetry_createsR00()
    {
        beginTest("retry_firstRetry_createsR00");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(41);
        auto m00Parent = makeDistinctBuffer(42);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        const auto result = harness.engine.retry();
        expect(result.success);
        expect(harness.store().hasRetry(0, 0));

        const auto retryEntry = harness.store().getEntry(0, 0);
        expect(retryEntry.has_value());
        expect(retryEntry->result[0] == static_cast<juce::uint8>('M'));
        expect(retryEntry->result[1] == static_cast<juce::uint8>('0'));
        expect(retryEntry->result[2] == static_cast<juce::uint8>('0'));
        expect(retryEntry->result[3] == static_cast<juce::uint8>('-'));
        expect(retryEntry->result[4] == static_cast<juce::uint8>('R'));
        expect(retryEntry->result[5] == static_cast<juce::uint8>('0'));
        expect(retryEntry->result[6] == static_cast<juce::uint8>('0'));
    }

    void retry_usesParentSnapshot_notResult()
    {
        beginTest("retry_usesParentSnapshot_notResult");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(51);
        auto m00Parent = makeDistinctBuffer(52);
        m00Parent.data()[8] = static_cast<juce::uint8>(0xAA);
        m00.data()[8] = static_cast<juce::uint8>(0xBB);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        const auto result = harness.engine.retry();
        expect(result.success);

        const auto retryEntry = harness.store().getEntry(0, 0);
        expect(retryEntry.has_value());
        expectEquals(static_cast<int>(retryEntry->parentSnapshot[8]), 0xAA);
    }

    void retry_fromSelectedRetry_usesThatEntryParentSnapshot()
    {
        beginTest("retry_fromSelectedRetry_usesThatEntryParentSnapshot");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(61);
        auto rootParent = makeDistinctBuffer(62);
        rootParent.data()[8] = static_cast<juce::uint8>(0x11);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, rootParent));

        auto retryResult = makeDistinctBuffer(63);
        auto retryParent = makeDistinctBuffer(64);
        retryParent.data()[8] = static_cast<juce::uint8>(0x22);
        Core::MutationNaming::applyPatchName(retryResult, 0, 0);
        expect(harness.store().insertRetry(0, 0, retryResult, retryParent));

        harness.engine.setAuditionSelection(0, 0);

        const auto result = harness.engine.retry();
        expect(result.success);

        const auto newRetry = harness.store().getEntry(0, 1);
        expect(newRetry.has_value());
        expectEquals(static_cast<int>(newRetry->parentSnapshot[8]), 0x22);
    }

    void retry_gapAllocation()
    {
        beginTest("retry_gapAllocation");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(71);
        auto m00Parent = makeDistinctBuffer(72);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto retryResult = makeDistinctBuffer(73);
        Core::MutationNaming::applyPatchName(retryResult, 0, 0);
        expect(harness.store().insertRetry(0, 0, retryResult, m00Parent));
        expect(harness.store().deleteRetry(0, 0));
        expect(harness.store().peekNextRetryIndex(0).value_or(-1) == 0);

        const auto result = harness.engine.retry();
        expect(result.success);
        expect(harness.store().hasRetry(0, 0));
    }

    void retry_limitBlocks()
    {
        beginTest("retry_limitBlocks");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(81);
        auto m00Parent = makeDistinctBuffer(82);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        for (int i = 0; i < Core::MutationHistoryStore::kMaxRetriesPerRoot; ++i)
        {
            auto resultPatch = makeDistinctBuffer(100 + i);
            const auto parentPatch = makeDistinctBuffer(200 + i);
            Core::MutationNaming::applyPatchName(resultPatch, 0, i);
            expect(harness.store().insertRetry(0, i, resultPatch, parentPatch));
        }

        const auto result = harness.engine.retry();
        expect(! result.success);
        expect(result.defragModalRequested);
    }

    void retry_noOpRecipe_blocked()
    {
        beginTest("retry_noOpRecipe_blocked");

        EngineHarness harness;
        harness.setRecipe(0, 100, true);

        auto m00 = makeDistinctBuffer(91);
        auto m00Parent = makeDistinctBuffer(92);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        const auto result = harness.engine.retry();
        expect(! result.success);
        expectEquals(harness.store().retryCount(0), 0);
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void retry_sendsSysExOnce()
    {
        beginTest("retry_sendsSysExOnce");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(101);
        auto m00Parent = makeDistinctBuffer(102);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        const auto result = harness.engine.retry();
        expect(result.success);
        expectEquals(countPatchSysExMessages(harness.queue), 1);
    }

    void retry_neverDeletesExistingRetries()
    {
        beginTest("retry_neverDeletesExistingRetries");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(111);
        auto m00Parent = makeDistinctBuffer(112);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto r00 = makeDistinctBuffer(113);
        Core::MutationNaming::applyPatchName(r00, 0, 0);
        expect(harness.store().insertRetry(0, 0, r00, m00Parent));

        auto r02 = makeDistinctBuffer(114);
        Core::MutationNaming::applyPatchName(r02, 0, 2);
        expect(harness.store().insertRetry(0, 2, r02, m00Parent));

        const auto result = harness.engine.retry();
        expect(result.success);
        expect(harness.store().hasRetry(0, 0));
        expect(harness.store().hasRetry(0, 2));
    }

    void retry_staysUnderSameRoot()
    {
        beginTest("retry_staysUnderSameRoot");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(121);
        auto m00Parent = makeDistinctBuffer(122);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        const int rootsBefore = harness.engine.rootCount();
        expect(harness.store().hasRoot(0));

        const auto result = harness.engine.retry();
        expect(result.success);
        expectEquals(harness.engine.rootCount(), rootsBefore);
        expect(harness.store().hasRoot(0));
    }

    void sync_emptyHistory_emptySentinel()
    {
        beginTest("sync_emptyHistory_emptySentinel");

        EngineHarness harness;
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);

        expect(harness.proc.apvts.state.getProperty(MutatorState::kHistoryMList).toString().isEmpty());
        expect(harness.proc.apvts.state.getProperty(MutatorState::kHistoryRList).toString().isEmpty());
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedM)), -1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedR)),
                     Core::MutationHistoryStore::kRootOnly);
    }

    void sync_afterInsertRoot_listsAndSelectsNewRoot()
    {
        beginTest("sync_afterInsertRoot_listsAndSelectsNewRoot");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto result = harness.engine.mutate();
        expect(result.success);

        const auto mList = harness.proc.apvts.state.getProperty(MutatorState::kHistoryMList).toString();
        expect(mList.contains("M00"));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedM)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedR)),
                     Core::MutationHistoryStore::kRootOnly);
    }

    void sync_sortedRoots_numericOrder()
    {
        beginTest("sync_sortedRoots_numericOrder");

        EngineHarness harness;

        const auto parent = makeDistinctBuffer(201);
        expect(harness.store().insertRoot(5, makeDistinctBuffer(205), parent));
        expect(harness.store().insertRoot(0, makeDistinctBuffer(200), parent));
        expect(harness.store().insertRoot(2, makeDistinctBuffer(202), parent));

        harness.engine.setAuditionSelection(2, Core::MutationHistoryStore::kRootOnly);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);

        expectEquals(harness.proc.apvts.state.getProperty(MutatorState::kHistoryMList).toString(),
                     juce::String("M00|M02|M05"));
    }

    void sync_retryListForSelectedRoot()
    {
        beginTest("sync_retryListForSelectedRoot");

        EngineHarness harness;

        const auto parent = makeDistinctBuffer(301);
        auto m05 = makeDistinctBuffer(305);
        Core::MutationNaming::applyPatchName(m05, 5);
        expect(harness.store().insertRoot(5, m05, parent));

        auto r00 = makeDistinctBuffer(310);
        Core::MutationNaming::applyPatchName(r00, 5, 0);
        expect(harness.store().insertRetry(5, 0, r00, parent));

        auto r02 = makeDistinctBuffer(312);
        Core::MutationNaming::applyPatchName(r02, 5, 2);
        expect(harness.store().insertRetry(5, 2, r02, parent));

        harness.engine.setAuditionSelection(5, Core::MutationHistoryStore::kRootOnly);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);

        const auto rList = harness.proc.apvts.state.getProperty(MutatorState::kHistoryRList).toString();
        const auto expectedRList = MutatorDisplayNames::kHistoryRootSentinel + "|R00|R02";
        expectEquals(rList, expectedRList);
    }

    void sync_changingSelectedM_rebuildsRList()
    {
        beginTest("sync_changingSelectedM_rebuildsRList");

        EngineHarness harness;

        const auto parent = makeDistinctBuffer(401);
        expect(harness.store().insertRoot(2, makeDistinctBuffer(402), parent));
        expect(harness.store().insertRoot(5, makeDistinctBuffer(405), parent));

        auto rOnM02 = makeDistinctBuffer(412);
        Core::MutationNaming::applyPatchName(rOnM02, 2, 1);
        expect(harness.store().insertRetry(2, 1, rOnM02, parent));

        auto rOnM05 = makeDistinctBuffer(415);
        Core::MutationNaming::applyPatchName(rOnM05, 5, 3);
        expect(harness.store().insertRetry(5, 3, rOnM05, parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 2, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR,
                                             MutatorState::kSelectedRRootOnly,
                                             nullptr);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);
        const auto rListM02 = harness.proc.apvts.state.getProperty(MutatorState::kHistoryRList).toString();

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 5, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR,
                                             MutatorState::kSelectedRRootOnly,
                                             nullptr);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);
        const auto rListM05 = harness.proc.apvts.state.getProperty(MutatorState::kHistoryRList).toString();

        expect(rListM02.contains("R01"));
        expect(rListM05.contains("R03"));
        expect(rListM02 != rListM05);
    }

    void applySelectionFromApvts_drivesAudition()
    {
        beginTest("applySelectionFromApvts_drivesAudition");

        EngineHarness harness;

        const auto parent = makeDistinctBuffer(501);
        auto m02 = makeDistinctBuffer(502);
        Core::MutationNaming::applyPatchName(m02, 2);
        expect(harness.store().insertRoot(2, m02, parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 2, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);
        harness.engine.applySelectionFromApvts();

        const auto audition = harness.engine.resolveAuditionBuffer();
        expect(std::memcmp(audition.data(), m02.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void mutate_success_updatesApvtsHistory()
    {
        beginTest("mutate_success_updatesApvtsHistory");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto result = harness.engine.mutate();
        expect(result.success);
        expect(! harness.proc.apvts.state.getProperty(MutatorState::kHistoryMList).toString().isEmpty());
    }

    void retry_success_updatesApvtsHistory()
    {
        beginTest("retry_success_updatesApvtsHistory");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(601);
        auto m00Parent = makeDistinctBuffer(602);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);

        const auto result = harness.engine.retry();
        expect(result.success);

        const auto rList = harness.proc.apvts.state.getProperty(MutatorState::kHistoryRList).toString();
        expect(rList.contains("R00"));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedR)), 0);
    }

    void audition_emptyHistory_noSysEx()
    {
        beginTest("audition_emptyHistory_noSysEx");

        EngineHarness harness;
        harness.engine.auditionSelectedHistoryEntry();
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void audition_selectedRoot_sendsSysExOnce()
    {
        beginTest("audition_selectedRoot_sendsSysExOnce");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(701);
        auto m00Parent = makeDistinctBuffer(702);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);

        harness.engine.auditionSelectedHistoryEntry();

        expectEquals(countPatchSysExMessages(harness.queue), 1);
        expect(std::memcmp(harness.model.data(), m00.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void audition_selectedRetry_sendsSysExOnce()
    {
        beginTest("audition_selectedRetry_sendsSysExOnce");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(711);
        auto m00Parent = makeDistinctBuffer(712);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto r00 = makeDistinctBuffer(713);
        Core::MutationNaming::applyPatchName(r00, 0, 0);
        expect(harness.store().insertRetry(0, 0, r00, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR, 0, nullptr);

        harness.engine.auditionSelectedHistoryEntry();

        expectEquals(countPatchSysExMessages(harness.queue), 1);
        expect(std::memcmp(harness.model.data(), r00.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void audition_idempotent_skipsDuplicateSysEx()
    {
        beginTest("audition_idempotent_skipsDuplicateSysEx");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(721);
        auto m00Parent = makeDistinctBuffer(722);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);

        harness.engine.auditionSelectedHistoryEntry();
        expectEquals(countPatchSysExMessages(harness.queue), 1);

        harness.engine.auditionSelectedHistoryEntry();
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void audition_compareActive_noSysEx()
    {
        beginTest("audition_compareActive_noSysEx");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(731);
        auto m00Parent = makeDistinctBuffer(732);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kCompareActive, true, nullptr);

        harness.engine.auditionSelectedHistoryEntry();
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void mutate_firstRoot_capturesInitialSnapshot()
    {
        beginTest("mutate_firstRoot_capturesInitialSnapshot");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto preMutateBuffer = makeDistinctBuffer(801);
        std::memcpy(harness.model.data(), preMutateBuffer.data(), Core::PatchModel::kBufferSize);

        const auto result = harness.engine.mutate();
        expect(result.success);
        expect(harness.store().hasInitialSnapshot());

        const auto snapshot = harness.store().getInitialSnapshot();
        expect(std::memcmp(snapshot.data(), preMutateBuffer.data(), Core::PatchModel::kBufferSize) == 0);

        const auto entry = harness.engine.getEntry(0);
        expect(entry.has_value());
        expect(std::memcmp(entry->result.data(), preMutateBuffer.data(), Core::PatchModel::kBufferSize) != 0);
    }

    void mutate_secondRoot_doesNotOverwriteInitialSnapshot()
    {
        beginTest("mutate_secondRoot_doesNotOverwriteInitialSnapshot");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto preFirstMutate = makeDistinctBuffer(811);
        std::memcpy(harness.model.data(), preFirstMutate.data(), Core::PatchModel::kBufferSize);

        expect(harness.engine.mutate().success);
        const auto firstSnapshot = harness.store().getInitialSnapshot();

        expect(harness.engine.mutate().success);
        const auto secondSnapshot = harness.store().getInitialSnapshot();
        expect(std::memcmp(firstSnapshot.data(), secondSnapshot.data(), Core::PatchModel::kBufferSize) == 0);
        expect(std::memcmp(secondSnapshot.data(), preFirstMutate.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void compare_emptyHistory_blocked()
    {
        beginTest("compare_emptyHistory_blocked");

        EngineHarness harness;

        const auto result = harness.engine.toggleCompare();
        expect(! result.success);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void compare_enter_auditionsInitialSnapshot()
    {
        beginTest("compare_enter_auditionsInitialSnapshot");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto preMutateBuffer = makeDistinctBuffer(821);
        std::memcpy(harness.model.data(), preMutateBuffer.data(), Core::PatchModel::kBufferSize);

        expect(harness.engine.mutate().success);
        expectEquals(countPatchSysExMessages(harness.queue), 1);

        const auto result = harness.engine.toggleCompare();
        expect(result.success);
        expectEquals(countPatchSysExMessages(harness.queue), 1);
        expect(std::memcmp(harness.model.data(), preMutateBuffer.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void compare_enter_setsCompareActive()
    {
        beginTest("compare_enter_setsCompareActive");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        expect(harness.engine.mutate().success);

        const auto result = harness.engine.toggleCompare();
        expect(result.success);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
    }

    void compare_exit_restoresSelection()
    {
        beginTest("compare_exit_restoresSelection");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(831);
        auto m00Parent = makeDistinctBuffer(832);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        harness.store().setInitialSnapshot(m00Parent);

        auto r00 = makeDistinctBuffer(833);
        Core::MutationNaming::applyPatchName(r00, 0, 0);
        expect(harness.store().insertRetry(0, 0, r00, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR, 0, nullptr);
        harness.engine.applySelectionFromApvts();

        expect(harness.engine.toggleCompare().success);
        expect(harness.engine.toggleCompare().success);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedM)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedR)), 0);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
    }

    void compare_exit_auditionsRestoredEntry()
    {
        beginTest("compare_exit_auditionsRestoredEntry");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(841);
        auto m00Parent = makeDistinctBuffer(842);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        harness.store().setInitialSnapshot(m00Parent);

        auto r00 = makeDistinctBuffer(843);
        Core::MutationNaming::applyPatchName(r00, 0, 0);
        expect(harness.store().insertRetry(0, 0, r00, m00Parent));

        std::memcpy(harness.model.data(), r00.data(), Core::PatchModel::kBufferSize);

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR, 0, nullptr);
        harness.engine.applySelectionFromApvts();

        expect(harness.engine.toggleCompare().success);
        expect(std::memcmp(harness.model.data(), m00Parent.data(), Core::PatchModel::kBufferSize) == 0);

        expect(harness.engine.toggleCompare().success);
        expect(std::memcmp(harness.model.data(), r00.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void compare_auditionBlockedWhileActive()
    {
        beginTest("compare_auditionBlockedWhileActive");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(851);
        auto m00Parent = makeDistinctBuffer(852);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        harness.store().setInitialSnapshot(m00Parent);

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);

        expect(harness.engine.toggleCompare().success);
        expectEquals(countPatchSysExMessages(harness.queue), 1);

        harness.engine.auditionSelectedHistoryEntry();
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void delete_emptyHistory_blocked()
    {
        beginTest("delete_emptyHistory_blocked");

        EngineHarness harness;

        const auto result = harness.engine.deleteSelected();
        expect(! result.success);
        expectEquals(result.footerMessage, juce::String("Mutation history is empty."));
        expectEquals(result.footerSeverity, juce::String("warning"));
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void delete_noSelection_blocked()
    {
        beginTest("delete_noSelection_blocked");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(901);
        auto m00Parent = makeDistinctBuffer(902);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, -1, nullptr);

        const auto result = harness.engine.deleteSelected();
        expect(! result.success);
        expectEquals(result.footerMessage, juce::String("No valid mutation history entry selected."));
        expectEquals(result.footerSeverity, juce::String("warning"));
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void delete_retry_removesSingleEntry()
    {
        beginTest("delete_retry_removesSingleEntry");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(911);
        auto m00Parent = makeDistinctBuffer(912);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto r00 = makeDistinctBuffer(913);
        Core::MutationNaming::applyPatchName(r00, 0, 0);
        expect(harness.store().insertRetry(0, 0, r00, m00Parent));

        auto r02 = makeDistinctBuffer(914);
        Core::MutationNaming::applyPatchName(r02, 0, 2);
        expect(harness.store().insertRetry(0, 2, r02, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR, 2, nullptr);

        expect(harness.engine.deleteSelected().success);
        expect(! harness.store().hasRetry(0, 2));
        expect(harness.store().hasRetry(0, 0));
        expect(! harness.store().hasRetry(0, 1));
    }

    void delete_root_cascadesRetries()
    {
        beginTest("delete_root_cascadesRetries");

        EngineHarness harness;

        auto m05 = makeDistinctBuffer(921);
        auto m05Parent = makeDistinctBuffer(922);
        Core::MutationNaming::applyPatchName(m05, 5);
        expect(harness.store().insertRoot(5, m05, m05Parent));

        auto r00 = makeDistinctBuffer(923);
        Core::MutationNaming::applyPatchName(r00, 5, 0);
        expect(harness.store().insertRetry(5, 0, r00, m05Parent));

        auto r01 = makeDistinctBuffer(924);
        Core::MutationNaming::applyPatchName(r01, 5, 1);
        expect(harness.store().insertRetry(5, 1, r01, m05Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 5, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);

        const auto result = harness.engine.deleteSelected();
        expect(result.success);
        expect(result.footerMessage.isNotEmpty());
        expectEquals(result.footerSeverity, juce::String("info"));
        expect(! harness.store().hasRoot(5));
        expectEquals(harness.store().retryCount(5), 0);
    }

    void delete_retry_selectionMovesToPrevious()
    {
        beginTest("delete_retry_selectionMovesToPrevious");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(931);
        auto m00Parent = makeDistinctBuffer(932);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto r00 = makeDistinctBuffer(933);
        Core::MutationNaming::applyPatchName(r00, 0, 0);
        expect(harness.store().insertRetry(0, 0, r00, m00Parent));

        auto r02 = makeDistinctBuffer(934);
        Core::MutationNaming::applyPatchName(r02, 0, 2);
        expect(harness.store().insertRetry(0, 2, r02, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR, 2, nullptr);

        std::memcpy(harness.model.data(), r02.data(), Core::PatchModel::kBufferSize);

        expect(harness.engine.deleteSelected().success);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedR)), 0);
        expectEquals(countPatchSysExMessages(harness.queue), 1);
        expect(std::memcmp(harness.model.data(), r00.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void delete_retry_firstRetry_fallsBackToRootOnly()
    {
        beginTest("delete_retry_firstRetry_fallsBackToRootOnly");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(941);
        auto m00Parent = makeDistinctBuffer(942);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto r00 = makeDistinctBuffer(943);
        Core::MutationNaming::applyPatchName(r00, 0, 0);
        expect(harness.store().insertRetry(0, 0, r00, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR, 0, nullptr);

        expect(harness.engine.deleteSelected().success);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedR)),
                     Core::MutationHistoryStore::kRootOnly);
    }

    void delete_root_selectionMovesToPreviousRoot()
    {
        beginTest("delete_root_selectionMovesToPreviousRoot");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(951);
        auto m00Parent = makeDistinctBuffer(952);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto m05 = makeDistinctBuffer(953);
        auto m05Parent = makeDistinctBuffer(954);
        Core::MutationNaming::applyPatchName(m05, 5);
        expect(harness.store().insertRoot(5, m05, m05Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 5, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);

        expect(harness.engine.deleteSelected().success);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedM)), 0);
    }

    void delete_lastEntry_emptyHistory_auditionsInitialSnapshot()
    {
        beginTest("delete_lastEntry_emptyHistory_auditionsInitialSnapshot");

        EngineHarness harness;

        const auto snapshot = makeDistinctBuffer(961);
        harness.store().setInitialSnapshot(snapshot);

        auto m00 = makeDistinctBuffer(962);
        auto m00Parent = makeDistinctBuffer(963);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        std::memcpy(harness.model.data(), m00.data(), Core::PatchModel::kBufferSize);

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);

        expect(harness.engine.deleteSelected().success);
        expect(harness.store().isEmpty());
        expect(harness.proc.apvts.state.getProperty(MutatorState::kHistoryMList).toString().isEmpty());
        expectEquals(countPatchSysExMessages(harness.queue), 1);
        expect(std::memcmp(harness.model.data(), snapshot.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void delete_lastEntry_disablesCompare()
    {
        beginTest("delete_lastEntry_disablesCompare");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(971);
        auto m00Parent = makeDistinctBuffer(972);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        harness.store().setInitialSnapshot(m00Parent);

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kCompareActive, true, nullptr);

        expect(harness.engine.deleteSelected().success);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
    }

    void delete_whileCompareActive_noStaleRestore()
    {
        beginTest("delete_whileCompareActive_noStaleRestore");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(981);
        auto m00Parent = makeDistinctBuffer(982);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto m05 = makeDistinctBuffer(983);
        auto m05Parent = makeDistinctBuffer(984);
        Core::MutationNaming::applyPatchName(m05, 5);
        expect(harness.store().insertRoot(5, m05, m05Parent));
        harness.store().setInitialSnapshot(m00Parent);

        auto r00 = makeDistinctBuffer(985);
        Core::MutationNaming::applyPatchName(r00, 5, 0);
        expect(harness.store().insertRetry(5, 0, r00, m05Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 5, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);
        harness.engine.applySelectionFromApvts();

        expect(harness.engine.toggleCompare().success);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));

        expect(harness.engine.deleteSelected().success);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedM)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedR)),
                     Core::MutationHistoryStore::kRootOnly);
    }

    void delete_syncsApvtsAfterSuccess()
    {
        beginTest("delete_syncsApvtsAfterSuccess");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(991);
        auto m00Parent = makeDistinctBuffer(992);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto m05 = makeDistinctBuffer(993);
        auto m05Parent = makeDistinctBuffer(994);
        Core::MutationNaming::applyPatchName(m05, 5);
        expect(harness.store().insertRoot(5, m05, m05Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 5, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedR,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);

        const auto listBefore = harness.proc.apvts.state.getProperty(MutatorState::kHistoryMList).toString();
        expect(listBefore.contains("M00"));
        expect(listBefore.contains("M05"));

        expect(harness.engine.deleteSelected().success);

        const auto listAfter = harness.proc.apvts.state.getProperty(MutatorState::kHistoryMList).toString();
        expect(listAfter.contains("M00"));
        expect(! listAfter.contains("M05"));

        const auto rListAfter = harness.proc.apvts.state.getProperty(MutatorState::kHistoryRList).toString();
        expectEquals(rListAfter, MutatorDisplayNames::kHistoryRootSentinel);
    }

    void clear_purgesHistory_emptySentinel()
    {
        beginTest("clear_purgesHistory_emptySentinel");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(1001);
        auto m00Parent = makeDistinctBuffer(1002);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto m05 = makeDistinctBuffer(1003);
        auto m05Parent = makeDistinctBuffer(1004);
        Core::MutationNaming::applyPatchName(m05, 5);
        expect(harness.store().insertRoot(5, m05, m05Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 0, nullptr);

        expect(harness.engine.clearHistory().success);
        expectEquals(harness.store().rootCount(), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedM)), -1);
        expect(harness.proc.apvts.state.getProperty(MutatorState::kHistoryMList).toString().isEmpty());
        expect(harness.proc.apvts.state.getProperty(MutatorState::kHistoryRList).toString().isEmpty());
    }

    void clear_disablesCompare()
    {
        beginTest("clear_disablesCompare");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(1011);
        auto m00Parent = makeDistinctBuffer(1012);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        harness.store().setInitialSnapshot(m00Parent);

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedM, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kCompareActive, true, nullptr);

        expect(harness.engine.clearHistory().success);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
    }

    void clear_auditionsInitialSnapshot()
    {
        beginTest("clear_auditionsInitialSnapshot");

        EngineHarness harness;

        const auto snapshot = makeDistinctBuffer(1021);
        harness.store().setInitialSnapshot(snapshot);

        auto m00 = makeDistinctBuffer(1022);
        auto m00Parent = makeDistinctBuffer(1023);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        std::memcpy(harness.model.data(), m00.data(), Core::PatchModel::kBufferSize);

        expect(harness.engine.clearHistory().success);
        expectEquals(countPatchSysExMessages(harness.queue), 1);
        expect(std::memcmp(harness.model.data(), snapshot.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void clear_keepsInitialSnapshot()
    {
        beginTest("clear_keepsInitialSnapshot");

        EngineHarness harness;

        const auto snapshot = makeDistinctBuffer(1031);
        harness.store().setInitialSnapshot(snapshot);

        auto m00 = makeDistinctBuffer(1032);
        auto m00Parent = makeDistinctBuffer(1033);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        expect(harness.engine.clearHistory().success);
        expect(harness.store().hasInitialSnapshot());
    }
};

static PatchMutatorEngineTests patchMutatorEngineTests;
