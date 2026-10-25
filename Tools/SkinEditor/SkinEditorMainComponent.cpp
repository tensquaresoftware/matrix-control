#include "Tools/SkinEditor/SkinEditorMainComponent.h"

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"
#include "Tools/SkinEditor/SkinEditorChromePanel.h"
#include "Tools/SkinEditor/SkinEditorChromeTable.h"

SkinEditorMainComponent::SkinEditorMainComponent()
{
    setWantsKeyboardFocus(true);
    setLookAndFeel(&lookAndFeel_);
    SkinEditorChrome::warmFonts();
    setupTitleBarChrome();
    setupStatusBarChrome();
    setupSessionWell();
    setupSkinsPanel();
    setupBodyPanels();
    setupLayerAndBridgeCallbacks();
    loadSessionOrBaseline();
    bridge_.start();
    refreshAll();
    // Second layout after the first visible size settle — catches any remaining
    // cold glyph-metric drift that warmFonts alone does not clear on some hosts.
    juce::MessageManager::callAsync([safe = juce::Component::SafePointer(this)]
    {
        if (safe != nullptr)
        {
            safe->resized();
            safe->repaint();
        }
    });
}

SkinEditorMainComponent::~SkinEditorMainComponent()
{
    compareBlinkTimer_.stopTimer();
    setLookAndFeel(nullptr);
    stopTimer();
    bridge_.stop();
}

void SkinEditorMainComponent::setupTitleBarChrome()
{
    titleLabel_.setText(SkinEditorChrome::kAppTitleText, juce::dontSendNotification);
    titleLabel_.setFont(SkinEditorChrome::fontAppTitle());
    titleLabel_.setColour(juce::Label::textColourId, SkinEditorChrome::textBrand());
    titleLabel_.setJustificationType(juce::Justification::centredLeft);
    SkinEditorChrome::preventTextSquish(titleLabel_);
    titleLabel_.setBorderSize({});
    addAndMakeVisible(titleLabel_);

    for (auto* label : { &mappingPrefixLabel_, &mappingDetailLabel_ })
    {
        label->setFont(SkinEditorChrome::fontRegular());
        label->setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
        label->setJustificationType(juce::Justification::centredLeft);
        SkinEditorChrome::preventTextSquish(*label);
        label->setBorderSize({});
        label->setVisible(false);
        addAndMakeVisible(*label);
    }
}

void SkinEditorMainComponent::setupStatusBarChrome()
{
    for (auto* label : { &connectionLabel_, &lastPushLabel_, &versionLabel_ })
    {
        label->setFont(SkinEditorChrome::fontRegular());
        label->setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
        SkinEditorChrome::preventTextSquish(*label);
        label->setBorderSize({});
        label->setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(*label);
    }
    versionLabel_.setText("v" + juce::String(JUCE_APPLICATION_VERSION_STRING),
                          juce::dontSendNotification);
}

void SkinEditorMainComponent::setupSkinsPanel()
{
    skinsPanel_.onDuplicateProduct = [this] { onDuplicateProduct(); };
    skinsPanel_.onDuplicateEditor = [this] { onDuplicateEditor(); };
    skinsPanel_.onNew = [this] { onNew(); };
    skinsPanel_.onRename = [this] { onRename(); };
    skinsPanel_.onDelete = [this] { onDelete(); };
    skinsPanel_.productCombo().addListener(this);
    skinsPanel_.editorCombo().addListener(this);
    addAndMakeVisible(skinsPanel_);
}

