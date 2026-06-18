#include <cmath>
#include <functional>

#include <juce_audio_devices/juce_audio_devices.h>

#include "PluginProcessor.h"
#include "Core/Actions/ActionDispatcher.h"
#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Actions/ActionPropertyRegistry.h"
#include "Core/Actions/ModuleActionHandler.h"
#include "Core/Actions/MutatorActionHandler.h"
#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/Audio/AudioPassthroughProcessor.h"
#include "Core/Audio/AudioInputSourceCatalog.h"
#include "Core/Audio/HardwareLatency.h"
#include "Core/Audio/InstrumentMidiForwarder.h"
#include "Core/Audio/StandaloneAudioInputRouter.h"

#include "Core/MIDI/KeyboardFromMidiInput.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/MIDI/MasterParameterSysExDispatcher.h"
#include "Core/MIDI/MatrixModBusParameterSysExDispatcher.h"
#include "Core/Init/InitTemplateLoader.h"
#include "Core/Init/MasterModuleInitService.h"
#include "Core/Init/PatchModuleInitService.h"
#include "Core/Init/MatrixModInitService.h"
#include "Core/Exceptions/ExceptionPropagator.h"
#include "Core/MIDI/MatrixModBusReorderService.h"
#include "Core/MIDI/PatchParameterSysExDispatcher.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Services/ClipboardPasteEnabledResolver.h"
#include "Core/Services/ClipboardService.h"
#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Shared/Definitions/PluginAudioConstants.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/PluginEditor.h"
#include "MIDI/MidiManager.h"
#include "Shared/Definitions/ApvtsTypes.h"
#include "Loggers/MidiLogger.h"
#include "Loggers/ApvtsLogger.h"
#include "Factories/ApvtsFactory.h"
#include "Shared/ProjectPaths.h"

namespace
{
    bool isStandaloneWrapper()
    {
        return juce::PluginHostType::getPluginLoadedAs() == juce::AudioProcessor::wrapperType_Standalone;
    }

    bool isVst3Wrapper()
    {
        return juce::PluginHostType::getPluginLoadedAs() == juce::AudioProcessor::wrapperType_VST3;
    }

    void runSyncOnMessageThread(std::function<void()> task)
    {
        if (juce::MessageManager::existsAndIsCurrentThread())
        {
            task();
            return;
        }

        juce::MessageManager::callAsync(std::move(task));
    }

    bool isMidiInputDeviceAvailable(const juce::String& deviceId)
    {
        if (deviceId.isEmpty())
            return false;

        for (const auto& device : juce::MidiInput::getAvailableDevices())
        {
            if (device.identifier == deviceId)
                return true;
        }

        return false;
    }

    bool isMidiOutputDeviceAvailable(const juce::String& deviceId)
    {
        if (deviceId.isEmpty())
            return false;

        for (const auto& device : juce::MidiOutput::getAvailableDevices())
        {
            if (device.identifier == deviceId)
                return true;
        }

        return false;
    }

    juce::String sanitizePersistedMidiInputPortId(const juce::String& deviceId)
    {
        return isMidiInputDeviceAvailable(deviceId) ? deviceId : juce::String();
    }

    juce::String sanitizePersistedMidiOutputPortId(const juce::String& deviceId)
    {
        return isMidiOutputDeviceAvailable(deviceId) ? deviceId : juce::String();
    }

