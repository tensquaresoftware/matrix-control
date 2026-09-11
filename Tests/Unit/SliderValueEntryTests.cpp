#include "SliderValueEntryTestSupport.h"

namespace
{

using namespace SliderValueEntryTestSupport;

class SliderValueEntryTests : public juce::UnitTest
{
public:
    SliderValueEntryTests() : juce::UnitTest("SliderValueEntry") {}

    void runTest() override
    {
        testDoubleClickOpensEditor();
        testEnterCommitsLegalValue();
        testEscapeCancelsEdit();
        testFocusLostCancelsEdit();
        testEmptyOrIllegalEnterKeepsPreviousValue();
        testCommandClickResetsWithoutDrag();
        testCommandClickWhileEditingCancelsThenResets();
        testPlainClickWhileEditingCancelsWithoutDrag();
        testIdleReturnDoesNotReset();
        testDisabledIgnoresGestures();
        testDisableWhileEditingClosesEditor();
        testOutOfRangeEnterSnapsToLegalRange();
        testBipolarEnterCommitsNegativeValue();
    }

private:
    void testDoubleClickOpensEditor()
    {
        beginTest("Double-click opens inline value editor");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        slider.setValue(12.0, juce::dontSendNotification);

        expect(! slider.isValueEditorOpen());
        openValueEditor(slider);
        expect(slider.isValueEditorOpen());

        auto* editor = valueEditorOf(slider);
        expect(editor != nullptr);
        expect(! editor->isCaretVisible());
        expectEquals(slider.getValue(), 12.0);
    }

    void testEnterCommitsLegalValue()
    {
        beginTest("Enter commits snapped legal value and closes editor");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        slider.setValue(10.0, juce::dontSendNotification);
        openValueEditor(slider);

        commitEditorText(slider, "42");

        expect(! slider.isValueEditorOpen());
        expectEquals(slider.getValue(), 42.0);
    }

    void testEscapeCancelsEdit()
    {
        beginTest("Escape cancels edit and keeps previous value");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        slider.setValue(17.0, juce::dontSendNotification);
        openValueEditor(slider);

        auto* editor = valueEditorOf(slider);
        expect(editor != nullptr);
        editor->setText("99", false);

        if (editor->onEscapeKey)
            editor->onEscapeKey();

        expect(! slider.isValueEditorOpen());
        expectEquals(slider.getValue(), 17.0);
    }

    void testFocusLostCancelsEdit()
    {
        beginTest("Focus-lost cancels edit and keeps previous value");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        slider.setValue(8.0, juce::dontSendNotification);
        openValueEditor(slider);

        auto* editor = valueEditorOf(slider);
        expect(editor != nullptr);
        editor->setText("55", false);

        if (editor->onFocusLost)
            editor->onFocusLost();

        expect(! slider.isValueEditorOpen());
        expectEquals(slider.getValue(), 8.0);
    }

    void testEmptyOrIllegalEnterKeepsPreviousValue()
    {
        beginTest("Empty or illegal Enter keeps previous value and closes editor");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        slider.setValue(21.0, juce::dontSendNotification);

        openValueEditor(slider);
        commitEditorText(slider, {});
        expect(! slider.isValueEditorOpen());
        expectEquals(slider.getValue(), 21.0);

        openValueEditor(slider);
        commitEditorText(slider, "-");
        expect(! slider.isValueEditorOpen());
        expectEquals(slider.getValue(), 21.0);

        openValueEditor(slider);
        commitEditorText(slider, ".");
        expect(! slider.isValueEditorOpen());
        expectEquals(slider.getValue(), 21.0);
    }

    void testCommandClickResetsWithoutDrag()
    {
        beginTest("Cmd/Ctrl+click resets to default without starting a drag");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        slider.setValue(33.0, juce::dontSendNotification);

        const auto click = makeMouseEvent(slider, juce::ModifierKeys::commandModifier);
        slider.mouseDown(click);
        slider.mouseDrag(makeMouseEvent(slider, juce::ModifierKeys::commandModifier));
        slider.mouseUp(click);

        expectEquals(slider.getValue(), static_cast<double>(descriptor->defaultValue));
        expect(! slider.isValueEditorOpen());
    }

