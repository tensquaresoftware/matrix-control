#include "MiscPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "Shared/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelConfig MiscPanel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginIDs::MasterEditSection::MiscModule::kGroupId;
    config.buttonSet = ModulePanelButtonSet::InitOnly;
    config.moduleType = ModulePanelModuleType::MasterEdit;
    config.initWidgetId = PluginIDs::MasterEditSection::MiscModule::StandaloneWidgets::kInit;
    
    config.parameters = {
        {PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMasterTune, ModulePanelParameterType::Slider},
        {PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMasterTranspose, ModulePanelParameterType::Slider},
        {PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kBendRange, ModulePanelParameterType::Slider},
        {PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable, ModulePanelParameterType::ComboBox},
        {PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kVolumeInvertEnable, ModulePanelParameterType::ComboBox},
        {PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kBankLockEnable, ModulePanelParameterType::ComboBox},
        {PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMemoryProtectEnable, ModulePanelParameterType::ComboBox},
        {"", ModulePanelParameterType::None}
    };
    
    return config;
}

MiscPanel::MiscPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : BaseModulePanel(skin, widgetFactory, apvts, createConfig(), getWidth(), getHeight())
{
}
