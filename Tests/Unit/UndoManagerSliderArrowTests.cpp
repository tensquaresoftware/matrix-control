#include <optional>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Core/Models/ApvtsPatchMapper.h"
#include "GUI/Widgets/Slider.h"
#include "Shared/Definitions/ApvtsTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{

std::optional<PluginDescriptors::IntParameterDescriptor> findDco1FrequencyDescriptor()
{
    for (const auto& d : Core::ApvtsPatchMapper::buildIntDescriptors())
    {
        if (d.parameterId == PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency)
            return d;
    }

    return std::nullopt;
}

juce::AudioProcessorValueTreeState::ParameterLayout makeDco1FrequencyLayout()
{
    const auto frequencyDesc = findDco1FrequencyDescriptor();
    jassert(frequencyDesc.has_value());

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID(frequencyDesc->parameterId, 1),
        frequencyDesc->displayName,
        frequencyDesc->minValue,
        frequencyDesc->maxValue,
        frequencyDesc->defaultValue));

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

class SliderArrowUndoAudioProcessor : public juce::AudioProcessor
{
public:
    explicit SliderArrowUndoAudioProcessor(juce::AudioProcessorValueTreeState::ParameterLayout layout)
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, &undoManager, "P", std::move(layout))
    {
    }

    juce::UndoManager undoManager;
    juce::AudioProcessorValueTreeState apvts;

    const juce::String getName() const override { return "SliderArrowHarness"; }
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

TSS::SliderConfig makeDco1FrequencySliderConfig(const PluginDescriptors::IntParameterDescriptor& descriptor)
{
    TSS::SliderConfig config;
    config.minValue = static_cast<double>(descriptor.minValue);
    config.maxValue = static_cast<double>(descriptor.maxValue);
    config.defaultValue = static_cast<double>(descriptor.defaultValue);
    config.step = 1.0;
    return config;
}

bool runWidgetArrowSession(TSS::Slider& slider, int incrementKeypressCount)
{
    slider.grabKeyboardFocus();
    slider.focusGained(juce::Component::FocusChangeType::focusChangedByTabKey);

    for (int i = 0; i < incrementKeypressCount; ++i)
    {
        if (! slider.keyPressed(juce::KeyPress(juce::KeyPress::upKey)))
            return false;
    }

    slider.focusLost(juce::Component::FocusChangeType::focusChangedByTabKey);
    return true;
}

void simulateArrowGestureWrites(juce::AudioProcessorValueTreeState& apvts,
                                juce::UndoManager& undoManager,
                                const juce::String& parameterId,
                                const std::vector<int>& valuesAfterEachKeypress)
{
    if (auto* param = apvts.getParameter(parameterId))
    {
        undoManager.beginNewTransaction("Slider edit");
        param->beginChangeGesture();

        for (const int value : valuesAfterEachKeypress)
            writeIntParameterValue(apvts, parameterId, value);

        param->endChangeGesture();
    }
}

class UndoManagerSliderArrowTests : public juce::UnitTest
{
public:
    UndoManagerSliderArrowTests() : juce::UnitTest("UndoManagerSliderArrow") {}

    void runTest() override
    {
        testSliderArrowKeysIncrementWithoutAttachment();
        testSliderYieldsCommandModifierArrows();
        testWidgetArrowKeysWithAttachmentUpdateSliderValue();
        testWidgetArrowSessionProducesSingleUndoTransaction();
        testUndoRestoresPreArrowSessionValue();
        testTwoArrowSessionsProduceTwoTransactions();
        testNonEditorialPropertyDoesNotChangeUndoStack();
    }

private:
    void testSliderArrowKeysIncrementWithoutAttachment()
    {
        beginTest("TSS::Slider arrow keys increment value without attachment");

        const auto frequencyDesc = findDco1FrequencyDescriptor();
        jassert(frequencyDesc.has_value());

        TSS::SliderLook look;
        TSS::Slider slider(100, 20, look, makeDco1FrequencySliderConfig(*frequencyDesc));
        slider.setValue(10.0, juce::dontSendNotification);
        slider.focusGained(juce::Component::FocusChangeType::focusChangedByTabKey);

        expect(slider.keyPressed(juce::KeyPress(juce::KeyPress::upKey)));
        expectEquals(slider.getValue(), 11.0);
    }

