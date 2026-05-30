#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace tss
{
    class ISkin;
}

class WidgetFactory;
class ParameterCell;

namespace tss
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
    BaseModulePanel(tss::ISkin& skin,
                    WidgetFactory& widgetFactory,
                    juce::AudioProcessorValueTreeState& apvts,
                    const ModulePanelConfig& config,
                    int width,
                    int height);
    ~BaseModulePanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setUiScale(float uiScale);
    
    ParameterCell* getParameterCellAt(size_t index);

protected:
    tss::ISkin* skin_;
    juce::AudioProcessorValueTreeState& apvts_;
    ModulePanelModuleType moduleType_;
    float uiScale_ = 1.0f;

    std::unique_ptr<tss::ModuleHeader> moduleHeader_;
    std::vector<std::unique_ptr<ParameterCell>> parameterCells_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseModulePanel)
};
