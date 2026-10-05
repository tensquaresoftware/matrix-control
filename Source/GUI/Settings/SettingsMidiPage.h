#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Helpers/ContextualHelpBinder.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Led.h"

namespace TSS
{
    class ISkin;
}

namespace Core
{
    class MidiActivityTracker;
}

/** Settings MIDI page: port combos + activity LEDs (plugin omits KEYBOARD FROM). */
class SettingsMidiPage : public juce::Component,
                         private juce::Timer
{
public:
    struct Config
    {
        TSS::ISkin* skin = nullptr;
        bool isPluginMode = false;
        std::function<const Core::MidiActivityTracker&()> activityTrackerProvider;
        std::function<void()> onPortListsRefreshRequested;
    };

    explicit SettingsMidiPage(Config config);
    ~SettingsMidiPage() override;

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    void setVisible(bool shouldBeVisible) override;
    void setMonitoringActive(bool shouldBeActive);

    void populatePortLists(const juce::String& keepOpenSynthFromId,
                           const juce::String& keepOpenSynthToId,
                           const juce::String& keepOpenKeyboardFromId);
    void refreshPortLists() { populatePortLists({}, {}, {}); }

    juce::String getSelectedSynthFromPortId() const;
    juce::String getSelectedSynthToPortId() const;
    juce::String getSelectedKeyboardFromPortId() const;

    void selectSynthFromPort(const juce::String& portId);
    void selectSynthToPort(const juce::String& portId);
    void selectKeyboardFromPort(const juce::String& portId);

    TSS::ComboBox& getSynthFromCombo() { return *synthFromCombo_; }
    TSS::ComboBox& getSynthToCombo() { return *synthToCombo_; }
    TSS::ComboBox* getKeyboardFromCombo() noexcept { return keyboardFromCombo_.get(); }

    bool isAnyPortPopupOpen() const noexcept;

    void registerContextualHelp(TSS::ContextualHelpBinder& binder);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void buildWidgets();
    void wirePopupRefresh();
    void updateKeyboardFromVisibility();

    std::unique_ptr<TSS::Label> makeLabel(TSS::ISkin& skin, const juce::String& text);
    std::unique_ptr<TSS::ComboBox> makeCombo(TSS::ISkin& skin);

    Config config_;
    TSS::ISkin* skin_ = nullptr;
    float uiScale_ = 1.0f;
    bool monitoringActive_ = false;

    std::unique_ptr<TSS::Label> keyboardFromLabel_;
    std::unique_ptr<TSS::ComboBox> keyboardFromCombo_;
    std::unique_ptr<TSS::Led> keyboardFromLed_;
    std::unique_ptr<TSS::Label> synthFromLabel_;
    std::unique_ptr<TSS::ComboBox> synthFromCombo_;
    std::unique_ptr<TSS::Led> synthFromLed_;
    std::unique_ptr<TSS::Label> synthToLabel_;
    std::unique_ptr<TSS::ComboBox> synthToCombo_;
    std::unique_ptr<TSS::Led> synthToLed_;

    std::vector<juce::String> keyboardFromPortIdentifiers_;
    std::vector<juce::String> synthFromPortIdentifiers_;
    std::vector<juce::String> synthToPortIdentifiers_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsMidiPage)
};
