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
    setupOperationWell();
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
    addAndMakeVisible(lastPushBadgeHit_);
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
            setStatusMessage("Acknowledgement / Rev " + juce::String(result.revision));
        else
            setStatusMessage("error [" + result.code + "] " + result.message);
        refreshStatus();
    };
}

void SkinEditorMainComponent::paintChromeWells(juce::Graphics& g) const
{
    if (! sessionWellBounds_.isEmpty())
        SkinEditorChrome::paintRoundedWell(g, sessionWellBounds_, SkinEditorChrome::surfacePanel());
    if (! operationWellBounds_.isEmpty())
        SkinEditorChrome::paintRoundedWell(g, operationWellBounds_, SkinEditorChrome::surfacePanel());
    if (! operationContentBounds_.isEmpty())
        SkinEditorChrome::paintRoundedWell(g, operationContentBounds_, SkinEditorChrome::surfaceApp());

    g.setColour(SkinEditorChrome::separatorLine().withAlpha(0.85f));
    for (const int x : sessionSeparatorXs_)
        if (x > 0)
            g.fillRect(x, sessionSeparatorY_, 1, sessionSeparatorH_);

    if (mappingWitnessActive_ && ! mappingBadgeBounds_.isEmpty())
        SkinEditorChrome::drawMappingModeBadge(g, mappingBadgeBounds_);
    if (mappingWitnessShowSwatch_ && ! mappingSwatchBounds_.isEmpty())
        SkinEditorChrome::paintColourSwatch(g, mappingSwatchBounds_, mappingWitnessColour_);
}

void SkinEditorMainComponent::paint(juce::Graphics& g)
{
    g.fillAll(SkinEditorChrome::surfaceApp());
    paintChromeWells(g);

    if (! connectionBadgeBounds_.isEmpty())
        SkinEditorChrome::drawChromeBadge(g, connectionBadgeBounds_, "BRIDGE");
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
    // Logo stays on the dark app fill — not inside the SESSION well.
    titleLabel_.setBounds(titleBar.getX(), titleY, titleW, titleH);
    layoutSessionWell(titleBar, titleBar.getX() + titleW);
}

void SkinEditorMainComponent::layoutOperationWitness(juce::Rectangle<int> content)
{
    const int rowH = SkinEditorChrome::kMappingBadgeHeight;
    const int rowY = chromeRowY(content.getY(), content.getHeight(), rowH, 0);
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

    const int pad = SkinEditorChrome::kOperationWitnessPad;
    int x = content.getX() + pad;
    if (x + badgeW > content.getRight())
        return;

    mappingBadgeBounds_ = { x, rowY, badgeW, rowH };
    x += badgeW + gap;

    const int swatchReserve = mappingWitnessShowSwatch_ ? (sw + gap) : 0;
    const int prefixMax = juce::jmax(0, content.getRight() - x - gap - swatchReserve);
    const auto prefixShown = SkinEditorChrome::elideToWidth(bodyFont, prefixFull, prefixMax);
    const int prefixW = juce::jmin(prefixMax,
                                   SkinEditorChrome::measureTextWidth(bodyFont, prefixShown) + 2);
    mappingPrefixLabel_.setText(prefixShown, juce::dontSendNotification);
    mappingPrefixLabel_.setBounds(x, rowY, prefixW, rowH);
    x += prefixW + gap;

    if (mappingWitnessShowSwatch_ && x + sw <= content.getRight())
    {
        mappingSwatchBounds_ = { x, rowY + (rowH - sw) / 2, sw, sw };
        x += sw + gap;
    }

    const int detailMax = juce::jmax(0, content.getRight() - x - pad);
    const auto detailShown = SkinEditorChrome::elideToWidth(bodyFont, detailFull, detailMax);
    const int detailW = juce::jmin(detailMax,
                                   SkinEditorChrome::measureTextWidth(bodyFont, detailShown) + 4);
    mappingDetailLabel_.setText(detailShown, juce::dontSendNotification);
    mappingDetailLabel_.setBounds(x, rowY, detailW, rowH);
}

void SkinEditorMainComponent::layoutOperationWell(juce::Rectangle<int> band)
{
    operationWellBounds_ = {};
    operationContentBounds_ = {};
    mappingBadgeBounds_ = {};
    mappingSwatchBounds_ = {};
    mappingPrefixLabel_.setBounds({});
    mappingDetailLabel_.setBounds({});
    operationTitleLabel_.setBounds({});

    if (band.isEmpty())
        return;

    operationWellBounds_ = band;
    const int titleW = SkinEditorChrome::titleTabWidth("OPERATION");
    if (band.getWidth() < titleW)
        return;

    operationTitleLabel_.setBounds(band.getX(), band.getY() - 1, titleW, band.getHeight() + 2);

    auto content = band;
    content.removeFromLeft(titleW + SkinEditorChrome::kButtonGap);
    content.removeFromTop(SkinEditorChrome::kGutter);
    content.removeFromBottom(SkinEditorChrome::kGutter);
    content.removeFromRight(SkinEditorChrome::kGutter);
    if (content.getWidth() < 1 || content.getHeight() < 1)
        return;

    operationContentBounds_ = content;
    if (mappingWitnessActive_)
        layoutOperationWitness(content);
}

