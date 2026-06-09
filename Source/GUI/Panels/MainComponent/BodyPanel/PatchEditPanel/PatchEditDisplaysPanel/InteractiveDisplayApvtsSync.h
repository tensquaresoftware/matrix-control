#pragma once

#include <array>
#include <functional>
#include <map>
#include <memory>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class EnvelopeDisplay;
    class Slider;
    class TrackGeneratorDisplay;
}

class PatchEditBottomModulesPanel;
class PatchEditTopModulesPanel;

class InteractiveDisplayApvtsSync : private juce::AudioProcessorValueTreeState::Listener
{
public:
    InteractiveDisplayApvtsSync(juce::AudioProcessorValueTreeState& apvts,
                                TSS::EnvelopeDisplay& envelope1,
                                TSS::EnvelopeDisplay& envelope2,
                                TSS::EnvelopeDisplay& envelope3,
                                TSS::TrackGeneratorDisplay& trackGenerator);
    ~InteractiveDisplayApvtsSync() override;

    void syncAllFromApvts();
    void connectSliderFastPaths(PatchEditTopModulesPanel& topModulesPanel,
                                PatchEditBottomModulesPanel& bottomModulesPanel);

    static constexpr int kEnvelopeParameterCount {5};
    static constexpr int kTrackPointCount {5};

    struct EnvelopeParameterBinding
    {
        const char* parameterId;
    };

private:
    inline static constexpr int kIntParameterMax {63};
    inline static constexpr int kTrackPointSliderStartIndex {3};

    class SliderToDisplayListener;

    juce::AudioProcessorValueTreeState& apvts_;
    TSS::EnvelopeDisplay& envelope1_;
    TSS::EnvelopeDisplay& envelope2_;
    TSS::EnvelopeDisplay& envelope3_;
    TSS::TrackGeneratorDisplay& trackGenerator_;
    juce::AudioProcessorParameter* activeGestureParameter_ {nullptr};
    std::vector<juce::String> listenedParameterIds_;
    std::vector<std::unique_ptr<SliderToDisplayListener>> sliderListeners_;
    std::map<juce::String, TSS::Slider*> slidersByParameterId_;

    void connectDisplayCallbacks();
    void connectEnvelopeDisplay(TSS::EnvelopeDisplay& display,
                                const EnvelopeParameterBinding* bindings,
                                int bindingCount);
    void connectTrackGeneratorDisplay();
    void registerApvtsListeners();

    int readIntParameter(const juce::String& parameterId) const;
    void writeIntParameter(const juce::String& parameterId, int value);
    void updateSliderUi(const juce::String& parameterId, int value);
    void registerSliderForParameter(TSS::Slider& slider, const juce::String& parameterId);
    void beginParameterGesture(const juce::String& parameterId);
    void endParameterGesture();

    bool applyApvtsChange(const juce::String& parameterId);
    void applyEnvelopeDisplayValue(TSS::EnvelopeDisplay& display, int paramIndex, int value);
    void applyTrackPointDisplayValue(int pointIndex, int value);

    void connectEnvelopeSlider(TSS::Slider& slider,
                               TSS::EnvelopeDisplay& display,
                               int paramIndex,
                               const juce::String& parameterId);
    void connectTrackPointSlider(TSS::Slider& slider, int pointIndex, const juce::String& parameterId);

    void parameterChanged(const juce::String& parameterId, float newValue) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InteractiveDisplayApvtsSync)
};
