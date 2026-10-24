#include "Tools/SkinEditor/SkinEditorMainComponent.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"

namespace
{
    juce::String askText(const juce::String& title,
                         const juce::String& message,
                         const juce::String& initial)
    {
        juce::AlertWindow window(title, message, juce::MessageBoxIconType::QuestionIcon);
        window.addTextEditor("value", initial, "Value");
        window.addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
        window.addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
        if (window.runModalLoop() != 1)
            return {};
        return window.getTextEditorContents("value").trim();
    }
}

SkinEditorMainComponent::SkinEditorMainComponent()
{
    setWantsKeyboardFocus(true);
    setLookAndFeel(&lookAndFeel_);

    titleLabel_.setText("MATRIX-CONTROL SKIN EDITOR", juce::dontSendNotification);
    titleLabel_.setFont(SkinEditorChrome::fontAppTitle());
    titleLabel_.setColour(juce::Label::textColourId, SkinEditorChrome::textBrand());
    titleLabel_.setJustificationType(juce::Justification::centredLeft);
    SkinEditorChrome::preventTextSquish(titleLabel_);
    // Match status-cluster optical nudge in the title bar.
    titleLabel_.setBorderSize(juce::BorderSize<int>(5, 0, 0, 0));
    addAndMakeVisible(titleLabel_);

    connectionLabel_.setFont(SkinEditorChrome::fontRegular());
    lastPushLabel_.setFont(SkinEditorChrome::fontRegular());
    versionLabel_.setFont(SkinEditorChrome::fontRegular());
    connectionLabel_.setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
    lastPushLabel_.setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
    versionLabel_.setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
    connectionLabel_.setJustificationType(juce::Justification::centredRight);
    lastPushLabel_.setJustificationType(juce::Justification::centredLeft);
    versionLabel_.setJustificationType(juce::Justification::centredRight);
    versionLabel_.setText(SkinEditorChrome::kAppVersionLabel, juce::dontSendNotification);
    SkinEditorChrome::preventTextSquish(connectionLabel_);
    SkinEditorChrome::preventTextSquish(lastPushLabel_);
    SkinEditorChrome::preventTextSquish(versionLabel_);
    connectionLabel_.setBorderSize(juce::BorderSize<int>(5, 0, 0, 0));
    lastPushLabel_.setBorderSize(juce::BorderSize<int>(5, 0, 0, 0));
    versionLabel_.setBorderSize(juce::BorderSize<int>(5, 0, 0, 0));
    addAndMakeVisible(connectionLabel_);
    addAndMakeVisible(lastPushLabel_);
    addAndMakeVisible(versionLabel_);

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
    // History lands in a separate chantier — buttons + shortcuts reserved.
    undoButton_.setEnabled(false);
    redoButton_.setEnabled(false);

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
    addAndMakeVisible(unboundPanel_);

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

bool SkinEditorMainComponent::tryQuitWithDirtyGate()
{
    stopTimer();

    if (! sessionDirty_)
        return true;

    juce::AlertWindow window("Unsaved Editor changes",
                             "Export the current Editor skin, discard changes, or cancel?",
                             juce::MessageBoxIconType::QuestionIcon);
    window.addButton("Export", 1);
    window.addButton("Discard", 2);
    window.addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
    const int choice = window.runModalLoop();
    if (choice == 0)
    {
        autosaveSession();
        return false;
    }
    if (choice == 2)
    {
        stopTimer();
        TSS::DevSkin::skinEditorSessionFile().deleteFile();
        markClean();
        return true;
    }

    // Export then quit if successful
    if (sessionState_.editorSkinId.isEmpty()
        || ! layer_.isEditorSkinExportAllowed(sessionState_.editorSkinId))
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::MessageBoxIconType::WarningIcon,
            "Export blocked",
            "Export needs an Editor skin with Unbound = 0.");
        return false;
    }

    auto chooser = std::make_shared<juce::FileChooser>(
        "Export Editor skin JSON",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
            .getChildFile("matrix-control-editor-skin.json"),
        "*.json");

    // Modal sync chooser for quit path
    if (chooser->browseForFileToSave(true))
    {
        if (exportCurrentEditorSkinToFile(chooser->getResult()))
        {
            markClean();
            return true;
        }
    }
    return false;
}

