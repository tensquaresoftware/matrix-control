#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/PanelDimensions.h"

namespace Core
{
    class PatchFileService;
}

namespace TSS
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
    PatchManagerPanel(TSS::ISkin& skin,
                      const PatchManagerPanelDimensions& dims,
                      WidgetFactory& widgetFactory,
                      juce::AudioProcessorValueTreeState& apvts,
                      const Core::PatchFileService& patchFileService);
    ~PatchManagerPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

private:
    PatchManagerPanelDimensions dims_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;

    std::unique_ptr<TSS::SectionHeader> sectionHeader_;
    std::unique_ptr<BankUtilityPanel> bankUtilityPanel_;
    std::unique_ptr<InternalPatchesPanel> internalPatchesPanel_;
    std::unique_ptr<ComputerPatchesPanel> computerPatchesPanel_;
    std::unique_ptr<PatchMutatorPanel> patchMutatorPanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchManagerPanel)
};
