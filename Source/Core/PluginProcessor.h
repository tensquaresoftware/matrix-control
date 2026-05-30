#pragma once

#include <memory>
#include <map>
#include <optional>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/Definitions/PluginDescriptors.h"

class MidiManager;

class PluginProcessor : public juce::AudioProcessor, public juce::ValueTree::Listener
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;
    
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getApvts() noexcept { return apvts; }
    const juce::AudioProcessorValueTreeState& getApvts() const noexcept { return apvts; }

    void startMidiThread();
    void stopMidiThread();

    MidiManager& getMidiManager() noexcept { return *midiManager; }
    const MidiManager& getMidiManager() const noexcept { return *midiManager; }

    void setMidiInputPort(const juce::String& deviceId);
    void setMidiOutputPort(const juce::String& deviceId);

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                 const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree& parentTree,
                            juce::ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(juce::ValueTree& parentTree,
                              juce::ValueTree& childWhichHasBeenRemoved,
                              int indexFromWhichChildWasRemoved) override;
    void valueTreeChildOrderChanged(juce::ValueTree& parentTreeWhoseChildrenHaveChanged,
                                   int oldIndex, int newIndex) override;
    void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override;
    void valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged) override;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void validatePluginDescriptorsAtStartup();
    void initializeMidiPortProperties();
    void enableFileLoggingForSession();
    void closeLogFileForSession();
    void enableApvtsLogging();
    void disableApvtsLogging();
    void ensureDevelopmentLoggingStarted();
    
    juce::String getThreadNameForLogging() const;
    static juce::String simplifyThreadNameForLogging(const juce::String& threadName);
    juce::String resolveParameterIdFromTree(juce::ValueTree& tree, const juce::Identifier& property) const;
    juce::String resolveParameterIdFromValueProperty(juce::ValueTree& tree, const juce::Identifier& property) const;
    juce::String getCanonicalParameterId(const juce::String& parameterId) const;
    juce::String findParameterIdInDirectTree(juce::ValueTree& tree) const;
    juce::String findParameterIdInParentTree(juce::ValueTree& tree) const;
    juce::String findParameterIdInChildren(juce::ValueTree& changedTree, const juce::var& newValue) const;
    
    void buildChoiceParameterMap();
    std::optional<juce::String> getChoiceLabel(const juce::String& parameterId, int value) const;
    juce::String getChoiceLabelForNumericValue(const juce::String& parameterId, const juce::var& newValue) const;
    void handleBankNumberChange(const juce::String& parameterId);
    void handlePatchNumberChange(const juce::String& parameterId);

    juce::AudioProcessorValueTreeState apvts;
    std::unique_ptr<MidiManager> midiManager;
    std::map<juce::String, PluginDescriptors::ChoiceParameterDescriptor> choiceParameterMap_;
    bool developmentLoggingStarted_ { false };
    
    static constexpr int kThreadStopTimeoutMs_ {5000};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};