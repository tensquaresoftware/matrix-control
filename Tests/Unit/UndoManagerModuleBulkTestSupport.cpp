#include "UndoManagerModuleBulkTestSupport.h"

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Init/InitTemplateLoader.h"
#include "PatchFixturePaths.h"
#include "Shared/Definitions/ApvtsTypes.h"
#include "Shared/Definitions/PluginIDs.h"

namespace UndoManagerModuleBulkTestSupport
{

UndoBulkTestAudioProcessor::UndoBulkTestAudioProcessor(
    juce::AudioProcessorValueTreeState::ParameterLayout layout)
    : juce::AudioProcessor(BusesProperties())
    , apvts(*this, &undoManager, "P", std::move(layout))
{
}

const juce::String UndoBulkTestAudioProcessor::getName() const { return "UndoBulkHarness"; }
void UndoBulkTestAudioProcessor::prepareToPlay(double, int) {}
void UndoBulkTestAudioProcessor::releaseResources() {}
void UndoBulkTestAudioProcessor::processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) {}
juce::AudioProcessorEditor* UndoBulkTestAudioProcessor::createEditor() { return nullptr; }
bool UndoBulkTestAudioProcessor::hasEditor() const { return false; }
bool UndoBulkTestAudioProcessor::acceptsMidi() const { return false; }
bool UndoBulkTestAudioProcessor::producesMidi() const { return false; }
bool UndoBulkTestAudioProcessor::isMidiEffect() const { return false; }
double UndoBulkTestAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int UndoBulkTestAudioProcessor::getNumPrograms() { return 1; }
int UndoBulkTestAudioProcessor::getCurrentProgram() { return 0; }
void UndoBulkTestAudioProcessor::setCurrentProgram(int) {}
const juce::String UndoBulkTestAudioProcessor::getProgramName(int) { return {}; }
void UndoBulkTestAudioProcessor::changeProgramName(int, const juce::String&) {}
void UndoBulkTestAudioProcessor::getStateInformation(juce::MemoryBlock&) {}
void UndoBulkTestAudioProcessor::setStateInformation(const void*, int) {}

bool moduleStateMatches(const Core::PatchModel& lhs,
                        const Core::PatchModel& rhs,
                        const juce::String& moduleGroupId)
{
    for (const auto& d : Core::ApvtsPatchMapper::buildIntDescriptors())
    {
        if (d.parentGroupId == moduleGroupId && lhs.getValue(d) != rhs.getValue(d))
            return false;
    }

    for (const auto& d : Core::ApvtsPatchMapper::buildChoiceDescriptors())
    {
        if (d.parentGroupId == moduleGroupId && lhs.getChoiceIndex(d) != rhs.getChoiceIndex(d))
            return false;
    }

    return true;
}

juce::AudioProcessorValueTreeState::ParameterLayout makeEnvelopePasteLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    for (const auto& d : PluginDescriptors::PatchEditSection::Envelope1Module::kIntParameters)
    {
        layout.add(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID(d.parameterId, 1), d.displayName,
            d.minValue, d.maxValue, d.defaultValue));
    }

    for (const auto& d : PluginDescriptors::PatchEditSection::Envelope1Module::kChoiceParameters)
    {
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(d.parameterId, 1), d.displayName,
            d.choices, d.defaultIndex));
    }

    return layout;
}

juce::AudioProcessorValueTreeState::ParameterLayout makeDco1Layout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    for (const auto& d : PluginDescriptors::PatchEditSection::Dco1Module::kIntParameters)
    {
        layout.add(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID(d.parameterId, 1), d.displayName,
            d.minValue, d.maxValue, d.defaultValue));
    }

    for (const auto& d : PluginDescriptors::PatchEditSection::Dco1Module::kChoiceParameters)
    {
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(d.parameterId, 1), d.displayName,
            d.choices, d.defaultIndex));
    }

    return layout;
}

