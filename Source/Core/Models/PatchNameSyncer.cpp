#include "PatchNameSyncer.h"

#include "Core/Models/PatchModel.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

PatchNameSyncer::PatchNameSyncer(juce::AudioProcessorValueTreeState& apvts, PatchModel& model)
    : apvts_(apvts)
    , model_(model)
{
}

void PatchNameSyncer::apvtsToBuffer()
{
    using PluginIDs::PatchEditSection::PatchNameModule::kPatchName;

    if (!apvts_.state.hasProperty(kPatchName))
        return;

    const auto raw = apvts_.state.getProperty(kPatchName).toString();
    model_.setName(truncateToMaxLength(raw));
}

void PatchNameSyncer::bufferToApvts()
{
    apvts_.state.setProperty(
        PluginIDs::PatchEditSection::PatchNameModule::kPatchName,
        model_.getName(),
        nullptr);
}

juce::String PatchNameSyncer::truncateToMaxLength(const juce::String& name)
{
    return name.substring(0, PatchModel::kNameLength);
}

} // namespace Core
