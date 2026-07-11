#pragma once

#include <functional>

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

namespace Core
{

// Shared combobox patch-send debounce window (FR-57 / FR-52 policy).
// 150 ms — no PRD millisecond value; aligns with human combobox scroll feel.
inline constexpr int kComboboxPatchSendDebounceMs = 150;

class ComboboxPatchSendDebouncer final : private juce::Timer
{
public:
    explicit ComboboxPatchSendDebouncer(int debounceMsOverride = kComboboxPatchSendDebounceMs);
    ~ComboboxPatchSendDebouncer() override;

    void schedule(std::function<void()> callback);

    // Unit-test seam — runs a pending debounced callback without wall-clock waits (CI-safe).
    void flushPendingSynchronouslyForTests();

private:
    void timerCallback() override;

    int debounceMs_;
    std::function<void()> pending_;
};

} // namespace Core
