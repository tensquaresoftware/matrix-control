#include "PatchManagerPanel.h"

#include "Modules/BankUtilityPanel.h"
#include "Modules/InternalPatchesPanel.h"
#include "Modules/ComputerPatchesPanel.h"
#include "Modules/PatchMutatorPanel.h"

#include "GUI/Themes/Skin.h"
#include "GUI/Widgets/SectionHeader.h"
#include "Shared/PluginDescriptors.h"
#include "Shared/PluginHelpers.h"
#include "Shared/PluginDimensions.h"
#include "GUI/Factories/WidgetFactory.h"


PatchManagerPanel::PatchManagerPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
    , sectionHeader_(std::make_unique<tss::SectionHeader>(
        skin,
        PluginDimensions::Widgets::Widths::SectionHeader::kPatchManager,
        PluginDimensions::Widgets::Heights::kSectionHeader,
        PluginHelpers::getSectionDisplayName(PluginIDs::PatchManagerSection::kGroupId),
        tss::SectionHeader::ColourVariant::Blue))
    , bankUtilityPanel_(std::make_unique<BankUtilityPanel>(skin, widgetFactory, apvts))
    , internalPatchesPanel_(std::make_unique<InternalPatchesPanel>(skin, widgetFactory, apvts))
    , computerPatchesPanel_(std::make_unique<ComputerPatchesPanel>(skin, widgetFactory, apvts))
    , patchMutatorPanel_(std::make_unique<PatchMutatorPanel>(skin, widgetFactory, apvts))
{
    setOpaque(false);
    addAndMakeVisible(*sectionHeader_);
    addAndMakeVisible(*bankUtilityPanel_);
    addAndMakeVisible(*internalPatchesPanel_);
    addAndMakeVisible(*computerPatchesPanel_);
    addAndMakeVisible(*patchMutatorPanel_);

    setSize(getWidth(), getHeight());
}

PatchManagerPanel::~PatchManagerPanel() = default;

void PatchManagerPanel::resized()
{
    const auto bounds = getLocalBounds();
    int y = 0;

    layoutSectionHeader(bounds, y);

    y += PluginDimensions::Widgets::Heights::kSectionHeader;
    layoutBankUtilityPanel(bounds, y);

    y += BankUtilityPanel::getHeight();
    layoutInternalPatchesPanel(bounds, y);

    y += InternalPatchesPanel::getHeight();
    layoutComputerPatchesPanel(bounds, y);

    y += ComputerPatchesPanel::getHeight();
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
            BankUtilityPanel::getWidth(),
            BankUtilityPanel::getHeight()
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
            InternalPatchesPanel::getWidth(),
            InternalPatchesPanel::getHeight()
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
            ComputerPatchesPanel::getWidth(),
            ComputerPatchesPanel::getHeight()
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
            PatchMutatorPanel::getWidth(),
            PatchMutatorPanel::getHeight()
        );
    }
}

void PatchManagerPanel::setSkin(tss::Skin& skin)
{
    skin_ = &skin;

    if (auto* header = sectionHeader_.get())
        header->setSkin(skin);

    if (auto* panel = bankUtilityPanel_.get())
        panel->setSkin(skin);

    if (auto* panel = internalPatchesPanel_.get())
        panel->setSkin(skin);

    if (auto* panel = computerPatchesPanel_.get())
        panel->setSkin(skin);

    if (auto* panel = patchMutatorPanel_.get())
        panel->setSkin(skin);
}

