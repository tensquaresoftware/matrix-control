#include "MainComponent.h"

#include <vector>

#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDesignDimensions.h"

using tss::SkinColourId;

MainComponent::MainComponent(tss::Skin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
    , headerPanel(skin, PluginDesignDimensions::Panels::Header::kWidth, PluginDesignDimensions::Panels::Header::kHeight)
    , bodyPanel(skin, PluginDesignDimensions::GUI::kWidth, PluginDesignDimensions::Panels::Body::kHeight, widgetFactory, apvts)
    , footerPanel(skin, PluginDesignDimensions::Panels::Footer::kWidth, PluginDesignDimensions::Panels::Footer::kHeight, apvts)
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
    const float sf = uiScale_;

    const std::vector<int> designHeights {
        PluginDesignDimensions::Panels::Header::kHeight,
        PluginDesignDimensions::Panels::Body::kHeight,
        PluginDesignDimensions::Panels::Footer::kHeight
    };
    const auto heights = tss::ScaledLayout::distributeHeights(bounds.getHeight(), designHeights, sf, 2);
    const int headerHeight = heights[0];
    const int bodyHeight = heights[1];
    const int footerHeight = heights[2];
    const int footerY = headerHeight + bodyHeight;

    headerPanel.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), headerHeight);
    uiElementsTestAreaY_ = headerHeight;

    if (uiElementsTestVisible_)
    {
        bodyPanel.setVisible(false);
        footerPanel.setVisible(false);
        return;
    }

    bodyPanel.setVisible(true);
    footerPanel.setVisible(true);
    bodyPanel.setBounds(bounds.getX(), bounds.getY() + headerHeight, bounds.getWidth(), bodyHeight);
    footerPanel.setBounds(bounds.getX(), bounds.getY() + footerY, bounds.getWidth(), footerHeight);
}

void MainComponent::setUiElementsTestVisible(bool visible)
{
    if (uiElementsTestVisible_ == visible)
        return;

    uiElementsTestVisible_ = visible;
    resized();
}

juce::Rectangle<int> MainComponent::getUiElementsTestAreaBounds() const
{
    const auto bounds = getLocalBounds();
    return bounds.withTrimmedTop(uiElementsTestAreaY_);
}

void MainComponent::setSkin(tss::Skin& skin)
{
    skin_ = &skin;
    headerPanel.setSkin(skin);
    bodyPanel.setSkin(skin);
    footerPanel.setSkin(skin);
}

void MainComponent::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    headerPanel.setUiScale(uiScale_);
    bodyPanel.setUiScale(uiScale_);
    footerPanel.setUiScale(uiScale_);
    resized();
    repaint();
}