    void testCommandClickWhileEditingCancelsThenResets()
    {
        beginTest("Cmd/Ctrl+click while editing cancels then resets");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        slider.setValue(40.0, juce::dontSendNotification);
        openValueEditor(slider);

        auto* editor = valueEditorOf(slider);
        expect(editor != nullptr);
        editor->setText("11", false);
        editor->mouseDown(makeMouseEvent(*editor, juce::ModifierKeys::commandModifier));

        expect(! slider.isValueEditorOpen());
        expectEquals(slider.getValue(), static_cast<double>(descriptor->defaultValue));
    }

    void testPlainClickWhileEditingCancelsWithoutDrag()
    {
        beginTest("Plain click while editing cancels without starting a drag");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        slider.setValue(19.0, juce::dontSendNotification);
        openValueEditor(slider);

        slider.mouseDown(makeMouseEvent(slider, {}));
        slider.mouseDrag(makeMouseEvent(slider, {}));
        slider.mouseUp(makeMouseEvent(slider, {}));

        expect(! slider.isValueEditorOpen());
        expectEquals(slider.getValue(), 19.0);
    }

    void testIdleReturnDoesNotReset()
    {
        beginTest("Return while idle does not reset the value");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        slider.setValue(27.0, juce::dontSendNotification);
        slider.focusGained(juce::Component::FocusChangeType::focusChangedByTabKey);

        expect(! slider.keyPressed(juce::KeyPress(juce::KeyPress::returnKey)));
        expectEquals(slider.getValue(), 27.0);
    }

    void testDisabledIgnoresGestures()
    {
        beginTest("Disabled slider ignores double-click and Cmd/Ctrl+click");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        slider.setValue(14.0, juce::dontSendNotification);
        slider.setEnabled(false);

        openValueEditor(slider);
        expect(! slider.isValueEditorOpen());

        slider.mouseDown(makeMouseEvent(slider, juce::ModifierKeys::commandModifier));
        expectEquals(slider.getValue(), 14.0);
    }

    void testDisableWhileEditingClosesEditor()
    {
        beginTest("Disabling while editing closes the editor and keeps the value");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        slider.setValue(18.0, juce::dontSendNotification);
        openValueEditor(slider);
        expect(slider.isValueEditorOpen());

        slider.setEnabled(false);

        expect(! slider.isValueEditorOpen());
        expectEquals(slider.getValue(), 18.0);
    }

    void testOutOfRangeEnterSnapsToLegalRange()
    {
        beginTest("Out-of-range Enter snaps into the legal range");

        const auto descriptor = findDco1FrequencyDescriptor();
        jassert(descriptor.has_value());

        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        slider.setValue(10.0, juce::dontSendNotification);
        openValueEditor(slider);

        commitEditorText(slider, juce::String(descriptor->maxValue + 50));

        expect(! slider.isValueEditorOpen());
        expectEquals(slider.getValue(), static_cast<double>(descriptor->maxValue));
    }

    void testBipolarEnterCommitsNegativeValue()
    {
        beginTest("Bipolar slider Enter commits a legal negative value");

        const auto descriptor = findBus0AmountDescriptor();
        jassert(descriptor.has_value());
        expectEquals(descriptor->minValue, -63);
        expectEquals(descriptor->maxValue, 63);

        TSS::Slider slider(100, 20, makeTestSliderLook(), makeSliderConfigFromDescriptor(*descriptor));
        slider.setValue(5.0, juce::dontSendNotification);
        openValueEditor(slider);

        commitEditorText(slider, "-10");

        expect(! slider.isValueEditorOpen());
        expectEquals(slider.getValue(), -10.0);
    }
};

static SliderValueEntryTests sliderValueEntryTests;

} // namespace
