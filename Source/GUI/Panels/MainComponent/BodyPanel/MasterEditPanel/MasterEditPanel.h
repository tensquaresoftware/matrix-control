#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/PluginDimensions.h"

namespace tss
{
    class Skin;
    class SectionHeader;
}

class WidgetFactory;
class MidiPanel;
class VibratoPanel;
class MiscPanel;

class MasterEditPanel : public juce::Component
{
public:
    MasterEditPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~MasterEditPanel() override;

    void resized() override;
    void setSkin(tss::Skin& skin);

    static int getWidth() { return PluginDimensions::Panels::Body::MasterEditSection::kWidth; }
    static int getHeight() { return PluginDimensions::Panels::Body::MasterEditSection::kHeight; }

private:
    tss::Skin* skin_;

    std::unique_ptr<tss::SectionHeader> sectionHeader_;
    std::unique_ptr<MidiPanel> midiPanel_;
    std::unique_ptr<VibratoPanel> vibratoPanel_;
    std::unique_ptr<MiscPanel> miscPanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterEditPanel)
};

