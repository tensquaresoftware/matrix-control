#include "BottomPanel.h"

#include "Modules/Env1Panel.h"
#include "Modules/Env2Panel.h"
#include "Modules/Env3Panel.h"
#include "Modules/Lfo1Panel.h"
#include "Modules/Lfo2Panel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Factories/WidgetFactory.h"


BottomPanel::~BottomPanel() = default;

BottomPanel::BottomPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
    , env1Panel_(std::make_unique<Env1Panel>(skin, widgetFactory, apvts))
    , env2Panel_(std::make_unique<Env2Panel>(skin, widgetFactory, apvts))
    , env3Panel_(std::make_unique<Env3Panel>(skin, widgetFactory, apvts))
    , lfo1Panel_(std::make_unique<Lfo1Panel>(skin, widgetFactory, apvts))
    , lfo2Panel_(std::make_unique<Lfo2Panel>(skin, widgetFactory, apvts))
{
    setOpaque(false);
    addAndMakeVisible(*env1Panel_);
    addAndMakeVisible(*env2Panel_);
    addAndMakeVisible(*env3Panel_);
    addAndMakeVisible(*lfo1Panel_);
    addAndMakeVisible(*lfo2Panel_);

    setSize(getWidth(), getHeight());
}

void BottomPanel::resized()
{
    const auto bounds = getLocalBounds();
    const auto spacing = getSpacing();
    
    const auto env1PanelX = 0;
    const auto env1PanelY = 0;
    env1Panel_->setBounds(
        bounds.getX() + env1PanelX,
        bounds.getY() + env1PanelY,
        Env1Panel::getWidth(),
        Env1Panel::getHeight()
    );
    
    const auto env2PanelX = env1PanelX + Env1Panel::getWidth() + spacing;
    const auto env2PanelY = 0;
    env2Panel_->setBounds(
        bounds.getX() + env2PanelX,
        bounds.getY() + env2PanelY,
        Env2Panel::getWidth(),
        Env2Panel::getHeight()
    );
    
    const auto env3PanelX = env2PanelX + Env2Panel::getWidth() + spacing;
    const auto env3PanelY = 0;
    env3Panel_->setBounds(
        bounds.getX() + env3PanelX,
        bounds.getY() + env3PanelY,
        Env3Panel::getWidth(),
        Env3Panel::getHeight()
    );
    
    const auto lfo1PanelX = env3PanelX + Env3Panel::getWidth() + spacing;
    const auto lfo1PanelY = 0;
    lfo1Panel_->setBounds(
        bounds.getX() + lfo1PanelX,
        bounds.getY() + lfo1PanelY,
        Lfo1Panel::getWidth(),
        Lfo1Panel::getHeight()
    );
    
    const auto lfo2PanelX = lfo1PanelX + Lfo1Panel::getWidth() + spacing;
    const auto lfo2PanelY = 0;
    lfo2Panel_->setBounds(
        bounds.getX() + lfo2PanelX,
        bounds.getY() + lfo2PanelY,
        Lfo2Panel::getWidth(),
        Lfo2Panel::getHeight()
    );
}

void BottomPanel::setSkin(tss::Skin& skin)
{
    skin_ = &skin;

    if (auto* panel = env1Panel_.get())
        panel->setSkin(skin);

    if (auto* panel = env2Panel_.get())
        panel->setSkin(skin);

    if (auto* panel = env3Panel_.get())
        panel->setSkin(skin);

    if (auto* panel = lfo1Panel_.get())
        panel->setSkin(skin);

    if (auto* panel = lfo2Panel_.get())
        panel->setSkin(skin);
}

