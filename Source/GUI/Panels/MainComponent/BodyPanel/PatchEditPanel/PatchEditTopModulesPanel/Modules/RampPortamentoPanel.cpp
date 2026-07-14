#include "RampPortamentoPanel.h"

#include "GUI/Helpers/GrayedControlHelper.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/ParameterCell.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"


ModulePanelLayout RampPortamentoPanel::createLayout()
{
    return makePatchEditInitOnlyModuleLayout(
        PluginIDs::PatchEditSection::RampPortamentoModule::kGroupId,
        PluginIDs::PatchEditSection::RampPortamentoModule::StandaloneWidgets::kInit,
        {
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Rate,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Trigger,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Rate,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Trigger,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoRate,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoModByVelocity,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoMode,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoLegato,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode,
            ""
        });
}

RampPortamentoPanel::RampPortamentoPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
                         const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims)
    : BaseModulePanel(skin, widgetFactory, apvts, createLayout(), width, height, moduleHeaderDims, parameterCellDims)
{
    apvts.addParameterListener(
        PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode,
        this);
    refreshLegatoPortaGraying();
}

RampPortamentoPanel::~RampPortamentoPanel()
{
    apvts_.removeParameterListener(
        PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode,
        this);
}

void RampPortamentoPanel::parameterChanged(const juce::String& parameterID, float)
{
    if (parameterID == PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode)
        refreshLegatoPortaGraying();
}

void RampPortamentoPanel::refreshLegatoPortaGraying()
{
    const auto* keyboardModeParam = apvts_.getParameter(
        PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode);
    const auto* choiceParam = dynamic_cast<const juce::AudioParameterChoice*>(keyboardModeParam);

    legatoPortaGrayed_ = choiceParam != nullptr && choiceParam->getIndex() == kUnisonKeyboardModeIndex;

    if (auto* legatoCell = getParameterCellAt(7))
    {
        if (auto* combo = legatoCell->getComboBox())
            combo->setEnabled(true);

        TSS::GrayedControlHelper::applyGrayedAppearance(*legatoCell, legatoPortaGrayed_);

        if (legatoPortaGrayed_)
        {
            TSS::GrayedControlHelper::setGrayedClickHandler(*legatoCell, true, [this]
            {
                TSS::GrayedControlHelper::setFooterInfoMessage(
                    apvts_,
                    PluginDisplayNames::PatchEditSection::RampPortamentoModule::kLegatoPortaUnisonBlockedFooter);
            });
        }
        else
        {
            TSS::GrayedControlHelper::clearGrayedClickHandler(*legatoCell);
        }
    }
}
