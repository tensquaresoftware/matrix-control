#include "MasterEditPanel.h"

#include "Modules/MidiPanel.h"
#include "Modules/VibratoPanel.h"
#include "Modules/MiscPanel.h"

#include "GUI/Themes/Skin.h"
#include "GUI/Widgets/SectionHeader.h"
#include "Shared/PluginDescriptors.h"
#include "Shared/PluginHelpers.h"
#include "Shared/PluginDimensions.h"
#include "Shared/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


MasterEditPanel::MasterEditPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
    , sectionHeader_(std::make_unique<tss::SectionHeader>(
        skin, 
        PluginDimensions::Widgets::Widths::SectionHeader::kMasterEdit,
        PluginDimensions::Widgets::Heights::kSectionHeader,
        PluginHelpers::getSectionDisplayName(PluginIDs::MasterEditSection::kGroupId),
        tss::SectionHeader::ColourVariant::Orange))
    , midiPanel_(std::make_unique<MidiPanel>(skin, widgetFactory, apvts))
    , vibratoPanel_(std::make_unique<VibratoPanel>(skin, widgetFactory, apvts))
    , miscPanel_(std::make_unique<MiscPanel>(skin, widgetFactory, apvts))
{
    setOpaque(false);
    addAndMakeVisible(*sectionHeader_);
    addAndMakeVisible(*midiPanel_);
    addAndMakeVisible(*vibratoPanel_);
    addAndMakeVisible(*miscPanel_);

    setSize(getWidth(), getHeight());
}

MasterEditPanel::~MasterEditPanel() = default;

void MasterEditPanel::resized()
{
    const auto bounds = getLocalBounds();
    
    const auto sectionHeaderY = 0;
    sectionHeader_->setBounds(
        bounds.getX() + 0,
        bounds.getY() + sectionHeaderY,
        bounds.getWidth(),
        PluginDimensions::Widgets::Heights::kSectionHeader
    );
    
    const auto midiPanelY = sectionHeaderY + PluginDimensions::Widgets::Heights::kSectionHeader;
    midiPanel_->setBounds(
        bounds.getX() + 0,
        bounds.getY() + midiPanelY,
        MidiPanel::getWidth(),
        MidiPanel::getHeight()
    );
    
    const auto vibratoPanelY = midiPanelY + MidiPanel::getHeight();
    vibratoPanel_->setBounds(
        bounds.getX() + 0,
        bounds.getY() + vibratoPanelY,
        VibratoPanel::getWidth(),
        VibratoPanel::getHeight()
    );
    
    const auto miscPanelY = vibratoPanelY + VibratoPanel::getHeight();
    miscPanel_->setBounds(
        bounds.getX() + 0,
        bounds.getY() + miscPanelY,
        MiscPanel::getWidth(),
        MiscPanel::getHeight()
    );
}

void MasterEditPanel::setSkin(tss::Skin& skin)
{
    skin_ = &skin;

    if (auto* header = sectionHeader_.get())
        header->setSkin(skin);

    if (auto* panel = midiPanel_.get())
        panel->setSkin(skin);

    if (auto* panel = vibratoPanel_.get())
        panel->setSkin(skin);

    if (auto* panel = miscPanel_.get())
        panel->setSkin(skin);
}

