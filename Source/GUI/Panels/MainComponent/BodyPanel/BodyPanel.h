#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/PluginDimensions.h"

namespace tss
{
    class Skin;
    class VerticalSeparator;
}

class PatchEditPanel;
class MatrixModulationPanel;
class PatchManagerPanel;
class MasterEditPanel;
class WidgetFactory;

class BodyPanel : public juce::Component
{
public:
    BodyPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~BodyPanel() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setSkin(tss::Skin& skin);

    static int getHeight() { return PluginDimensions::Panels::BodyPanel::kHeight; }
    static int getPadding() { return PluginDimensions::Panels::BodyPanel::kPadding; }

private:
    tss::Skin* skin_;

    std::unique_ptr<PatchEditPanel> patchEditPanel_;
    std::unique_ptr<tss::VerticalSeparator> verticalSeparator1_;
    std::unique_ptr<MatrixModulationPanel> matrixModulationPanel_;
    std::unique_ptr<PatchManagerPanel> patchManagerPanel_;
    std::unique_ptr<tss::VerticalSeparator> verticalSeparator2_;
    std::unique_ptr<MasterEditPanel> masterEditPanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BodyPanel)
};

