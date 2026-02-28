#include "BaseModulePanel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Panels/Reusable/ModuleHeaderPanel.h"
#include "GUI/Panels/Reusable/ParameterPanel.h"
#include "GUI/Factories/WidgetFactory.h"

BaseModulePanel::BaseModulePanel(tss::ISkin& skin,
                                 WidgetFactory& widgetFactory,
                                 juce::AudioProcessorValueTreeState& apvts,
                                 const ModulePanelConfig& config,
                                 int width,
                                 int height)
    : skin_(&skin)
    , apvts_(apvts)
    , moduleType_(config.moduleType)
{
    setOpaque(false);
    
    ModuleHeaderPanel::ButtonSet buttonSet = (config.buttonSet == ModulePanelButtonSet::InitCopyPaste)
        ? ModuleHeaderPanel::ButtonSet::InitCopyPaste
        : ModuleHeaderPanel::ButtonSet::InitOnly;
    
    ModuleHeaderPanel::ModuleType moduleType = (config.moduleType == ModulePanelModuleType::PatchEdit)
        ? ModuleHeaderPanel::ModuleType::PatchEdit
        : ModuleHeaderPanel::ModuleType::MasterEdit;
    
    moduleHeaderPanel_ = std::make_unique<ModuleHeaderPanel>(
        skin,
        widgetFactory,
        config.moduleId,
        buttonSet,
        moduleType,
        apvts_,
        config.initWidgetId,
        config.copyWidgetId,
        config.pasteWidgetId);
    addAndMakeVisible(*moduleHeaderPanel_);

    for (const auto& paramConfig : config.parameters)
    {
        ParameterPanel::ParameterType paramType = ParameterPanel::ParameterType::None;
        if (paramConfig.parameterType == ModulePanelParameterType::Slider)
            paramType = ParameterPanel::ParameterType::Slider;
        else if (paramConfig.parameterType == ModulePanelParameterType::ComboBox)
            paramType = ParameterPanel::ParameterType::ComboBox;
        
        ParameterPanel::ModuleType modType = (config.moduleType == ModulePanelModuleType::PatchEdit)
            ? ParameterPanel::ModuleType::PatchEdit
            : ParameterPanel::ModuleType::MasterEdit;
        
        parameterPanels_.push_back(std::make_unique<ParameterPanel>(
            skin,
            widgetFactory,
            paramConfig.parameterId,
            paramType,
            modType,
            apvts_));
        addAndMakeVisible(*parameterPanels_.back());
    }

    setSize(width, height);
}

BaseModulePanel::~BaseModulePanel() = default;

void BaseModulePanel::resized()
{
    auto bounds = getLocalBounds();

    if (auto* header = moduleHeaderPanel_.get())
        header->setBounds(bounds.removeFromTop(header->getHeight()));

    for (auto& paramPanel : parameterPanels_)
        if (paramPanel != nullptr)
            paramPanel->setBounds(bounds.removeFromTop(paramPanel->getTotalHeight()));
}

void BaseModulePanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    tss::propagateSkin(skin, moduleHeaderPanel_.get());

    for (auto& paramPanel : parameterPanels_)
        tss::propagateSkin(skin, paramPanel.get());
}

ParameterPanel* BaseModulePanel::getParameterPanelAt(size_t index)
{
    if (index >= parameterPanels_.size())
        return nullptr;
    
    return parameterPanels_[index].get();
}
