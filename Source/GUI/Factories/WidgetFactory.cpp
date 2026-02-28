#include "WidgetFactory.h"

#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Button.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/Definitions/PluginDimensions.h"

WidgetFactory::WidgetFactory(juce::AudioProcessorValueTreeState& inApvts)
    : validator(inApvts)
{
    buildSearchMaps();
}

std::unique_ptr<tss::Slider> WidgetFactory::createSliderFromDescriptor(
    const PluginDescriptors::IntParameterDescriptor* desc,
    tss::ISkin& skin,
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
    tss::ISkin& skin,
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
    tss::ISkin& skin)
{
    return createIntParameterSlider(
        parameterId,
        skin,
        PluginDimensions::Widgets::Widths::Slider::kStandard,
        PluginDimensions::Widgets::Heights::kSlider);
}

std::unique_ptr<tss::Slider> WidgetFactory::createIntParameterSlider(
    const juce::String& parameterId,
    tss::ISkin& skin,
    int width,
    int height)
{
    validator.throwIfParameterIdEmpty(parameterId);
    const auto* desc = findIntParameter(parameterId);
    validator.getIntParameterDescriptorOrThrow(desc, parameterId);
    validator.validateIntParameterValues(desc, parameterId);
    return createSliderFromDescriptor(desc, skin, width, height);
}

std::unique_ptr<tss::ComboBox> WidgetFactory::createChoiceParameterComboBox(
    const juce::String& parameterId,
    tss::ISkin& skin,
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
    tss::ISkin& skin,
    int height)
{
    validator.throwIfWidgetIdEmpty(widgetId);
    const auto* desc = findStandaloneWidget(widgetId);
    validator.getStandaloneWidgetDescriptorOrThrow(desc, widgetId);
    validator.validateWidgetType(desc, widgetId);
    
    const auto buttonWidth = desc->buttonWidth.value_or(PluginDimensions::Widgets::Widths::Button::kInit);
    
    return std::make_unique<tss::Button>(
        skin, 
        buttonWidth,
        height,
        desc->displayName
    );
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
    return PluginHelpers::getGroupDisplayName(groupId);
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
    addMatrixModulationStandaloneWidgetsToMap();
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
    addIntParametersToMap(PluginDescriptors::PatchEditSection::Dco1Module::kIntParameters);
    addIntParametersToMap(PluginDescriptors::PatchEditSection::Dco2Module::kIntParameters);
    addIntParametersToMap(PluginDescriptors::PatchEditSection::VcfVcaModule::kIntParameters);
    addIntParametersToMap(PluginDescriptors::PatchEditSection::FmTrackModule::kIntParameters);
    addIntParametersToMap(PluginDescriptors::PatchEditSection::RampPortamentoModule::kIntParameters);
    addIntParametersToMap(PluginDescriptors::PatchEditSection::Envelope1Module::kIntParameters);
    addIntParametersToMap(PluginDescriptors::PatchEditSection::Envelope2Module::kIntParameters);
    addIntParametersToMap(PluginDescriptors::PatchEditSection::Envelope3Module::kIntParameters);
    addIntParametersToMap(PluginDescriptors::PatchEditSection::Lfo1Module::kIntParameters);
    addIntParametersToMap(PluginDescriptors::PatchEditSection::Lfo2Module::kIntParameters);
}

void WidgetFactory::addMatrixModulationBusIntParametersToMap()
{
    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        addIntParametersToMap(PluginDescriptors::MatrixModulationSection::kModulationBusIntParameters[static_cast<size_t>(bus)]);
}

void WidgetFactory::addMasterEditIntParametersToMap()
{
    addIntParametersToMap(PluginDescriptors::MasterEditSection::kIntParameters);
}

void WidgetFactory::addChoiceParametersToMap(const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& parameters)
{
    for (const auto& param : parameters)
        choiceParameterMap[param.parameterId] = &param;
}

void WidgetFactory::addPatchEditModuleChoiceParametersToMap()
{
    addChoiceParametersToMap(PluginDescriptors::PatchEditSection::Dco1Module::kChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::PatchEditSection::Dco2Module::kChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::PatchEditSection::VcfVcaModule::kChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::PatchEditSection::FmTrackModule::kChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::PatchEditSection::RampPortamentoModule::kChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::PatchEditSection::Envelope1Module::kChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::PatchEditSection::Envelope2Module::kChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::PatchEditSection::Envelope3Module::kChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::PatchEditSection::Lfo1Module::kChoiceParameters);
    addChoiceParametersToMap(PluginDescriptors::PatchEditSection::Lfo2Module::kChoiceParameters);
}

void WidgetFactory::addMatrixModulationBusChoiceParametersToMap()
{
    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        addChoiceParametersToMap(PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[static_cast<size_t>(bus)]);
}

void WidgetFactory::addMasterEditChoiceParametersToMap()
{
    addChoiceParametersToMap(PluginDescriptors::MasterEditSection::kChoiceParameters);
}

void WidgetFactory::addStandaloneWidgetsToMap(const std::vector<PluginDescriptors::StandaloneWidgetDescriptor>& widgets)
{
    for (const auto& widget : widgets)
        standaloneWidgetMap[widget.widgetId] = &widget;
}

void WidgetFactory::addPatchEditStandaloneWidgetsToMap()
{
    addStandaloneWidgetsToMap(PluginDescriptors::PatchEditSection::Dco1Module::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchEditSection::Dco2Module::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchEditSection::VcfVcaModule::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchEditSection::FmTrackModule::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchEditSection::RampPortamentoModule::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchEditSection::Envelope1Module::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchEditSection::Envelope2Module::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchEditSection::Envelope3Module::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchEditSection::Lfo1Module::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchEditSection::Lfo2Module::kStandaloneWidgets);
}

void WidgetFactory::addMatrixModulationStandaloneWidgetsToMap()
{
    addStandaloneWidgetsToMap(PluginDescriptors::MatrixModulationSection::kStandaloneWidgets);
    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        addStandaloneWidgetsToMap(PluginDescriptors::MatrixModulationSection::kModulationBusStandaloneWidgets[static_cast<size_t>(bus)]);
}

void WidgetFactory::addPatchManagerStandaloneWidgetsToMap()
{
    addStandaloneWidgetsToMap(PluginDescriptors::PatchManagerSection::BankUtilityModule::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchManagerSection::InternalPatchesModule::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchManagerSection::ComputerPatchesModule::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchManagerSection::PatchMutatorModule::kStandaloneWidgets);
}

void WidgetFactory::addMasterEditStandaloneWidgetsToMap()
{
    addStandaloneWidgetsToMap(PluginDescriptors::MasterEditSection::kStandaloneWidgets);
}

