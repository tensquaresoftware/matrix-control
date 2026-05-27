#pragma once

#include <array>
#include <functional>
#include <map>
#include <memory>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
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
                                tss::EnvelopeDisplay& envelope1,
                                tss::EnvelopeDisplay& envelope2,
                                tss::EnvelopeDisplay& envelope3,
                                tss::TrackGeneratorDisplay& trackGenerator);
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
    tss::EnvelopeDisplay& envelope1_;
    tss::EnvelopeDisplay& envelope2_;
    tss::EnvelopeDisplay& envelope3_;
    tss::TrackGeneratorDisplay& trackGenerator_;
    juce::AudioProcessorParameter* activeGestureParameter_ {nullptr};
    std::vector<juce::String> listenedParameterIds_;
    std::vector<std::unique_ptr<SliderToDisplayListener>> sliderListeners_;
    std::map<juce::String, tss::Slider*> slidersByParameterId_;

    void connectDisplayCallbacks();
    void connectEnvelopeDisplay(tss::EnvelopeDisplay& display,
                                const EnvelopeParameterBinding* bindings,
                                int bindingCount);
    void connectTrackGeneratorDisplay();
    void registerApvtsListeners();

    int readIntParameter(const juce::String& parameterId) const;
    void writeIntParameter(const juce::String& parameterId, int value);
    void updateSliderUi(const juce::String& parameterId, int value);
    void registerSliderForParameter(tss::Slider& slider, const juce::String& parameterId);
    void beginParameterGesture(const juce::String& parameterId);
    void endParameterGesture();

    bool applyApvtsChange(const juce::String& parameterId);
    void applyEnvelopeDisplayValue(tss::EnvelopeDisplay& display, int paramIndex, int value);
    void applyTrackPointDisplayValue(int pointIndex, int value);

    void connectEnvelopeSlider(tss::Slider& slider,
                               tss::EnvelopeDisplay& display,
                               int paramIndex,
                               const juce::String& parameterId);
    void connectTrackPointSlider(tss::Slider& slider, int pointIndex, const juce::String& parameterId);

    void parameterChanged(const juce::String& parameterId, float newValue) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InteractiveDisplayApvtsSync)
};
