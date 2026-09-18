#include "ApvtsMasterMapper.h"

#include "Core/Models/MasterModel.h"
#include "Core/Models/MidiChannelMasterCodec.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

namespace
{
    bool isMidiChannelChoice(const PluginDescriptors::ChoiceParameterDescriptor& d) noexcept
    {
        return d.parameterId
            == PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel;
    }

    bool isPackedMasterInt(const PluginDescriptors::IntParameterDescriptor& d) noexcept
    {
        return d.sysExOffset != PluginDescriptors::kNoSysExOffset;
    }
}

ApvtsMasterMapper::ApvtsMasterMapper(juce::AudioProcessorValueTreeState& apvts, MasterModel& model)
    : apvts_(apvts)
    , model_(model)
    , intDescriptors_(buildIntDescriptors())
    , choiceDescriptors_(buildChoiceDescriptors())
{
}

void ApvtsMasterMapper::apvtsToBuffer()
{
    for (const auto& d : intDescriptors_)
        syncIntToBuffer(d);

    for (const auto& d : choiceDescriptors_)
        syncChoiceToBuffer(d);
}

void ApvtsMasterMapper::bufferToApvts()
{
    for (const auto& d : intDescriptors_)
        pushIntToApvts(d);

    for (const auto& d : choiceDescriptors_)
        pushChoiceToApvts(d);
}

void ApvtsMasterMapper::pushModuleToApvts(const juce::String& moduleGroupId)
{
    for (const auto& d : intDescriptors_)
    {
        if (d.parentGroupId == moduleGroupId)
            pushIntToApvts(d);
    }

    for (const auto& d : choiceDescriptors_)
    {
        if (d.parentGroupId == moduleGroupId)
            pushChoiceToApvts(d);
    }
}

void ApvtsMasterMapper::syncIntToBuffer(const PluginDescriptors::IntParameterDescriptor& d)
{
    auto* rawValue = apvts_.getRawParameterValue(d.parameterId);
    if (rawValue != nullptr)
        model_.setValue(d, juce::roundToInt(rawValue->load()));
}

void ApvtsMasterMapper::syncChoiceToBuffer(const PluginDescriptors::ChoiceParameterDescriptor& d)
{
    auto* rawValue = apvts_.getRawParameterValue(d.parameterId);
    if (rawValue == nullptr)
        return;

    const int index = juce::roundToInt(rawValue->load());

    if (isMidiChannelChoice(d))
    {
        MidiChannelMasterCodec::applyComboIndex(model_.data(), MasterModel::kBufferSize, index);
        return;
    }

    model_.setChoiceIndex(d, index);
}

void ApvtsMasterMapper::pushIntToApvts(const PluginDescriptors::IntParameterDescriptor& d)
{
    auto* param = apvts_.getParameter(d.parameterId);
    if (param != nullptr)
        param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(model_.getValue(d))));
}

void ApvtsMasterMapper::pushChoiceToApvts(const PluginDescriptors::ChoiceParameterDescriptor& d)
{
    auto* param = apvts_.getParameter(d.parameterId);
    if (param == nullptr)
        return;

    const int index = isMidiChannelChoice(d)
                          ? MidiChannelMasterCodec::readComboIndex(model_.data(), MasterModel::kBufferSize)
                          : model_.getChoiceIndex(d);

    param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(index)));
}

std::vector<PluginDescriptors::IntParameterDescriptor> ApvtsMasterMapper::buildIntDescriptors()
{
    std::vector<PluginDescriptors::IntParameterDescriptor> packed;
    packed.reserve(PluginDescriptors::MasterEditSection::kIntParameters.size());

    for (const auto& d : PluginDescriptors::MasterEditSection::kIntParameters)
    {
        if (isPackedMasterInt(d))
            packed.push_back(d);
    }

    return packed;
}

std::vector<PluginDescriptors::ChoiceParameterDescriptor> ApvtsMasterMapper::buildChoiceDescriptors()
{
    return PluginDescriptors::MasterEditSection::kChoiceParameters;
}

} // namespace Core
