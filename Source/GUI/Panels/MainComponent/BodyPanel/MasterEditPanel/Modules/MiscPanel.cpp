#include "MiscPanel.h"

#include "GUI/Themes/Skin.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "Shared/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelConfig MiscPanel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginDescriptors::ModuleIds::kMisc;
    config.buttonSet = ModulePanelButtonSet::InitOnly;
    config.moduleType = ModulePanelModuleType::MasterEdit;
    config.initWidgetId = PluginDescriptors::StandaloneWidgetIds::kMiscInit;
    
    config.parameters = {
        {PluginDescriptors::ParameterIds::kMasterTune, ModulePanelParameterType::Slider},
        {PluginDescriptors::ParameterIds::kMasterTranspose, ModulePanelParameterType::Slider},
        {PluginDescriptors::ParameterIds::kBendRange, ModulePanelParameterType::Slider},
        {PluginDescriptors::ParameterIds::kUnisonEnable, ModulePanelParameterType::ComboBox},
        {PluginDescriptors::ParameterIds::kVolumeInvertEnable, ModulePanelParameterType::ComboBox},
        {PluginDescriptors::ParameterIds::kBankLockEnable, ModulePanelParameterType::ComboBox},
        {PluginDescriptors::ParameterIds::kMemoryProtectEnable, ModulePanelParameterType::ComboBox},
        {"", ModulePanelParameterType::None}
    };
    
    return config;
}

MiscPanel::MiscPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : BaseModulePanel(skin, widgetFactory, apvts, createConfig(), getWidth(), getHeight())
{
}
