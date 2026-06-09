#include "VibratoPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelConfig VibratoPanel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginIDs::MasterEditSection::VibratoModule::kGroupId;
    config.buttonSet = ModulePanelButtonSet::InitOnly;
    config.moduleType = ModulePanelModuleType::MasterEdit;
    config.initWidgetId = PluginIDs::MasterEditSection::VibratoModule::StandaloneWidgets::kInit;
    
    config.parameters = {
        {PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeed, ModulePanelParameterType::Slider},
        {PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kWaveform, ModulePanelParameterType::ComboBox},
        {PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmplitude, ModulePanelParameterType::Slider},
        {PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModSource, ModulePanelParameterType::ComboBox},
        {PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModAmount, ModulePanelParameterType::Slider},
        {PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmpModSource, ModulePanelParameterType::ComboBox},
        {PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmpModAmount, ModulePanelParameterType::Slider}
    };
    
    return config;
}

VibratoPanel::VibratoPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
                         const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims)
    : BaseModulePanel(skin, widgetFactory, apvts, createConfig(), width, height, moduleHeaderDims, parameterCellDims)
{
}
