#pragma once

#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Core/PluginProcessor.h"
#include "MainComponent.h"
#include "GUI/Layout/PanelDimensions.h"
#include "Skins/Skin.h"

#if JUCE_DEBUG
#include "Tests/TestComponent.h"
#endif

class WidgetFactory;
class HeaderPanel;
class SettingsPanel;
class SettingsWindow;
class AboutWindow;
class MasterInitConfirmDialog;

class PluginEditor : public juce::AudioProcessorEditor,
                     private juce::ChangeListener,
                     private juce::ValueTree::Listener
{
public:
    explicit PluginEditor(PluginProcessor&);
    ~PluginEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    class HeaderRefreshTimer;

    void refreshAudioFromCombo(HeaderPanel* headerOverride = nullptr);
    void attachStandaloneAudioDeviceListener();
    void detachStandaloneAudioDeviceListener();
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    void valueTreeRedirected(juce::ValueTree&) override;

    void openSettingsWindow();
    void closeSettingsWindow();
    void openAboutWindow();
    void closeAboutWindow();
    void openMasterInitConfirmDialog(const juce::String& moduleDisplayName, std::function<void()> onConfirm);
    void closeMasterInitConfirmDialog();
    SettingsPanel* getSettingsPanelIfOpen();
    void wireSettingsPanel(SettingsPanel& panel);
    void wireHeaderPanel(HeaderPanel& headerPanel);
    void restoreSettingsPanelFromState(SettingsPanel& panel);
    void restoreHeaderPanelFromState(HeaderPanel& headerPanel);
    void updateSettingsWindowLayout(float uiScale);
    void updateAboutWindowLayout(float uiScale);
    void updateMasterInitConfirmDialogLayout(float uiScale);

    void applySkinFromItemId(int skinItemId, bool persistToState = true);
    void applyUiScaleFromItemId(int scaleId, bool persistToState = true);

    PluginProcessor& pluginProcessor;

    std::unique_ptr<TSS::Skin> skinBlack_;
    std::unique_ptr<TSS::Skin> skinCream_;
    TSS::Skin* skin_ = nullptr;
    std::unique_ptr<WidgetFactory> widgetFactory_;
    GuiLayoutDimensions layoutDimensions_;
    std::unique_ptr<MainComponent> mainComponent_;
#if JUCE_DEBUG
    std::unique_ptr<TestComponent> testComponent_;
    bool uiElementsTestVisible_ = false;
#endif
    std::unique_ptr<SettingsWindow> settingsWindow_;
    std::unique_ptr<AboutWindow> aboutWindow_;
    std::unique_ptr<MasterInitConfirmDialog> masterInitConfirmDialog_;
    float appliedUiScale_ = 1.0f;
    std::unique_ptr<HeaderRefreshTimer> headerRefreshTimer_;

    void updateSkin();
    void applyUiScale(float uiScale);
    void syncUiScaleFromEditor();
    void syncStandaloneWindowSize();
#if JUCE_DEBUG
    void setUiElementsTestVisible(bool visible);
    void layoutUiElementsTestComponent();
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
