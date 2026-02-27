#include "FmTrackPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "Shared/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelConfig FmTrackPanel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginIDs::PatchEditSection::FmTrackModule::kGroupId;
    config.buttonSet = ModulePanelButtonSet::InitOnly;
    config.moduleType = ModulePanelModuleType::PatchEdit;
    config.initWidgetId = PluginIDs::PatchEditSection::FmTrackModule::StandaloneWidgets::kInit;
    
    config.parameters = {
        {PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmAmount, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmModByEnv3, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmModByPressure, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint1, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint2, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint4, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint5, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackInput, ModulePanelParameterType::ComboBox},
        {"", ModulePanelParameterType::None}
    };
    
    return config;
}

FmTrackPanel::FmTrackPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : BaseModulePanel(skin, widgetFactory, apvts, createConfig(), getWidth(), getHeight())
{
}
