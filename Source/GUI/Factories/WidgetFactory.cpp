#include "WidgetFactory.h"

#include "DimensionFactory.h"
#include "GUI/Layout/WidgetDimensionRegistry.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Looks/LookBuilders.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Layout/Design/Design.h"

WidgetFactory::WidgetFactory(juce::AudioProcessorValueTreeState& inApvts)
    : validator(inApvts)
{
    buildSearchMaps();
}

std::unique_ptr<TSS::Slider> WidgetFactory::createSliderFromDescriptor(
    const PluginDescriptors::IntParameterDescriptor* desc,
    TSS::ISkin& skin,
    int width,
    int height)
{
    auto slider = std::make_unique<TSS::Slider>(
        width,
        height,
        TSS::sliderLookFromSkin(skin),
        TSS::SliderConfig{
            static_cast<double>(desc->minValue),
            static_cast<double>(desc->maxValue),
            static_cast<double>(desc->defaultValue),
            1.0,
            {},
            {},
            {},
            {}});
    return slider;
}

std::unique_ptr<TSS::ComboBox> WidgetFactory::createComboBoxFromDescriptor(
    const PluginDescriptors::ChoiceParameterDescriptor* desc,
    TSS::ISkin& skin,
    int width,
    int height)
{
    auto comboBox = std::make_unique<TSS::ComboBox>(width, height, TSS::comboBoxLookFromSkin(skin));
    comboBox->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));

    for (const auto& choice : desc->choices)
        comboBox->addItem(choice, comboBox->getNumItems() + 1);
    
    comboBox->setSelectedItemIndex(desc->defaultIndex);
    return comboBox;
}

std::unique_ptr<TSS::Slider> WidgetFactory::createIntParameterSlider(
    const juce::String& parameterId,
    TSS::ISkin& skin)
{
    return createIntParameterSlider(
        parameterId,
        skin,
        TSS::Design::Recipes::Slider::kStandard,
        TSS::Design::Atoms::Heights::kSlider);
}

std::unique_ptr<TSS::Slider> WidgetFactory::createIntParameterSlider(
    const juce::String& parameterId,
    TSS::ISkin& skin,
    int width,
    int height)
{
    validator.throwIfParameterIdEmpty(parameterId);
    const auto* desc = findIntParameter(parameterId);
    validator.getIntParameterDescriptorOrThrow(desc, parameterId);
    validator.validateIntParameterValues(desc, parameterId);
    return createSliderFromDescriptor(desc, skin, width, height);
}

std::unique_ptr<TSS::ComboBox> WidgetFactory::createChoiceParameterComboBox(
    const juce::String& parameterId,
    TSS::ISkin& skin,
    int width,
    int height)
{
    validator.throwIfParameterIdEmpty(parameterId);
    const auto* desc = findChoiceParameter(parameterId);
    validator.getChoiceParameterDescriptorOrThrow(desc, parameterId);
    validator.validateChoiceParameterValues(desc, parameterId);
    return createComboBoxFromDescriptor(desc, skin, width, height);
}

std::unique_ptr<TSS::Button> WidgetFactory::createStandaloneButton(
    const juce::String& widgetId,
    TSS::ISkin& skin,
    int height)
{
    validator.throwIfWidgetIdEmpty(widgetId);
    const auto* desc = findStandaloneWidget(widgetId);
    validator.getStandaloneWidgetDescriptorOrThrow(desc, widgetId);
    validator.validateWidgetType(desc, widgetId);
    
    const auto buttonWidth = validator.resolveStandaloneButtonWidthOrThrow(widgetId);

    auto button = std::make_unique<TSS::Button>(
        buttonWidth,
        height,
        TSS::buttonLookFromSkin(skin),
        desc->displayName);
    return button;
}

PluginEditorDimensions WidgetFactory::getRootGuiDimensions()
{
    return DimensionFactory::buildPluginEditorDimensions();
}

GuiLayoutDimensions WidgetFactory::buildGuiLayoutDimensions()
{
    return DimensionFactory::buildGuiLayoutDimensions();
}

std::optional<juce::String> WidgetFactory::getParameterDisplayName(const juce::String& parameterId) const
{
    if (auto* intParam = findIntParameter(parameterId))
        return intParam->displayName;
    
    if (auto* choiceParam = findChoiceParameter(parameterId))
        return choiceParam->displayName;
    
    return std::nullopt;
}

juce::String WidgetFactory::getGroupDisplayName(const juce::String& groupId) const
{
    return PluginHelpers::getGroupDisplayName(groupId);
}

std::optional<juce::String> WidgetFactory::getStandaloneWidgetDisplayName(const juce::String& widgetId) const
{
    if (auto* desc = findStandaloneWidget(widgetId))
        return desc->displayName;
    
    return std::nullopt;
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
    addAllPatchEditDescriptorsToMap();
    addAllMatrixModulationDescriptorsToMap();
    addAllMasterEditDescriptorsToMap();
}

void WidgetFactory::buildChoiceParameterMap()
{
    addAllPatchEditDescriptorsToMap();
    addAllMatrixModulationDescriptorsToMap();
    addAllMasterEditDescriptorsToMap();
}

void WidgetFactory::buildStandaloneWidgetMap()
{
    addAllPatchEditDescriptorsToMap();
    addAllMatrixModulationDescriptorsToMap();
    addAllPatchManagerDescriptorsToMap();
    addAllMasterEditDescriptorsToMap();
}

void WidgetFactory::addIntParametersToMap(const std::vector<PluginDescriptors::IntParameterDescriptor>& parameters)
{
    for (const auto& param : parameters)
        intParameterMap[param.parameterId] = &param;
}

