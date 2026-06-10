#include "MasterEditPanel.h"

#include "Modules/MidiPanel.h"
#include "Modules/VibratoPanel.h"
#include "Modules/MiscPanel.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Widgets/SectionHeader.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


MasterEditPanel::MasterEditPanel(TSS::ISkin& skin, const MasterEditPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : dims_(dims)
    , skin_(&skin)
    , sectionHeader_(std::make_unique<TSS::SectionHeader>(
        dims_.sectionHeaderWidth,
        dims_.sectionHeaderHeight,
        TSS::sectionHeaderLookFromSkin(skin),
        PluginHelpers::getSectionDisplayName(PluginIDs::MasterEditSection::kGroupId),
        TSS::SectionHeader::ColourVariant::Orange))
    , midiPanel_(std::make_unique<MidiPanel>(skin, dims_.childModuleWidth, dims_.midiPanelHeight, widgetFactory, apvts, dims_.moduleHeader, dims_.parameterCell))
    , vibratoPanel_(std::make_unique<VibratoPanel>(skin, dims_.childModuleWidth, dims_.vibratoPanelHeight, widgetFactory, apvts, dims_.moduleHeader, dims_.parameterCell))
    , miscPanel_(std::make_unique<MiscPanel>(skin, dims_.childModuleWidth, dims_.miscPanelHeight, widgetFactory, apvts, dims_.moduleHeader, dims_.parameterCell))
{
    setOpaque(false);
    addAndMakeVisible(*sectionHeader_);
    addAndMakeVisible(*midiPanel_);
    addAndMakeVisible(*vibratoPanel_);
    addAndMakeVisible(*miscPanel_);

    setSize(dims_.width, dims_.height);
}

MasterEditPanel::~MasterEditPanel() = default;

void MasterEditPanel::resized()
{
    const auto bounds = getLocalBounds();
    const float sf = uiScale_;

    const int sectionHeaderHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.sectionHeaderHeight), sf);
    const int childWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.childModuleWidth), sf);
    const int midiH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.midiPanelHeight), sf);
    const int vibratoH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.vibratoPanelHeight), sf);
    const int miscH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.miscPanelHeight), sf);
    const int interModuleGap = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.interModuleGap), sf);

    sectionHeader_->setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), sectionHeaderHeight);

    int y = bounds.getY() + sectionHeaderHeight;
    midiPanel_->setBounds(bounds.getX(), y, childWidth, midiH);
    y += midiH + interModuleGap;
    vibratoPanel_->setBounds(bounds.getX(), y, childWidth, vibratoH);
    y += vibratoH + interModuleGap;
    miscPanel_->setBounds(bounds.getX(), y, childWidth, miscH);
}

void MasterEditPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    sectionHeader_->setLook(TSS::sectionHeaderLookFromSkin(skin));
    TSS::propagateSkin(skin,
        midiPanel_.get(),
        vibratoPanel_.get(),
        miscPanel_.get());
}

void MasterEditPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    
    if (sectionHeader_)
        sectionHeader_->setUiScale(uiScale_);
    if (midiPanel_)
        midiPanel_->setUiScale(uiScale_);
    if (vibratoPanel_)
        vibratoPanel_->setUiScale(uiScale_);
    if (miscPanel_)
        miscPanel_->setUiScale(uiScale_);
    
    resized();
    repaint();
}
