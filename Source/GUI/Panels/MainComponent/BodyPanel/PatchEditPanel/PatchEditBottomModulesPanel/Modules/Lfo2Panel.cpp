#include "Lfo2Panel.h"

#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "GUI/Widgets/ParameterCell.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginIDs.h"

ModulePanelLayout Lfo2Panel::createLayout()
{
    return makePatchEditModuleLayout(
        PluginIDs::PatchEditSection::Lfo2Module::kGroupId,
        {
            .initWidgetId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kInit,
            .copyWidgetId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kCopy,
            .pasteWidgetId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kPaste,
            .pasteEnabledPropertyId = PluginIDs::PatchEditSection::Lfo2Module::StandaloneWidgets::kPasteEnabled
        },
        {
            PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kSpeed,
            PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kSpeedModByKeyboard,
            PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kRetriggerPoint,
            PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kAmplitude,
            PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kAmplitudeModByRamp2,
            PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kWaveform,
            PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kTriggerMode,
            PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kLag,
            PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kSampleInput,
            ""
        });
}

Lfo2Panel::Lfo2Panel(const Config& config)
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

void Lfo2Panel::refreshStrigGate(bool clearIfCurrentStrig)
{
    auto* cell = getParameterCellAt(static_cast<size_t>(kTriggerModeCellIndex));
    TSS::StrigUnisonGateHelper::refreshTriggerParameter({
        .apvts = apvts_,
        .combo = cell != nullptr ? cell->getComboBox() : nullptr,
        .triggerParameterId = PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kTriggerMode,
        .strigChoiceIndex = kTriggerStrigIndex,
        .clearIfCurrentStrig = clearIfCurrentStrig});
}
