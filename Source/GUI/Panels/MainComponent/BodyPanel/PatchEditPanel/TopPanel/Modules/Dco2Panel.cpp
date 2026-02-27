#include "Dco2Panel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "Shared/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelConfig Dco2Panel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginIDs::PatchEditSection::Dco2Module::kGroupId;
    config.buttonSet = ModulePanelButtonSet::InitCopyPaste;
    config.moduleType = ModulePanelModuleType::PatchEdit;
    config.initWidgetId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kInit;
    config.copyWidgetId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kCopy;
    config.pasteWidgetId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kPaste;
    
    config.parameters = {
        {PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kFrequency, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kFrequencyModByLfo1, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kDetune, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kPulseWidth, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kPulseWidthModByLfo2, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kWaveShape, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kWaveSelect, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kLevers, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kKeyboardPortamento, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kKeyClick, ModulePanelParameterType::ComboBox}
    };
    
    return config;
}

Dco2Panel::Dco2Panel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : BaseModulePanel(skin, widgetFactory, apvts, createConfig(), getWidth(), getHeight())
{
}
