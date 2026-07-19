#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/PanelDimensions.h"
#include "GUI/Widgets/ModuleHeader.h"

namespace TSS
{
    class ISkin;
    class SectionHeader;
}

class WidgetFactory;
class MidiPanel;
class VibratoPanel;
class MiscPanel;

class MasterEditPanel : public juce::Component,
                        public juce::ValueTree::Listener
{
public:
    MasterEditPanel(TSS::ISkin& skin, const MasterEditPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~MasterEditPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    void mouseDown(const juce::MouseEvent& event) override;

    void setInitConfirmationGate(TSS::ModuleHeader::InitConfirmationGate gate);

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    void valueTreeRedirected(juce::ValueTree&) override;

private:
    MasterEditPanelDimensions dims_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    juce::AudioProcessorValueTreeState& apvts_;
    bool masterEditGrayed_ = false;

    std::unique_ptr<TSS::SectionHeader> sectionHeader_;
    std::unique_ptr<MidiPanel> midiPanel_;
    std::unique_ptr<VibratoPanel> vibratoPanel_;
    std::unique_ptr<MiscPanel> miscPanel_;

    void refreshDeviceGating();
    void setMasterEditGrayed(bool grayed);
    void showMatrix6PatchOnlyFooterMessage();
    void applyGrayedToChild(juce::Component* child, bool grayed);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterEditPanel)
};
