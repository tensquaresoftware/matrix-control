#include "PatchEditPanel.h"

#include <vector>

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
#include "GUI/Layout/Design/Design.h"
#include "GUI/Factories/WidgetFactory.h"


PatchEditPanel::~PatchEditPanel() = default;

PatchEditPanel::PatchEditPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , topPanelHeight_(TSS::Design::Panels::Body::PatchEditSection::TopModules::kHeight)
    , middlePanelHeight_(TSS::Design::Panels::Body::PatchEditSection::MiddleModules::kHeight)
    , bottomPanelHeight_(TSS::Design::Panels::Body::PatchEditSection::BottomModules::kHeight)
    , skin_(&skin)
    , sectionHeader_(std::make_unique<TSS::SectionHeader>(
        TSS::Design::PanelWidgets::Widths::SectionHeader::kPatchEdit,
        TSS::Design::Atoms::Heights::kSectionHeader,
        TSS::sectionHeaderLookFromSkin(skin),
        PluginHelpers::getSectionDisplayName(PluginIDs::PatchEditSection::kGroupId)))
    , patchEditTopModulesPanel_(std::make_unique<PatchEditTopModulesPanel>(skin, width, topPanelHeight_, widgetFactory, apvts))
    , patchEditDisplaysPanel_(std::make_unique<PatchEditDisplaysPanel>(skin, width, middlePanelHeight_, apvts))
    , patchEditBottomModulesPanel_(std::make_unique<PatchEditBottomModulesPanel>(skin, width, bottomPanelHeight_, widgetFactory, apvts))
{
    setOpaque(false);
    addAndMakeVisible(*sectionHeader_);
    addAndMakeVisible(*patchEditTopModulesPanel_);
    addAndMakeVisible(*patchEditDisplaysPanel_);
    addAndMakeVisible(*patchEditBottomModulesPanel_);

    if (patchEditDisplaysPanel_ != nullptr && patchEditTopModulesPanel_ != nullptr && patchEditBottomModulesPanel_ != nullptr)
    {
        patchEditDisplaysPanel_->connectSliderFastPaths(*patchEditTopModulesPanel_, *patchEditBottomModulesPanel_);
    }

    setSize(width_, height_);
}

void PatchEditPanel::resized()
{
    const auto bounds = getLocalBounds();
    const float sf = uiScale_;

    const std::vector<int> designHeights {
        TSS::Design::Atoms::Heights::kSectionHeader,
        topPanelHeight_,
        middlePanelHeight_,
        bottomPanelHeight_
    };
    const auto heights = TSS::ScaledLayout::distributeHeights(bounds.getHeight(), designHeights, sf, 3);

    int y = bounds.getY();
    sectionHeader_->setBounds(bounds.getX(), y, bounds.getWidth(), heights[0]);
    y += heights[0];
    patchEditTopModulesPanel_->setBounds(bounds.getX(), y, bounds.getWidth(), heights[1]);
    y += heights[1];
    patchEditDisplaysPanel_->setBounds(bounds.getX(), y, bounds.getWidth(), heights[2]);
    y += heights[2];
    patchEditBottomModulesPanel_->setBounds(bounds.getX(), y, bounds.getWidth(), heights[3]);
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
