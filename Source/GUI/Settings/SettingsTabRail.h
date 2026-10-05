#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"
#include "Shared/Definitions/PluginIDs.h"

namespace TSS
{
    class ISkin;
}

class SettingsTabRail : public juce::Component
{
public:
    SettingsTabRail(TSS::ISkin& skin, bool isPluginMode, std::function<void(int)> onTabSelected);
    ~SettingsTabRail() override = default;

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    void setSelectedTab(int tabId);
    int getSelectedTab() const noexcept { return selectedTabId_; }

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    class TabButton : public juce::Button
    {
    public:
        TabButton(const juce::String& text);

        void setLook(const TSS::LabelLook& look);
        void setSelected(bool isSelected);
        void setUiScale(float uiScale);
        void setSelectedFill(juce::Colour fill, juce::Colour text);

        void paintButton(juce::Graphics& g,
                         bool shouldDrawButtonAsHighlighted,
                         bool shouldDrawButtonAsDown) override;

    private:
        TSS::LabelLook look_{};
        juce::Colour selectedFill_{};
        juce::Colour selectedText_{};
        float uiScale_ = 1.0f;
        bool selected_ = false;
    };

    void rebuildLooks();

    TSS::ISkin* skin_ = nullptr;
    float uiScale_ = 1.0f;
    bool isPluginMode_ = false;
    int selectedTabId_ = 1;
    std::function<void(int)> onTabSelected_;
    std::vector<std::unique_ptr<TabButton>> tabs_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsTabRail)
};
