#include "Tools/SkinEditor/SkinEditorMainComponent.h"

bool SkinEditorMainComponent::tryQuitWithDirtyGate()
{
    flushPendingSessionAutosave();

    if (! editorDirty_)
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
        return quitAfterDiscardChoice();

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
        suggestedEditorSkinExportFile(),
        "*.json");

    // Modal sync chooser for quit path
    if (chooser->browseForFileToSave(true)
        && exportCurrentEditorSkinToFile(chooser->getResult()))
    {
        markEditorClean();
        return true;
    }
    return false;
}

bool SkinEditorMainComponent::quitAfterDiscardChoice()
{
    if (discardEditorWorkKeepingSessionPrefs())
        return true;

    juce::AlertWindow::showMessageBoxAsync(
        juce::MessageBoxIconType::WarningIcon,
        "Discard failed",
        "Could not rewrite the session file. Quit cancelled so Editor work is not left on disk.");
    return false;
}

bool SkinEditorMainComponent::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        if (mappingMode_ != MappingMode::kIdle)
        {
            clearMappingModeToIdle();
            return true;
        }
        return false;
    }

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

void SkinEditorMainComponent::applyProductComboChange()
{
    const int id = skinsPanel_.productCombo().getSelectedId();
    sessionState_.productSkinId = (id == 1) ? TSS::DevSkin::kCompiledBlackId
                                            : TSS::DevSkin::kCompiledCreamId;
    refreshAll();
    if (bridge_.isConnected() && isCompareProductView())
        pushVisible();
    scheduleSessionAutosave();
}

void SkinEditorMainComponent::applyEditorComboChange()
{
    const auto label = skinsPanel_.editorCombo().getText();
    for (const auto& skin : layer_.skins())
    {
        if (skin.kind != TSS::DevSkin::SkinKind::kDev || ! skin.name.equalsIgnoreCase(label))
            continue;

        if (sessionState_.editorSkinId != skin.id)
            clearMappingModeToIdle();
        sessionState_.editorSkinId = skin.id;

        // Selection only — do not treat as unexported Editor edits (selectSkin bumps onChanged).
        runWithoutLayerChangeNotify([this, &skin] { layer_.selectSkin(skin.id); });

        refreshAll();
        if (bridge_.isConnected() && ! isCompareProductView())
            pushVisible();
        scheduleSessionAutosave();
        return;
    }
}

void SkinEditorMainComponent::comboBoxChanged(juce::ComboBox* box)
{
    if (suppressCombo_)
        return;
    if (box == &skinsPanel_.productCombo())
        applyProductComboChange();
    else if (box == &skinsPanel_.editorCombo())
        applyEditorComboChange();
}

void SkinEditorMainComponent::timerCallback()
{
    flushPendingSessionAutosave();
}

void SkinEditorMainComponent::refreshAll()
{
    refreshCombos();
    refreshStatus();
    refreshExportEnabled();

    const auto visibleId = visibleSkinId();
    const bool compareProduct = isCompareProductView();
    const bool hasEditorSkin = sessionState_.editorSkinId.isNotEmpty();
    const bool editable = canEditEditorSkin();
    // No editor skin: empty Palette/Mappings (do not fall back to product colours/tables).
    const juce::String panelSkinId = hasEditorSkin ? visibleId : juce::String();

    syncCompareButtonChrome();

    skinsPanel_.setProductDuplicateEnabled(true);
    skinsPanel_.setEditorActionsEnabled(hasEditorSkin, editable, true);

    palettePanel_.setActiveSkinId(panelSkinId);
    palettePanel_.setReadOnly(! editable);
    palettePanel_.refresh();

    mappingsPanel_.setActiveSkinId(panelSkinId);
    // Editor skin id (empty → 0) — not panelSkinId, so Compare Product keeps Export-honest count.
    mappingsPanel_.setUnboundCount(layer_.unboundCount(sessionState_.editorSkinId));
    mappingsPanel_.setReadOnly(compareProduct || ! editable);
    mappingsPanel_.refresh();

    if (! editable && mappingMode_ != MappingMode::kIdle)
        clearMappingModeToIdle();

    if (selectedPastille_.isNotEmpty())
    {
        bool found = false;
        if (const auto* skin = layer_.findSkin(panelSkinId))
        {
            for (const auto& pastille : skin->palette)
            {
                if (pastille.name == selectedPastille_)
                {
                    found = true;
                    break;
                }
            }
        }
        if (found)
            palettePanel_.selectPastille(selectedPastille_);
        else if (mappingMode_ == MappingMode::kHeldColour
                 || mappingMode_ == MappingMode::kEditingColour)
            clearMappingModeToIdle();
        else
            selectedPastille_.clear();
    }
    syncColourPickerToSelection();
    updateMappingWitness();
}

