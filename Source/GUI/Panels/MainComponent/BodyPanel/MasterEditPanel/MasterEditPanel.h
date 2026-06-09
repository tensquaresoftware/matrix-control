#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/PanelDimensions.h"

namespace TSS
{
    class ISkin;
    class SectionHeader;
}

class WidgetFactory;
class MidiPanel;
class VibratoPanel;
class MiscPanel;

class MasterEditPanel : public juce::Component
{
public:
    MasterEditPanel(TSS::ISkin& skin, const MasterEditPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~MasterEditPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

private:
    MasterEditPanelDimensions dims_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;

    std::unique_ptr<TSS::SectionHeader> sectionHeader_;
    std::unique_ptr<MidiPanel> midiPanel_;
    std::unique_ptr<VibratoPanel> vibratoPanel_;
    std::unique_ptr<MiscPanel> miscPanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterEditPanel)
};
