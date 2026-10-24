#include "Tools/SkinEditor/SkinEditorMainComponent.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"

SkinEditorMainComponent::SkinEditorMainComponent()
{
    setWantsKeyboardFocus(true);
    setLookAndFeel(&lookAndFeel_);
    setupTitleBarChrome();
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

    for (auto* label : { &connectionLabel_, &lastPushLabel_, &versionLabel_ })
    {
        label->setFont(SkinEditorChrome::fontRegular());
        label->setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
        SkinEditorChrome::preventTextSquish(*label);
        label->setBorderSize(juce::BorderSize<int>(5, 0, 0, 0));
        addAndMakeVisible(*label);
    }
    connectionLabel_.setJustificationType(juce::Justification::centredRight);
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
    wire(newButton_, [this] { onNew(); });
    wire(duplicateButton_, [this] { onDuplicate(); });
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
    addAndMakeVisible(palettePanel_);

    mappingsPanel_.setLayer(&layer_);
    mappingsPanel_.onBindRequested = [this](TSS::SkinColourKey key, const juce::String& pastille)
    {
        if (! canEditEditorSkin())
            return;
        const auto result = layer_.setMapping(sessionState_.editorSkinId, key, pastille);
        if (result.failed())
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                                   "Mapping failed", result.getErrorMessage());
    };
    mappingsPanel_.onBindNeedsPastille = [this]
    {
        setStatusMessage("Select a Palette colour before binding");
    };
    addAndMakeVisible(mappingsPanel_);

    unboundPanel_.setLayer(&layer_);
    unboundPanel_.onBindRequested = [this](TSS::SkinColourKey key, const juce::String& pastille)
    {
        if (! canEditEditorSkin())
            return;
        const auto result = layer_.bindUnbound(sessionState_.editorSkinId, key, pastille);
        if (result.failed())
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                                   "Bind failed", result.getErrorMessage());
    };
    unboundPanel_.onBindNeedsPastille = [this]
    {
        setStatusMessage("Select a Palette colour before binding");
    };
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

    if (titleBarSeparatorX_ >= 0)
    {
        g.setColour(SkinEditorChrome::separatorLine().withAlpha(0.85f));
        g.fillRect(titleBarSeparatorX_, titleBarSeparatorY_, 1, titleBarSeparatorH_);
    }

    bounds.removeFromTop(SkinEditorChrome::kGutter);
    bounds.removeFromLeft(SkinEditorChrome::kGutter);
    bounds.removeFromRight(SkinEditorChrome::kGutter);
    auto skins = bounds.removeFromTop(SkinEditorChrome::kSkinsPanelHeight);
    g.setColour(SkinEditorChrome::surfacePanel());
    g.fillRoundedRectangle(skins.toFloat(), SkinEditorChrome::kControlCorner);

    g.setColour(SkinEditorChrome::separatorLine().withAlpha(0.85f));
    for (const int x : separatorXs_)
        if (x > 0)
            g.fillRect(x, separatorY_, 1, separatorH_);
}

void SkinEditorMainComponent::layoutTitleBar(juce::Rectangle<int> titleBar)
{
    const auto statusFont = SkinEditorChrome::fontRegular();
    const int versionW = SkinEditorChrome::measureTextWidth(statusFont, versionLabel_.getText()) + 4;
    const int titleW = SkinEditorChrome::measureTextWidth(SkinEditorChrome::fontAppTitle(),
                                                         titleLabel_.getText())
                       + 8;
    titleLabel_.setBounds(titleBar.getX(), titleBar.getY(), titleW, titleBar.getHeight());
    versionLabel_.setBounds(titleBar.getRight() - versionW, titleBar.getY(),
                            versionW, titleBar.getHeight());

    constexpr int kStatusSepGap = 10;
    const int connW = SkinEditorChrome::measureTextWidth(statusFont, connectionLabel_.getText());
    const int pushW = SkinEditorChrome::measureTextWidth(statusFont, lastPushLabel_.getText());
    const int clusterW = connW + kStatusSepGap + 1 + kStatusSepGap + pushW;
    int clusterX = (getWidth() - clusterW) / 2;
    const int leftLimit = titleLabel_.getBounds().getRight() + 8;
    const int rightLimit = versionLabel_.getBounds().getX() - 8;
    if (clusterX < leftLimit)
        clusterX = leftLimit;
    if (clusterX + clusterW > rightLimit)
        clusterX = juce::jmax(leftLimit, rightLimit - clusterW);

    connectionLabel_.setBounds(clusterX, titleBar.getY(), connW, titleBar.getHeight());
    titleBarSeparatorX_ = clusterX + connW + kStatusSepGap;
    titleBarSeparatorY_ = titleBar.getCentreY() - 4;
    titleBarSeparatorH_ = 12;
    lastPushLabel_.setBounds(titleBarSeparatorX_ + 1 + kStatusSepGap,
                             titleBar.getY(),
                             pushW,
                             titleBar.getHeight());
}

