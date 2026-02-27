#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/PluginDimensions.h"

namespace tss
{
    class Skin;
}

class WidgetFactory;
class Dco1Panel;
class Dco2Panel;
class VcfVcaPanel;
class FmTrackPanel;
class RampPortamentoPanel;

class TopPanel : public juce::Component
{
public:
    TopPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~TopPanel() override;

    void resized() override;
    void setSkin(tss::Skin& skin);
    
    FmTrackPanel* getFmTrackPanel() { return fmTrackPanel_.get(); }

    static int getWidth() { return PluginDimensions::Panels::Body::PatchEdit::Top::kWidth; }
    static int getHeight() { return PluginDimensions::Panels::Body::PatchEdit::Top::kHeight; }
    static int getSpacing() { return PluginDimensions::Panels::Body::kPadding; }

private:

    tss::Skin* skin_;

    std::unique_ptr<Dco1Panel> dco1Panel_;
    std::unique_ptr<Dco2Panel> dco2Panel_;
    std::unique_ptr<VcfVcaPanel> vcfVcaPanel_;
    std::unique_ptr<FmTrackPanel> fmTrackPanel_;
    std::unique_ptr<RampPortamentoPanel> rampPortamentoPanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopPanel)
};

