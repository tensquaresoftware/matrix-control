#include "Env1Panel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelConfig Env1Panel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId;
    config.buttonSet = ModulePanelButtonSet::InitCopyPaste;
    config.moduleType = ModulePanelModuleType::PatchEdit;
    config.initWidgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kInit;
    config.copyWidgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kCopy;
    config.pasteWidgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kPaste;
    
    config.parameters = {
        {PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDelay, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDecay, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kSustain, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kRelease, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitude, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitudeModByVelocity, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kTriggerMode, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kEnvelopeMode, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kLfo1Trigger, ModulePanelParameterType::ComboBox}
    };
    
    return config;
}

Env1Panel::Env1Panel(tss::Skin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : BaseModulePanel(skin, widgetFactory, apvts, createConfig(), width, height)
{
}
