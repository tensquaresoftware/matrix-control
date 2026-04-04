#include "MainComponent.h"

#include <vector>

#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Layout/ScaledLayout.h"
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
    const float sf = uiDisplayScale_;

    const std::vector<int> designHeights {
        PluginDimensions::Panels::Header::kHeight,
        PluginDimensions::Panels::Body::kHeight,
        PluginDimensions::Panels::Footer::kHeight
    };
    const auto heights = tss::ScaledLayout::distributeHeights(bounds.getHeight(), designHeights, sf, 2);
    const int headerHeight = heights[0];
    const int bodyHeight = heights[1];
    const int footerHeight = heights[2];
    const int footerY = headerHeight + bodyHeight;

    headerPanel.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), headerHeight);
    bodyPanel.setBounds(bounds.getX(), bounds.getY() + headerHeight, bounds.getWidth(), bodyHeight);
    footerPanel.setBounds(bounds.getX(), bounds.getY() + footerY, bounds.getWidth(), footerHeight);
}

void MainComponent::setSkin(tss::Skin& skin)
{
    skin_ = &skin;
    headerPanel.setSkin(skin);
    bodyPanel.setSkin(skin);
    footerPanel.setSkin(skin);
}

void MainComponent::setDisplayScale(float displayScale)
{
    if (juce::approximatelyEqual(uiDisplayScale_, displayScale))
        return;
    
    uiDisplayScale_ = displayScale;
    headerPanel.setDisplayScale(uiDisplayScale_);
    bodyPanel.setDisplayScale(uiDisplayScale_);
    footerPanel.setDisplayScale(uiDisplayScale_);
    resized();
    repaint();
}

