#include "BottomPanel.h"

#include "Modules/Env1Panel.h"
#include "Modules/Env2Panel.h"
#include "Modules/Env3Panel.h"
#include "Modules/Lfo1Panel.h"
#include "Modules/Lfo2Panel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginDimensions.h"


BottomPanel::~BottomPanel() = default;

BottomPanel::BottomPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , childModuleWidth_(PluginDimensions::Panels::Body::PatchEditSection::BottomModules::ChildModules::kWidth)
    , childModuleHeight_(PluginDimensions::Panels::Body::PatchEditSection::BottomModules::ChildModules::kHeight)
    , spacing_(PluginDimensions::Panels::Body::kPadding)
    , skin_(&skin)
    , env1Panel_(std::make_unique<Env1Panel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
    , env2Panel_(std::make_unique<Env2Panel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
    , env3Panel_(std::make_unique<Env3Panel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
    , lfo1Panel_(std::make_unique<Lfo1Panel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
    , lfo2Panel_(std::make_unique<Lfo2Panel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
{
    setOpaque(false);
    addAndMakeVisible(*env1Panel_);
    addAndMakeVisible(*env2Panel_);
    addAndMakeVisible(*env3Panel_);
    addAndMakeVisible(*lfo1Panel_);
    addAndMakeVisible(*lfo2Panel_);

    setSize(width_, height_);
}

void BottomPanel::resized()
{
    const auto bounds = getLocalBounds();
    
    const auto env1PanelX = 0;
    const auto env1PanelY = 0;
    env1Panel_->setBounds(
        bounds.getX() + env1PanelX,
        bounds.getY() + env1PanelY,
        childModuleWidth_,
        childModuleHeight_
    );
    
    const auto env2PanelX = env1PanelX + childModuleWidth_ + spacing_;
    const auto env2PanelY = 0;
    env2Panel_->setBounds(
        bounds.getX() + env2PanelX,
        bounds.getY() + env2PanelY,
        childModuleWidth_,
        childModuleHeight_
    );
    
    const auto env3PanelX = env2PanelX + childModuleWidth_ + spacing_;
    const auto env3PanelY = 0;
    env3Panel_->setBounds(
        bounds.getX() + env3PanelX,
        bounds.getY() + env3PanelY,
        childModuleWidth_,
        childModuleHeight_
    );
    
    const auto lfo1PanelX = env3PanelX + childModuleWidth_ + spacing_;
    const auto lfo1PanelY = 0;
    lfo1Panel_->setBounds(
        bounds.getX() + lfo1PanelX,
        bounds.getY() + lfo1PanelY,
        childModuleWidth_,
        childModuleHeight_
    );
    
    const auto lfo2PanelX = lfo1PanelX + childModuleWidth_ + spacing_;
    const auto lfo2PanelY = 0;
    lfo2Panel_->setBounds(
        bounds.getX() + lfo2PanelX,
        bounds.getY() + lfo2PanelY,
        childModuleWidth_,
        childModuleHeight_
    );
}

void BottomPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    tss::propagateSkin(skin,
        env1Panel_.get(),
        env2Panel_.get(),
        env3Panel_.get(),
        lfo1Panel_.get(),
        lfo2Panel_.get());
}

