#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace TSS
{
    enum class LabelStyle
    {
        Default,
        HeaderPanel
    };

    class Label : public juce::Component
    {
    public:
        explicit Label(int width, int height, const LabelLook& look,
                       const juce::String& text = juce::String(),
                       LabelStyle style = LabelStyle::Default);
        ~Label() override = default;
        
        void setLook(const LabelLook& look);
        void setUiScale(float uiScale);
        
        void setText(const juce::String& text);
        juce::String getText() const { return labelText_; }

        void paint(juce::Graphics& g) override;
        
        static int getBaseWidth() { return kDefaultWidth_; }
        static int getBaseHeight() { return kDefaultHeight_; }

    private:
        static constexpr int kDefaultWidth_ = 50;
        static constexpr int kDefaultHeight_ = 20;
        static constexpr int kTextLeftPadding_ = 0;

        LabelLook look_{};
        int width_;
        int height_;
        juce::String labelText_;
        LabelStyle style_;
        float uiScale_ = 1.0f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Label)
    };
}
