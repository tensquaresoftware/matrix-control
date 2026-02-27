#include "Env3Panel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "Shared/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelConfig Env3Panel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId;
    config.buttonSet = ModulePanelButtonSet::InitCopyPaste;
    config.moduleType = ModulePanelModuleType::PatchEdit;
    config.initWidgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kInit;
    config.copyWidgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kCopy;
    config.pasteWidgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kPaste;
    
    config.parameters = {
        {PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDelay, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAttack, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDecay, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kSustain, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kRelease, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitude, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitudeModByVelocity, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kTriggerMode, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kEnvelopeMode, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kLfo1Trigger, ModulePanelParameterType::ComboBox}
    };
    
    return config;
}

Env3Panel::Env3Panel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : BaseModulePanel(skin, widgetFactory, apvts, createConfig(), getWidth(), getHeight())
{
}
