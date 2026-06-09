#include "MainComponent.h"

#include <vector>

#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/Skin.h"

using TSS::SkinColourId;

MainComponent::MainComponent(TSS::Skin& skin, const GuiLayoutDimensions& layoutDimensions, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
    , layoutDimensions_(layoutDimensions)
    , headerPanel(skin, layoutDimensions_.header.width, layoutDimensions_.header.height)
    , bodyPanel(skin, layoutDimensions_, widgetFactory, apvts)
    , footerPanel(skin, layoutDimensions_.footer.width, layoutDimensions_.footer.height, apvts)
{
    setOpaque(true);
    setSize(layoutDimensions_.editor.width, layoutDimensions_.editor.height);
    
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
        layoutDimensions_.header.height,
        layoutDimensions_.body.height,
        layoutDimensions_.footer.height
    };
    const auto heights = TSS::ScaledLayout::distributeHeights(bounds.getHeight(), designHeights, sf, 2);
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

void MainComponent::setSkin(TSS::Skin& skin)
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

void MainComponent::setBusReorderHandler(BusReorderHandler handler)
{
    bodyPanel.setBusReorderHandler(std::move(handler));
}
