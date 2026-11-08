#include "Standalone/StandaloneTitleBarDoubleClick.h"

#include "Standalone/StandaloneWindowPlacement.h"

#if JUCE_WINDOWS
 #include <unordered_map>
 #include <windows.h>
#elif JUCE_MAC
 #include <CoreGraphics/CGGeometry.h>
 #include <objc/message.h>
 #include <objc/objc.h>
 #include <objc/runtime.h>
 #include <unordered_map>
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

namespace
{
// JUCE sets the NSWindow as its own NSWindowDelegate; title-bar double-click / zoom
// asks windowShouldZoom:toFrame:. Return NO and recentre so OS zoom is not product behaviour.
// (Do not swizzle windowWillUseStandardFrame:defaultFrame: — CGRect IMP replace is
// ABI-fragile and was observed to pin windows at 0,0.)
using WindowShouldZoomFn = BOOL (*) (id, SEL, id, CGRect);

std::unordered_map<const void*, TitleBarDoubleClickHook::Callback>& macZoomCallbacks()
{
    static std::unordered_map<const void*, TitleBarDoubleClickHook::Callback> callbacks;
    return callbacks;
}

std::unordered_map<Class, WindowShouldZoomFn>& macZoomOriginals()
{
    static std::unordered_map<Class, WindowShouldZoomFn> originals;
    return originals;
}

id nsWindowFromComponent (juce::Component& window)
{
    auto* peer = window.getPeer();
    if (peer == nullptr)
        return nil;

    const id nsView = static_cast<id> (peer->getNativeHandle());
    if (nsView == nil)
        return nil;

    using WindowGetter = id (*) (id, SEL);
    return reinterpret_cast<WindowGetter> (objc_msgSend) (nsView, sel_registerName ("window"));
}

BOOL macWindowShouldZoom (id self, SEL sel, id windowArg, CGRect proposedFrame)
{
    const id nsWindow = windowArg != nil ? windowArg : self;
    auto& callbacks = macZoomCallbacks();
    const auto it = callbacks.find (nsWindow);

    if (it != callbacks.end() && it->second != nullptr)
    {
        it->second();
        return NO;
    }

    const Class cls = object_getClass (self);
    const auto origIt = macZoomOriginals().find (cls);

    if (origIt != macZoomOriginals().end() && origIt->second != nullptr)
        return origIt->second (self, sel, windowArg, proposedFrame);

    return YES;
}

bool installMacZoomHookOnClass (Class cls)
{
    if (cls == nil)
        return false;

    auto& originals = macZoomOriginals();

    if (originals.find (cls) != originals.end())
        return true;

    const SEL sel = sel_registerName ("windowShouldZoom:toFrame:");
    Method method = class_getInstanceMethod (cls, sel);

    if (method == nullptr)
        return false;

    const auto previous = reinterpret_cast<WindowShouldZoomFn> (
        method_setImplementation (method, reinterpret_cast<IMP> (macWindowShouldZoom)));
    originals.emplace (cls, previous);
    return true;
}

const void* installMacTitleBarZoomHook (juce::Component& window, TitleBarDoubleClickHook::Callback callback)
{
    if (callback == nullptr)
        return nullptr;

    const id nsWindow = nsWindowFromComponent (window);
    if (nsWindow == nil)
        return nullptr;

    if (! installMacZoomHookOnClass (object_getClass (nsWindow)))
        return nullptr;

    macZoomCallbacks()[nsWindow] = std::move (callback);
    return nsWindow;
}

void removeMacTitleBarZoomHook (const void* nsWindowKey)
{
    if (nsWindowKey != nullptr)
        macZoomCallbacks().erase (nsWindowKey);
}
} // namespace

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
    platform_->nsWindowKey = installMacTitleBarZoomHook (window, std::move (onDoubleClick));

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
        removeMacTitleBarZoomHook (platform_->nsWindowKey);
   #endif

    platform_.reset();
}
} // namespace MatrixStandalone
