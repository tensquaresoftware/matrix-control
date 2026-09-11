#include "Env3Panel.h"

#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "GUI/Widgets/ParameterCell.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginIDs.h"

ModulePanelLayout Env3Panel::createLayout()
{
    return makePatchEditModuleLayout(
        PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
        {
            .initWidgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kInit,
            .copyWidgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kCopy,
            .pasteWidgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kPaste,
            .pasteEnabledPropertyId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kPasteEnabled
        },
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

Env3Panel::Env3Panel(const Config& config)
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

void Env3Panel::refreshStrigGate(bool clearIfCurrentStrig)
{
    auto* cell = getParameterCellAt(static_cast<size_t>(kTriggerModeCellIndex));
    TSS::StrigUnisonGateHelper::refreshTriggerParameter({
        .apvts = apvts_,
        .combo = cell != nullptr ? cell->getComboBox() : nullptr,
        .triggerParameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kTriggerMode,
        .strigChoiceIndex = kTriggerStrigIndex,
        .clearIfCurrentStrig = clearIfCurrentStrig});
}
