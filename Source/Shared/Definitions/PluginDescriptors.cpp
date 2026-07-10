// This file defines the APVTS group hierarchy. Per-section descriptor data lives in
// the other PluginDescriptors*.cpp translation units.

#include "PluginDescriptors.h"

#include "PluginDisplayNames.h"
#include "PluginIDs.h"

namespace PluginDescriptors
{
    // =================================================================================================================
    // APVTS Groups
    // =================================================================================================================

    const std::vector<ApvtsGroupDescriptor> kAllApvtsGroups =
    {
        // Plugin Modes ------------------------------------------------------------------------------------------------
        {
            .parentId = kNoParentId,
            .groupId = PluginIDs::Mode::kMaster,
            .displayName = PluginDisplayNames::Mode::kMaster
        },
        {
            .parentId = kNoParentId,
            .groupId = PluginIDs::Mode::kPatch,
            .displayName = PluginDisplayNames::Mode::kPatch
        },
        // Plugin Sections ---------------------------------------------------------------------------------------------
        {
            .parentId = PluginIDs::Mode::kMaster,
            .groupId = PluginIDs::MasterEditSection::kGroupId,
            .displayName = PluginDisplayNames::MasterEditSection::kName
        },
        {
            .parentId = PluginIDs::Mode::kPatch,
            .groupId = PluginIDs::PatchEditSection::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::kName
        },
        {
            .parentId = PluginIDs::Mode::kPatch,
            .groupId = PluginIDs::MatrixModulationSection::kGroupId,
            .displayName = PluginDisplayNames::MatrixModulationSection::kName
        },
        {
            .parentId = PluginIDs::Mode::kPatch,
            .groupId = PluginIDs::PatchManagerSection::kGroupId,
            .displayName = PluginDisplayNames::PatchManagerSection::kName
        },
        // Master Edit Section -----------------------------------------------------------------------------------------
        {
            .parentId = PluginIDs::MasterEditSection::kGroupId,
            .groupId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
            .displayName = PluginDisplayNames::MasterEditSection::MidiModule::kName
        },
        {
            .parentId = PluginIDs::MasterEditSection::kGroupId,
            .groupId = PluginIDs::MasterEditSection::VibratoModule::kGroupId,
            .displayName = PluginDisplayNames::MasterEditSection::VibratoModule::kName
        },
        {
            .parentId = PluginIDs::MasterEditSection::kGroupId,
            .groupId = PluginIDs::MasterEditSection::MiscModule::kGroupId,
            .displayName = PluginDisplayNames::MasterEditSection::MiscModule::kName
        },
        // Patch Edit Section ------------------------------------------------------------------------------------------
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::Dco1Module::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::Dco2Module::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::Dco2Module::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::VcfVcaModule::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::FmTrackModule::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::RampPortamentoModule::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo1Module::kName
        },
        {
            .parentId = PluginIDs::PatchEditSection::kGroupId,
            .groupId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
            .displayName = PluginDisplayNames::PatchEditSection::Lfo2Module::kName
        },
        // Matrix Modulation Section -----------------------------------------------------------------------------------
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus0
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus1
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus2
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus3
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus4
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus5
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus6
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus7
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus8
        },
        {
            .parentId = PluginIDs::MatrixModulationSection::kGroupId,
            .groupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus9,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::kBus9
        },
        // Patch Manager Section ---------------------------------------------------------------------------------------
        {
            .parentId = PluginIDs::PatchManagerSection::kGroupId,
            .groupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::kName
        },
        {
            .parentId = PluginIDs::PatchManagerSection::kGroupId,
            .groupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::kName
        },
        {
            .parentId = PluginIDs::PatchManagerSection::kGroupId,
            .groupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
            .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::kName
        },
        {
            .parentId = PluginIDs::PatchManagerSection::kGroupId,
            .groupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::kName
        }
    };

}
