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

class TestAudioProcessor : public juce::AudioProcessor
{
public:
    explicit TestAudioProcessor(juce::AudioProcessorValueTreeState::ParameterLayout layout)
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, nullptr, "P", std::move(layout))
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

class MatrixModBusReorderServiceTests : public juce::UnitTest
{
public:
    MatrixModBusReorderServiceTests() : juce::UnitTest("MatrixModBusReorderService") {}

    void runTest() override
    {
        testSwapActiveBusesModelAndQueue();
        testSwapInactiveBuses();
        testIdenticalIndicesNoOp();
        testInvalidIndexNoOp();
        testApvtsMatchesPostSwapModel();
        testProcessorPathSwapWithApvtsListener();
    }

private:
    struct ReorderTestHarness
    {
        TestAudioProcessor proc;
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;
        Core::MatrixModBusParameterSysExDispatcher dispatcher;
        Core::MatrixModBusReorderService reorderService;

        explicit ReorderTestHarness(juce::AudioProcessorValueTreeState::ParameterLayout layout)
            : proc(std::move(layout))
            , mapper(proc.apvts, model)
            , dispatcher(model,
                         [this](juce::uint8 bus, juce::uint8 source, juce::uint8 amount, juce::uint8 destination)
                         {
                             Core::EditorPath editorPath(queue, tracker);
                             editorPath.enqueueSysEx(encoder.encodeMatrixModBusEdit(bus, source, amount, destination));
                         })
            , reorderService(model, mapper, dispatcher)
        {
        }
    };

    static bool sysExMatchesMatrixModBusEdit(const juce::MemoryBlock& block,
                                             juce::uint8 expectedBus,
                                             juce::uint8 expectedSource,
                                             juce::uint8 expectedAmount,
                                             juce::uint8 expectedDestination)
    {
        if (block.getSize() != SysExConstants::kMatrixModBusMessageLength)
            return false;

        const auto* data = static_cast<const juce::uint8*>(block.getData());
        return data[0] == SysExConstants::kSysExStart
            && data[1] == SysExConstants::kManufacturerIdOberheim
            && data[2] == SysExConstants::kDeviceIdMatrix1000
            && data[3] == SysExConstants::Opcode::kRemoteParameterEditMatrix
            && data[4] == expectedBus
            && data[5] == expectedSource
            && data[6] == expectedAmount
            && data[7] == expectedDestination
            && data[8] == SysExConstants::kSysExEnd;
    }

    static juce::AudioProcessorValueTreeState::ParameterLayout makeTwoBusLayout(int busA, int busB)
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        const auto addBus = [&layout](int busIndex)
        {
            using namespace PluginDescriptors::MatrixModulationSection;

            const auto& amountDesc = kModulationBusIntParameters[static_cast<size_t>(busIndex)][0];
            const auto& sourceDesc = kModulationBusChoiceParameters[static_cast<size_t>(busIndex)][0];
            const auto& destinationDesc = kModulationBusChoiceParameters[static_cast<size_t>(busIndex)][1];

            layout.add(std::make_unique<juce::AudioParameterInt>(
                juce::ParameterID(amountDesc.parameterId, 1), amountDesc.displayName,
                amountDesc.minValue, amountDesc.maxValue, amountDesc.defaultValue));
            layout.add(std::make_unique<juce::AudioParameterChoice>(
                juce::ParameterID(sourceDesc.parameterId, 1), sourceDesc.displayName,
                sourceDesc.choices, sourceDesc.defaultIndex));
            layout.add(std::make_unique<juce::AudioParameterChoice>(
                juce::ParameterID(destinationDesc.parameterId, 1), destinationDesc.displayName,
                destinationDesc.choices, destinationDesc.defaultIndex));
        };

