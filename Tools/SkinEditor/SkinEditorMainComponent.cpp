#include "Tools/SkinEditor/SkinEditorMainComponent.h"

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"
#include "Tools/SkinEditor/SkinEditorChromePanel.h"
#include "Tools/SkinEditor/SkinEditorChromeTable.h"
#include "Tools/SkinEditor/SkinEditorUiHelpers.h"

SkinEditorMainComponent::SkinEditorMainComponent()
{
    setWantsKeyboardFocus(true);
    setLookAndFeel(&lookAndFeel_);
    // AlertWindow / MessageBoxOptions use the default LAF, not the component tree.
    juce::LookAndFeel::setDefaultLookAndFeel(&lookAndFeel_);
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
    // Nested clicks (gutters / Color / Palette / …) can clear Mappings row selection.
    addMouseListener(this, true);
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
    colourPreviewCoalesceTimer_.stopTimer();
    importWatchTimer_.stopTimer();
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    setLookAndFeel(nullptr);
    flushPendingSessionAutosave();
    bridge_.stop();
}

void SkinEditorMainComponent::setupTitleBarChrome()
{
    for (auto* label : { &mappingPrefixLabel_, &mappingNameLabel_, &mappingDetailLabel_ })
    {
        label->setFont(SkinEditorChrome::fontRegular());
        label->setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
        label->setJustificationType(juce::Justification::centredLeft);
        SkinEditorChrome::preventTextSquish(*label);
        label->setBorderSize({});
        label->setTooltip({});
        label->setVisible(false);
        addAndMakeVisible(*label);
    }
    mappingNameLabel_.setColour(juce::Label::textColourId, SkinEditorChrome::textPrimary());
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
    jsonSyncNoticeCloseHit_.onClick = [this] { dismissJsonSyncReloadNotice(); };
    addAndMakeVisible(jsonSyncNoticeCloseHit_);
    jsonSyncNoticeCloseHit_.setVisible(false);
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
    const auto escapeMapping = [this]() -> bool
    {
        if (mappingMode_ == MappingMode::kIdle)
            return false;
        clearMappingModeToIdle();
        return true;
    };

    wireColourPicker(escapeMapping);

    palettePanel_.setLayer(&layer_);
    palettePanel_.onPastilleSelected = [this](const juce::String& name) { onPastilleSelected(name); };
    palettePanel_.onMappingCountClicked = [this](const juce::String& name)
    {
        mappingsPanel_.revealPastilleMappings(name);
    };
    palettePanel_.onAddRequested = [this] { onAddPastille(); };
    palettePanel_.onDuplicateRequested = [this] { onDuplicatePastille(); };
    palettePanel_.onDeleteRequested = [this] { onDeletePastille(); };
    palettePanel_.onRenamePastilleRequested = [this](const juce::String& name)
    {
        onRenamePastille(name);
    };
    palettePanel_.onEscapeRequested = escapeMapping;
    addAndMakeVisible(palettePanel_);

    mappingsPanel_.setLayer(&layer_);
    mappingsPanel_.onRowActivated = [this](TSS::SkinColourKey key)
    {
        onMappingRowActivated(key);
    };
    mappingsPanel_.onMappedColourClicked = [this](TSS::SkinColourKey key)
    {
        onMappedColourClicked(key);
    };
    mappingsPanel_.onHoldColourChanged = [this]
    {
        updateMappingWitness();
    };
    mappingsPanel_.onEscapeRequested = escapeMapping;
    addAndMakeVisible(mappingsPanel_);
}

