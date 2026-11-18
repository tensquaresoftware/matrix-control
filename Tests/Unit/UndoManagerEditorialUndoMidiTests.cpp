#include <bitset>
#include <memory>
#include <unordered_set>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Init/InitDefaults.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/MatrixModBusParameterSysExDispatcher.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/DeviceMemoryLimits.h"
#include "Shared/Definitions/ApvtsTypes.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Core/PluginProcessorInternal.h"
#include "PatchMutatorEngineTestSupport.h"

namespace
{

juce::AudioProcessorValueTreeState::ParameterLayout makeBus012SourceLayout()
{
    using namespace PluginDescriptors::MatrixModulationSection;

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    for (int bus = 0; bus < 3; ++bus)
    {
        const auto& sourceDesc = kModulationBusChoiceParameters[static_cast<size_t>(bus)][0];
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(sourceDesc.parameterId, 1),
            sourceDesc.hostName,
            sourceDesc.choices,
            sourceDesc.defaultIndex));
    }

    return layout;
}

juce::String resolveParameterIdFromTree(juce::ValueTree& tree, const juce::Identifier& property)
{
    const juce::String propertyId = property.toString();
    if (propertyId != ApvtsTypes::kValue)
        return propertyId;

    if (tree.getType().toString() == ApvtsTypes::kParam)
    {
        const juce::var idProperty = tree.getProperty("id");
        if (idProperty.isString() && idProperty.toString().isNotEmpty())
            return idProperty.toString();
    }

    return tree.getType().toString();
}

struct MatrixModCoalesceTimer final : juce::Timer
{
    static constexpr int kCoalesceDelayMs = 40;

    explicit MatrixModCoalesceTimer(Core::MatrixModBusParameterSysExDispatcher& dispatcherIn) noexcept
        : dispatcher_(dispatcherIn)
    {
    }

    void noteParameterChanged(const juce::String& parameterId)
    {
        const int busIndex = dispatcher_.busIndexForParameterId(parameterId);
        if (busIndex < 0)
            return;

        pendingBuses_.set(static_cast<size_t>(busIndex));

        if (!isTimerRunning())
            startTimer(kCoalesceDelayMs);
    }

    void cancelPending() noexcept
    {
        stopTimer();
        pendingBuses_.reset();
    }

    void flushPendingSynchronouslyForTests()
    {
        if (isTimerRunning())
            timerCallback();
    }

private:
    void timerCallback() override
    {
        stopTimer();

        for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        {
            if (!pendingBuses_.test(static_cast<size_t>(bus)))
                continue;

            pendingBuses_.reset(static_cast<size_t>(bus));
            dispatcher_.dispatchBus(bus);
        }
    }

    Core::MatrixModBusParameterSysExDispatcher& dispatcher_;
    std::bitset<Matrix1000Limits::kModulationBusCount> pendingBuses_;
};

class EditorialUndoMidiAudioProcessor : public juce::AudioProcessor
{
public:
    explicit EditorialUndoMidiAudioProcessor(juce::AudioProcessorValueTreeState::ParameterLayout layout)
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, &undoManager, "P", std::move(layout))
    {
    }

    juce::UndoManager undoManager;
    juce::AudioProcessorValueTreeState apvts;

    const juce::String getName() const override { return "EditorialUndoMidiHarness"; }
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

