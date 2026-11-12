#include "Standalone/StandaloneTitleBarDoubleClick.h"

#include "Standalone/StandaloneWindowPlacement.h"

#if JUCE_WINDOWS
 #include <unordered_map>
 #include <windows.h>
#elif JUCE_MAC
 #include "Standalone/StandaloneTitleBarDoubleClick_mac.h"
#endif

namespace MatrixStandalone
{
#if JUCE_WINDOWS
namespace
{
struct WindowsHookState
{
    WNDPROC previousProc = nullptr;
    TitleBarDoubleClickHook::Callback callback;
};

std::unordered_map<HWND, WindowsHookState>& windowsHookStates()
{
    static std::unordered_map<HWND, WindowsHookState> states;
    return states;
}

LRESULT CALLBACK titleBarWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    auto& states = windowsHookStates();
    const auto it = states.find (hwnd);

    if (it == states.end())
        return DefWindowProc (hwnd, message, wParam, lParam);

    const auto previousProc = it->second.previousProc;

    if (message == WM_NCLBUTTONDBLCLK
        && (wParam == HTCAPTION || wParam == HTTOP)
        && it->second.callback != nullptr)
    {
        const auto callback = it->second.callback;
        juce::MessageManager::callAsync ([callback]
        {
            if (callback != nullptr)
                callback();
        });
        return 0;
    }

    if (message == WM_NCDESTROY)
    {
        auto callback = std::move (it->second.callback);
        juce::ignoreUnused (callback);
        states.erase (it);

        if (previousProc != nullptr)
            return CallWindowProc (previousProc, hwnd, message, wParam, lParam);

        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    if (previousProc != nullptr)
        return CallWindowProc (previousProc, hwnd, message, wParam, lParam);

    return DefWindowProc (hwnd, message, wParam, lParam);
}

bool installWindowsTitleBarHook (HWND hwnd, TitleBarDoubleClickHook::Callback callback)
{
    if (hwnd == nullptr || callback == nullptr)
        return false;

    auto& states = windowsHookStates();

    if (states.find (hwnd) != states.end())
        return false;

    SetLastError (0);
    const auto previous = reinterpret_cast<WNDPROC> (
        SetWindowLongPtr (hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR> (titleBarWndProc)));

    if (previous == nullptr && GetLastError() != 0)
        return false;

    states.emplace (hwnd, WindowsHookState { previous, std::move (callback) });
    return true;
}

void removeWindowsTitleBarHook (HWND hwnd)
{
    if (hwnd == nullptr)
        return;

    auto& states = windowsHookStates();
    const auto it = states.find (hwnd);

    if (it == states.end())
        return;

    const auto current = reinterpret_cast<WNDPROC> (GetWindowLongPtr (hwnd, GWLP_WNDPROC));

    if (current == titleBarWndProc)
        SetWindowLongPtr (hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR> (it->second.previousProc));

    states.erase (it);
}
} // namespace

struct TitleBarDoubleClickHook::PlatformState
{
    HWND hwnd = nullptr;
};

#elif JUCE_MAC

struct TitleBarDoubleClickHook::PlatformState
{
    const void* nsWindowKey = nullptr;
};

#elif JUCE_LINUX

namespace
{
class TitleBarDoubleClickPoller final : private juce::Timer
{
public:
    TitleBarDoubleClickPoller (juce::Component& window, TitleBarDoubleClickHook::Callback callback)
        : window_ (window)
        , callback_ (std::move (callback))
    {
        startTimerHz (kPollHz);
    }

    ~TitleBarDoubleClickPoller() override { stopTimer(); }

private:
    static constexpr int kPollHz = 60;
    static constexpr int kMaxClickDistancePx = 6;
    static constexpr int kDoubleClickIntervalMs = 500;

    juce::BorderSize<int> nativeFrame() const
    {
        if (auto* peer = window_.getPeer())
            if (const auto frameSize = peer->getFrameSizeIfPresent())
                return *frameSize;

        return {};
    }

