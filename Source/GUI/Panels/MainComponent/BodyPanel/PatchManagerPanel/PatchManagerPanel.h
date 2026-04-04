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
class BankUtilityPanel;
class InternalPatchesPanel;
class ComputerPatchesPanel;
class PatchMutatorPanel;

class PatchManagerPanel : public juce::Component
{
public:
    PatchManagerPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~PatchManagerPanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setDisplayScale(float displayScale);

private:
    int width_;
    int height_;
    int bankUtilityPanelHeight_;
    int internalPatchesPanelHeight_;
    int computerPatchesPanelHeight_;
    int patchMutatorPanelHeight_;
    tss::ISkin* skin_;
    float displayScale_ = 1.0f;

    std::unique_ptr<tss::SectionHeader> sectionHeader_;
    std::unique_ptr<BankUtilityPanel> bankUtilityPanel_;
    std::unique_ptr<InternalPatchesPanel> internalPatchesPanel_;
    std::unique_ptr<ComputerPatchesPanel> computerPatchesPanel_;
    std::unique_ptr<PatchMutatorPanel> patchMutatorPanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchManagerPanel)
};

