#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;

    class Logo : public juce::Component,
                 private juce::Timer
    {
    public:
        Logo();
        ~Logo() override = default;

        void setSkin(ISkin& skin);
        void setUiScale(float uiScale);
        void setHighlighted(bool highlighted);

        int getPreferredWidth() const;

        std::function<void()> onPopupRequested;
        std::function<void()> onUiScaleReset;

        void paint(juce::Graphics& g) override;
        void mouseUp(const juce::MouseEvent& e) override;
        void mouseDoubleClick(const juce::MouseEvent& e) override;
        void mouseEnter(const juce::MouseEvent& e) override;
        void mouseExit(const juce::MouseEvent& e) override;

    private:
        void timerCallback() override;

        ISkin* skin_ = nullptr;
        float uiScale_ = 1.0f;
        bool isHighlighted_ = false;
        juce::String logoText_;

        juce::Font getScaledFont() const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Logo)
    };
}
