#include "Tools/SkinDesigner/SkinDesignerMainComponent.h"

#include "GUI/Helpers/EditorialUndoRedoShortcuts.h"
#include "Tools/SkinDesigner/SkinDesignerAlertWindow.h"
#include "Tools/SkinDesigner/SkinDesignerUiHelpers.h"

namespace
{
    constexpr int kQuitCancelChoice = 0;
    constexpr int kQuitSaveChoice = 1;
    constexpr int kQuitDiscardChoice = 2;
}

void SkinDesignerMainComponent::mouseUp(const juce::MouseEvent& e)
{
    if (e.mouseWasClicked() && ! mappingsPanel_.preservesRowSelectionForEvent(e))
        mappingsPanel_.clearTableRowSelection();
}

bool SkinDesignerMainComponent::tryQuitWithDirtyGate()
{
    flushPendingSessionAutosave();

    // Nothing worth saving when the workspace holds no Designer skin.
    if (! SkinDesignerUi::shouldPromptQuitForUnsavedChanges(designerDirty_, hasAnyDesignerSkin()))
        return true;

    SkinDesignerChrome::AlertWindow window(SkinDesignerUi::kQuitUnsavedTitle,
                                           SkinDesignerUi::kQuitUnsavedMessage,
                                           juce::MessageBoxIconType::QuestionIcon);
    window.addButton(SkinDesignerUi::kQuitSaveButton, kQuitSaveChoice);
    window.addButton(SkinDesignerUi::kQuitDiscardButton, kQuitDiscardChoice);
    window.addButton(SkinDesignerUi::kQuitCancelButton, kQuitCancelChoice,
                     juce::KeyPress(juce::KeyPress::escapeKey));
    const int choice = window.runModalLoop();
    if (choice == kQuitDiscardChoice)
        return true;
    if (choice != kQuitSaveChoice)
        return false;

    return saveForQuit();
}

bool SkinDesignerMainComponent::keyPressed(const juce::KeyPress& key)
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

    const auto shortcut = TSS::classifyEditorialUndoRedoShortcut(key);
    if (shortcut == TSS::EditorialUndoRedoShortcut::kNone || isTextEditingFocused())
        return false;

    if (shortcut == TSS::EditorialUndoRedoShortcut::kRedo)
        onRedo();
    else
        onUndo();
    return true;
}

void SkinDesignerMainComponent::applyProductComboChange()
{
    sessionState_.productSkinId =
        SkinDesignerUi::productSkinIdFromComboId(skinsPanel_.productCombo().getSelectedId());
    refreshAll();
    if (bridge_.isConnected() && isCompareProductView())
        pushVisible();
    scheduleSessionAutosave();
}

void SkinDesignerMainComponent::applyDesignerComboChange()
{
    const auto label = skinsPanel_.designerCombo().getText();
    for (const auto& skin : layer_.skins())
    {
        if (skin.kind != TSS::DevSkin::SkinKind::kDev || ! skin.name.equalsIgnoreCase(label))
            continue;

        if (sessionState_.designerSkinId != skin.id)
            clearMappingModeToIdle();
        sessionState_.designerSkinId = skin.id;

        // Selection only — do not treat as unexported Designer edits (selectSkin bumps onChanged).
        runWithoutLayerChangeNotify([this, &skin] { layer_.selectSkin(skin.id); });
        syncHistoryCurrentSelection();

        refreshAll();
        if (bridge_.isConnected() && ! isCompareProductView())
            pushVisible();
        scheduleSessionAutosave();
        return;
    }
}

void SkinDesignerMainComponent::timerCallback()
{
    flushPendingSessionAutosave();
}

void SkinDesignerMainComponent::wireColourPicker(const std::function<bool()>& escapeMapping)
{
    colourPicker_.onColourChanged = [this](juce::Colour c) { onColourChanged(c); };
    colourPicker_.onColourGestureBegan = [this]
    {
        // Seal any open mapping burst so this press cannot coalesce with a prior edit.
        closeChangeBurst();
        colourGestureActive_ = true;
        colourGestureMadeEdit_ = false;
        refreshSessionButtonsEnabled();
    };
    colourPicker_.onColourGestureEnded = [this]
    {
        colourGestureActive_ = false;
        // One press = one Undo step (click or drag), committed with the final colour.
        if (colourGestureMadeEdit_)
            recordDocumentChange();
        colourGestureMadeEdit_ = false;
        refreshSessionButtonsEnabled();
        flushColourPreviewCoalesce();
    };
    colourPicker_.onEscapeRequested = escapeMapping;
    colourPreviewCoalesceTimer_.onTick = [this] { tickColourPreviewCoalesce(); };
    addAndMakeVisible(colourPicker_);
}

