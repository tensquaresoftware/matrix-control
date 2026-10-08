// Appearance + Getting Started Settings wiring (extracted from PluginEditorSettings.cpp).

#include "PluginEditor.h"

#include "Core/Services/GettingStartedMachineDefaults.h"
#include "GUI/Settings/SettingsPanel.h"
#include "Shared/Definitions/PluginIDs.h"

void PluginEditor::restoreSettingsAppearanceFromState(SettingsPanel& panel)
{
    using namespace PluginIDs::Settings;

    const int scaleId = pluginProcessor.getGuiScaleId();
    const int normalizedScale = (scaleId >= ScaleLevels::kMin && scaleId <= ScaleLevels::kMax)
        ? scaleId
        : ScaleLevels::kDefault;

    const int skinId = pluginProcessor.getSkinVariantId();
    const int normalizedSkin = (skinId == SkinVariants::kBlack || skinId == SkinVariants::kCream)
        ? skinId
        : SkinVariants::kDefault;

    panel.getUiScaleCombo().setSelectedId(normalizedScale, juce::dontSendNotification);
    panel.getSkinCombo().setSelectedId(normalizedSkin, juce::dontSendNotification);
    panel.getGettingStartedAutoOpenCombo().setSelectedId(
        Core::GettingStartedMachineDefaults::loadAutoOpenPreference(),
        juce::dontSendNotification);
}

void PluginEditor::wireSettingsAppearanceCombos(SettingsPanel& panel)
{
    panel.getUiScaleCombo().onChange = [this, &panel]
    {
        using namespace PluginIDs::Settings::ScaleLevels;
        const int selectedId = panel.getUiScaleCombo().getSelectedId();
        if (selectedId < kMin || selectedId > kMax)
            return;

        applyUiScaleFromItemId(selectedId, true);
    };

    panel.getSkinCombo().onChange = [this, &panel]
    {
        using namespace PluginIDs::Settings::SkinVariants;
        const int selectedId = panel.getSkinCombo().getSelectedId();
        if (selectedId != kBlack && selectedId != kCream)
            return;

        applySkinFromItemId(selectedId, true);
    };
}

void PluginEditor::wireSettingsGettingStartedControls(SettingsPanel& panel)
{
    panel.getGettingStartedAutoOpenCombo().onChange = [&panel]
    {
        using namespace PluginIDs::Settings::GettingStartedAutoOpen;
        using namespace Core::GettingStartedMachineDefaults;

        const int selectedId = panel.getGettingStartedAutoOpenCombo().getSelectedId();
        if (selectedId != kShowWhenIncomplete && selectedId != kNeverAtLaunch)
            return;

        writeAutoOpenPreference(selectedId);
        if (clearsConfigureLaterSilenceOnAutoOpenPreference(selectedId))
            persistConfigureLaterArm(ConfigureLaterArm::kNormal, false);
    };

    panel.getRunSetupAgainButton().onClick = [this]
    {
        const bool isPluginMode = ! pluginProcessor.isStandalone();
        GettingStartedWizard::runSetupAgain(
            [isPluginMode]
            {
                Core::GettingStartedMachineDefaults::resetForRunSetupAgain(isPluginMode);
            },
            [this](GettingStartedWizard::Step step) { openGettingStartedWizard(step); });
    };
}
