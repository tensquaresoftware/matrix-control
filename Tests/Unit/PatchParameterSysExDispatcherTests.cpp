#include <vector>

#include <juce_core/juce_core.h>

#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/PatchParameterSysExDispatcher.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PackedFieldCodec.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/Models/PatchModel.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

class PatchParameterSysExDispatcherTests : public juce::UnitTest
{
public:
    PatchParameterSysExDispatcherTests() : juce::UnitTest("PatchParameterSysExDispatcher") {}

    void runTest() override
    {
        testIntParameterSixBit();
        testIntParameterSevenBitSigned();
        testChoiceParameter();
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

    static bool sysExMatchesRemoteEdit(const juce::MemoryBlock& block,
                                       juce::uint8 expectedParam,
                                       juce::uint8 expectedValue)
    {
        if (block.getSize() != 7)
            return false;

        const auto* data = static_cast<const juce::uint8*>(block.getData());
        return data[0] == SysExConstants::kSysExStart
            && data[1] == SysExConstants::kManufacturerIdOberheim
            && data[2] == SysExConstants::kDeviceIdMatrix1000
            && data[3] == SysExConstants::Opcode::kRemoteParameterEdit
            && data[4] == expectedParam
            && data[5] == expectedValue
            && data[6] == SysExConstants::kSysExEnd;
    }

    void testIntParameterSixBit()
    {
        beginTest("Int 6-bit — DCO1 Frequency → param 0, packed byte");

        Core::PatchModel model;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;

        const auto intDescs = Core::ApvtsPatchMapper::buildIntDescriptors();
        const auto* frequencyDesc = findDescriptorByParameterId(
            intDescs,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency);
        expect(frequencyDesc != nullptr);
        expect(frequencyDesc->sysExId == 0);

        model.setValue(*frequencyDesc, 42);

        Core::PatchParameterSysExDispatcher dispatcher(
            model,
            [&](int parameterNumber, juce::uint8 packedValue)
            {
                Core::EditorPath editorPath(queue, tracker);
                editorPath.enqueueSysEx(encoder.encodeRemoteParameterEdit(
                    static_cast<juce::uint8>(parameterNumber),
                    packedValue));
            });

        dispatcher.dispatch(frequencyDesc->parameterId);

        auto msg = queue.dequeue();
        expect(msg.has_value());
        expect(sysExMatchesRemoteEdit(msg->sysExData, 0, 42));
        expect(queue.isEmpty());
    }

    void testIntParameterSevenBitSigned()
    {
        beginTest("Int 7-bit signed — DCO1 Freq mod LFO1 → param 1, packed byte");

        Core::PatchModel model;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;

        const auto intDescs = Core::ApvtsPatchMapper::buildIntDescriptors();
        const auto* modDesc = findDescriptorByParameterId(
            intDescs,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequencyModByLfo1);
        expect(modDesc != nullptr);
        expect(modDesc->sysExId == 1);
        expect(modDesc->minValue < 0);

        model.setValue(*modDesc, -5);

        Core::PatchParameterSysExDispatcher dispatcher(
            model,
            [&](int parameterNumber, juce::uint8 packedValue)
            {
                Core::EditorPath editorPath(queue, tracker);
                editorPath.enqueueSysEx(encoder.encodeRemoteParameterEdit(
                    static_cast<juce::uint8>(parameterNumber),
                    packedValue));
            });

        dispatcher.dispatch(modDesc->parameterId);

        const size_t offset = Core::PackedFieldCodec::safeOffset(modDesc->sysExOffset, Core::PatchModel::kBufferSize);
        const juce::uint8 expectedPacked = model.data()[offset];

        auto msg = queue.dequeue();
        expect(msg.has_value());
        expect(sysExMatchesRemoteEdit(msg->sysExData, 1, expectedPacked));
        expect(queue.isEmpty());
    }

    void testChoiceParameter()
    {
        beginTest("Choice — DCO1 Sync enqueues 0x06");

        const auto choiceDescs = Core::ApvtsPatchMapper::buildChoiceDescriptors();
        const auto* desc = findDescriptorByParameterId(
            choiceDescs,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kSync);
        expect(desc != nullptr);
        expect(desc->sysExId != PluginDescriptors::kNoSysExId);

        Core::PatchModel model;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;

        model.setChoiceIndex(*desc, 2);

        Core::PatchParameterSysExDispatcher dispatcher(
            model,
            [&](int parameterNumber, juce::uint8 packedValue)
            {
                Core::EditorPath editorPath(queue, tracker);
                editorPath.enqueueSysEx(encoder.encodeRemoteParameterEdit(
                    static_cast<juce::uint8>(parameterNumber),
                    packedValue));
            });

        dispatcher.dispatch(desc->parameterId);

        auto msg = queue.dequeue();
        expect(msg.has_value());
        expect(sysExMatchesRemoteEdit(msg->sysExData,
                                       static_cast<juce::uint8>(desc->sysExId),
                                       2));
        expect(queue.isEmpty());
    }

    void testUnknownParameterNoEnqueue()
    {
        // AC #5: no PatchEdit descriptor uses kNoSysExId; skip branch is defensive.
        // Unknown parameterId covers the no-enqueue path; Matrix Mod (2.6) is out of scope here.
        beginTest("Unknown parameterId — queue stays empty");

        Core::PatchModel model;
        Core::MidiOutboundQueue queue;

        Core::PatchParameterSysExDispatcher dispatcher(
            model,
            [&](int, juce::uint8)
            {
                jassertfalse;
            });

        dispatcher.dispatch("nonexistent.parameter.id");
        expect(queue.isEmpty());
    }
};

static PatchParameterSysExDispatcherTests patchParameterSysExDispatcherTests;
