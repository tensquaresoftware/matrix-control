#include "Lfo1Panel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelConfig Lfo1Panel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginIDs::PatchEditSection::Lfo1Module::kGroupId;
    config.buttonSet = ModulePanelButtonSet::InitCopyPaste;
    config.moduleType = ModulePanelModuleType::PatchEdit;
    config.initWidgetId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kInit;
    config.copyWidgetId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kCopy;
    config.pasteWidgetId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kPaste;
    
    config.parameters = {
        {PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kSpeed, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kSpeedModByPressure, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kRetriggerPoint, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kAmplitude, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kAmplitudeModByRamp1, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kWaveform, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kTriggerMode, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kLag, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kSampleInput, ModulePanelParameterType::ComboBox},
        {"", ModulePanelParameterType::None}
    };
    
    return config;
}

Lfo1Panel::Lfo1Panel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : BaseModulePanel(skin, widgetFactory, apvts, createConfig(), width, height)
{
}
