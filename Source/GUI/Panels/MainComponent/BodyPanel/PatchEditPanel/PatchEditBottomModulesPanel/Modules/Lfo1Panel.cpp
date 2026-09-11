#include "Lfo1Panel.h"

#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "GUI/Widgets/ParameterCell.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginIDs.h"

ModulePanelLayout Lfo1Panel::createLayout()
{
    return makePatchEditModuleLayout(
        PluginIDs::PatchEditSection::Lfo1Module::kGroupId,
        {
            .initWidgetId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kInit,
            .copyWidgetId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kCopy,
            .pasteWidgetId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kPaste,
            .pasteEnabledPropertyId = PluginIDs::PatchEditSection::Lfo1Module::StandaloneWidgets::kPasteEnabled
        },
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

Lfo1Panel::Lfo1Panel(const Config& config)
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

void Lfo1Panel::refreshStrigGate(bool clearIfCurrentStrig)
{
    auto* cell = getParameterCellAt(static_cast<size_t>(kTriggerModeCellIndex));
    TSS::StrigUnisonGateHelper::refreshTriggerParameter({
        .apvts = apvts_,
        .combo = cell != nullptr ? cell->getComboBox() : nullptr,
        .triggerParameterId = PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kTriggerMode,
        .strigChoiceIndex = kTriggerStrigIndex,
        .clearIfCurrentStrig = clearIfCurrentStrig});
}
