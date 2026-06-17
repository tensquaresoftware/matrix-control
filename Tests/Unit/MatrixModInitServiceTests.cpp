#include <unordered_set>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Init/MatrixModInitService.h"
#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MatrixModBusParameterSysExDispatcher.h"
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

class MatrixModInitServiceTests : public juce::UnitTest
{
public:
    MatrixModInitServiceTests() : juce::UnitTest("MatrixModInitService") {}

    void runTest() override
    {
        testInitBusActiveBusModelAndQueue();
        testInitBusInvalidIndexNoOp();
        testInitAllBusesModelAndQueue();
        testSysExPayloadAllZeros();
        testApvtsMatchesPostInitModel();
        testProcessorPathInitWithApvtsListener();
    }

private:
    struct InitTestHarness
    {
        TestAudioProcessor proc;
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;
        Core::MatrixModBusParameterSysExDispatcher dispatcher;
        Core::MatrixModInitService initService;

        explicit InitTestHarness(juce::AudioProcessorValueTreeState::ParameterLayout layout)
            : proc(std::move(layout))
            , mapper(proc.apvts, model)
            , dispatcher(model,
                         [this](juce::uint8 bus, juce::uint8 source, juce::uint8 amount, juce::uint8 destination)
                         {
                             Core::EditorPath editorPath(queue, tracker);
                             editorPath.enqueueSysEx(encoder.encodeMatrixModBusEdit(bus, source, amount, destination));
                         })
            , initService(model, mapper, dispatcher)
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

    static juce::AudioProcessorValueTreeState::ParameterLayout makeBusLayout(const std::vector<int>& busIndices)
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        for (const int busIndex : busIndices)
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
        }

