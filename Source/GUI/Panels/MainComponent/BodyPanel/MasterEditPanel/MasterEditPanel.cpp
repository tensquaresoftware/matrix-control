#include "MasterEditPanel.h"

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

    const int sectionHeaderHeight = juce::roundToInt(static_cast<float>(PluginDimensions::Widgets::Heights::kSectionHeader) * sf);
    const int childWidth          = juce::roundToInt(static_cast<float>(childModuleWidth_) * sf);
    const int midiPanelHeight     = juce::roundToInt(static_cast<float>(midiPanelHeight_) * sf);
    const int vibratoPanelHeight  = juce::roundToInt(static_cast<float>(vibratoPanelHeight_) * sf);
    const int miscPanelHeight     = juce::roundToInt(static_cast<float>(miscPanelHeight_) * sf);

    // Y positions computed independently from float origin to avoid rounding accumulation.
    const float originY = static_cast<float>(bounds.getY());
    const int sectionHeaderY = bounds.getY();
    const int midiPanelY     = juce::roundToInt(originY + static_cast<float>(PluginDimensions::Widgets::Heights::kSectionHeader) * sf);
    const int vibratoPanelY  = juce::roundToInt(originY + static_cast<float>(PluginDimensions::Widgets::Heights::kSectionHeader + midiPanelHeight_) * sf);
    const int miscPanelY     = juce::roundToInt(originY + static_cast<float>(PluginDimensions::Widgets::Heights::kSectionHeader + midiPanelHeight_ + vibratoPanelHeight_) * sf);

    sectionHeader_->setBounds(bounds.getX(), sectionHeaderY, bounds.getWidth(), sectionHeaderHeight);
    midiPanel_->setBounds(bounds.getX(), midiPanelY, childWidth, midiPanelHeight);
    vibratoPanel_->setBounds(bounds.getX(), vibratoPanelY, childWidth, vibratoPanelHeight);
    miscPanel_->setBounds(bounds.getX(), miscPanelY, childWidth, miscPanelHeight);
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