void SkinEditorMainComponent::paint(juce::Graphics& g)
{
    g.fillAll(SkinEditorChrome::surfaceApp());

    auto bounds = getLocalBounds();
    bounds.removeFromTop(SkinEditorChrome::kTitleBarHeight); // no fill — show GUI background

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

void SkinEditorMainComponent::resized()
{
    separatorXs_.clear();
    titleBarSeparatorX_ = -1;

    auto bounds = getLocalBounds();
    // Horizontal pad only — full height keeps labels vertically centred via Justification.
    const auto titleBar = bounds.removeFromTop(SkinEditorChrome::kTitleBarHeight)
                              .withTrimmedLeft(SkinEditorChrome::kPanelPad)
                              .withTrimmedRight(SkinEditorChrome::kPanelPad);

    const auto statusFont = SkinEditorChrome::fontRegular();
    const int versionW = SkinEditorChrome::measureTextWidth(statusFont, versionLabel_.getText()) + 4;
    const int titleW = SkinEditorChrome::measureTextWidth(SkinEditorChrome::fontAppTitle(),
                                                         titleLabel_.getText())
                       + 8;
    titleLabel_.setBounds(titleBar.getX(), titleBar.getY(), titleW, titleBar.getHeight());
    versionLabel_.setBounds(titleBar.getRight() - versionW, titleBar.getY(),
                            versionW, titleBar.getHeight());

    // Centre the status cluster in the full window, not in the leftover middle strip.
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
    titleBarSeparatorY_ = titleBar.getCentreY() - 4; // optical align with nudged text
    titleBarSeparatorH_ = 12;
    lastPushLabel_.setBounds(titleBarSeparatorX_ + 1 + kStatusSepGap,
                             titleBar.getY(),
                             pushW,
                             titleBar.getHeight());

    bounds.removeFromTop(SkinEditorChrome::kGutter);
    bounds.removeFromLeft(SkinEditorChrome::kGutter);
    bounds.removeFromRight(SkinEditorChrome::kGutter);
    auto skins = bounds.removeFromTop(SkinEditorChrome::kSkinsPanelHeight);
    // Same rhythm as COLOR: top gutter, then title band; title flush left (no side pad).
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

    auto placeGap = [&row]()
    {
        row.removeFromLeft(SkinEditorChrome::kButtonGap);
    };
    auto placeButton = [&row](juce::TextButton& b)
    {
        const int w = SkinEditorChrome::measureButtonWidth(b);
        b.setBounds(row.removeFromLeft(w).withHeight(SkinEditorChrome::kButtonHeight));
    };
    auto placeSep = [this, &row, &placeGap]()
    {
        placeGap();
        placeGap(); // double gap on each side of vertical separators
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

    bounds.removeFromTop(SkinEditorChrome::kGutter);
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

bool SkinEditorMainComponent::keyPressed(const juce::KeyPress& key)
{
    const auto mods = key.getModifiers();
    if (mods.isCommandDown() && ! mods.isAltDown())
    {
        if (key.getKeyCode() == 'Z' || key.getKeyCode() == 'z')
        {
            if (mods.isShiftDown())
                onRedo();
            else
                onUndo();
            return true;
        }
    }
    return false;
}

void SkinEditorMainComponent::comboBoxChanged(juce::ComboBox* box)
{
    if (suppressCombo_)
        return;

    if (box == &productCombo_)
    {
        const int id = productCombo_.getSelectedId();
        sessionState_.productSkinId = (id == 1) ? TSS::DevSkin::kCompiledBlackId
                                                : TSS::DevSkin::kCompiledCreamId;
        markDirty();
        refreshAll();
        if (bridge_.isConnected() && isCompareProductView())
            pushVisible();
        startTimer(400);
        return;
    }

    if (box == &editorCombo_)
    {
        const auto label = editorCombo_.getText();
        for (const auto& skin : layer_.skins())
        {
            if (skin.kind == TSS::DevSkin::SkinKind::kDev && skin.name.equalsIgnoreCase(label))
            {
                if (sessionState_.editorSkinId != skin.id)
                    selectedPastille_.clear();
                sessionState_.editorSkinId = skin.id;
                layer_.selectSkin(skin.id);
                markDirty();
                refreshAll();
                if (bridge_.isConnected() && ! isCompareProductView())
                    pushVisible();
                startTimer(400);
                return;
            }
        }
    }
}

void SkinEditorMainComponent::timerCallback()
{
    stopTimer();
    autosaveSession();
}

void SkinEditorMainComponent::refreshAll()
{
    refreshCombos();
    refreshStatus();
    refreshExportEnabled();

    const auto visibleId = visibleSkinId();
    const bool compareProduct = isCompareProductView();
    const bool editable = canEditEditorSkin();

    syncCompareButtonChrome();

    renameButton_.setEnabled(editable);
    deleteButton_.setEnabled(editable);
    newButton_.setEnabled(true);
    duplicateButton_.setEnabled(true);

    palettePanel_.setActiveSkinId(visibleId);
    palettePanel_.setReadOnly(! editable);
    palettePanel_.refresh();

    mappingsPanel_.setActiveSkinId(visibleId);
    mappingsPanel_.setReadOnly(compareProduct || ! editable);
    mappingsPanel_.setSelectedPastilleName(selectedPastille_);
    mappingsPanel_.refresh();

    unboundPanel_.setActiveSkinId(visibleId);
    unboundPanel_.setForcedEmpty(compareProduct);
    unboundPanel_.setReadOnly(compareProduct || ! editable);
    unboundPanel_.setSelectedPastilleName(selectedPastille_);
    unboundPanel_.refresh();

    if (selectedPastille_.isNotEmpty())
    {
        bool found = false;
        if (const auto* skin = layer_.findSkin(visibleId))
        {
            for (const auto& pastille : skin->palette)
            {
                if (pastille.name == selectedPastille_)
                {
                    colourPicker_.setColour(juce::Colour(pastille.argb), juce::dontSendNotification);
                    found = true;
                    break;
                }
            }
        }
        if (found)
            palettePanel_.selectPastille(selectedPastille_);
        else
            selectedPastille_.clear();
    }
    colourPicker_.setEnabledEditing(editable && selectedPastille_.isNotEmpty());
}

void SkinEditorMainComponent::refreshCombos()
{
    const juce::ScopedValueSetter<bool> scope(suppressCombo_, true);
    productCombo_.clear(juce::dontSendNotification);
    productCombo_.addItem("BLACK", 1);
    productCombo_.addItem("CREAM", 2);
    productCombo_.setSelectedId(sessionState_.productSkinId == TSS::DevSkin::kCompiledBlackId ? 1 : 2,
                                juce::dontSendNotification);

    editorCombo_.clear(juce::dontSendNotification);
    int selected = 0;
    int index = 1;
    for (const auto& skin : layer_.skins())
    {
        if (skin.kind != TSS::DevSkin::SkinKind::kDev)
            continue;
        editorCombo_.addItem(skin.name.toUpperCase(), index);
        if (skin.id == sessionState_.editorSkinId)
            selected = index;
        ++index;
    }
    if (selected > 0)
    {
        editorCombo_.setSelectedId(selected, juce::dontSendNotification);
    }
    else if (editorCombo_.getNumItems() > 0)
    {
        editorCombo_.setSelectedItemIndex(0, juce::dontSendNotification);
        for (const auto& skin : layer_.skins())
        {
            if (skin.kind == TSS::DevSkin::SkinKind::kDev
                && skin.name.equalsIgnoreCase(editorCombo_.getText()))
            {
                if (sessionState_.editorSkinId != skin.id)
                    selectedPastille_.clear();
                sessionState_.editorSkinId = skin.id;
                break;
            }
        }
    }
    else
    {
        if (sessionState_.editorSkinId.isNotEmpty())
            selectedPastille_.clear();
        sessionState_.editorSkinId = {};
    }
}

void SkinEditorMainComponent::refreshStatus()
{
    connectionLabel_.setText(bridge_.session().connectionStatusText(), juce::dontSendNotification);
    lastPushLabel_.setText(statusDetail_.isNotEmpty() ? statusDetail_ : "Last push: none",
                           juce::dontSendNotification);
    // Re-centre the Connection | Last push cluster for the new string widths.
    resized();
}

void SkinEditorMainComponent::refreshExportEnabled()
{
    exportButton_.setEnabled(layer_.isEditorSkinExportAllowed(sessionState_.editorSkinId));
}

void SkinEditorMainComponent::pushVisible()
{
    bridge_.push(layer_.makePushPayloadForSkin(visibleSkinId()));
    if (! bridge_.isConnected())
        setStatusMessage("Last push: queued until Standalone Dev connects");
    refreshStatus();
}

void SkinEditorMainComponent::setStatusMessage(const juce::String& text)
{
    statusDetail_ = text;
}

void SkinEditorMainComponent::markDirty()
{
    sessionDirty_ = true;
}

void SkinEditorMainComponent::markClean()
{
    sessionDirty_ = false;
}

void SkinEditorMainComponent::autosaveSession()
{
    juce::String error;
    if (! TSS::DevSkin::saveSessionFile(layer_, sessionState_, error))
        setStatusMessage("Session autosave failed: " + error);
}

void SkinEditorMainComponent::loadSessionOrBaseline()
{
    const auto loaded = TSS::DevSkin::loadSessionFile(layer_, sessionState_);
    if (! loaded.ok)
    {
        if (loaded.message != "No session file")
            setStatusMessage(loaded.message);
        sessionState_.productSkinId = TSS::DevSkin::kCompiledCreamId;
        sessionState_.editorSkinId = {};
        sessionState_.compareOn = false;
    }
    if (sessionState_.editorSkinId.isNotEmpty())
        layer_.selectSkin(sessionState_.editorSkinId);
    markClean();
}

juce::String SkinEditorMainComponent::visibleSkinId() const
{
    if (isCompareProductView())
        return sessionState_.productSkinId;
    if (sessionState_.editorSkinId.isNotEmpty())
        return sessionState_.editorSkinId;
    return sessionState_.productSkinId;
}

bool SkinEditorMainComponent::isCompareProductView() const
{
    return sessionState_.compareOn;
}

bool SkinEditorMainComponent::canEditEditorSkin() const
{
    if (isCompareProductView() || sessionState_.editorSkinId.isEmpty())
        return false;
    const auto* skin = layer_.findSkin(sessionState_.editorSkinId);
    return skin != nullptr && skin->kind == TSS::DevSkin::SkinKind::kDev;
}

void SkinEditorMainComponent::onDuplicate()
{
    const auto sourceId = isCompareProductView() ? sessionState_.productSkinId
                                                 : (sessionState_.editorSkinId.isNotEmpty()
                                                        ? sessionState_.editorSkinId
                                                        : sessionState_.productSkinId);
    const auto name = askText("Duplicate", "New Editor skin name:", "My Skin");
    if (name.isEmpty())
        return;
    const auto result = layer_.duplicateSkin(sourceId, name);
    if (result.failed())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Duplicate failed", result.getErrorMessage());
        return;
    }
    sessionState_.editorSkinId = layer_.activeSkinId();
    sessionState_.compareOn = false;
    selectedPastille_.clear();
}

void SkinEditorMainComponent::onNew()
{
    const auto name = askText("New Editor skin", "Name:", "New Skin");
    if (name.isEmpty())
        return;
    const auto result = layer_.createEmptyDevSkin(name);
    if (result.failed())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "New failed", result.getErrorMessage());
        return;
    }
    sessionState_.editorSkinId = layer_.activeSkinId();
    sessionState_.compareOn = false;
    selectedPastille_.clear();
}