void SkinEditorMainComponent::layoutSkinsBar(juce::Rectangle<int> skins)
{
    skins.removeFromTop(SkinEditorChrome::kGutter);
    auto titleRow = skins.removeFromTop(SkinEditorChrome::kPanelTitleHeight);
    const int buttonY = titleRow.getY()
                        + (titleRow.getHeight() - SkinEditorChrome::kButtonHeight) / 2;
    auto row = juce::Rectangle<int>(titleRow.getX(),
                                    buttonY,
                                    titleRow.getWidth(),
                                    SkinEditorChrome::kButtonHeight);
    separatorY_ = row.getY();
    separatorH_ = row.getHeight();

    const auto labelFont = SkinEditorChrome::fontRegular();
    const int skinsTitleW = SkinEditorChrome::titleTabWidth("SKINS");
    const int productLabelW = SkinEditorChrome::measureLabelWidth(productLabel_.getText(), labelFont);
    const int editorLabelW = SkinEditorChrome::measureLabelWidth(editorLabel_.getText(), labelFont);

    skinsTitleLabel_.setBounds(titleRow.getX(), titleRow.getY() - 1, skinsTitleW, titleRow.getHeight());
    row.removeFromLeft(skinsTitleW);
    row.removeFromLeft(SkinEditorChrome::kButtonGap);
    productLabel_.setBounds(row.removeFromLeft(productLabelW));
    productCombo_.setBounds(row.removeFromLeft(120));
    row.removeFromLeft(12);
    editorLabel_.setBounds(row.removeFromLeft(editorLabelW));
    editorCombo_.setBounds(row.removeFromLeft(160));
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
    auto placeSep = [this, &row, &placeGap]()
    {
        placeGap();
        placeGap();
        separatorXs_.push_back(row.getX());
        row.removeFromLeft(1);
        placeGap();
        placeGap();
    };

    placeSep();
    placeButton(newButton_);
    placeGap();
    placeButton(duplicateButton_);
    placeGap();
    placeButton(renameButton_);
    placeGap();
    placeButton(deleteButton_);
    placeSep();
    placeButton(undoButton_);
    placeGap();
    placeButton(redoButton_);
    placeSep();
    placeButton(compareButton_);
    placeGap();
    placeButton(pushButton_);
    placeSep();
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
    separatorXs_.clear();
    titleBarSeparatorX_ = -1;

    auto bounds = getLocalBounds();
    const auto titleBar = bounds.removeFromTop(SkinEditorChrome::kTitleBarHeight)
                              .withTrimmedLeft(SkinEditorChrome::kPanelPad)
                              .withTrimmedRight(SkinEditorChrome::kPanelPad);
    layoutTitleBar(titleBar);

    bounds.removeFromTop(SkinEditorChrome::kGutter);
    bounds.removeFromLeft(SkinEditorChrome::kGutter);
    bounds.removeFromRight(SkinEditorChrome::kGutter);
    layoutSkinsBar(bounds.removeFromTop(SkinEditorChrome::kSkinsPanelHeight));

    bounds.removeFromTop(SkinEditorChrome::kGutter);
    layoutBodyPanels(bounds);
}
