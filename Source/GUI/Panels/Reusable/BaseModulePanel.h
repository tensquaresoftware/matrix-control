#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class WidgetFactory;
class ParameterCell;

namespace TSS
{
    class ModuleHeader;
}

enum class ModulePanelButtonSet { InitCopyPaste, InitOnly };
enum class ModulePanelModuleType { PatchEdit, MasterEdit };
enum class ModulePanelParameterType { Slider, ComboBox, None };

struct ModulePanelConfig
{
    juce::String moduleId;
    ModulePanelButtonSet buttonSet;
    ModulePanelModuleType moduleType;
    juce::String initWidgetId;
    juce::String copyWidgetId;
    juce::String pasteWidgetId;
    
    struct ParameterConfig
    {
        juce::String parameterId;
        ModulePanelParameterType parameterType;
    };
    
    std::vector<ParameterConfig> parameters;
};

class BaseModulePanel : public juce::Component
{
public:
    BaseModulePanel(TSS::ISkin& skin,
                    WidgetFactory& widgetFactory,
                    juce::AudioProcessorValueTreeState& apvts,
                    const ModulePanelConfig& config,
                    int width,
                    int height,
                    const ModuleHeaderDimensions& moduleHeaderDims,
                    const ParameterCellDimensions& parameterCellDims);
    ~BaseModulePanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    
    ParameterCell* getParameterCellAt(size_t index);

protected:
    TSS::ISkin* skin_;
    juce::AudioProcessorValueTreeState& apvts_;
    ModulePanelModuleType moduleType_;
    ModuleHeaderDimensions moduleHeaderDims_;
    ParameterCellDimensions parameterCellDims_;
    float uiScale_ = 1.0f;

    std::unique_ptr<TSS::ModuleHeader> moduleHeader_;
    std::vector<std::unique_ptr<ParameterCell>> parameterCells_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseModulePanel)
};
