#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;
}

class AboutPanel : public juce::Component
{
public:
    static constexpr int kDesignWidth = 440;
    static constexpr int kDesignHeight = 232;

    explicit AboutPanel(TSS::ISkin& skin);
    ~AboutPanel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

private:
    struct SpecGridLayout
    {
        int rowHeight = 0;
        int firstRowY = 0;
        juce::Rectangle<int> labelColumn;
        juce::Rectangle<int> valueColumn;
    };

    SpecGridLayout getSpecGridLayout() const;
    juce::Rectangle<int> getSpecRowBounds(int rowIndex, bool labelColumn) const;
    juce::Rectangle<int> getSpecValueRowBounds(int rowIndex) const;
    juce::String getSpecLabel(int rowIndex) const;
    juce::String getSpecValue(int rowIndex) const;
    void layoutHyperlinkButtons();

    inline constexpr static int kPadding_ = 8;
    inline constexpr static int kTitleHeight_ = 28;
    inline constexpr static int kTaglineHeight_ = 36;
    inline constexpr static int kGapAfterTitle_ = 4;
    inline constexpr static int kGapAfterTagline_ = 8;
    inline constexpr static int kSpecRowHeight_ = 18;
    inline constexpr static int kColumnGapDesign_ = 16;
    inline constexpr static int kSpecRowCount_ = 7;

    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;

    juce::HyperlinkButton emailLink_;
    juce::HyperlinkButton githubLink_;
    juce::HyperlinkButton linkedInLink_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AboutPanel)
};
