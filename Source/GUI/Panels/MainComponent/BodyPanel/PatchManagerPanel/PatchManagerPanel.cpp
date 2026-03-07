#include "PatchManagerPanel.h"

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
    const float sf = scalingFactor_;
    const float originY = static_cast<float>(bounds.getY());
    const int panelWidth = juce::roundToInt(static_cast<float>(width_) * sf);

    const int sectionHeaderHeight    = juce::roundToInt(static_cast<float>(PluginDimensions::Widgets::Heights::kSectionHeader) * sf);
    const int bankUtilityHeight      = juce::roundToInt(static_cast<float>(bankUtilityPanelHeight_) * sf);
    const int internalPatchesHeight  = juce::roundToInt(static_cast<float>(internalPatchesPanelHeight_) * sf);
    const int computerPatchesHeight  = juce::roundToInt(static_cast<float>(computerPatchesPanelHeight_) * sf);
    const int patchMutatorHeight     = juce::roundToInt(static_cast<float>(patchMutatorPanelHeight_) * sf);

    // Y positions computed independently from float origin to avoid rounding accumulation.
    const int sectionHeaderY   = bounds.getY();
    const int bankUtilityY     = juce::roundToInt(originY + static_cast<float>(PluginDimensions::Widgets::Heights::kSectionHeader) * sf);
    const int internalY        = juce::roundToInt(originY + static_cast<float>(PluginDimensions::Widgets::Heights::kSectionHeader + bankUtilityPanelHeight_) * sf);
    const int computerY        = juce::roundToInt(originY + static_cast<float>(PluginDimensions::Widgets::Heights::kSectionHeader + bankUtilityPanelHeight_ + internalPatchesPanelHeight_) * sf);
    const int patchMutatorY    = juce::roundToInt(originY + static_cast<float>(PluginDimensions::Widgets::Heights::kSectionHeader + bankUtilityPanelHeight_ + internalPatchesPanelHeight_ + computerPatchesPanelHeight_) * sf);

    if (auto* header = sectionHeader_.get())
        header->setBounds(bounds.getX(), sectionHeaderY, bounds.getWidth(), sectionHeaderHeight);
    if (auto* panel = bankUtilityPanel_.get())
        panel->setBounds(bounds.getX(), bankUtilityY, panelWidth, bankUtilityHeight);
    if (auto* panel = internalPatchesPanel_.get())
        panel->setBounds(bounds.getX(), internalY, panelWidth, internalPatchesHeight);
    if (auto* panel = computerPatchesPanel_.get())
        panel->setBounds(bounds.getX(), computerY, panelWidth, computerPatchesHeight);
    if (auto* panel = patchMutatorPanel_.get())
        panel->setBounds(bounds.getX(), patchMutatorY, panelWidth, patchMutatorHeight);
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

void PatchManagerPanel::setScalingFactor(float scalingFactor)
{
    if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
        return;
    
    scalingFactor_ = scalingFactor;
    
    if (sectionHeader_)
        sectionHeader_->setScalingFactor(scalingFactor_);
    if (bankUtilityPanel_)
        bankUtilityPanel_->setScalingFactor(scalingFactor_);
    if (internalPatchesPanel_)
        internalPatchesPanel_->setScalingFactor(scalingFactor_);
    if (computerPatchesPanel_)
        computerPatchesPanel_->setScalingFactor(scalingFactor_);
    if (patchMutatorPanel_)
        patchMutatorPanel_->setScalingFactor(scalingFactor_);
    
    resized();
    repaint();
}

