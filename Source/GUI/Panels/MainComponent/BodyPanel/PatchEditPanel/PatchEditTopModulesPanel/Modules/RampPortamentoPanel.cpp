#include "RampPortamentoPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout RampPortamentoPanel::createLayout()
{
    return makePatchEditInitOnlyModuleLayout(
        PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
        PluginIDs::PatchEditSection::RampPortamentoModule::StandaloneWidgets::kInit,
        {
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Rate,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Trigger,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Rate,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Trigger,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoRate,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoModByVelocity,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoMode,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoLegato,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode,
            ""
        });
}

RampPortamentoPanel::RampPortamentoPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
                         const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims)
    : BaseModulePanel(skin, widgetFactory, apvts, createLayout(), width, height, moduleHeaderDims, parameterCellDims)
{
}
