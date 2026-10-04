#include "SettingsTabRail.h"

#include <memory>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Settings/SettingsShellMetrics.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinValues.h"
#include "Shared/Definitions/PluginIDs.h"

using TSS::SkinColourId;

SettingsTabRail::TabButton::TabButton(const juce::String& text)
    : juce::Button(text)
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void SettingsTabRail::TabButton::setLook(const TSS::LabelLook& look)
{
    look_ = look;
    repaint();
}

void SettingsTabRail::TabButton::setSelected(bool isSelected)
{
    if (selected_ == isSelected)
        return;

    selected_ = isSelected;
    repaint();
}

void SettingsTabRail::TabButton::setUiScale(float uiScale)
{
    uiScale_ = uiScale;
    repaint();
}

void SettingsTabRail::TabButton::setHoverFill(juce::Colour colour)
{
    hoverFill_ = colour;
    repaint();
}

void SettingsTabRail::TabButton::paintButton(juce::Graphics& g,
                                             bool shouldDrawButtonAsHighlighted,
                                             bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

    if (selected_)
        g.fillAll(hoverFill_);

    const int inset = TSS::ScaledLayout::scaledInt(8.0f, uiScale_);
    auto textBounds = getLocalBounds().reduced(inset, 0).toFloat();

    g.setColour(look_.text);
    g.setFont(look_.font.withHeight(look_.font.getHeight() * uiScale_));
    g.drawText(getButtonText(), textBounds, juce::Justification::centredLeft, false);
}

SettingsTabRail::SettingsTabRail(TSS::ISkin& skin, std::function<void(int)> onTabSelected)
    : skin_(&skin)
    , onTabSelected_(std::move(onTabSelected))
{
    using namespace PluginIDs::Settings::LastTab;

    for (int i = 0; i < kCount; ++i)
    {
        const int tabId = kFirst + i;
        tabs_[i] = std::make_unique<TabButton>(SettingsShellMetrics::tabLabel(i));
        tabs_[i]->onClick = [this, tabId]
        {
            setSelectedTab(tabId);

            if (onTabSelected_)
                onTabSelected_(tabId);
        };
        addAndMakeVisible(*tabs_[i]);
    }

    rebuildLooks();
    setSelectedTab(kUserInterface);
}

void SettingsTabRail::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildLooks();
    repaint();
}

void SettingsTabRail::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    rebuildLooks();
    resized();
    repaint();
}

void SettingsTabRail::setSelectedTab(int tabId)
{
    selectedTabId_ = PluginIDs::Settings::LastTab::normalize(tabId);

    using namespace PluginIDs::Settings::LastTab;

    for (int i = 0; i < kCount; ++i)
        tabs_[i]->setSelected((kFirst + i) == selectedTabId_);
}

void SettingsTabRail::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kBodyPanelBackground));
}

void SettingsTabRail::resized()
{
    auto inner = getLocalBounds();
    const int topPad = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kPadding), uiScale_);
    inner.removeFromTop(topPad);

    using namespace PluginIDs::Settings::LastTab;

    for (int i = 0; i < kCount; ++i)
    {
        tabs_[i]->setBounds(SettingsShellMetrics::tabRowBounds(i, inner, uiScale_));
        tabs_[i]->setUiScale(uiScale_);
    }
}

void SettingsTabRail::rebuildLooks()
{
    const auto labelLook = TSS::labelLookFromSkin(*skin_);
    const auto hoverFill = skin_->getPopupMenuBackgroundHooverColour(false);

    using namespace PluginIDs::Settings::LastTab;

    for (int i = 0; i < kCount; ++i)
    {
        tabs_[i]->setLook(labelLook);
        tabs_[i]->setHoverFill(hoverFill);
        tabs_[i]->setUiScale(uiScale_);
    }
}
