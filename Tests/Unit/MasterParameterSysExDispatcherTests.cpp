#include <cstring>

#include <juce_core/juce_core.h>

#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MasterParameterSysExDispatcher.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/MasterModel.h"
#include "Shared/Definitions/PluginIDs.h"

class MasterParameterSysExDispatcherTests : public juce::UnitTest
{
public:
    MasterParameterSysExDispatcherTests() : juce::UnitTest("MasterParameterSysExDispatcher") {}

    void runTest() override
    {
        testIntParameterEnqueuesFullMaster();
        testChoiceParameterEnqueuesFullMaster();
        testUnknownParameterNoEnqueue();
    }

private:
    template<typename Descriptor>
    static const Descriptor* findDescriptorByParameterId(const std::vector<Descriptor>& descriptors,
                                                         const char* parameterId)
    {
        for (const auto& descriptor : descriptors)
        {
            if (descriptor.parameterId == parameterId)
                return &descriptor;
        }

        return nullptr;
    }

    static bool sysExMatchesMasterMessage(const juce::MemoryBlock& block, juce::uint8 expectedVersion)
    {
        if (block.getSize() != SysExConstants::kMasterMessageLength)
            return false;

        const auto* data = static_cast<const juce::uint8*>(block.getData());
        return data[0] == SysExConstants::kSysExStart
            && data[1] == SysExConstants::kManufacturerIdOberheim
            && data[2] == SysExConstants::kDeviceIdMatrix1000
            && data[3] == SysExConstants::Opcode::kMasterParameterData
            && data[4] == expectedVersion
            && data[block.getSize() - 1] == SysExConstants::kSysExEnd;
    }

    void testIntParameterEnqueuesFullMaster()
    {
        beginTest("Int master param — Pedal1 Select → full 0x03 message");

        Core::MasterModel model;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;

        constexpr juce::uint8 kMasterVersion = 0x03;

        const auto intDescs = Core::ApvtsMasterMapper::buildIntDescriptors();
        const auto* pedal1Desc = findDescriptorByParameterId(
            intDescs,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPedal1Select);
        expect(pedal1Desc != nullptr);

        model.setValue(*pedal1Desc, 7);

        Core::MasterParameterSysExDispatcher dispatcher(
            model,
            [&](const juce::uint8* packedData)
            {
                Core::EditorPath editorPath(queue, tracker);
                editorPath.enqueueSysEx(encoder.encodeMasterSysEx(kMasterVersion, packedData));
            });

        dispatcher.dispatch(pedal1Desc->parameterId);

        auto msg = queue.dequeue();
        expect(msg.has_value());
        expect(sysExMatchesMasterMessage(msg->sysExData, kMasterVersion));

        const auto expected = encoder.encodeMasterSysEx(kMasterVersion, model.data());
        expect(msg->sysExData == expected);
        expect(queue.isEmpty());
    }

    void testChoiceParameterEnqueuesFullMaster()
    {
        beginTest("Choice master param — MIDI Channel → full 0x03 message");

        Core::MasterModel model;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;

        constexpr juce::uint8 kMasterVersion = 0x03;

        const auto choiceDescs = Core::ApvtsMasterMapper::buildChoiceDescriptors();
        const auto* channelDesc = findDescriptorByParameterId(
            choiceDescs,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel);
        expect(channelDesc != nullptr);

        model.setChoiceIndex(*channelDesc, 3);

        Core::MasterParameterSysExDispatcher dispatcher(
            model,
            [&](const juce::uint8* packedData)
            {
                Core::EditorPath editorPath(queue, tracker);
                editorPath.enqueueSysEx(encoder.encodeMasterSysEx(kMasterVersion, packedData));
            });

        dispatcher.dispatch(channelDesc->parameterId);

        auto msg = queue.dequeue();
        expect(msg.has_value());
        expect(sysExMatchesMasterMessage(msg->sysExData, kMasterVersion));
        expect(queue.isEmpty());
    }

    void testUnknownParameterNoEnqueue()
    {
        beginTest("Unknown parameterId → no enqueue");

        Core::MasterModel model;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;

        Core::MasterParameterSysExDispatcher dispatcher(
            model,
            [&](const juce::uint8* packedData)
            {
                juce::ignoreUnused(packedData);
                Core::EditorPath editorPath(queue, tracker);
                editorPath.enqueueSysEx(juce::MemoryBlock());
            });

        dispatcher.dispatch("notAMasterParameter");
        expect(queue.isEmpty());
    }
};

static MasterParameterSysExDispatcherTests masterParameterSysExDispatcherTests;
