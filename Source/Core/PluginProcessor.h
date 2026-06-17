#pragma once

#include <atomic>
#include <bitset>
#include <memory>
#include <map>
#include <optional>
#include <unordered_set>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/MIDI/MatrixModBusParameterSysExDispatcher.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/PluginDescriptors.h"

class MidiManager;
class SysExDecoder;
class SysExParser;

namespace Core
{
    class PatchModel;
    class ApvtsPatchMapper;
    class MasterModel;
    class ApvtsMasterMapper;
    class PatchNameSyncer;
    class MasterParameterSysExDispatcher;
    class MatrixModBusParameterSysExDispatcher;
    class MatrixModBusReorderService;
    class MatrixModInitService;
    class InitTemplateLoader;
    struct InitTemplateLoadResult;
    class MasterModuleInitService;
    class PatchParameterSysExDispatcher;
    class MidiOutboundQueue;
    class InstrumentMidiForwarder;
    class KeyboardFromMidiInput;
    class AudioPassthroughProcessor;
    class MidiActivityTracker;
}

class PluginProcessor : public juce::AudioProcessor, public juce::ValueTree::Listener
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

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

    Core::PatchModel& getPatchModel() noexcept { return *patchModel_; }
    const Core::PatchModel& getPatchModel() const noexcept { return *patchModel_; }

    Core::ApvtsPatchMapper& getApvtsPatchMapper() noexcept { return *apvtsPatchMapper_; }
    const Core::ApvtsPatchMapper& getApvtsPatchMapper() const noexcept { return *apvtsPatchMapper_; }

    Core::MasterModel& getMasterModel() noexcept { return *masterModel_; }
    const Core::MasterModel& getMasterModel() const noexcept { return *masterModel_; }

    Core::ApvtsMasterMapper& getApvtsMasterMapper() noexcept { return *apvtsMasterMapper_; }
    const Core::ApvtsMasterMapper& getApvtsMasterMapper() const noexcept { return *apvtsMasterMapper_; }

    Core::PatchNameSyncer& getPatchNameSyncer() noexcept { return *patchNameSyncer_; }
    const Core::PatchNameSyncer& getPatchNameSyncer() const noexcept { return *patchNameSyncer_; }

    bool setMidiInputPort(const juce::String& deviceId);
    bool setMidiOutputPort(const juce::String& deviceId);
    bool setKeyboardFromPort(const juce::String& deviceId);

    void setInputGainDb(float gainDb);
    void setHardwareLatencyMs(float latencyMs);
    float getHardwareLatencyMs() const;
    int getGuiScaleId() const;
    void setGuiScaleId(int scaleId);
    int getSkinVariantId() const;
    void setSkinVariantId(int skinVariantId);
    void syncHardwareLatencyFromState();
    void syncMidiPortsFromState(bool reportOpenFailures = true);
    void restoreMidiPortsForHost();
    void setAudioFromChannelMode(int mode);
    void setAudioFromSourceId(const juce::String& sourceId);
    void syncAudioPassthroughFromSourceId(const juce::String& sourceId);
    juce::StringArray getAudioInputSourceNames() const;
    juce::StringArray getAudioInputSourceIds() const;

    void swapMatrixModBusContents(int fromBus, int toBus);

    Core::AudioPassthroughProcessor& getAudioPassthroughProcessor() noexcept { return *audioPassthroughProcessor_; }
    const Core::AudioPassthroughProcessor& getAudioPassthroughProcessor() const noexcept { return *audioPassthroughProcessor_; }

    Core::MidiActivityTracker& getMidiActivityTracker() noexcept { return *midiActivityTracker_; }
    const Core::MidiActivityTracker& getMidiActivityTracker() const noexcept { return *midiActivityTracker_; }

    bool isStandalone() const;

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
    struct DeferredMidiPortSyncTimer final : juce::Timer
    {
        explicit DeferredMidiPortSyncTimer(PluginProcessor& processorIn);

        void startRetrySeries();

    private:
        static constexpr int kMaxAttempts_ = 4;

        void timerCallback() override;
        void scheduleNextAttempt();
        static int delayMsForAttempt(int attemptIndex);

        PluginProcessor& processor;
        int attemptIndex_ = 0;
    };

    struct MatrixModSysExCoalesceTimer final : juce::Timer
    {
        static constexpr int kCoalesceDelayMs = 10;

        explicit MatrixModSysExCoalesceTimer(Core::MatrixModBusParameterSysExDispatcher& dispatcherIn) noexcept
            : dispatcher_(dispatcherIn)
        {
        }

        void noteParameterChanged(const juce::String& parameterId)
        {
            const int busIndex = dispatcher_.busIndexForParameterId(parameterId);
            if (busIndex < 0)
                return;

            pendingBuses_.set(static_cast<size_t>(busIndex));

            if (!isTimerRunning())
                startTimer(kCoalesceDelayMs);
        }

    private:
        void timerCallback() override
        {
            stopTimer();

            for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
            {
                if (!pendingBuses_.test(static_cast<size_t>(bus)))
                    continue;

                pendingBuses_.reset(static_cast<size_t>(bus));
                dispatcher_.dispatchBus(bus);
            }
        }

        Core::MatrixModBusParameterSysExDispatcher& dispatcher_;
        std::bitset<Matrix1000Limits::kModulationBusCount> pendingBuses_;
    };

    static BusesProperties makeBusesProperties();
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void validatePluginDescriptorsAtStartup();
    void initializeMidiPortProperties();
    void initializeAudioProperties();
    void initializeHardwareLatencyProperty();
    void initializeInitTemplatesFolderProperty();
    void applyHardwareLatencyToHost();
    void notifyNonParameterStateChanged();
    void scheduleDeferredMidiPortSyncForPluginHost();
    void syncMidiPortsFromStateImpl(bool reportOpenFailures);
    bool arePersistedMidiPortsOpen() const;
    void initializePatchNameProperty();
    bool getInstrumentPathEnabled(const juce::MidiBuffer& midiMessages) const;
    void ensureAudioInputBusEnabled();
    int getAudioFromInputChannelCount() const noexcept;
    int getMainOutputChannelCount() const noexcept;
    void refreshAudioPassthroughLayout(double sampleRate);
    void syncAudioRuntimeFromState();
    static float dbToLinearGain(float gainDb) noexcept;
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
    void handleMatrixModInitPropertyChange(const juce::String& propertyId);
    void handleMasterModuleInitPropertyChange(const juce::String& propertyId);
    void propagateInitTemplateFooterMessage(const Core::InitTemplateLoadResult& result);
    int parseMatrixModBusInitIndex(const juce::String& propertyId) const;
    void buildPatchParameterIdSet();
    void buildMasterParameterIdSet();
    void buildMatrixModParameterIdSet();

    juce::AudioProcessorValueTreeState apvts;
    std::unique_ptr<Core::MidiActivityTracker> midiActivityTracker_;
    std::unique_ptr<Core::MidiOutboundQueue> outboundQueue_;
    std::unique_ptr<Core::InstrumentMidiForwarder> instrumentForwarder_;
    std::unique_ptr<Core::AudioPassthroughProcessor> audioPassthroughProcessor_;
    std::unique_ptr<Core::KeyboardFromMidiInput> keyboardFromMidiInput_;
    std::atomic<float> inputGainLinear_{ 1.0f };
    double audioPassthroughSampleRate_ { 44100.0 };
    std::unique_ptr<MidiManager> midiManager;
    std::unique_ptr<Core::PatchModel> patchModel_;
    std::unique_ptr<Core::ApvtsPatchMapper> apvtsPatchMapper_;
    std::unique_ptr<Core::MasterModel> masterModel_;
    std::unique_ptr<Core::ApvtsMasterMapper> apvtsMasterMapper_;
    std::unique_ptr<Core::PatchNameSyncer> patchNameSyncer_;
    std::unique_ptr<Core::PatchParameterSysExDispatcher> patchParameterSysExDispatcher_;
    std::unique_ptr<Core::MasterParameterSysExDispatcher> masterParameterSysExDispatcher_;
    std::unique_ptr<Core::MatrixModBusParameterSysExDispatcher> matrixModBusParameterSysExDispatcher_;
    std::unique_ptr<MatrixModSysExCoalesceTimer> matrixModSysExCoalesceTimer_;
    std::unique_ptr<Core::MatrixModBusReorderService> matrixModBusReorderService_;
    std::unique_ptr<Core::MatrixModInitService> matrixModInitService_;
    std::unique_ptr<SysExParser> sysExParser_;
    std::unique_ptr<SysExDecoder> sysExDecoder_;
    std::unique_ptr<Core::InitTemplateLoader> initTemplateLoader_;
    std::unique_ptr<Core::MasterModuleInitService> masterModuleInitService_;
    std::map<juce::String, PluginDescriptors::ChoiceParameterDescriptor> choiceParameterMap_;
    std::unordered_set<juce::String> patchParameterIds_;
    std::unordered_set<juce::String> masterParameterIds_;
    std::unordered_set<juce::String> matrixModParameterIds_;
    bool suppressMatrixModParameterSysEx_ { false };
    bool suppressMasterParameterSysEx_ { false };
    bool developmentLoggingStarted_ { false };
    std::unique_ptr<DeferredMidiPortSyncTimer> deferredMidiPortSyncTimer_;
    
    static constexpr int kThreadStopTimeoutMs_ {5000};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};