void SkinDesignerMainComponent::scheduleColourPreviewCoalesce()
{
    constexpr int kColourPreviewCoalesceMs = 40;
    if (! colourPreviewCoalesceTimer_.isTimerRunning())
        colourPreviewCoalesceTimer_.startTimer(kColourPreviewCoalesceMs);
}

void SkinDesignerMainComponent::applyColourPreviewRefresh()
{
    refreshAll();
    if (bridge_.isConnected() && ! isCompareProductView() && canEditDesignerSkin())
        pushVisible();
}

void SkinDesignerMainComponent::tickColourPreviewCoalesce()
{
    colourPreviewCoalesceTimer_.stopTimer();
    applyColourPreviewRefresh();
}

void SkinDesignerMainComponent::flushColourPreviewCoalesce()
{
    colourPreviewCoalesceTimer_.stopTimer();
    applyColourPreviewRefresh();
}

void SkinDesignerMainComponent::syncPastilleSelectionAfterRefresh(const juce::String& panelSkinId)
{
    if (selectedPastille_.isEmpty())
        return;

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

void SkinDesignerMainComponent::refreshAll()
{
    refreshCombos();
    refreshStatus();
    refreshSessionButtonsEnabled();

    const auto visibleId = visibleSkinId();
    const bool compareProduct = isCompareProductView();
    const bool hasDesignerSkin = sessionState_.designerSkinId.isNotEmpty();
    const bool editable = canEditDesignerSkin();
    // No designer skin: empty Palette/Mappings (do not fall back to product colours/tables).
    const juce::String panelSkinId = hasDesignerSkin ? visibleId : juce::String();

    syncCompareButtonChrome();

    skinsPanel_.setProductDuplicateEnabled(true);
    skinsPanel_.syncProductNavButtons();
    skinsPanel_.setDesignerActionsEnabled(hasDesignerSkin, editable, true);
    skinsPanel_.syncDesignerNavButtons();

    palettePanel_.setActiveSkinId(panelSkinId);
    palettePanel_.setReadOnly(! editable);
    palettePanel_.refresh();

    mappingsPanel_.setActiveSkinId(panelSkinId);
    // Designer skin id (empty → 0) — not panelSkinId, so Compare Product keeps the honest count.
    mappingsPanel_.setUnboundCount(layer_.unboundCount(sessionState_.designerSkinId));
    mappingsPanel_.setReadOnly(compareProduct || ! editable);
    mappingsPanel_.refresh();

    if (! editable && mappingMode_ != MappingMode::kIdle)
        clearMappingModeToIdle();

    syncPastilleSelectionAfterRefresh(panelSkinId);
    // Keep SV handle sticky during continuous COLOR drag (ARGB sync would snap HSB).
    if (! colourGestureActive_)
        syncColourPickerToSelection();
    updateMappingWitness();
}

void SkinDesignerMainComponent::refreshCombos()
{
    const juce::ScopedValueSetter<bool> scope(suppressCombo_, true);
    auto& productCombo = skinsPanel_.productCombo();
    auto& designerCombo = skinsPanel_.designerCombo();
    productCombo.clear(juce::dontSendNotification);
    productCombo.addItem("BLACK", 1);
    productCombo.addItem("BLACK/DUO", 2);
    productCombo.addItem("CREAM", 3);
    productCombo.setSelectedId(SkinDesignerUi::productComboIdFromSkinId(sessionState_.productSkinId),
                               juce::dontSendNotification);

    designerCombo.clear(juce::dontSendNotification);

    const auto designerSkins = SkinDesignerUi::sortedDesignerSkins(layer_);
    int selected = 0;
    int index = 1;
    for (const auto* skin : designerSkins)
    {
        designerCombo.addItem(skin->name.toUpperCase(), index);
        if (skin->id == sessionState_.designerSkinId)
            selected = index;
        ++index;
    }

    if (selected > 0)
    {
        designerCombo.setSelectedId(selected, juce::dontSendNotification);
    }
    else if (! designerSkins.empty())
    {
        designerCombo.setSelectedItemIndex(0, juce::dontSendNotification);
        const auto* first = designerSkins.front();
        if (sessionState_.designerSkinId != first->id)
            clearMappingModeToIdle();
        sessionState_.designerSkinId = first->id;
    }
    else
    {
        if (sessionState_.designerSkinId.isNotEmpty())
            clearMappingModeToIdle();
        sessionState_.designerSkinId = {};
    }

    skinsPanel_.syncProductNavButtons();
    skinsPanel_.syncDesignerNavButtons();
}

void SkinDesignerMainComponent::refreshStatus()
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

void SkinDesignerMainComponent::refreshSessionButtonsEnabled()
{
    const bool canSave = hasAnyDesignerSkin();
    saveButton_.setEnabled(canSave);
    saveAsButton_.setEnabled(canSave);
    const bool historyUsable = canUseDocumentHistory() && ! colourGestureActive_;
    undoButton_.setEnabled(historyUsable && history_.canUndo());
    redoButton_.setEnabled(historyUsable && history_.canRedo());
}

void SkinDesignerMainComponent::pushVisible()
{
    bridge_.push(layer_.makePushPayloadForSkin(visibleSkinId()));
    if (! bridge_.isConnected())
        setStatusMessage("queued until Standalone Dev connects");
    refreshStatus();
}

void SkinDesignerMainComponent::setStatusMessage(const juce::String& text)
{
    // LAST PUSH label is sentence-style chrome — always lead with a capital.
    if (text.isEmpty())
        statusDetail_.clear();
    else
        statusDetail_ = text.substring(0, 1).toUpperCase() + text.substring(1);
}

void SkinDesignerMainComponent::syncDesignerDirtyFromHistory()
{
    designerDirty_ = ! history_.isAtCleanMarker();
}

void SkinDesignerMainComponent::markDesignerClean()
{
    closeChangeBurst();
    history_.markClean();
    designerDirty_ = false;
}

void SkinDesignerMainComponent::scheduleSessionAutosave()
{
    startTimer(400);
}

void SkinDesignerMainComponent::flushPendingSessionAutosave()
{
    if (! isTimerRunning())
        return;
    stopTimer();
    autosaveSession();
}

bool SkinDesignerMainComponent::autosaveSession()
{
    juce::String error;
    if (! TSS::DevSkin::saveSessionFile(sessionState_, error))
    {
        setStatusMessage("Session autosave failed: " + error);
        return false;
    }
    return true;
}

void SkinDesignerMainComponent::runWithoutLayerChangeNotify(const std::function<void()>& action)
{
    const auto previousOnChanged = std::move(layer_.onChanged);
    layer_.onChanged = nullptr;
    action();
    layer_.onChanged = std::move(previousOnChanged);
}

void SkinDesignerMainComponent::adoptActiveDesignerSkinAfterCreate()
{
    // Cancel any pending colour preview push so New / Designer Duplicate cannot
    // still pushVisible() after the session id has moved to the new skin.
    colourPreviewCoalesceTimer_.stopTimer();
    colourGestureActive_ = false;
    colourGestureMadeEdit_ = false;
    closeChangeBurst();

    sessionState_.designerSkinId = layer_.activeSkinId();
    sessionState_.compareOn = false;
    clearMappingModeToIdle();
    recordDocumentChange();
    scheduleSessionAutosave();
    refreshAll();
    // Caller may pushVisible() — Product Duplicate only.
}

void SkinDesignerMainComponent::loadSessionOrBaseline()
{
    // Blank launch: only preferences come back from disk (a missing, old or corrupt
    // session file simply leaves the defaults). Designer skins are filled with Load.
    (void) TSS::DevSkin::loadSessionFile(sessionState_);
    TSS::DevSkin::forceProductToFirstAvailable(sessionState_);
    sessionState_.designerSkinId = {};
    sessionState_.compareOn = false;
    stopTimer();
    resetHistoryToCurrentDocument();
    applyJsonSyncSessionUi();
}

juce::String SkinDesignerMainComponent::visibleSkinId() const
{
    if (isCompareProductView())
        return sessionState_.productSkinId;
    if (sessionState_.designerSkinId.isNotEmpty())
        return sessionState_.designerSkinId;
    return sessionState_.productSkinId;
}

bool SkinDesignerMainComponent::isCompareProductView() const
{
    return sessionState_.compareOn;
}

bool SkinDesignerMainComponent::canEditDesignerSkin() const
{
    if (isCompareProductView() || sessionState_.designerSkinId.isEmpty())
        return false;
    const auto* skin = layer_.findSkin(sessionState_.designerSkinId);
    return skin != nullptr && skin->kind == TSS::DevSkin::SkinKind::kDev;
}

