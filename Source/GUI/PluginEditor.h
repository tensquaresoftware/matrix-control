#pragma once

#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Core/PluginProcessor.h"
#include "MainComponent.h"
#include "GUI/Layout/PanelDimensions.h"
#include "Skins/Skin.h"
#include "Tests/TestComponent.h"

class WidgetFactory;
class HeaderPanel;
class SettingsPanel;
class SettingsWindow;

class PluginEditor : public juce::AudioProcessorEditor,
                     private juce::ChangeListener
{
public:
    explicit PluginEditor(PluginProcessor&);
    ~PluginEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;

private:
    class HeaderRefreshTimer;

    void refreshAudioFromCombo();
    void attachStandaloneAudioDeviceListener();
    void detachStandaloneAudioDeviceListener();
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void openSettingsWindow();
    void closeSettingsWindow();
    SettingsPanel* getSettingsPanelIfOpen();
    void wireSettingsPanel(SettingsPanel& panel);
    void wireHeaderPanel(HeaderPanel& headerPanel);
    void restoreSettingsPanelFromState(SettingsPanel& panel);
    void restoreHeaderPanelFromState(HeaderPanel& headerPanel);
    void updateSettingsWindowLayout(float uiScale);

    void applySkinFromItemId(int skinItemId, bool persistToState = true);
    void applyUiScaleFromItemId(int scaleId, bool persistToState = true);

    PluginProcessor& pluginProcessor;

    std::unique_ptr<TSS::Skin> skinBlack_;
    std::unique_ptr<TSS::Skin> skinCream_;
    TSS::Skin* skin_ = nullptr;
    std::unique_ptr<WidgetFactory> widgetFactory_;
    GuiLayoutDimensions layoutDimensions_;
    std::unique_ptr<MainComponent> mainComponent_;
    std::unique_ptr<TestComponent> testComponent_;
    std::unique_ptr<SettingsWindow> settingsWindow_;
    bool uiElementsTestVisible_ = false;
    std::unique_ptr<HeaderRefreshTimer> headerRefreshTimer_;

    void updateSkin();
    void applyUiScale(float uiScale);
    void syncUiScaleFromEditor();
    void setUiElementsTestVisible(bool visible);
    void layoutUiElementsTestComponent();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
