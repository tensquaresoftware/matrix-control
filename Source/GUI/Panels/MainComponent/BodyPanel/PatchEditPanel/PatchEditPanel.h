#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace tss
{
    class ISkin;
    class SectionHeader;
}

class WidgetFactory;
class PatchEditTopModulesPanel;
class PatchEditDisplaysPanel;
class PatchEditBottomModulesPanel;

class PatchEditPanel : public juce::Component
{
public:
    PatchEditPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~PatchEditPanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setUiScale(float uiScale);

private:
    int width_;
    int height_;
    int topPanelHeight_;
    int middlePanelHeight_;
    int bottomPanelHeight_;
    tss::ISkin* skin_;
    float uiScale_ = 1.0f;

    std::unique_ptr<tss::SectionHeader> sectionHeader_;
    std::unique_ptr<PatchEditTopModulesPanel> patchEditTopModulesPanel_;
    std::unique_ptr<PatchEditDisplaysPanel> patchEditDisplaysPanel_;
    std::unique_ptr<PatchEditBottomModulesPanel> patchEditBottomModulesPanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchEditPanel)
};
