#include "Dco1Panel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout Dco1Panel::createLayout()
{
    return makePatchEditModuleLayout(
        PluginIDs::PatchEditSection::Dco1Module::kGroupId,
        PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kInit,
        PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kCopy,
        PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kPaste,
        {
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequencyModByLfo1,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kSync,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kPulseWidth,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kPulseWidthModByLfo2,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kWaveShape,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kWaveSelect,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kLevers,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kKeyboardPortamento,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kKeyClick
        });
}

Dco1Panel::Dco1Panel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
                         const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims)
    : BaseModulePanel(skin, widgetFactory, apvts, createLayout(), width, height, moduleHeaderDims, parameterCellDims)
{
}
