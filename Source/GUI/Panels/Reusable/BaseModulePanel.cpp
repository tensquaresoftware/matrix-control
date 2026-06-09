#include "BaseModulePanel.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/ParameterCell.h"
#include "GUI/Factories/WidgetFactory.h"

BaseModulePanel::BaseModulePanel(TSS::ISkin& skin,
                                 WidgetFactory& widgetFactory,
                                 juce::AudioProcessorValueTreeState& apvts,
                                 const ModulePanelConfig& config,
                                 int width,
                                 int height,
                                 const ModuleHeaderDimensions& moduleHeaderDims,
                                 const ParameterCellDimensions& parameterCellDims)
    : skin_(&skin)
    , apvts_(apvts)
    , moduleType_(config.moduleType)
    , moduleHeaderDims_(moduleHeaderDims)
    , parameterCellDims_(parameterCellDims)
{
    setOpaque(false);
    
    const auto buttonSet = (config.buttonSet == ModulePanelButtonSet::InitCopyPaste)
        ? TSS::ModuleHeader::ButtonSet::InitCopyPaste
        : TSS::ModuleHeader::ButtonSet::InitOnly;
    
    const auto columnLayout = (config.moduleType == ModulePanelModuleType::PatchEdit)
        ? TSS::ModuleHeader::ColumnLayout::PatchEdit
        : TSS::ModuleHeader::ColumnLayout::MasterEdit;
    
    const TSS::ModuleHeader::WithActionsSpec spec {
        skin,
        widgetFactory,
        apvts,
        moduleHeaderDims,
        config.moduleId,
        columnLayout,
        buttonSet,
        config.initWidgetId,
        config.copyWidgetId,
        config.pasteWidgetId
    };
    moduleHeader_ = std::make_unique<TSS::ModuleHeader>(spec);
    addAndMakeVisible(*moduleHeader_);

    for (const auto& paramConfig : config.parameters)
    {
        ParameterCell::ParameterType paramType = ParameterCell::ParameterType::None;
        if (paramConfig.parameterType == ModulePanelParameterType::Slider)
            paramType = ParameterCell::ParameterType::Slider;
        else if (paramConfig.parameterType == ModulePanelParameterType::ComboBox)
            paramType = ParameterCell::ParameterType::ComboBox;

        ParameterCell::ModuleType modType = (config.moduleType == ModulePanelModuleType::PatchEdit)
            ? ParameterCell::ModuleType::PatchEdit
            : ParameterCell::ModuleType::MasterEdit;

        parameterCells_.push_back(std::make_unique<ParameterCell>(
            skin,
            widgetFactory,
            paramConfig.parameterId,
            paramType,
            modType,
            apvts_,
            parameterCellDims));
        addAndMakeVisible(*parameterCells_.back());
    }

    setSize(width, height);
}

BaseModulePanel::~BaseModulePanel() = default;

void BaseModulePanel::resized()
{
    auto bounds = getLocalBounds();

    if (auto* header = moduleHeader_.get())
    {
        const int headerHeight = TSS::ScaledLayout::scaledInt(
            static_cast<float>(moduleHeaderDims_.height), uiScale_);
        header->setBounds(bounds.removeFromTop(headerHeight));
    }

    const size_t paramCount = parameterCells_.size();
    if (paramCount == 0)
        return;

    const int designRowTotal = parameterCellDims_.rowHeight;
    const auto rowHeights = TSS::ScaledLayout::distributeFixedDesignRowsWithRemainderAtBottom(
        bounds.getHeight(), paramCount, designRowTotal, uiScale_);

    int y = bounds.getY();
    for (size_t i = 0; i < paramCount; ++i)
    {
        if (auto* cell = parameterCells_[i].get())
            cell->setBounds(bounds.getX(), y, bounds.getWidth(), rowHeights[i]);
        y += rowHeights[i];
    }
}

void BaseModulePanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    TSS::propagateSkin(skin, moduleHeader_.get());

    for (auto& cell : parameterCells_)
        TSS::propagateSkin(skin, cell.get());
}

void BaseModulePanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    
    if (moduleHeader_)
        moduleHeader_->setUiScale(uiScale_);
    
    for (auto& cell : parameterCells_)
    {
        if (cell)
            cell->setUiScale(uiScale_);
    }
    
    resized();
    repaint();
}

ParameterCell* BaseModulePanel::getParameterCellAt(size_t index)
{
    if (index >= parameterCells_.size())
        return nullptr;

    return parameterCells_[index].get();
}
