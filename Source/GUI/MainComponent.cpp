#include "MainComponent.h"

#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDimensions.h"

MainComponent::MainComponent(tss::Skin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
    , headerPanel(skin, PluginDimensions::Panels::Header::kWidth, PluginDimensions::Panels::Header::kHeight)
    , bodyPanel(skin, PluginDimensions::GUI::kWidth, PluginDimensions::Panels::Body::kHeight, widgetFactory, apvts)
    , footerPanel(skin, PluginDimensions::Panels::Footer::kWidth, PluginDimensions::Panels::Footer::kHeight, apvts)
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
    y += PluginDimensions::Panels::Header::kHeight;
    
    layoutBodyPanel(bounds, y);
    y += PluginDimensions::Panels::Body::kHeight;
    
    layoutFooterPanel(bounds, y);
}

void MainComponent::layoutHeaderPanel(juce::Rectangle<int> bounds, int y)
{
    headerPanel.setBounds(
        bounds.getX(),
        bounds.getY() + y,
        bounds.getWidth(),
        PluginDimensions::Panels::Header::kHeight
    );
}

void MainComponent::layoutBodyPanel(juce::Rectangle<int> bounds, int y)
{
    bodyPanel.setBounds(
        bounds.getX(),
        bounds.getY() + y,
        bounds.getWidth(),
        PluginDimensions::Panels::Body::kHeight
    );
}

void MainComponent::layoutFooterPanel(juce::Rectangle<int> bounds, int y)
{
    footerPanel.setBounds(
        bounds.getX(),
        bounds.getY() + y,
        bounds.getWidth(),
        PluginDimensions::Panels::Footer::kHeight
    );
}

void MainComponent::setSkin(tss::Skin& skin)
{
    skin_ = &skin;
    headerPanel.setSkin(skin);
    bodyPanel.setSkin(skin);
    footerPanel.setSkin(skin);
}

