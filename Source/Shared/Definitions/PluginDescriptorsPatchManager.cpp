// This file defines Patch Manager Section descriptor data: standalone widgets and int parameters
// for Bank Utility, Internal Patches, Computer Patches, and Patch Mutator modules.
// Extracted from PluginDescriptors for modularity.

#include "PluginDescriptors.h"

#include "PluginDisplayNames.h"
#include "PluginHelpers.h"
#include "PluginIDs.h"


namespace PluginDescriptors
{
    // =================================================================================================================
    // Patch Manager Section | Bank Utility Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchManagerSection::BankUtilityModule::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kLockBank,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kLockBank,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank0,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank0,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank1,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank1,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank2,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank2,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank3,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank3,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank4,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank4,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank5,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank5,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank6,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank6,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank7,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank7,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank8,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank8,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank9,
            .displayName = PluginDisplayNames::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank9,
            .parentGroupId = PluginIDs::PatchManagerSection::BankUtilityModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };


    // =================================================================================================================
    // Patch Manager Section | Internal Patches Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchManagerSection::InternalPatchesModule::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadNextPatch,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadNextPatch,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kNumber
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kNumber
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kInitPatch,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kInitPatch,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCopyPatch,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCopyPatch,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch,
            .displayName = PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch,
            .parentGroupId = PluginIDs::PatchManagerSection::InternalPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };


    // =================================================================================================================
    // Patch Manager Section | Computer Patches Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchManagerSection::ComputerPatchesModule::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile,
            .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile,
            .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile,
            .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile,
            .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kComboBox
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kOpenPatchFolder,
            .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kOpenPatchFolder,
            .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchAs,
            .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchAsFile,
            .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchFile,
            .displayName = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchFile,
            .parentGroupId = PluginIDs::PatchManagerSection::ComputerPatchesModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };


    // =================================================================================================================
    // Patch Manager Section | Patch Mutator Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchManagerSection::PatchMutatorModule::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kLabel
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kLabel
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistory,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistory,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kComboBox
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kMutate,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kMutate,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableDco1,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableDco1,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableDco2,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableDco2,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableVcfVca,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableVcfVca,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableFmTrack,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableFmTrack,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableRampPortamento,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableRampPortamento,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope1,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope1,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope2,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope2,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope3,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope3,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableLfo1,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableLfo1,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableLfo2,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableLfo2,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };

    // Int Parameters ----------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> PatchManagerSection::PatchMutatorModule::kIntParameters = {
        {
            .parameterId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .minValue = 0,
            .maxValue = 100,
            .defaultValue = 0,
            .sysExOffset = 0,
            .sysExId = kNoSysExId
        },
        {
            .parameterId = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom,
            .displayName = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom,
            .parentGroupId = PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId,
            .minValue = 0,
            .maxValue = 100,
            .defaultValue = 0,
            .sysExOffset = 0,
            .sysExId = kNoSysExId
        }
    };
}
