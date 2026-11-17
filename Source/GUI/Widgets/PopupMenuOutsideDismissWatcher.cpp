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

        /** Inside = over the popup or a child, OR still within the popup's bounding box.
            Do not treat "component under mouse is something else" as outside when that
            something is only visible through a hitTest hole in an L-shaped host. */
        bool isPointerInsidePopup(const juce::Component& popup) noexcept
        {
            if (auto* under = juce::Desktop::getInstance().getMainMouseSource().getComponentUnderMouse())
            {
                for (auto* c = under; c != nullptr; c = c->getParentComponent())
                    if (c == &popup)
                        return true;
            }

            if (auto* top = popup.getTopLevelComponent())
            {
                const auto mouseInTop = top->getLocalPoint(nullptr, mouseScreenPosition());
                const auto popupInTop = top->getLocalArea(&popup, popup.getLocalBounds()).toFloat();
                return popupInTop.contains(mouseInTop);
            }

            return popup.getScreenBounds().toFloat().contains(mouseScreenPosition());
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

    void PopupMenuOutsideDismissWatcher::arm(int ignoreRisingEdgeMs)
    {
        if (dismissing_ || onOutsideDismiss_ == nullptr)
            return;

        waitingForOpenRelease_ = isLeftMouseButtonDownRealtime();
        previousLeftDown_ = waitingForOpenRelease_;
        ignoreRisingEdgesUntilMs_ = juce::Time::getMillisecondCounter()
            + static_cast<juce::uint32>(juce::jmax(0, ignoreRisingEdgeMs));
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

        if (juce::Time::getMillisecondCounter() < ignoreRisingEdgesUntilMs_)
        {
            // Keep previousLeftDown_ false while the button is held through grace so the
            // first post-grace tick still sees a rising edge (outside → dismiss).
            if (! down)
                previousLeftDown_ = false;
            return;
        }

        const bool risingEdge = down && ! previousLeftDown_;
        previousLeftDown_ = down;

        if (dismissing_ || onOutsideDismiss_ == nullptr || ! popup_.isCurrentlyModal() || ! risingEdge)
            return;

        if (isPointerInsidePopup(popup_))
            return;

        dismissForOutsidePress();
    }
}
