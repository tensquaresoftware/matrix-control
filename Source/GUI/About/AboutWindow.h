#pragma once

#include <functional>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;
}

class AboutPanel;

class AboutCloseButton : public juce::Button
{
public:
    AboutCloseButton();
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    TSS::ISkin* skin_ = nullptr;
    float uiScale_ = 1.0f;
    juce::Path crossShape_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AboutCloseButton)
};

class AboutWindow : public juce::Component
{
public:
    AboutWindow(TSS::ISkin& skin, std::function<void()> onCloseRequested);
    ~AboutWindow() override;

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    int getBorderThickness() const;
    juce::Rectangle<int> getDialogBounds() const;

    std::function<void()> onCloseRequested_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;

    AboutCloseButton closeButton_;
    std::unique_ptr<AboutPanel> aboutPanel_;

    inline constexpr static int kTitleBarHeight_ = 28;
    inline constexpr static int kBorderThickness_ = 4;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AboutWindow)
};
