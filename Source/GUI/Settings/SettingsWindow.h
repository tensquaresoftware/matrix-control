#pragma once

#include <functional>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;
}

class SettingsPanel;

class SettingsCloseButton : public juce::Button
{
public:
    SettingsCloseButton();
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    TSS::ISkin* skin_ = nullptr;
    float uiScale_ = 1.0f;
    juce::Path crossShape_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsCloseButton)
};

class SettingsWindow : public juce::Component
{
public:
    SettingsWindow(TSS::ISkin& skin,
                   bool isPluginMode,
                   std::function<void(SettingsPanel&)> onPanelReady,
                   std::function<void()> onCloseRequested);
    ~SettingsWindow() override = default;

    SettingsPanel& getSettingsPanel() { return *settingsPanel_; }

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void paint(juce::Graphics& g) override;
    void resized() override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    int getBorderThickness() const;
    juce::Rectangle<int> getDialogBounds() const;

    std::function<void()> onCloseRequested_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;

    SettingsCloseButton closeButton_;
    std::unique_ptr<SettingsPanel> settingsPanel_;

    inline constexpr static int kTitleBarHeight_ = 28;
    inline constexpr static int kBorderThickness_ = 4;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsWindow)
};