void SkinEditorMainComponent::refreshCombos()
{
    const juce::ScopedValueSetter<bool> scope(suppressCombo_, true);
    auto& productCombo = skinsPanel_.productCombo();
    auto& editorCombo = skinsPanel_.editorCombo();
    productCombo.clear(juce::dontSendNotification);
    productCombo.addItem("BLACK", 1);
    productCombo.addItem("CREAM", 2);
    productCombo.setSelectedId(sessionState_.productSkinId == TSS::DevSkin::kCompiledBlackId ? 1 : 2,
                               juce::dontSendNotification);

    editorCombo.clear(juce::dontSendNotification);
    int selected = 0;
    int index = 1;
    for (const auto& skin : layer_.skins())
    {
        if (skin.kind != TSS::DevSkin::SkinKind::kDev)
            continue;
        editorCombo.addItem(skin.name.toUpperCase(), index);
        if (skin.id == sessionState_.editorSkinId)
            selected = index;
        ++index;
    }
    if (selected > 0)
    {
        editorCombo.setSelectedId(selected, juce::dontSendNotification);
    }
    else if (editorCombo.getNumItems() > 0)
    {
        editorCombo.setSelectedItemIndex(0, juce::dontSendNotification);
        for (const auto& skin : layer_.skins())
        {
            if (skin.kind == TSS::DevSkin::SkinKind::kDev
                && skin.name.equalsIgnoreCase(editorCombo.getText()))
            {
                if (sessionState_.editorSkinId != skin.id)
                    clearMappingModeToIdle();
                sessionState_.editorSkinId = skin.id;
                break;
            }
        }
    }
    else
    {
        if (sessionState_.editorSkinId.isNotEmpty())
            clearMappingModeToIdle();
        sessionState_.editorSkinId = {};
    }
}

void SkinEditorMainComponent::refreshStatus()
{
    connectionLabel_.setText(bridge_.isConnected() ? "Connected to Matrix-Control"
                                                   : "Disconnected from Matrix-Control",
                             juce::dontSendNotification);

    // Re-layout: fixed badges + path-style elide + export-path tooltip when truncated.
    resized();
    // BRIDGE fill is custom-painted; Labels already refresh via setText / setBounds.
    if (! connectionBadgeBounds_.isEmpty())
        repaint(connectionBadgeBounds_);
}

void SkinEditorMainComponent::refreshExportEnabled()
{
    exportButton_.setEnabled(layer_.isEditorSkinExportAllowed(sessionState_.editorSkinId));
}

void SkinEditorMainComponent::pushVisible()
{
    bridge_.push(layer_.makePushPayloadForSkin(visibleSkinId()));
    if (! bridge_.isConnected())
        setStatusMessage("queued until Standalone Dev connects");
    refreshStatus();
}

void SkinEditorMainComponent::setStatusMessage(const juce::String& text)
{
    statusDetail_ = text;
}

void SkinEditorMainComponent::markEditorDirty()
{
    editorDirty_ = true;
}

void SkinEditorMainComponent::markEditorClean()
{
    editorDirty_ = false;
}

void SkinEditorMainComponent::scheduleSessionAutosave()
{
    startTimer(400);
}

void SkinEditorMainComponent::flushPendingSessionAutosave()
{
    if (! isTimerRunning())
        return;
    stopTimer();
    autosaveSession();
}

bool SkinEditorMainComponent::autosaveSession()
{
    juce::String error;
    if (! TSS::DevSkin::saveSessionFile(layer_, sessionState_, error))
    {
        setStatusMessage("Session autosave failed: " + error);
        return false;
    }
    return true;
}

void SkinEditorMainComponent::runWithoutLayerChangeNotify(const std::function<void()>& action)
{
    const auto previousOnChanged = std::move(layer_.onChanged);
    layer_.onChanged = nullptr;
    action();
    layer_.onChanged = std::move(previousOnChanged);
}

bool SkinEditorMainComponent::discardEditorWorkKeepingSessionPrefs()
{
    stopTimer();

    const auto productSkinId = sessionState_.productSkinId;
    const auto lastSkinJsonDirectory = sessionState_.lastSkinJsonDirectory;

    runWithoutLayerChangeNotify([this] { layer_.resetToCompiledBaseline(); });

    sessionState_.productSkinId = productSkinId;
    sessionState_.editorSkinId = {};
    sessionState_.compareOn = false;
    sessionState_.lastSkinJsonDirectory = lastSkinJsonDirectory;

    clearMappingModeToIdle();
    if (! autosaveSession())
    {
        // Memory already baseline; restore disk so UI matches the abandoned session file.
        runWithoutLayerChangeNotify([this]
        {
            (void) TSS::DevSkin::loadSessionFile(layer_, sessionState_);
            sessionState_.compareOn = false;
            if (sessionState_.editorSkinId.isNotEmpty())
                layer_.selectSkin(sessionState_.editorSkinId);
        });
        stopTimer();
        markEditorDirty();
        refreshAll();
        return false;
    }

    markEditorClean();
    refreshAll();
    if (bridge_.isConnected())
        pushVisible();
    return true;
}

void SkinEditorMainComponent::loadSessionOrBaseline()
{
    runWithoutLayerChangeNotify([this]
    {
        const auto loaded = TSS::DevSkin::loadSessionFile(layer_, sessionState_);
        if (! loaded.ok)
        {
            if (loaded.message != "No session file")
                setStatusMessage(loaded.message);
            sessionState_.productSkinId = TSS::DevSkin::kCompiledCreamId;
            sessionState_.editorSkinId = {};
        }
        // Compare is never restored from disk — always start editing, not comparing.
        sessionState_.compareOn = false;
        if (sessionState_.editorSkinId.isNotEmpty())
            layer_.selectSkin(sessionState_.editorSkinId);
    });
    stopTimer();
    markEditorClean();
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

