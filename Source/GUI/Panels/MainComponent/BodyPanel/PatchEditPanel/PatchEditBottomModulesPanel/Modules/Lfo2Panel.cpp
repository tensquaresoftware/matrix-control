#include "Lfo2Panel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelConfig Lfo2Panel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginIDs::PatchEditSection::Lfo2Module::kGroupId;
    config.buttonSet = ModulePanelButtonSet::InitCopyPaste;
    config.moduleType = ModulePanelModuleType::PatchEdit;
    config.initWidgetId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kInit;
    config.copyWidgetId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kCopy;
    config.pasteWidgetId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kPaste;
    
    config.parameters = {
        {PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kSpeed, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kSpeedModByKeyboard, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kRetriggerPoint, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kAmplitude, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kAmplitudeModByRamp2, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kWaveform, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kTriggerMode, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kLag, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kSampleInput, ModulePanelParameterType::ComboBox},
        {"", ModulePanelParameterType::None}
    };
    
    return config;
}

Lfo2Panel::Lfo2Panel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : BaseModulePanel(skin, widgetFactory, apvts, createConfig(), width, height)
{
}
