#include "MainComponent.h"


#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDimensions.h"

using tss::SkinColourId;

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
        g.fillAll(skin_->getColour(SkinColourId::kBodyPanelBackground));
}

void MainComponent::resized()
{
    const auto bounds = getLocalBounds();
    const float headerHeight = static_cast<float>(PluginDimensions::Panels::Header::kHeight) * scalingFactor_;
    const float bodyHeight = static_cast<float>(PluginDimensions::Panels::Body::kHeight) * scalingFactor_;
    float y = 0.0f;
    
    layoutHeaderPanel(bounds, y);
    y += headerHeight;
    
    layoutBodyPanel(bounds, y);
    y += bodyHeight;
    
    layoutFooterPanel(bounds, y);
}

void MainComponent::layoutHeaderPanel(juce::Rectangle<int> bounds, float y)
{
    const float height = static_cast<float>(PluginDimensions::Panels::Header::kHeight) * scalingFactor_;
    headerPanel.setBounds(
        bounds.getX(),
        juce::roundToInt(static_cast<float>(bounds.getY()) + y),
        bounds.getWidth(),
        juce::roundToInt(height)
    );
}

void MainComponent::layoutBodyPanel(juce::Rectangle<int> bounds, float y)
{
    const float height = static_cast<float>(PluginDimensions::Panels::Body::kHeight) * scalingFactor_;
    bodyPanel.setBounds(
        bounds.getX(),
        juce::roundToInt(static_cast<float>(bounds.getY()) + y),
        bounds.getWidth(),
        juce::roundToInt(height)
    );
}

void MainComponent::layoutFooterPanel(juce::Rectangle<int> bounds, float y)
{
    const float height = static_cast<float>(PluginDimensions::Panels::Footer::kHeight) * scalingFactor_;
    footerPanel.setBounds(
        bounds.getX(),
        juce::roundToInt(static_cast<float>(bounds.getY()) + y),
        bounds.getWidth(),
        juce::roundToInt(height)
    );
}

void MainComponent::setSkin(tss::Skin& skin)
{
    skin_ = &skin;
    headerPanel.setSkin(skin);
    bodyPanel.setSkin(skin);
    footerPanel.setSkin(skin);
}

void MainComponent::setScalingFactor(float scalingFactor)
{
    if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
        return;
    
    scalingFactor_ = scalingFactor;
    headerPanel.setScalingFactor(scalingFactor_);
    bodyPanel.setScalingFactor(scalingFactor_);
    footerPanel.setScalingFactor(scalingFactor_);
    repaint();
}

