#include "Lfo1Panel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout Lfo1Panel::createLayout()
{
    return makePatchEditModuleLayout(
        PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
        PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kInit,
        PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kCopy,
        PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kPaste,
        {
            PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kSpeed,
            PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kSpeedModByPressure,
            PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kRetriggerPoint,
            PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kAmplitude,
            PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kAmplitudeModByRamp1,
            PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kWaveform,
            PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kTriggerMode,
            PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kLag,
            PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kSampleInput,
            ""
        });
}

Lfo1Panel::Lfo1Panel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
                         const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims)
    : BaseModulePanel(skin, widgetFactory, apvts, createLayout(), width, height, moduleHeaderDims, parameterCellDims)
{
}