void SkinEditorMainComponent::setupBodyPanels()
{
    colourPicker_.onColourChanged = [this](juce::Colour c) { onColourChanged(c); };
    addAndMakeVisible(colourPicker_);

    palettePanel_.setLayer(&layer_);
    palettePanel_.onPastilleSelected = [this](const juce::String& name) { onPastilleSelected(name); };
    palettePanel_.onAddRequested = [this] { onAddPastille(); };
    palettePanel_.onDeleteRequested = [this] { onDeletePastille(); };
    const auto escapeMapping = [this]() -> bool
    {
        if (mappingMode_ == MappingMode::kIdle)
            return false;
        clearMappingModeToIdle();
        return true;
    };
    palettePanel_.onEscapeRequested = escapeMapping;
    addAndMakeVisible(palettePanel_);

    mappingsPanel_.setLayer(&layer_);
    mappingsPanel_.onRowActivated = [this](TSS::SkinColourKey key)
    {
        onMappingRowActivated(key);
    };
    mappingsPanel_.onEscapeRequested = escapeMapping;
    addAndMakeVisible(mappingsPanel_);
}

void SkinEditorMainComponent::setupLayerAndBridgeCallbacks()
{
    layer_.onChanged = [this]
    {
        markDirty();
        refreshAll();
        if (bridge_.isConnected() && ! isCompareProductView() && canEditEditorSkin())
            pushVisible();
        startTimer(400);
    };

    bridge_.onConnectionChanged = [this](bool)
    {
        refreshStatus();
        if (bridge_.isConnected())
            pushVisible();
    };
    bridge_.onApplyResult = [this](const TSS::DevSkin::ApplyResult& result)
    {
        if (result.ok)
            setStatusMessage("ack (rev " + juce::String(result.revision) + ")");
        else
            setStatusMessage("error [" + result.code + "] " + result.message);
        refreshStatus();
    };
}

void SkinEditorMainComponent::paint(juce::Graphics& g)
{
    g.fillAll(SkinEditorChrome::surfaceApp());

    if (! sessionWellBounds_.isEmpty())
        SkinEditorChrome::paintRoundedWell(g, sessionWellBounds_, SkinEditorChrome::surfacePanel());

    g.setColour(SkinEditorChrome::separatorLine().withAlpha(0.85f));
    for (const int x : sessionSeparatorXs_)
        if (x > 0)
            g.fillRect(x, sessionSeparatorY_, 1, sessionSeparatorH_);

    if (mappingWitnessActive_ && ! mappingBadgeBounds_.isEmpty())
        SkinEditorChrome::drawMappingModeBadge(g, mappingBadgeBounds_);

    if (mappingWitnessShowSwatch_ && ! mappingSwatchBounds_.isEmpty())
        SkinEditorChrome::paintColourSwatch(g, mappingSwatchBounds_, mappingWitnessColour_);

    if (! connectionBadgeBounds_.isEmpty())
        SkinEditorChrome::drawChromeBadge(g, connectionBadgeBounds_, "CONNECTION");
    if (! lastPushBadgeBounds_.isEmpty())
        SkinEditorChrome::drawChromeBadge(g, lastPushBadgeBounds_, "LAST PUSH");
    if (! appBadgeBounds_.isEmpty())
        SkinEditorChrome::drawChromeBadge(g, appBadgeBounds_, "APP");
}

int SkinEditorMainComponent::chromeRowY(int barY, int barH, int rowH, int opticalNudgeY) noexcept
{
    return barY + (barH - rowH) / 2 + opticalNudgeY;
}

void SkinEditorMainComponent::layoutTitleBar(juce::Rectangle<int> titleBar)
{
    const int titleH = juce::roundToInt(SkinEditorChrome::kAppTitleFont);
    const int titleW = SkinEditorChrome::measureTextWidth(SkinEditorChrome::fontAppTitle(),
                                                         titleLabel_.getText())
                       + 8;
    const int titleY = chromeRowY(titleBar.getY(), titleBar.getHeight(), titleH,
                                  SkinEditorChrome::kLogoTitleNudgeY);
    titleLabel_.setBounds(titleBar.getX(), titleY, titleW, titleH);
    layoutSessionWell(titleBar, titleBar.getX() + titleW);
}

