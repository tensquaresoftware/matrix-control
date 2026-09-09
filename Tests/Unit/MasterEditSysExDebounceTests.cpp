#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MasterParameterSysExDispatcher.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Util/ComboboxPatchSendDebouncer.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{

constexpr int kTestDebounceMs = 20;
constexpr juce::uint8 kMasterVersion = 0x03;

// Mirrors PluginProcessor::dispatchMasterParameterChange + firePendingMasterEditSysEx.
struct MasterEditSysExDebounceHarness
{
    Core::MasterModel model;
    Core::MidiOutboundQueue queue;
    Core::MidiActivityTracker tracker;
    SysExEncoder encoder;
    Core::MasterParameterSysExDispatcher dispatcher;
    Core::ComboboxPatchSendDebouncer debouncer;
    bool suppressMasterParameterSysEx = false;
    bool editorialQuiet = false;
    bool outboundAllowed = true;
    int enqueueCount = 0;

    MasterEditSysExDebounceHarness()
        : dispatcher(model,
                     [this](const juce::uint8* packedData)
                     {
                         ++enqueueCount;
                         Core::EditorPath editorPath(queue, tracker);
                         editorPath.enqueueSysEx(encoder.encodeMasterSysEx(kMasterVersion, packedData));
                     })
        , debouncer(kTestDebounceMs)
    {
    }

    void noteMasterParameterChanged(const juce::String& parameterId)
    {
        // Immediate model update (caller already wrote the buffer); only SysEx is deferred.
        juce::ignoreUnused(parameterId);

        if (suppressMasterParameterSysEx || editorialQuiet)
            return;

        if (! outboundAllowed)
            return;

        debouncer.schedule([this] { firePendingMasterEditSysEx(); });
    }

    void firePendingMasterEditSysEx()
    {
        if (suppressMasterParameterSysEx || editorialQuiet)
            return;

        if (! outboundAllowed)
            return;

        dispatcher.dispatchFull();
    }

    void cancelPending() noexcept { debouncer.cancel(); }

    void flushPendingSynchronouslyForTests()
    {
        debouncer.flushPendingSynchronouslyForTests();
    }

    // INIT / file-load: cancel pending, then one intentional dispatchFull.
    void dispatchFullImmediateAfterCancel()
    {
        cancelPending();
        dispatcher.dispatchFull();
    }

    int drainMasterEnqueueCount()
    {
        int count = 0;

        while (! queue.isEmpty())
        {
            const auto msg = queue.dequeue();
            if (! msg.has_value())
                break;

            if (msg->sysExData.getSize() != SysExConstants::kMasterMessageLength)
                continue;

            const auto* data = static_cast<const juce::uint8*>(msg->sysExData.getData());
            if (data[3] == SysExConstants::Opcode::kMasterParameterData)
                ++count;
        }

        return count;
    }
};

} // namespace

class MasterEditSysExDebounceTests : public juce::UnitTest
{
public:
    MasterEditSysExDebounceTests() : juce::UnitTest("MasterEditSysExDebounce") {}

    void runTest() override
    {
        rapidChanges_coalesceToOneEnqueue();
        cancel_dropsPendingWithoutEnqueue();
        initOrLoad_cancelThenDispatchFull_noStaleDoubleSend();
        gateFalseAtFire_noEnqueue();
        suppressOrQuiet_doesNotArmDebounce();
        suppressOrQuietAfterSchedule_dropsAtFire();
        secondBurstAfterFlush_sendsAgain();
    }

private:
    static const char* pedal1ParameterId()
    {
        return PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPedal1Select;
    }

    void rapidChanges_coalesceToOneEnqueue()
    {
        beginTest("N rapid Master changes coalesce to one enqueue after sync flush");

        MasterEditSysExDebounceHarness harness;
        const auto intDescs = Core::ApvtsMasterMapper::buildIntDescriptors();
        const auto* pedal1 = [&]() -> const PluginDescriptors::IntParameterDescriptor*
        {
            for (const auto& d : intDescs)
            {
                if (d.parameterId == pedal1ParameterId())
                    return &d;
            }
            return nullptr;
        }();
        expect(pedal1 != nullptr);
        if (pedal1 == nullptr)
            return;

        for (int value = 1; value <= 5; ++value)
        {
            harness.model.setValue(*pedal1, value);
            harness.noteMasterParameterChanged(pedal1->parameterId);
        }

        expectEquals(harness.drainMasterEnqueueCount(), 0);
        harness.flushPendingSynchronouslyForTests();
        expectEquals(harness.drainMasterEnqueueCount(), 1);
        expectEquals(harness.enqueueCount, 1);
        expectEquals(harness.model.getValue(*pedal1), 5);
    }

