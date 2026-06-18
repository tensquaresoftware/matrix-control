#include "Dco2Panel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout Dco2Panel::createLayout()
{
    return makePatchEditModuleLayout(
        PluginIDs::PatchEditSection::Dco2Module::kGroupId,
        PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kInit,
        PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kCopy,
        PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kPaste,
        PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kPasteEnabled,
        {
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kFrequency,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kFrequencyModByLfo1,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kDetune,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kPulseWidth,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kPulseWidthModByLfo2,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kWaveShape,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kWaveSelect,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kLevers,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kKeyboardPortamento,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kKeyClick
        });
}

Dco2Panel::Dco2Panel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
                         const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims)
    : BaseModulePanel(skin, widgetFactory, apvts, createLayout(), width, height, moduleHeaderDims, parameterCellDims)
{
}
