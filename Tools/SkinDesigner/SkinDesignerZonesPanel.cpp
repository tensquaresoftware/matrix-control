#include "Tools/SkinDesigner/SkinDesignerZonesPanel.h"

#include "Tools/SkinDesigner/SkinDesignerChrome.h"

namespace
{
    void styleMutedLabel(juce::Label& label, const juce::String& text, bool header)
    {
        label.setText(text, juce::dontSendNotification);
        label.setFont(SkinDesignerChrome::fontRegular());
        label.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        label.setColour(juce::Label::textColourId,
                        header ? SkinDesignerChrome::textTableHeader()
                               : SkinDesignerChrome::textTableBody());
        label.setJustificationType(juce::Justification::centredLeft);
        SkinDesignerChrome::preventTextSquish(label);
    }

    int measureSubTabWidth(const juce::Label& label)
    {
        return SkinDesignerChrome::measureTextWidth(SkinDesignerChrome::fontRegular(),
                                                    label.getText())
             + SkinDesignerChrome::kButtonGap * 2;
    }
}

void SkinDesignerZonesPanel::buildTitleChrome()
{
    SkinDesignerChrome::styleWorkSurfaceTabTitle(peerTitleLabel_,
                                                 SkinDesignerChrome::kMappingsWorkSurfaceTitle,
                                                 false);
    addAndMakeVisible(peerTitleLabel_);
    SkinDesignerChrome::styleWorkSurfaceTabTitle(titleLabel_,
                                                 SkinDesignerChrome::kZonesWorkSurfaceTitle,
                                                 true);
    addAndMakeVisible(titleLabel_);

    resetButton_.onClick = [this] { resetToDefaults(); };
    content_.addAndMakeVisible(resetButton_);
}

void SkinDesignerZonesPanel::buildSubTabs()
{
    addAndMakeVisible(matrixSubTab_);
    addAndMakeVisible(sharedSubTab_);
    addAndMakeVisible(exceptionsSubTab_);
    styleSubTab(matrixSubTab_, true);
    styleSubTab(sharedSubTab_, false);
    styleSubTab(exceptionsSubTab_, false);

    introLabel_.setFont(SkinDesignerChrome::fontRegular());
    introLabel_.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    introLabel_.setColour(juce::Label::textColourId, SkinDesignerChrome::textMuted());
    introLabel_.setJustificationType(juce::Justification::topLeft);
    SkinDesignerChrome::preventTextSquish(introLabel_);
    addAndMakeVisible(introLabel_);
    updateIntroText();
}

void SkinDesignerZonesPanel::buildReadOnlySections()
{
    styleMutedLabel(exceptionObjectHeader_, "Object", true);
    styleMutedLabel(locationHeader_, "Location", true);
    styleMutedLabel(usesHeader_, "Uses", true);
    exceptionObjectHeader_.setFont(SkinDesignerChrome::fontBold());
    locationHeader_.setFont(SkinDesignerChrome::fontBold());
    usesHeader_.setFont(SkinDesignerChrome::fontBold());
    exceptionObjectHeader_.setJustificationType(juce::Justification::centredLeft);
    locationHeader_.setJustificationType(juce::Justification::centredLeft);
    usesHeader_.setJustificationType(juce::Justification::centredLeft);
    const juce::BorderSize<int> headerPad (0, SkinDesignerChrome::kMappingsColumnPad, 0,
                                           SkinDesignerChrome::kMappingsColumnPad);
    exceptionObjectHeader_.setBorderSize(headerPad);
    locationHeader_.setBorderSize(headerPad);
    usesHeader_.setBorderSize(headerPad);
    content_.addAndMakeVisible(exceptionObjectHeader_);
    content_.addAndMakeVisible(locationHeader_);
    content_.addAndMakeVisible(usesHeader_);
}

