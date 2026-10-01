// Extracted from PluginProcessor.cpp for modular maintenance.
// Parameter-id resolution, choice labels, and Patch Manager bank / patch coordinates.

#include "PluginProcessor.h"

#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/MIDI/MasterEditGate.h"
#include "Core/MIDI/PatchSelectionMidiSync.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Factories/ApvtsFactory.h"
#include "MIDI/MidiManager.h"
#include "Shared/Definitions/ApvtsTypes.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

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

juce::String PluginProcessor::getChoiceLabelForNumericValue(const juce::String& parameterId, const juce::var& newValue) const
{
    if (!newValue.isInt() && !newValue.isInt64() && !newValue.isDouble())
        return {};

    if (auto label = getChoiceLabel(parameterId, static_cast<int>(newValue)))
        return *label;

    return {};
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

    if (suppressPatchSelectionMidiSync_)
    {
        trackSuppressedPatchNumberChange(parameterId);
        return;
    }

    const auto limits = getResolvedDeviceMemoryLimits();
    const int patchNumber = static_cast<int>(apvts.state.getProperty(parameterId, 0));
    const int clampedPatch = juce::jlimit(limits.minPatchNumber(), limits.maxPatchNumber(), patchNumber);

    if (clampPatchNumberIfOutOfRange(parameterId, patchNumber, clampedPatch))
        return;

    if (! lastKnownPatchNumberInitialized_)
    {
        // Seed from the APVTS value that was current before this write is impossible here
        // (property already updated). Prefer startup/session sync; fall back to min patch.
        lastKnownPatchNumber_ = limits.minPatchNumber();
        lastKnownPatchNumberInitialized_ = true;
    }

    // History gate before this patch-context change; on Cancel restore the previous number.
    if (! confirmPatchContextChangeGate())
    {
        suppressPatchSelectionMidiSync_ = true;
        apvts.state.setProperty(parameterId, lastKnownPatchNumber_, nullptr);
        suppressPatchSelectionMidiSync_ = false;
        return;
    }

    applyAcceptedPatchNumberChange(parameterId, clampedPatch);
}

// Coordinate writes made under suppression (bank buttons, reset) still refresh the last
// user-visible patch number so the history-gate revert always has something to restore.
void PluginProcessor::trackSuppressedPatchNumberChange(const juce::String& parameterId)
{
    lastKnownPatchNumber_ = static_cast<int>(apvts.state.getProperty(parameterId, lastKnownPatchNumber_));
    lastKnownPatchNumberInitialized_ = true;
}

bool PluginProcessor::clampPatchNumberIfOutOfRange(const juce::String& parameterId,
                                                   int patchNumber,
                                                   int clampedPatch)
{
    if (clampedPatch == patchNumber)
        return false;

    apvts.state.setProperty(parameterId, clampedPatch, nullptr);
    return true;
}

void PluginProcessor::applyAcceptedPatchNumberChange(const juce::String& parameterId, int clampedPatch)
{
    juce::ignoreUnused(parameterId);

    const auto limits = getResolvedDeviceMemoryLimits();
    const int priorPatchNumber = lastKnownPatchNumber_;
    const int priorBank = static_cast<int>(apvts.state.getProperty(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber,
        limits.minBankNumber()));
    const int priorSelectedBank = static_cast<int>(apvts.state.getProperty(
        PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kSelectedBank,
        priorBank));
    const bool priorCoordinatesEstablished = static_cast<bool>(apvts.state.getProperty(
        PluginIDs::PatchManagerSection::StateProperties::kPatchCoordinatesEstablished,
        false));
    const int priorNavigationFocus = static_cast<int>(apvts.state.getProperty(
        PluginIDs::PatchManagerSection::StateProperties::kNavigationFocus,
        PluginIDs::PatchManagerSection::NavigationFocus::kDefault));

    lastKnownPatchNumber_ = clampedPatch;
    lastKnownPatchNumberInitialized_ = true;

    // Defer patchLoadContext_ until a successful dump (handler success hook). Updating it here
    // would leave Mutator Export naming on the NumberBox target even when the dump fails and the
    // editor buffer was never replaced (KEEP_DISPLAYED keeps the shown slot).

    sendPatchSelectionForAcceptedChange(clampedPatch);

    // Typing a slot number is an explicit Internal choice, so it pins the coordinates too.
    apvts.state.setProperty(
        PluginIDs::PatchManagerSection::StateProperties::kPatchCoordinatesEstablished,
        true,
        nullptr);
    apvts.state.setProperty(
        PluginIDs::PatchManagerSection::StateProperties::kNavigationFocus,
        PluginIDs::PatchManagerSection::NavigationFocus::kInternal,
        nullptr);

    // Mirror the synth's edit buffer into the editor (clears Mutator history via onPatchLoaded).
    // Pass the pre-navigation UI snapshot with the pending load; dump fail keeps the displayed
    // NumberBox (KEEP_DISPLAYED). The mid-wait edit abort gate uses bufferAtRequest vs live model.
    if (patchManagerActionHandler_ != nullptr)
    {
        patchManagerActionHandler_->loadCurrentPatchFromDevice(
            limits,
            Core::PatchManagerActionHandler::InternalCoordinatesSnapshot {
                priorBank, priorPatchNumber, priorSelectedBank,
                priorCoordinatesEstablished, priorNavigationFocus });
    }
}