    bool shouldUseDevelopmentLogging()
    {
#if JUCE_DEBUG
        if (isStandaloneWrapper())
            return true;

        return !ProjectPaths::isUsingFallbackRoot();
#else
        return false;
#endif
    }
}

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
    , apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
    , midiActivityTracker_{ std::make_unique<Core::MidiActivityTracker>() }
    , outboundQueue_{ std::make_unique<Core::MidiOutboundQueue>() }
    , instrumentForwarder_{ std::make_unique<Core::InstrumentMidiForwarder>() }
    , audioPassthroughProcessor_{ std::make_unique<Core::AudioPassthroughProcessor>() }
    , keyboardFromMidiInput_{ std::make_unique<Core::KeyboardFromMidiInput>(*outboundQueue_, *midiActivityTracker_) }
    , midiManager(std::make_unique<MidiManager>(apvts, *outboundQueue_, *midiActivityTracker_))
    , patchModel_{ std::make_unique<Core::PatchModel>() }
    , apvtsPatchMapper_{ std::make_unique<Core::ApvtsPatchMapper>(apvts, *patchModel_) }
    , masterModel_{ std::make_unique<Core::MasterModel>() }
    , apvtsMasterMapper_{ std::make_unique<Core::ApvtsMasterMapper>(apvts, *masterModel_) }
    , patchNameSyncer_{ std::make_unique<Core::PatchNameSyncer>(apvts, *patchModel_) }
    , clipboardService_{ std::make_unique<Core::ClipboardService>() }
{
    patchParameterSysExDispatcher_ = std::make_unique<Core::PatchParameterSysExDispatcher>(
        *patchModel_,
        [this](int parameterNumber, juce::uint8 packedValue)
        {
            midiManager->enqueueRemoteParameterEdit(parameterNumber, packedValue);
        });

    masterParameterSysExDispatcher_ = std::make_unique<Core::MasterParameterSysExDispatcher>(
        *masterModel_,
        [this](const juce::uint8* packedData)
        {
            midiManager->sendMaster(0x03, packedData);
        });

    matrixModBusParameterSysExDispatcher_ = std::make_unique<Core::MatrixModBusParameterSysExDispatcher>(
        *patchModel_,
        [this](juce::uint8 bus, juce::uint8 source, juce::uint8 amount, juce::uint8 destination)
        {
            midiManager->enqueueMatrixModBusEdit(bus, source, amount, destination);
        });

    matrixModSysExCoalesceTimer_ = std::make_unique<MatrixModSysExCoalesceTimer>(
        *matrixModBusParameterSysExDispatcher_);

    matrixModBusReorderService_ = std::make_unique<Core::MatrixModBusReorderService>(
        *patchModel_,
        *apvtsPatchMapper_,
        *matrixModBusParameterSysExDispatcher_);

    matrixModInitService_ = std::make_unique<Core::MatrixModInitService>(
        *patchModel_,
        *apvtsPatchMapper_,
        *matrixModBusParameterSysExDispatcher_);

    sysExParser_ = std::make_unique<SysExParser>();
    sysExDecoder_ = std::make_unique<SysExDecoder>(*sysExParser_);
    initTemplateLoader_ = std::make_unique<Core::InitTemplateLoader>(*sysExDecoder_);
    masterModuleInitService_ = std::make_unique<Core::MasterModuleInitService>(
        *masterModel_,
        *apvtsMasterMapper_,
        *initTemplateLoader_,
        *masterParameterSysExDispatcher_,
        [this]()
        {
            return juce::File(apvts.state.getProperty(PluginIDs::Settings::kInitTemplatesFolderPath).toString());
        });

    patchModuleInitService_ = std::make_unique<Core::PatchModuleInitService>(
        *patchModel_,
        *apvtsPatchMapper_,
        *initTemplateLoader_,
        *patchParameterSysExDispatcher_,
        [this]()
        {
            return juce::File(apvts.state.getProperty(PluginIDs::Settings::kInitTemplatesFolderPath).toString());
        });

    const Core::ActionExecutionHooks actionHooks{
        [this](bool suppress) { suppressMatrixModParameterSysEx_ = suppress; },
        [this](bool suppress) { suppressMasterParameterSysEx_ = suppress; },
        [this](bool suppress) { suppressPatchParameterSysEx_ = suppress; }
    };

    moduleActionHandler_ = std::make_unique<Core::ModuleActionHandler>(
        apvts,
        patchModel_.get(),
        apvtsPatchMapper_.get(),
        clipboardService_.get(),
        matrixModInitService_.get(),
        masterModuleInitService_.get(),
        patchModuleInitService_.get(),
        patchParameterSysExDispatcher_.get(),
        matrixModBusParameterSysExDispatcher_.get(),
        [this]() { refreshClipboardPasteEnabledProperties(); },
        actionHooks);

    patchManagerActionHandler_ = std::make_unique<Core::PatchManagerActionHandler>(
        apvts,
        [this]() { return getResolvedDeviceMemoryLimits(); });

    mutatorActionHandler_ = std::make_unique<Core::MutatorActionHandler>();

    actionDispatcher_ = std::make_unique<Core::ActionDispatcher>(
        *moduleActionHandler_,
        *patchManagerActionHandler_,
        *mutatorActionHandler_);

    validatePluginDescriptorsAtStartup();
    buildChoiceParameterMap();
    buildPatchParameterIdSet();
    buildMasterParameterIdSet();
    buildMatrixModParameterIdSet();
    initializeMidiPortProperties();
    initializeAudioProperties();
    initializeHardwareLatencyProperty();
    initializeInitTemplatesFolderProperty();
    initializePatchNameProperty();
    initializeClipboardPasteEnabledProperties();
    apvts.state.addListener(this);
    deferredMidiPortSyncTimer_ = std::make_unique<DeferredMidiPortSyncTimer>(*this);
    startMidiThread();
    refreshClipboardPasteEnabledProperties();
}

PluginProcessor::~PluginProcessor()
{
    deferredMidiPortSyncTimer_.reset();
    apvts.state.removeListener(this);
}

PluginProcessor::DeferredMidiPortSyncTimer::DeferredMidiPortSyncTimer(PluginProcessor& processorIn)
    : processor(processorIn)
{
}

void PluginProcessor::DeferredMidiPortSyncTimer::startRetrySeries()
{
    stopTimer();
    attemptIndex_ = 0;
    scheduleNextAttempt();
}

int PluginProcessor::DeferredMidiPortSyncTimer::delayMsForAttempt(int attemptIndex)
{
    if (isVst3Wrapper())
    {
        constexpr int vstDelaysMs[] = { 400, 1200, 3000, 6000 };
        return vstDelaysMs[juce::jmin(attemptIndex, kMaxAttempts_ - 1)];
    }

    constexpr int pluginDelaysMs[] = { 300, 800, 2000, 2000 };
    return pluginDelaysMs[juce::jmin(attemptIndex, kMaxAttempts_ - 1)];
}

void PluginProcessor::DeferredMidiPortSyncTimer::scheduleNextAttempt()
{
    startTimer(delayMsForAttempt(attemptIndex_));
}

void PluginProcessor::DeferredMidiPortSyncTimer::timerCallback()
{
    stopTimer();

    const bool isLastAttempt = attemptIndex_ >= kMaxAttempts_ - 1;
    processor.syncMidiPortsFromStateImpl(isLastAttempt);

    if (!processor.arePersistedMidiPortsOpen() && !isLastAttempt)
    {
        ++attemptIndex_;
        scheduleNextAttempt();
    }
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

    if (isStandaloneWrapper())
        Core::StandaloneAudioInputRouter::enableInputMonitoring();

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

void PluginProcessor::ensureAudioInputBusEnabled()
{
    if (getBusCount(true) <= 0)
        return;

    enableAllBuses();
}

int PluginProcessor::getAudioFromInputChannelCount() const noexcept
{
    if (getBusCount(true) <= 0)
        return 0;

    return getChannelCountOfBus(true, 0);
}

int PluginProcessor::getMainOutputChannelCount() const noexcept
{
    if (getBusCount(false) <= 0)
        return 0;

    return getChannelCountOfBus(false, 0);
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
    instrumentForwarder_->forward(midiMessages, getInstrumentPathEnabled(midiMessages), *outboundQueue_, *midiActivityTracker_);

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

void PluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
            syncAudioRuntimeFromState();
            syncHardwareLatencyFromState();
            syncMidiPortsFromState(false);
            scheduleDeferredMidiPortSyncForPluginHost();

            if (shouldUseDevelopmentLogging())
                ApvtsLogger::getInstance().logStateLoaded("DAW state");
        }
    }
}

