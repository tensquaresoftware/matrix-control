#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace tss
{
    enum class LabelStyle
    {
        Default,
        HeaderPanel
    };

    class Label : public juce::Component
    {
    public:
        explicit Label(int width, int height, const juce::String& text = juce::String(),
                      LabelStyle style = LabelStyle::Default);
        ~Label() override = default;
        
        void setLook(const LabelLook& look);
        void setScalingFactor(float scalingFactor);
        
        void setText(const juce::String& text);
        juce::String getText() const { return labelText_; }

        void paint(juce::Graphics& g) override;
        
        static int getBaseWidth() { return kDefaultWidth_; }
        static int getBaseHeight() { return kDefaultHeight_; }

    private:
        static constexpr int kDefaultWidth_ = 50;
        static constexpr int kDefaultHeight_ = 20;
        static constexpr int kTextLeftPadding_ = 2;
        static constexpr float kFontSize_ = 14.0f;

        LabelLook look_{};
        int width_;
        int height_;
        juce::String labelText_;
        LabelStyle style_;
        float scalingFactor_ = 1.0f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Label)
    };
}
