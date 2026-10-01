#include "PopupMenuOutsideDismissWatcher.h"

#if JUCE_MAC
 #include <objc/message.h>
 #include <objc/runtime.h>
#endif

namespace TSS
{
    namespace
    {
        bool isLeftMouseButtonDownRealtime() noexcept
        {
#if JUCE_MAC
            Class nsEventClass = objc_getClass("NSEvent");
            if (nsEventClass == nullptr)
                return juce::ModifierKeys::getCurrentModifiers().isLeftButtonDown();

            using PressedMouseButtonsFn = unsigned long (*)(Class, SEL);
            const SEL sel = sel_registerName("pressedMouseButtons");
            const auto buttons = reinterpret_cast<PressedMouseButtonsFn>(objc_msgSend)(nsEventClass, sel);
            return (buttons & 1ul) != 0ul;
#else
            // Windows/Linux realtime modifiers already query the OS button state.
            return juce::ComponentPeer::getCurrentModifiersRealtime().isLeftButtonDown();
#endif
        }

        juce::Point<float> mouseScreenPosition() noexcept
        {
            return juce::Desktop::getInstance().getMainMouseSource().getScreenPosition();
        }
    }

    PopupMenuOutsideDismissWatcher::PopupMenuOutsideDismissWatcher(
        juce::Component& popup,
        std::function<void()> onOutsideDismiss)
        : popup_(popup)
        , onOutsideDismiss_(std::move(onOutsideDismiss))
    {
        jassert(onOutsideDismiss_ != nullptr);
    }

    PopupMenuOutsideDismissWatcher::~PopupMenuOutsideDismissWatcher()
    {
        stopTimer();
    }

    void PopupMenuOutsideDismissWatcher::arm()
    {
        if (dismissing_ || onOutsideDismiss_ == nullptr)
            return;

        waitingForOpenRelease_ = isLeftMouseButtonDownRealtime();
        previousLeftDown_ = waitingForOpenRelease_;
        startTimerHz(kPollHz_);
    }

    void PopupMenuOutsideDismissWatcher::dismissForOutsidePress()
    {
        dismissing_ = true;
        stopTimer();

        auto dismiss = std::move(onOutsideDismiss_);
        onOutsideDismiss_ = nullptr;
        dismiss();
    }

    void PopupMenuOutsideDismissWatcher::timerCallback()
    {
        const bool down = isLeftMouseButtonDownRealtime();

        if (waitingForOpenRelease_)
        {
            if (! down)
                waitingForOpenRelease_ = false;

            previousLeftDown_ = down;
            return;
        }

        const bool risingEdge = down && ! previousLeftDown_;
        previousLeftDown_ = down;

        if (dismissing_ || onOutsideDismiss_ == nullptr || ! popup_.isCurrentlyModal() || ! risingEdge)
            return;

        if (popup_.getScreenBounds().toFloat().contains(mouseScreenPosition()))
            return;

        dismissForOutsidePress();
    }
}