bool PluginProcessor::setMidiInputPort(const juce::String& deviceId)
{
    if (midiManager == nullptr)
        return false;

    if (midiManager->setMidiInputPort(deviceId))
    {
        apvts.state.setProperty("midiInputPortId", deviceId, nullptr);
        notifyNonParameterStateChanged();
        return true;
    }

    return false;
}

bool PluginProcessor::setMidiOutputPort(const juce::String& deviceId)
{
    if (midiManager == nullptr)
        return false;

    if (midiManager->setMidiOutputPort(deviceId))
    {
        apvts.state.setProperty("midiOutputPortId", deviceId, nullptr);
        notifyNonParameterStateChanged();
        return true;
    }

    return false;
}

bool PluginProcessor::isStandalone() const
{
    return isStandaloneWrapper();
}

bool PluginProcessor::setKeyboardFromPort(const juce::String& deviceId)
{
    if (!isStandaloneWrapper() || keyboardFromMidiInput_ == nullptr)
        return false;

    if (deviceId.isEmpty())
    {
        keyboardFromMidiInput_->closePort();
        apvts.state.setProperty("keyboardFromEnabled", false, nullptr);
        apvts.state.setProperty("keyboardFromPortId", juce::String(), nullptr);
        return true;
    }

    if (!keyboardFromMidiInput_->setPort(deviceId))
    {
        apvts.state.setProperty("keyboardFromEnabled", false, nullptr);
        apvts.state.setProperty("keyboardFromPortId", juce::String(), nullptr);
        return false;
    }

    apvts.state.setProperty("keyboardFromEnabled", true, nullptr);
    apvts.state.setProperty("keyboardFromPortId", deviceId, nullptr);
    return true;
}

void PluginProcessor::swapMatrixModBusContents(int fromBus, int toBus)
{
    if (matrixModBusReorderService_ == nullptr)
        return;

    suppressMatrixModParameterSysEx_ = true;
    matrixModBusReorderService_->swapBusContents(fromBus, toBus);
    suppressMatrixModParameterSysEx_ = false;
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
        for (const auto& error : validationResult.errors)
        {
            DBG("  ERROR: " + error);
        }
    }
}

float PluginProcessor::dbToLinearGain(float gainDb) noexcept
{
    if (gainDb <= PluginAudioConstants::kSilenceInputGainDb)
        return 0.0f;

    return std::pow(10.0f, gainDb / 20.0f);
}

bool PluginProcessor::getInstrumentPathEnabled(const juce::MidiBuffer& midiMessages) const
{
    if (isStandaloneWrapper())
    {
        const auto property = apvts.state.getProperty("keyboardFromEnabled", false);
        return property.isBool() && static_cast<bool>(property);
    }

    return midiMessages.getNumEvents() > 0;
}

void PluginProcessor::refreshAudioPassthroughLayout(double sampleRate)
{
    const bool inputEnabled = getTotalNumInputChannels() > 0
                              && getAudioFromInputChannelCount() > 0;
    audioPassthroughProcessor_->prepare(getAudioFromInputChannelCount(),
                                        getMainOutputChannelCount(),
                                        inputEnabled,
                                        sampleRate);
}

void PluginProcessor::syncAudioRuntimeFromState()
{
    const auto gainDb = static_cast<float>(apvts.state.getProperty("inputGainDb", 0.0f));
    const float sanitizedDb = std::isfinite(gainDb) ? gainDb : 0.0f;
    const float snappedDb = PluginAudioConstants::snapInputGainDb(sanitizedDb);
    inputGainLinear_.store(dbToLinearGain(snappedDb), std::memory_order_relaxed);

    const auto sourceId = apvts.state.getProperty("audioFromSourceId", juce::String()).toString();

    if (sourceId.isNotEmpty())
    {
        syncAudioPassthroughFromSourceId(sourceId);
    }
    else
    {
        const int channelMode = static_cast<int>(apvts.state.getProperty("audioFromChannelMode", 0));
        audioPassthroughProcessor_->setChannelMode(
            static_cast<Core::AudioFromChannelMode>(juce::jlimit(0, 2, channelMode)));
    }
}

void PluginProcessor::syncAudioPassthroughFromSourceId(const juce::String& sourceId)
{
    const int channelMode = Core::AudioInputSourceCatalog::channelModeForSourceId(sourceId);
    audioPassthroughProcessor_->setChannelMode(static_cast<Core::AudioFromChannelMode>(channelMode));
    audioPassthroughProcessor_->setMonoSourceChannelIndex(
        Core::AudioInputSourceCatalog::monoChannelIndexForSourceId(sourceId));
}

void PluginProcessor::setInputGainDb(float gainDb)
{
    const float snappedDb = PluginAudioConstants::snapInputGainDb(gainDb);
    inputGainLinear_.store(dbToLinearGain(snappedDb), std::memory_order_relaxed);
    apvts.state.setProperty("inputGainDb", snappedDb, nullptr);
}