void PluginProcessor::sendPatchSelectionForAcceptedChange(int clampedPatch)
{
    const auto limits = getResolvedDeviceMemoryLimits();

    if (patchSelectionMidiSync_ == nullptr)
    {
        if (midiManager != nullptr)
            midiManager->sendProgramChange(clampedPatch);
        return;
    }

    const int bankNumber = static_cast<int>(apvts.state.getProperty(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber,
        limits.minBankNumber()));
    patchSelectionMidiSync_->syncSelection(bankNumber, clampedPatch, limits, false);
}

void PluginProcessor::updateDevicePatchLoadContext()
{
    using namespace PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;

    const auto limits = getResolvedDeviceMemoryLimits();
    const int bank = static_cast<int>(apvts.state.getProperty(kCurrentBankNumber, limits.minBankNumber()));
    const int patch = static_cast<int>(apvts.state.getProperty(kCurrentPatchNumber, limits.minPatchNumber()));
    patchLoadContext_ = Core::PatchLoadContext::deviceMemory(bank, patch);
}

Core::DeviceMemoryLimits PluginProcessor::getResolvedDeviceMemoryLimits() const
{
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    return Core::DeviceMemoryLimits::resolve(deviceType);
}

bool PluginProcessor::isMasterEditOutboundAllowed() const
{
    const bool deviceDetected = static_cast<bool>(apvts.state.getProperty("deviceDetected", false));
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    return Core::isMasterEditAllowed(deviceDetected, deviceType);
}

void PluginProcessor::reconcilePatchManagerCoordinatesForDeviceType()
{
    const auto limits = getResolvedDeviceMemoryLimits();

    using namespace PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;
    namespace BankState = PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties;

    const int bankNumber = static_cast<int>(apvts.state.getProperty(kCurrentBankNumber, 0));

    if (!limits.hasBankConcept())
    {
        if (bankNumber != 0)
        {
            apvts.state.setProperty(kCurrentBankNumber, 0, nullptr);
            apvts.state.setProperty(BankState::kSelectedBank, 0, nullptr);
        }
    }
    else
    {
        const int clampedBank = juce::jlimit(limits.minBankNumber(),
                                             limits.maxBankNumber(),
                                             bankNumber);
        if (clampedBank != bankNumber)
        {
            apvts.state.setProperty(kCurrentBankNumber, clampedBank, nullptr);
            apvts.state.setProperty(BankState::kSelectedBank, clampedBank, nullptr);
        }
    }

    const int patchNumber = static_cast<int>(apvts.state.getProperty(kCurrentPatchNumber, 0));
    const int clampedPatch = juce::jlimit(limits.minPatchNumber(), limits.maxPatchNumber(), patchNumber);
    if (clampedPatch != patchNumber)
        apvts.state.setProperty(kCurrentPatchNumber, clampedPatch, nullptr);

    if (patchSelectionMidiSync_ != nullptr)
        patchSelectionMidiSync_->clearSyncedBankState();
}

void PluginProcessor::resetInternalPatchCoordinatesToDefaults()
{
    using namespace PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;
    namespace BankState = PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties;
    namespace PatchManagerState = PluginIDs::PatchManagerSection::StateProperties;

    const auto limits = getResolvedDeviceMemoryLimits();
    const int defaultBank = limits.hasBankConcept() ? limits.minBankNumber() : 0;
    const int defaultPatch = limits.minPatchNumber();

    suppressPatchSelectionMidiSync_ = true;
    apvts.state.setProperty(kCurrentBankNumber, defaultBank, nullptr);
    apvts.state.setProperty(kCurrentPatchNumber, defaultPatch, nullptr);
    apvts.state.setProperty(BankState::kSelectedBank, defaultBank, nullptr);

    // The synth cannot be queried for its current slot, so these defaults are a guess: the UI
    // must show them as undefined until the user pins a real slot.
    apvts.state.setProperty(PatchManagerState::kPatchCoordinatesEstablished, false, nullptr);
    apvts.state.setProperty(PatchManagerState::kNavigationFocus,
                            PluginIDs::PatchManagerSection::NavigationFocus::kNone,
                            nullptr);
    suppressPatchSelectionMidiSync_ = false;

    lastKnownPatchNumber_ = defaultPatch;
    lastKnownPatchNumberInitialized_ = true;

    if (patchSelectionMidiSync_ != nullptr)
        patchSelectionMidiSync_->clearSyncedBankState();
}
