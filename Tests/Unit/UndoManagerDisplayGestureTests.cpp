#include <optional>
#include <unordered_set>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/PatchParameterSysExDispatcher.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PackedFieldCodec.h"
#include "Core/Models/PatchModel.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Shared/Definitions/ApvtsTypes.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
using IntDesc = PluginDescriptors::IntParameterDescriptor;

std::optional<IntDesc> findIntDescriptor(const juce::String& parameterId)
{
    for (const auto& d : Core::ApvtsPatchMapper::buildIntDescriptors())
    {
        if (d.parameterId == parameterId)
            return d;
    }

    return std::nullopt;
}

bool sysExMatchesRemoteEdit(const juce::MemoryBlock& block,
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

juce::AudioProcessorValueTreeState::ParameterLayout makeDisplayGestureLayout()
{
    const auto attackDesc = findIntDescriptor(
        PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack);
    const auto trackPointDesc = findIntDescriptor(
        PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3);
    jassert(attackDesc.has_value());
    jassert(trackPointDesc.has_value());

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID(attackDesc->parameterId, 1),
        attackDesc->hostName,
        attackDesc->minValue,
        attackDesc->maxValue,
        attackDesc->defaultValue));
    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID(trackPointDesc->parameterId, 1),
        trackPointDesc->hostName,
        trackPointDesc->minValue,
        trackPointDesc->maxValue,
        trackPointDesc->defaultValue));

    return layout;
}

int readIntParameterValue(const juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterId)
{
    return juce::roundToInt(apvts.getParameterAsValue(parameterId).getValue());
}

void writeIntParameterValue(juce::AudioProcessorValueTreeState& apvts,
                            const juce::String& parameterId,
                            int value)
{
    apvts.getParameterAsValue(parameterId).setValue(static_cast<float>(value));
}

void simulateDisplayDragWithoutUndoManager(juce::AudioProcessorValueTreeState& apvts,
                                           const juce::String& parameterId,
                                           const std::vector<int>& dragValues)
{
    if (auto* param = apvts.getParameter(parameterId))
        param->beginChangeGesture();

    for (int value : dragValues)
        writeIntParameterValue(apvts, parameterId, value);

    if (auto* param = apvts.getParameter(parameterId))
        param->endChangeGesture();
}

class UndoTestAudioProcessor : public juce::AudioProcessor
{
public:
    explicit UndoTestAudioProcessor(juce::AudioProcessorValueTreeState::ParameterLayout layout,
                                    juce::UndoManager* undoManagerToUse)
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, undoManagerToUse, "P", std::move(layout))
    {
    }

    juce::AudioProcessorValueTreeState apvts;

    const juce::String getName() const override { return "UndoDisplayGestureHarness"; }
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

struct ProcessorPathHarness : UndoTestAudioProcessor, juce::ValueTree::Listener
{
    juce::UndoManager undoManager;
    Core::PatchModel model;
    Core::ApvtsPatchMapper mapper;
    Core::MidiOutboundQueue queue;
    Core::MidiActivityTracker tracker;
    SysExEncoder encoder;
    Core::PatchParameterSysExDispatcher dispatcher;
    std::unordered_set<juce::String> patchParameterIds_;

    explicit ProcessorPathHarness(juce::AudioProcessorValueTreeState::ParameterLayout layout)
        : UndoTestAudioProcessor(std::move(layout), &undoManager)
        , mapper(apvts, model)
        , dispatcher(model,
                     [this](int parameterNumber, juce::uint8 packedValue)
                     {
                         const bool deviceDetected = static_cast<bool>(
                             apvts.state.getProperty("deviceDetected", false));
                         const auto deviceType = MatrixDeviceTypes::fromApvtsString(
                             apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName).toString());
                         const auto encoded = encoder.encodeRemoteParameterEdit(
                             static_cast<juce::uint8>(parameterNumber),
                             packedValue);

                         if (!Core::maySendEditorSysEx(deviceDetected, deviceType, encoded))
                             return;

                         Core::EditorPath editorPath(queue, tracker);
                         editorPath.enqueueSysEx(encoded);
                     })
    {
        patchParameterIds_.insert(
            PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack);
        patchParameterIds_.insert(
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3);
        apvts.state.addListener(this);
    }

    ~ProcessorPathHarness() override
    {
        apvts.state.removeListener(this);
    }

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override
    {
        const juce::String parameterId = resolveParameterIdFromTree(treeWhosePropertyHasChanged, property);

        if (patchParameterIds_.count(parameterId) == 0)
            return;

        mapper.apvtsToBuffer();
        dispatcher.dispatch(parameterId);
    }

    juce::uint8 packedByteFor(const juce::String& parameterId) const
    {
        const auto desc = findIntDescriptor(parameterId);
        jassert(desc.has_value());
        const size_t offset = Core::PackedFieldCodec::safeOffset(desc->sysExOffset, Core::PatchModel::kBufferSize);
        return model.data()[offset];
    }

private:
    static juce::String resolveParameterIdFromTree(juce::ValueTree& tree, const juce::Identifier& property)
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