void PluginProcessor::setHardwareLatencyMs(float latencyMs)
{
    const float quantizedMs = Core::HardwareLatency::quantizeMs(latencyMs);
    apvts.state.setProperty(PluginIDs::Settings::kHardwareLatencyMs, quantizedMs, nullptr);
    applyHardwareLatencyToHost();
    notifyNonParameterStateChanged();
}

float PluginProcessor::getHardwareLatencyMs() const
{
    const auto property = apvts.state.getProperty(PluginIDs::Settings::kHardwareLatencyMs, 0.0f);
    return Core::HardwareLatency::quantizeMs(static_cast<float>(property));
}

int PluginProcessor::getGuiScaleId() const
{
    return static_cast<int>(apvts.state.getProperty(
        PluginIDs::Settings::kGuiScale,
        PluginIDs::Settings::ScaleLevels::kDefault));
}

void PluginProcessor::setGuiScaleId(int scaleId)
{
    apvts.state.setProperty(PluginIDs::Settings::kGuiScale, scaleId, nullptr);
    notifyNonParameterStateChanged();
}

int PluginProcessor::getSkinVariantId() const
{
    return static_cast<int>(apvts.state.getProperty(
        PluginIDs::Settings::kSkinVariant,
        PluginIDs::Settings::SkinVariants::kDefault));
}

void PluginProcessor::setSkinVariantId(int skinVariantId)
{
    apvts.state.setProperty(PluginIDs::Settings::kSkinVariant, skinVariantId, nullptr);
    notifyNonParameterStateChanged();
}

void PluginProcessor::notifyNonParameterStateChanged()
{
    updateHostDisplay(juce::AudioProcessorListener::ChangeDetails().withNonParameterStateChanged(true));
}

void PluginProcessor::initializeHardwareLatencyProperty()
{
    if (!apvts.state.hasProperty(PluginIDs::Settings::kHardwareLatencyMs))
        apvts.state.setProperty(PluginIDs::Settings::kHardwareLatencyMs, Core::HardwareLatency::kMinMs, nullptr);

    syncHardwareLatencyFromState();
}

void PluginProcessor::initializeInitTemplatesFolderProperty()
{
    if (!apvts.state.hasProperty(PluginIDs::Settings::kInitTemplatesFolderPath))
        apvts.state.setProperty(PluginIDs::Settings::kInitTemplatesFolderPath, juce::String(), nullptr);
}

void PluginProcessor::syncHardwareLatencyFromState()
{
    const auto property = apvts.state.getProperty(PluginIDs::Settings::kHardwareLatencyMs, Core::HardwareLatency::kMinMs);
    const float quantizedMs = Core::HardwareLatency::quantizeMs(static_cast<float>(property));

    if (! juce::approximatelyEqual(static_cast<float>(property), quantizedMs))
        apvts.state.setProperty(PluginIDs::Settings::kHardwareLatencyMs, quantizedMs, nullptr);

    applyHardwareLatencyToHost();
}

void PluginProcessor::syncMidiPortsFromStateImpl(bool reportOpenFailures)
{
    if (midiManager == nullptr)
        return;

    auto inputPortId = apvts.state.getProperty("midiInputPortId", juce::String()).toString();
    const auto sanitizedInputPortId = sanitizePersistedMidiInputPortId(inputPortId);

    if (sanitizedInputPortId != inputPortId)
        apvts.state.setProperty("midiInputPortId", sanitizedInputPortId, nullptr);

    midiManager->setMidiInputPort(sanitizedInputPortId, reportOpenFailures);

    auto outputPortId = apvts.state.getProperty("midiOutputPortId", juce::String()).toString();
    const auto sanitizedOutputPortId = sanitizePersistedMidiOutputPortId(outputPortId);

    if (sanitizedOutputPortId != outputPortId)
        apvts.state.setProperty("midiOutputPortId", sanitizedOutputPortId, nullptr);

    midiManager->setMidiOutputPort(sanitizedOutputPortId, reportOpenFailures);
}

void PluginProcessor::syncMidiPortsFromState(bool reportOpenFailures)
{
    if (midiManager == nullptr)
        return;

    if (isStandaloneWrapper())
    {
        syncMidiPortsFromStateImpl(reportOpenFailures);
        return;
    }

    runSyncOnMessageThread([this, reportOpenFailures]()
    {
        syncMidiPortsFromStateImpl(reportOpenFailures);
    });
}

bool PluginProcessor::arePersistedMidiPortsOpen() const
{
    if (midiManager == nullptr)
        return true;

    const auto inputPortId = apvts.state.getProperty("midiInputPortId", juce::String()).toString();
    if (inputPortId.isNotEmpty() && !midiManager->isInputPortOpenWithDevice(inputPortId))
        return false;

    const auto outputPortId = apvts.state.getProperty("midiOutputPortId", juce::String()).toString();
    if (outputPortId.isNotEmpty() && !midiManager->isOutputPortOpenWithDevice(outputPortId))
        return false;

    return true;
}

void PluginProcessor::restoreMidiPortsForHost()
{
    if (isStandaloneWrapper())
    {
        syncMidiPortsFromStateImpl(true);
        return;
    }

    runSyncOnMessageThread([this]()
    {
        syncMidiPortsFromStateImpl(false);

        if (!arePersistedMidiPortsOpen())
            scheduleDeferredMidiPortSyncForPluginHost();
    });
}

void PluginProcessor::scheduleDeferredMidiPortSyncForPluginHost()
{
    if (isStandaloneWrapper() || deferredMidiPortSyncTimer_ == nullptr)
        return;

    runSyncOnMessageThread([this]()
    {
        if (deferredMidiPortSyncTimer_ != nullptr)
            deferredMidiPortSyncTimer_->startRetrySeries();
    });
}

