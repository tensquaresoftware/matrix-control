#include "Env2Panel.h"

#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "GUI/Widgets/ParameterCell.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginIDs.h"

ModulePanelLayout Env2Panel::createLayout()
{
    return makePatchEditModuleLayout(
        PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
        {
            .initWidgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kInit,
            .copyWidgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kCopy,
            .pasteWidgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kPaste,
            .pasteEnabledPropertyId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kPasteEnabled
        },
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

Env2Panel::Env2Panel(const Config& config)
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

void Env2Panel::refreshStrigGate(bool clearIfCurrentStrig)
{
    auto* cell = getParameterCellAt(static_cast<size_t>(kTriggerModeCellIndex));
    TSS::StrigUnisonGateHelper::refreshTriggerParameter({
        .apvts = apvts_,
        .combo = cell != nullptr ? cell->getComboBox() : nullptr,
        .triggerParameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kTriggerMode,
        .strigChoiceIndex = kTriggerStrigIndex,
        .clearIfCurrentStrig = clearIfCurrentStrig});
}
