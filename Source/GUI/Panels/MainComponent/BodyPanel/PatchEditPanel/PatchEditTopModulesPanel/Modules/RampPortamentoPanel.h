#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Helpers/StrigUnisonGateHelper.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class WidgetFactory;

class RampPortamentoPanel : public BaseModulePanel,
                            private juce::AudioProcessorValueTreeState::Listener,
                            private juce::ValueTree::Listener
{
public:
    struct Config
    {
        TSS::ISkin& skin;
        int width = 0;
        int height = 0;
        WidgetFactory& widgetFactory;
        juce::AudioProcessorValueTreeState& apvts;
        const ModuleHeaderDimensions& moduleHeaderDims;
        const ParameterCellDimensions& parameterCellDims;
    };

    explicit RampPortamentoPanel(const Config& config);
    ~RampPortamentoPanel() override;

    void resized() override;

    static ModulePanelLayout createLayout();

private:
    class MasterOverrideBadge;

    static constexpr int kKeyboardModeCellIndex = 8;
    static constexpr int kLegatoPortaCellIndex = 7;
    static constexpr int kRamp1TriggerCellIndex = 1;
    static constexpr int kRamp2TriggerCellIndex = 3;
    static constexpr int kRampTriggerStrigIndex = 0;

    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    void refreshLegatoPortaGraying();
    void refreshStrigGates(bool clearIfCurrentStrig);
    void refreshMasterOverrideBadge();
    void layoutMasterOverrideBadge();
    void showMasterOverrideFooter();

    bool legatoPortaGrayed_ = false;
    std::unique_ptr<MasterOverrideBadge> masterOverrideBadge_;
    std::unique_ptr<TSS::StrigUnisonGateHelper::KeyboardModeChangeListener> keyboardModeListener_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RampPortamentoPanel)
};
