#include "MasterEditPanel.h"

#include <vector>

#include "GUI/Layout/ScaledLayout.h"
#include "Modules/MidiPanel.h"
#include "Modules/VibratoPanel.h"
#include "Modules/MiscPanel.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Widgets/SectionHeader.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "Shared/Definitions/PluginDimensions.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


MasterEditPanel::MasterEditPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , childModuleWidth_(PluginDimensions::Panels::Body::MasterEditSection::ChildModules::kWidth)
    , midiPanelHeight_(PluginDimensions::Panels::Body::MasterEditSection::MidiModule::kHeight)
    , vibratoPanelHeight_(PluginDimensions::Panels::Body::MasterEditSection::VibratoModule::kHeight)
    , miscPanelHeight_(PluginDimensions::Panels::Body::MasterEditSection::MiscModule::kHeight)
    , skin_(&skin)
    , sectionHeader_(std::make_unique<tss::SectionHeader>(
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
    const float sf = scalingFactor_;

    const int sectionHeaderHeight = tss::ScaledLayout::scaledInt(
        static_cast<float>(PluginDimensions::Widgets::Heights::kSectionHeader), sf);
    const int childWidth = tss::ScaledLayout::scaledInt(static_cast<float>(childModuleWidth_), sf);

    const int contentTop = bounds.getY() + sectionHeaderHeight;
    const int contentHeight = bounds.getHeight() - sectionHeaderHeight;
    const std::vector<int> moduleDesignHeights { midiPanelHeight_, vibratoPanelHeight_, miscPanelHeight_ };
    const auto moduleHeights = tss::ScaledLayout::distributeHeights(contentHeight, moduleDesignHeights, sf, 2);

    sectionHeader_->setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), sectionHeaderHeight);

    int y = contentTop;
    midiPanel_->setBounds(bounds.getX(), y, childWidth, moduleHeights[0]);
    y += moduleHeights[0];
    vibratoPanel_->setBounds(bounds.getX(), y, childWidth, moduleHeights[1]);
    y += moduleHeights[1];
    miscPanel_->setBounds(bounds.getX(), y, childWidth, moduleHeights[2]);
}

void MasterEditPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    sectionHeader_->setLook(tss::sectionHeaderLookFromSkin(skin));
    tss::propagateSkin(skin,
        midiPanel_.get(),
        vibratoPanel_.get(),
        miscPanel_.get());
}

void MasterEditPanel::setScalingFactor(float scalingFactor)
{
    if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
        return;
    
    scalingFactor_ = scalingFactor;
    
    if (sectionHeader_)
        sectionHeader_->setScalingFactor(scalingFactor_);
    if (midiPanel_)
        midiPanel_->setScalingFactor(scalingFactor_);
    if (vibratoPanel_)
        vibratoPanel_->setScalingFactor(scalingFactor_);
    if (miscPanel_)
        miscPanel_->setScalingFactor(scalingFactor_);
    
    resized();
    repaint();
}