void WidgetFactory::addAllPatchEditDescriptorsToMap()
{
    const std::vector<const std::vector<PluginDescriptors::IntParameterDescriptor>*> intParamSets = {
        &PluginDescriptors::PatchEditSection::Dco1Module::kIntParameters,
        &PluginDescriptors::PatchEditSection::Dco2Module::kIntParameters,
        &PluginDescriptors::PatchEditSection::VcfVcaModule::kIntParameters,
        &PluginDescriptors::PatchEditSection::FmTrackModule::kIntParameters,
        &PluginDescriptors::PatchEditSection::RampPortamentoModule::kIntParameters,
        &PluginDescriptors::PatchEditSection::Envelope1Module::kIntParameters,
        &PluginDescriptors::PatchEditSection::Envelope2Module::kIntParameters,
        &PluginDescriptors::PatchEditSection::Envelope3Module::kIntParameters,
        &PluginDescriptors::PatchEditSection::Lfo1Module::kIntParameters,
        &PluginDescriptors::PatchEditSection::Lfo2Module::kIntParameters
    };
    for (const auto* params : intParamSets)
        addIntParametersToMap(*params);
    
    const std::vector<const std::vector<PluginDescriptors::ChoiceParameterDescriptor>*> choiceParamSets = {
        &PluginDescriptors::PatchEditSection::Dco1Module::kChoiceParameters,
        &PluginDescriptors::PatchEditSection::Dco2Module::kChoiceParameters,
        &PluginDescriptors::PatchEditSection::VcfVcaModule::kChoiceParameters,
        &PluginDescriptors::PatchEditSection::FmTrackModule::kChoiceParameters,
        &PluginDescriptors::PatchEditSection::RampPortamentoModule::kChoiceParameters,
        &PluginDescriptors::PatchEditSection::Envelope1Module::kChoiceParameters,
        &PluginDescriptors::PatchEditSection::Envelope2Module::kChoiceParameters,
        &PluginDescriptors::PatchEditSection::Envelope3Module::kChoiceParameters,
        &PluginDescriptors::PatchEditSection::Lfo1Module::kChoiceParameters,
        &PluginDescriptors::PatchEditSection::Lfo2Module::kChoiceParameters
    };
    for (const auto* params : choiceParamSets)
        addChoiceParametersToMap(*params);
    
    const std::vector<const std::vector<PluginDescriptors::StandaloneWidgetDescriptor>*> standaloneWidgetSets = {
        &PluginDescriptors::PatchEditSection::Dco1Module::kStandaloneWidgets,
        &PluginDescriptors::PatchEditSection::Dco2Module::kStandaloneWidgets,
        &PluginDescriptors::PatchEditSection::VcfVcaModule::kStandaloneWidgets,
        &PluginDescriptors::PatchEditSection::FmTrackModule::kStandaloneWidgets,
        &PluginDescriptors::PatchEditSection::RampPortamentoModule::kStandaloneWidgets,
        &PluginDescriptors::PatchEditSection::Envelope1Module::kStandaloneWidgets,
        &PluginDescriptors::PatchEditSection::Envelope2Module::kStandaloneWidgets,
        &PluginDescriptors::PatchEditSection::Envelope3Module::kStandaloneWidgets,
        &PluginDescriptors::PatchEditSection::Lfo1Module::kStandaloneWidgets,
        &PluginDescriptors::PatchEditSection::Lfo2Module::kStandaloneWidgets
    };
    for (const auto* widgets : standaloneWidgetSets)
        addStandaloneWidgetsToMap(*widgets);
}

void WidgetFactory::addAllMatrixModulationDescriptorsToMap()
{
    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
    {
        addIntParametersToMap(PluginDescriptors::MatrixModulationSection::kModulationBusIntParameters[static_cast<size_t>(bus)]);
        addChoiceParametersToMap(PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[static_cast<size_t>(bus)]);
    }
    
    addStandaloneWidgetsToMap(PluginDescriptors::MatrixModulationSection::kStandaloneWidgets);
    for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        addStandaloneWidgetsToMap(PluginDescriptors::MatrixModulationSection::kModulationBusStandaloneWidgets[static_cast<size_t>(bus)]);
}

void WidgetFactory::addAllPatchManagerDescriptorsToMap()
{
    addStandaloneWidgetsToMap(PluginDescriptors::PatchManagerSection::BankUtilityModule::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchManagerSection::InternalPatchesModule::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchManagerSection::ComputerPatchesModule::kStandaloneWidgets);
    addStandaloneWidgetsToMap(PluginDescriptors::PatchManagerSection::PatchMutatorModule::kStandaloneWidgets);
}

void WidgetFactory::addAllMasterEditDescriptorsToMap()
{
    addIntParametersToMap(PluginDescriptors::MasterEditSection::kIntParameters);
    addChoiceParametersToMap(PluginDescriptors::MasterEditSection::kChoiceParameters);
    addStandaloneWidgetsToMap(PluginDescriptors::MasterEditSection::kStandaloneWidgets);
}

void WidgetFactory::addChoiceParametersToMap(const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& parameters)
{
    for (const auto& param : parameters)
        choiceParameterMap[param.parameterId] = &param;
}

void WidgetFactory::addStandaloneWidgetsToMap(const std::vector<PluginDescriptors::StandaloneWidgetDescriptor>& widgets)
{
    for (const auto& widget : widgets)
        standaloneWidgetMap[widget.widgetId] = &widget;
}