void simulateDisplayDrag(ProcessorPathHarness& harness,
                         const juce::String& parameterId,
                         const juce::String& transactionName,
                         const std::vector<int>& dragValues)
{
    harness.undoManager.beginNewTransaction(transactionName);

    if (auto* param = harness.apvts.getParameter(parameterId))
        param->beginChangeGesture();

    for (int value : dragValues)
        writeIntParameterValue(harness.apvts, parameterId, value);

    if (auto* param = harness.apvts.getParameter(parameterId))
        param->endChangeGesture();
}
} // namespace

class UndoManagerDisplayGestureTests : public juce::UnitTest
{
public:
    UndoManagerDisplayGestureTests() : juce::UnitTest("UndoManagerDisplayGesture") {}

    void runTest() override
    {
        testEnvelopeDisplayDragGroupsOneTransaction();
        testTrackGeneratorDisplayDragGroupsOneTransaction();
        testTwoConsecutiveDisplayDragsCreateTwoTransactions();
        testUndoDispatchesSysExForDisplayEdit();
        testTrackGeneratorUndoDispatchesSysEx();
        testDisplayDragUndoRedoRestoresValues();
        testTrackGeneratorDisplayDragUndoRedoRestoresValues();
        testExternalParamWriteDuringGestureStaysOneTransaction();
        testNullUndoManagerDragDoesNotCrash();
    }

private:
    void testEnvelopeDisplayDragGroupsOneTransaction()
    {
        beginTest("Envelope display drag groups intermediate samples into one undo transaction");

        // Arrange
        const auto attackId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack;
        ProcessorPathHarness harness(makeDisplayGestureLayout());

        writeIntParameterValue(harness.apvts, attackId, 10);
        harness.mapper.apvtsToBuffer();
        harness.undoManager.clearUndoHistory();

        // Act
        simulateDisplayDrag(harness, attackId, "Envelope edit", { 15, 20, 25, 30 });
        expectEquals(harness.undoManager.getUndoDescriptions().size(), 1);
        harness.undoManager.undo();

        // Assert
        expectEquals(readIntParameterValue(harness.apvts, attackId), 10);
    }

    void testTrackGeneratorDisplayDragGroupsOneTransaction()
    {
        beginTest("Track Generator display drag records one undo transaction");

        // Arrange
        const auto trackPointId = PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3;
        ProcessorPathHarness harness(makeDisplayGestureLayout());

        writeIntParameterValue(harness.apvts, trackPointId, 5);
        harness.mapper.apvtsToBuffer();
        harness.undoManager.clearUndoHistory();

        // Act
        simulateDisplayDrag(harness, trackPointId, "Track Generator edit", { 12, 28, 40 });
        expectEquals(harness.undoManager.getUndoDescriptions().size(), 1);
        harness.undoManager.undo();

        // Assert
        expectEquals(readIntParameterValue(harness.apvts, trackPointId), 5);
    }

    void testTwoConsecutiveDisplayDragsCreateTwoTransactions()
    {
        beginTest("Two consecutive display drags create two undo transactions");

        // Arrange
        const auto attackId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack;
        ProcessorPathHarness harness(makeDisplayGestureLayout());

        writeIntParameterValue(harness.apvts, attackId, 10);
        harness.mapper.apvtsToBuffer();
        harness.undoManager.clearUndoHistory();

        // Act
        simulateDisplayDrag(harness, attackId, "Envelope edit", { 30 });
        simulateDisplayDrag(harness, attackId, "Envelope edit", { 45 });
        expectEquals(harness.undoManager.getUndoDescriptions().size(), 2);

        harness.undoManager.undo();
        expectEquals(readIntParameterValue(harness.apvts, attackId), 30);

        harness.undoManager.undo();

        // Assert
        expectEquals(readIntParameterValue(harness.apvts, attackId), 10);
    }

    void testUndoDispatchesSysExForDisplayEdit()
    {
        beginTest("Undo after display edit dispatches 0x06 SysEx with restored value");

        // Arrange
        const auto attackId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack;
        const auto attackDesc = findIntDescriptor(attackId);
        expect(attackDesc.has_value());

        ProcessorPathHarness harness(makeDisplayGestureLayout());
        harness.apvts.state.setProperty("deviceDetected", true, nullptr);
        harness.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                        MatrixDeviceTypes::kMatrix1000Id,
                                        nullptr);

        writeIntParameterValue(harness.apvts, attackId, 12);
        harness.mapper.apvtsToBuffer();
        harness.undoManager.clearUndoHistory();
        while (harness.queue.dequeue().has_value()) {}

        // Act
        simulateDisplayDrag(harness, attackId, "Envelope edit", { 33 });
        while (harness.queue.dequeue().has_value()) {}

        harness.undoManager.undo();

        // Assert
        expectEquals(readIntParameterValue(harness.apvts, attackId), 12);
        expectEquals(static_cast<int>(harness.packedByteFor(attackId)), 12);

