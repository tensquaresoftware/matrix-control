#pragma once

#include <memory>
#include <map>
#include <vector>
#include <optional>

#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/PanelDimensions.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "WidgetFactoryValidator.h"

namespace TSS
{
    class ISkin;
    class Slider;
    class ComboBox;
    class Button;
}

class WidgetFactory
{
public:
    explicit WidgetFactory(juce::AudioProcessorValueTreeState& inApvts);

    // ============================================================================
    // Widget Creation Methods
    // ============================================================================
    
    std::unique_ptr<TSS::Slider> createIntParameterSlider(
        const juce::String& parameterId,
        TSS::ISkin& skin);

    std::unique_ptr<TSS::Slider> createIntParameterSlider(
        const juce::String& parameterId,
        TSS::ISkin& skin,
        int width,
        int height);
    
    std::unique_ptr<TSS::ComboBox> createChoiceParameterComboBox(
        const juce::String& parameterId,
        TSS::ISkin& skin,
        int width,
        int height);
    
    std::unique_ptr<TSS::Button> createStandaloneButton(
        const juce::String& widgetId,
        TSS::ISkin& skin,
        int height);

    static PluginEditorDimensions getRootGuiDimensions();
    static GuiLayoutDimensions buildGuiLayoutDimensions();

    // ============================================================================
    // Display Name Helper Methods
    // ============================================================================
    
    std::optional<juce::String> getParameterDisplayName(const juce::String& parameterId) const;
    juce::String getGroupDisplayName(const juce::String& groupId) const;
    std::optional<juce::String> getStandaloneWidgetDisplayName(const juce::String& widgetId) const;

private:
    WidgetFactoryValidator validator;
    
    std::map<juce::String, const PluginDescriptors::IntParameterDescriptor*> intParameterMap;
    std::map<juce::String, const PluginDescriptors::ChoiceParameterDescriptor*> choiceParameterMap;
    std::map<juce::String, const PluginDescriptors::StandaloneWidgetDescriptor*> standaloneWidgetMap;
    std::map<juce::String, const PluginDescriptors::ApvtsGroupDescriptor*> groupMap;
    
    void buildSearchMaps();
    void buildIntParameterMap();
    void buildChoiceParameterMap();
    void buildStandaloneWidgetMap();
    void buildGroupMap();
    
    void addAllPatchEditDescriptorsToMap();
    void addAllMatrixModulationDescriptorsToMap();
    void addAllPatchManagerDescriptorsToMap();
    void addAllMasterEditDescriptorsToMap();
    
    const PluginDescriptors::IntParameterDescriptor* findIntParameter(const juce::String& parameterId) const;
    const PluginDescriptors::ChoiceParameterDescriptor* findChoiceParameter(const juce::String& parameterId) const;
    const PluginDescriptors::StandaloneWidgetDescriptor* findStandaloneWidget(const juce::String& widgetId) const;
    const PluginDescriptors::ApvtsGroupDescriptor* findGroup(const juce::String& groupId) const;
    
    void addIntParametersToMap(const std::vector<PluginDescriptors::IntParameterDescriptor>& parameters);
    void addChoiceParametersToMap(const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& parameters);
    void addStandaloneWidgetsToMap(const std::vector<PluginDescriptors::StandaloneWidgetDescriptor>& widgets);
    
    std::unique_ptr<TSS::Slider> createSliderFromDescriptor(const PluginDescriptors::IntParameterDescriptor* desc, TSS::ISkin& skin, int width, int height);
    std::unique_ptr<TSS::ComboBox> createComboBoxFromDescriptor(const PluginDescriptors::ChoiceParameterDescriptor* desc, TSS::ISkin& skin, int width, int height);
};

