#include "WidgetFactory.h"

#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Button.h"
#include "Shared/PluginDescriptors.h"
#include "Shared/PluginIDs.h"
#include "Shared/PluginDimensions.h"

WidgetFactory::WidgetFactory(juce::AudioProcessorValueTreeState& inApvts)
    : validator(inApvts)
{
    buildSearchMaps();
}

std::unique_ptr<tss::Slider> WidgetFactory::createSliderFromDescriptor(
    const PluginDescriptors::IntParameterDescriptor* desc,
    tss::Skin& skin,
    int width,
    int height)
{
    auto slider = std::make_unique<tss::Slider>(skin, width, height, static_cast<double>(desc->defaultValue));
    slider->setRange(static_cast<double>(desc->minValue), static_cast<double>(desc->maxValue), 1.0);
    slider->setValue(static_cast<double>(desc->defaultValue));
    return slider;
}

std::unique_ptr<tss::ComboBox> WidgetFactory::createComboBoxFromDescriptor(
    const PluginDescriptors::ChoiceParameterDescriptor* desc,
    tss::Skin& skin,
    int width,
    int height)
{
    auto comboBox = std::make_unique<tss::ComboBox>(skin, width, height);
    
    for (const auto& choice : desc->choices)
        comboBox->addItem(choice, comboBox->getNumItems() + 1);
    
    comboBox->setSelectedItemIndex(desc->defaultIndex);
    return comboBox;
}

std::unique_ptr<tss::Slider> WidgetFactory::createIntParameterSlider(
    const juce::String& parameterId,
    tss::Skin& skin)
{
    validator.throwIfParameterIdEmpty(parameterId);
    const auto* desc = findIntParameter(parameterId);
    validator.getIntParameterDescriptorOrThrow(desc, parameterId);
    validator.validateIntParameterValues(desc, parameterId);
    return createSliderFromDescriptor(
        desc,
        skin,
        PluginDimensions::Widgets::Widths::Slider::kStandard,
        PluginDimensions::Widgets::Heights::kSlider
    );
}

std::unique_ptr<tss::ComboBox> WidgetFactory::createChoiceParameterComboBox(
    const juce::String& parameterId,
    tss::Skin& skin,
    int width,
    int height)
{
    validator.throwIfParameterIdEmpty(parameterId);
    const auto* desc = findChoiceParameter(parameterId);
    validator.getChoiceParameterDescriptorOrThrow(desc, parameterId);
    validator.validateChoiceParameterValues(desc, parameterId);
    return createComboBoxFromDescriptor(desc, skin, width, height);
}

std::unique_ptr<tss::Button> WidgetFactory::createStandaloneButton(
    const juce::String& widgetId,
    tss::Skin& skin,
    int height)
{
    validator.throwIfWidgetIdEmpty(widgetId);
    const auto* desc = findStandaloneWidget(widgetId);
    validator.getStandaloneWidgetDescriptorOrThrow(desc, widgetId);
    validator.validateWidgetType(desc, widgetId);
    
    const auto buttonWidth = getButtonWidthForWidgetId(widgetId);
    
    return std::make_unique<tss::Button>(
        skin, 
        buttonWidth,
        height,
        desc->displayName
    );
}

