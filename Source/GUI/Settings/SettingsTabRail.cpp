#include "SettingsTabRail.h"

#include <memory>

#include "GUI/Dialogs/DialogMatrixHelpers.h"
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

void SettingsTabRail::TabButton::setSelectedFill(juce::Colour fill, juce::Colour text)
{
    selectedFill_ = fill;
    selectedText_ = text;
    repaint();
}

void SettingsTabRail::TabButton::paintButton(juce::Graphics& g,
                                             bool shouldDrawButtonAsHighlighted,
                                             bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

    if (selected_)
        g.fillAll(selectedFill_);

    const int inset = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kPadding), uiScale_);
    auto textBounds = getLocalBounds().reduced(inset, 0).toFloat();

    g.setColour(selected_ ? selectedText_ : look_.text);
    g.setFont(look_.font.withHeight(look_.font.getHeight() * uiScale_));
    g.drawText(getButtonText(), textBounds, juce::Justification::centredLeft, false);
}

SettingsTabRail::SettingsTabRail(TSS::ISkin& skin,
                                 bool isPluginMode,
                                 std::function<void(int)> onTabSelected)
    : skin_(&skin)
    , isPluginMode_(isPluginMode)
    , onTabSelected_(std::move(onTabSelected))
{
    using namespace PluginIDs::Settings::LastTab;

    const int n = count(isPluginMode_);
    tabs_.reserve(static_cast<size_t>(n));

    for (int i = 0; i < n; ++i)
    {
        const int tabId = idAt(i, isPluginMode_);
        auto button = std::make_unique<TabButton>(SettingsShellMetrics::tabLabel(i, isPluginMode_));
        button->onClick = [this, tabId]
        {
            setSelectedTab(tabId);

            if (onTabSelected_)
                onTabSelected_(tabId);
        };
        addAndMakeVisible(*button);
        tabs_.push_back(std::move(button));
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
    selectedTabId_ = PluginIDs::Settings::LastTab::normalize(tabId, isPluginMode_);

    using namespace PluginIDs::Settings::LastTab;

    for (int i = 0; i < static_cast<int>(tabs_.size()); ++i)
        tabs_[static_cast<size_t>(i)]->setSelected(idAt(i, isPluginMode_) == selectedTabId_);
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

    for (int i = 0; i < static_cast<int>(tabs_.size()); ++i)
    {
        tabs_[static_cast<size_t>(i)]->setBounds(SettingsShellMetrics::tabRowBounds(i, inner, uiScale_));
        tabs_[static_cast<size_t>(i)]->setUiScale(uiScale_);
    }
}

void SettingsTabRail::rebuildLooks()
{
    const auto labelLook = TSS::labelLookFromSkin(*skin_);
    const auto selectedFill = juce::Colour(DialogMatrixHelpers::kModalTitleBandColour);
    const auto selectedText = skin_->getColour(SkinColourId::kButtonTextOff);

    for (auto& tab : tabs_)
    {
        tab->setLook(labelLook);
        tab->setSelectedFill(selectedFill, selectedText);
        tab->setUiScale(uiScale_);
    }
}