struct EditorialUndoMidiHarness : EditorialUndoMidiAudioProcessor, juce::ValueTree::Listener
{
    Core::PatchModel model;
    Core::ApvtsPatchMapper mapper;
    Core::PatchNameSyncer patchNameSyncer;
    Core::MidiOutboundQueue queue;
    Core::MidiActivityTracker tracker;
    MidiManager midiManager;
    Core::MatrixModBusParameterSysExDispatcher dispatcher;
    MatrixModCoalesceTimer coalesceTimer;
    std::unordered_set<juce::String> patchParameterIds_;
    std::unordered_set<juce::String> matrixModParameterIds_;
    bool suppressPatchParameterSysEx_ { false };
    bool suppressMatrixModParameterSysEx_ { false };
    juce::uint32 editorialResyncGranularMidiQuietUntilMs_ { 0 };
    Core::DeviceMemoryLimits deviceLimits {
        Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000)
    };

    explicit EditorialUndoMidiHarness(juce::AudioProcessorValueTreeState::ParameterLayout layout)
        : EditorialUndoMidiAudioProcessor(std::move(layout))
        , mapper(apvts, model)
        , patchNameSyncer(apvts, model)
        , midiManager(apvts, queue, tracker)
        , dispatcher(model,
                     [this](juce::uint8 bus, juce::uint8 source, juce::uint8 amount, juce::uint8 destination)
                     {
                         midiManager.enqueueMatrixModBusEdit(bus, source, amount, destination);
                     })
        , coalesceTimer(dispatcher)
    {
        using namespace PluginDescriptors::MatrixModulationSection;

        for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        {
            patchParameterIds_.insert(kModulationBusChoiceParameters[static_cast<size_t>(bus)][0].parameterId);
            matrixModParameterIds_.insert(kModulationBusChoiceParameters[static_cast<size_t>(bus)][0].parameterId);
        }

        apvts.state.addListener(this);
        apvts.state.setProperty("deviceDetected", true, nullptr);
        apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                MatrixDeviceTypes::kMatrix1000Id,
                                nullptr);
        model.loadFrom(Core::InitDefaults::patchData());
    }

    ~EditorialUndoMidiHarness() override
    {
        apvts.state.removeListener(this);
    }

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override
    {
        dispatchPatchOrMatrixModParameterChange(resolveParameterIdFromTree(treeWhosePropertyHasChanged, property));
    }

    void dispatchPatchOrMatrixModParameterChange(const juce::String& parameterId)
    {
        if (patchParameterIds_.count(parameterId) == 0)
            return;

        if (matrixModParameterIds_.count(parameterId) > 0)
        {
            if (suppressMatrixModParameterSysEx_ || isEditorialResyncGranularMidiQuiet())
                return;

            mapper.apvtsToBuffer();
            coalesceTimer.noteParameterChanged(parameterId);
            return;
        }

        if (suppressPatchParameterSysEx_ || isEditorialResyncGranularMidiQuiet())
            return;

        mapper.apvtsToBuffer();
    }

    void beginEditorialResyncGranularMidiQuietPeriod()
    {
        editorialResyncGranularMidiQuietUntilMs_ =
            juce::Time::getMillisecondCounter()
            + static_cast<juce::uint32>(PluginProcessorInternal::kEditorialUndoRedoGranularMidiQuietMs);
    }

    bool isEditorialResyncGranularMidiQuiet() const
    {
        return juce::Time::getMillisecondCounter() < editorialResyncGranularMidiQuietUntilMs_;
    }

    void writeSourceIndexForUndo(int bus, int choiceIndex)
    {
        namespace BusIds = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets;
        const juce::String sourceIds[] = { BusIds::kBus0Source, BusIds::kBus1Source, BusIds::kBus2Source };

        undoManager.beginNewTransaction("Matrix Mod source");

        if (auto* param = apvts.getParameter(sourceIds[bus]))
            param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(choiceIndex)));
    }

    void writeSourceIndexLive(int bus, int choiceIndex)
    {
        namespace BusIds = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets;
        const juce::String sourceIds[] = { BusIds::kBus0Source, BusIds::kBus1Source, BusIds::kBus2Source };

        apvts.getParameterAsValue(sourceIds[bus]).setValue(static_cast<float>(choiceIndex));
        (void) apvts.copyState();
    }

    void resyncSynthAfterEditorialUndoRedo()
    {
        mapper.apvtsToBuffer();
        patchNameSyncer.apvtsToBuffer();
        midiManager.sendFullPatchForAudition(model.data(), 0, deviceLimits.hasBankConcept());
    }

    bool performEditorialUndo()
    {
        if (!undoManager.canUndo())
            return false;

        beginEditorialResyncGranularMidiQuietPeriod();
        coalesceTimer.cancelPending();

        suppressPatchParameterSysEx_ = true;
        suppressMatrixModParameterSysEx_ = true;

        const bool ok = undoManager.undo();

        if (ok)
            resyncSynthAfterEditorialUndoRedo();

        PluginProcessorInternal::flushDeferredApvtsParameterSync(apvts);
        coalesceTimer.cancelPending();

        suppressPatchParameterSysEx_ = false;
        suppressMatrixModParameterSysEx_ = false;

        return ok;
    }

    bool performEditorialRedo()
    {
        if (!undoManager.canRedo())
            return false;

        beginEditorialResyncGranularMidiQuietPeriod();
        coalesceTimer.cancelPending();

        suppressPatchParameterSysEx_ = true;
        suppressMatrixModParameterSysEx_ = true;

        const bool ok = undoManager.redo();

        if (ok)
            resyncSynthAfterEditorialUndoRedo();

        PluginProcessorInternal::flushDeferredApvtsParameterSync(apvts);
        coalesceTimer.cancelPending();

        suppressPatchParameterSysEx_ = false;
        suppressMatrixModParameterSysEx_ = false;

        return ok;
    }

    void simulateDeferredApvtsSideEffects()
    {
        coalesceTimer.flushPendingSynchronouslyForTests();
        (void) apvts.copyState();
        coalesceTimer.flushPendingSynchronouslyForTests();
    }

    static void drainQueue(Core::MidiOutboundQueue& outboundQueue)
    {
        while (outboundQueue.dequeue().has_value()) {}
    }
};

