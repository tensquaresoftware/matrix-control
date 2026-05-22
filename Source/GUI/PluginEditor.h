#pragma once

#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Core/PluginProcessor.h"
#include "MainComponent.h"
#include "Shared/Definitions/PluginDesignDimensions.h"
#include "Skins/Skin.h"
#include "Tests/TestComponent.h"

class WidgetFactory;

class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor(PluginProcessor&);
    ~PluginEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;

    static int getDesignWidth() { return PluginDesignDimensions::GUI::kWidth; }
    static int getDesignHeight() { return PluginDesignDimensions::GUI::kHeight; }

private:
    PluginProcessor& pluginProcessor;

    std::unique_ptr<tss::Skin> skinBlack_;
    std::unique_ptr<tss::Skin> skinCream_;
    tss::Skin* skin_ = nullptr;
    std::unique_ptr<WidgetFactory> widgetFactory_;
    std::unique_ptr<MainComponent> mainComponent_;
    std::unique_ptr<TestComponent> testComponent_;
    bool uiElementsTestVisible_ = false;

    void updateSkin();
    void applyUiScale(float uiScale);
    void syncUiScaleFromEditor();
    void setUiElementsTestVisible(bool visible);
    void layoutUiElementsTestComponent();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
