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
class ModuleHeaderPanel;
class ParameterPanel;

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
    
    ParameterPanel* getParameterPanelAt(size_t index);

protected:
    tss::ISkin* skin_;
    juce::AudioProcessorValueTreeState& apvts_;
    ModulePanelModuleType moduleType_;

    std::unique_ptr<ModuleHeaderPanel> moduleHeaderPanel_;
    std::vector<std::unique_ptr<ParameterPanel>> parameterPanels_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseModulePanel)
};
