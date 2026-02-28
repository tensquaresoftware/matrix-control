#include "PatchManagerPanel.h"

#include "Modules/BankUtilityPanel.h"
#include "Modules/InternalPatchesPanel.h"
#include "Modules/ComputerPatchesPanel.h"
#include "Modules/PatchMutatorPanel.h"

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
        skin,
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
    const auto sectionHeaderHeight = PluginDimensions::Widgets::Heights::kSectionHeader;
    int y = 0;

    layoutSectionHeader(bounds, y);

    y += sectionHeaderHeight;
    layoutBankUtilityPanel(bounds, y);

    y += bankUtilityPanelHeight_;
    layoutInternalPatchesPanel(bounds, y);

    y += internalPatchesPanelHeight_;
    layoutComputerPatchesPanel(bounds, y);

    y += computerPatchesPanelHeight_;
    layoutPatchMutatorPanel(bounds, y);
}

void PatchManagerPanel::layoutSectionHeader(const juce::Rectangle<int>& bounds, int y)
{
    if (auto* header = sectionHeader_.get())
    {
        header->setBounds(
            bounds.getX(),
            bounds.getY() + y,
            bounds.getWidth(),
            PluginDimensions::Widgets::Heights::kSectionHeader
        );
    }
}

void PatchManagerPanel::layoutBankUtilityPanel(const juce::Rectangle<int>& bounds, int y)
{
    if (auto* panel = bankUtilityPanel_.get())
    {
        panel->setBounds(
            bounds.getX(),
            bounds.getY() + y,
            width_,
            bankUtilityPanelHeight_
        );
    }
}

void PatchManagerPanel::layoutInternalPatchesPanel(const juce::Rectangle<int>& bounds, int y)
{
    if (auto* panel = internalPatchesPanel_.get())
    {
        panel->setBounds(
            bounds.getX(),
            bounds.getY() + y,
            width_,
            internalPatchesPanelHeight_
        );
    }
}

void PatchManagerPanel::layoutComputerPatchesPanel(const juce::Rectangle<int>& bounds, int y)
{
    if (auto* panel = computerPatchesPanel_.get())
    {
        panel->setBounds(
            bounds.getX(),
            bounds.getY() + y,
            width_,
            computerPatchesPanelHeight_
        );
    }
}

void PatchManagerPanel::layoutPatchMutatorPanel(const juce::Rectangle<int>& bounds, int y)
{
    if (auto* panel = patchMutatorPanel_.get())
    {
        panel->setBounds(
            bounds.getX(),
            bounds.getY() + y,
            width_,
            patchMutatorPanelHeight_
        );
    }
}

void PatchManagerPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    tss::propagateSkin(skin,
        sectionHeader_.get(),
        bankUtilityPanel_.get(),
        internalPatchesPanel_.get(),
        computerPatchesPanel_.get(),
        patchMutatorPanel_.get());
}

