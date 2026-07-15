#include <functional>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

#include "Core/Actions/MutatorActionHandler.h"
#include "Core/Factories/ApvtsFactory.h"
#include "Core/Services/PatchMutator/PatchMutatorEngine.h"
#include "Shared/Definitions/PluginIDs.h"

namespace PatchMutator = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;

class TestAudioProcessorMutatorHandler : public juce::AudioProcessor
{
public:
    TestAudioProcessorMutatorHandler()
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

class RecordingPatchMutatorEngine final : public Core::PatchMutatorEnginePort
{
public:
    int mutateCallCount { 0 };
    int retryCallCount { 0 };
    int compareCallCount { 0 };
    int deleteCallCount { 0 };
    int clearCallCount { 0 };
    int exportCallCount { 0 };
    int defragCallCount { 0 };
    int rebuildMirrorCallCount { 0 };
    int auditionCallCount { 0 };
    juce::File lastExportFolder;

    Core::MutatorActionResult mutateResult;
    Core::MutatorActionResult retryResult;
    Core::MutatorActionResult compareResult;
    Core::MutatorActionResult deleteResult;
    Core::MutatorActionResult clearResult;
    Core::MutatorActionResult exportResult;
    Core::MutatorActionResult defragResult;

    Core::MutatorActionResult mutate() override
    {
        ++mutateCallCount;
        return mutateResult;
    }

    Core::MutatorActionResult retry() override
    {
        ++retryCallCount;
        return retryResult;
    }

    Core::MutatorActionResult toggleCompare() override
    {
        ++compareCallCount;
        return compareResult;
    }

    Core::MutatorActionResult deleteSelected() override
    {
        ++deleteCallCount;
        return deleteResult;
    }

    Core::MutatorActionResult clearHistory() override
    {
        ++clearCallCount;
        return clearResult;
    }

    Core::MutatorActionResult exportHistory(const juce::File& destinationFolder) override
    {
        ++exportCallCount;
        lastExportFolder = destinationFolder;
        return exportResult;
    }

    Core::MutatorActionResult defragHistory() override
    {
        ++defragCallCount;
        return defragResult;
    }

    void auditionSelectedHistoryEntry() override
    {
        ++auditionCallCount;
    }

    void rebuildHistoryListMirrors() override
    {
        ++rebuildMirrorCallCount;
    }
};

class MutatorActionHandlerTests : public juce::UnitTest
{
public:
    MutatorActionHandlerTests() : juce::UnitTest("MutatorActionHandler") {}

    void runTest() override
    {
        mutate_delegatesToEngine();
        retry_delegatesToEngine();
        compare_delegatesToEngine();
        delete_delegatesToEngine();
        clear_delegatesToEngine();
        export_invokesPickerThenEngine();
        export_cancelledPicker_noEngineCall();
        mutate_blocked_setsFooter();
        historySelection_debounced();
        historySelection_rootChange_rebuildsMirrors();
    }

private:
    static constexpr int kTestDebounceMs = 20;

    struct Harness
    {
        TestAudioProcessorMutatorHandler proc;
        RecordingPatchMutatorEngine engine;
        Core::MutatorActionHandler handler;
        int exportPickerCallCount { 0 };
        juce::File exportPickerResult;

        explicit Harness(int debounceMs = Core::kComboboxPatchSendDebounceMs)
            : handler(proc.apvts,
                      &engine,
                      [this]()
                      {
                          ++exportPickerCallCount;
                          return exportPickerResult;
                      },
                      {},
                      debounceMs)
        {
        }
    };

    void mutate_delegatesToEngine()
    {
        beginTest("mutate_delegatesToEngine");

        Harness harness;
        harness.engine.mutateResult.success = true;

        harness.handler.handleAction(PatchMutator::kMutate, juce::int64(1));

        expectEquals(harness.engine.mutateCallCount, 1);
    }