void SkinEditorMainComponent::layoutStatusBar(juce::Rectangle<int> statusBar)
{
    const int gap = SkinEditorChrome::kChromeBadgeGap;
    const int badgeH = SkinEditorChrome::kMappingBadgeHeight;
    const int badgeY = chromeRowY(statusBar.getY(), statusBar.getHeight(), badgeH,
                                  SkinEditorChrome::kStatusChromeNudgeY);
    const auto statusFont = SkinEditorChrome::fontRegular();
    const int connBadgeW = SkinEditorChrome::chromeBadgeWidth("BRIDGE");
    const int pushBadgeW = SkinEditorChrome::chromeBadgeWidth("LAST PUSH");
    const int appBadgeW = SkinEditorChrome::chromeBadgeWidth("APP");
    const int versionW = SkinEditorChrome::measureTextWidth(statusFont, versionLabel_.getText()) + 4;

    const int leftX = statusBar.getX();
    connectionBadgeBounds_ = { leftX, badgeY, connBadgeW, badgeH };
    // Align LAST PUSH with Palette column (Color width + Palette side gap).
    const int pushBadgeX = leftX + SkinEditorChrome::kColorPanelWidth
                           + SkinEditorChrome::kPaletteSideGap;
    lastPushBadgeBounds_ = { pushBadgeX, badgeY, pushBadgeW, badgeH };
    const int appX = statusBar.getRight() - appBadgeW - gap - versionW;
    appBadgeBounds_ = { appX, badgeY, appBadgeW, badgeH };

    const int connLabelX = connectionBadgeBounds_.getRight() + gap;
    const int connLabelMaxW = juce::jmax(0, lastPushBadgeBounds_.getX() - gap - connLabelX);
    const auto connFull = bridge_.isConnected() ? "Connected to Matrix-Control"
                                                : "Disconnected from Matrix-Control";
    connectionLabel_.setText(SkinEditorChrome::elideToWidth(statusFont, connFull, connLabelMaxW),
                             juce::dontSendNotification);
    connectionLabel_.setTooltip({});
    connectionLabel_.setBounds(connLabelX, badgeY, connLabelMaxW, badgeH);

    const int pushLabelX = lastPushBadgeBounds_.getRight() + gap;
    const int pushLabelMaxW = juce::jmax(0, appBadgeBounds_.getX() - gap - pushLabelX);
    const auto pushFull = statusDetail_.isNotEmpty() ? statusDetail_ : "none";
    const auto pushShown = SkinEditorChrome::elidePathToWidth(statusFont, pushFull, pushLabelMaxW);
    const bool exportPathTip = pushFull.startsWith("Exported: ") && pushShown != pushFull;
    const auto pushTip = exportPathTip ? pushFull : juce::String {};
    lastPushLabel_.setText(pushShown, juce::dontSendNotification);
    lastPushLabel_.setTooltip(pushTip);
    lastPushLabel_.setBounds(pushLabelX, badgeY, pushLabelMaxW, badgeH);
    lastPushBadgeHit_.setBounds(lastPushBadgeBounds_);
    lastPushBadgeHit_.setTooltip(pushTip);

    versionLabel_.setBounds(appX + appBadgeW + gap, badgeY, versionW, badgeH);
}

void SkinEditorMainComponent::layoutBodyPanels(juce::Rectangle<int> bounds)
{
    auto left = bounds.removeFromLeft(SkinEditorChrome::kColorPanelWidth);
    const int skinsH = SkinEditorSkinsPanel::preferredHeight();
    const int colorH = SkinEditorColourPicker::preferredHeightForWidth(
        SkinEditorChrome::kColorPanelWidth);
    // SKINS↔COLOR: each panel's facing kGutter → kChromeStackGap. COLOR fixed height.
    skinsPanel_.setBounds(left.getX(), left.getY(), left.getWidth(), skinsH);
    colourPicker_.setBounds(left.getX(), left.getY() + skinsH, left.getWidth(), colorH);

    const int paletteGap = SkinEditorChrome::kPaletteSideGap;
    bounds.removeFromLeft(paletteGap);
    palettePanel_.setBounds(bounds.removeFromLeft(SkinEditorChrome::kPalettePanelWidth));
    bounds.removeFromLeft(paletteGap);
    mappingsPanel_.setBounds(bounds.removeFromLeft(SkinEditorChrome::kMappingsPanelWidth));
}

void SkinEditorMainComponent::resized()
{
    sessionSeparatorXs_.clear();
    sessionWellBounds_ = {};
    operationWellBounds_ = {};
    operationContentBounds_ = {};
    connectionBadgeBounds_ = {};
    lastPushBadgeBounds_ = {};
    appBadgeBounds_ = {};
    mappingBadgeBounds_ = {};
    mappingSwatchBounds_ = {};

    auto bounds = getLocalBounds();
    const int edge = SkinEditorChrome::kWindowEdgeGutter;
    // Left/right edge — SESSION / OPERATION / status align with SKINS / body columns.
    const auto titleBar = bounds.removeFromTop(SkinEditorChrome::kTitleBarHeight)
                              .withTrimmedLeft(edge)
                              .withTrimmedRight(edge);
    layoutTitleBar(titleBar);

    // Title-bar bottom gutter (6) + this strip (6) = kChromeStackGap before OPERATION.
    bounds.removeFromTop(SkinEditorChrome::kGutter);
    const auto operationBand = bounds.removeFromTop(SkinEditorChrome::kMappingBandHeight)
                                   .withTrimmedLeft(edge)
                                   .withTrimmedRight(edge);
    layoutOperationWell(operationBand);

    const auto statusBar = bounds.removeFromBottom(SkinEditorChrome::kStatusBarHeight)
                               .withTrimmedLeft(edge)
                               .withTrimmedRight(edge);
    layoutStatusBar(statusBar);

    // Body flush to status (COLOR / Palette / Mappings use bottomGutter 0).
    // OPERATION bottom + SKINS panel top gutter = kChromeStackGap.
    bounds.removeFromTop(SkinEditorChrome::kGutter);
    bounds.removeFromLeft(edge);
    bounds.removeFromRight(edge);
    layoutBodyPanels(bounds);
}