void SkinEditorMainComponent::onDelete()
{
    if (! canEditEditorSkin())
        return;

    const auto* skin = layer_.findSkin(sessionState_.editorSkinId);
    if (skin == nullptr)
        return;

    juce::AlertWindow confirm("Delete Editor skin",
                              "Delete \"" + skin->name + "\"? This cannot be undone.",
                              juce::MessageBoxIconType::WarningIcon);
    confirm.addButton("Delete", 1);
    confirm.addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
    if (confirm.runModalLoop() != 1)
        return;

    const auto result = layer_.deleteDevSkin(sessionState_.editorSkinId);
    if (result.failed())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Delete failed", result.getErrorMessage());
        return;
    }
    sessionState_.editorSkinId = {};
    selectedPastille_.clear();
}

void SkinEditorMainComponent::onRename()
{
    if (! canEditEditorSkin())
        return;
    const auto* skin = layer_.findSkin(sessionState_.editorSkinId);
    if (skin == nullptr)
        return;
    const auto name = askText("Rename Editor skin", "New name:", skin->name);
    if (name.isEmpty())
        return;
    const auto result = layer_.renameDevSkin(skin->id, name);
    if (result.failed())
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Rename failed", result.getErrorMessage());
}

void SkinEditorMainComponent::syncCompareButtonChrome()
{
    compareButton_.setButtonText("Compare");
    compareButton_.setToggleState(sessionState_.compareOn, juce::dontSendNotification);

    if (sessionState_.compareOn)
    {
        if (! compareBlinkTimer_.isTimerRunning())
        {
            compareBlinkLit_ = true;
            compareBlinkTimer_.startTimerHz(2); // ~500 ms phase
        }
        const auto colour = compareBlinkLit_ ? juce::Colour(0xffff3b3b)
                                             : SkinEditorChrome::textMuted();
        compareButton_.setColour(juce::TextButton::textColourOffId, colour);
        compareButton_.setColour(juce::TextButton::textColourOnId, colour);
    }
    else
    {
        compareBlinkTimer_.stopTimer();
        compareBlinkLit_ = true;
        const auto normal = SkinEditorChrome::textPrimary();
        compareButton_.setColour(juce::TextButton::textColourOffId, normal);
        compareButton_.setColour(juce::TextButton::textColourOnId, normal);
    }
    compareButton_.repaint();
}

