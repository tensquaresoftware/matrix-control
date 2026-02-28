#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/EnvelopeDisplay.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/PatchNameDisplay.h"
#include "GUI/Widgets/TrackGeneratorDisplay.h"
#include "Shared/PluginDisplayNames.h"

namespace tss
{
    class Skin;
}

class MiddlePanel : public juce::Component,
                    public juce::ValueTree::Listener
{
public:
    MiddlePanel(tss::Skin& skin, int width, int height, juce::AudioProcessorValueTreeState& apvts);
    ~MiddlePanel() override;

    void resized() override;
    void setSkin(tss::Skin& skin);
    
    tss::TrackGeneratorDisplay& getTrackGeneratorDisplay() { return trackGeneratorDisplay_; }
    tss::EnvelopeDisplay& getEnvelope1Display() { return envelope1Display_; }
    tss::EnvelopeDisplay& getEnvelope2Display() { return envelope2Display_; }
    tss::EnvelopeDisplay& getEnvelope3Display() { return envelope3Display_; }

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    void valueTreeRedirected(juce::ValueTree&) override {}

private:
    inline constexpr static int kSpacing = 15;
    inline constexpr static int kEnvParameterMax = 63;
    inline constexpr static int kTrackPointMax = 63;
    inline constexpr static int kPatchNameSectionPaddingTop = 8;
    inline constexpr static int kPatchNameSectionSpacing = 12;

    int width_;
    int height_;
    tss::Skin* skin_;
    juce::AudioProcessorValueTreeState* apvts_ = nullptr;

    tss::EnvelopeDisplay envelope1Display_;
    tss::EnvelopeDisplay envelope2Display_;
    tss::EnvelopeDisplay envelope3Display_;
    tss::TrackGeneratorDisplay trackGeneratorDisplay_;
    tss::ModuleHeader patchNameModuleHeader_;
    tss::PatchNameDisplay patchNameDisplay_;

    void syncTrackGeneratorDisplayFromApvts();
    void syncEnvelopeDisplaysFromApvts();
    int getTrackPointValueFromApvts(const juce::String& parameterId) const;
    int getEnvParameterFromApvts(const juce::String& parameterId) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MiddlePanel)
};

