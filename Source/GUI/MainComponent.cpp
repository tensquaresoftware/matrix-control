#include "MainComponent.h"

#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Skins/Skin.h"

MainComponent::MainComponent(tss::Skin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
    , headerPanel(skin)
    , bodyPanel(skin, widgetFactory, apvts)
    , footerPanel(skin, apvts)
{
    setOpaque(true);
    setSize(width, height);
    
    addAndMakeVisible(headerPanel);
    addAndMakeVisible(bodyPanel);
    addAndMakeVisible(footerPanel);
}

void MainComponent::paint(juce::Graphics& g)
{
    if (skin_ != nullptr)
        g.fillAll(skin_->getBodyPanelBackgroundColour());
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    auto y = 0;
    
    layoutHeaderPanel(bounds, y);
    y += HeaderPanel::getHeight();
    
    layoutBodyPanel(bounds, y);
    y += BodyPanel::getHeight();
    
    layoutFooterPanel(bounds, y);
}

void MainComponent::layoutHeaderPanel(juce::Rectangle<int> bounds, int y)
{
    headerPanel.setBounds(
        bounds.getX(),
        bounds.getY() + y,
        bounds.getWidth(),
        HeaderPanel::getHeight()
    );
}

void MainComponent::layoutBodyPanel(juce::Rectangle<int> bounds, int y)
{
    bodyPanel.setBounds(
        bounds.getX(),
        bounds.getY() + y,
        bounds.getWidth(),
        BodyPanel::getHeight()
    );
}

void MainComponent::layoutFooterPanel(juce::Rectangle<int> bounds, int y)
{
    footerPanel.setBounds(
        bounds.getX(),
        bounds.getY() + y,
        bounds.getWidth(),
        FooterPanel::getHeight()
    );
}

void MainComponent::setSkin(tss::Skin& skin)
{
    skin_ = &skin;
    headerPanel.setSkin(skin);
    bodyPanel.setSkin(skin);
    footerPanel.setSkin(skin);
}