void SkinEditorMainComponent::tickCompareBlink()
{
    if (! sessionState_.compareOn)
        return;

    compareBlinkLit_ = ! compareBlinkLit_;
    const auto colour = compareBlinkLit_ ? juce::Colour(0xffff3b3b)
                                         : SkinEditorChrome::textMuted();
    compareButton_.setColour(juce::TextButton::textColourOffId, colour);
    compareButton_.setColour(juce::TextButton::textColourOnId, colour);
    compareButton_.repaint();
}

void SkinEditorMainComponent::onCompareToggle()
{
    sessionState_.compareOn = ! sessionState_.compareOn;
    markDirty();
    refreshAll();
    if (bridge_.isConnected())
        pushVisible();
    startTimer(400);
}

void SkinEditorMainComponent::onPush()
{
    pushVisible();
}

void SkinEditorMainComponent::onUndo()
{
    // Placeholder until the dedicated Undo/Redo chantier.
    setStatusMessage("Undo: not implemented yet");
    refreshStatus();
}

void SkinEditorMainComponent::onRedo()
{
    // Placeholder until the dedicated Undo/Redo chantier.
    setStatusMessage("Redo: not implemented yet");
    refreshStatus();
}

void SkinEditorMainComponent::onExportJson()
{
    if (! layer_.isEditorSkinExportAllowed(sessionState_.editorSkinId))
        return;

    auto chooser = std::make_shared<juce::FileChooser>(
        "Export Editor skin JSON",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
            .getChildFile("matrix-control-editor-skin.json"),
        "*.json");
    chooser->launchAsync(juce::FileBrowserComponent::saveMode
                             | juce::FileBrowserComponent::canSelectFiles
                             | juce::FileBrowserComponent::warnAboutOverwriting,
                         [this, chooser](const juce::FileChooser& fc)
                         {
                             const auto file = fc.getResult();
                             if (file == juce::File())
                                 return;
                             if (exportCurrentEditorSkinToFile(file))
                             {
                                 markClean();
                                 setStatusMessage("Exported: " + file.getFullPathName());
                                 refreshStatus();
                             }
                         });
}

