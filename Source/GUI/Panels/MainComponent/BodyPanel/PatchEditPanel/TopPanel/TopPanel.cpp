#include "TopPanel.h"

#include "Modules/Dco1Panel.h"
#include "Modules/Dco2Panel.h"
#include "Modules/VcfVcaPanel.h"
#include "Modules/FmTrackPanel.h"
#include "Modules/RampPortamentoPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Factories/WidgetFactory.h"


TopPanel::~TopPanel() = default;

TopPanel::TopPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
    , dco1Panel_(std::make_unique<Dco1Panel>(skin, widgetFactory, apvts))
    , dco2Panel_(std::make_unique<Dco2Panel>(skin, widgetFactory, apvts))
    , vcfVcaPanel_(std::make_unique<VcfVcaPanel>(skin, widgetFactory, apvts))
    , fmTrackPanel_(std::make_unique<FmTrackPanel>(skin, widgetFactory, apvts))
    , rampPortamentoPanel_(std::make_unique<RampPortamentoPanel>(skin, widgetFactory, apvts))
{
    setOpaque(false);
    addAndMakeVisible(*dco1Panel_);
    addAndMakeVisible(*dco2Panel_);
    addAndMakeVisible(*vcfVcaPanel_);
    addAndMakeVisible(*fmTrackPanel_);
    addAndMakeVisible(*rampPortamentoPanel_);

    setSize(getWidth(), getHeight());
}

void TopPanel::resized()
{
    const auto bounds = getLocalBounds();
    const auto spacing = getSpacing();
    
    const auto dco1PanelX = 0;
    const auto dco1PanelY = 0;
    dco1Panel_->setBounds(
        bounds.getX() + dco1PanelX,
        bounds.getY() + dco1PanelY,
        Dco1Panel::getWidth(),
        Dco1Panel::getHeight()
    );
    
    const auto dco2PanelX = dco1PanelX + Dco1Panel::getWidth() + spacing;
    const auto dco2PanelY = 0;
    dco2Panel_->setBounds(
        bounds.getX() + dco2PanelX,
        bounds.getY() + dco2PanelY,
        Dco2Panel::getWidth(),
        Dco2Panel::getHeight()
    );
    
    const auto vcfVcaPanelX = dco2PanelX + Dco2Panel::getWidth() + spacing;
    const auto vcfVcaPanelY = 0;
    vcfVcaPanel_->setBounds(
        bounds.getX() + vcfVcaPanelX,
        bounds.getY() + vcfVcaPanelY,
        VcfVcaPanel::getWidth(),
        VcfVcaPanel::getHeight()
    );
    
    const auto fmTrackPanelX = vcfVcaPanelX + VcfVcaPanel::getWidth() + spacing;
    const auto fmTrackPanelY = 0;
    fmTrackPanel_->setBounds(
        bounds.getX() + fmTrackPanelX,
        bounds.getY() + fmTrackPanelY,
        FmTrackPanel::getWidth(),
        FmTrackPanel::getHeight()
    );
    
    const auto rampPortamentoPanelX = fmTrackPanelX + FmTrackPanel::getWidth() + spacing;
    const auto rampPortamentoPanelY = 0;
    rampPortamentoPanel_->setBounds(
        bounds.getX() + rampPortamentoPanelX,
        bounds.getY() + rampPortamentoPanelY,
        RampPortamentoPanel::getWidth(),
        RampPortamentoPanel::getHeight()
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

