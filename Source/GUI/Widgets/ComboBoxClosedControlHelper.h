#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    /** Shared closed-state focus / scale / look helpers for ComboBox and HierarchicalComboBox. */
    namespace ComboBoxClosedControlHelper
    {
        inline bool applyUiScale(float& currentScale, float newScale, juce::Component& component)
        {
            if (juce::approximatelyEqual(currentScale, newScale))
                return false;

            currentScale = newScale;
            component.repaint();
            return true;
        }

        template <typename LookT>
        inline void applyLook(LookT& target, const LookT& look, juce::Component& component)
        {
            target = look;
            component.repaint();
        }

        template <typename LookT>
        inline void applyPopupMenuLook(LookT& target, const LookT& look)
        {
            target = look;
        }

        inline void applyFocusGained(bool& hasFocus, juce::Component& component, bool enabled)
        {
            if (enabled && ! hasFocus)
            {
                hasFocus = true;
                component.repaint();
            }
        }

        inline void applyFocusLost(bool& hasFocus, juce::Component& component)
        {
            hasFocus = false;
            component.repaint();
        }

        inline bool shouldShowFocusRing(bool hasFocus, bool isPopupOpen)
        {
            return hasFocus || isPopupOpen;
        }

        inline void applyPopupOpened(bool& isPopupOpen, juce::Component& component)
        {
            isPopupOpen = true;
            component.repaint();
        }

        inline void applyPopupClosed(bool& isPopupOpen, juce::Component& component)
        {
            isPopupOpen = false;
            component.repaint();
            component.grabKeyboardFocus();
        }
    }
}
