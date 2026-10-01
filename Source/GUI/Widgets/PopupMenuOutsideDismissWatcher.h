#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    /** Dismisses a modal popup when the left mouse button is pressed outside it,
        including native title-bar / host-chrome presses that never reach JUCE components. */
    class PopupMenuOutsideDismissWatcher : private juce::Timer
    {
    public:
        explicit PopupMenuOutsideDismissWatcher(juce::Component& popup,
                                                std::function<void()> onOutsideDismiss);
        ~PopupMenuOutsideDismissWatcher() override;

        PopupMenuOutsideDismissWatcher(const PopupMenuOutsideDismissWatcher&) = delete;
        PopupMenuOutsideDismissWatcher& operator=(const PopupMenuOutsideDismissWatcher&) = delete;

        /** Call after enterModalState. Ignores the opening button-down until it is released. */
        void arm();

    private:
        void timerCallback() override;
        void dismissForOutsidePress();

        juce::Component& popup_;
        std::function<void()> onOutsideDismiss_;
        bool waitingForOpenRelease_ = false;
        bool previousLeftDown_ = false;
        bool dismissing_ = false;

        static constexpr int kPollHz_ = 60;
    };
}
