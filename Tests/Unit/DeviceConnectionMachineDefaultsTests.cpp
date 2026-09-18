#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "Core/Services/DeviceConnectionMachineDefaults.h"
#include "Core/Services/DeviceSetupDeviceRow.h"
#include "Shared/Definitions/PluginIDs.h"

class DeviceConnectionMachineDefaultsTests : public juce::UnitTest
{
public:
    DeviceConnectionMachineDefaultsTests()
        : juce::UnitTest("DeviceConnectionMachineDefaults")
    {
    }

    void runTest() override
    {
        testSeedApvtsFromMachineDefaults();
        testApplyConfirmWritesAllFields();
        testApplySpecifyLaterLeavesEpromUnchanged();
        testApplyMidiPortsAndEpromIndependently();
        testApplySinglePortWriteThrough();
        testShouldReseedAfterHostRestore();
        testOpenBlockedWhenMachinePromptDoneEvenIfSessionFalse();
        testOpenBlockedWhenSessionPromptDoneWithEmptyPorts();
    }

private:
    static juce::ValueTree makeEmptyConnectionState()
    {
        juce::ValueTree state("PARAMETERS");
        state.setProperty(PluginIDs::Settings::kEpromTypePromptDone, false, nullptr);
        state.setProperty("midiInputPortId", juce::String(), nullptr);
        state.setProperty("midiOutputPortId", juce::String(), nullptr);
        state.setProperty(PluginIDs::Settings::kEpromType,
                          PluginIDs::Settings::EpromType::kDefault,
                          nullptr);
        return state;
    }

    void testSeedApvtsFromMachineDefaults()
    {
        beginTest("seedApvtsState - fresh instance inherits machine defaults");

        Core::DeviceConnectionMachineDefaults::Values machine;
        machine.promptDone = true;
        machine.midiInputPortId = "in-port";
        machine.midiOutputPortId = "out-port";
        machine.epromTypeId = PluginIDs::Settings::EpromType::kTauntek;

        auto state = makeEmptyConnectionState();
        Core::DeviceConnectionMachineDefaults::seedApvtsState(state, machine);

        expect(static_cast<bool>(state.getProperty(PluginIDs::Settings::kEpromTypePromptDone)));
        expectEquals(state.getProperty("midiInputPortId").toString(), juce::String("in-port"));
        expectEquals(state.getProperty("midiOutputPortId").toString(), juce::String("out-port"));
        expectEquals(static_cast<int>(state.getProperty(PluginIDs::Settings::kEpromType)),
                     PluginIDs::Settings::EpromType::kTauntek);
        expect(! Core::shouldOpenDeviceSetupAssistant(
            static_cast<bool>(state.getProperty(PluginIDs::Settings::kEpromTypePromptDone)),
            false));
    }

    void testApplyConfirmWritesAllFields()
    {
        beginTest("applyConfirm - promptDone + ports + EPROM");

        Core::DeviceConnectionMachineDefaults::Values prior;
        prior.promptDone = false;
        prior.epromTypeId = PluginIDs::Settings::EpromType::kUnknown;

        const auto next = Core::DeviceConnectionMachineDefaults::applyConfirm(
            prior,
            PluginIDs::Settings::EpromType::kGligli,
            "from",
            "to");

        expect(next.promptDone);
        expectEquals(next.epromTypeId, PluginIDs::Settings::EpromType::kGligli);
        expectEquals(next.midiInputPortId, juce::String("from"));
        expectEquals(next.midiOutputPortId, juce::String("to"));
    }

    void testApplySpecifyLaterLeavesEpromUnchanged()
    {
        beginTest("applySpecifyLater - promptDone + ports; EPROM unchanged");

        Core::DeviceConnectionMachineDefaults::Values prior;
        prior.promptDone = false;
        prior.epromTypeId = PluginIDs::Settings::EpromType::kFactory;
        prior.midiInputPortId = "old-in";

        const auto next = Core::DeviceConnectionMachineDefaults::applySpecifyLater(
            prior, "new-in", "new-out");

        expect(next.promptDone);
        expectEquals(next.epromTypeId, PluginIDs::Settings::EpromType::kFactory);
        expectEquals(next.midiInputPortId, juce::String("new-in"));
        expectEquals(next.midiOutputPortId, juce::String("new-out"));
    }

