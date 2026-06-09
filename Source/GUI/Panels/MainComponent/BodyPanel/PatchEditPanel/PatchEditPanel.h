#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/PanelDimensions.h"
#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
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
    PatchEditPanel(TSS::ISkin& skin,
                   const PatchEditPanelDimensions& dims,
                   const ParameterCellDimensions& parameterCellDims,
                   const ModuleHeaderDimensions& moduleHeaderDims,
                   WidgetFactory& widgetFactory,
                   juce::AudioProcessorValueTreeState& apvts);
    ~PatchEditPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

private:
    PatchEditPanelDimensions dims_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;

    std::unique_ptr<TSS::SectionHeader> sectionHeader_;
    std::unique_ptr<PatchEditTopModulesPanel> patchEditTopModulesPanel_;
    std::unique_ptr<PatchEditDisplaysPanel> patchEditDisplaysPanel_;
    std::unique_ptr<PatchEditBottomModulesPanel> patchEditBottomModulesPanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchEditPanel)
};