int WidgetFactory::getButtonWidthForWidgetId(const juce::String& widgetId) const
{
    using namespace PluginDescriptors::StandaloneWidgetIds;
    
    if (widgetId == kDco1Init || widgetId == kDco2Init ||
        widgetId == kVcfVcaInit || widgetId == kFmTrackInit ||
        widgetId == kRampPortamentoInit || widgetId == kEnv1Init ||
        widgetId == kEnv2Init || widgetId == kEnv3Init ||
        widgetId == kLfo1Init || widgetId == kLfo2Init ||
        widgetId == kInitPatch)
    {
        return PluginDimensions::Widgets::Widths::Button::kInit;
    }
    
    if (widgetId == kDco1Copy || widgetId == kDco2Copy ||
        widgetId == kEnv1Copy || widgetId == kEnv2Copy ||
        widgetId == kEnv3Copy || widgetId == kLfo1Copy ||
        widgetId == kLfo2Copy || widgetId == kCopyPatch)
    {
        return PluginDimensions::Widgets::Widths::Button::kCopy;
    }
    
    if (widgetId == kDco1Paste || widgetId == kDco2Paste ||
        widgetId == kEnv1Paste || widgetId == kEnv2Paste ||
        widgetId == kEnv3Paste || widgetId == kLfo1Paste ||
        widgetId == kLfo2Paste || widgetId == kPastePatch)
    {
        return PluginDimensions::Widgets::Widths::Button::kPaste;
    }
    
    if (widgetId == kSelectBank0 || widgetId == kSelectBank1 ||
        widgetId == kSelectBank2 || widgetId == kSelectBank3 ||
        widgetId == kSelectBank4 || widgetId == kSelectBank5 ||
        widgetId == kSelectBank6 || widgetId == kSelectBank7 ||
        widgetId == kSelectBank8 || widgetId == kSelectBank9)
    {
        return PluginDimensions::Widgets::Widths::Button::kPatchManagerBankSelect;
    }
    
    if (widgetId == kLoadPreviousPatch || widgetId == kLoadNextPatch ||
        widgetId == kLoadPreviousPatchFile || widgetId == kLoadNextPatchFile ||
        widgetId == kOpenPatchFolder || widgetId == kSavePatchFile ||
        widgetId == kStorePatch)
    {
        return PluginDimensions::Widgets::Widths::Button::kInternalPatchesMemory;
    }
    
    if (widgetId == kSavePatchAs)
    {
        return PluginDimensions::Widgets::Widths::Button::kComputerPatchesSaveAs;
    }
    
    if (widgetId == kUnlockBank)
    {
        return PluginDimensions::Widgets::Widths::Button::kPatchManagerUnlockBank;
    }
    
    return PluginDimensions::Widgets::Widths::Button::kInit;
}

juce::String WidgetFactory::getParameterDisplayName(const juce::String& parameterId) const
{
    if (auto* intParam = findIntParameter(parameterId))
        return intParam->displayName;
    
    if (auto* choiceParam = findChoiceParameter(parameterId))
        return choiceParam->displayName;
    
    return juce::String();
}

juce::String WidgetFactory::getGroupDisplayName(const juce::String& groupId) const
{
    return PluginDescriptors::getGroupDisplayName(groupId);
}

juce::String WidgetFactory::getStandaloneWidgetDisplayName(const juce::String& widgetId) const
{
    if (auto* desc = findStandaloneWidget(widgetId))
        return desc->displayName;
    
    return juce::String();
}

const PluginDescriptors::IntParameterDescriptor* WidgetFactory::findIntParameter(const juce::String& parameterId) const
{
    auto it = intParameterMap.find(parameterId);
    return (it != intParameterMap.end()) ? it->second : nullptr;
}

const PluginDescriptors::ChoiceParameterDescriptor* WidgetFactory::findChoiceParameter(const juce::String& parameterId) const
{
    auto it = choiceParameterMap.find(parameterId);
    return (it != choiceParameterMap.end()) ? it->second : nullptr;
}

const PluginDescriptors::StandaloneWidgetDescriptor* WidgetFactory::findStandaloneWidget(const juce::String& widgetId) const
{
    auto it = standaloneWidgetMap.find(widgetId);
    return (it != standaloneWidgetMap.end()) ? it->second : nullptr;
}

void WidgetFactory::buildGroupMap()
{
    for (const auto& group : PluginDescriptors::kAllApvtsGroups)
        groupMap[group.groupId] = &group;
}

const PluginDescriptors::ApvtsGroupDescriptor* WidgetFactory::findGroup(const juce::String& groupId) const
{
    auto it = groupMap.find(groupId);
    return (it != groupMap.end()) ? it->second : nullptr;
}

void WidgetFactory::buildSearchMaps()
{
    buildIntParameterMap();
    buildChoiceParameterMap();
    buildStandaloneWidgetMap();
    buildGroupMap();
}

void WidgetFactory::buildIntParameterMap()
{
    addPatchEditModuleIntParametersToMap();
    addMatrixModulationBusIntParametersToMap();
    addMasterEditIntParametersToMap();
}

