#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"
#include "GUI/Widgets/RadioButtonGroupLayout.h"

namespace TSS
{
    class ISkin;

    /** Exclusive square Matrix options that wrap inside a given width. */
    class RadioButtonGroup : public juce::Component
    {
    public:
        RadioButtonGroup();
        ~RadioButtonGroup() override = default;

        void setSkin(ISkin& skin);
        void setUiScale(float uiScale);
        void setOptions(const juce::StringArray& labels);
        void setSelectedIndex(int index, juce::NotificationType notification = juce::sendNotification);
        int getSelectedIndex() const noexcept { return selectedIndex_; }
        int getOptionCount() const noexcept { return static_cast<int>(options_.size()); }
        int getPreferredHeight(int availableWidth) const noexcept;

        std::function<void()> onSelectionChanged;

        void resized() override;
        void paint(juce::Graphics& g) override;

    private:
        struct Look
        {
            juce::Colour squareOff{};
            juce::Colour squareOn{};
            juce::Colour border{};
            juce::Colour borderDisabled{};
            juce::Colour text{};
            juce::Colour textDisabled{};
            juce::Font font{ juce::FontOptions{} };
        };

        class OptionButton : public juce::Button
        {
        public:
            OptionButton(const juce::String& text);

            void setLook(const Look& look);
            void setUiScale(float uiScale);
            void setSelected(bool isSelected);

            void paintButton(juce::Graphics& g,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown) override;

        private:
            inline constexpr static int kSquareDesign_ = 12;
            inline constexpr static int kSquareTextGapDesign_ = 4;
            inline constexpr static int kBorderThickness_ = 2;

            Look look_{};
            float uiScale_ = 1.0f;
            bool selected_ = false;
        };

        void rebuildButtons();
        void applyLooks();
        Look lookFromSkin(ISkin& skin) const;
        RadioButtonGroupLayout::OptionMetrics optionMetrics() const noexcept;

        ISkin* skin_ = nullptr;
        float uiScale_ = 1.0f;
        int selectedIndex_ = -1;
        juce::StringArray labels_;
        std::vector<std::unique_ptr<OptionButton>> options_;
        Look look_{};

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RadioButtonGroup)
    };
}