PatchMutatorEngineTestSupport::FullPatchOpcodeCounts countAndDrainQueuedOpcodes(Core::MidiOutboundQueue& outboundQueue)
{
    PatchMutatorEngineTestSupport::FullPatchOpcodeCounts counts;

    while (! outboundQueue.isEmpty())
    {
        const auto msg = outboundQueue.dequeue();
        if (! msg.has_value())
            break;

        if (msg->category != Core::MidiOutboundQueue::MessageCategory::kSysEx)
            continue;

        const auto& block = msg->sysExData;
        if (block.getSize() < 4)
            continue;

        const auto* data = static_cast<const juce::uint8*>(block.getData());
        if (data[3] == SysExConstants::Opcode::kSinglePatchData)
            ++counts.slotWrite;
        else if (data[3] == SysExConstants::Opcode::kSinglePatchToEditBuffer)
            ++counts.editBuffer;
        else if (data[3] == SysExConstants::Opcode::kRemoteParameterEdit)
            ++counts.remoteParamEdit;
        else if (data[3] == SysExConstants::Opcode::kRemoteParameterEditMatrix)
            ++counts.matrixModEdit;
    }

    return counts;
}

class UndoManagerEditorialUndoMidiTests : public juce::UnitTest
{
public:
    UndoManagerEditorialUndoMidiTests() : juce::UnitTest("UndoManagerEditorialUndoMidi") {}

    void runTest() override
    {
        testEditorialUndoEmitsOnlyFullPatchResync();
        testEditorialUndoWithDeferredApvtsFlushEmitsOnlyFullPatchResync();
        testEditorialRedoEmitsOnlyFullPatchResync();
        testLiveMatrixModEditStillCoalescesToMatrixModSysEx();
    }

private:
    void testEditorialUndoEmitsOnlyFullPatchResync()
    {
        beginTest("Editorial undo after three Matrix Mod source edits emits one 0x0D only");

        EditorialUndoMidiHarness harness(makeBus012SourceLayout());
        harness.undoManager.clearUndoHistory();
        EditorialUndoMidiHarness::drainQueue(harness.queue);

        for (int bus = 0; bus < 3; ++bus)
        {
            harness.writeSourceIndexForUndo(bus, bus + 1);
            harness.simulateDeferredApvtsSideEffects();
            EditorialUndoMidiHarness::drainQueue(harness.queue);
        }

        for (int step = 0; step < 3; ++step)
        {
            EditorialUndoMidiHarness::drainQueue(harness.queue);

            expect(harness.performEditorialUndo());
            harness.simulateDeferredApvtsSideEffects();

            const auto counts = countAndDrainQueuedOpcodes(harness.queue);
            expectEquals(counts.editBuffer, 1);
            expectEquals(counts.matrixModEdit, 0);
            expectEquals(counts.remoteParamEdit, 0);
        }
    }

