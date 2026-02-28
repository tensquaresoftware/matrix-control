#include "VcfVcaPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelConfig VcfVcaPanel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginIDs::PatchEditSection::VcfVcaModule::kGroupId;
    config.buttonSet = ModulePanelButtonSet::InitOnly;
    config.moduleType = ModulePanelModuleType::PatchEdit;
    config.initWidgetId = PluginIDs::PatchEditSection::VcfVcaModule::StandaloneWidgets::kInit;
    
    config.parameters = {
        {PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kBalance, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequency, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequencyModByEnv1, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequencyModByPressure, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kResonance, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca1Volume, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca1ModByVelocity, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kVca2ModByEnv2, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kLevers, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kKeyboardPortamento, ModulePanelParameterType::ComboBox}
    };
    
    return config;
}

VcfVcaPanel::VcfVcaPanel(tss::Skin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : BaseModulePanel(skin, widgetFactory, apvts, createConfig(), width, height)
{
}
