#include "Env2Panel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout Env2Panel::createLayout()
{
    return makePatchEditModuleLayout(
        PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
        PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kInit,
        PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kCopy,
        PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kPaste,
        {
            PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDelay,
            PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAttack,
            PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDecay,
            PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kSustain,
            PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kRelease,
            PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitude,
            PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitudeModByVelocity,
            PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kTriggerMode,
            PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kEnvelopeMode,
            PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kLfo1Trigger
        });
}

Env2Panel::Env2Panel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
                         const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims)
    : BaseModulePanel(skin, widgetFactory, apvts, createLayout(), width, height, moduleHeaderDims, parameterCellDims)
{
}