void SkinEditorMainComponent::onImportJson()
{
    auto chooser = std::make_shared<juce::FileChooser>(
        "Import Editor skin JSON",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
        "*.json");
    chooser->launchAsync(juce::FileBrowserComponent::openMode
                             | juce::FileBrowserComponent::canSelectFiles,
                         [this, chooser](const juce::FileChooser& fc)
                         {
                             const auto file = fc.getResult();
                             if (file == juce::File())
                                 return;
                             const auto result = TSS::DevSkin::appendEditorSkinsFromJson(
                                 layer_, file.loadFileAsString());
                             if (! result.ok)
                             {
                                 juce::AlertWindow::showMessageBoxAsync(
                                     juce::MessageBoxIconType::WarningIcon,
                                     "Import refused",
                                     result.message);
                                 return;
                             }
                             if (result.importedSkinIds.size() > 0)
                                 sessionState_.editorSkinId = result.importedSkinIds[0];
                             sessionState_.compareOn = false;
                             setStatusMessage("Imported: " + file.getFullPathName());
                             refreshAll();
                             if (bridge_.isConnected())
                                 pushVisible();
                         });
}

void SkinEditorMainComponent::onAddPastille()
{
    if (! canEditEditorSkin())
        return;
    const auto name = askText("Add colour", "Pastille name:", "kCustomAccent");
    if (name.isEmpty())
        return;
    const auto result = layer_.addPastille(sessionState_.editorSkinId, name, 0xffff0000);
    if (result.failed())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Add failed", result.getErrorMessage());
        return;
    }
    selectedPastille_ = name;
}