void SkinEditorMainComponent::layoutMappingBand(juce::Rectangle<int> band)
{
    mappingBadgeBounds_ = {};
    mappingSwatchBounds_ = {};
    mappingPrefixLabel_.setBounds({});
    mappingDetailLabel_.setBounds({});
    if (! mappingWitnessActive_)
        return;

    const int rowH = SkinEditorChrome::kMappingBadgeHeight;
    const int rowY = chromeRowY(band.getY(), band.getHeight(), rowH,
                                SkinEditorChrome::kLogoChromeNudgeY);
    const int badgeW = SkinEditorChrome::mappingModeBadgeWidth();
    const int sw = SkinEditorChrome::kColourSwatchSize;
    const auto bodyFont = SkinEditorChrome::fontRegular();
    const int gap = SkinEditorChrome::kChromeBadgeGap;
    const auto prefixFull = mappingPrefixLabel_.getTooltip().isNotEmpty()
                                ? mappingPrefixLabel_.getTooltip()
                                : mappingPrefixLabel_.getText();
    const auto detailFull = mappingDetailLabel_.getTooltip().isNotEmpty()
                                ? mappingDetailLabel_.getTooltip()
                                : mappingDetailLabel_.getText();

    int x = band.getX();
    if (x + badgeW > band.getRight())
        return;

    mappingBadgeBounds_ = { x, rowY, badgeW, rowH };
    x += badgeW + gap;

    const int swatchReserve = mappingWitnessShowSwatch_ ? (sw + gap) : 0;
    const int prefixMax = juce::jmax(0, band.getRight() - x - gap - swatchReserve);
    const auto prefixShown = SkinEditorChrome::elideToWidth(bodyFont, prefixFull, prefixMax);
    const int prefixW = juce::jmin(prefixMax,
                                   SkinEditorChrome::measureTextWidth(bodyFont, prefixShown) + 2);
    mappingPrefixLabel_.setText(prefixShown, juce::dontSendNotification);
    mappingPrefixLabel_.setBounds(x, rowY, prefixW, rowH);
    x += prefixW + gap;

    if (mappingWitnessShowSwatch_)
    {
        if (x + sw <= band.getRight())
        {
            mappingSwatchBounds_ = { x, rowY + (rowH - sw) / 2, sw, sw };
            x += sw + gap;
        }
    }

    const int detailMax = juce::jmax(0, band.getRight() - x);
    const auto detailShown = SkinEditorChrome::elideToWidth(bodyFont, detailFull, detailMax);
    const int detailW = juce::jmin(detailMax,
                                   SkinEditorChrome::measureTextWidth(bodyFont, detailShown) + 4);
    mappingDetailLabel_.setText(detailShown, juce::dontSendNotification);
    mappingDetailLabel_.setBounds(x, rowY, detailW, rowH);
}