void WidgetFactory::buildChoiceParameterMap()
{
    addPatchEditModuleChoiceParametersToMap();
    addMatrixModulationBusChoiceParametersToMap();
    addMasterEditChoiceParametersToMap();
}

void WidgetFactory::buildStandaloneWidgetMap()
{
    addPatchEditStandaloneWidgetsToMap();
    addPatchManagerStandaloneWidgetsToMap();
    addMasterEditStandaloneWidgetsToMap();
}

void WidgetFactory::addIntParametersToMap(const std::vector<PluginDescriptors::IntParameterDescriptor>& parameters)
{
    for (const auto& param : parameters)
        intParameterMap[param.parameterId] = &param;
}

void WidgetFactory::addPatchEditModuleIntParametersToMap()
{
    addIntParametersToMap(PluginDescriptors::kDco1IntParameters);
    addIntParametersToMap(PluginDescriptors::kDco2IntParameters);
    addIntParametersToMap(PluginDescriptors::kVcfVcaIntParameters);
    addIntParametersToMap(PluginDescriptors::kFmTrackIntParameters);
    addIntParametersToMap(PluginDescriptors::kRampPortamentoIntParameters);
    addIntParametersToMap(PluginDescriptors::kEnv1IntParameters);
    addIntParametersToMap(PluginDescriptors::kEnv2IntParameters);
    addIntParametersToMap(PluginDescriptors::kEnv3IntParameters);
    addIntParametersToMap(PluginDescriptors::kLfo1IntParameters);
    addIntParametersToMap(PluginDescriptors::kLfo2IntParameters);
}

void WidgetFactory::addMatrixModulationBusIntParametersToMap()
{
    for (int bus = 0; bus < PluginDescriptors::kModulationBusCount; ++bus)
        addIntParametersToMap(PluginDescriptors::kModulationBusIntParameters[static_cast<size_t>(bus)]);
}

void WidgetFactory::addMasterEditIntParametersToMap()
{
    addIntParametersToMap(PluginDescriptors::kMasterEditIntParameters);
}

void WidgetFactory::addChoiceParametersToMap(const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& parameters)
{
    for (const auto& param : parameters)
        choiceParameterMap[param.parameterId] = &param;
}

void WidgetFactory::addPatchEditModuleChoiceParametersToMap()
{
    addChoiceParametersToMap(PluginDescriptors::kDco1ChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::kDco2ChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::kVcfVcaChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::kFmTrackChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::kRampPortamentoChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::kEnv1ChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::kEnv2ChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::kEnv3ChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::kLfo1ChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::kLfo2ChoiceParameters);
}

void WidgetFactory::addMatrixModulationBusChoiceParametersToMap()
{
    for (int bus = 0; bus < PluginDescriptors::kModulationBusCount; ++bus)
        addChoiceParametersToMap(PluginDescriptors::kModulationBusChoiceParameters[static_cast<size_t>(bus)]);
}

void WidgetFactory::addMasterEditChoiceParametersToMap()
{
    addChoiceParametersToMap(PluginDescriptors::kMasterEditChoiceParameters);
}

void WidgetFactory::addStandaloneWidgetsToMap(const std::vector<PluginDescriptors::StandaloneWidgetDescriptor>& widgets)
{
    for (const auto& widget : widgets)
        standaloneWidgetMap[widget.widgetId] = &widget;
}

void WidgetFactory::addPatchEditStandaloneWidgetsToMap()
{
    addStandaloneWidgetsToMap(PluginDescriptors::kDco1StandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::kDco2StandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::kVcfVcaStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::kFmTrackStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::kRampPortamentoStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::kEnv1StandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::kEnv2StandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::kEnv3StandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::kLfo1StandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::kLfo2StandaloneWidgets);
}

void WidgetFactory::addPatchManagerStandaloneWidgetsToMap()
{
    addStandaloneWidgetsToMap(PluginDescriptors::kBankUtilityWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::kInternalPatchesWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::kComputerPatchesWidgets);
}

void WidgetFactory::addMasterEditStandaloneWidgetsToMap()
{
    addStandaloneWidgetsToMap(PluginDescriptors::kMasterEditStandaloneWidgets);
}

