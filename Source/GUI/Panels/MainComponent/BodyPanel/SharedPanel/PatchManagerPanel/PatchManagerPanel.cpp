#include "PatchManagerPanel.h"

#include "Modules/BankUtilityPanel.h"
#include "Modules/InternalPatchesPanel.h"
#include "Modules/ComputerPatchesPanel.h"
#include "Modules/PatchMutatorPanel.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Widgets/SectionHeader.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "GUI/Factories/WidgetFactory.h"


PatchManagerPanel::PatchManagerPanel(TSS::ISkin& skin,
                                     const PatchManagerPanelDimensions& dims,
                                     WidgetFactory& widgetFactory,
                                     juce::AudioProcessorValueTreeState& apvts,
                                     const Core::PatchFileService& patchFileService)
    : dims_(dims)
    , skin_(&skin)
    , sectionHeader_(std::make_unique<TSS::SectionHeader>(
        dims_.sectionHeaderWidth,
        dims_.sectionHeaderHeight,
        TSS::sectionHeaderLookFromSkin(skin),
        PluginHelpers::getSectionDisplayName(PluginIDs::PatchManagerSection::kGroupId),
        TSS::SectionHeader::ColourVariant::Blue))
    , bankUtilityPanel_(std::make_unique<BankUtilityPanel>(skin, dims_.bankUtility, widgetFactory, apvts))
    , internalPatchesPanel_(std::make_unique<InternalPatchesPanel>(skin, dims_.internalPatches, widgetFactory, apvts))
    , computerPatchesPanel_(std::make_unique<ComputerPatchesPanel>(skin, dims_.computerPatches, widgetFactory, apvts, patchFileService))
    , patchMutatorPanel_(std::make_unique<PatchMutatorPanel>(skin, dims_.patchMutator, widgetFactory, apvts))
{
    setOpaque(false);
    addAndMakeVisible(*sectionHeader_);
    addAndMakeVisible(*bankUtilityPanel_);
    addAndMakeVisible(*internalPatchesPanel_);
    addAndMakeVisible(*computerPatchesPanel_);
    addAndMakeVisible(*patchMutatorPanel_);

    setSize(dims_.width, dims_.height);
}

PatchManagerPanel::~PatchManagerPanel() = default;

void PatchManagerPanel::resized()
{
    const auto bounds = getLocalBounds();
    const float sf = uiScale_;
    const int panelWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.width), sf);
    const int sectionHeaderHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.sectionHeaderHeight), sf);
    const int moduleStackGap = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.moduleStackGap), sf);
    const int bankUtilityH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.bankUtilityHeight), sf);
    const int internalPatchesH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.internalPatchesHeight), sf);
    const int computerPatchesH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.computerPatchesHeight), sf);
    const int patchMutatorH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.patchMutatorHeight), sf);

    if (auto* header = sectionHeader_.get())
        header->setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), sectionHeaderHeight);

    int y = bounds.getY() + sectionHeaderHeight;

    if (auto* panel = bankUtilityPanel_.get())
    {
        panel->setBounds(bounds.getX(), y, panelWidth, bankUtilityH);
        y += bankUtilityH + moduleStackGap;
    }
    if (auto* panel = internalPatchesPanel_.get())
    {
        panel->setBounds(bounds.getX(), y, panelWidth, internalPatchesH);
        y += internalPatchesH + moduleStackGap;
    }
    if (auto* panel = computerPatchesPanel_.get())
    {
        panel->setBounds(bounds.getX(), y, panelWidth, computerPatchesH);
        y += computerPatchesH + moduleStackGap;
    }
    if (auto* panel = patchMutatorPanel_.get())
        panel->setBounds(bounds.getX(), y, panelWidth, patchMutatorH);
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
