#include "MidiPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout MidiPanel::createLayout()
{
    return makeMasterEditModuleLayout(
        PluginIDs::MasterEditSection::MidiModule::kGroupId,
        PluginIDs::MasterEditSection::MidiModule::StandaloneWidgets::kInit,
        {
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kMidiEcho,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kControllers,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPatchChanges,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPedal1Select,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPedal2Select,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kLever2Select,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kLever3Select
        });
}

MidiPanel::MidiPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
                         const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims)
    : BaseModulePanel(skin, widgetFactory, apvts, createLayout(), width, height, moduleHeaderDims, parameterCellDims)
{
}
