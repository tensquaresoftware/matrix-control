#include "Env2Panel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelConfig Env2Panel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId;
    config.buttonSet = ModulePanelButtonSet::InitCopyPaste;
    config.moduleType = ModulePanelModuleType::PatchEdit;
    config.initWidgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kInit;
    config.copyWidgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kCopy;
    config.pasteWidgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kPaste;
    
    config.parameters = {
        {PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDelay, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAttack, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDecay, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kSustain, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kRelease, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitude, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitudeModByVelocity, ModulePanelParameterType::Slider},
        {PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kTriggerMode, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kEnvelopeMode, ModulePanelParameterType::ComboBox},
        {PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kLfo1Trigger, ModulePanelParameterType::ComboBox}
    };
    
    return config;
}

Env2Panel::Env2Panel(tss::Skin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : BaseModulePanel(skin, widgetFactory, apvts, createConfig(), width, height)
{
}
