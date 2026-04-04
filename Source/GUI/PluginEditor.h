#pragma once

#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Core/PluginProcessor.h"
#include "Shared/Definitions/PluginDimensions.h"
#include "Skins/Skin.h"
#include "Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "Panels/MainComponent/BodyPanel/BodyPanel.h"
#include "Panels/MainComponent/FooterPanel/FooterPanel.h"
#include "MainComponent.h"

class WidgetFactory;

// High-frequency Timer polls editor width so layout scale tracks live resize smoothly.
// (AsyncUpdater coalesced too aggressively and made corner-drag scaling feel stepped.)
class PluginEditor : public juce::AudioProcessorEditor,
                    private juce::Timer
{
public:
    explicit PluginEditor(PluginProcessor&);
    ~PluginEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;

    static int getDesignWidth() { return PluginDimensions::GUI::kWidth; }
    static int getDesignHeight() { return PluginDimensions::GUI::kHeight; }

private:
    PluginProcessor& pluginProcessor;
    
    std::unique_ptr<tss::Skin> skinBlack_;
    std::unique_ptr<tss::Skin> skinCream_;
    tss::Skin* skin_ = nullptr;
    std::unique_ptr<WidgetFactory> widgetFactory;
    std::unique_ptr<MainComponent> mainComponent;
    std::unique_ptr<juce::ResizableCornerComponent> resizeCorner_;

    void layoutResizeCorner();
    void updateSkin();
    void applyGuiScale(float scaleFactor);
    void syncLayoutScaleFromEditor();
    void timerCallback() override;

    int lastSyncedEditorWidth_ = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};