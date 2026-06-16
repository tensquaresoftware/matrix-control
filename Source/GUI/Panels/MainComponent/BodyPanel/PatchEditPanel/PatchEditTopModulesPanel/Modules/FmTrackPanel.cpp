#include "FmTrackPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout FmTrackPanel::createLayout()
{
    return makePatchEditInitOnlyModuleLayout(
        PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
        PluginIDs::PatchEditSection::FmTrackModule::StandaloneWidgets::kInit,
        {
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmAmount,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmModByEnv3,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmModByPressure,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint1,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint2,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint4,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint5,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackInput,
            ""
        });
}

FmTrackPanel::FmTrackPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
                         const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims)
    : BaseModulePanel(skin, widgetFactory, apvts, createLayout(), width, height, moduleHeaderDims, parameterCellDims)
{
}
