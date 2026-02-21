#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Core/PluginProcessor.h"
#include "Shared/PluginDimensions.h"
#include "Themes/Skin.h"
#include "Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "Panels/MainComponent/BodyPanel/BodyPanel.h"
#include "Panels/MainComponent/FooterPanel/FooterPanel.h"
#include "MainComponent.h"

class WidgetFactory;

class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor(PluginProcessor&);
    ~PluginEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;

    static int getWidth() { return PluginDimensions::GUI::kWidth; }
    static int getHeight() { return PluginDimensions::GUI::kHeight; }

private:
    PluginProcessor& pluginProcessor;
    
    std::unique_ptr<tss::Skin> skinBlack_;
    std::unique_ptr<tss::Skin> skinCream_;
    tss::Skin* skin_ = nullptr;
    std::unique_ptr<WidgetFactory> widgetFactory;
    std::unique_ptr<MainComponent> mainComponent;
    
    void updateSkin();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};