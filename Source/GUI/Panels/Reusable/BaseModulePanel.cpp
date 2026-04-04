#include "BaseModulePanel.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Panels/Reusable/ModuleHeaderPanel.h"
#include "GUI/Panels/Reusable/ParameterPanel.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginDimensions.h"

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
    {
        const int headerHeight = tss::ScaledLayout::scaledInt(static_cast<float>(ModuleHeaderPanel::getHeight()), displayScale_);
        header->setBounds(bounds.removeFromTop(headerHeight));
    }

    const size_t paramCount = parameterPanels_.size();
    if (paramCount == 0)
        return;

    const int designRowTotal = PluginDimensions::Widgets::Heights::kLabel + PluginDimensions::Widgets::Heights::kHorizontalSeparator;
    const auto rowHeights = tss::ScaledLayout::distributeFixedDesignRowsWithRemainderAtBottom(
        bounds.getHeight(), paramCount, designRowTotal, displayScale_);

    int y = bounds.getY();
    for (size_t i = 0; i < paramCount; ++i)
    {
        if (auto* panel = parameterPanels_[i].get())
            panel->setBounds(bounds.getX(), y, bounds.getWidth(), rowHeights[i]);
        y += rowHeights[i];
    }
}

void BaseModulePanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    tss::propagateSkin(skin, moduleHeaderPanel_.get());

    for (auto& paramPanel : parameterPanels_)
        tss::propagateSkin(skin, paramPanel.get());
}

void BaseModulePanel::setDisplayScale(float displayScale)
{
    if (juce::approximatelyEqual(displayScale_, displayScale))
        return;
    
    displayScale_ = displayScale;
    
    if (moduleHeaderPanel_)
        moduleHeaderPanel_->setDisplayScale(displayScale_);
    
    for (auto& paramPanel : parameterPanels_)
    {
        if (paramPanel)
            paramPanel->setDisplayScale(displayScale_);
    }
    
    resized();
    repaint();
}

ParameterPanel* BaseModulePanel::getParameterPanelAt(size_t index)
{
    if (index >= parameterPanels_.size())
        return nullptr;
    
    return parameterPanels_[index].get();
}