        return layout;
    }

    static juce::AudioProcessorValueTreeState::ParameterLayout makeAllBusLayout()
    {
        std::vector<int> buses;
        for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
            buses.push_back(bus);
        return makeBusLayout(buses);
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

    static bool matrixModBytesAreZero(const Core::PatchModel& model)
    {
        for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        {
            using namespace PluginDescriptors::MatrixModulationSection;

            const auto& amountDesc = kModulationBusIntParameters[static_cast<size_t>(bus)][0];
            const auto& sourceDesc = kModulationBusChoiceParameters[static_cast<size_t>(bus)][0];
            const auto& destinationDesc = kModulationBusChoiceParameters[static_cast<size_t>(bus)][1];

            if (readModelByte(model, sourceDesc.sysExOffset) != 0)
                return false;
            if (readModelByte(model, amountDesc.sysExOffset) != 0)
                return false;
            if (readModelByte(model, destinationDesc.sysExOffset) != 0)
                return false;
        }

        return true;
    }

    void testInitBusActiveBusModelAndQueue()
    {
        beginTest("initBus active bus → model reset + one 0x0B message");

        InitTestHarness harness(makeBusLayout({ 2 }));

        configureBus(harness.model, 2, 5, -20, 12);

        using namespace PluginDescriptors::MatrixModulationSection;

        const auto& sourceDesc = kModulationBusChoiceParameters[2][0];
        const auto& amountDesc = kModulationBusIntParameters[2][0];
        const auto& destinationDesc = kModulationBusChoiceParameters[2][1];

        harness.initService.initBus(2);

        expectEquals(readModelByte(harness.model, sourceDesc.sysExOffset), static_cast<juce::uint8>(0));
        expectEquals(readModelByte(harness.model, amountDesc.sysExOffset), static_cast<juce::uint8>(0));
        expectEquals(readModelByte(harness.model, destinationDesc.sysExOffset), static_cast<juce::uint8>(0));

        auto msg = harness.queue.dequeue();
        expect(msg.has_value());
        expect(sysExMatchesMatrixModBusEdit(msg->sysExData, 2, 0, 0, 0));
        expect(harness.queue.isEmpty());
    }

    void testInitBusInvalidIndexNoOp()
    {
        beginTest("initBus invalid index → no-op");

        InitTestHarness harness(makeBusLayout({ 0 }));

        configureBus(harness.model, 0, 1, 5, 2);

        harness.initService.initBus(99);

        using namespace PluginDescriptors::MatrixModulationSection;

        expectEquals(harness.model.getChoiceIndex(kModulationBusChoiceParameters[0][0]), 1);
        expectEquals(harness.model.getValue(kModulationBusIntParameters[0][0]), 5);
        expectEquals(harness.model.getChoiceIndex(kModulationBusChoiceParameters[0][1]), 2);
        expect(harness.queue.isEmpty());
    }

    void testInitAllBusesModelAndQueue()
    {
        beginTest("initAllBuses → all bytes 104–133 zero + ten 0x0B messages ascending");

        InitTestHarness harness(makeAllBusLayout());

        configureBus(harness.model, 0, 3, 10, 4);
        configureBus(harness.model, 4, 7, -31, 15);
        configureBus(harness.model, 9, 2, 22, 8);

        harness.initService.initAllBuses();

        expect(matrixModBytesAreZero(harness.model));

        for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        {
            auto msg = harness.queue.dequeue();
            expect(msg.has_value());
            expect(sysExMatchesMatrixModBusEdit(msg->sysExData,
                                                  static_cast<juce::uint8>(bus), 0, 0, 0));
        }

        expect(harness.queue.isEmpty());
    }

    void testSysExPayloadAllZeros()
    {
        beginTest("SysEx payload → F0 10 06 0B <bus> 00 00 00 F7");

        InitTestHarness harness(makeBusLayout({ 3 }));

        configureBus(harness.model, 3, 4, 18, 6);
        harness.initService.initBus(3);

        auto msg = harness.queue.dequeue();
        expect(msg.has_value());

        const auto* data = static_cast<const juce::uint8*>(msg->sysExData.getData());
        expectEquals(data[0], SysExConstants::kSysExStart);
        expectEquals(data[1], SysExConstants::kManufacturerIdOberheim);
        expectEquals(data[2], SysExConstants::kDeviceIdMatrix1000);
        expectEquals(data[3], SysExConstants::Opcode::kRemoteParameterEditMatrix);
        expectEquals(data[4], static_cast<juce::uint8>(3));
        expectEquals(data[5], static_cast<juce::uint8>(0));
        expectEquals(data[6], static_cast<juce::uint8>(0));
        expectEquals(data[7], static_cast<juce::uint8>(0));
        expectEquals(data[8], SysExConstants::kSysExEnd);
    }

    void testApvtsMatchesPostInitModel()
    {
        beginTest("APVTS values match post-init PatchModel");

        InitTestHarness harness(makeBusLayout({ 2 }));

        configureBus(harness.model, 2, 5, -20, 12);
        harness.initService.initBus(2);

        using namespace PluginDescriptors::MatrixModulationSection;

        const auto& sourceDesc = kModulationBusChoiceParameters[2][0];
        const auto& amountDesc = kModulationBusIntParameters[2][0];
        const auto& destinationDesc = kModulationBusChoiceParameters[2][1];

        expectEquals(juce::roundToInt(harness.proc.apvts.getRawParameterValue(sourceDesc.parameterId)->load()), 0);
        expectEquals(juce::roundToInt(harness.proc.apvts.getRawParameterValue(amountDesc.parameterId)->load()), 0);
        expectEquals(juce::roundToInt(harness.proc.apvts.getRawParameterValue(destinationDesc.parameterId)->load()), 0);

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
        Core::MatrixModInitService initService;
        std::unordered_set<juce::String> patchParameterIds_;
        std::unordered_set<juce::String> matrixModParameterIds_;
        bool suppressMatrixModParameterSysEx_ { false };

        ProcessorPathHarness(juce::AudioProcessorValueTreeState::ParameterLayout layout, int busIndex)
            : TestAudioProcessor(std::move(layout))
            , mapper(apvts, model)
            , dispatcher(model,
                         [this](juce::uint8 bus, juce::uint8 source, juce::uint8 amount, juce::uint8 destination)
                         {
                             Core::EditorPath editorPath(queue, tracker);
                             editorPath.enqueueSysEx(encoder.encodeMatrixModBusEdit(bus, source, amount, destination));
                         })
            , initService(model, mapper, dispatcher)
        {
            buildParameterIdSets(busIndex);
            apvts.state.addListener(this);
        }

        ~ProcessorPathHarness() override
        {
            apvts.state.removeListener(this);
        }

        void initBusWithSuppress(int busIndex)
        {
            suppressMatrixModParameterSysEx_ = true;
            initService.initBus(busIndex);
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

            if (isMatrixModParam && !suppressMatrixModParameterSysEx_)
                dispatcher.dispatch(parameterId);
        }

    private:
        void buildParameterIdSets(int busIndex)
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

    void testProcessorPathInitWithApvtsListener()
    {
        beginTest("PluginProcessor path → APVTS listener + suppress enqueues exactly one 0x0B");

        ProcessorPathHarness harness(makeBusLayout({ 2 }), 2);

        configureBus(harness.model, 2, 5, -20, 12);
        harness.initBusWithSuppress(2);

        using namespace PluginDescriptors::MatrixModulationSection;

        const auto& sourceDesc = kModulationBusChoiceParameters[2][0];
        const auto& amountDesc = kModulationBusIntParameters[2][0];
        const auto& destinationDesc = kModulationBusChoiceParameters[2][1];

        expectEquals(harness.model.getChoiceIndex(sourceDesc), 0);
        expectEquals(harness.model.getValue(amountDesc), 0);
        expectEquals(harness.model.getChoiceIndex(destinationDesc), 0);

        expectEquals(juce::roundToInt(harness.apvts.getRawParameterValue(sourceDesc.parameterId)->load()), 0);
        expectEquals(juce::roundToInt(harness.apvts.getRawParameterValue(amountDesc.parameterId)->load()), 0);
        expectEquals(juce::roundToInt(harness.apvts.getRawParameterValue(destinationDesc.parameterId)->load()), 0);

        auto msg = harness.queue.dequeue();
        expect(msg.has_value());
        expect(sysExMatchesMatrixModBusEdit(msg->sysExData, 2, 0, 0, 0));
        expect(harness.queue.isEmpty());
    }
};

static MatrixModInitServiceTests matrixModInitServiceTests;
