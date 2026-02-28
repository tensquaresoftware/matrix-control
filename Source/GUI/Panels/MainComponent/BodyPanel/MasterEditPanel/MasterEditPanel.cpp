#include "MasterEditPanel.h"

#include "Modules/MidiPanel.h"
#include "Modules/VibratoPanel.h"
#include "Modules/MiscPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Widgets/SectionHeader.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "Shared/Definitions/PluginDimensions.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


MasterEditPanel::MasterEditPanel(tss::Skin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , childModuleWidth_(PluginDimensions::Panels::Body::MasterEditSection::ChildModules::kWidth)
    , midiPanelHeight_(PluginDimensions::Panels::Body::MasterEditSection::MidiModule::kHeight)
    , vibratoPanelHeight_(PluginDimensions::Panels::Body::MasterEditSection::VibratoModule::kHeight)
    , miscPanelHeight_(PluginDimensions::Panels::Body::MasterEditSection::MiscModule::kHeight)
    , skin_(&skin)
    , sectionHeader_(std::make_unique<tss::SectionHeader>(
        skin,
        PluginDimensions::Widgets::Widths::SectionHeader::kMasterEdit,
        PluginDimensions::Widgets::Heights::kSectionHeader,
        PluginHelpers::getSectionDisplayName(PluginIDs::MasterEditSection::kGroupId),
        tss::SectionHeader::ColourVariant::Orange))
    , midiPanel_(std::make_unique<MidiPanel>(skin, childModuleWidth_, midiPanelHeight_, widgetFactory, apvts))
    , vibratoPanel_(std::make_unique<VibratoPanel>(skin, childModuleWidth_, vibratoPanelHeight_, widgetFactory, apvts))
    , miscPanel_(std::make_unique<MiscPanel>(skin, childModuleWidth_, miscPanelHeight_, widgetFactory, apvts))
{
    setOpaque(false);
    addAndMakeVisible(*sectionHeader_);
    addAndMakeVisible(*midiPanel_);
    addAndMakeVisible(*vibratoPanel_);
    addAndMakeVisible(*miscPanel_);

    setSize(width_, height_);
}

MasterEditPanel::~MasterEditPanel() = default;

void MasterEditPanel::resized()
{
    const auto bounds = getLocalBounds();
    const auto sectionHeaderHeight = PluginDimensions::Widgets::Heights::kSectionHeader;
    
    const auto sectionHeaderY = 0;
    sectionHeader_->setBounds(
        bounds.getX() + 0,
        bounds.getY() + sectionHeaderY,
        bounds.getWidth(),
        sectionHeaderHeight
    );
    
    const auto midiPanelY = sectionHeaderY + sectionHeaderHeight;
    midiPanel_->setBounds(
        bounds.getX() + 0,
        bounds.getY() + midiPanelY,
        childModuleWidth_,
        midiPanelHeight_
    );
    
    const auto vibratoPanelY = midiPanelY + midiPanelHeight_;
    vibratoPanel_->setBounds(
        bounds.getX() + 0,
        bounds.getY() + vibratoPanelY,
        childModuleWidth_,
        vibratoPanelHeight_
    );
    
    const auto miscPanelY = vibratoPanelY + vibratoPanelHeight_;
    miscPanel_->setBounds(
        bounds.getX() + 0,
        bounds.getY() + miscPanelY,
        childModuleWidth_,
        miscPanelHeight_
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

