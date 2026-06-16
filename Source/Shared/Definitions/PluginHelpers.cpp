#include "PluginHelpers.h"

#include "Matrix1000Limits.h"
#include "PluginDescriptors.h"
#include "Shared/Exceptions/WidgetFactoryExceptions.h"

namespace PluginHelpers
{
    namespace
    {
        bool containsParameterId(const std::vector<PluginDescriptors::IntParameterDescriptor>& parameters,
                                 const juce::String& parameterId)
        {
            for (const auto& parameter : parameters)
            {
                if (parameter.parameterId == parameterId)
                    return true;
            }

            return false;
        }

        bool containsParameterId(const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& parameters,
                                 const juce::String& parameterId)
        {
            for (const auto& parameter : parameters)
            {
                if (parameter.parameterId == parameterId)
                    return true;
            }

            return false;
        }
    }

    juce::StringArray makeStringArray(std::initializer_list<const char*> strings)
    {
        juce::StringArray result;
        for (const char* str : strings)
        {
            result.add(str);
        }
        return result;
    }

    juce::String getGroupDisplayName(const juce::String& groupId)
    {
        for (const auto& group : PluginDescriptors::kAllApvtsGroups)
        {
            if (group.groupId == groupId)
            {
                return group.displayName;
            }
        }
        return groupId;
    }

    juce::String getSectionDisplayName(const char* sectionId)
    {
        return getGroupDisplayName(sectionId);
    }

    std::optional<ParameterWidgetKind> resolveParameterWidgetKind(const juce::String& parameterId)
    {
        const auto isInt = [&](const auto& parameters) { return containsParameterId(parameters, parameterId); };
        const auto isChoice = [&](const auto& parameters) { return containsParameterId(parameters, parameterId); };

        bool foundInt = false;
        bool foundChoice = false;

        auto checkInt = [&](const auto& parameters)
        {
            if (isInt(parameters))
                foundInt = true;
        };

        auto checkChoice = [&](const auto& parameters)
        {
            if (isChoice(parameters))
                foundChoice = true;
        };

        checkInt(PluginDescriptors::PatchEditSection::Dco1Module::kIntParameters);
        checkInt(PluginDescriptors::PatchEditSection::Dco2Module::kIntParameters);
        checkInt(PluginDescriptors::PatchEditSection::VcfVcaModule::kIntParameters);
        checkInt(PluginDescriptors::PatchEditSection::FmTrackModule::kIntParameters);
        checkInt(PluginDescriptors::PatchEditSection::RampPortamentoModule::kIntParameters);
        checkInt(PluginDescriptors::PatchEditSection::Envelope1Module::kIntParameters);
        checkInt(PluginDescriptors::PatchEditSection::Envelope2Module::kIntParameters);
        checkInt(PluginDescriptors::PatchEditSection::Envelope3Module::kIntParameters);
        checkInt(PluginDescriptors::PatchEditSection::Lfo1Module::kIntParameters);
        checkInt(PluginDescriptors::PatchEditSection::Lfo2Module::kIntParameters);
        checkInt(PluginDescriptors::PatchManagerSection::PatchMutatorModule::kIntParameters);
        checkInt(PluginDescriptors::MasterEditSection::kIntParameters);

        for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
            checkInt(PluginDescriptors::MatrixModulationSection::kModulationBusIntParameters[static_cast<size_t>(bus)]);

        checkChoice(PluginDescriptors::PatchEditSection::Dco1Module::kChoiceParameters);
        checkChoice(PluginDescriptors::PatchEditSection::Dco2Module::kChoiceParameters);
        checkChoice(PluginDescriptors::PatchEditSection::VcfVcaModule::kChoiceParameters);
        checkChoice(PluginDescriptors::PatchEditSection::FmTrackModule::kChoiceParameters);
        checkChoice(PluginDescriptors::PatchEditSection::RampPortamentoModule::kChoiceParameters);
        checkChoice(PluginDescriptors::PatchEditSection::Envelope1Module::kChoiceParameters);
        checkChoice(PluginDescriptors::PatchEditSection::Envelope2Module::kChoiceParameters);
        checkChoice(PluginDescriptors::PatchEditSection::Envelope3Module::kChoiceParameters);
        checkChoice(PluginDescriptors::PatchEditSection::Lfo1Module::kChoiceParameters);
        checkChoice(PluginDescriptors::PatchEditSection::Lfo2Module::kChoiceParameters);
        checkChoice(PluginDescriptors::MasterEditSection::kChoiceParameters);

        for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
            checkChoice(PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[static_cast<size_t>(bus)]);

        if (foundInt && foundChoice)
            throw InvalidParameterException(parameterId, "Parameter ID exists in both int and choice descriptor collections");

        if (foundInt)
            return ParameterWidgetKind::Slider;

        if (foundChoice)
            return ParameterWidgetKind::ComboBox;

        return std::nullopt;
    }
}
