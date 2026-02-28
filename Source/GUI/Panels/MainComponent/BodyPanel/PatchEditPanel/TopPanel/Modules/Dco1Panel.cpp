#include "Dco1Panel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "Shared/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelConfig Dco1Panel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginIDs::PatchEditSection::Dco1Module::kGroupId;
    config.buttonSet = ModulePanelButtonSet::InitCopyPaste;
    config.moduleType = ModulePanelModuleType::PatchEdit;
    config.initWidgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kInit;
    config.copyWidgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kCopy;
    config.pasteWidgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kPaste;
    
    config.parameters = {
        {PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequencyModByLfo1, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kSync, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kPulseWidth, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kPulseWidthModByLfo2, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kWaveShape, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kWaveSelect, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kLevers, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kKeyboardPortamento, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kKeyClick, ModulePanelParameterType::ComboBox}
    };
    
    return config;
}

Dco1Panel::Dco1Panel(tss::Skin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : BaseModulePanel(skin, widgetFactory, apvts, createConfig(), width, height)
{
}