    void testApplyMidiPortsAndEpromIndependently()
    {
        beginTest("applyMidiInput/OutputPort / applyEpromType - partial write-through");

        Core::DeviceConnectionMachineDefaults::Values prior;
        prior.promptDone = true;
        prior.epromTypeId = PluginIDs::Settings::EpromType::kTauntek;
        prior.midiInputPortId = "a";
        prior.midiOutputPortId = "b";

        const auto inOnly = Core::DeviceConnectionMachineDefaults::applyMidiInputPort(prior, "c");
        expect(inOnly.promptDone);
        expectEquals(inOnly.epromTypeId, PluginIDs::Settings::EpromType::kTauntek);
        expectEquals(inOnly.midiInputPortId, juce::String("c"));
        expectEquals(inOnly.midiOutputPortId, juce::String("b"));

        const auto outOnly = Core::DeviceConnectionMachineDefaults::applyMidiOutputPort(prior, "d");
        expectEquals(outOnly.midiInputPortId, juce::String("a"));
        expectEquals(outOnly.midiOutputPortId, juce::String("d"));

        const auto epromOnly = Core::DeviceConnectionMachineDefaults::applyEpromType(
            prior, PluginIDs::Settings::EpromType::kUntergeek);
        expect(epromOnly.promptDone);
        expectEquals(epromOnly.epromTypeId, PluginIDs::Settings::EpromType::kUntergeek);
        expectEquals(epromOnly.midiInputPortId, juce::String("a"));
        expectEquals(epromOnly.midiOutputPortId, juce::String("b"));
    }

    void testApplySinglePortWriteThrough()
    {
        beginTest("single-side port write - other side preserved; empty clears that side");

        Core::DeviceConnectionMachineDefaults::Values prior;
        prior.midiInputPortId = "stored-in";
        prior.midiOutputPortId = "stored-out";

        const auto keepOut = Core::DeviceConnectionMachineDefaults::applyMidiInputPort(
            prior, "new-in");
        expectEquals(keepOut.midiInputPortId, juce::String("new-in"));
        expectEquals(keepOut.midiOutputPortId, juce::String("stored-out"));

        const auto keepIn = Core::DeviceConnectionMachineDefaults::applyMidiOutputPort(
            prior, "new-out");
        expectEquals(keepIn.midiInputPortId, juce::String("stored-in"));
        expectEquals(keepIn.midiOutputPortId, juce::String("new-out"));

        const auto clearIn = Core::DeviceConnectionMachineDefaults::applyMidiInputPort(prior, {});
        expectEquals(clearIn.midiInputPortId, juce::String());
        expectEquals(clearIn.midiOutputPortId, juce::String("stored-out"));

        const auto clearOut = Core::DeviceConnectionMachineDefaults::applyMidiOutputPort(prior, {});
        expectEquals(clearOut.midiInputPortId, juce::String("stored-in"));
        expectEquals(clearOut.midiOutputPortId, juce::String());
    }

    void testShouldReseedAfterHostRestore()
    {
        beginTest("shouldReseedAfterHostRestore - only when machine done and session not");

        expect(Core::DeviceConnectionMachineDefaults::shouldReseedAfterHostRestore(true, false));
        expect(! Core::DeviceConnectionMachineDefaults::shouldReseedAfterHostRestore(true, true));
        expect(! Core::DeviceConnectionMachineDefaults::shouldReseedAfterHostRestore(false, false));
        expect(! Core::DeviceConnectionMachineDefaults::shouldReseedAfterHostRestore(false, true));
    }

    void testOpenBlockedWhenMachinePromptDoneEvenIfSessionFalse()
    {
        beginTest("shouldOpen - machine promptDone blocks restored empty-port sessions");

        expect(! Core::shouldOpenDeviceSetupAssistant(false, false, true));
        expect(Core::shouldOpenDeviceSetupAssistant(false, false, false));
    }

    void testOpenBlockedWhenSessionPromptDoneWithEmptyPorts()
    {
        beginTest("shouldOpen - session promptDone blocks even with empty ports");

        expect(! Core::shouldOpenDeviceSetupAssistant(true, false, false));
        expect(! Core::shouldOpenDeviceSetupAssistant(true, false, true));
    }
};

static DeviceConnectionMachineDefaultsTests deviceConnectionMachineDefaultsTests;
