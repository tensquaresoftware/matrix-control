#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"
#include "PopupMenuPositioner.h"

namespace TSS
{
    /** What flat / hierarchical popup menus need from the closed control. */
    class IPopupMenuHost
    {
    public:
        virtual ~IPopupMenuHost() = default;

        virtual float getUiScale() const = 0;
        virtual const PopupMenuLook& getPopupMenuLook() const = 0;
        virtual juce::Component& asHostComponent() = 0;
        virtual const juce::Component& asHostComponent() const = 0;
        virtual int getBaseComponentWidth() const = 0;
        virtual int getScaledVerticalMargin() const = 0;
        virtual PopupVerticalPlacement getPopupVerticalPlacement() const = 0;

        virtual void notifyPopupOpened() = 0;
        virtual void notifyPopupClosed() = 0;
    };
}
