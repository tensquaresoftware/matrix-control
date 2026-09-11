#include "Env1Panel.h"

#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "GUI/Widgets/ParameterCell.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginIDs.h"

ModulePanelLayout Env1Panel::createLayout()
{
    return makePatchEditModuleLayout(
        PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
        {
            .initWidgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kInit,
            .copyWidgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kCopy,
            .pasteWidgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kPaste,
            .pasteEnabledPropertyId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kPasteEnabled
        },
        {
            PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDelay,
            PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack,
            PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDecay,
            PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kSustain,
            PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kRelease,
            PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitude,
            PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitudeModByVelocity,
            PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kTriggerMode,
            PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kEnvelopeMode,
            PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kLfo1Trigger
        });
}

Env1Panel::Env1Panel(const Config& config)
    : BaseModulePanel(BaseModulePanel::Config{
          .skin = config.skin,
          .widgetFactory = config.widgetFactory,
          .apvts = config.apvts,
          .layout = createLayout(),
          .width = config.width,
          .height = config.height,
          .moduleHeaderDims = config.moduleHeaderDims,
          .parameterCellDims = config.parameterCellDims})
{
    keyboardModeListener_ = std::make_unique<TSS::StrigUnisonGateHelper::KeyboardModeChangeListener>(
        apvts_,
        [this](bool leftUnison) { refreshStrigGate(leftUnison); });
    refreshStrigGate(false);
}

void Env1Panel::refreshStrigGate(bool clearIfCurrentStrig)
{
    auto* cell = getParameterCellAt(static_cast<size_t>(kTriggerModeCellIndex));
    TSS::StrigUnisonGateHelper::refreshTriggerParameter({
        .apvts = apvts_,
        .combo = cell != nullptr ? cell->getComboBox() : nullptr,
        .triggerParameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kTriggerMode,
        .strigChoiceIndex = kTriggerStrigIndex,
        .clearIfCurrentStrig = clearIfCurrentStrig});
}