void SkinEditorMainComponent::setupLayerAndBridgeCallbacks()
{
    layer_.onChanged = [this]
    {
        markEditorDirty();
        scheduleSessionAutosave();
        if (colourGestureActive_)
        {
            scheduleColourPreviewCoalesce();
            return;
        }
        applyColourPreviewRefresh();
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

    paintMappingWitnessDecorations(g);
}

void SkinEditorMainComponent::paint(juce::Graphics& g)
{
    g.fillAll(SkinEditorChrome::surfaceApp());
    paintChromeWells(g);

    if (! connectionBadgeBounds_.isEmpty())
    {
        const auto bridgeFill = bridge_.isConnected() ? SkinEditorChrome::textMuted()
                                                      : SkinEditorChrome::danger();
        SkinEditorChrome::drawChromeBadge(g, connectionBadgeBounds_, "BRIDGE", bridgeFill);
    }
    if (! lastPushBadgeBounds_.isEmpty())
    {
        if (jsonSyncReloadNoticeVisible_)
            SkinEditorChrome::drawChromeBadge(g,
                                              lastPushBadgeBounds_,
                                              "WARNING",
                                              SkinEditorChrome::attention());
        else
            SkinEditorChrome::drawChromeBadge(g, lastPushBadgeBounds_, "LAST PUSH");
    }
    if (! jsonSyncNoticeCloseBounds_.isEmpty())
        SkinEditorChrome::drawChromeBadge(g,
                                          jsonSyncNoticeCloseBounds_,
                                          "X",
                                          SkinEditorChrome::attention());
    if (! appBadgeBounds_.isEmpty())
        SkinEditorChrome::drawChromeBadge(g, appBadgeBounds_, "APP");
}

int SkinEditorMainComponent::chromeRowY(int barY, int barH, int rowH, int opticalNudgeY) noexcept
{
    return barY + (barH - rowH) / 2 + opticalNudgeY;
}

namespace
{
    struct WitnessElidePlace
    {
        juce::Label* label = nullptr;
        int x = 0;
        int maxW = 0;
        int rowY = 0;
        int rowH = 0;
    };

    int placeElidedWitnessLabel(WitnessElidePlace place,
                                const juce::Font& font,
                                const juce::String& full)
    {
        auto& label = *place.label;
        const auto shown = SkinEditorChrome::elideToWidth(font, full, place.maxW);
        const int w = juce::jmin(place.maxW, SkinEditorChrome::measureTextWidth(font, shown) + 2);
        label.setText(shown, juce::dontSendNotification);
        label.setBounds(place.x, place.rowY, w, place.rowH);
        return place.x + w;
    }
}

void SkinEditorMainComponent::layoutOperationWitness(juce::Rectangle<int> content)
{
    // Order: badge | role | [swatch] identity | arrow | next-step
    const int rowH = SkinEditorChrome::kMappingBadgeHeight;
    const int rowY = chromeRowY(content.getY(), content.getHeight(), rowH, 0);
    const int badgeW = mappingWitnessSameColourCollision_
                           ? SkinEditorChrome::chromeBadgeWidth(
                                 SkinEditorMappingPolicy::kSameColourBadge)
                           : SkinEditorChrome::mappingModeBadgeWidth();
    const int sw = SkinEditorChrome::kColourSwatchSize;
    const int arrowW = SkinEditorChrome::kOperationArrowWidth;
    const auto bodyFont = SkinEditorChrome::fontRegular();
    const int badgeGap = SkinEditorChrome::kChromeBadgeGap;
    const int afterColon = SkinEditorChrome::kOperationTightGap;
    const int swatchToName = SkinEditorChrome::kChromeBadgeGap;
    const int arrowGap = SkinEditorChrome::kOperationArrowGap;
    const int pad = SkinEditorChrome::kOperationWitnessPad;
    const auto& detailFull = mappingWitnessDetailFull_;

    int x = content.getX() + pad;
    if (x + badgeW > content.getRight())
        return;

    mappingBadgeBounds_ = { x, rowY, badgeW, rowH };
    x += badgeW + badgeGap;

    // Role ends with " "; only a tight gap before the swatch (or identity when no swatch).
    const auto& roleFull = mappingWitnessRoleFull_;
    const int roleMax = juce::jmin(SkinEditorChrome::measureTextWidth(bodyFont, roleFull) + 2,
                                   juce::jmax(0, content.getRight() - x - pad));
    x = placeElidedWitnessLabel({ &mappingPrefixLabel_, x, roleMax, rowY, rowH }, bodyFont, roleFull)
        + afterColon;

    if (mappingWitnessShowSwatch_ && x + sw <= content.getRight())
    {
        mappingSwatchBounds_ = { x, rowY + (rowH - sw) / 2, sw, sw };
        x += sw + swatchToName;
    }

    // Identity yields first so next-step coaching stays readable (never below half).
    const int available = juce::jmax(0, content.getRight() - x - (arrowGap + arrowW + arrowGap + pad));
    const int nextStepNeed = SkinEditorChrome::measureTextWidth(bodyFont, detailFull) + 4;
    const int nameMax = juce::jmax(available / 2, available - nextStepNeed);
    x = placeElidedWitnessLabel({ &mappingNameLabel_, x, nameMax, rowY, rowH },
                                bodyFont,
                                mappingWitnessIdentityFull_)
        + arrowGap;

    if (x + arrowW <= content.getRight() - pad)
    {
        mappingArrowBounds_ = { x, rowY, arrowW, rowH };
        x += arrowW + arrowGap;
    }

    placeElidedWitnessLabel({ &mappingDetailLabel_, x, juce::jmax(0, content.getRight() - x - pad),
                              rowY, rowH },
                            bodyFont,
                            detailFull);
}

void SkinEditorMainComponent::layoutOperationStartHint(juce::Rectangle<int> content)
{
    const int rowH = SkinEditorChrome::kMappingBadgeHeight;
    const int rowY = chromeRowY(content.getY(), content.getHeight(), rowH, 0);
    const int pad = SkinEditorChrome::kOperationWitnessPad;
    const int badgeGap = SkinEditorChrome::kChromeBadgeGap;
    const int badgeW = SkinEditorChrome::chromeBadgeWidth(SkinEditorUi::kNoEditorSkinBadge);

    int x = content.getX() + pad;
    if (x + badgeW > content.getRight())
        return;

    mappingBadgeBounds_ = { x, rowY, badgeW, rowH };
    x += badgeW + badgeGap;

    placeElidedWitnessLabel({ &mappingDetailLabel_, x,
                              juce::jmax(0, content.getRight() - x - pad), rowY, rowH },
                            SkinEditorChrome::fontRegular(),
                            mappingWitnessDetailFull_);
}

void SkinEditorMainComponent::layoutOperationWell(juce::Rectangle<int> well)
{
    operationWellBounds_ = {};
    operationContentBounds_ = {};
    mappingBadgeBounds_ = {};
    mappingSwatchBounds_ = {};
    mappingArrowBounds_ = {};
    mappingPrefixLabel_.setBounds({});
    mappingNameLabel_.setBounds({});
    mappingDetailLabel_.setBounds({});
    operationTitleLabel_.setBounds({});

    if (well.isEmpty())
        return;

    operationWellBounds_ = well;
    const int titleW = SkinEditorChrome::titleTabWidth("OPERATION");
    if (well.getWidth() < titleW)
        return;

    operationTitleLabel_.setBounds(well.getX(), well.getY() - 1, titleW, well.getHeight() + 2);

    auto content = well;
    // No extra gap after tab: kTitleTabPadH already matches left title air.
    content.removeFromLeft(titleW);
    content.removeFromTop(SkinEditorChrome::kGutter);
    content.removeFromBottom(SkinEditorChrome::kGutter);
    content.removeFromRight(SkinEditorChrome::kGutter);
    if (content.getWidth() < 1 || content.getHeight() < 1)
        return;

    operationContentBounds_ = content;
    if (mappingWitnessActive_)
        layoutOperationWitness(content);
    else if (operationStartHintActive_)
        layoutOperationStartHint(content);
}

void SkinEditorMainComponent::layoutStatusBar(juce::Rectangle<int> statusBar)
{
    const int gap = SkinEditorChrome::kChromeBadgeGap;
    const int badgeH = SkinEditorChrome::kMappingBadgeHeight;
    const int badgeY = chromeRowY(statusBar.getY(), statusBar.getHeight(), badgeH,
                                  SkinEditorChrome::kStatusChromeNudgeY);
    const auto statusFont = SkinEditorChrome::fontRegular();
    const int connBadgeW = SkinEditorChrome::chromeBadgeWidth("BRIDGE");
    const int pushBadgeW = SkinEditorChrome::chromeBadgeWidth(
        jsonSyncReloadNoticeVisible_ ? "WARNING" : "LAST PUSH");
    const int appBadgeW = SkinEditorChrome::chromeBadgeWidth("APP");
    const int versionW = SkinEditorChrome::measureTextWidth(statusFont, versionLabel_.getText()) + 4;

    const int leftX = statusBar.getX();
    connectionBadgeBounds_ = { leftX, badgeY, connBadgeW, badgeH };
    // Align LAST PUSH / WARNING with Palette column (Color width + Palette side gap).
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

    layoutStatusPushOrNoticeStrip(badgeY, badgeH, gap, statusFont);
    versionLabel_.setBounds(appX + appBadgeW + gap, badgeY, versionW, badgeH);
}

void SkinEditorMainComponent::resized()
{
    sessionSeparatorXs_.clear();
    sessionWellBounds_ = {};
    operationWellBounds_ = {};
    operationContentBounds_ = {};
    connectionBadgeBounds_ = {};
    lastPushBadgeBounds_ = {};
    jsonSyncNoticeCloseBounds_ = {};
    appBadgeBounds_ = {};
    mappingBadgeBounds_ = {};
    mappingSwatchBounds_ = {};
    mappingArrowBounds_ = {};

    auto bounds = getLocalBounds();
    const int edge = SkinEditorChrome::kWindowEdgeGutter;
    // Left/right edge — title chrome / status align with SKINS / body columns.
    const auto titleBar = bounds.removeFromTop(SkinEditorChrome::kTitleBarHeight)
                              .withTrimmedLeft(edge)
                              .withTrimmedRight(edge);
    layoutTitleBar(titleBar);

    const auto statusBar = bounds.removeFromBottom(SkinEditorChrome::kStatusBarHeight)
                               .withTrimmedLeft(edge)
                               .withTrimmedRight(edge);
    layoutStatusBar(statusBar);

    // Body flush to status (COLOR / Palette / Mappings use bottomGutter 0).
    // Title-bar bottom gutter + this strip = kChromeStackGap before SKINS.
    bounds.removeFromTop(SkinEditorChrome::kGutter);
    bounds.removeFromLeft(edge);
    bounds.removeFromRight(edge);
    layoutBodyPanels(bounds);
}
