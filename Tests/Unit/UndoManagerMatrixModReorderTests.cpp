#include <unordered_set>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MatrixModBusParameterSysExDispatcher.h"
#include "Core/MIDI/MatrixModBusReorderService.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PackedFieldCodec.h"
#include "Core/Models/PatchModel.h"
#include "Shared/Definitions/ApvtsTypes.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{

juce::AudioProcessorValueTreeState::ParameterLayout makeTwoBusLayout(int busA, int busB)
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    const auto addBus = [&layout](int busIndex)
    {
        using namespace PluginDescriptors::MatrixModulationSection;

        const auto& amountDesc = kModulationBusIntParameters[static_cast<size_t>(busIndex)][0];
        const auto& sourceDesc = kModulationBusChoiceParameters[static_cast<size_t>(busIndex)][0];
        const auto& destinationDesc = kModulationBusChoiceParameters[static_cast<size_t>(busIndex)][1];

        layout.add(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID(amountDesc.parameterId, 1), amountDesc.hostName,
            amountDesc.minValue, amountDesc.maxValue, amountDesc.defaultValue));
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(sourceDesc.parameterId, 1), sourceDesc.hostName,
            sourceDesc.choices, sourceDesc.defaultIndex));
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(destinationDesc.parameterId, 1), destinationDesc.hostName,
            destinationDesc.choices, destinationDesc.defaultIndex));
    };

    addBus(busA);
    addBus(busB);
    return layout;
}

struct BusTriplet
{
    int sourceIndex;
    int amount;
    int destinationIndex;
};

void configureBus(Core::PatchModel& model, int busIndex, const BusTriplet& triplet)
{
    using namespace PluginDescriptors::MatrixModulationSection;

    const auto& intParams = kModulationBusIntParameters[static_cast<size_t>(busIndex)];
    const auto& choiceParams = kModulationBusChoiceParameters[static_cast<size_t>(busIndex)];

    model.setChoiceIndex(choiceParams[0], triplet.sourceIndex);
    model.setValue(intParams[0], triplet.amount);
    model.setChoiceIndex(choiceParams[1], triplet.destinationIndex);
}

juce::uint8 readModelByte(const Core::PatchModel& model, int sysExOffset)
{
    const size_t offset = Core::PackedFieldCodec::safeOffset(sysExOffset, Core::PatchModel::kBufferSize);
    return model.data()[offset];
}

