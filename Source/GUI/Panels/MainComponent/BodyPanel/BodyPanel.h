#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace tss
{
    class ISkin;
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
    BodyPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~BodyPanel() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setDisplayScale(float displayScale);

private:
    int width_;
    int height_;
    int padding_;
    int patchEditPanelWidth_;
    int patchEditPanelHeight_;
    int matrixModulationPanelWidth_;
    int matrixModulationPanelHeight_;
    int patchManagerPanelWidth_;
    int patchManagerPanelHeight_;
    int masterEditPanelWidth_;
    int masterEditPanelHeight_;
    tss::ISkin* skin_;
    float displayScale_ = 1.0f;

    std::unique_ptr<PatchEditPanel> patchEditPanel_;
    std::unique_ptr<tss::VerticalSeparator> verticalSeparator1_;
    std::unique_ptr<MatrixModulationPanel> matrixModulationPanel_;
    std::unique_ptr<PatchManagerPanel> patchManagerPanel_;
    std::unique_ptr<tss::VerticalSeparator> verticalSeparator2_;
    std::unique_ptr<MasterEditPanel> masterEditPanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BodyPanel)
};