SkinDesignerZonesPanel::SkinDesignerZonesPanel()
{
    buildTitleChrome();
    buildSubTabs();
    buildMatrixHeaders();
    buildReadOnlySections();
    rebuildUsedCells();
    rebuildUcsLabels();
    rebuildExceptionLabels();
    applySubPageVisibility();

    content_.setOpaque(false);
    viewport_.setViewedComponent(&content_, false);
    viewport_.setScrollBarsShown(true, false);
    viewport_.setScrollBarThickness(SkinDesignerChrome::kScrollTrackWidth);
    viewport_.setColour(juce::ScrollBar::backgroundColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(viewport_);
}

void SkinDesignerZonesPanel::setLayer(TSS::DevSkin::Layer* layer)
{
    layer_ = layer;
}

void SkinDesignerZonesPanel::setActiveSkinId(const juce::String& skinId)
{
    if (skinId_ == skinId)
        return;
    skinId_ = skinId;
}

void SkinDesignerZonesPanel::setReadOnly(bool readOnly)
{
    readOnly_ = readOnly;
    resetButton_.setEnabled(! readOnly_ && skinId_.isNotEmpty());
    for (auto& cell : usedCells_)
        if (cell.combo != nullptr)
            cell.combo->setEnabled(! readOnly_ && skinId_.isNotEmpty());
}

void SkinDesignerZonesPanel::refresh()
{
    setReadOnly(readOnly_);
    syncCombosFromMap();
    resized();
    repaint();
}

void SkinDesignerZonesPanel::paintSubTabUnderline(juce::Graphics& g) const
{
    if (matrixSubTab_.getBounds().isEmpty())
        return;

    constexpr int kSegmentH = 3;
    constexpr int kNameToSegmentGap = 1;
    const int y = matrixSubTab_.getBottom() + kNameToSegmentGap;

    const auto paintSegment = [&g, y](juce::Rectangle<int> tab, bool active)
    {
        if (tab.isEmpty())
            return;
        g.setColour(active ? SkinDesignerChrome::attention()
                           : SkinDesignerChrome::separatorLine());
        g.fillRect(tab.getX(), y, tab.getWidth(), kSegmentH);
    };

    paintSegment(matrixSubTab_.getBounds(), subPage_ == SubPage::kMatrix);
    paintSegment(sharedSubTab_.getBounds(), subPage_ == SubPage::kShared);
    paintSegment(exceptionsSubTab_.getBounds(), subPage_ == SubPage::kExceptions);
}

void SkinDesignerZonesPanel::paint(juce::Graphics& g)
{
    SkinDesignerChrome::paintWorkSurfacePanelFrame(
        g,
        { getLocalBounds(), peerTitleBounds_, titleTabBounds_, true, 0 });
    SkinDesignerChrome::paintRoundedWell(g, viewport_.getBounds(), SkinDesignerChrome::surfacePanel());
    paintSubTabUnderline(g);
}

void SkinDesignerZonesPanel::resized()
{
    const auto chrome = SkinDesignerChrome::layoutWorkSurfaceTabs(getLocalBounds(), 0);
    peerTitleBounds_ = chrome.mappingsTab;
    titleTabBounds_ = chrome.zonesTab;
    peerTitleLabel_.setBounds(chrome.mappingsTab);
    titleLabel_.setBounds(chrome.zonesTab);

    auto content = chrome.content;
    auto subRow = content.removeFromTop(SkinDesignerChrome::kButtonHeight);
    const int matrixW = measureSubTabWidth(matrixSubTab_);
    const int sharedW = measureSubTabWidth(sharedSubTab_);
    const int exceptionsW = measureSubTabWidth(exceptionsSubTab_);
    matrixSubTab_.setBounds(subRow.removeFromLeft(matrixW));
    subRow.removeFromLeft(SkinDesignerChrome::kButtonGap);
    sharedSubTab_.setBounds(subRow.removeFromLeft(sharedW));
    subRow.removeFromLeft(SkinDesignerChrome::kButtonGap);
    exceptionsSubTab_.setBounds(subRow.removeFromLeft(exceptionsW));
    // Air under the names then segment height — keeps intro clear of underlines.
    constexpr int kNameToSegmentGap = 1;
    constexpr int kSegmentH = 3;
    content.removeFromTop(kNameToSegmentGap + kSegmentH);

    content.removeFromTop(SkinDesignerChrome::kPanelContentPad);
    // Height follows the current intro copy so MCS / UCS / Exceptions share the same
    // air under the last intro line (fixed introH left a taller empty band on short copy).
    juce::TextLayout introLayout;
    {
        juce::AttributedString as (introLabel_.getText());
        as.setFont(SkinDesignerChrome::fontRegular());
        as.setColour(SkinDesignerChrome::textMuted());
        introLayout.createLayout(as, (float) juce::jmax(1, content.getWidth()));
    }
    const int introH = juce::jmax(1, juce::roundToInt(introLayout.getHeight()));
    introLabel_.setBounds(content.removeFromTop(introH));
    // Same air under every intro (MCS / UCS / Exceptions).
    content.removeFromTop(SkinDesignerChrome::kPanelContentPad * 2);

    auto listBounds = content;
    listBounds.setRight(listBounds.getRight() + SkinDesignerChrome::kPanelContentPad);
    viewport_.setBounds(listBounds);
    layoutContent();
}

void SkinDesignerZonesPanel::mouseUp(const juce::MouseEvent& e)
{
    if (! e.mouseWasClicked())
        return;

    const auto pos = e.getPosition();
    if (peerTitleBounds_.contains(pos) && onPeerWorkSurfaceRequested != nullptr)
    {
        onPeerWorkSurfaceRequested();
        return;
    }
    if (matrixSubTab_.getBounds().contains(pos))
    {
        setSubPage(SubPage::kMatrix);
        return;
    }
    if (sharedSubTab_.getBounds().contains(pos))
    {
        setSubPage(SubPage::kShared);
        return;
    }
    if (exceptionsSubTab_.getBounds().contains(pos))
        setSubPage(SubPage::kExceptions);
}
