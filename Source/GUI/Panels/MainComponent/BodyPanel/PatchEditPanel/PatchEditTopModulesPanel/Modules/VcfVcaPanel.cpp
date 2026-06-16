#include "VcfVcaPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout VcfVcaPanel::createLayout()
{
    return makePatchEditInitOnlyModuleLayout(
        PluginIDs::PatchEditSection::VcfVcaModule::kGroupId,
        PluginIDs::PatchEditSection::VcfVcaModule::StandaloneWidgets::kInit,
        {
            PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kBalance,
            PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequency,
            PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequencyModByEnv1,
            PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequencyModByPressure,
            PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kResonance,
            PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca1Volume,
            PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca1ModByVelocity,
            PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca2ModByEnv2,
            PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kLevers,
            PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kKeyboardPortamento
        });
}

VcfVcaPanel::VcfVcaPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
                         const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims)
    : BaseModulePanel(skin, widgetFactory, apvts, createLayout(), width, height, moduleHeaderDims, parameterCellDims)
{
}
