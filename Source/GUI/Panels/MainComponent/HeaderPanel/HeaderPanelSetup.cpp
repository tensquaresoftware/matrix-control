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

    editorActivityLed_.setSkin(skin);
    addAndMakeVisible(editorActivityLed_);
    addAndMakeVisible(midiFromLabel_);

    midiToActivityLed_.setSkin(skin);
    addAndMakeVisible(midiToActivityLed_);
    addAndMakeVisible(midiToLabel_);

    addAndMakeVisible(inputGainLabel_);
    addAndMakeVisible(inputGainSlider_);
    peakIndicator_.setSkin(skin);
    addAndMakeVisible(peakIndicator_);
    addAndMakeVisible(undoButton_);
    addAndMakeVisible(redoButton_);
    addAndMakeVisible(panicButton_);
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
    // Grayed + non-clickable when SYNTH TO is unset — Panic has nowhere to send.
    panicButton_.setInactiveAppearance(! available);
    panicButton_.setEnabled(available);
}

void HeaderPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    logo_.setSkin(skin);
    keyboardFromLabel_.setLook(TSS::darkPanelLabelLookFromSkin(skin));
    midiFromLabel_.setLook(TSS::darkPanelLabelLookFromSkin(skin));
    midiToLabel_.setLook(TSS::darkPanelLabelLookFromSkin(skin));
    editorActivityLed_.setSkin(skin);
    midiToActivityLed_.setSkin(skin);
    instrumentActivityLed_.setSkin(skin);
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
