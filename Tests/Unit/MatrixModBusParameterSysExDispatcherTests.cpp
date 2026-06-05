#include <vector>

#include <juce_core/juce_core.h>

#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MatrixModBusParameterSysExDispatcher.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/Models/PackedFieldCodec.h"
#include "Core/Models/PatchModel.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

class MatrixModBusParameterSysExDispatcherTests : public juce::UnitTest
{
public:
    MatrixModBusParameterSysExDispatcherTests() : juce::UnitTest("MatrixModBusParameterSysExDispatcher") {}

    void runTest() override
    {
        testSourceAmountDestinationProduceIdenticalMessage();
        testBus3UsesBusByte3();
        testUnknownParameterNoEnqueue();
    }

private:
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

    static void configureBus0(Core::PatchModel& model,
                              int sourceIndex,
                              int amount,
                              int destinationIndex)
    {
        using namespace PluginDescriptors::MatrixModulationSection;

        const auto& intParams = kModulationBusIntParameters[0];
        const auto& choiceParams = kModulationBusChoiceParameters[0];

        model.setChoiceIndex(choiceParams[0], sourceIndex);
        model.setValue(intParams[0], amount);
        model.setChoiceIndex(choiceParams[1], destinationIndex);
    }

    void testSourceAmountDestinationProduceIdenticalMessage()
    {
        beginTest("Source / amount / destination on same bus → identical 0x0B message");

        using namespace PluginDescriptors::MatrixModulationSection;

        Core::PatchModel model;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;

        configureBus0(model, 5, -20, 12);

        const auto& intParams = kModulationBusIntParameters[0];

        const size_t amountOffset = Core::PackedFieldCodec::safeOffset(intParams[0].sysExOffset,
                                                                       Core::PatchModel::kBufferSize);
        const juce::uint8 expectedAmount = model.data()[amountOffset];
        const juce::uint8 expectedSource = static_cast<juce::uint8>(5);
        const juce::uint8 expectedDestination = static_cast<juce::uint8>(12);

        const auto expectedMessage = encoder.encodeMatrixModBusEdit(
            0, expectedSource, expectedAmount, expectedDestination);

        Core::MatrixModBusParameterSysExDispatcher dispatcher(
            model,
            [&](juce::uint8 bus, juce::uint8 source, juce::uint8 amount, juce::uint8 destination)
            {
                Core::EditorPath editorPath(queue, tracker);
                editorPath.enqueueSysEx(encoder.encodeMatrixModBusEdit(bus, source, amount, destination));
            });

        const std::vector<const char*> parameterIds {
            PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Source,
            PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Amount,
            PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Destination
        };

        for (const auto* parameterId : parameterIds)
        {
            dispatcher.dispatch(parameterId);

            auto msg = queue.dequeue();
            expect(msg.has_value());
            expect(msg->sysExData == expectedMessage);
            expect(sysExMatchesMatrixModBusEdit(msg->sysExData, 0, expectedSource, expectedAmount, expectedDestination));
        }

        expect(queue.isEmpty());
    }

    void testBus3UsesBusByte3()
    {
        beginTest("Bus 3 → bus byte 3 in 0x0B message");

        using namespace PluginDescriptors::MatrixModulationSection;

        Core::PatchModel model;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;

        const auto& intParams = kModulationBusIntParameters[3];
        const auto& choiceParams = kModulationBusChoiceParameters[3];

        model.setChoiceIndex(choiceParams[0], 2);
        model.setValue(intParams[0], 31);
        model.setChoiceIndex(choiceParams[1], 7);

        Core::MatrixModBusParameterSysExDispatcher dispatcher(
            model,
            [&](juce::uint8 bus, juce::uint8 source, juce::uint8 amount, juce::uint8 destination)
            {
                Core::EditorPath editorPath(queue, tracker);
                editorPath.enqueueSysEx(encoder.encodeMatrixModBusEdit(bus, source, amount, destination));
            });

        dispatcher.dispatch(PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Source);

        auto msg = queue.dequeue();
        expect(msg.has_value());
        expect(sysExMatchesMatrixModBusEdit(msg->sysExData, 3, 2, 31, 7));
        expect(queue.isEmpty());
    }

    void testUnknownParameterNoEnqueue()
    {
        beginTest("Unknown parameterId → no enqueue");

        Core::PatchModel model;
        Core::MidiOutboundQueue queue;

        Core::MatrixModBusParameterSysExDispatcher dispatcher(
            model,
            [&](juce::uint8, juce::uint8, juce::uint8, juce::uint8)
            {
                jassertfalse;
            });

        dispatcher.dispatch("notAMatrixModParameter");
        expect(queue.isEmpty());
    }
};

static MatrixModBusParameterSysExDispatcherTests matrixModBusParameterSysExDispatcherTests;
