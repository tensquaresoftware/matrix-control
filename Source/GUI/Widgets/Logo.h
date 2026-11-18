#pragma once

#include <functional>

#include "GUI/Widgets/Label.h"

namespace TSS
{
    class ISkin;

    class Logo : public Label
    {
    public:
        explicit Logo(ISkin& skin, int width, int height);
        ~Logo() override = default;

        void setSkin(ISkin& skin);
        void setHighlighted(bool highlighted);

        std::function<void()> onPopupRequested;
        std::function<void()> onSettingsRequested;
        std::function<void()> onUiScaleReset;

        void mouseUp(const juce::MouseEvent& e) override;
        void mouseDoubleClick(const juce::MouseEvent& e) override;
        void mouseEnter(const juce::MouseEvent& e) override;
        void mouseExit(const juce::MouseEvent& e) override;
        void paint(juce::Graphics& g) override;

    private:
        void applyTextColour();
        void requestPopupOpen();
        void cancelPendingPopupOpen();

        ISkin* skin_ = nullptr;
        bool isHighlighted_ = false;
        int popupShowGeneration_ = 0;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Logo)
    };
}
