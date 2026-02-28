#include "RampPortamentoPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "Shared/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelConfig RampPortamentoPanel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId;
    config.buttonSet = ModulePanelButtonSet::InitOnly;
    config.moduleType = ModulePanelModuleType::PatchEdit;
    config.initWidgetId = PluginIDs::PatchEditSection::RampPortamentoModule::StandaloneWidgets::kInit;
    
    config.parameters = {
        {PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Rate, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Trigger, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Rate, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Trigger, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoRate, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoModByVelocity, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoMode, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoLegato, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode, ModulePanelParameterType::ComboBox},
        {"", ModulePanelParameterType::None}
    };
    
    return config;
}

RampPortamentoPanel::RampPortamentoPanel(tss::Skin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : BaseModulePanel(skin, widgetFactory, apvts, createConfig(), width, height)
{
}
