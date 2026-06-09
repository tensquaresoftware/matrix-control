#include "MidiPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelConfig MidiPanel::createConfig()
{
    ModulePanelConfig config;
    config.moduleId = PluginIDs::MasterEditSection::MidiModule::kGroupId;
    config.buttonSet = ModulePanelButtonSet::InitOnly;
    config.moduleType = ModulePanelModuleType::MasterEdit;
    config.initWidgetId = PluginIDs::MasterEditSection::MidiModule::StandaloneWidgets::kInit;
    
    config.parameters = {
        {PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel, ModulePanelParameterType::ComboBox},
        {PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kMidiEcho, ModulePanelParameterType::ComboBox},
        {PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kControllers, ModulePanelParameterType::ComboBox},
        {PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPatchChanges, ModulePanelParameterType::ComboBox},
        {PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPedal1Select, ModulePanelParameterType::Slider},
        {PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPedal2Select, ModulePanelParameterType::Slider},
        {PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kLever2Select, ModulePanelParameterType::Slider},
        {PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kLever3Select, ModulePanelParameterType::Slider}
    };
    
    return config;
}

MidiPanel::MidiPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
                         const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims)
    : BaseModulePanel(skin, widgetFactory, apvts, createConfig(), width, height, moduleHeaderDims, parameterCellDims)
{
}
