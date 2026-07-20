#include "PatchEditPanel.h"

#include "PatchEditTopModulesPanel/PatchEditTopModulesPanel.h"
#include "PatchEditDisplaysPanel/PatchEditDisplaysPanel.h"
#include "PatchEditBottomModulesPanel/PatchEditBottomModulesPanel.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Widgets/SectionHeader.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


PatchEditPanel::~PatchEditPanel() = default;

PatchEditPanel::PatchEditPanel(TSS::ISkin& skin,
                               const PatchEditPanelDimensions& dims,
                               const ParameterCellDimensions& parameterCellDims,
                               const ModuleHeaderDimensions& moduleHeaderDims,
                               WidgetFactory& widgetFactory,
                               juce::AudioProcessorValueTreeState& apvts)
    : dims_(dims)
    , skin_(&skin)
    , sectionHeader_(std::make_unique<TSS::SectionHeader>(
        dims_.sectionHeaderWidth,
        dims_.sectionHeaderHeight,
        TSS::sectionHeaderLookFromSkin(skin),
        PluginHelpers::getSectionDisplayName(PluginIDs::PatchEditSection::kGroupId)))
    , patchEditTopModulesPanel_(std::make_unique<PatchEditTopModulesPanel>(
        skin, dims_.topModules, dims_.width, dims_.topHeight, parameterCellDims, moduleHeaderDims, widgetFactory, apvts))
    , patchEditDisplaysPanel_(std::make_unique<PatchEditDisplaysPanel>(skin, dims_.displays, apvts))
    , patchEditBottomModulesPanel_(std::make_unique<PatchEditBottomModulesPanel>(
        skin, dims_.bottomModules, dims_.width, dims_.bottomHeight, parameterCellDims, moduleHeaderDims, widgetFactory, apvts))
{
    setOpaque(false);
    addAndMakeVisible(*sectionHeader_);
    addAndMakeVisible(*patchEditTopModulesPanel_);
    addAndMakeVisible(*patchEditDisplaysPanel_);
    addAndMakeVisible(*patchEditBottomModulesPanel_);

    setSize(dims_.width, dims_.height);
}

void PatchEditPanel::resized()
{
    const auto bounds = getLocalBounds();
    const float sf = uiScale_;

    const int sectionHeaderHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.sectionHeaderHeight), sf);
    const int topHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.topHeight), sf);
    const int middleHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.middleHeight), sf);
    const int bottomHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.bottomHeight), sf);

    int y = bounds.getY();
    sectionHeader_->setBounds(bounds.getX(), y, bounds.getWidth(), sectionHeaderHeight);
    y += sectionHeaderHeight;
    patchEditTopModulesPanel_->setBounds(bounds.getX(), y, bounds.getWidth(), topHeight);
    y += topHeight;
    patchEditDisplaysPanel_->setBounds(bounds.getX(), y, bounds.getWidth(), middleHeight);
    y += middleHeight;
    patchEditBottomModulesPanel_->setBounds(bounds.getX(), y, bounds.getWidth(), bottomHeight);
}

void PatchEditPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    sectionHeader_->setLook(TSS::sectionHeaderLookFromSkin(skin));
    TSS::propagateSkin(skin,
        patchEditTopModulesPanel_.get(),
        patchEditDisplaysPanel_.get(),
        patchEditBottomModulesPanel_.get());
}

void PatchEditPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    
    if (sectionHeader_)
        sectionHeader_->setUiScale(uiScale_);
    if (patchEditTopModulesPanel_)
        patchEditTopModulesPanel_->setUiScale(uiScale_);
    if (patchEditDisplaysPanel_)
        patchEditDisplaysPanel_->setUiScale(uiScale_);
    if (patchEditBottomModulesPanel_)
        patchEditBottomModulesPanel_->setUiScale(uiScale_);
    
    resized();
    repaint();
}