void PluginProcessor::applyHardwareLatencyToHost()
{
    const double sampleRate = audioPassthroughSampleRate_ > 0.0 ? audioPassthroughSampleRate_ : 44100.0;
    const float latencyMs = getHardwareLatencyMs();
    const int latencySamples = Core::HardwareLatency::msToSamples(latencyMs, sampleRate);
    setLatencySamples(latencySamples);
    updateHostDisplay();
}

void PluginProcessor::setAudioFromChannelMode(int mode)
{
    const int clampedMode = juce::jlimit(0, 2, mode);
    audioPassthroughProcessor_->setChannelMode(static_cast<Core::AudioFromChannelMode>(clampedMode));
    apvts.state.setProperty("audioFromChannelMode", clampedMode, nullptr);
}

void PluginProcessor::setAudioFromSourceId(const juce::String& sourceId)
{
    apvts.state.setProperty("audioFromSourceId", sourceId, nullptr);
    syncAudioPassthroughFromSourceId(sourceId);

    const int channelMode = Core::AudioInputSourceCatalog::channelModeForSourceId(sourceId);
    apvts.state.setProperty("audioFromChannelMode", channelMode, nullptr);

    if (isStandaloneWrapper())
        Core::StandaloneAudioInputRouter::enableInputMonitoring();
}

juce::StringArray PluginProcessor::getAudioInputSourceNames() const
{
    juce::StringArray names;
    const auto entries = Core::AudioInputSourceCatalog::buildForProcessor(isStandaloneWrapper());

    for (const auto& entry : entries)
        names.add(entry.displayName);

    return names;
}

juce::StringArray PluginProcessor::getAudioInputSourceIds() const
{
    juce::StringArray ids;
    const auto entries = Core::AudioInputSourceCatalog::buildForProcessor(isStandaloneWrapper());

    for (const auto& entry : entries)
        ids.add(entry.sourceId);

    return ids;
}

void PluginProcessor::initializeAudioProperties()
{
    if (!apvts.state.hasProperty("inputGainDb"))
        apvts.state.setProperty("inputGainDb", 0.0f, nullptr);

    if (!apvts.state.hasProperty("audioFromChannelMode"))
        apvts.state.setProperty("audioFromChannelMode", 0, nullptr);

    if (!apvts.state.hasProperty("audioFromSourceId"))
        apvts.state.setProperty("audioFromSourceId", juce::String(), nullptr);

    const auto savedGainDb = static_cast<float>(apvts.state.getProperty("inputGainDb", 0.0f));
    setInputGainDb(savedGainDb);

    auto savedSourceId = apvts.state.getProperty("audioFromSourceId", juce::String()).toString();

    if (savedSourceId.isEmpty())
    {
        const auto savedChannelMode = static_cast<int>(apvts.state.getProperty("audioFromChannelMode", 0));

        switch (savedChannelMode)
        {
            case 1: savedSourceId = "mono:0"; break;
            case 2: savedSourceId = "mono:1"; break;
            case 0:
            default: savedSourceId = "stereo:0"; break;
        }
    }

    if (savedSourceId.isNotEmpty())
        setAudioFromSourceId(savedSourceId);
}

void PluginProcessor::initializeMidiPortProperties()
{
    if (!apvts.state.hasProperty("midiInputPortId"))
    {
        apvts.state.setProperty("midiInputPortId", juce::String(), nullptr);
    }
    if (!apvts.state.hasProperty("midiOutputPortId"))
    {
        apvts.state.setProperty("midiOutputPortId", juce::String(), nullptr);
    }

    if (!apvts.state.hasProperty("keyboardFromEnabled"))
    {
        apvts.state.setProperty("keyboardFromEnabled", false, nullptr);
    }

    if (!apvts.state.hasProperty("keyboardFromPortId"))
    {
        apvts.state.setProperty("keyboardFromPortId", juce::String(), nullptr);
    }
    
    if (apvts.state.hasProperty("guiZoomLevelId"))
    {
        const auto oldValue = apvts.state.getProperty("guiZoomLevelId");
        apvts.state.setProperty(PluginIDs::Settings::kGuiScale, oldValue, nullptr);
        apvts.state.removeProperty("guiZoomLevelId", nullptr);
    }
    
    if (!apvts.state.hasProperty(PluginIDs::Settings::kGuiScale))
    {
        apvts.state.setProperty(PluginIDs::Settings::kGuiScale,
                                PluginIDs::Settings::ScaleLevels::kDefault,
                                nullptr);
    }

    if (!apvts.state.hasProperty(PluginIDs::Settings::kSkinVariant))
    {
        apvts.state.setProperty(PluginIDs::Settings::kSkinVariant,
                                PluginIDs::Settings::SkinVariants::kDefault,
                                nullptr);
    }
}

void PluginProcessor::initializePatchNameProperty()
{
    using namespace PluginIDs::PatchEditSection::PatchNameModule;
    using namespace PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets;

    if (!apvts.state.hasProperty(kPatchName))
        apvts.state.setProperty(kPatchName, juce::String(kDefaultPatchName), nullptr);
}

void PluginProcessor::ensureDevelopmentLoggingStarted()
{
    if (!shouldUseDevelopmentLogging() || developmentLoggingStarted_)
        return;

    developmentLoggingStarted_ = true;
    enableFileLoggingForSession();
    enableApvtsLogging();
}

void PluginProcessor::enableFileLoggingForSession()
{
    MidiLogger::getInstance().setLogLevel(MidiLogger::LogLevel::kDebug);
    MidiLogger::getInstance().setLogToFile(true);
}

void PluginProcessor::closeLogFileForSession()
{
    MidiLogger::getInstance().setLogToFile(false);
}