    void retry_delegatesToEngine()
    {
        beginTest("retry_delegatesToEngine");

        Harness harness;
        harness.engine.retryResult.success = true;

        harness.handler.handleAction(PatchMutator::kRetry, juce::int64(1));

        expectEquals(harness.engine.retryCallCount, 1);
    }

    void compare_delegatesToEngine()
    {
        beginTest("compare_delegatesToEngine");

        Harness harness;
        harness.engine.compareResult.success = true;

        harness.handler.handleAction(PatchMutator::kCompare, juce::int64(1));

        expectEquals(harness.engine.compareCallCount, 1);
    }

    void delete_delegatesToEngine()
    {
        beginTest("delete_delegatesToEngine");

        Harness harness;
        harness.engine.deleteResult.success = true;

        harness.handler.handleAction(PatchMutator::kDelete, juce::int64(1));

        expectEquals(harness.engine.deleteCallCount, 1);
    }

    void clear_delegatesToEngine()
    {
        beginTest("clear_delegatesToEngine");

        Harness harness;
        harness.engine.clearResult.success = true;

        harness.handler.handleAction(PatchMutator::kClear, juce::int64(1));

        expectEquals(harness.engine.clearCallCount, 1);
    }

    void export_invokesPickerThenEngine()
    {
        beginTest("export_invokesPickerThenEngine");

        Harness harness;

        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getNonexistentChildFile("MatrixControlMutatorExport", "", false);
        if (! tempDir.createDirectory())
        {
            expect(false, "Failed to create temp export directory");
            return;
        }

        harness.exportPickerResult = tempDir;
        harness.engine.exportResult.success = true;
        harness.engine.exportResult.footerMessage = "Exported 1 mutation file(s).";
        harness.engine.exportResult.footerSeverity = "info";

        harness.handler.handleAction(PatchMutator::kExport, juce::int64(1));

        expectEquals(harness.exportPickerCallCount, 1);
        expectEquals(harness.engine.exportCallCount, 1);
        expect(harness.engine.lastExportFolder == tempDir);
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString().contains("Exported"));

        tempDir.deleteRecursively();
    }

    void export_cancelledPicker_noEngineCall()
    {
        beginTest("export_cancelledPicker_noEngineCall");

        Harness harness;
        harness.exportPickerResult = juce::File();

        harness.handler.handleAction(PatchMutator::kExport, juce::int64(1));

        expectEquals(harness.exportPickerCallCount, 1);
        expectEquals(harness.engine.exportCallCount, 0);
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString().isEmpty());
    }

    void mutate_blocked_setsFooter()
    {
        beginTest("mutate_blocked_setsFooter");

        Harness harness;
        harness.engine.mutateResult.footerMessage = "Set Amount and Random above 0 to mutate.";
        harness.engine.mutateResult.footerSeverity = "warning";

        harness.handler.handleAction(PatchMutator::kMutate, juce::int64(1));

        expectEquals(harness.engine.mutateCallCount, 1);
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString().isNotEmpty());
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "warning");
    }

    void historySelection_debounced()
    {
        beginTest("historySelection_debounced");

        Harness harness(kTestDebounceMs);

        for (int i = 0; i < 5; ++i)
            harness.handler.onHistorySelectionChanged();

        expectEquals(harness.engine.auditionCallCount, 0);

        harness.handler.flushHistorySelectionDebouncerForTests();

        expectEquals(harness.engine.auditionCallCount, 1);
    }

    void historySelection_rootChange_rebuildsMirrors()
    {
        beginTest("historySelection_rootChange_rebuildsMirrors");

        Harness harness(kTestDebounceMs);

        harness.handler.onHistorySelectionChanged(true);
        expectEquals(harness.engine.rebuildMirrorCallCount, 1);
        expectEquals(harness.engine.auditionCallCount, 0);

        harness.handler.onHistorySelectionChanged(false);
        expectEquals(harness.engine.rebuildMirrorCallCount, 1);

        harness.handler.flushHistorySelectionDebouncerForTests();
        expectEquals(harness.engine.auditionCallCount, 1);
    }
};

static MutatorActionHandlerTests mutatorActionHandlerTests;