void SkinEditorMainComponent::onDeletePastille()
{
    if (! canEditEditorSkin())
        return;
    const auto name = palettePanel_.getSelectedPastilleName();
    if (name.isEmpty() || layer_.isProductPastilleName(name))
        return;

    bool removeRefs = false;
    const int refs = layer_.countMappingsReferencing(sessionState_.editorSkinId, name);
    if (refs > 0)
    {
        juce::AlertWindow window("Delete colour",
                                 name + " is used by " + juce::String(refs)
                                     + " mapping(s). Delete and clear those mappings?",
                                 juce::MessageBoxIconType::WarningIcon);
        window.addButton("Delete", 1);
        window.addButton("Cancel", 0);
        if (window.runModalLoop() != 1)
            return;
        removeRefs = true;
    }

    const auto result = layer_.deletePastille(sessionState_.editorSkinId, name, removeRefs);
    if (result.failed())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Delete failed", result.getErrorMessage());
        return;
    }
    if (selectedPastille_ == name)
        selectedPastille_.clear();
}

void SkinEditorMainComponent::onColourChanged(juce::Colour colour)
{
    if (! canEditEditorSkin() || selectedPastille_.isEmpty())
        return;
    const auto result = layer_.setPastilleArgb(sessionState_.editorSkinId,
                                               selectedPastille_,
                                               colour.getARGB());
    if (result.failed())
        setStatusMessage(result.getErrorMessage());
}

void SkinEditorMainComponent::onPastilleSelected(const juce::String& name)
{
    selectedPastille_ = name;
    mappingsPanel_.setSelectedPastilleName(selectedPastille_);
    unboundPanel_.setSelectedPastilleName(selectedPastille_);
    if (const auto* skin = layer_.findSkin(visibleSkinId()))
    {
        for (const auto& pastille : skin->palette)
        {
            if (pastille.name == name)
            {
                colourPicker_.setColour(juce::Colour(pastille.argb), juce::dontSendNotification);
                colourPicker_.setEnabledEditing(canEditEditorSkin());
                break;
            }
        }
    }
}

bool SkinEditorMainComponent::exportCurrentEditorSkinToFile(const juce::File& file)
{
    if (! layer_.isEditorSkinExportAllowed(sessionState_.editorSkinId))
        return false;
    const auto json = TSS::DevSkin::exportEditorSkinToJson(layer_, sessionState_.editorSkinId);
    if (json.isEmpty() || ! file.replaceWithText(json))
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Export failed",
                                               "Could not write file");
        return false;
    }
    return true;
}
