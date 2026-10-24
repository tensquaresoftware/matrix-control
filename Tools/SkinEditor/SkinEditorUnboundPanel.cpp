#include "Tools/SkinEditor/SkinEditorUnboundPanel.h"

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinEditor/SkinEditorChrome.h"

namespace
{
    /** Air between title glyphs and the count badge (tab side pads use kTitleTabPadH). */
    constexpr int kTitleBadgeGap = 4;
}

SkinEditorUnboundPanel::SkinEditorUnboundPanel()
{
    SkinEditorChrome::stylePanelTitle(titleLabel_, "UNBOUND");
    addAndMakeVisible(titleLabel_);

    // Same body-row height as Mappings / Palette visual rows (token name is historical).
    list_.setRowHeight(SkinEditorChrome::kColumnHeaderHeight);
    list_.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(list_);
}

void SkinEditorUnboundPanel::setLayer(TSS::DevSkin::Layer* layer)
{
    layer_ = layer;
}

void SkinEditorUnboundPanel::setActiveSkinId(const juce::String& skinId)
{
    skinId_ = skinId;
}

void SkinEditorUnboundPanel::setReadOnly(bool readOnly)
{
    readOnly_ = readOnly;
}

void SkinEditorUnboundPanel::setEmptyForCompareProduct(bool empty)
{
    emptyForCompareProduct_ = empty;
}

void SkinEditorUnboundPanel::setSelectedPastilleName(const juce::String& name)
{
    selectedPastille_ = name;
}

void SkinEditorUnboundPanel::refresh()
{
    keys_.clear();
    if (! emptyForCompareProduct_ && layer_ != nullptr)
        keys_ = layer_->unboundKeys(skinId_);

    list_.deselectAllRows();
    list_.updateContent();
    list_.repaint();
    resized();
    repaint();
}

void SkinEditorUnboundPanel::paint(juce::Graphics& g)
{
    SkinEditorChrome::paintPanelFrame(g, getLocalBounds(), titleTabBounds_);
    SkinEditorChrome::paintRoundedWell(g, list_.getBounds(), SkinEditorChrome::surfacePanel());
    const int count = emptyForCompareProduct_ ? 0 : static_cast<int>(keys_.size());
    SkinEditorChrome::drawCountBadge(g, badgeBounds_, count);
}

void SkinEditorUnboundPanel::resized()
{
    const int count = emptyForCompareProduct_ ? 0 : static_cast<int>(keys_.size());
    const int badgeW = SkinEditorChrome::countBadgeWidth(count);
    const int pad = SkinEditorChrome::kTitleTabPadH;
    const int textW = SkinEditorChrome::measureTextWidth(SkinEditorChrome::fontTitle(), "UNBOUND");
    // | pad | UNBOUND | gap | badge | pad | — right pad matches left; tab grows with badge.
    const int tabW = pad + textW + kTitleBadgeGap + badgeW + pad;
    const auto chrome = SkinEditorChrome::layoutPanel(getLocalBounds(), tabW);
    titleTabBounds_ = chrome.tab;

    auto tab = chrome.tab;
    tab.removeFromLeft(pad);
    titleLabel_.setBounds(tab.removeFromLeft(textW));
    tab.removeFromLeft(kTitleBadgeGap);
    badgeBounds_ = tab.removeFromLeft(badgeW)
                       .withSizeKeepingCentre(badgeW, SkinEditorChrome::kCountBadgeDiameter)
                       .translated(0, 1);
    list_.setBounds(chrome.content);
}

int SkinEditorUnboundPanel::getNumRows()
{
    return static_cast<int>(keys_.size());
}

void SkinEditorUnboundPanel::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected)
{
    if (row < 0 || row >= static_cast<int>(keys_.size()))
        return;
    if (selected)
    {
        g.setColour(juce::Colour(0xff303030));
        g.fillRoundedRectangle(0.0f, 0.0f, (float) width, (float) height,
                               SkinEditorChrome::kControlCorner);
    }
    g.setColour(SkinEditorChrome::textTableBody());
    g.setFont(SkinEditorChrome::fontRegular());
    g.drawText(TSS::DevSkin::keyLabel(keys_[static_cast<size_t>(row)]),
               6, 0, width - 12, height, juce::Justification::centredLeft, false);
}

void SkinEditorUnboundPanel::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    tryBindRow(row);
}

void SkinEditorUnboundPanel::tryBindRow(int row)
{
    if (readOnly_ || emptyForCompareProduct_ || onBindRequested == nullptr)
        return;
    if (row < 0 || row >= static_cast<int>(keys_.size()))
        return;

    const auto key = keys_[static_cast<size_t>(row)];
    if (selectedPastille_.isEmpty())
    {
        if (onBindNeedsPastille)
            onBindNeedsPastille(key);
        return;
    }

    onBindRequested(key, selectedPastille_);
}