        auto undoMsg = harness.queue.dequeue();
        expect(undoMsg.has_value());
        expect(sysExMatchesRemoteEdit(undoMsg->sysExData,
                                      static_cast<juce::uint8>(attackDesc->sysExId),
                                      12));
        expect(harness.queue.isEmpty());
    }

    void testTrackGeneratorUndoDispatchesSysEx()
    {
        beginTest("Undo after Track Generator display edit dispatches 0x06 SysEx");

        // Arrange
        const auto trackPointId = PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3;
        const auto trackPointDesc = findIntDescriptor(trackPointId);
        expect(trackPointDesc.has_value());

        ProcessorPathHarness harness(makeDisplayGestureLayout());
        harness.apvts.state.setProperty("deviceDetected", true, nullptr);
        harness.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                        MatrixDeviceTypes::kMatrix1000Id,
                                        nullptr);

        writeIntParameterValue(harness.apvts, trackPointId, 7);
        harness.mapper.apvtsToBuffer();
        harness.undoManager.clearUndoHistory();
        while (harness.queue.dequeue().has_value()) {}

        // Act
        simulateDisplayDrag(harness, trackPointId, "Track Generator edit", { 22, 35 });
        while (harness.queue.dequeue().has_value()) {}

        harness.undoManager.undo();

        // Assert
        expectEquals(readIntParameterValue(harness.apvts, trackPointId), 7);
        expectEquals(static_cast<int>(harness.packedByteFor(trackPointId)), 7);

        auto undoMsg = harness.queue.dequeue();
        expect(undoMsg.has_value());
        expect(sysExMatchesRemoteEdit(undoMsg->sysExData,
                                      static_cast<juce::uint8>(trackPointDesc->sysExId),
                                      7));
        expect(harness.queue.isEmpty());
    }

    void testDisplayDragUndoRedoRestoresValues()
    {
        beginTest("Display drag undo then redo restores post-drag value");

        // Arrange
        const auto attackId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack;
        ProcessorPathHarness harness(makeDisplayGestureLayout());

        writeIntParameterValue(harness.apvts, attackId, 10);
        harness.mapper.apvtsToBuffer();
        harness.undoManager.clearUndoHistory();

        // Act
        simulateDisplayDrag(harness, attackId, "Envelope edit", { 15, 20, 30 });
        expectEquals(readIntParameterValue(harness.apvts, attackId), 30);

        harness.undoManager.undo();
        expectEquals(readIntParameterValue(harness.apvts, attackId), 10);

        harness.undoManager.redo();

        // Assert
        expectEquals(readIntParameterValue(harness.apvts, attackId), 30);
    }

    void testTrackGeneratorDisplayDragUndoRedoRestoresValues()
    {
        beginTest("Track Generator display drag undo then redo restores post-drag value");

        // Arrange
        const auto trackPointId = PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3;
        ProcessorPathHarness harness(makeDisplayGestureLayout());

        writeIntParameterValue(harness.apvts, trackPointId, 5);
        harness.mapper.apvtsToBuffer();
        harness.undoManager.clearUndoHistory();

        // Act
        simulateDisplayDrag(harness, trackPointId, "Track Generator edit", { 12, 28, 40 });
        expectEquals(readIntParameterValue(harness.apvts, trackPointId), 40);

        harness.undoManager.undo();
        expectEquals(readIntParameterValue(harness.apvts, trackPointId), 5);

        harness.undoManager.redo();

        // Assert
        expectEquals(readIntParameterValue(harness.apvts, trackPointId), 40);
    }

    void testExternalParamWriteDuringGestureStaysOneTransaction()
    {
        beginTest("Non-active parameter write during display gesture stays in one transaction");

        // Arrange
        const auto attackId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack;
        const auto trackPointId = PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3;
        ProcessorPathHarness harness(makeDisplayGestureLayout());

        writeIntParameterValue(harness.apvts, attackId, 8);
        writeIntParameterValue(harness.apvts, trackPointId, 16);
        harness.mapper.apvtsToBuffer();
        harness.undoManager.clearUndoHistory();

        // Act — one display session; unrelated param touched mid-gesture (same open transaction)
        harness.undoManager.beginNewTransaction("Envelope edit");
        if (auto* attackParam = harness.apvts.getParameter(attackId))
            attackParam->beginChangeGesture();

        writeIntParameterValue(harness.apvts, attackId, 22);
        writeIntParameterValue(harness.apvts, trackPointId, 50);

        if (auto* attackParam = harness.apvts.getParameter(attackId))
            attackParam->endChangeGesture();

        expectEquals(harness.undoManager.getUndoDescriptions().size(), 1);
        harness.undoManager.undo();

        // Assert
        expectEquals(readIntParameterValue(harness.apvts, attackId), 8);
        expectEquals(readIntParameterValue(harness.apvts, trackPointId), 16);
    }

    void testNullUndoManagerDragDoesNotCrash()
    {
        beginTest("Display drag simulation with null UndoManager updates APVTS safely");

        // Arrange
        const auto attackId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack;
        UndoTestAudioProcessor processor(makeDisplayGestureLayout(), nullptr);

        // Act
        simulateDisplayDragWithoutUndoManager(processor.apvts, attackId, { 11, 18, 27 });

        // Assert
        expectEquals(readIntParameterValue(processor.apvts, attackId), 27);
    }
};

static UndoManagerDisplayGestureTests undoManagerDisplayGestureTests;