void PluginProcessor::enableApvtsLogging()
{
    ApvtsLogger::getInstance().setLogLevel(ApvtsLogger::LogLevel::kDebug);
    ApvtsLogger::getInstance().setLogToConsole(true);
    ApvtsLogger::getInstance().setLogToFile(true);
    ApvtsLogger::getInstance().logInfo("APVTS logging enabled");
}

void PluginProcessor::disableApvtsLogging()
{
    ApvtsLogger::getInstance().setLogToFile(false);
}

juce::String PluginProcessor::getThreadNameForLogging() const
{
    juce::String threadName;
    if (juce::Thread::getCurrentThread() != nullptr)
        threadName = juce::Thread::getCurrentThread()->getThreadName();
    else if (juce::MessageManager::getInstance()->isThisTheMessageThread())
        threadName = "MessageThread";
    else
        threadName = "Unknown";
    return simplifyThreadNameForLogging(threadName);
}

juce::String PluginProcessor::simplifyThreadNameForLogging(const juce::String& threadName)
{
    if (threadName == "MessageThread")
        return "Message";
    if (threadName.startsWith("Audio"))
        return "Audio";
    if (threadName.startsWith("MIDI") || threadName.startsWith("Midi"))
        return "MIDI";
    return threadName;
}

juce::String PluginProcessor::findParameterIdInDirectTree(juce::ValueTree& tree) const
{
    juce::Identifier treeType = tree.getType();
    juce::String treeTypeStr = treeType.toString();
    
    if (treeTypeStr == ApvtsTypes::kParam)
    {
        juce::var idProperty = tree.getProperty("id");
        if (idProperty.isString() && idProperty.toString().isNotEmpty())
            return idProperty.toString();
        
        idProperty = tree.getProperty("parameterID");
        if (idProperty.isString() && idProperty.toString().isNotEmpty())
            return idProperty.toString();
    }
    
    return treeTypeStr;
}

juce::String PluginProcessor::findParameterIdInParentTree(juce::ValueTree& tree) const
{
    juce::ValueTree parentTree = tree.getParent();
    if (!parentTree.isValid())
        return juce::String();
    
    juce::Identifier parentType = parentTree.getType();
    juce::String parentTypeStr = parentType.toString();
    
    if (parentTypeStr == ApvtsTypes::kParam || parentTypeStr == ApvtsTypes::kRoot)
        return juce::String();
    
    juce::String parameterId = parentTypeStr;
    auto* parameter = apvts.getParameter(parameterId);
    
    if (parameter != nullptr)
    {
        juce::String paramId = parameter->getParameterID();
        if (paramId.isNotEmpty())
            parameterId = paramId;
    }
    
    return parameterId;
}

juce::String PluginProcessor::findParameterIdInChildren(juce::ValueTree& changedTree, const juce::var& newValue) const
{
    for (int i = 0; i < apvts.state.getNumChildren(); ++i)
    {
        juce::ValueTree child = apvts.state.getChild(i);
        if (!child.isValid())
            continue;
        
        juce::Identifier childType = child.getType();
        juce::String childTypeStr = childType.toString();
        
        if (childTypeStr == ApvtsTypes::kParam)
        {
            juce::var idProperty = child.getProperty("id");
            if (idProperty.isString() && idProperty.toString().isNotEmpty())
            {
                juce::String childParamId = idProperty.toString();
                if (child == changedTree || 
                    child.getChildWithProperty(ApvtsTypes::kValue, newValue) == changedTree)
                {
                    return childParamId;
                }
            }
        }
        else if (child == changedTree)
        {
            return childTypeStr;
        }
    }
    
    return juce::String();
}

juce::String PluginProcessor::resolveParameterIdFromTree(juce::ValueTree& tree, const juce::Identifier& property) const
{
    juce::String propertyId = property.toString();
    if (propertyId != ApvtsTypes::kValue)
        return propertyId;

    juce::String parameterId = resolveParameterIdFromValueProperty(tree, property);
    return getCanonicalParameterId(parameterId);
}

juce::String PluginProcessor::resolveParameterIdFromValueProperty(
    juce::ValueTree& tree, const juce::Identifier& property) const
{
    juce::String parameterId = findParameterIdInDirectTree(tree);
    if (apvts.getParameter(parameterId) != nullptr)
        return parameterId;

    juce::String parentParamId = findParameterIdInParentTree(tree);
    if (parentParamId.isNotEmpty())
        return parentParamId;

    juce::var newValue = tree.getProperty(property);
    juce::String childParamId = findParameterIdInChildren(tree, newValue);
    if (childParamId.isNotEmpty())
        return childParamId;

    return parameterId;
}

juce::String PluginProcessor::getCanonicalParameterId(const juce::String& parameterId) const
{
    auto* parameter = apvts.getParameter(parameterId);
    if (parameter == nullptr)
        return parameterId;
    juce::String canonicalId = parameter->getParameterID();
    return canonicalId.isNotEmpty() ? canonicalId : parameterId;
}

void PluginProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                              const juce::Identifier& property)
{
    juce::var newValue = treeWhosePropertyHasChanged.getProperty(property);
    juce::String threadName = getThreadNameForLogging();
    juce::String parameterId = resolveParameterIdFromTree(treeWhosePropertyHasChanged, property);
    juce::String choiceLabel = getChoiceLabelForNumericValue(parameterId, newValue);

    if (shouldUseDevelopmentLogging())
    {
        ApvtsLogger::getInstance().logValueTreePropertyChanged(
            juce::Identifier(parameterId),
            juce::var(),
            newValue,
            threadName,
            choiceLabel
        );
    }

    if (patchParameterIds_.count(parameterId) > 0)
    {
        const bool isMatrixModParam = matrixModParameterIds_.count(parameterId) > 0;

        if (isMatrixModParam)
        {
            if (!suppressMatrixModParameterSysEx_)
                apvtsPatchMapper_->apvtsToBuffer();

            if (!suppressMatrixModParameterSysEx_)
                matrixModSysExCoalesceTimer_->noteParameterChanged(parameterId);
        }
        else
        {
            if (!suppressPatchParameterSysEx_)
                apvtsPatchMapper_->apvtsToBuffer();

            if (!suppressPatchParameterSysEx_)
                patchParameterSysExDispatcher_->dispatch(parameterId);
        }
    }

    if (masterParameterIds_.count(parameterId) > 0)
    {
        if (!suppressMasterParameterSysEx_)
            apvtsMasterMapper_->apvtsToBuffer();

        if (!suppressMasterParameterSysEx_)
            masterParameterSysExDispatcher_->dispatch(parameterId);
    }

    if (parameterId == PluginIDs::PatchEditSection::PatchNameModule::kPatchName)
        patchNameSyncer_->apvtsToBuffer();

    handleBankNumberChange(parameterId);
    handlePatchNumberChange(parameterId);

    if (Core::ActionPropertyRegistry::isActionProperty(parameterId))
        actionDispatcher_->onActionPropertyChanged(parameterId, newValue);

    const auto propertyName = property.toString();
    if (propertyName == MatrixDeviceTypes::kApvtsPropertyName
        || propertyName == "deviceDetected")
    {
        reconcilePatchManagerCoordinatesForDeviceType();
    }
}

juce::String PluginProcessor::getChoiceLabelForNumericValue(const juce::String& parameterId, const juce::var& newValue) const
{
    if (!newValue.isInt() && !newValue.isInt64() && !newValue.isDouble())
        return {};
    
    if (auto label = getChoiceLabel(parameterId, static_cast<int>(newValue)))
        return *label;
    
    return {};
}

void PluginProcessor::handleBankNumberChange(const juce::String& parameterId)
{
    if (parameterId != PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber)
        return;

    const auto limits = getResolvedDeviceMemoryLimits();
    const int bankNumber = static_cast<int>(apvts.state.getProperty(parameterId, 0));

    if (!limits.hasBankConcept())
    {
        if (bankNumber != 0)
            apvts.state.setProperty(parameterId, 0, nullptr);
        return;
    }

    if (bankNumber < limits.minBankNumber() || bankNumber > limits.maxBankNumber())
    {
        apvts.state.setProperty(parameterId,
                                juce::jlimit(limits.minBankNumber(),
                                             limits.maxBankNumber(),
                                             bankNumber),
                                nullptr);
    }
}

void PluginProcessor::handlePatchNumberChange(const juce::String& parameterId)
{
    if (parameterId != PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber)
        return;

    const auto limits = getResolvedDeviceMemoryLimits();
    const int patchNumber = static_cast<int>(apvts.state.getProperty(parameterId, 0));
    const int clampedPatch = juce::jlimit(limits.minPatchNumber(), limits.maxPatchNumber(), patchNumber);

    if (clampedPatch != patchNumber)
    {
        apvts.state.setProperty(parameterId, clampedPatch, nullptr);
        return;
    }

    if (midiManager != nullptr)
        midiManager->sendProgramChange(clampedPatch);
}

Core::DeviceMemoryLimits PluginProcessor::getResolvedDeviceMemoryLimits() const
{
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    return Core::DeviceMemoryLimits::resolve(deviceType);
}

void PluginProcessor::reconcilePatchManagerCoordinatesForDeviceType()
{
    const auto limits = getResolvedDeviceMemoryLimits();

    using namespace PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;

    const int bankNumber = static_cast<int>(apvts.state.getProperty(kCurrentBankNumber, 0));

    if (!limits.hasBankConcept())
    {
        if (bankNumber != 0)
            apvts.state.setProperty(kCurrentBankNumber, 0, nullptr);
    }
    else
    {
        const int clampedBank = juce::jlimit(limits.minBankNumber(),
                                             limits.maxBankNumber(),
                                             bankNumber);
        if (clampedBank != bankNumber)
            apvts.state.setProperty(kCurrentBankNumber, clampedBank, nullptr);
    }

    const int patchNumber = static_cast<int>(apvts.state.getProperty(kCurrentPatchNumber, 0));
    const int clampedPatch = juce::jlimit(limits.minPatchNumber(), limits.maxPatchNumber(), patchNumber);
    if (clampedPatch != patchNumber)
        apvts.state.setProperty(kCurrentPatchNumber, clampedPatch, nullptr);
}

void PluginProcessor::initializeClipboardPasteEnabledProperties()
{
    namespace PatchEdit = PluginIDs::PatchEditSection;
    namespace InternalPatches = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;
    namespace MatrixMod = PluginIDs::MatrixModulationSection::StandaloneWidgets;

    const char* pasteEnabledIds[] = {
        PatchEdit::Dco1Module::StandaloneWidgets::kPasteEnabled,
        PatchEdit::Dco2Module::StandaloneWidgets::kPasteEnabled,
        PatchEdit::Envelope1Module::StandaloneWidgets::kPasteEnabled,
        PatchEdit::Envelope2Module::StandaloneWidgets::kPasteEnabled,
        PatchEdit::Envelope3Module::StandaloneWidgets::kPasteEnabled,
        PatchEdit::Lfo1Module::StandaloneWidgets::kPasteEnabled,
        PatchEdit::Lfo2Module::StandaloneWidgets::kPasteEnabled,
        InternalPatches::kPastePatchEnabled,
        MatrixMod::kMatrixModulationPasteEnabled
    };

    for (const auto* propertyId : pasteEnabledIds)
    {
        if (!apvts.state.hasProperty(propertyId))
            apvts.state.setProperty(propertyId, false, nullptr);
    }
}