namespace
{

juce::String resolveParameterIdFromTree(juce::ValueTree& tree, const juce::Identifier& property)
{
    const juce::String propertyId = property.toString();
    if (propertyId != ApvtsTypes::kValue)
        return {};

    if (tree.getType().toString() == ApvtsTypes::kParam)
    {
        const juce::var idProperty = tree.getProperty("id");
        if (idProperty.isString() && idProperty.toString().isNotEmpty())
            return idProperty.toString();
    }

    return {};
}

Core::ActionExecutionHooks makeBulkUndoHooks(ModuleBulkUndoHarnessBase& harness,
                                             juce::UndoManager& undoManager)
{
    Core::ActionExecutionHooks hooks;
    hooks.setSuppressPatchSysEx = [&harness](bool suppress) { harness.suppressPatchSysEx = suppress; };
    hooks.beginEditorialTransaction = [&undoManager](const juce::String& name)
    {
        undoManager.beginNewTransaction(name);
    };
    return hooks;
}

} // namespace

void ModuleBulkUndoHarnessBase::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                                       const juce::Identifier& property)
{
    if (suppressPatchSysEx || trackedApvtsMapper == nullptr || trackedPatchDispatcher == nullptr)
        return;

    const juce::String parameterId = resolveParameterIdFromTree(treeWhosePropertyHasChanged, property);
    if (parameterId.isEmpty())
        return;

    trackedApvtsMapper->apvtsToBuffer();
    trackedPatchDispatcher->dispatch(parameterId);
}

PasteUndoHarness::PasteUndoHarness(juce::AudioProcessorValueTreeState::ParameterLayout layout)
    : proc(std::move(layout))
    , apvtsMapper(proc.apvts, model)
    , patchParameterSysExDispatcher(model,
                                    [this](int, juce::uint8) { ++patchDispatchCount; })
    , handler(Core::ModuleActionHandler::Dependencies {
                  proc.apvts,
                  &model,
                  &apvtsMapper,
                  &clipboard,
                  nullptr,
                  nullptr,
                  nullptr,
                  &patchParameterSysExDispatcher,
                  nullptr,
                  nullptr
              },
              makeBulkUndoHooks(*this, proc.undoManager))
{
    ModuleBulkUndoHarnessBase::trackedApvtsMapper = &apvtsMapper;
    ModuleBulkUndoHarnessBase::trackedPatchDispatcher = &patchParameterSysExDispatcher;
    proc.apvts.state.addListener(this);
}

PasteUndoHarness::~PasteUndoHarness()
{
    proc.apvts.state.removeListener(this);
}

InitUndoHarness::InitUndoHarness(juce::AudioProcessorValueTreeState::ParameterLayout layout,
                                 const juce::File& folder)
    : proc(std::move(layout))
    , apvtsMapper(proc.apvts, model)
    , decoder(parser)
    , loader(decoder)
    , patchParameterSysExDispatcher(model,
                                    [this](int, juce::uint8) { ++patchDispatchCount; })
    , initService(model, apvtsMapper, loader, patchParameterSysExDispatcher, [this]() { return templatesFolder; })
    , handler(Core::ModuleActionHandler::Dependencies {
                  proc.apvts,
                  &model,
                  &apvtsMapper,
                  nullptr,
                  nullptr,
                  nullptr,
                  &initService,
                  &patchParameterSysExDispatcher,
                  nullptr,
                  nullptr
              },
              makeBulkUndoHooks(*this, proc.undoManager))
    , templatesFolder(folder)
{
    ModuleBulkUndoHarnessBase::trackedApvtsMapper = &apvtsMapper;
    ModuleBulkUndoHarnessBase::trackedPatchDispatcher = &patchParameterSysExDispatcher;
    proc.apvts.state.addListener(this);
}

InitUndoHarness::~InitUndoHarness()
{
    proc.apvts.state.removeListener(this);
}

juce::File createTempTemplatesDir()
{
    auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                   .getNonexistentChildFile("MatrixControlUndoModuleBulk", "", false);
    jassert(dir.createDirectory());
    return dir;
}

void copyInitFixtureToDir(const juce::File& dir)
{
    const auto source = PatchTestFixtures::initFixturesDir()
                            .getChildFile(Core::InitTemplateLoader::kPatchInitFileName);
    source.copyFileTo(dir.getChildFile(Core::InitTemplateLoader::kPatchInitFileName));
}

} // namespace UndoManagerModuleBulkTestSupport
