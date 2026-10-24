#include "Tools/SkinEditor/SkinEditorMainComponent.h"

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"
#include "Tools/SkinEditor/SkinEditorChromeTable.h"

SkinEditorMainComponent::SkinEditorMainComponent()
{
    setWantsKeyboardFocus(true);
    setLookAndFeel(&lookAndFeel_);
    setupTitleBarChrome();
    setupStatusBarChrome();
    setupSkinsBar();
    setupBodyPanels();
    setupLayerAndBridgeCallbacks();
    loadSessionOrBaseline();
    bridge_.start();
    refreshAll();
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
    titleLabel_.setBorderSize(juce::BorderSize<int>(5, 0, 0, 0));
    addAndMakeVisible(titleLabel_);

    mappingDetailLabel_.setFont(SkinEditorChrome::fontRegular());
    mappingDetailLabel_.setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
    mappingDetailLabel_.setJustificationType(juce::Justification::centredRight);
    SkinEditorChrome::preventTextSquish(mappingDetailLabel_);
    mappingDetailLabel_.setBorderSize(juce::BorderSize<int>(5, 0, 0, 0));
    mappingDetailLabel_.setVisible(false);
    addAndMakeVisible(mappingDetailLabel_);
}

void SkinEditorMainComponent::setupStatusBarChrome()
{
    for (auto* label : { &connectionLabel_, &lastPushLabel_, &versionLabel_ })
    {
        label->setFont(SkinEditorChrome::fontRegular());
        label->setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
        SkinEditorChrome::preventTextSquish(*label);
        label->setBorderSize(juce::BorderSize<int>(5, 0, 0, 0));
        addAndMakeVisible(*label);
    }
    connectionLabel_.setJustificationType(juce::Justification::centredLeft);
    lastPushLabel_.setJustificationType(juce::Justification::centredLeft);
    versionLabel_.setJustificationType(juce::Justification::centredRight);
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
            setStatusMessage("Last push: ack (rev " + juce::String(result.revision) + ")");
        else
            setStatusMessage("Last push: error [" + result.code + "] " + result.message);
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

    if (statusBarSeparatorX_ >= 0)
    {
        g.setColour(SkinEditorChrome::separatorLine().withAlpha(0.85f));
        g.fillRect(statusBarSeparatorX_, statusBarSeparatorY_, 1, statusBarSeparatorH_);
    }

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

void SkinEditorMainComponent::layoutTitleBar(juce::Rectangle<int> titleBar)
{
    const int titleW = SkinEditorChrome::measureTextWidth(SkinEditorChrome::fontAppTitle(),
                                                         titleLabel_.getText())
                       + 8;
    titleLabel_.setBounds(titleBar.getX(), titleBar.getY(), titleW, titleBar.getHeight());

    mappingBadgeBounds_ = {};
    mappingSwatchBounds_ = {};

    if (! mappingWitnessActive_)
    {
        mappingDetailLabel_.setBounds({});
        return;
    }

    constexpr int kGap = 8;
    layoutMappingWitness(titleBar, titleLabel_.getRight() + kGap);
}

void SkinEditorMainComponent::layoutMappingWitness(juce::Rectangle<int> titleBar, int leftLimit)
{
    constexpr int kGap = 8;
    int right = titleBar.getRight();
    const int badgeW = SkinEditorChrome::mappingModeBadgeWidth();
    const int badgeH = SkinEditorChrome::kMappingBadgeHeight;
    const int sw = SkinEditorChrome::kMappingWitnessSwatch;
    const int detailFullW = SkinEditorChrome::measureTextWidth(SkinEditorChrome::fontRegular(),
                                                              mappingDetailLabel_.getText())
                            + 4;

    int budget = juce::jmax(0, right - leftLimit);
    if (budget >= badgeW + kGap)
    {
        mappingBadgeBounds_ = juce::Rectangle<int>(right - badgeW,
                                                   titleBar.getCentreY() - badgeH / 2,
                                                   badgeW,
                                                   badgeH);
        right -= badgeW + kGap;
        budget = juce::jmax(0, right - leftLimit);
    }

    if (mappingWitnessShowSwatch_ && budget >= sw + kGap)
    {
        mappingSwatchBounds_ = juce::Rectangle<int>(right - sw,
                                                    titleBar.getCentreY() - sw / 2,
                                                    sw,
                                                    sw);
        right -= sw + kGap;
        budget = juce::jmax(0, right - leftLimit);
    }

    const int detailW = juce::jmin(detailFullW, budget);
    mappingDetailLabel_.setBounds(detailW > 0
                                      ? juce::Rectangle<int>(right - detailW, titleBar.getY(),
                                                             detailW, titleBar.getHeight())
                                      : juce::Rectangle<int>{});
}

void SkinEditorMainComponent::layoutStatusBar(juce::Rectangle<int> statusBar)
{
    const auto statusFont = SkinEditorChrome::fontRegular();
    const int versionW = SkinEditorChrome::measureTextWidth(statusFont, versionLabel_.getText()) + 4;
    versionLabel_.setBounds(statusBar.getRight() - versionW, statusBar.getY(),
                            versionW, statusBar.getHeight());

    constexpr int kStatusSepGap = 10;
    const int maxConnW = juce::jmax(0, versionLabel_.getX() - statusBar.getX() - kStatusSepGap * 2 - 40);
    const int connW = juce::jmin(SkinEditorChrome::measureTextWidth(statusFont, connectionLabel_.getText()),
                                 maxConnW);
    connectionLabel_.setBounds(statusBar.getX(), statusBar.getY(), connW, statusBar.getHeight());

    statusBarSeparatorX_ = statusBar.getX() + connW + kStatusSepGap;
    statusBarSeparatorY_ = statusBar.getCentreY() - 4;
    statusBarSeparatorH_ = 12;

    const int pushX = statusBarSeparatorX_ + 1 + kStatusSepGap;
    const int pushW = juce::jmax(0, versionLabel_.getX() - 8 - pushX);
    lastPushLabel_.setBounds(pushX, statusBar.getY(), pushW, statusBar.getHeight());
}

void SkinEditorMainComponent::layoutSkinsBar(juce::Rectangle<int> skins)
{
    skins.removeFromTop(SkinEditorChrome::kGutter);
    auto titleRow = skins.removeFromTop(SkinEditorChrome::kPanelTitleHeight);
    const int buttonY = titleRow.getY()
                        + (titleRow.getHeight() - SkinEditorChrome::kButtonHeight) / 2;
    auto row = juce::Rectangle<int>(titleRow.getX(), buttonY, titleRow.getWidth(),
                                    SkinEditorChrome::kButtonHeight);
    skinsBarSeparatorY_ = row.getY();
    skinsBarSeparatorH_ = row.getHeight();

    // Equal combos; width chosen so Import's right edge matches the pre-dual-Duplicate bar.
    constexpr int kSkinComboWidth = 100;
    const auto labelFont = SkinEditorChrome::fontRegular();
    const int skinsTitleW = SkinEditorChrome::titleTabWidth("SKINS");
    const int productLabelW = SkinEditorChrome::measureLabelWidth(productLabel_.getText(), labelFont);
    const int editorLabelW = SkinEditorChrome::measureLabelWidth(editorLabel_.getText(), labelFont);
    auto placeGap = [&row]() { row.removeFromLeft(SkinEditorChrome::kButtonGap); };
    auto placeButton = [&row](juce::TextButton& b)
    {
        b.setBounds(row.removeFromLeft(SkinEditorChrome::measureButtonWidth(b))
                        .withHeight(SkinEditorChrome::kButtonHeight));
    };
    auto placeSkinsBarSeparator = [this, &row, &placeGap]()
    {
        placeGap();
        placeGap();
        skinsBarSeparatorXs_.push_back(row.getX());
        row.removeFromLeft(1);
        placeGap();
        placeGap();
    };

    skinsTitleLabel_.setBounds(titleRow.getX(), titleRow.getY() - 1, skinsTitleW, titleRow.getHeight());
    row.removeFromLeft(skinsTitleW);
    placeGap();
    productLabel_.setBounds(row.removeFromLeft(productLabelW));
    productCombo_.setBounds(row.removeFromLeft(kSkinComboWidth));
    placeGap();
    placeButton(productDuplicateButton_);
    placeSkinsBarSeparator();
    editorLabel_.setBounds(row.removeFromLeft(editorLabelW));
    editorCombo_.setBounds(row.removeFromLeft(kSkinComboWidth));
    placeGap();
    placeButton(editorDuplicateButton_);
    placeGap();
    placeButton(newButton_);
    placeGap();
    placeButton(renameButton_);
    placeGap();
    placeButton(deleteButton_);
    layoutSkinsActionButtons(row);
}

void SkinEditorMainComponent::layoutSkinsActionButtons(juce::Rectangle<int>& row)
{
    auto placeGap = [&row]() { row.removeFromLeft(SkinEditorChrome::kButtonGap); };
    auto placeButton = [&row](juce::TextButton& b)
    {
        const int w = SkinEditorChrome::measureButtonWidth(b);
        b.setBounds(row.removeFromLeft(w).withHeight(SkinEditorChrome::kButtonHeight));
    };
    auto placeSkinsBarSeparator = [this, &row, &placeGap]()
    {
        placeGap();
        placeGap();
        skinsBarSeparatorXs_.push_back(row.getX());
        row.removeFromLeft(1);
        placeGap();
        placeGap();
    };

    placeSkinsBarSeparator();
    placeButton(undoButton_);
    placeGap();
    placeButton(redoButton_);
    placeSkinsBarSeparator();
    placeButton(compareButton_);
    placeGap();
    placeButton(pushButton_);
    placeSkinsBarSeparator();
    placeButton(exportButton_);
    placeGap();
    placeButton(importButton_);
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
    statusBarSeparatorX_ = -1;

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