void PluginProcessor::refreshClipboardPasteEnabledProperties()
{
    if (clipboardService_ == nullptr)
        return;

    namespace PatchEdit = PluginIDs::PatchEditSection;
    namespace InternalPatches = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;
    namespace MatrixMod = PluginIDs::MatrixModulationSection::StandaloneWidgets;

    const auto state = Core::resolvePasteEnabled(*clipboardService_);

    apvts.state.setProperty(PatchEdit::Dco1Module::StandaloneWidgets::kPasteEnabled, state.dco1, nullptr);
    apvts.state.setProperty(PatchEdit::Dco2Module::StandaloneWidgets::kPasteEnabled, state.dco2, nullptr);
    apvts.state.setProperty(PatchEdit::Envelope1Module::StandaloneWidgets::kPasteEnabled, state.env1, nullptr);
    apvts.state.setProperty(PatchEdit::Envelope2Module::StandaloneWidgets::kPasteEnabled, state.env2, nullptr);
    apvts.state.setProperty(PatchEdit::Envelope3Module::StandaloneWidgets::kPasteEnabled, state.env3, nullptr);
    apvts.state.setProperty(PatchEdit::Lfo1Module::StandaloneWidgets::kPasteEnabled, state.lfo1, nullptr);
    apvts.state.setProperty(PatchEdit::Lfo2Module::StandaloneWidgets::kPasteEnabled, state.lfo2, nullptr);
    apvts.state.setProperty(InternalPatches::kPastePatchEnabled, state.internalPatches, nullptr);
    apvts.state.setProperty(MatrixMod::kMatrixModulationPasteEnabled, state.matrixModulation, nullptr);
}

void PluginProcessor::valueTreeChildAdded(juce::ValueTree& parentTree,
                                         juce::ValueTree& childWhichHasBeenAdded)
{
    juce::ignoreUnused(parentTree, childWhichHasBeenAdded);

    if (shouldUseDevelopmentLogging())
        ApvtsLogger::getInstance().logDebug("ValueTree child added: " + childWhichHasBeenAdded.getType().toString());
}

void PluginProcessor::valueTreeChildRemoved(juce::ValueTree& parentTree,
                                           juce::ValueTree& childWhichHasBeenRemoved,
                                           int indexFromWhichChildWasRemoved)
{
    juce::ignoreUnused(parentTree, childWhichHasBeenRemoved, indexFromWhichChildWasRemoved);

    if (shouldUseDevelopmentLogging())
        ApvtsLogger::getInstance().logDebug("ValueTree child removed: " + childWhichHasBeenRemoved.getType().toString());
}

void PluginProcessor::valueTreeChildOrderChanged(juce::ValueTree& parentTreeWhoseChildrenHaveChanged,
                                                int oldIndex, int newIndex)
{
    juce::ignoreUnused(parentTreeWhoseChildrenHaveChanged, oldIndex, newIndex);

    if (shouldUseDevelopmentLogging())
        ApvtsLogger::getInstance().logDebug("ValueTree child order changed");
}

void PluginProcessor::valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged)
{
    juce::ignoreUnused(treeWhoseParentHasChanged);

    if (shouldUseDevelopmentLogging())
        ApvtsLogger::getInstance().logDebug("ValueTree parent changed");
}

void PluginProcessor::valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged)
{
    juce::ignoreUnused(treeWhichHasBeenChanged);

    if (shouldUseDevelopmentLogging())
        ApvtsLogger::getInstance().logStateReplaced();

    apvtsPatchMapper_->apvtsToBuffer();
    apvtsMasterMapper_->apvtsToBuffer();
    patchNameSyncer_->apvtsToBuffer();
    syncAudioRuntimeFromState();
    refreshClipboardPasteEnabledProperties();
}

void PluginProcessor::buildPatchParameterIdSet()
{
    for (const auto& d : Core::ApvtsPatchMapper::buildIntDescriptors())
        patchParameterIds_.insert(d.parameterId);

    for (const auto& d : Core::ApvtsPatchMapper::buildChoiceDescriptors())
        patchParameterIds_.insert(d.parameterId);
}

void PluginProcessor::buildMasterParameterIdSet()
{
    for (const auto& d : Core::ApvtsMasterMapper::buildIntDescriptors())
        masterParameterIds_.insert(d.parameterId);

    for (const auto& d : Core::ApvtsMasterMapper::buildChoiceDescriptors())
        masterParameterIds_.insert(d.parameterId);
}

void PluginProcessor::buildMatrixModParameterIdSet()
{
    using namespace PluginDescriptors::MatrixModulationSection;

    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
    {
        for (const auto& d : kModulationBusIntParameters[static_cast<size_t>(bus)])
            matrixModParameterIds_.insert(d.parameterId);

        for (const auto& d : kModulationBusChoiceParameters[static_cast<size_t>(bus)])
            matrixModParameterIds_.insert(d.parameterId);
    }
}

void PluginProcessor::buildChoiceParameterMap()
{
    auto allChoiceParams = ApvtsFactory::getAllChoiceParameters();
    
    for (const auto& param : allChoiceParams)
    {
        choiceParameterMap_[param.parameterId] = param;
    }
}

std::optional<juce::String> PluginProcessor::getChoiceLabel(const juce::String& parameterId, int value) const
{
    auto it = choiceParameterMap_.find(parameterId);
    if (it == choiceParameterMap_.end())
        return std::nullopt;
    
    const auto& descriptor = it->second;
    
    if (value < 0 || value >= descriptor.choices.size())
        return std::nullopt;
    
    return descriptor.choices[value];
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}