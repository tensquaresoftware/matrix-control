#include "ApvtsPatchMapper.h"

#include "Core/Models/PatchModel.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/PluginDescriptors.h"

namespace Core
{

ApvtsPatchMapper::ApvtsPatchMapper(juce::AudioProcessorValueTreeState& apvts, PatchModel& model)
    : apvts_(apvts)
    , model_(model)
    , intDescriptors_(buildIntDescriptors())
    , choiceDescriptors_(buildChoiceDescriptors())
{
}

void ApvtsPatchMapper::apvtsToBuffer()
{
    for (const auto& d : intDescriptors_)
        syncIntToBuffer(d);

    for (const auto& d : choiceDescriptors_)
        syncChoiceToBuffer(d);
}

void ApvtsPatchMapper::bufferToApvts()
{
    for (const auto& d : intDescriptors_)
        pushIntToApvts(d);

    for (const auto& d : choiceDescriptors_)
        pushChoiceToApvts(d);
}

void ApvtsPatchMapper::pushBusToApvts(int busIndex)
{
    jassert(busIndex >= 0 && busIndex < Matrix1000Limits::kModulationBusCount);

    using namespace PluginDescriptors::MatrixModulationSection;

    const auto& intParams = kModulationBusIntParameters[static_cast<size_t>(busIndex)];
    const auto& choiceParams = kModulationBusChoiceParameters[static_cast<size_t>(busIndex)];

    pushIntToApvts(intParams[0]);
    pushChoiceToApvts(choiceParams[0]);
    pushChoiceToApvts(choiceParams[1]);
}

void ApvtsPatchMapper::pushModuleToApvts(const juce::String& moduleGroupId)
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

void ApvtsPatchMapper::syncIntToBuffer(const PluginDescriptors::IntParameterDescriptor& d)
{
    auto* rawValue = apvts_.getRawParameterValue(d.parameterId);
    if (rawValue != nullptr)
        model_.setValue(d, juce::roundToInt(rawValue->load()));
}

void ApvtsPatchMapper::syncChoiceToBuffer(const PluginDescriptors::ChoiceParameterDescriptor& d)
{
    auto* rawValue = apvts_.getRawParameterValue(d.parameterId);
    if (rawValue != nullptr)
        model_.setChoiceIndex(d, juce::roundToInt(rawValue->load()));
}

void ApvtsPatchMapper::pushIntToApvts(const PluginDescriptors::IntParameterDescriptor& d)
{
    auto* param = apvts_.getParameter(d.parameterId);
    if (param != nullptr)
        param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(model_.getValue(d))));
}

void ApvtsPatchMapper::pushChoiceToApvts(const PluginDescriptors::ChoiceParameterDescriptor& d)
{
    auto* param = apvts_.getParameter(d.parameterId);
    if (param != nullptr)
        param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(model_.getChoiceIndex(d))));
}

std::vector<PluginDescriptors::IntParameterDescriptor> ApvtsPatchMapper::buildIntDescriptors()
{
    using namespace PluginDescriptors::PatchEditSection;
    std::vector<PluginDescriptors::IntParameterDescriptor> result;

    auto add = [&result](const std::vector<PluginDescriptors::IntParameterDescriptor>& params)
    {
        result.insert(result.end(), params.begin(), params.end());
    };

    add(Dco1Module::kIntParameters);
    add(Dco2Module::kIntParameters);
    add(VcfVcaModule::kIntParameters);
    add(FmTrackModule::kIntParameters);
    add(RampPortamentoModule::kIntParameters);
    add(Envelope1Module::kIntParameters);
    add(Envelope2Module::kIntParameters);
    add(Envelope3Module::kIntParameters);
    add(Lfo1Module::kIntParameters);
    add(Lfo2Module::kIntParameters);

    using namespace PluginDescriptors::MatrixModulationSection;
    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        add(kModulationBusIntParameters[static_cast<size_t>(bus)]);

    return result;
}

std::vector<PluginDescriptors::ChoiceParameterDescriptor> ApvtsPatchMapper::buildChoiceDescriptors()
{
    using namespace PluginDescriptors::PatchEditSection;
    std::vector<PluginDescriptors::ChoiceParameterDescriptor> result;

    auto add = [&result](const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& params)
    {
        result.insert(result.end(), params.begin(), params.end());
    };

    add(Dco1Module::kChoiceParameters);
    add(Dco2Module::kChoiceParameters);
    add(VcfVcaModule::kChoiceParameters);
    add(FmTrackModule::kChoiceParameters);
    add(RampPortamentoModule::kChoiceParameters);
    add(Envelope1Module::kChoiceParameters);
    add(Envelope2Module::kChoiceParameters);
    add(Envelope3Module::kChoiceParameters);
    add(Lfo1Module::kChoiceParameters);
    add(Lfo2Module::kChoiceParameters);

    using namespace PluginDescriptors::MatrixModulationSection;
    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        add(kModulationBusChoiceParameters[static_cast<size_t>(bus)]);

    return result;
}

} // namespace Core
