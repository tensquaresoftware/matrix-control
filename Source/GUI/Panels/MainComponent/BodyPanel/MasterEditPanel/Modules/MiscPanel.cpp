#include "MiscPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout MiscPanel::createLayout()
{
    return makeMasterEditModuleLayout(
        PluginIDs::MasterEditSection::MiscModule::kGroupId,
        PluginIDs::MasterEditSection::MiscModule::StandaloneWidgets::kInit,
        {
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMasterTune,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMasterTranspose,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kBendRange,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kVolumeInvertEnable,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kBankLockEnable,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMemoryProtectEnable,
            ""
        });
}

MiscPanel::MiscPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
                         const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims)
    : BaseModulePanel(skin, widgetFactory, apvts, createLayout(), width, height, moduleHeaderDims, parameterCellDims)
{
}
