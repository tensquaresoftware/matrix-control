#pragma once

#include <functional>

#include "GUI/Widgets/Label.h"

namespace TSS
{
    class ISkin;

    class Logo : public Label,
                 private juce::Timer
    {
    public:
        explicit Logo(ISkin& skin, int width, int height);
        ~Logo() override = default;

        void setSkin(ISkin& skin);
        void setHighlighted(bool highlighted);

        std::function<void()> onPopupRequested;
        std::function<void()> onSettingsRequested;
        std::function<void()> onUiTestsToggleRequested;
        std::function<void()> onUiScaleReset;

        void mouseUp(const juce::MouseEvent& e) override;
        void mouseDoubleClick(const juce::MouseEvent& e) override;
        void mouseEnter(const juce::MouseEvent& e) override;
        void mouseExit(const juce::MouseEvent& e) override;

    private:
        void timerCallback() override;
        void applyTextColour();

        ISkin* skin_ = nullptr;
        bool isHighlighted_ = false;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Logo)
    };
}