    bool cursorInTitleBarStrip() const
    {
        const auto frame = nativeFrame();
        if (frame.getTop() <= 0)
            return false;

        const auto strip = titleBarDragStrip (window_.getBounds(), frame);
        return strip.toFloat().contains (
            juce::Desktop::getInstance().getMainMouseSource().getScreenPosition());
    }

    void timerCallback() override
    {
        if (callback_ == nullptr || ! window_.isShowing())
        {
            previousLeftDown_ = juce::ComponentPeer::getCurrentModifiersRealtime().isLeftButtonDown();
            return;
        }

        const bool down = juce::ComponentPeer::getCurrentModifiersRealtime().isLeftButtonDown();
        const bool risingEdge = down && ! previousLeftDown_;
        previousLeftDown_ = down;

        if (! risingEdge || ! cursorInTitleBarStrip())
            return;

        const auto now = juce::Time::getMillisecondCounter();
        const auto pos = juce::Desktop::getInstance().getMainMouseSource().getScreenPosition().roundToInt();

        const bool secondClick = hasLastClick_
                                 && (now - lastClickMs_) <= (uint32_t) kDoubleClickIntervalMs
                                 && std::abs (pos.x - lastClickPos_.x) <= kMaxClickDistancePx
                                 && std::abs (pos.y - lastClickPos_.y) <= kMaxClickDistancePx;

        if (secondClick)
        {
            hasLastClick_ = false;
            lastClickPos_ = {};
            callback_();
            return;
        }

        hasLastClick_ = true;
        lastClickMs_ = now;
        lastClickPos_ = pos;
    }

    juce::Component& window_;
    TitleBarDoubleClickHook::Callback callback_;
    bool previousLeftDown_ = false;
    bool hasLastClick_ = false;
    uint32_t lastClickMs_ = 0;
    juce::Point<int> lastClickPos_;
};
} // namespace

struct TitleBarDoubleClickHook::PlatformState
{
    std::unique_ptr<TitleBarDoubleClickPoller> poller;
};

#else

struct TitleBarDoubleClickHook::PlatformState
{
};

#endif

TitleBarDoubleClickHook::TitleBarDoubleClickHook() = default;

TitleBarDoubleClickHook::~TitleBarDoubleClickHook()
{
    detach();
}

void TitleBarDoubleClickHook::attach (juce::Component& window, Callback onDoubleClick)
{
    detach();

    if (onDoubleClick == nullptr)
        return;

    platform_ = std::make_unique<PlatformState>();

   #if JUCE_WINDOWS
    auto* peer = window.getPeer();
    if (peer == nullptr)
    {
        platform_.reset();
        return;
    }

    platform_->hwnd = static_cast<HWND> (peer->getNativeHandle());

    if (! installWindowsTitleBarHook (platform_->hwnd, std::move (onDoubleClick)))
        platform_.reset();
   #elif JUCE_MAC
    platform_->nsWindowKey = MacTitleBarDetail::attach (window, std::move (onDoubleClick));

    if (platform_->nsWindowKey == nullptr)
        platform_.reset();
   #elif JUCE_LINUX
    platform_->poller = std::make_unique<TitleBarDoubleClickPoller> (window, std::move (onDoubleClick));
   #else
    juce::ignoreUnused (window, onDoubleClick);
    platform_.reset();
   #endif
}

void TitleBarDoubleClickHook::detach()
{
   #if JUCE_WINDOWS
    if (platform_ != nullptr && platform_->hwnd != nullptr)
        removeWindowsTitleBarHook (platform_->hwnd);
   #elif JUCE_MAC
    if (platform_ != nullptr && platform_->nsWindowKey != nullptr)
        MacTitleBarDetail::detach (platform_->nsWindowKey);
   #endif

    platform_.reset();
}
} // namespace MatrixStandalone