juce::MemoryBlock expectedMatrixModBusMessage(const Core::PatchModel& model,
                                              const SysExEncoder& encoder,
                                              int busIndex)
{
    using namespace PluginDescriptors::MatrixModulationSection;

    const auto& sourceDesc = kModulationBusChoiceParameters[static_cast<size_t>(busIndex)][0];
    const auto& amountDesc = kModulationBusIntParameters[static_cast<size_t>(busIndex)][0];
    const auto& destinationDesc = kModulationBusChoiceParameters[static_cast<size_t>(busIndex)][1];

    return encoder.encodeMatrixModBusEdit(
        static_cast<juce::uint8>(busIndex),
        readModelByte(model, sourceDesc.sysExOffset),
        readModelByte(model, amountDesc.sysExOffset),
        readModelByte(model, destinationDesc.sysExOffset));
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

class UndoReorderAudioProcessor : public juce::AudioProcessor
{
public:
    explicit UndoReorderAudioProcessor(juce::AudioProcessorValueTreeState::ParameterLayout layout)
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, &undoManager, "P", std::move(layout))
    {
    }

    juce::UndoManager undoManager;
    juce::AudioProcessorValueTreeState apvts;

    const juce::String getName() const override { return "UndoReorderHarness"; }
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

struct UndoReorderHarness : UndoReorderAudioProcessor, juce::ValueTree::Listener
{
    Core::PatchModel model;
    Core::ApvtsPatchMapper mapper;
    Core::MidiOutboundQueue queue;
    Core::MidiActivityTracker tracker;
    SysExEncoder encoder;
    Core::MatrixModBusParameterSysExDispatcher dispatcher;
    Core::MatrixModBusReorderService reorderService;
    std::unordered_set<juce::String> matrixModParameterIds_;
    bool suppressMatrixModParameterSysEx_ { false };

    explicit UndoReorderHarness(juce::AudioProcessorValueTreeState::ParameterLayout layout)
        : UndoReorderAudioProcessor(std::move(layout))
        , mapper(apvts, model)
        , dispatcher(model,
                     [this](juce::uint8 bus, juce::uint8 source, juce::uint8 amount, juce::uint8 destination)
                     {
                         Core::EditorPath editorPath(queue, tracker);
                         editorPath.enqueueSysEx(encoder.encodeMatrixModBusEdit(bus, source, amount, destination));
                     })
        , reorderService(model, mapper, dispatcher)
    {
        using namespace PluginDescriptors::MatrixModulationSection;

        for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        {
            matrixModParameterIds_.insert(kModulationBusIntParameters[static_cast<size_t>(bus)][0].parameterId);
            matrixModParameterIds_.insert(kModulationBusChoiceParameters[static_cast<size_t>(bus)][0].parameterId);
            matrixModParameterIds_.insert(kModulationBusChoiceParameters[static_cast<size_t>(bus)][1].parameterId);
        }

        apvts.state.addListener(this);
    }

    ~UndoReorderHarness() override
    {
        apvts.state.removeListener(this);
    }

    void swapMatrixModBusContents(int fromBus, int toBus)
    {
        if (fromBus == toBus)
            return;

        if (fromBus < 0 || fromBus >= Matrix1000Limits::kModulationBusCount
            || toBus < 0 || toBus >= Matrix1000Limits::kModulationBusCount)
            return;

        undoManager.beginNewTransaction("Matrix Mod reorder");

        suppressMatrixModParameterSysEx_ = true;
        reorderService.swapBusContents(fromBus, toBus);
        suppressMatrixModParameterSysEx_ = false;
    }

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override
    {
        const juce::String parameterId = resolveParameterIdFromTree(treeWhosePropertyHasChanged, property);

        if (matrixModParameterIds_.count(parameterId) == 0)
            return;

        if (! suppressMatrixModParameterSysEx_)
            mapper.apvtsToBuffer();

        if (suppressMatrixModParameterSysEx_)
            return;

        dispatcher.dispatch(parameterId);
    }
};

class UndoManagerMatrixModReorderTests : public juce::UnitTest
{
public:
    UndoManagerMatrixModReorderTests() : juce::UnitTest("UndoManagerMatrixModReorder") {}

    void runTest() override
    {
        testReorderProducesSingleUndoTransaction();
        testUndoRestoresPreSwapLayout();
        testRedoRestoresPostSwapLayout();
        testSysExAfterUndo();
        testIdenticalBusIndicesDoNotRecordUndo();
        testInvalidBusIndicesDoNotRecordUndo();
    }

private:
    void testReorderProducesSingleUndoTransaction()
    {
        beginTest("Matrix Mod reorder records one undo transaction");

        UndoReorderHarness harness(makeTwoBusLayout(0, 7));

        configureBus(harness.model, 0, { 4, -15, 9 });
        configureBus(harness.model, 7, { 1, 22, 3 });
        harness.suppressMatrixModParameterSysEx_ = true;
        harness.mapper.pushBusToApvts(0);
        harness.mapper.pushBusToApvts(7);
        harness.suppressMatrixModParameterSysEx_ = false;
        harness.undoManager.clearUndoHistory();

        const auto undoCountBefore = harness.undoManager.getUndoDescriptions().size();
        harness.swapMatrixModBusContents(0, 7);

        expectEquals(static_cast<int>(harness.undoManager.getUndoDescriptions().size()), undoCountBefore + 1);
    }

    void testUndoRestoresPreSwapLayout()
    {
        beginTest("One undo restores pre-reorder bus contents");

        UndoReorderHarness harness(makeTwoBusLayout(0, 7));

        configureBus(harness.model, 0, { 4, -15, 9 });
        configureBus(harness.model, 7, { 1, 22, 3 });
        harness.suppressMatrixModParameterSysEx_ = true;
        harness.mapper.pushBusToApvts(0);
        harness.mapper.pushBusToApvts(7);
        harness.suppressMatrixModParameterSysEx_ = false;
        harness.undoManager.clearUndoHistory();

        harness.swapMatrixModBusContents(0, 7);
        harness.undoManager.undo();
        harness.mapper.apvtsToBuffer();

        using namespace PluginDescriptors::MatrixModulationSection;

        expectEquals(harness.model.getChoiceIndex(kModulationBusChoiceParameters[0][0]), 4);
        expectEquals(harness.model.getValue(kModulationBusIntParameters[0][0]), -15);
        expectEquals(harness.model.getChoiceIndex(kModulationBusChoiceParameters[0][1]), 9);

        expectEquals(harness.model.getChoiceIndex(kModulationBusChoiceParameters[7][0]), 1);
        expectEquals(harness.model.getValue(kModulationBusIntParameters[7][0]), 22);
        expectEquals(harness.model.getChoiceIndex(kModulationBusChoiceParameters[7][1]), 3);
    }

    void testRedoRestoresPostSwapLayout()
    {
        beginTest("Redo restores post-reorder bus contents");

        UndoReorderHarness harness(makeTwoBusLayout(0, 7));

        configureBus(harness.model, 0, { 4, -15, 9 });
        configureBus(harness.model, 7, { 1, 22, 3 });
        harness.suppressMatrixModParameterSysEx_ = true;
        harness.mapper.pushBusToApvts(0);
        harness.mapper.pushBusToApvts(7);
        harness.suppressMatrixModParameterSysEx_ = false;
        harness.undoManager.clearUndoHistory();

        harness.swapMatrixModBusContents(0, 7);
        harness.undoManager.undo();
        harness.undoManager.redo();
        harness.mapper.apvtsToBuffer();

        using namespace PluginDescriptors::MatrixModulationSection;

        expectEquals(harness.model.getChoiceIndex(kModulationBusChoiceParameters[0][0]), 1);
        expectEquals(harness.model.getValue(kModulationBusIntParameters[0][0]), 22);
        expectEquals(harness.model.getChoiceIndex(kModulationBusChoiceParameters[0][1]), 3);

        expectEquals(harness.model.getChoiceIndex(kModulationBusChoiceParameters[7][0]), 4);
        expectEquals(harness.model.getValue(kModulationBusIntParameters[7][0]), -15);
        expectEquals(harness.model.getChoiceIndex(kModulationBusChoiceParameters[7][1]), 9);
    }

    void testSysExAfterUndo()
    {
        beginTest("Undo after reorder dispatches Matrix Mod SysEx for affected buses");

        UndoReorderHarness harness(makeTwoBusLayout(0, 1));

        configureBus(harness.model, 0, { 5, -20, 12 });
        configureBus(harness.model, 1, { 2, 31, 7 });
        harness.suppressMatrixModParameterSysEx_ = true;
        harness.mapper.pushBusToApvts(0);
        harness.mapper.pushBusToApvts(1);
        harness.suppressMatrixModParameterSysEx_ = false;
        harness.undoManager.clearUndoHistory();

        harness.swapMatrixModBusContents(0, 1);

        while (harness.queue.dequeue().has_value()) {}

        harness.undoManager.undo();
        harness.mapper.apvtsToBuffer();
        while (harness.queue.dequeue().has_value()) {}

        harness.dispatcher.dispatchBus(0);
        harness.dispatcher.dispatchBus(1);

        const auto expectedBus0 = expectedMatrixModBusMessage(harness.model, harness.encoder, 0);
        const auto expectedBus1 = expectedMatrixModBusMessage(harness.model, harness.encoder, 1);

        auto firstMsg = harness.queue.dequeue();
        auto secondMsg = harness.queue.dequeue();
        expect(firstMsg.has_value());
        expect(secondMsg.has_value());
        expect(firstMsg->sysExData == expectedBus0);
        expect(secondMsg->sysExData == expectedBus1);
        expect(harness.queue.isEmpty());
    }

    void testIdenticalBusIndicesDoNotRecordUndo()
    {
        beginTest("Identical bus indices do not add undo transaction");

        UndoReorderHarness harness(makeTwoBusLayout(0, 1));

        const auto undoCountBefore = harness.undoManager.getUndoDescriptions().size();
        harness.swapMatrixModBusContents(0, 0);

        expectEquals(static_cast<int>(harness.undoManager.getUndoDescriptions().size()), undoCountBefore);
    }

    void testInvalidBusIndicesDoNotRecordUndo()
    {
        beginTest("Invalid bus indices do not add undo transaction");

        UndoReorderHarness harness(makeTwoBusLayout(0, 1));

        const auto undoCountBefore = harness.undoManager.getUndoDescriptions().size();
        harness.swapMatrixModBusContents(-1, 0);
        harness.swapMatrixModBusContents(0, Matrix1000Limits::kModulationBusCount);

        expectEquals(static_cast<int>(harness.undoManager.getUndoDescriptions().size()), undoCountBefore);
    }
};

static UndoManagerMatrixModReorderTests undoManagerMatrixModReorderTests;

} // namespace
