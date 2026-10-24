#include "Tools/SkinEditor/SkinEditorMainComponent.h"

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"
#include "Tools/SkinEditor/SkinEditorChromeTable.h"

SkinEditorMainComponent::SkinEditorMainComponent()
{
    setWantsKeyboardFocus(true);
    setLookAndFeel(&lookAndFeel_);
    SkinEditorChrome::warmFonts();
    setupTitleBarChrome();
    setupStatusBarChrome();
    setupSkinsBar();
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
    titleLabel_.setText("MATRIX-CONTROL SKIN EDITOR", juce::dontSendNotification);
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

void SkinEditorMainComponent::setupSkinsBar()
{
    SkinEditorChrome::stylePanelTitle(skinsTitleLabel_, "SKINS");
    addAndMakeVisible(skinsTitleLabel_);

    for (auto* label : { &productLabel_, &editorLabel_ })
    {
        label->setFont(SkinEditorChrome::fontRegular());
        label->setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
        SkinEditorChrome::preventTextSquish(*label);
        addAndMakeVisible(*label);
    }
    productCombo_.addListener(this);
    editorCombo_.addListener(this);
    addAndMakeVisible(productCombo_);
    addAndMakeVisible(editorCombo_);

    auto wire = [this](juce::TextButton& button, auto handler)
    {
        addAndMakeVisible(button);
        button.onClick = handler;
    };
    wire(productDuplicateButton_, [this] { onDuplicateProduct(); });
    wire(editorDuplicateButton_, [this] { onDuplicateEditor(); });
    wire(newButton_, [this] { onNew(); });
    wire(renameButton_, [this] { onRename(); });
    wire(deleteButton_, [this] { onDelete(); });
    wire(undoButton_, [this] { onUndo(); });
    wire(redoButton_, [this] { onRedo(); });
    wire(compareButton_, [this] { onCompareToggle(); });
    wire(pushButton_, [this] { onPush(); });
    wire(exportButton_, [this] { onExportJson(); });
    wire(importButton_, [this] { onImportJson(); });
    compareBlinkTimer_.onTick = [this] { tickCompareBlink(); };
    undoButton_.setEnabled(false);
    redoButton_.setEnabled(false);
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
        onMappingRowActivated(key, false);
    };
    mappingsPanel_.onEscapeRequested = escapeMapping;
    addAndMakeVisible(mappingsPanel_);

    unboundPanel_.setLayer(&layer_);
    unboundPanel_.onRowActivated = [this](TSS::SkinColourKey key)
    {
        onMappingRowActivated(key, true);
    };
    unboundPanel_.onEscapeRequested = escapeMapping;
    addAndMakeVisible(unboundPanel_);
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

    auto bounds = getLocalBounds();
    bounds.removeFromTop(SkinEditorChrome::kTitleBarHeight);
    bounds.removeFromBottom(SkinEditorChrome::kStatusBarHeight);

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

    bounds.removeFromTop(SkinEditorChrome::kGutter);
    bounds.removeFromLeft(SkinEditorChrome::kGutter);
    bounds.removeFromRight(SkinEditorChrome::kGutter);
    auto skins = bounds.removeFromTop(SkinEditorChrome::kSkinsPanelHeight);
    g.setColour(SkinEditorChrome::surfacePanel());
    g.fillRoundedRectangle(skins.toFloat(), SkinEditorChrome::kControlCorner);

    g.setColour(SkinEditorChrome::separatorLine().withAlpha(0.85f));
    for (const int x : skinsBarSeparatorXs_)
        if (x > 0)
            g.fillRect(x, skinsBarSeparatorY_, 1, skinsBarSeparatorH_);
}

int SkinEditorMainComponent::chromeRowY(int barY, int barH, int rowH, int opticalNudgeY) noexcept
{
    return barY + (barH - rowH) / 2 + opticalNudgeY;
}

int SkinEditorMainComponent::mappingsPanelLeftX() noexcept
{
    // Matches layoutBodyPanels after left gutter (see resized).
    return SkinEditorChrome::kGutter
           + SkinEditorChrome::kColorPanelWidth
           + SkinEditorChrome::kGutter
           + SkinEditorChrome::kPalettePanelWidth
           + SkinEditorChrome::kGutter;
}

void SkinEditorMainComponent::layoutTitleBar(juce::Rectangle<int> titleBar)
{
    const int rowH = SkinEditorChrome::kMappingBadgeHeight;
    const int titleH = juce::roundToInt(SkinEditorChrome::kAppTitleFont);
    const int titleW = SkinEditorChrome::measureTextWidth(SkinEditorChrome::fontAppTitle(),
                                                         titleLabel_.getText())
                       + 8;
    const int titleY = chromeRowY(titleBar.getY(), titleBar.getHeight(), titleH,
                                  SkinEditorChrome::kLogoTitleNudgeY);
    titleLabel_.setBounds(titleBar.getX(), titleY, titleW, titleH);

    mappingBadgeBounds_ = {};
    mappingSwatchBounds_ = {};
    mappingPrefixLabel_.setBounds({});
    mappingDetailLabel_.setBounds({});
    if (mappingWitnessActive_)
    {
        const int rowY = chromeRowY(titleBar.getY(), titleBar.getHeight(), rowH,
                                    SkinEditorChrome::kLogoChromeNudgeY);
        layoutMappingWitness(rowY, rowH, titleBar.getRight());
    }
}

