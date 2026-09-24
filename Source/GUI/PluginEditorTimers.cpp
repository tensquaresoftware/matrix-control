// Extracted from PluginEditor.cpp for modular maintenance.
// Method bodies for the HeaderRefreshTimer + ClipboardFeedbackPhaseTimer nested classes
// (declared as private members of PluginEditor in PluginEditor.h).

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "Core/Audio/AudioPassthroughProcessor.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/Queue/RealtimeQueuePressureMonitor.h"
#include "GUI/Panels/MainComponent/FooterPanel/FooterPanel.h"
#include "GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "Shared/Definitions/PluginIDs.h"

PluginEditor::HeaderRefreshTimer::HeaderRefreshTimer(PluginProcessor& processor,
                                                      HeaderPanel& headerPanel,
                                                      PluginEditor& owner)
    : processor_(processor)
    , headerPanel_(headerPanel)
    , owner_(owner)
{
    startTimerHz(30);
}

void PluginEditor::HeaderRefreshTimer::pollOpenMidiPopups()
{
    // CoreMIDI sometimes delays or skips list callbacks while a modal popup is open
    // (e.g. power-off of a USB interface). Poll slowly so open menus stay truthful.
    constexpr int kMidiPopupPollIntervalTicks = 15; // 0.5 s at 30 Hz
    if (++midiPopupPollTicks_ < kMidiPopupPollIntervalTicks)
        return;

    midiPopupPollTicks_ = 0;
    if (headerPanel_.getMidiFromComboBox().isPopupOpen()
        || headerPanel_.getMidiToComboBox().isPopupOpen()
        || headerPanel_.getKeyboardFromComboBox().isPopupOpen())
    {
        // List-only: do not force-reopen ports while the user is browsing the menu.
        owner_.refreshMidiPortListsFromOsChange(false);
    }
}

void PluginEditor::HeaderRefreshTimer::timerCallback()
{
    if (processor_.isStandalone())
    {
        if (audioFromRefreshAttempts_ < 60)
        {
            const auto names = processor_.getAudioInputSourceNames();

            if (names.isEmpty())
            {
                ++audioFromRefreshAttempts_;
                owner_.refreshAudioFromCombo();
            }
        }

        headerPanel_.getPeakIndicator().setLevel(
            processor_.getAudioPassthroughProcessor().getPeakLevel());
    }

    pollOpenMidiPopups();

    const auto& tracker = processor_.getMidiActivityTracker();
    headerPanel_.getInstrumentActivityLed().setLevel(
        tracker.getActivityLevel(Core::MidiActivityTracker::Path::kInstrument));
    headerPanel_.getEditorActivityLed().setLevel(
        tracker.getActivityLevel(Core::MidiActivityTracker::Path::kMidiFromInbound));
    headerPanel_.getMidiToActivityLed().setLevel(
        tracker.getActivityLevel(Core::MidiActivityTracker::Path::kOutbound));

    const bool editorialUndoRedoBlocked = owner_.isEditorialUndoBlockedByTextFocus()
                                          || owner_.isEditorialUndoBlockedByModalOverlay();
    headerPanel_.syncEditorialUndoRedoAvailability(
        ! editorialUndoRedoBlocked && processor_.canPerformEditorialUndo(),
        ! editorialUndoRedoBlocked && processor_.canPerformEditorialRedo());

    const auto realtimeDepth = processor_.getMidiManager().getRealtimeOutboundDepth();
    if (queuePressureMonitor_.update(realtimeDepth, juce::Time::getMillisecondCounter()))
    {
        const bool alert = queuePressureMonitor_.isAlertActive();
        headerPanel_.setPanicQueuePressureAlert(alert);

        if (owner_.mainComponent_ != nullptr)
            owner_.mainComponent_->getFooterPanel().setMidiQueuePressureAlert(alert);
    }
}

PluginEditor::ClipboardFeedbackPhaseTimer::ClipboardFeedbackPhaseTimer(juce::AudioProcessorValueTreeState& apvts)
    : state_(apvts.state)
{
    state_.addListener(this);
    syncTimerFromState();
}

PluginEditor::ClipboardFeedbackPhaseTimer::~ClipboardFeedbackPhaseTimer()
{
    state_.removeListener(this);
    stopTimer();
}

void PluginEditor::ClipboardFeedbackPhaseTimer::timerCallback()
{
    const bool copyLit = static_cast<bool>(
        state_.getProperty(PluginIDs::ClipboardFeedback::kCopyLit, true));
    state_.setProperty(PluginIDs::ClipboardFeedback::kCopyLit, ! copyLit, nullptr);
}

void PluginEditor::ClipboardFeedbackPhaseTimer::valueTreePropertyChanged(juce::ValueTree&,
                                                                         const juce::Identifier& property)
{
    if (property.toString() == PluginIDs::ClipboardFeedback::kActive)
        syncTimerFromState();
}

void PluginEditor::ClipboardFeedbackPhaseTimer::valueTreeRedirected(juce::ValueTree&)
{
    syncTimerFromState();
}

void PluginEditor::ClipboardFeedbackPhaseTimer::syncTimerFromState()
{
    const bool active = static_cast<bool>(
        state_.getProperty(PluginIDs::ClipboardFeedback::kActive, false));
    if (active)
        startTimerHz(2);
    else
        stopTimer();
}
