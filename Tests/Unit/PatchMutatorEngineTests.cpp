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

namespace PatchMutator = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;

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
};

static PatchMutatorEngineTests patchMutatorEngineTests;
