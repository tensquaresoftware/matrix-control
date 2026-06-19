#include "Core/Actions/ActionPropertyRegistry.h"

#include <unordered_map>

#include "Shared/Definitions/PluginIDs.h"

namespace Core
{
    namespace
    {
        struct RegistryEntry
        {
            const char* propertyId;
            ActionHandlerKind handlerKind;
        };

        // Event-only APVTS properties (timestamp actions). State, uiMirror, and parameter IDs are excluded.
        constexpr RegistryEntry kRegistryEntries[] = {
            // MasterEditSection — ModuleHeader init buttons
            { PluginIDs::MasterEditSection::MidiModule::StandaloneWidgets::kInit, ActionHandlerKind::Module },
            { PluginIDs::MasterEditSection::VibratoModule::StandaloneWidgets::kInit, ActionHandlerKind::Module },
            { PluginIDs::MasterEditSection::MiscModule::StandaloneWidgets::kInit, ActionHandlerKind::Module },

            // PatchEditSection — module I/C/P (ModuleHeader)
            { PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kInit, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kCopy, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kPaste, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kInit, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kCopy, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kPaste, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::VcfVcaModule::StandaloneWidgets::kInit, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::FmTrackModule::StandaloneWidgets::kInit, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::RampPortamentoModule::StandaloneWidgets::kInit, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kInit, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kCopy, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kPaste, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kInit, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kCopy, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kPaste, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kInit, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kCopy, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kPaste, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kInit, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kCopy, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kPaste, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kInit, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kCopy, ActionHandlerKind::Module },
            { PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kPaste, ActionHandlerKind::Module },

            // MatrixModulationSection — section I/C/P + per-bus init (ModulationBusCell)
            { PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationInit, ActionHandlerKind::Module },
            { PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationCopy, ActionHandlerKind::Module },
            { PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationPaste, ActionHandlerKind::Module },
            { PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus0Init, ActionHandlerKind::Module },
            { PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus1Init, ActionHandlerKind::Module },
            { PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus2Init, ActionHandlerKind::Module },
            { PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus3Init, ActionHandlerKind::Module },
            { PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus4Init, ActionHandlerKind::Module },
            { PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus5Init, ActionHandlerKind::Module },
            { PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus6Init, ActionHandlerKind::Module },
            { PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus7Init, ActionHandlerKind::Module },
            { PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus8Init, ActionHandlerKind::Module },
            { PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus9Init, ActionHandlerKind::Module },

            // BankUtilityModule
            { PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kUnlockBank, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank0, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank1, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank2, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank3, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank4, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank5, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank6, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank7, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank8, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank9, ActionHandlerKind::PatchManager },

            // InternalPatchesModule
            { PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadNextPatch, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kInitPatch, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCopyPatch, ActionHandlerKind::Module },
            { PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch, ActionHandlerKind::PatchManager },

            // ComputerPatchesModule — stubs until Story 7.3 / Epic 4
            { PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kOpenPatchFolder, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchAs, ActionHandlerKind::PatchManager },
            { PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchFile, ActionHandlerKind::PatchManager },

            // PatchMutatorModule — event buttons only (Amount/Random/History/enables are state or parameters)
            { PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kMutate, ActionHandlerKind::Mutator },
            { PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry, ActionHandlerKind::Mutator },
            { PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare, ActionHandlerKind::Mutator },
            { PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete, ActionHandlerKind::Mutator },
            { PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear, ActionHandlerKind::Mutator },
            { PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport, ActionHandlerKind::Mutator },
        };

        const std::unordered_map<juce::String, ActionHandlerKind>& registryMap()
        {
            static const std::unordered_map<juce::String, ActionHandlerKind> map = []() {
                std::unordered_map<juce::String, ActionHandlerKind> entries;
                entries.reserve(std::size(kRegistryEntries));

                for (const auto& entry : kRegistryEntries)
                    entries.emplace(entry.propertyId, entry.handlerKind);

                return entries;
            }();

            return map;
        }
    } // namespace

    bool ActionPropertyRegistry::isActionProperty(const juce::String& propertyId)
    {
        return handlerKindFor(propertyId).has_value();
    }

    std::optional<ActionHandlerKind> ActionPropertyRegistry::handlerKindFor(const juce::String& propertyId)
    {
        const auto& map = registryMap();
        const auto it = map.find(propertyId);

        if (it == map.end())
            return std::nullopt;

        return it->second;
    }

} // namespace Core