        addBus(busA);
        addBus(busB);
        return layout;
    }

    static void configureBus(Core::PatchModel& model,
                             int busIndex,
                             int sourceIndex,
                             int amount,
                             int destinationIndex)
    {
        using namespace PluginDescriptors::MatrixModulationSection;

        const auto& intParams = kModulationBusIntParameters[static_cast<size_t>(busIndex)];
        const auto& choiceParams = kModulationBusChoiceParameters[static_cast<size_t>(busIndex)];

        model.setChoiceIndex(choiceParams[0], sourceIndex);
        model.setValue(intParams[0], amount);
        model.setChoiceIndex(choiceParams[1], destinationIndex);
    }

    static juce::uint8 readModelByte(const Core::PatchModel& model, int sysExOffset)
    {
        const size_t offset = Core::PackedFieldCodec::safeOffset(sysExOffset, Core::PatchModel::kBufferSize);
        return model.data()[offset];
    }

    void testSwapActiveBusesModelAndQueue()
    {
        beginTest("Swap active buses → model permutation + two 0x0B messages (ascending bus index)");

        ReorderTestHarness harness(makeTwoBusLayout(0, 7));

        configureBus(harness.model, 0, 5, -20, 12);
        configureBus(harness.model, 7, 2, 31, 7);

        const juce::uint8 expectedAmountBus7 = readModelByte(harness.model,
            PluginDescriptors::MatrixModulationSection::kModulationBusIntParameters[0][0].sysExOffset);

        harness.reorderService.swapBusContents(0, 7);

        expectEquals(harness.model.getChoiceIndex(
            PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[0][0]), 2);
        expectEquals(harness.model.getValue(
            PluginDescriptors::MatrixModulationSection::kModulationBusIntParameters[0][0]), 31);
        expectEquals(harness.model.getChoiceIndex(
            PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[0][1]), 7);

        expectEquals(harness.model.getChoiceIndex(
            PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[7][0]), 5);
        expectEquals(harness.model.getValue(
            PluginDescriptors::MatrixModulationSection::kModulationBusIntParameters[7][0]), -20);
        expectEquals(harness.model.getChoiceIndex(
            PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[7][1]), 12);

        auto firstMsg = harness.queue.dequeue();
        auto secondMsg = harness.queue.dequeue();
        expect(firstMsg.has_value());
        expect(secondMsg.has_value());
        expect(sysExMatchesMatrixModBusEdit(firstMsg->sysExData, 0, 2, 31, 7));
        expect(sysExMatchesMatrixModBusEdit(secondMsg->sysExData, 7, 5, expectedAmountBus7, 12));
        expect(harness.queue.isEmpty());
    }

    void testSwapInactiveBuses()
    {
        beginTest("Swap inactive buses → zero triplets + two 0x0B messages");

        ReorderTestHarness harness(makeTwoBusLayout(2, 5));

        harness.reorderService.swapBusContents(2, 5);

        auto firstMsg = harness.queue.dequeue();
        auto secondMsg = harness.queue.dequeue();
        expect(firstMsg.has_value());
        expect(secondMsg.has_value());
        expect(sysExMatchesMatrixModBusEdit(firstMsg->sysExData, 2, 0, 0, 0));
        expect(sysExMatchesMatrixModBusEdit(secondMsg->sysExData, 5, 0, 0, 0));
        expect(harness.queue.isEmpty());
    }

    void testIdenticalIndicesNoOp()
    {
        beginTest("Identical bus indices → no model change, no SysEx");

        ReorderTestHarness harness(makeTwoBusLayout(0, 1));

        configureBus(harness.model, 0, 3, 10, 4);
        const auto sourceBefore = harness.model.getChoiceIndex(
            PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[0][0]);

        harness.reorderService.swapBusContents(3, 3);

        expectEquals(harness.model.getChoiceIndex(
            PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[0][0]), sourceBefore);
        expect(harness.queue.isEmpty());
    }

    void testInvalidIndexNoOp()
    {
        beginTest("Out-of-range bus index → no-op");

        ReorderTestHarness harness(makeTwoBusLayout(0, 1));

        configureBus(harness.model, 0, 1, 5, 2);
        harness.reorderService.swapBusContents(-1, 0);
        harness.reorderService.swapBusContents(0, Matrix1000Limits::kModulationBusCount);

        expectEquals(harness.model.getChoiceIndex(
            PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[0][0]), 1);
        expect(harness.queue.isEmpty());
    }

    void testApvtsMatchesPostSwapModel()
    {
        beginTest("APVTS values match post-swap PatchModel for both buses");

        ReorderTestHarness harness(makeTwoBusLayout(0, 1));

        configureBus(harness.model, 0, 4, -15, 9);
        configureBus(harness.model, 1, 1, 22, 3);

        harness.reorderService.swapBusContents(0, 1);

        using namespace PluginDescriptors::MatrixModulationSection;

        const auto& bus0Source = kModulationBusChoiceParameters[0][0];
        const auto& bus0Amount = kModulationBusIntParameters[0][0];
        const auto& bus0Destination = kModulationBusChoiceParameters[0][1];
        const auto& bus1Source = kModulationBusChoiceParameters[1][0];
        const auto& bus1Amount = kModulationBusIntParameters[1][0];
        const auto& bus1Destination = kModulationBusChoiceParameters[1][1];

        expectEquals(harness.model.getChoiceIndex(bus0Source), 1);
        expectEquals(harness.model.getValue(bus0Amount), 22);
        expectEquals(harness.model.getChoiceIndex(bus0Destination), 3);

        expectEquals(harness.model.getChoiceIndex(bus1Source), 4);
        expectEquals(harness.model.getValue(bus1Amount), -15);
        expectEquals(harness.model.getChoiceIndex(bus1Destination), 9);

        expectEquals(juce::roundToInt(harness.proc.apvts.getRawParameterValue(bus0Source.parameterId)->load()), 1);
        expectEquals(juce::roundToInt(harness.proc.apvts.getRawParameterValue(bus0Amount.parameterId)->load()), 22);
        expectEquals(juce::roundToInt(harness.proc.apvts.getRawParameterValue(bus0Destination.parameterId)->load()), 3);

        expectEquals(juce::roundToInt(harness.proc.apvts.getRawParameterValue(bus1Source.parameterId)->load()), 4);
        expectEquals(juce::roundToInt(harness.proc.apvts.getRawParameterValue(bus1Amount.parameterId)->load()), -15);
        expectEquals(juce::roundToInt(harness.proc.apvts.getRawParameterValue(bus1Destination.parameterId)->load()), 9);

        while (harness.queue.dequeue().has_value()) {}
    }

    struct ProcessorPathHarness : TestAudioProcessor, juce::ValueTree::Listener
    {
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;
        Core::MatrixModBusParameterSysExDispatcher dispatcher;
        Core::MatrixModBusReorderService reorderService;
        std::unordered_set<juce::String> patchParameterIds_;
        std::unordered_set<juce::String> matrixModParameterIds_;
        bool suppressMatrixModParameterSysEx_ { false };

        ProcessorPathHarness(juce::AudioProcessorValueTreeState::ParameterLayout layout, int busA, int busB)
            : TestAudioProcessor(std::move(layout))
            , mapper(apvts, model)
            , dispatcher(model,
                         [this](juce::uint8 bus, juce::uint8 source, juce::uint8 amount, juce::uint8 destination)
                         {
                             Core::EditorPath editorPath(queue, tracker);
                             editorPath.enqueueSysEx(encoder.encodeMatrixModBusEdit(bus, source, amount, destination));
                         })
            , reorderService(model, mapper, dispatcher)
        {
            buildParameterIdSets(busA, busB);
            apvts.state.addListener(this);
        }

        ~ProcessorPathHarness() override
        {
            apvts.state.removeListener(this);
        }

        void swapMatrixModBusContents(int fromBus, int toBus)
        {
            suppressMatrixModParameterSysEx_ = true;
            reorderService.swapBusContents(fromBus, toBus);
            suppressMatrixModParameterSysEx_ = false;
        }

        void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                      const juce::Identifier& property) override
        {
            const juce::String parameterId = resolveParameterIdFromTree(treeWhosePropertyHasChanged, property);

            if (patchParameterIds_.count(parameterId) == 0)
                return;

            const bool isMatrixModParam = matrixModParameterIds_.count(parameterId) > 0;

            if (!isMatrixModParam || !suppressMatrixModParameterSysEx_)
                mapper.apvtsToBuffer();

            if (isMatrixModParam)
            {
                if (!suppressMatrixModParameterSysEx_)
                    dispatcher.dispatch(parameterId);
            }
        }

    private:
        void buildParameterIdSets(int busA, int busB)
        {
            const auto addBus = [this](int busIndex)
            {
                using namespace PluginDescriptors::MatrixModulationSection;

                for (const auto& d : kModulationBusIntParameters[static_cast<size_t>(busIndex)])
                {
                    patchParameterIds_.insert(d.parameterId);
                    matrixModParameterIds_.insert(d.parameterId);
                }

                for (const auto& d : kModulationBusChoiceParameters[static_cast<size_t>(busIndex)])
                {
                    patchParameterIds_.insert(d.parameterId);
                    matrixModParameterIds_.insert(d.parameterId);
                }
            };

            addBus(busA);
            addBus(busB);
        }

        juce::String resolveParameterIdFromTree(juce::ValueTree& tree, const juce::Identifier& property) const
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
    };

    void testProcessorPathSwapWithApvtsListener()
    {
        beginTest("PluginProcessor path → APVTS listener + suppress preserves swap and enqueues two 0x0B");

        ProcessorPathHarness harness(makeTwoBusLayout(0, 1), 0, 1);

        configureBus(harness.model, 0, 4, -15, 9);
        configureBus(harness.model, 1, 1, 22, 3);

        harness.swapMatrixModBusContents(0, 1);

        using namespace PluginDescriptors::MatrixModulationSection;

        const auto& bus0Source = kModulationBusChoiceParameters[0][0];
        const auto& bus0Amount = kModulationBusIntParameters[0][0];
        const auto& bus0Destination = kModulationBusChoiceParameters[0][1];
        const auto& bus1Source = kModulationBusChoiceParameters[1][0];
        const auto& bus1Amount = kModulationBusIntParameters[1][0];
        const auto& bus1Destination = kModulationBusChoiceParameters[1][1];

        expectEquals(harness.model.getChoiceIndex(bus0Source), 1);
        expectEquals(harness.model.getValue(bus0Amount), 22);
        expectEquals(harness.model.getChoiceIndex(bus0Destination), 3);
        expectEquals(harness.model.getChoiceIndex(bus1Source), 4);
        expectEquals(harness.model.getValue(bus1Amount), -15);
        expectEquals(harness.model.getChoiceIndex(bus1Destination), 9);

        expectEquals(juce::roundToInt(harness.apvts.getRawParameterValue(bus0Source.parameterId)->load()), 1);
        expectEquals(juce::roundToInt(harness.apvts.getRawParameterValue(bus0Amount.parameterId)->load()), 22);
        expectEquals(juce::roundToInt(harness.apvts.getRawParameterValue(bus0Destination.parameterId)->load()), 3);
        expectEquals(juce::roundToInt(harness.apvts.getRawParameterValue(bus1Source.parameterId)->load()), 4);
        expectEquals(juce::roundToInt(harness.apvts.getRawParameterValue(bus1Amount.parameterId)->load()), -15);
        expectEquals(juce::roundToInt(harness.apvts.getRawParameterValue(bus1Destination.parameterId)->load()), 9);

        auto firstMsg = harness.queue.dequeue();
        auto secondMsg = harness.queue.dequeue();
        expect(firstMsg.has_value());
        expect(secondMsg.has_value());
        expect(sysExMatchesMatrixModBusEdit(firstMsg->sysExData, 0, 1, 22, 3));
        expect(sysExMatchesMatrixModBusEdit(secondMsg->sysExData, 1, 4,
            readModelByte(harness.model, bus1Amount.sysExOffset), 9));
        expect(harness.queue.isEmpty());
    }
};

static MatrixModBusReorderServiceTests matrixModBusReorderServiceTests;
