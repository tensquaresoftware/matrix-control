#pragma once

#include <functional>

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

namespace Core
{

// Shared combobox patch-send debounce window (FR-57 / FR-52 policy).
// 150 ms — no PRD millisecond value; aligns with human combobox scroll feel.
inline constexpr int kComboboxPatchSendDebounceMs = 150;

// Patch Manager Next/Previous button navigation settle window (separate from combobox 150).
inline constexpr int kPatchNavButtonDebounceMs = 300;

// Master Edit outbound full-dump (0x03) settle window. Same 150 ms order as combobox /
// Mutator History, but named separately: Master always sends a full Parameter Data dump,
// not a granular patch-nav / audition path (do not alias kPatchNavButtonDebounceMs).
inline constexpr int kMasterEditSysExDebounceMs = 150;

class ComboboxPatchSendDebouncer final : private juce::Timer
{
public:
    explicit ComboboxPatchSendDebouncer(int debounceMsOverride = kComboboxPatchSendDebounceMs);
    ~ComboboxPatchSendDebouncer() override;

    void schedule(std::function<void()> callback);

    // Drops a pending callback without running it (superseded / cancelled settle).
    void cancel();

    bool isPending() const noexcept { return isTimerRunning(); }

    // Unit-test seam — runs a pending debounced callback without wall-clock waits (CI-safe).
    void flushPendingSynchronouslyForTests();

private:
    void timerCallback() override;

    int debounceMs_;
    std::function<void()> pending_;
};

} // namespace Core
