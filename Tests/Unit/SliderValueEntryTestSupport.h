#pragma once

#include <optional>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Core/Models/ApvtsPatchMapper.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Widgets/Slider.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

namespace SliderValueEntryTestSupport
{

inline std::optional<PluginDescriptors::IntParameterDescriptor> findIntDescriptor(
    const juce::String& parameterId)
{
    for (const auto& descriptor : Core::ApvtsPatchMapper::buildIntDescriptors())
    {
        if (descriptor.parameterId == parameterId)
            return descriptor;
    }

    return std::nullopt;
}

inline std::optional<PluginDescriptors::IntParameterDescriptor> findDco1FrequencyDescriptor()
{
    return findIntDescriptor(PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency);
}

inline std::optional<PluginDescriptors::IntParameterDescriptor> findBus0AmountDescriptor()
{
    return findIntDescriptor(
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Amount);
}

inline TSS::SliderConfig makeSliderConfigFromDescriptor(
    const PluginDescriptors::IntParameterDescriptor& descriptor)
{
    TSS::SliderConfig config;
    config.minValue = static_cast<double>(descriptor.minValue);
    config.maxValue = static_cast<double>(descriptor.maxValue);
    config.defaultValue = static_cast<double>(descriptor.defaultValue);
    config.step = 1.0;
    return config;
}

inline int readIntParameterValue(const juce::AudioProcessorValueTreeState& apvts,
                                 const juce::String& parameterId)
{
    return juce::roundToInt(apvts.getParameterAsValue(parameterId).getValue());
}

inline void setSliderValueWithUndoableGesture(TSS::Slider& slider, double value)
{
    const juce::Slider::ScopedDragNotification dragSession(slider);
    slider.setValue(value, juce::sendNotificationSync);
}

inline TSS::SliderLook makeTestSliderLook()
{
    TSS::SliderLook look;
    look.editorText = juce::Colour(ColourChart::kWhite);
    look.editorCaret = juce::Colour(ColourChart::kWhite);
    return look;
}

inline juce::MouseEvent makeMouseEvent(juce::Component& component,
                                       juce::ModifierKeys modifiers,
                                       int numberOfClicks = 1)
{
    auto source = juce::Desktop::getInstance().getMainMouseSource();
    const juce::Point<float> position(1.0f, 1.0f);
    const auto now = juce::Time::getCurrentTime();

    return {
        source,
        position,
        modifiers,
        1.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        &component,
        &component,
        now,
        position,
        now,
        numberOfClicks,
        false
    };
}

inline juce::TextEditor* valueEditorOf(TSS::Slider& slider)
{
    for (int index = 0; index < slider.getNumChildComponents(); ++index)
        if (auto* editor = dynamic_cast<juce::TextEditor*>(slider.getChildComponent(index)))
            return editor;

    return nullptr;
}

inline void openValueEditor(TSS::Slider& slider)
{
    slider.mouseDoubleClick(makeMouseEvent(slider, {}, 2));
}

inline void flushPendingUiUpdates(int milliseconds = 50)
{
    if (auto* messageManager = juce::MessageManager::getInstanceWithoutCreating())
        messageManager->runDispatchLoopUntil(milliseconds);
}

inline void commitEditorText(TSS::Slider& slider, const juce::String& text)
{
    auto* editor = valueEditorOf(slider);
    jassert(editor != nullptr);
    editor->setText(text, false);

    if (editor->onReturnKey)
        editor->onReturnKey();
}

class ValueEntryAudioProcessor final : public juce::AudioProcessor
{
public:
    explicit ValueEntryAudioProcessor(juce::AudioProcessorValueTreeState::ParameterLayout layout)
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, &undoManager, "P", std::move(layout))
    {
    }

    juce::UndoManager undoManager;
    juce::AudioProcessorValueTreeState apvts;

    const juce::String getName() const override { return "SliderValueEntryHarness"; }
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

inline juce::AudioProcessorValueTreeState::ParameterLayout makeLayoutFromDescriptor(
    const PluginDescriptors::IntParameterDescriptor& descriptor)
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID(descriptor.parameterId, 1),
        descriptor.displayName,
        descriptor.minValue,
        descriptor.maxValue,
        descriptor.defaultValue));
    return layout;
}

} // namespace SliderValueEntryTestSupport
