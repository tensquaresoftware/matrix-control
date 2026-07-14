#include "SessionPersistencePolicy.h"

#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/ApvtsTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core::SessionPersistencePolicy
{
    namespace
    {
        juce::StringArray buildPatchMasterAndMatrixModGroupIds()
        {
            juce::StringArray groupIds {
                PluginIDs::PatchEditSection::kGroupId,
                PluginIDs::MasterEditSection::kGroupId,
                PluginIDs::MatrixModulationSection::kGroupId
            };

            bool added = true;
            while (added)
            {
                added = false;

                for (const auto& group : PluginDescriptors::kAllApvtsGroups)
                {
                    if (groupIds.contains(group.parentId) && ! groupIds.contains(group.groupId))
                    {
                        groupIds.add(group.groupId);
                        added = true;
                    }
                }
            }

            return groupIds;
        }

        bool isPatchMasterOrMatrixModGroup(const juce::String& parentGroupId,
                                           const juce::StringArray& managedGroupIds)
        {
            return managedGroupIds.contains(parentGroupId);
        }

        juce::StringArray collectManagedParameterIds()
        {
            const auto managedGroupIds = buildPatchMasterAndMatrixModGroupIds();
            juce::StringArray parameterIds;

            auto collectFromInt = [&](const auto& parameters)
            {
                for (const auto& parameter : parameters)
                {
                    if (isPatchMasterOrMatrixModGroup(parameter.parentGroupId, managedGroupIds))
                        parameterIds.addIfNotAlreadyThere(parameter.parameterId);
                }
            };

            auto collectFromChoice = [&](const auto& parameters)
            {
                for (const auto& parameter : parameters)
                {
                    if (isPatchMasterOrMatrixModGroup(parameter.parentGroupId, managedGroupIds))
                        parameterIds.addIfNotAlreadyThere(parameter.parameterId);
                }
            };

            using namespace PluginDescriptors;

            collectFromInt(PatchEditSection::Dco1Module::kIntParameters);
            collectFromChoice(PatchEditSection::Dco1Module::kChoiceParameters);
            collectFromInt(PatchEditSection::Dco2Module::kIntParameters);
            collectFromChoice(PatchEditSection::Dco2Module::kChoiceParameters);
            collectFromInt(PatchEditSection::VcfVcaModule::kIntParameters);
            collectFromChoice(PatchEditSection::VcfVcaModule::kChoiceParameters);
            collectFromInt(PatchEditSection::FmTrackModule::kIntParameters);
            collectFromChoice(PatchEditSection::FmTrackModule::kChoiceParameters);
            collectFromInt(PatchEditSection::RampPortamentoModule::kIntParameters);
            collectFromChoice(PatchEditSection::RampPortamentoModule::kChoiceParameters);
            collectFromInt(PatchEditSection::Envelope1Module::kIntParameters);
            collectFromChoice(PatchEditSection::Envelope1Module::kChoiceParameters);
            collectFromInt(PatchEditSection::Envelope2Module::kIntParameters);
            collectFromChoice(PatchEditSection::Envelope2Module::kChoiceParameters);
            collectFromInt(PatchEditSection::Envelope3Module::kIntParameters);
            collectFromChoice(PatchEditSection::Envelope3Module::kChoiceParameters);
            collectFromInt(PatchEditSection::Lfo1Module::kIntParameters);
            collectFromChoice(PatchEditSection::Lfo1Module::kChoiceParameters);
            collectFromInt(PatchEditSection::Lfo2Module::kIntParameters);
            collectFromChoice(PatchEditSection::Lfo2Module::kChoiceParameters);

            collectFromInt(MasterEditSection::MidiModule::kIntParameters);
            collectFromChoice(MasterEditSection::MidiModule::kChoiceParameters);
            collectFromInt(MasterEditSection::VibratoModule::kIntParameters);
            collectFromChoice(MasterEditSection::VibratoModule::kChoiceParameters);
            collectFromInt(MasterEditSection::MiscModule::kIntParameters);
            collectFromChoice(MasterEditSection::MiscModule::kChoiceParameters);

            for (size_t bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
            {
                collectFromInt(MatrixModulationSection::kModulationBusIntParameters[bus]);
                collectFromChoice(MatrixModulationSection::kModulationBusChoiceParameters[bus]);
            }

            return parameterIds;
        }

        const juce::StringArray& managedParameterIds()
        {
            static const juce::StringArray ids = collectManagedParameterIds();
            return ids;
        }
    }

    bool shouldStripPatchAndMasterParameters(const juce::ValueTree& state)
    {
        const auto inputPortId = state.getProperty("midiInputPortId", juce::String()).toString().trim();
        const auto outputPortId = state.getProperty("midiOutputPortId", juce::String()).toString().trim();
        return inputPortId.isNotEmpty() && outputPortId.isNotEmpty();
    }

    void stripPatchAndMasterParameters(juce::ValueTree& state)
    {
        const auto& parameterIds = managedParameterIds();

        for (int i = state.getNumChildren(); --i >= 0;)
        {
            auto child = state.getChild(i);
            if (! child.hasType(juce::Identifier(ApvtsTypes::kParam)))
                continue;

            const auto paramId = child.getProperty("id", juce::String()).toString();
            if (parameterIds.contains(paramId))
                state.removeChild(i, nullptr);
        }
    }
}
