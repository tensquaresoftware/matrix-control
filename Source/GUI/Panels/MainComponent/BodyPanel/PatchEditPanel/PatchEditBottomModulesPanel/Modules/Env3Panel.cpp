#include "Env3Panel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout Env3Panel::createLayout()
{
    return makePatchEditModuleLayout(
        PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
        PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kInit,
        PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kCopy,
        PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kPaste,
        {
            PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDelay,
            PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAttack,
            PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDecay,
            PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kSustain,
            PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kRelease,
            PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitude,
            PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitudeModByVelocity,
            PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kTriggerMode,
            PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kEnvelopeMode,
            PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kLfo1Trigger
        });
}

Env3Panel::Env3Panel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
                         const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims)
    : BaseModulePanel(skin, widgetFactory, apvts, createLayout(), width, height, moduleHeaderDims, parameterCellDims)
{
}
