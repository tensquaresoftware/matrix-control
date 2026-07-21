#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "IPopupMenuHost.h"

namespace TSS
{
    /** Shared modal dismiss / teardown for ComboBox-family popups (self-deleting). */
    namespace PopupMenuModalHelpers
    {
        /** exitModal → notifyClosed → removeChild → optional afterClose → delete popup. */
        inline void dismissAndDelete(juce::Component& popup,
                                     IPopupMenuHost& host,
                                     std::function<void()> afterClose = nullptr)
        {
            popup.exitModalState(0);
            host.notifyPopupClosed();

            if (auto* parent = popup.getParentComponent())
                parent->removeChildComponent(&popup);

            if (afterClose != nullptr)
                afterClose();

            delete &popup;
        }

        inline bool handleEscapeKey(const juce::KeyPress& key, juce::Component& popup, IPopupMenuHost& host)
        {
            if (key.getKeyCode() != juce::KeyPress::escapeKey)
                return false;

            dismissAndDelete(popup, host);
            return true;
        }
    }
}
