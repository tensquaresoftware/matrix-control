#pragma once

#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    // Locks a set of components while the Mutator Compare mode is active (kCompareActive).
    // Locked components stop passing mouse events to their children and are grayed; on exit
    // they are restored. Listens to the APVTS state tree so panels stay in sync automatically.
    class CompareLockBinder : public juce::ValueTree::Listener
    {
    public:
        CompareLockBinder(juce::AudioProcessorValueTreeState& apvts,
                          std::vector<juce::Component*> lockedComponents);
        ~CompareLockBinder() override;

        void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override;
        void valueTreeRedirected(juce::ValueTree& tree) override;

        void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
        void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
        void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
        void valueTreeParentChanged(juce::ValueTree&) override {}

    private:
        void apply();

        juce::AudioProcessorValueTreeState& apvts_;
        std::vector<juce::Component::SafePointer<juce::Component>> locked_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompareLockBinder)
    };
}
