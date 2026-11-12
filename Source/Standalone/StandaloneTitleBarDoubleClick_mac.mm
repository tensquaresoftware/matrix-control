#include "Standalone/StandaloneTitleBarDoubleClick_mac.h"

#import <AppKit/AppKit.h>
#import <objc/runtime.h>

#include <unordered_map>

// macOS title-bar double-click: AppKit may Zoom, Minimize, or Fill depending on System Settings.
// windowShouldZoom:toFrame: only covers Zoom (and JUCE already returns NO for non-resizable
// native title bars). Golden Gate+ needs an NSEvent local monitor that recentres and swallows
// the double-click so Minimize/Fill cannot steal the gesture.

namespace MatrixStandalone::MacTitleBarDetail
{
namespace
{
using WindowShouldZoomFn = BOOL (*) (id, SEL, id, NSRect);

struct HookState
{
    TitleBarDoubleClickHook::Callback callback;
    id eventMonitor = nil;
};

std::unordered_map<const void*, HookState>& hookStates()
{
    static std::unordered_map<const void*, HookState> states;
    return states;
}

std::unordered_map<Class, WindowShouldZoomFn>& zoomOriginals()
{
    static std::unordered_map<Class, WindowShouldZoomFn> originals;
    return originals;
}

NSWindow* nsWindowFromComponent (juce::Component& window)
{
    auto* peer = window.getPeer();
    if (peer == nullptr)
        return nil;

    const id nsView = static_cast<id> (peer->getNativeHandle());
    if (nsView == nil)
        return nil;

    return [nsView window];
}

BOOL macWindowShouldZoom (id self, SEL sel, id windowArg, NSRect proposedFrame)
{
    const id nsWindow = windowArg != nil ? windowArg : self;
    auto& states = hookStates();
    const auto it = states.find (nsWindow);

    if (it != states.end() && it->second.callback != nullptr)
    {
        const auto callback = it->second.callback;
        juce::MessageManager::callAsync ([callback]
        {
            if (callback != nullptr)
                callback();
        });
        return NO;
    }

    const Class cls = object_getClass (self);
    const auto origIt = zoomOriginals().find (cls);

    if (origIt != zoomOriginals().end() && origIt->second != nullptr)
        return origIt->second (self, sel, windowArg, proposedFrame);

    juce::ignoreUnused (proposedFrame);
    return YES;
}

bool installZoomHookOnClass (Class cls)
{
    if (cls == nil)
        return false;

    auto& originals = zoomOriginals();

    if (originals.find (cls) != originals.end())
        return true;

    const SEL sel = @selector (windowShouldZoom:toFrame:);
    Method method = class_getInstanceMethod (cls, sel);

    if (method == nullptr)
        return false;

    const auto previous = reinterpret_cast<WindowShouldZoomFn> (
        method_setImplementation (method, reinterpret_cast<IMP> (macWindowShouldZoom)));
    originals.emplace (cls, previous);
    return true;
}

bool pointInTrafficLight (NSWindow* window, NSPoint locationInWindow)
{
    const NSWindowButton kinds[] = {
        NSWindowCloseButton,
        NSWindowMiniaturizeButton,
        NSWindowZoomButton
    };

    for (const auto kind : kinds)
    {
        NSButton* button = [window standardWindowButton: kind];
        if (button == nil)
            continue;

        const NSRect buttonInWindow = [button convertRect: button.bounds toView: nil];
        if (NSPointInRect (locationInWindow, NSInsetRect (buttonInWindow, -4.0, -4.0)))
            return true;
    }

    return false;
}

bool isTitleBarDoubleClick (NSWindow* window, NSEvent* event)
{
    if (window == nil || event == nil)
        return false;

    if (event.type != NSEventTypeLeftMouseDown || event.clickCount != 2)
        return false;

    if (event.window != window)
        return false;

    const NSPoint location = event.locationInWindow;
    const NSRect contentLayout = window.contentLayoutRect;

    // contentLayoutRect is the area below the title bar (window base coords).
    if (NSPointInRect (location, contentLayout))
        return false;

    const NSSize size = window.frame.size;
    if (location.x < 0.0 || location.y < 0.0
        || location.x > size.width || location.y > size.height)
        return false;

    if (pointInTrafficLight (window, location))
        return false;

    return true;
}

void invokeCallbackAsync (const TitleBarDoubleClickHook::Callback& callback)
{
    juce::MessageManager::callAsync ([callback]
    {
        if (callback != nullptr)
            callback();
    });
}
} // namespace

const void* attach (juce::Component& window, TitleBarDoubleClickHook::Callback callback)
{
    if (callback == nullptr)
        return nullptr;

    NSWindow* nsWindow = nsWindowFromComponent (window);
    if (nsWindow == nil)
        return nullptr;

    auto& states = hookStates();
    if (states.find (nsWindow) != states.end())
        return nullptr;

    juce::ignoreUnused (installZoomHookOnClass (object_getClass (nsWindow)));

    HookState state;
    state.callback = std::move (callback);

    __block NSWindow* watched = nsWindow;
    state.eventMonitor = [NSEvent addLocalMonitorForEventsMatchingMask: NSEventMaskLeftMouseDown
                                                               handler: ^NSEvent* (NSEvent* event)
    {
        auto& live = hookStates();
        const auto it = live.find (watched);
        if (it == live.end() || it->second.callback == nullptr)
            return event;

        if (! isTitleBarDoubleClick (watched, event))
            return event;

        invokeCallbackAsync (it->second.callback);
        return nil; // swallow Zoom / Minimize / Fill
    }];

    if (state.eventMonitor == nil)
        return nullptr;

    states.emplace (nsWindow, std::move (state));
    return nsWindow;
}

void detach (const void* nsWindowKey)
{
    if (nsWindowKey == nullptr)
        return;

    auto& states = hookStates();
    const auto it = states.find (nsWindowKey);
    if (it == states.end())
        return;

    if (it->second.eventMonitor != nil)
        [NSEvent removeMonitor: it->second.eventMonitor];

    states.erase (it);
}
} // namespace MatrixStandalone::MacTitleBarDetail
