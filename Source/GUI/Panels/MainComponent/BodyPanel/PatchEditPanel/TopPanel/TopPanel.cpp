#include "TopPanel.h"

#include "Modules/Dco1Panel.h"
#include "Modules/Dco2Panel.h"
#include "Modules/VcfVcaPanel.h"
#include "Modules/FmTrackPanel.h"
#include "Modules/RampPortamentoPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/PluginDimensions.h"


TopPanel::~TopPanel() = default;

TopPanel::TopPanel(tss::Skin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , childModuleWidth_(PluginDimensions::Panels::Body::PatchEditSection::TopModules::ChildModules::kWidth)
    , childModuleHeight_(PluginDimensions::Panels::Body::PatchEditSection::TopModules::ChildModules::kHeight)
    , spacing_(PluginDimensions::Panels::Body::kPadding)
    , skin_(&skin)
    , dco1Panel_(std::make_unique<Dco1Panel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
    , dco2Panel_(std::make_unique<Dco2Panel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
    , vcfVcaPanel_(std::make_unique<VcfVcaPanel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
    , fmTrackPanel_(std::make_unique<FmTrackPanel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
    , rampPortamentoPanel_(std::make_unique<RampPortamentoPanel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
{
    setOpaque(false);
    addAndMakeVisible(*dco1Panel_);
    addAndMakeVisible(*dco2Panel_);
    addAndMakeVisible(*vcfVcaPanel_);
    addAndMakeVisible(*fmTrackPanel_);
    addAndMakeVisible(*rampPortamentoPanel_);

    setSize(width_, height_);
}

void TopPanel::resized()
{
    const auto bounds = getLocalBounds();
    
    const auto dco1PanelX = 0;
    const auto dco1PanelY = 0;
    dco1Panel_->setBounds(
        bounds.getX() + dco1PanelX,
        bounds.getY() + dco1PanelY,
        childModuleWidth_,
        childModuleHeight_
    );
    
    const auto dco2PanelX = dco1PanelX + childModuleWidth_ + spacing_;
    const auto dco2PanelY = 0;
    dco2Panel_->setBounds(
        bounds.getX() + dco2PanelX,
        bounds.getY() + dco2PanelY,
        childModuleWidth_,
        childModuleHeight_
    );
    
    const auto vcfVcaPanelX = dco2PanelX + childModuleWidth_ + spacing_;
    const auto vcfVcaPanelY = 0;
    vcfVcaPanel_->setBounds(
        bounds.getX() + vcfVcaPanelX,
        bounds.getY() + vcfVcaPanelY,
        childModuleWidth_,
        childModuleHeight_
    );
    
    const auto fmTrackPanelX = vcfVcaPanelX + childModuleWidth_ + spacing_;
    const auto fmTrackPanelY = 0;
    fmTrackPanel_->setBounds(
        bounds.getX() + fmTrackPanelX,
        bounds.getY() + fmTrackPanelY,
        childModuleWidth_,
        childModuleHeight_
    );
    
    const auto rampPortamentoPanelX = fmTrackPanelX + childModuleWidth_ + spacing_;
    const auto rampPortamentoPanelY = 0;
    rampPortamentoPanel_->setBounds(
        bounds.getX() + rampPortamentoPanelX,
        bounds.getY() + rampPortamentoPanelY,
        childModuleWidth_,
        childModuleHeight_
    );
}

void TopPanel::setSkin(tss::Skin& skin)
{
    skin_ = &skin;

    if (auto* panel = dco1Panel_.get())
        panel->setSkin(skin);

    if (auto* panel = dco2Panel_.get())
        panel->setSkin(skin);

    if (auto* panel = vcfVcaPanel_.get())
        panel->setSkin(skin);

    if (auto* panel = fmTrackPanel_.get())
        panel->setSkin(skin);

    if (auto* panel = rampPortamentoPanel_.get())
        panel->setSkin(skin);
}

