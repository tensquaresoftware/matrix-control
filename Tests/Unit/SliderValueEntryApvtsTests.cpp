#include "GUI/Helpers/ApvtsUndoableParameterAttachments.h"
#include "SliderValueEntryTestSupport.h"

namespace
{

using namespace SliderValueEntryTestSupport;

class SliderValueEntryApvtsTests : public juce::UnitTest
{
public:
    SliderValueEntryApvtsTests() : juce::UnitTest("SliderValueEntryApvts") {}

    void runTest() override
    {
        testEnterCommitUpdatesApvtsAndUndoRestoresPrior();
        testCommandClickResetUpdatesApvtsAndUndoRestoresPrior();
        testAttachmentDisablesDoubleClickReturn();
    }

private:
    void testEnterCommitUpdatesApvtsAndUndoRestoresPrior()
    {
        beginTest("Enter commit updates APVTS and one undo restores the prior value");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        ValueEntryAudioProcessor processor(makeLayoutFromDescriptor(*descriptor));
        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        TSS::ApvtsUndoableSliderAttachment attachment(
            processor.apvts,
            descriptor->parameterId,
            slider);

        setSliderValueWithUndoableGesture(slider, 10.0);
        processor.undoManager.clearUndoHistory();
        expectEquals(readIntParameterValue(processor.apvts, descriptor->parameterId), 10);
        expectEquals(slider.getValue(), 10.0);

        openValueEditor(slider);
        commitEditorText(slider, "42");

        expectEquals(slider.getValue(), 42.0);
        expectEquals(readIntParameterValue(processor.apvts, descriptor->parameterId), 42);
        expect(processor.undoManager.canUndo());

        processor.undoManager.undo();
        flushPendingUiUpdates();

        expectEquals(readIntParameterValue(processor.apvts, descriptor->parameterId), 10);
        expectEquals(slider.getValue(), 10.0);
    }

    void testCommandClickResetUpdatesApvtsAndUndoRestoresPrior()
    {
        beginTest("Cmd/Ctrl+click reset updates APVTS and one undo restores the prior value");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        ValueEntryAudioProcessor processor(makeLayoutFromDescriptor(*descriptor));
        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        TSS::ApvtsUndoableSliderAttachment attachment(
            processor.apvts,
            descriptor->parameterId,
            slider);

        setSliderValueWithUndoableGesture(slider, 33.0);
        processor.undoManager.clearUndoHistory();
        expectEquals(readIntParameterValue(processor.apvts, descriptor->parameterId), 33);
        expectEquals(slider.getValue(), 33.0);

        const auto click = makeMouseEvent(slider, juce::ModifierKeys::commandModifier);
        slider.mouseDown(click);
        slider.mouseUp(click);

        expectEquals(slider.getValue(), static_cast<double>(descriptor->defaultValue));
        expectEquals(readIntParameterValue(processor.apvts, descriptor->parameterId),
                     descriptor->defaultValue);
        expect(processor.undoManager.canUndo());

        processor.undoManager.undo();
        flushPendingUiUpdates();

        expectEquals(readIntParameterValue(processor.apvts, descriptor->parameterId), 33);
        expectEquals(slider.getValue(), 33.0);
    }

    void testAttachmentDisablesDoubleClickReturn()
    {
        beginTest("Undoable slider attachment disables JUCE double-click return");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        ValueEntryAudioProcessor processor(makeLayoutFromDescriptor(*descriptor));
        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        slider.setDoubleClickReturnValue(true, static_cast<double>(descriptor->defaultValue));

        TSS::ApvtsUndoableSliderAttachment attachment(
            processor.apvts,
            descriptor->parameterId,
            slider);

        expect(! slider.isDoubleClickReturnEnabled());
    }
};

static SliderValueEntryApvtsTests sliderValueEntryApvtsTests;

} // namespace
