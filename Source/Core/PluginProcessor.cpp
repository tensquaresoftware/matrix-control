#include <juce_audio_devices/juce_audio_devices.h>

#include "PluginProcessor.h"
#include "GUI/PluginEditor.h"
#include "MIDI/MidiManager.h"
#include "Shared/Definitions/ApvtsTypes.h"
#include "Loggers/MidiLogger.h"
#include "Loggers/ApvtsLogger.h"
#include "Factories/ApvtsFactory.h"

PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
    , apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
    , midiManager(std::make_unique<MidiManager>(apvts))
{
    validatePluginDescriptorsAtStartup();
    buildChoiceParameterMap();
    initializeMidiPortProperties();
    apvts.state.addListener(this);
    
#if JUCE_DEBUG
    enableApvtsLogging();
#endif
}

PluginProcessor::~PluginProcessor()
{
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
    juce::ignoreUnused(sampleRate, samplesPerBlock);
    
#if JUCE_DEBUG
    enableFileLoggingForSession();
    enableApvtsLogging();
#endif
    startMidiThread();
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
    stopMidiThread();
#if JUCE_DEBUG
    disableApvtsLogging();
    closeLogFileForSession();
#endif
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
    juce::ignoreUnused(audioBuffer);
    juce::ignoreUnused(midiMessages);
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
            ApvtsLogger::getInstance().logStateLoaded("DAW state");
        }
    }
}

void PluginProcessor::setMidiInputPort(const juce::String& deviceId)
{
    if (midiManager != nullptr)
    {
        if (midiManager->setMidiInputPort(deviceId))
        {
            apvts.state.setProperty("midiInputPortId", deviceId, nullptr);
        }
    }
}

void PluginProcessor::setMidiOutputPort(const juce::String& deviceId)
{
    if (midiManager != nullptr)
    {
        if (midiManager->setMidiOutputPort(deviceId))
        {
            apvts.state.setProperty("midiOutputPortId", deviceId, nullptr);
        }
    }
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
        jassertfalse;
    }
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
    
    if (apvts.state.hasProperty("guiZoomLevelId"))
    {
        const auto oldValue = apvts.state.getProperty("guiZoomLevelId");
        apvts.state.setProperty(PluginIDs::Settings::kGuiScaleId, oldValue, nullptr);
        apvts.state.removeProperty("guiZoomLevelId", nullptr);
    }
    
    if (!apvts.state.hasProperty(PluginIDs::Settings::kGuiScaleId))
    {
        apvts.state.setProperty(PluginIDs::Settings::kGuiScaleId,
                                PluginIDs::Settings::ScaleLevels::kDefault,
                                nullptr);
    }

    if (!apvts.state.hasProperty(PluginIDs::Settings::kGuiScaleSchemaId))
    {
        const auto legacyVar = apvts.state.getProperty(PluginIDs::Settings::kGuiScaleId);
        const int legacyId = legacyVar.isInt() ? static_cast<int>(legacyVar) : PluginIDs::Settings::ScaleLevels::kDefault;
        const int migratedId = PluginIDs::Settings::ScaleLevels::migrateFromLegacyScaleId(legacyId);
        apvts.state.setProperty(PluginIDs::Settings::kGuiScaleId, migratedId, nullptr);
        apvts.state.setProperty(PluginIDs::Settings::kGuiScaleSchemaId, 1, nullptr);
    }
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

    ApvtsLogger::getInstance().logValueTreePropertyChanged(
        juce::Identifier(parameterId),
        juce::var(),
        newValue,
        threadName,
        choiceLabel
    );

    handleBankNumberChange(parameterId);
    handlePatchNumberChange(parameterId);
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
    juce::ignoreUnused(parameterId);
}

void PluginProcessor::handlePatchNumberChange(const juce::String& parameterId)
{
    if (parameterId != PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber)
        return;
    juce::ignoreUnused(parameterId);
}

void PluginProcessor::valueTreeChildAdded(juce::ValueTree& parentTree,
                                         juce::ValueTree& childWhichHasBeenAdded)
{
    juce::ignoreUnused(parentTree, childWhichHasBeenAdded);
    ApvtsLogger::getInstance().logDebug("ValueTree child added: " + childWhichHasBeenAdded.getType().toString());
}

void PluginProcessor::valueTreeChildRemoved(juce::ValueTree& parentTree,
                                           juce::ValueTree& childWhichHasBeenRemoved,
                                           int indexFromWhichChildWasRemoved)
{
    juce::ignoreUnused(parentTree, childWhichHasBeenRemoved, indexFromWhichChildWasRemoved);
    ApvtsLogger::getInstance().logDebug("ValueTree child removed: " + childWhichHasBeenRemoved.getType().toString());
}

void PluginProcessor::valueTreeChildOrderChanged(juce::ValueTree& parentTreeWhoseChildrenHaveChanged,
                                                int oldIndex, int newIndex)
{
    juce::ignoreUnused(parentTreeWhoseChildrenHaveChanged, oldIndex, newIndex);
    ApvtsLogger::getInstance().logDebug("ValueTree child order changed");
}

void PluginProcessor::valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged)
{
    juce::ignoreUnused(treeWhoseParentHasChanged);
    ApvtsLogger::getInstance().logDebug("ValueTree parent changed");
}

void PluginProcessor::valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged)
{
    juce::ignoreUnused(treeWhichHasBeenChanged);
    ApvtsLogger::getInstance().logStateReplaced();
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