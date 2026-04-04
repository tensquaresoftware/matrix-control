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
#include "Shared/Definitions/PluginDimensions.h"
#include "GUI/Factories/WidgetFactory.h"


PatchManagerPanel::PatchManagerPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , bankUtilityPanelHeight_(PluginDimensions::Panels::Body::PatchManagerSection::BankUtilityModule::kHeight)
    , internalPatchesPanelHeight_(PluginDimensions::Panels::Body::PatchManagerSection::InternalPatchesModule::kHeight)
    , computerPatchesPanelHeight_(PluginDimensions::Panels::Body::PatchManagerSection::ComputerPatchesModule::kHeight)
    , patchMutatorPanelHeight_(PluginDimensions::Panels::Body::PatchManagerSection::PatchMutatorModule::kHeight)
    , skin_(&skin)
    , sectionHeader_(std::make_unique<tss::SectionHeader>(
        PluginDimensions::Widgets::Widths::SectionHeader::kPatchManager,
        PluginDimensions::Widgets::Heights::kSectionHeader,
        PluginHelpers::getSectionDisplayName(PluginIDs::PatchManagerSection::kGroupId),
        tss::SectionHeader::ColourVariant::Blue))
    , bankUtilityPanel_(std::make_unique<BankUtilityPanel>(skin,
        PluginDimensions::Panels::Body::PatchManagerSection::kWidth, bankUtilityPanelHeight_, widgetFactory, apvts))
    , internalPatchesPanel_(std::make_unique<InternalPatchesPanel>(skin,
        PluginDimensions::Panels::Body::PatchManagerSection::kWidth, internalPatchesPanelHeight_, widgetFactory, apvts))
    , computerPatchesPanel_(std::make_unique<ComputerPatchesPanel>(skin,
        PluginDimensions::Panels::Body::PatchManagerSection::kWidth, computerPatchesPanelHeight_, widgetFactory, apvts))
    , patchMutatorPanel_(std::make_unique<PatchMutatorPanel>(skin,
        PluginDimensions::Panels::Body::PatchManagerSection::kWidth, patchMutatorPanelHeight_, widgetFactory, apvts))
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
    const float sf = displayScale_;
    const int panelWidth = tss::ScaledLayout::scaledInt(static_cast<float>(width_), sf);

    const int sectionHeaderHeight = tss::ScaledLayout::scaledInt(
        static_cast<float>(PluginDimensions::Widgets::Heights::kSectionHeader), sf);
    const int contentTop = bounds.getY() + sectionHeaderHeight;
    const int contentHeight = bounds.getHeight() - sectionHeaderHeight;

    const std::vector<int> moduleDesignHeights {
        bankUtilityPanelHeight_,
        internalPatchesPanelHeight_,
        computerPatchesPanelHeight_,
        patchMutatorPanelHeight_
    };
    const auto moduleHeights = tss::ScaledLayout::distributeHeights(contentHeight, moduleDesignHeights, sf, 3);

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

void PatchManagerPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    sectionHeader_->setLook(tss::sectionHeaderLookFromSkin(skin));
    tss::propagateSkin(skin,
        bankUtilityPanel_.get(),
        internalPatchesPanel_.get(),
        computerPatchesPanel_.get(),
        patchMutatorPanel_.get());
}

void PatchManagerPanel::setDisplayScale(float displayScale)
{
    if (juce::approximatelyEqual(displayScale_, displayScale))
        return;
    
    displayScale_ = displayScale;
    
    if (sectionHeader_)
        sectionHeader_->setDisplayScale(displayScale_);
    if (bankUtilityPanel_)
        bankUtilityPanel_->setDisplayScale(displayScale_);
    if (internalPatchesPanel_)
        internalPatchesPanel_->setDisplayScale(displayScale_);
    if (computerPatchesPanel_)
        computerPatchesPanel_->setDisplayScale(displayScale_);
    if (patchMutatorPanel_)
        patchMutatorPanel_->setDisplayScale(displayScale_);
    
    resized();
    repaint();
}

