#include "PatchManagerPanel.h"

#include <vector>

#include "GUI/Layout/ScaledLayout.h"
#include "Modules/BankUtilityPanel.h"
#include "Modules/InternalPatchesPanel.h"
#include "Modules/ComputerPatchesPanel.h"
#include "Modules/PatchMutatorPanel.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Widgets/SectionHeader.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "GUI/Layout/Design/Design.h"
#include "GUI/Factories/WidgetFactory.h"


PatchManagerPanel::PatchManagerPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , bankUtilityPanelHeight_(TSS::Design::Panels::Body::PatchManagerSection::BankUtilityModule::kHeight)
    , internalPatchesPanelHeight_(TSS::Design::Panels::Body::PatchManagerSection::InternalPatchesModule::kHeight)
    , computerPatchesPanelHeight_(TSS::Design::Panels::Body::PatchManagerSection::ComputerPatchesModule::kHeight)
    , patchMutatorPanelHeight_(TSS::Design::Panels::Body::PatchManagerSection::PatchMutatorModule::kHeight)
    , skin_(&skin)
    , sectionHeader_(std::make_unique<TSS::SectionHeader>(
        TSS::Design::PanelWidgets::Widths::SectionHeader::kPatchManager,
        TSS::Design::Atoms::Heights::kSectionHeader,
        TSS::sectionHeaderLookFromSkin(skin),
        PluginHelpers::getSectionDisplayName(PluginIDs::PatchManagerSection::kGroupId),
        TSS::SectionHeader::ColourVariant::Blue))
    , bankUtilityPanel_(std::make_unique<BankUtilityPanel>(skin,
        TSS::Design::Panels::Body::PatchManagerSection::kWidth, bankUtilityPanelHeight_, widgetFactory, apvts))
    , internalPatchesPanel_(std::make_unique<InternalPatchesPanel>(skin,
        TSS::Design::Panels::Body::PatchManagerSection::kWidth, internalPatchesPanelHeight_, widgetFactory, apvts))
    , computerPatchesPanel_(std::make_unique<ComputerPatchesPanel>(skin,
        TSS::Design::Panels::Body::PatchManagerSection::kWidth, computerPatchesPanelHeight_, widgetFactory, apvts))
    , patchMutatorPanel_(std::make_unique<PatchMutatorPanel>(skin,
        TSS::Design::Panels::Body::PatchManagerSection::kWidth, patchMutatorPanelHeight_, widgetFactory, apvts))
{
    setOpaque(false);
    addAndMakeVisible(*sectionHeader_);
    addAndMakeVisible(*bankUtilityPanel_);
    addAndMakeVisible(*internalPatchesPanel_);
    addAndMakeVisible(*computerPatchesPanel_);
    addAndMakeVisible(*patchMutatorPanel_);

    setSize(width_, height_);
}

PatchManagerPanel::~PatchManagerPanel() = default;

void PatchManagerPanel::resized()
{
    const auto bounds = getLocalBounds();
    const float sf = uiScale_;
    const int panelWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(width_), sf);

    const int sectionHeaderHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(TSS::Design::Atoms::Heights::kSectionHeader), sf);
    const int contentTop = bounds.getY() + sectionHeaderHeight;
    const int contentHeight = bounds.getHeight() - sectionHeaderHeight;

    const std::vector<int> moduleDesignHeights {
        bankUtilityPanelHeight_,
        internalPatchesPanelHeight_,
        computerPatchesPanelHeight_,
        patchMutatorPanelHeight_
    };
    const auto moduleHeights = TSS::ScaledLayout::distributeHeights(contentHeight, moduleDesignHeights, sf, 3);

    if (auto* header = sectionHeader_.get())
        header->setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), sectionHeaderHeight);

    int y = contentTop;
    if (auto* panel = bankUtilityPanel_.get())
    {
        panel->setBounds(bounds.getX(), y, panelWidth, moduleHeights[0]);
        y += moduleHeights[0];
    }
    if (auto* panel = internalPatchesPanel_.get())
    {
        panel->setBounds(bounds.getX(), y, panelWidth, moduleHeights[1]);
        y += moduleHeights[1];
    }
    if (auto* panel = computerPatchesPanel_.get())
    {
        panel->setBounds(bounds.getX(), y, panelWidth, moduleHeights[2]);
        y += moduleHeights[2];
    }
    if (auto* panel = patchMutatorPanel_.get())
        panel->setBounds(bounds.getX(), y, panelWidth, moduleHeights[3]);
}

void PatchManagerPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    sectionHeader_->setLook(TSS::sectionHeaderLookFromSkin(skin));
    TSS::propagateSkin(skin,
        bankUtilityPanel_.get(),
        internalPatchesPanel_.get(),
        computerPatchesPanel_.get(),
        patchMutatorPanel_.get());
}

void PatchManagerPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    
    if (sectionHeader_)
        sectionHeader_->setUiScale(uiScale_);
    if (bankUtilityPanel_)
        bankUtilityPanel_->setUiScale(uiScale_);
    if (internalPatchesPanel_)
        internalPatchesPanel_->setUiScale(uiScale_);
    if (computerPatchesPanel_)
        computerPatchesPanel_->setUiScale(uiScale_);
    if (patchMutatorPanel_)
        patchMutatorPanel_->setUiScale(uiScale_);
    
    resized();
    repaint();
}