    void cancel_dropsPendingWithoutEnqueue()
    {
        beginTest("cancel drops pending Master debounce");

        MasterEditSysExDebounceHarness harness;
        harness.noteMasterParameterChanged(pedal1ParameterId());
        harness.cancelPending();
        harness.flushPendingSynchronouslyForTests();

        expectEquals(harness.drainMasterEnqueueCount(), 0);
        expectEquals(harness.enqueueCount, 0);
    }

    void initOrLoad_cancelThenDispatchFull_noStaleDoubleSend()
    {
        beginTest("INIT/load path cancels pending then one dispatchFull with no stale fire");

        MasterEditSysExDebounceHarness harness;
        harness.noteMasterParameterChanged(pedal1ParameterId());
        expect(harness.debouncer.isPending());

        harness.dispatchFullImmediateAfterCancel();
        expectEquals(harness.drainMasterEnqueueCount(), 1);

        harness.flushPendingSynchronouslyForTests();
        expectEquals(harness.drainMasterEnqueueCount(), 0);
        expectEquals(harness.enqueueCount, 1);
    }

    void gateFalseAtFire_noEnqueue()
    {
        beginTest("gate false at fire yields no enqueue");

        MasterEditSysExDebounceHarness harness;
        harness.noteMasterParameterChanged(pedal1ParameterId());
        harness.outboundAllowed = false;
        harness.flushPendingSynchronouslyForTests();

        expectEquals(harness.drainMasterEnqueueCount(), 0);
        expectEquals(harness.enqueueCount, 0);
    }

    void suppressOrQuiet_doesNotArmDebounce()
    {
        beginTest("suppress / editorial quiet does not arm debounce");

        MasterEditSysExDebounceHarness harness;
        harness.suppressMasterParameterSysEx = true;
        harness.noteMasterParameterChanged(pedal1ParameterId());
        expect(! harness.debouncer.isPending());

        harness.suppressMasterParameterSysEx = false;
        harness.editorialQuiet = true;
        harness.noteMasterParameterChanged(pedal1ParameterId());
        expect(! harness.debouncer.isPending());

        harness.flushPendingSynchronouslyForTests();
        expectEquals(harness.enqueueCount, 0);
    }

    void suppressOrQuietAfterSchedule_dropsAtFire()
    {
        beginTest("suppress or quiet after schedule drops at fire");

        {
            MasterEditSysExDebounceHarness harness;
            harness.noteMasterParameterChanged(pedal1ParameterId());
            expect(harness.debouncer.isPending());
            harness.suppressMasterParameterSysEx = true;
            harness.flushPendingSynchronouslyForTests();
            expectEquals(harness.enqueueCount, 0);
        }

        {
            MasterEditSysExDebounceHarness harness;
            harness.noteMasterParameterChanged(pedal1ParameterId());
            expect(harness.debouncer.isPending());
            harness.editorialQuiet = true;
            harness.flushPendingSynchronouslyForTests();
            expectEquals(harness.enqueueCount, 0);
        }
    }

    void secondBurstAfterFlush_sendsAgain()
    {
        beginTest("second burst after flush sends again");

        MasterEditSysExDebounceHarness harness;
        harness.noteMasterParameterChanged(pedal1ParameterId());
        harness.flushPendingSynchronouslyForTests();
        expectEquals(harness.enqueueCount, 1);

        harness.noteMasterParameterChanged(pedal1ParameterId());
        harness.flushPendingSynchronouslyForTests();
        expectEquals(harness.enqueueCount, 2);
        expectEquals(harness.drainMasterEnqueueCount(), 2);
    }
};

static MasterEditSysExDebounceTests masterEditSysExDebounceTests;
