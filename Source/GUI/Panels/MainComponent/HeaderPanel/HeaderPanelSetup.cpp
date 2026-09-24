// Extracted from HeaderPanel.cpp for modular maintenance.
// Logo callback wiring and child control attachment.

#include "HeaderPanel.h"

#include "GUI/Looks/LookBuilders.h"

void HeaderPanel::wireLogoCallbacks()
{
    logo_.onPopupRequested = [this] { showLogoPopup(); };
    logo_.onSettingsRequested = [this]
    {
        if (onSettingsRequested)
            onSettingsRequested();
    };
    logo_.onAudioMidiSettingsRequested = [this]
    {
        if (isPluginMode_)
            return;

        if (onAudioMidiSettingsRequested)
            onAudioMidiSettingsRequested();
    };
#if JUCE_DEBUG
    logo_.onUiTestsToggleRequested = [this]
    {
        if (onUiTestsToggleRequested)
            onUiTestsToggleRequested();
    };
#endif
    logo_.onUiScaleReset = [this]
    {
        if (onUiScaleReset)
            onUiScaleReset();
    };
}

void HeaderPanel::wireActionButtons()
{
    panicButton_.onClick = [this]
    {
        if (onPanicRequested)
            onPanicRequested();
    };

    undoButton_.setEnabled(false);
    redoButton_.setEnabled(false);
}

void HeaderPanel::addChildControls(TSS::ISkin& skin)
{
    addAndMakeVisible(logo_);

    instrumentActivityLed_.setSkin(skin);
    addAndMakeVisible(instrumentActivityLed_);
    addAndMakeVisible(keyboardFromLabel_);
    keyboardFromComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    addAndMakeVisible(keyboardFromComboBox_);

    editorActivityLed_.setSkin(skin);
    addAndMakeVisible(editorActivityLed_);
    addAndMakeVisible(midiFromLabel_);
    midiFromComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    addAndMakeVisible(midiFromComboBox_);

    midiToActivityLed_.setSkin(skin);
    addAndMakeVisible(midiToActivityLed_);
    addAndMakeVisible(midiToLabel_);
    midiToComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    addAndMakeVisible(midiToComboBox_);

    addAndMakeVisible(audioFromLabel_);
    audioFromComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    addAndMakeVisible(audioFromComboBox_);
    addAndMakeVisible(inputGainLabel_);
    addAndMakeVisible(inputGainSlider_);
    peakIndicator_.setSkin(skin);
    addAndMakeVisible(peakIndicator_);
    addAndMakeVisible(undoButton_);
    addAndMakeVisible(redoButton_);
    addAndMakeVisible(panicButton_);

    const auto refreshMidiPortsBeforeOpen = [this]()
    {
        if (onMidiPortListsRefreshRequested)
            onMidiPortListsRefreshRequested();
        else
            refreshPortLists();
    };
    midiFromComboBox_.onAboutToShowPopup = refreshMidiPortsBeforeOpen;
    midiToComboBox_.onAboutToShowPopup = refreshMidiPortsBeforeOpen;
    keyboardFromComboBox_.onAboutToShowPopup = refreshMidiPortsBeforeOpen;
}

void HeaderPanel::applyPanicButtonLook()
{
    if (skin_ == nullptr)
        return;

    panicButton_.setLook(panicAlertActive_
                             ? TSS::buttonAlertLookFromSkin(*skin_)
                             : TSS::buttonLookFromSkin(*skin_));
}

void HeaderPanel::setPanicQueuePressureAlert(bool active)
{
    if (panicAlertActive_ == active)
        return;

    panicAlertActive_ = active;
    applyPanicButtonLook();
}

void HeaderPanel::setPanicMidiOutputAvailable(bool available)
{
    // Grayed + non-clickable when MIDI To is unset — Panic has nowhere to send.
    panicButton_.setInactiveAppearance(! available);
    panicButton_.setEnabled(available);
}

void HeaderPanel::syncPanicEnabledFromMidiToSelection()
{
    setPanicMidiOutputAvailable(getSelectedMidiToPortIdentifier().isNotEmpty());
}

void HeaderPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    logo_.setSkin(skin);
    midiFromLabel_.setLook(TSS::darkPanelLabelLookFromSkin(skin));
    midiFromComboBox_.setLook(TSS::comboBoxLookFromSkin(skin));
    midiFromComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    midiToLabel_.setLook(TSS::darkPanelLabelLookFromSkin(skin));
    midiToComboBox_.setLook(TSS::comboBoxLookFromSkin(skin));
    midiToComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    keyboardFromLabel_.setLook(TSS::darkPanelLabelLookFromSkin(skin));
    keyboardFromComboBox_.setLook(TSS::comboBoxLookFromSkin(skin));
    keyboardFromComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    editorActivityLed_.setSkin(skin);
    midiToActivityLed_.setSkin(skin);
    instrumentActivityLed_.setSkin(skin);
    audioFromLabel_.setLook(TSS::darkPanelLabelLookFromSkin(skin));
    audioFromComboBox_.setLook(TSS::comboBoxLookFromSkin(skin));
    audioFromComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    inputGainLabel_.setLook(TSS::darkPanelLabelLookFromSkin(skin));
    inputGainSlider_.setLook(TSS::sliderLookFromSkin(skin));
    peakIndicator_.setSkin(skin);
    undoButton_.setLook(TSS::buttonLookFromSkin(skin));
    redoButton_.setLook(TSS::buttonLookFromSkin(skin));
    applyPanicButtonLook();
}

void HeaderPanel::syncEditorialUndoRedoAvailability(bool canUndo, bool canRedo)
{
    undoButton_.setEnabled(canUndo);
    redoButton_.setEnabled(canRedo);
}
