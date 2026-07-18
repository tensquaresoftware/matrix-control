#pragma once

#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    // Composes FR-2 device lock with Mutator Compare lock on a set of section containers.
    // Interactive children are enabled only when deviceDetected && !(compareActive && lockOnCompare).
    // Use lockOnCompare=false for Patch Mutator (COMPARE must stay live while Compare is active).
    class CompareLockBinder : public juce::ValueTree::Listener
    {
    public:
        CompareLockBinder(juce::AudioProcessorValueTreeState& apvts,
                          std::vector<juce::Component*> lockedComponents,
                          bool lockOnCompare = true);
        ~CompareLockBinder() override;

        void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override;
        void valueTreeRedirected(juce::ValueTree& tree) override;

        void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
        void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
        void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
        void valueTreeParentChanged(juce::ValueTree&) override {}

    private:
        void apply();
        void syncDeviceLockFooter(bool deviceDetected);

        juce::AudioProcessorValueTreeState& apvts_;
        std::vector<juce::Component::SafePointer<juce::Component>> locked_;
        bool lockOnCompare_ = true;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompareLockBinder)
    };
}
