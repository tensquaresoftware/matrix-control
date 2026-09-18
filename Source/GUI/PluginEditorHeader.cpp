// Extracted from PluginEditor.cpp for modular maintenance.
// Header panel skin/scale/settings/about control wiring and state restore.

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "Core/MIDI/MidiManager.h"
#include "GUI/About/AboutWindow.h"
#include "GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "GUI/Settings/SettingsWindow.h"
#include "Shared/Definitions/PluginIDs.h"

void PluginEditor::wireHeaderPanel(HeaderPanel& headerPanel)
{
    headerPanel.onSkinSelected = [this](int skinItemId)
    {
        applySkinFromItemId(skinItemId, true);
    };

    headerPanel.onUiScaleSelected = [this](int scaleId)
    {
        applyUiScaleFromItemId(scaleId, true);
    };

    headerPanel.onUiScaleReset = [this]
    {
        applyUiScaleFromItemId(PluginIDs::Settings::ScaleLevels::k100, true);
    };

    headerPanel.onSettingsRequested = [this]
    {
        if (settingsWindow_ != nullptr && settingsWindow_->isVisible())
            closeSettingsWindow();
        else
            openSettingsWindow();
    };

    headerPanel.onAboutRequested = [this]
    {
        if (aboutWindow_ != nullptr && aboutWindow_->isVisible())
            closeAboutWindow();
        else
            openAboutWindow();
    };

    headerPanel.onAudioMidiSettingsRequested = []
    {
        Core::StandaloneAudioInputRouter::showAudioMidiSettingsDialog();
    };

    headerPanel.onPanicRequested = [this]
    {
        pluginProcessor.getMidiManager().sendPanic();
    };

    wireHeaderEditorialUndoRedoButtons(headerPanel);

#if JUCE_DEBUG
    headerPanel.onUiTestsToggleRequested = [this]
    {
        setUiElementsTestVisible(!uiElementsTestVisible_);
    };
#endif
}

void PluginEditor::wireHeaderEditorialUndoRedoButtons(HeaderPanel& headerPanel)
{
    headerPanel.getUndoButton().onClick = [this]
    {
        if (isEditorialUndoBlockedByTextFocus())
            return;

        if (isEditorialUndoBlockedByModalOverlay())
            return;

        if (! pluginProcessor.canPerformEditorialUndo())
            return;

        prepareEditorialUndoRedo();
        pluginProcessor.performEditorialUndo();
    };

    headerPanel.getRedoButton().onClick = [this]
    {
        if (isEditorialUndoBlockedByTextFocus())
            return;

        if (isEditorialUndoBlockedByModalOverlay())
            return;

        if (! pluginProcessor.canPerformEditorialRedo())
            return;

        prepareEditorialUndoRedo();
        pluginProcessor.performEditorialRedo();
    };
}

void PluginEditor::restoreHeaderPanelFromState(HeaderPanel& headerPanel)
{
    headerPanel.setCurrentUiScaleId(pluginProcessor.getGuiScaleId());
    headerPanel.setCurrentSkinItemId(pluginProcessor.getSkinVariantId());
}

void PluginEditor::refreshMidiPortListsFromOsChange()
{
    if (mainComponent_ == nullptr)
        return;

    auto& headerPanel = mainComponent_->getHeaderPanel();
    headerPanel.refreshPortLists();
    headerPanel.selectMidiFromPort(
        pluginProcessor.getApvts().state.getProperty("midiInputPortId", juce::String()).toString());
    headerPanel.selectMidiToPort(
        pluginProcessor.getApvts().state.getProperty("midiOutputPortId", juce::String()).toString());

    if (pluginProcessor.isStandalone())
    {
        headerPanel.selectKeyboardFromPort(
            pluginProcessor.getApvts().state.getProperty("keyboardFromPortId", juce::String()).toString());
    }

    refreshEpromTypePromptDialogPorts();
}