void SkinEditorMainComponent::layoutStatusBar(juce::Rectangle<int> statusBar)
{
    const int gap = SkinEditorChrome::kChromeBadgeGap;
    const int rowH = SkinEditorChrome::kMappingBadgeHeight;
    const int rowY = chromeRowY(statusBar.getY(), statusBar.getHeight(), rowH,
                                SkinEditorChrome::kStatusChromeNudgeY);
    const auto statusFont = SkinEditorChrome::fontRegular();
    const int connBadgeW = SkinEditorChrome::chromeBadgeWidth("CONNECTION");
    const int pushBadgeW = SkinEditorChrome::chromeBadgeWidth("LAST PUSH");
    const int appBadgeW = SkinEditorChrome::chromeBadgeWidth("APP");
    const int versionW = SkinEditorChrome::measureTextWidth(statusFont, versionLabel_.getText()) + 4;

    const int leftX = statusBar.getX();
    connectionBadgeBounds_ = { leftX, rowY, connBadgeW, rowH };
    lastPushBadgeBounds_ = { statusBar.getCentreX() - pushBadgeW / 2, rowY, pushBadgeW, rowH };
    const int appX = statusBar.getRight() - appBadgeW - gap - versionW;
    appBadgeBounds_ = { appX, rowY, appBadgeW, rowH };

    const int connLabelX = connectionBadgeBounds_.getRight() + gap;
    const int connLabelMaxW = juce::jmax(0, lastPushBadgeBounds_.getX() - gap - connLabelX);
    const auto connFull = connectionLabel_.getTooltip().isNotEmpty()
                              ? connectionLabel_.getTooltip()
                              : connectionLabel_.getText();
    connectionLabel_.setText(SkinEditorChrome::elideToWidth(statusFont, connFull, connLabelMaxW),
                             juce::dontSendNotification);
    connectionLabel_.setBounds(connLabelX, rowY, connLabelMaxW, rowH);

    const int pushLabelX = lastPushBadgeBounds_.getRight() + gap;
    const int pushLabelMaxW = juce::jmax(0, appBadgeBounds_.getX() - gap - pushLabelX);
    const auto pushFull = lastPushLabel_.getTooltip().isNotEmpty()
                              ? lastPushLabel_.getTooltip()
                              : lastPushLabel_.getText();
    lastPushLabel_.setText(SkinEditorChrome::elideToWidth(statusFont, pushFull, pushLabelMaxW),
                           juce::dontSendNotification);
    lastPushLabel_.setBounds(pushLabelX, rowY, pushLabelMaxW, rowH);

    versionLabel_.setBounds(appX + appBadgeW + gap, rowY, versionW, rowH);
}

void SkinEditorMainComponent::layoutBodyPanels(juce::Rectangle<int> bounds)
{
    const int gutter = SkinEditorChrome::kGutter;
    auto left = bounds.removeFromLeft(SkinEditorChrome::kColorPanelWidth);
    const int skinsH = SkinEditorSkinsPanel::preferredHeight();
    skinsPanel_.setBounds(left.removeFromTop(skinsH));
    left.removeFromTop(gutter);
    colourPicker_.setBounds(left);

    bounds.removeFromLeft(gutter);
    palettePanel_.setBounds(bounds.removeFromLeft(SkinEditorChrome::kPalettePanelWidth));
    bounds.removeFromLeft(gutter);
    mappingsPanel_.setBounds(bounds);
}

void SkinEditorMainComponent::resized()
{
    sessionSeparatorXs_.clear();
    sessionWellBounds_ = {};
    connectionBadgeBounds_ = {};
    lastPushBadgeBounds_ = {};
    appBadgeBounds_ = {};
    mappingBadgeBounds_ = {};
    mappingSwatchBounds_ = {};

    auto bounds = getLocalBounds();
    const auto titleBar = bounds.removeFromTop(SkinEditorChrome::kTitleBarHeight)
                              .withTrimmedLeft(SkinEditorChrome::kPanelPad)
                              .withTrimmedRight(SkinEditorChrome::kPanelPad);
    layoutTitleBar(titleBar);

    const auto mappingBand = bounds.removeFromTop(SkinEditorChrome::kMappingBandHeight)
                                 .withTrimmedLeft(SkinEditorChrome::kPanelPad)
                                 .withTrimmedRight(SkinEditorChrome::kPanelPad);
    layoutMappingBand(mappingBand);

    const auto statusBar = bounds.removeFromBottom(SkinEditorChrome::kStatusBarHeight)
                               .withTrimmedLeft(SkinEditorChrome::kPanelPad)
                               .withTrimmedRight(SkinEditorChrome::kPanelPad);
    layoutStatusBar(statusBar);

    bounds.removeFromTop(SkinEditorChrome::kGutter);
    bounds.removeFromBottom(SkinEditorChrome::kGutter);
    bounds.removeFromLeft(SkinEditorChrome::kGutter);
    bounds.removeFromRight(SkinEditorChrome::kGutter);
    layoutBodyPanels(bounds);
}