void SkinEditorMainComponent::layoutMappingWitness(int rowY, int rowH, int rightLimit)
{
    constexpr int kGap = 8;
    const int badgeW = SkinEditorChrome::mappingModeBadgeWidth();
    const int sw = SkinEditorChrome::kColourSwatchSize;
    const auto bodyFont = SkinEditorChrome::fontRegular();
    const int prefixW = SkinEditorChrome::measureTextWidth(bodyFont, mappingPrefixLabel_.getText()) + 2;
    const int detailW = SkinEditorChrome::measureTextWidth(bodyFont, mappingDetailLabel_.getText()) + 4;

    // Start at Mappings panel left + inset. Shared integer rowY for all.
    int x = mappingsPanelLeftX() + SkinEditorChrome::kMappingWitnessInsetX;
    if (x + badgeW > rightLimit)
        return;

    mappingBadgeBounds_ = { x, rowY, badgeW, rowH };
    x += badgeW + kGap;

    mappingPrefixLabel_.setBounds(x, rowY, prefixW, rowH);
    x += prefixW + kGap;

    if (mappingWitnessShowSwatch_)
    {
        mappingSwatchBounds_ = { x, rowY + (rowH - sw) / 2, sw, sw };
        x += sw + kGap;
    }

    mappingDetailLabel_.setBounds(x, rowY, detailW, rowH);
}

void SkinEditorMainComponent::layoutStatusBar(juce::Rectangle<int> statusBar)
{
    constexpr int kGap = 8;
    const int rowH = SkinEditorChrome::kMappingBadgeHeight;
    const int rowY = chromeRowY(statusBar.getY(), statusBar.getHeight(), rowH,
                                SkinEditorChrome::kStatusChromeNudgeY);
    const auto statusFont = SkinEditorChrome::fontRegular();
    const int connBadgeW = SkinEditorChrome::chromeBadgeWidth("CONNECTION");
    const int pushBadgeW = SkinEditorChrome::chromeBadgeWidth("LAST PUSH");
    const int appBadgeW = SkinEditorChrome::chromeBadgeWidth("APP");
    const int versionW = SkinEditorChrome::measureTextWidth(statusFont, versionLabel_.getText()) + 4;

    // Fixed badge anchors: left / centre / right. Labels use the slots between them.
    const int leftX = statusBar.getX();
    connectionBadgeBounds_ = { leftX, rowY, connBadgeW, rowH };
    lastPushBadgeBounds_ = { statusBar.getCentreX() - pushBadgeW / 2, rowY, pushBadgeW, rowH };
    const int appX = statusBar.getRight() - appBadgeW - kGap - versionW;
    appBadgeBounds_ = { appX, rowY, appBadgeW, rowH };

    const int connLabelX = connectionBadgeBounds_.getRight() + kGap;
    const int connLabelMaxW = juce::jmax(0, lastPushBadgeBounds_.getX() - kGap - connLabelX);
    const auto connFull = connectionLabel_.getTooltip().isNotEmpty()
                              ? connectionLabel_.getTooltip()
                              : connectionLabel_.getText();
    connectionLabel_.setText(SkinEditorChrome::elideToWidth(statusFont, connFull, connLabelMaxW),
                             juce::dontSendNotification);
    connectionLabel_.setBounds(connLabelX, rowY, connLabelMaxW, rowH);

    const int pushLabelX = lastPushBadgeBounds_.getRight() + kGap;
    const int pushLabelMaxW = juce::jmax(0, appBadgeBounds_.getX() - kGap - pushLabelX);
    const auto pushFull = lastPushLabel_.getTooltip().isNotEmpty()
                              ? lastPushLabel_.getTooltip()
                              : lastPushLabel_.getText();
    lastPushLabel_.setText(SkinEditorChrome::elideToWidth(statusFont, pushFull, pushLabelMaxW),
                           juce::dontSendNotification);
    lastPushLabel_.setBounds(pushLabelX, rowY, pushLabelMaxW, rowH);

    versionLabel_.setBounds(appX + appBadgeW + kGap, rowY, versionW, rowH);
}

void SkinEditorMainComponent::layoutBodyPanels(juce::Rectangle<int> bounds)
{
    const int gutter = SkinEditorChrome::kGutter;
    colourPicker_.setBounds(bounds.removeFromLeft(SkinEditorChrome::kColorPanelWidth));
    bounds.removeFromLeft(gutter);
    palettePanel_.setBounds(bounds.removeFromLeft(SkinEditorChrome::kPalettePanelWidth));
    bounds.removeFromLeft(gutter);
    const int mapW = juce::jmax(0, (bounds.getWidth() - gutter) / 2);
    mappingsPanel_.setBounds(bounds.removeFromLeft(mapW));
    bounds.removeFromLeft(gutter);
    unboundPanel_.setBounds(bounds);
}

void SkinEditorMainComponent::resized()
{
    skinsBarSeparatorXs_.clear();
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

    const auto statusBar = bounds.removeFromBottom(SkinEditorChrome::kStatusBarHeight)
                               .withTrimmedLeft(SkinEditorChrome::kPanelPad)
                               .withTrimmedRight(SkinEditorChrome::kPanelPad);
    layoutStatusBar(statusBar);

    bounds.removeFromTop(SkinEditorChrome::kGutter);
    bounds.removeFromBottom(SkinEditorChrome::kGutter);
    bounds.removeFromLeft(SkinEditorChrome::kGutter);
    bounds.removeFromRight(SkinEditorChrome::kGutter);
    layoutSkinsBar(bounds.removeFromTop(SkinEditorChrome::kSkinsPanelHeight));

    bounds.removeFromTop(SkinEditorChrome::kGutter);
    layoutBodyPanels(bounds);
}
