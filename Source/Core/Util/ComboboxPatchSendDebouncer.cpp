#include "Core/Util/ComboboxPatchSendDebouncer.h"

namespace Core
{

ComboboxPatchSendDebouncer::ComboboxPatchSendDebouncer(int debounceMsOverride)
    : debounceMs_(debounceMsOverride)
{
}

ComboboxPatchSendDebouncer::~ComboboxPatchSendDebouncer()
{
    stopTimer();
}

void ComboboxPatchSendDebouncer::schedule(std::function<void()> callback)
{
    pending_ = std::move(callback);
    startTimer(debounceMs_);
}

void ComboboxPatchSendDebouncer::flushPendingSynchronouslyForTests()
{
    if (isTimerRunning())
    {
        stopTimer();
        timerCallback();
    }
}

void ComboboxPatchSendDebouncer::timerCallback()
{
    stopTimer();

    if (pending_)
    {
        auto callback = std::move(pending_);
        pending_ = nullptr;
        callback();
    }
}

} // namespace Core
