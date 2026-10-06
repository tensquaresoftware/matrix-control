#include <juce_audio_devices/juce_audio_devices.h>

#include "PluginProcessor.h"
#include "PluginProcessorInternal.h"

#include "Core/Actions/ActionDispatcher.h"
#include "Core/Actions/ModuleActionHandler.h"
#include "Core/Actions/MutatorActionHandler.h"
#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/Audio/AudioPassthroughProcessor.h"
#include "Core/Audio/InstrumentMidiForwarder.h"
#include "Core/Init/InitTemplateLoader.h"
#include "Core/Init/MasterModuleInitService.h"
#include "Core/Init/MatrixModInitService.h"
#include "Core/Init/PatchInitService.h"
#include "Core/Init/PatchModuleInitService.h"
#include "Core/MIDI/ActiveNoteRegistry.h"
#include "Core/MIDI/KeyboardFromMidiInput.h"
#include "Core/MIDI/MasterParameterSysExDispatcher.h"
#include "Core/MIDI/MatrixModBusParameterSysExDispatcher.h"
#include "Core/MIDI/MatrixModBusReorderService.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/PatchParameterSysExDispatcher.h"
#include "Core/MIDI/PatchSelectionMidiSync.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/ClipboardService.h"
#include "Core/Services/DirtyPatchTracker.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchMutator/PatchMutatorEngine.h"
#include "Factories/ApvtsFactory.h"
#include "GUI/PluginEditor.h"
#include "MIDI/MidiManager.h"

using namespace PluginProcessorInternal;

#if !JucePlugin_IsMidiEffect
juce::AudioProcessor::BusesProperties PluginProcessor::makeBusesProperties()
{
    auto properties = BusesProperties()
                          .withOutput("Output", juce::AudioChannelSet::stereo(), true);

    if (isStandaloneWrapper())
        return properties.withInput("Audio From", juce::AudioChannelSet::stereo(), true);

    return properties;
}
#endif

PluginProcessor::PluginProcessor()
    : AudioProcessor(
#if JucePlugin_IsMidiEffect
          BusesProperties()
#else
          makeBusesProperties()
#endif
      )
    , apvts(*this, &undoManager_, "PARAMETERS", createParameterLayout())
    , midiActivityTracker_{ std::make_unique<Core::MidiActivityTracker>() }
    , outboundQueue_{ std::make_unique<Core::MidiOutboundQueue>() }
    , activeNoteRegistry_{ std::make_unique<Core::ActiveNoteRegistry>() }
    , instrumentForwarder_{ std::make_unique<Core::InstrumentMidiForwarder>(*activeNoteRegistry_) }
    , audioPassthroughProcessor_{ std::make_unique<Core::AudioPassthroughProcessor>() }
    , keyboardFromMidiInput_{ std::make_unique<Core::KeyboardFromMidiInput>(
          *outboundQueue_, *midiActivityTracker_, *activeNoteRegistry_) }
    , midiManager(std::make_unique<MidiManager>(apvts, *outboundQueue_, *midiActivityTracker_))
    , patchModel_{ std::make_unique<Core::PatchModel>() }
    , apvtsPatchMapper_{ std::make_unique<Core::ApvtsPatchMapper>(apvts, *patchModel_) }
    , masterModel_{ std::make_unique<Core::MasterModel>() }
    , apvtsMasterMapper_{ std::make_unique<Core::ApvtsMasterMapper>(apvts, *masterModel_) }
    , patchNameSyncer_{ std::make_unique<Core::PatchNameSyncer>(apvts, *patchModel_) }
    , clipboardService_{ std::make_unique<Core::ClipboardService>() }
    , dirtyPatchTracker_{ std::make_unique<Core::DirtyPatchTracker>() }
{
    midiManager->setActiveNoteRegistry(activeNoteRegistry_.get());
    createSysExDispatchers();
    createInitAndFileServices();
    createActionSubsystem();
    undoManager_.setMaxNumberOfStoredUnits(PluginProcessorInternal::kEditorialUndoMaxStoredUnits,
                                           PluginProcessorInternal::kEditorialUndoMaxStoredUnits);
    finishConstructionSetup();
}

PluginProcessor::~PluginProcessor()
{
    midiDeviceListConnection_.reset();
    deferredMidiPortSyncTimer_.reset();
    apvts.state.removeListener(this);
}

const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String PluginProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void PluginProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    ensureAudioInputBusEnabled();
    syncAudioRuntimeFromState();

    audioPassthroughSampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
    refreshAudioPassthroughLayout(audioPassthroughSampleRate_);
    applyHardwareLatencyToHost();

    if (shouldUseDevelopmentLogging())
        ensureDevelopmentLoggingStarted();

    startMidiThread();

    if (!isStandaloneWrapper() && !arePersistedMidiPortsOpen())
        restoreMidiPortsForHost();
}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.outputBuses.isEmpty())
        return false;

    const auto& outputLayout = layouts.getChannelSet(false, 0);

    if (outputLayout != juce::AudioChannelSet::stereo()
        && outputLayout != juce::AudioChannelSet::mono())
    {
        return false;
    }

    if (layouts.inputBuses.isEmpty())
        return true;

    const auto& inputLayout = layouts.getChannelSet(true, 0);

    if (inputLayout.isDisabled())
        return true;

    return inputLayout == juce::AudioChannelSet::stereo()
        || inputLayout == juce::AudioChannelSet::mono();
#endif
}

void PluginProcessor::startMidiThread()
{
    if (midiManager != nullptr && !midiManager->isThreadRunning())
    {
        midiManager->startThread();
    }
}

void PluginProcessor::releaseResources()
{
    if (shouldUseDevelopmentLogging())
    {
        disableApvtsLogging();
        closeLogFileForSession();
        developmentLoggingStarted_ = false;
    }
}

void PluginProcessor::stopMidiThread()
{
    if (midiManager != nullptr && midiManager->isThreadRunning())
    {
        midiManager->stopThread(kThreadStopTimeoutMs_);
    }
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& audioBuffer,
                                   juce::MidiBuffer& midiMessages)
{
    instrumentForwarder_->forward(midiMessages,
                                  getInstrumentPathEnabled(midiMessages),
                                  *outboundQueue_,
                                  *midiActivityTracker_);

    auto inputBusBuffer = getBusBuffer(audioBuffer, true, 0);
    auto outputBusBuffer = getBusBuffer(audioBuffer, false, 0);

    const bool inputEnabled = getTotalNumInputChannels() > 0
                              && inputBusBuffer.getNumChannels() > 0;

    audioPassthroughProcessor_->updateChannelLayout(inputBusBuffer.getNumChannels(),
                                                    outputBusBuffer.getNumChannels(),
                                                    inputEnabled);
    audioPassthroughProcessor_->process(inputBusBuffer,
                                        outputBusBuffer,
                                        inputGainLinear_.load(std::memory_order_relaxed));
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor(*this);
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    return ApvtsFactory::createParameterLayout();
}

void PluginProcessor::validatePluginDescriptorsAtStartup()
{
    auto validationResult = ApvtsFactory::validatePluginDescriptors();
    if (!validationResult.isValid)
    {
        DBG("SynthDescriptors validation failed:");
        for (int i = 0; i < validationResult.errors.size(); ++i)
            DBG("  ERROR: " + validationResult.errors[i]);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
