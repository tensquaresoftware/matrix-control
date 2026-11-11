#include "Tools/SkinDesigner/SkinDesignerMainComponent.h"

#include "Tools/SkinDesigner/SkinDesignerChromeFonts.h"
#include "Tools/SkinDesigner/SkinDesignerChromePanel.h"
#include "Tools/SkinDesigner/SkinDesignerChromeTable.h"
#include "Tools/SkinDesigner/SkinDesignerUiHelpers.h"

SkinDesignerMainComponent::SkinDesignerMainComponent()
{
    setWantsKeyboardFocus(true);
    setLookAndFeel(&lookAndFeel_);
    // AlertWindow / MessageBoxOptions use the default LAF, not the component tree.
    juce::LookAndFeel::setDefaultLookAndFeel(&lookAndFeel_);
    SkinDesignerChrome::warmFonts();
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

SkinDesignerMainComponent::~SkinDesignerMainComponent()
{
    compareBlinkTimer_.stopTimer();
    colourPreviewCoalesceTimer_.stopTimer();
    importWatchTimer_.stopTimer();
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    setLookAndFeel(nullptr);
    flushPendingSessionAutosave();
    bridge_.stop();
}

void SkinDesignerMainComponent::setupTitleBarChrome()
{
    for (auto* label : { &mappingPrefixLabel_, &mappingNameLabel_, &mappingDetailLabel_ })
    {
        label->setFont(SkinDesignerChrome::fontRegular());
        label->setColour(juce::Label::textColourId, SkinDesignerChrome::textMuted());
        label->setJustificationType(juce::Justification::centredLeft);
        SkinDesignerChrome::preventTextSquish(*label);
        label->setBorderSize({});
        label->setTooltip({});
        label->setVisible(false);
        addAndMakeVisible(*label);
    }
    mappingNameLabel_.setColour(juce::Label::textColourId, SkinDesignerChrome::textPrimary());
}

void SkinDesignerMainComponent::setupStatusBarChrome()
{
    for (auto* label : { &connectionLabel_, &lastPushLabel_, &versionLabel_ })
    {
        label->setFont(SkinDesignerChrome::fontRegular());
        label->setColour(juce::Label::textColourId, SkinDesignerChrome::textMuted());
        SkinDesignerChrome::preventTextSquish(*label);
        label->setBorderSize({});
        label->setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(*label);
    }
    versionLabel_.setText("v" + juce::String(JUCE_APPLICATION_VERSION_STRING),
                          juce::dontSendNotification);
    addAndMakeVisible(lastPushBadgeHit_);
    jsonSyncNoticeCloseHit_.onClick = [this] { dismissJsonSyncReloadNotice(); };
    jsonSyncNoticeCloseHit_.onHoverChanged = [this]
    {
        if (! jsonSyncNoticeCloseBounds_.isEmpty())
            repaint(jsonSyncNoticeCloseBounds_);
    };
    addAndMakeVisible(jsonSyncNoticeCloseHit_);
    jsonSyncNoticeCloseHit_.setVisible(false);
}

void SkinDesignerMainComponent::setupSkinsPanel()
{
    skinsPanel_.onDuplicateProduct = [this] { onDuplicateProduct(); };
    skinsPanel_.onDuplicateDesigner = [this] { onDuplicateDesigner(); };
    skinsPanel_.onNew = [this] { onNew(); };
    skinsPanel_.onRename = [this] { onRename(); };
    skinsPanel_.onDelete = [this] { onDelete(); };
    skinsPanel_.productCombo().addListener(this);
    skinsPanel_.designerCombo().addListener(this);
    addAndMakeVisible(skinsPanel_);
}

void SkinDesignerMainComponent::setupBodyPanels()
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

void SkinDesignerMainComponent::setupLayerAndBridgeCallbacks()
{
    layer_.onChanged = [this]
    {
        markDesignerDirty();
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

void SkinDesignerMainComponent::paintChromeWells(juce::Graphics& g) const
{
    if (! sessionWellBounds_.isEmpty())
        SkinDesignerChrome::paintRoundedWell(g, sessionWellBounds_, SkinDesignerChrome::surfacePanel());
    if (! operationWellBounds_.isEmpty())
        SkinDesignerChrome::paintRoundedWell(g, operationWellBounds_, SkinDesignerChrome::surfacePanel());
    if (! operationContentBounds_.isEmpty())
        SkinDesignerChrome::paintRoundedWell(g, operationContentBounds_, SkinDesignerChrome::surfaceApp());

    g.setColour(SkinDesignerChrome::separatorLine().withAlpha(0.85f));
    for (const int x : sessionSeparatorXs_)
        if (x > 0)
            g.fillRect(x, sessionSeparatorY_, 1, sessionSeparatorH_);

    paintMappingWitnessDecorations(g);
}

void SkinDesignerMainComponent::paint(juce::Graphics& g)
{
    g.fillAll(SkinDesignerChrome::surfaceApp());
    paintChromeWells(g);

    if (! connectionBadgeBounds_.isEmpty())
    {
        const auto bridgeFill = bridge_.isConnected() ? SkinDesignerChrome::textMuted()
                                                      : SkinDesignerChrome::danger();
        SkinDesignerChrome::drawChromeBadge(g, connectionBadgeBounds_, "BRIDGE", bridgeFill);
    }
    if (! lastPushBadgeBounds_.isEmpty())
    {
        if (jsonSyncReloadNoticeVisible_)
            SkinDesignerChrome::drawChromeBadge(g,
                                              lastPushBadgeBounds_,
                                              "WARNING",
                                              SkinDesignerChrome::attention());
        else
            SkinDesignerChrome::drawChromeBadge(g, lastPushBadgeBounds_, "LAST PUSH");
    }
    if (! jsonSyncNoticeCloseBounds_.isEmpty())
    {
        const auto closeFill = jsonSyncNoticeCloseHit_.isHovered()
                                   ? SkinDesignerChrome::attention().brighter(0.25f)
                                   : SkinDesignerChrome::attention();
        SkinDesignerChrome::drawChromeBadge(g, jsonSyncNoticeCloseBounds_, "X", closeFill);
    }
    if (! appBadgeBounds_.isEmpty())
        SkinDesignerChrome::drawChromeBadge(g, appBadgeBounds_, "APP");
}

int SkinDesignerMainComponent::chromeRowY(int barY, int barH, int rowH, int opticalNudgeY) noexcept
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
        const auto shown = SkinDesignerChrome::elideToWidth(font, full, place.maxW);
        const int w = juce::jmin(place.maxW, SkinDesignerChrome::measureTextWidth(font, shown) + 2);
        label.setText(shown, juce::dontSendNotification);
        label.setBounds(place.x, place.rowY, w, place.rowH);
        return place.x + w;
    }
}

void SkinDesignerMainComponent::layoutOperationWitness(juce::Rectangle<int> content)
{
    // Order: badge | role | [swatch] identity | arrow | next-step
    const int rowH = SkinDesignerChrome::kMappingBadgeHeight;
    const int rowY = chromeRowY(content.getY(), content.getHeight(), rowH, 0);
    const int badgeW = mappingWitnessSameColourCollision_
                           ? SkinDesignerChrome::chromeBadgeWidth(
                                 SkinDesignerMappingPolicy::kSameColourBadge)
                           : SkinDesignerChrome::mappingModeBadgeWidth();
    const int sw = SkinDesignerChrome::kColourSwatchSize;
    const int arrowW = SkinDesignerChrome::kOperationArrowWidth;
    const auto bodyFont = SkinDesignerChrome::fontRegular();
    const int badgeGap = SkinDesignerChrome::kChromeBadgeGap;
    const int afterColon = SkinDesignerChrome::kOperationTightGap;
    const int swatchToName = SkinDesignerChrome::kChromeBadgeGap;
    const int arrowGap = SkinDesignerChrome::kOperationArrowGap;
    const int pad = SkinDesignerChrome::kOperationWitnessPad;
    const auto& detailFull = mappingWitnessDetailFull_;

    int x = content.getX() + pad;
    if (x + badgeW > content.getRight())
        return;

    mappingBadgeBounds_ = { x, rowY, badgeW, rowH };
    x += badgeW + badgeGap;

    // Role ends with " "; only a tight gap before the swatch (or identity when no swatch).
    const auto& roleFull = mappingWitnessRoleFull_;
    const int roleMax = juce::jmin(SkinDesignerChrome::measureTextWidth(bodyFont, roleFull) + 2,
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
    const int nextStepNeed = SkinDesignerChrome::measureTextWidth(bodyFont, detailFull) + 4;
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

void SkinDesignerMainComponent::layoutOperationStartHint(juce::Rectangle<int> content)
{
    const int rowH = SkinDesignerChrome::kMappingBadgeHeight;
    const int rowY = chromeRowY(content.getY(), content.getHeight(), rowH, 0);
    const int pad = SkinDesignerChrome::kOperationWitnessPad;
    const int badgeGap = SkinDesignerChrome::kChromeBadgeGap;
    const int badgeW = SkinDesignerChrome::chromeBadgeWidth(SkinDesignerUi::kNoDesignerSkinBadge);

    int x = content.getX() + pad;
    if (x + badgeW > content.getRight())
        return;

    mappingBadgeBounds_ = { x, rowY, badgeW, rowH };
    x += badgeW + badgeGap;

    placeElidedWitnessLabel({ &mappingDetailLabel_, x,
                              juce::jmax(0, content.getRight() - x - pad), rowY, rowH },
                            SkinDesignerChrome::fontRegular(),
                            mappingWitnessDetailFull_);
}

void SkinDesignerMainComponent::layoutOperationWell(juce::Rectangle<int> well)
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
    const int titleW = SkinDesignerChrome::titleTabWidth("OPERATION");
    if (well.getWidth() < titleW)
        return;

    operationTitleLabel_.setBounds(well.getX(), well.getY() - 1, titleW, well.getHeight() + 2);

    auto content = well;
    // No extra gap after tab: kTitleTabPadH already matches left title air.
    content.removeFromLeft(titleW);
    content.removeFromTop(SkinDesignerChrome::kGutter);
    content.removeFromBottom(SkinDesignerChrome::kGutter);
    content.removeFromRight(SkinDesignerChrome::kGutter);
    if (content.getWidth() < 1 || content.getHeight() < 1)
        return;

    operationContentBounds_ = content;
    if (mappingWitnessActive_)
        layoutOperationWitness(content);
    else if (operationStartHintActive_)
        layoutOperationStartHint(content);
}

void SkinDesignerMainComponent::layoutStatusBar(juce::Rectangle<int> statusBar)
{
    const int gap = SkinDesignerChrome::kChromeBadgeGap;
    const int badgeH = SkinDesignerChrome::kMappingBadgeHeight;
    const int badgeY = chromeRowY(statusBar.getY(), statusBar.getHeight(), badgeH,
                                  SkinDesignerChrome::kStatusChromeNudgeY);
    const auto statusFont = SkinDesignerChrome::fontRegular();
    const int connBadgeW = SkinDesignerChrome::chromeBadgeWidth("BRIDGE");
    const int pushBadgeW = SkinDesignerChrome::chromeBadgeWidth(
        jsonSyncReloadNoticeVisible_ ? "WARNING" : "LAST PUSH");
    const int appBadgeW = SkinDesignerChrome::chromeBadgeWidth("APP");
    const int versionW = SkinDesignerChrome::measureTextWidth(statusFont, versionLabel_.getText()) + 4;

    const int leftX = statusBar.getX();
    connectionBadgeBounds_ = { leftX, badgeY, connBadgeW, badgeH };
    // Align LAST PUSH column (and WARNING close) with Palette (Color width + side gap).
    const int pushColumnX = leftX + SkinDesignerChrome::kColorPanelWidth
                            + SkinDesignerChrome::kPaletteSideGap;
    if (jsonSyncReloadNoticeVisible_)
    {
        // [X] starts where LAST PUSH normally starts; WARNING + message shift right.
        jsonSyncNoticeCloseBounds_ = { pushColumnX, badgeY, badgeH, badgeH };
        lastPushBadgeBounds_ = { pushColumnX + badgeH + SkinDesignerChrome::kJsonSyncNoticeCloseGap,
                                 badgeY,
                                 pushBadgeW,
                                 badgeH };
    }
    else
    {
        jsonSyncNoticeCloseBounds_ = {};
        lastPushBadgeBounds_ = { pushColumnX, badgeY, pushBadgeW, badgeH };
    }
    const int appX = statusBar.getRight() - appBadgeW - gap - versionW;
    appBadgeBounds_ = { appX, badgeY, appBadgeW, badgeH };

    const int pushClusterLeft = pushColumnX;
    const int connLabelX = connectionBadgeBounds_.getRight() + gap;
    const int connLabelMaxW = juce::jmax(0, pushClusterLeft - gap - connLabelX);
    const auto connFull = bridge_.isConnected() ? "Connected to Matrix-Control"
                                                : "Disconnected from Matrix-Control";
    connectionLabel_.setText(SkinDesignerChrome::elideToWidth(statusFont, connFull, connLabelMaxW),
                             juce::dontSendNotification);
    connectionLabel_.setTooltip({});
    connectionLabel_.setBounds(connLabelX, badgeY, connLabelMaxW, badgeH);

    layoutStatusPushOrNoticeStrip(badgeY, badgeH, gap, statusFont);
    versionLabel_.setBounds(appX + appBadgeW + gap, badgeY, versionW, badgeH);
}

void SkinDesignerMainComponent::resized()
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
    const int edge = SkinDesignerChrome::kWindowEdgeGutter;
    // Left/right edge — title chrome / status align with SKINS / body columns.
    const auto titleBar = bounds.removeFromTop(SkinDesignerChrome::kTitleBarHeight)
                              .withTrimmedLeft(edge)
                              .withTrimmedRight(edge);
    layoutTitleBar(titleBar);

    const auto statusBar = bounds.removeFromBottom(SkinDesignerChrome::kStatusBarHeight)
                               .withTrimmedLeft(edge)
                               .withTrimmedRight(edge);
    layoutStatusBar(statusBar);

    // Body flush to status (COLOR / Palette / Mappings use bottomGutter 0).
    // Title-bar bottom gutter + this strip = kChromeStackGap before SKINS.
    bounds.removeFromTop(SkinDesignerChrome::kGutter);
    bounds.removeFromLeft(edge);
    bounds.removeFromRight(edge);
    layoutBodyPanels(bounds);
}