    void testEditorialUndoWithDeferredApvtsFlushEmitsOnlyFullPatchResync()
    {
        beginTest("Editorial undo with deferred APVTS flush emits one 0x0D only");

        EditorialUndoMidiHarness harness(makeBus012SourceLayout());
        harness.undoManager.clearUndoHistory();
        EditorialUndoMidiHarness::drainQueue(harness.queue);

        for (int bus = 0; bus < 3; ++bus)
        {
            harness.writeSourceIndexForUndo(bus, bus + 1);
            PluginProcessorInternal::flushDeferredApvtsParameterSync(harness.apvts);
            EditorialUndoMidiHarness::drainQueue(harness.queue);
        }

        for (int step = 0; step < 3; ++step)
        {
            EditorialUndoMidiHarness::drainQueue(harness.queue);

            expect(harness.performEditorialUndo());
            harness.simulateDeferredApvtsSideEffects();

            const auto counts = countAndDrainQueuedOpcodes(harness.queue);
            expectEquals(counts.editBuffer, 1);
            expectEquals(counts.matrixModEdit, 0);
            expectEquals(counts.remoteParamEdit, 0);
        }
    }

    void testEditorialRedoEmitsOnlyFullPatchResync()
    {
        beginTest("Editorial redo after Matrix Mod undos emits one 0x0D per step only");

        EditorialUndoMidiHarness harness(makeBus012SourceLayout());
        harness.undoManager.clearUndoHistory();
        EditorialUndoMidiHarness::drainQueue(harness.queue);

        for (int bus = 0; bus < 3; ++bus)
        {
            harness.writeSourceIndexForUndo(bus, bus + 1);
            harness.simulateDeferredApvtsSideEffects();
            EditorialUndoMidiHarness::drainQueue(harness.queue);
        }

        for (int step = 0; step < 3; ++step)
            expect(harness.performEditorialUndo());

        for (int step = 0; step < 3; ++step)
        {
            EditorialUndoMidiHarness::drainQueue(harness.queue);

            expect(harness.performEditorialRedo());
            harness.simulateDeferredApvtsSideEffects();

            const auto counts = countAndDrainQueuedOpcodes(harness.queue);
            expectEquals(counts.editBuffer, 1);
            expectEquals(counts.matrixModEdit, 0);
            expectEquals(counts.remoteParamEdit, 0);
        }
    }

    void testLiveMatrixModEditStillCoalescesToMatrixModSysEx()
    {
        beginTest("Live Matrix Mod source edit still coalesces to one 0x0B");

        EditorialUndoMidiHarness harness(makeBus012SourceLayout());
        harness.undoManager.clearUndoHistory();
        EditorialUndoMidiHarness::drainQueue(harness.queue);

        harness.writeSourceIndexLive(0, 2);
        harness.coalesceTimer.flushPendingSynchronouslyForTests();

        const auto counts = countAndDrainQueuedOpcodes(harness.queue);
        expectEquals(counts.matrixModEdit, 1);
        expectEquals(counts.editBuffer, 0);
        expectEquals(counts.remoteParamEdit, 0);
    }
};

static UndoManagerEditorialUndoMidiTests undoManagerEditorialUndoMidiTests;

} // namespace
