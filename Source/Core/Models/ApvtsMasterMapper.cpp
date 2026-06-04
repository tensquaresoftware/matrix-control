#include "ApvtsMasterMapper.h"

#include "Core/Models/MasterModel.h"
#include "Shared/Definitions/PluginDescriptors.h"

namespace Core
{

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

void ApvtsMasterMapper::syncIntToBuffer(const PluginDescriptors::IntParameterDescriptor& d)
{
    auto* rawValue = apvts_.getRawParameterValue(d.parameterId);
    if (rawValue != nullptr)
        model_.setValue(d, juce::roundToInt(rawValue->load()));
}

void ApvtsMasterMapper::syncChoiceToBuffer(const PluginDescriptors::ChoiceParameterDescriptor& d)
{
    auto* rawValue = apvts_.getRawParameterValue(d.parameterId);
    if (rawValue != nullptr)
        model_.setChoiceIndex(d, juce::roundToInt(rawValue->load()));
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
    if (param != nullptr)
        param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(model_.getChoiceIndex(d))));
}

std::vector<PluginDescriptors::IntParameterDescriptor> ApvtsMasterMapper::buildIntDescriptors()
{
    return PluginDescriptors::MasterEditSection::kIntParameters;
}

std::vector<PluginDescriptors::ChoiceParameterDescriptor> ApvtsMasterMapper::buildChoiceDescriptors()
{
    return PluginDescriptors::MasterEditSection::kChoiceParameters;
}

} // namespace Core
