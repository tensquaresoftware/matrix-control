#include "ApvtsFactory.h"

#include "ApvtsLayoutBuilder.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "Shared/Definitions/PluginIDs.h"

juce::AudioProcessorValueTreeState::ParameterLayout ApvtsFactory::createParameterLayout()
{
    return ApvtsLayoutBuilder::createParameterLayout();
}

std::vector<PluginDescriptors::IntParameterDescriptor> ApvtsFactory::getAllIntParameters()
{
    std::vector<PluginDescriptors::IntParameterDescriptor> allParams;

    auto addParams = [&allParams](const std::vector<PluginDescriptors::IntParameterDescriptor>& params)
    {
        allParams.insert(allParams.end(), params.begin(), params.end());
    };

    addParams(PluginDescriptors::PatchEditSection::Dco1Module::kIntParameters);
    addParams(PluginDescriptors::PatchEditSection::Dco2Module::kIntParameters);
    addParams(PluginDescriptors::PatchEditSection::VcfVcaModule::kIntParameters);
    addParams(PluginDescriptors::PatchEditSection::FmTrackModule::kIntParameters);
    addParams(PluginDescriptors::PatchEditSection::RampPortamentoModule::kIntParameters);
    addParams(PluginDescriptors::PatchEditSection::Envelope1Module::kIntParameters);
    addParams(PluginDescriptors::PatchEditSection::Envelope2Module::kIntParameters);
    addParams(PluginDescriptors::PatchEditSection::Envelope3Module::kIntParameters);
    addParams(PluginDescriptors::PatchEditSection::Lfo1Module::kIntParameters);
    addParams(PluginDescriptors::PatchEditSection::Lfo2Module::kIntParameters);

    addMatrixModulationBusIntParameters(allParams);

    addParams(PluginDescriptors::MasterEditSection::kIntParameters);

    return allParams;
}

std::vector<PluginDescriptors::ChoiceParameterDescriptor> ApvtsFactory::getAllChoiceParameters()
{
    std::vector<PluginDescriptors::ChoiceParameterDescriptor> allParams;

    auto addParams = [&allParams](const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& params)
    {
        allParams.insert(allParams.end(), params.begin(), params.end());
    };

    addParams(PluginDescriptors::PatchEditSection::Dco1Module::kChoiceParameters);
    addParams(PluginDescriptors::PatchEditSection::Dco2Module::kChoiceParameters);
    addParams(PluginDescriptors::PatchEditSection::VcfVcaModule::kChoiceParameters);
    addParams(PluginDescriptors::PatchEditSection::FmTrackModule::kChoiceParameters);
    addParams(PluginDescriptors::PatchEditSection::RampPortamentoModule::kChoiceParameters);
    addParams(PluginDescriptors::PatchEditSection::Envelope1Module::kChoiceParameters);
    addParams(PluginDescriptors::PatchEditSection::Envelope2Module::kChoiceParameters);
    addParams(PluginDescriptors::PatchEditSection::Envelope3Module::kChoiceParameters);
    addParams(PluginDescriptors::PatchEditSection::Lfo1Module::kChoiceParameters);
    addParams(PluginDescriptors::PatchEditSection::Lfo2Module::kChoiceParameters);

    addMatrixModulationBusChoiceParameters(allParams);

    addParams(PluginDescriptors::MasterEditSection::kChoiceParameters);

    return allParams;
}

std::vector<PluginDescriptors::StandaloneWidgetDescriptor> ApvtsFactory::getAllStandaloneWidgets()
{
    std::vector<PluginDescriptors::StandaloneWidgetDescriptor> allWidgets;

    auto addWidgets = [&allWidgets](const std::vector<PluginDescriptors::StandaloneWidgetDescriptor>& widgets)
    {
        allWidgets.insert(allWidgets.end(), widgets.begin(), widgets.end());
    };

    addWidgets(PluginDescriptors::PatchEditSection::Dco1Module::kStandaloneWidgets);
    addWidgets(PluginDescriptors::PatchEditSection::Dco2Module::kStandaloneWidgets);
    addWidgets(PluginDescriptors::PatchEditSection::Envelope1Module::kStandaloneWidgets);
    addWidgets(PluginDescriptors::PatchEditSection::Envelope2Module::kStandaloneWidgets);
    addWidgets(PluginDescriptors::PatchEditSection::Envelope3Module::kStandaloneWidgets);
    addWidgets(PluginDescriptors::PatchEditSection::Lfo1Module::kStandaloneWidgets);
    addWidgets(PluginDescriptors::PatchEditSection::Lfo2Module::kStandaloneWidgets);
    addWidgets(PluginDescriptors::MatrixModulationSection::kStandaloneWidgets);
    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        addWidgets(PluginDescriptors::MatrixModulationSection::kModulationBusStandaloneWidgets[static_cast<size_t>(bus)]);
    addWidgets(PluginDescriptors::PatchManagerSection::BankUtilityModule::kStandaloneWidgets);
    addWidgets(PluginDescriptors::PatchManagerSection::InternalPatchesModule::kStandaloneWidgets);
    addWidgets(PluginDescriptors::PatchManagerSection::ComputerPatchesModule::kStandaloneWidgets);
    addWidgets(PluginDescriptors::PatchManagerSection::PatchMutatorModule::kStandaloneWidgets);
    addWidgets(PluginDescriptors::MasterEditSection::kStandaloneWidgets);

    return allWidgets;
}

void ApvtsFactory::addMatrixModulationBusIntParameters(
    std::vector<PluginDescriptors::IntParameterDescriptor>& allParams)
{
    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
    {
        allParams.insert(allParams.end(),
                        PluginDescriptors::MatrixModulationSection::kModulationBusIntParameters[static_cast<size_t>(bus)].begin(),
                        PluginDescriptors::MatrixModulationSection::kModulationBusIntParameters[static_cast<size_t>(bus)].end());
    }
}

void ApvtsFactory::addMatrixModulationBusChoiceParameters(
    std::vector<PluginDescriptors::ChoiceParameterDescriptor>& allParams)
{
    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
    {
        allParams.insert(allParams.end(),
                        PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[static_cast<size_t>(bus)].begin(),
                        PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[static_cast<size_t>(bus)].end());
    }
}

ApvtsFactory::ValidationResult ApvtsFactory::validatePluginDescriptors()
{
    return ApvtsValidator::validatePluginDescriptors();
}

