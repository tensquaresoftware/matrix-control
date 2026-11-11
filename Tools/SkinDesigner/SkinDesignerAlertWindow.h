#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "Tools/SkinDesigner/SkinDesignerChromeTokens.h"

namespace SkinDesignerChrome
{
    /**
     * AlertWindow that re-packs action buttons with kButtonGap (GUI chrome gap)
     * instead of JUCE's hardcoded 16 px spacer in AlertWindow::updateLayout.
     */
    class AlertWindow final : public juce::AlertWindow
    {
    public:
        using juce::AlertWindow::AlertWindow;

    private:
        void childBoundsChanged(juce::Component* child) override
        {
            if (reflowing_ || dynamic_cast<juce::TextButton*>(child) == nullptr)
                return;

            reflowing_ = true;
            reflowButtons();
            reflowing_ = false;
        }

        void reflowButtons()
        {
            const int n = getNumButtons();
            if (n < 1)
                return;

            juce::Array<juce::TextButton*> buttons;
            buttons.ensureStorageAllocated(n);
            for (int i = 0; i < n; ++i)
            {
                auto* button = dynamic_cast<juce::TextButton*>(getButton(i));
                if (button == nullptr)
                    return;
                buttons.add(button);
            }

            int totalWidth = 0;
            for (auto* button : buttons)
                totalWidth += button->getWidth();
            if (n > 1)
                totalWidth += kButtonGap * (n - 1);

            int x = (getWidth() - totalWidth) / 2;
            // Bottom air = 2× inter-button gap (JUCE used ~5% of height).
            const int y = getHeight() - buttons.getFirst()->getHeight() - (kButtonGap * 2);
            for (auto* button : buttons)
            {
                button->setTopLeftPosition(x, y);
                x += button->getWidth() + kButtonGap;
            }
        }

        bool reflowing_ = false;
    };

    /** Same button order as LookAndFeel_V2, with Skin Designer AlertWindow packing. */
    std::unique_ptr<juce::AlertWindow> makeAlertWindowForOptions(
        const juce::MessageBoxOptions& options);
}