    void testSliderYieldsCommandModifierArrows()
    {
        beginTest("TSS::Slider yields Cmd/Ctrl(+Shift)+arrows without nudging");

        const auto frequencyDesc = findDco1FrequencyDescriptor();
        jassert(frequencyDesc.has_value());

        using juce::ModifierKeys;
        const auto command = ModifierKeys::commandModifier;
        const auto commandShift = command | ModifierKeys::shiftModifier;

        TSS::SliderLook look;
        TSS::Slider slider(100, 20, look, makeDco1FrequencySliderConfig(*frequencyDesc));
        slider.setValue(10.0, juce::dontSendNotification);
        slider.focusGained(juce::Component::FocusChangeType::focusChangedByTabKey);

        const int arrowKeys[] = {
            juce::KeyPress::upKey,
            juce::KeyPress::downKey,
            juce::KeyPress::leftKey,
            juce::KeyPress::rightKey
        };

        for (const int arrowKey : arrowKeys)
        {
            expect(! slider.keyPressed(juce::KeyPress(arrowKey, command, 0)));
            expect(! slider.keyPressed(juce::KeyPress(arrowKey, commandShift, 0)));
        }

        expectEquals(slider.getValue(), 10.0);
        expect(slider.keyPressed(juce::KeyPress(juce::KeyPress::upKey)));
        expectEquals(slider.getValue(), 11.0);
    }

    void testWidgetArrowKeysWithAttachmentUpdateSliderValue()
    {
        beginTest("TSS::Slider arrow keys update slider value with attachment");

        SliderArrowUndoAudioProcessor processor(makeDco1FrequencyLayout());
        const auto frequencyDesc = findDco1FrequencyDescriptor();
        jassert(frequencyDesc.has_value());

        TSS::SliderLook look;
        TSS::Slider slider(100, 20, look, makeDco1FrequencySliderConfig(*frequencyDesc));
        juce::AudioProcessorValueTreeState::SliderAttachment attachment(
            processor.apvts,
            frequencyDesc->parameterId,
            slider);

        writeIntParameterValue(processor.apvts, frequencyDesc->parameterId, 10);
        expect(runWidgetArrowSession(slider, 3));
        expectEquals(slider.getValue(), 13.0);
    }

    void testWidgetArrowSessionProducesSingleUndoTransaction()
    {
        beginTest("Arrow focus session records one undo transaction");

        SliderArrowUndoAudioProcessor processor(makeDco1FrequencyLayout());
        const auto frequencyDesc = findDco1FrequencyDescriptor();
        jassert(frequencyDesc.has_value());

        writeIntParameterValue(processor.apvts, frequencyDesc->parameterId, 10);
        processor.undoManager.clearUndoHistory();

        const auto undoCountBefore = processor.undoManager.getUndoDescriptions().size();
        simulateArrowGestureWrites(
            processor.apvts,
            processor.undoManager,
            frequencyDesc->parameterId,
            { 11, 12, 13 });

        expectEquals(static_cast<int>(processor.undoManager.getUndoDescriptions().size()), undoCountBefore + 1);
        expectEquals(readIntParameterValue(processor.apvts, frequencyDesc->parameterId), 13);
    }

    void testUndoRestoresPreArrowSessionValue()
    {
        beginTest("One undo restores value before arrow session");

        SliderArrowUndoAudioProcessor processor(makeDco1FrequencyLayout());
        const auto frequencyDesc = findDco1FrequencyDescriptor();
        jassert(frequencyDesc.has_value());

        writeIntParameterValue(processor.apvts, frequencyDesc->parameterId, 10);
        processor.undoManager.clearUndoHistory();
        simulateArrowGestureWrites(
            processor.apvts,
            processor.undoManager,
            frequencyDesc->parameterId,
            { 11, 12, 13 });

        processor.undoManager.undo();

        expectEquals(readIntParameterValue(processor.apvts, frequencyDesc->parameterId), 10);
    }

    void testTwoArrowSessionsProduceTwoTransactions()
    {
        beginTest("Two arrow sessions produce two undo transactions");

        SliderArrowUndoAudioProcessor processor(makeDco1FrequencyLayout());
        const auto frequencyDesc = findDco1FrequencyDescriptor();
        jassert(frequencyDesc.has_value());

        writeIntParameterValue(processor.apvts, frequencyDesc->parameterId, 10);
        processor.undoManager.clearUndoHistory();
        simulateArrowGestureWrites(processor.apvts, processor.undoManager, frequencyDesc->parameterId, { 11, 12 });
        simulateArrowGestureWrites(processor.apvts, processor.undoManager, frequencyDesc->parameterId, { 13, 14 });

        expectEquals(static_cast<int>(processor.undoManager.getUndoDescriptions().size()), 2);

        processor.undoManager.undo();
        expectEquals(readIntParameterValue(processor.apvts, frequencyDesc->parameterId), 12);

        processor.undoManager.undo();
        expectEquals(readIntParameterValue(processor.apvts, frequencyDesc->parameterId), 10);
    }

    void testNonEditorialPropertyDoesNotChangeUndoStack()
    {
        beginTest("Non-editorial property write does not change undo stack");

        SliderArrowUndoAudioProcessor processor(makeDco1FrequencyLayout());
        const auto undoCountBefore = processor.undoManager.getUndoDescriptions().size();

        processor.apvts.state.setProperty("uiMessageText", juce::String("info"), nullptr);

        expectEquals(static_cast<int>(processor.undoManager.getUndoDescriptions().size()), undoCountBefore);
    }
};

static UndoManagerSliderArrowTests undoManagerSliderArrowTests;

} // namespace
