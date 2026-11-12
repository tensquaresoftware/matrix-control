#include "Tools/SkinDesigner/SkinDesignerMainComponent.h"

SkinDesignerHistory::Snapshot SkinDesignerMainComponent::captureHistorySnapshot() const
{
    return { layer_.skins(), layer_.activeSkinId(), sessionState_.designerSkinId };
}

void SkinDesignerMainComponent::recordDocumentChange()
{
    history_.recordChange(captureHistorySnapshot());
    syncDesignerDirtyFromHistory();
}

void SkinDesignerMainComponent::handleLayerDocumentChanged()
{
    // Colour press: live preview only; one history step is sealed on gesture end.
    if (colourGestureActive_)
    {
        colourGestureMadeEdit_ = true;
        designerDirty_ = true;
        scheduleSessionAutosave();
        scheduleColourPreviewCoalesce();
        return;
    }

    openChangeBurst();
    recordDocumentChange();
    scheduleSessionAutosave();
    applyColourPreviewRefresh();
}

void SkinDesignerMainComponent::resetHistoryToCurrentDocument()
{
    closeChangeBurst();
    history_.reset(captureHistorySnapshot());
    designerDirty_ = false;
}

void SkinDesignerMainComponent::cancelInFlightColourGesture()
{
    colourPreviewCoalesceTimer_.stopTimer();
    colourGestureActive_ = false;
    colourGestureMadeEdit_ = false;
}

void SkinDesignerMainComponent::sealColourGestureBeforeSave()
{
    if (! colourGestureActive_)
        return;

    colourGestureActive_ = false;
    if (colourGestureMadeEdit_)
        recordDocumentChange();
    colourGestureMadeEdit_ = false;
    refreshSessionButtonsEnabled();
}

void SkinDesignerMainComponent::openChangeBurst()
{
    if (history_.isGestureActive())
        return;

    history_.beginGesture();
    changeBurstOpen_ = true;
    juce::MessageManager::callAsync([safe = juce::Component::SafePointer(this)]
    {
        if (safe == nullptr || ! safe->changeBurstOpen_)
            return;
        safe->changeBurstOpen_ = false;
        safe->history_.endGesture();
    });
}

void SkinDesignerMainComponent::closeChangeBurst()
{
    changeBurstOpen_ = false;
    history_.endGesture();
}

void SkinDesignerMainComponent::syncHistoryCurrentSelection()
{
    history_.syncCurrentSelection(layer_.activeSkinId(), sessionState_.designerSkinId);
}

bool SkinDesignerMainComponent::canUseDocumentHistory() const
{
    return SkinDesignerHistory::documentHistoryAllowed(isCompareProductView());
}

bool SkinDesignerMainComponent::isTextEditingFocused()
{
    return SkinDesignerHistory::shouldDeferDocumentUndoRedoToTextField(
        juce::Component::getCurrentlyFocusedComponent());
}

void SkinDesignerMainComponent::applyHistorySnapshot(const SkinDesignerHistory::Snapshot& snapshot)
{
    colourPreviewCoalesceTimer_.stopTimer();
    colourGestureActive_ = false;
    colourGestureMadeEdit_ = false;
    closeChangeBurst();

    // Copy first: the snapshot lives inside the history that keeps being used below.
    auto skins = snapshot.skins;
    runWithoutLayerChangeNotify([this, &skins, &snapshot]
    {
        layer_.replaceAll(std::move(skins), snapshot.activeSkinId);
    });
    sessionState_.designerSkinId = snapshot.designerSkinId;

    // Keep the selected pastille so COLOR SV follows Undo/Redo of colour edits.
    // Only drop bind-arming (those keys may not match the restored mappings).
    if (mappingMode_ == MappingMode::kArmedRow)
    {
        mappingMode_ = MappingMode::kIdle;
        armedKey_ = {};
        armedFromUnbound_ = false;
        mappingsPanel_.setPropertyBindHoverEnabled(false);
    }

    syncDesignerDirtyFromHistory();
    scheduleSessionAutosave();
    refreshAll(); // re-selects pastille if it still exists, syncs SV to restored ARGB

    if (sessionState_.designerSkinId.isNotEmpty())
        runWithoutLayerChangeNotify([this] { layer_.selectSkin(sessionState_.designerSkinId); });
    syncHistoryCurrentSelection();
    if (bridge_.isConnected())
        pushVisible();
}

void SkinDesignerMainComponent::finishSilentDocumentEdit()
{
    scheduleSessionAutosave();
    refreshAll();
    recordDocumentChange();
    refreshSessionButtonsEnabled();
    if (bridge_.isConnected() && ! isCompareProductView())
        pushVisible();
}

void SkinDesignerMainComponent::onUndo()
{
    // Mid-drag: the picker keeps sending ticks, so a restore here would be overwritten.
    if (! canUseDocumentHistory() || colourGestureActive_)
        return;
    if (const auto* previous = history_.undo())
        applyHistorySnapshot(*previous);
}

void SkinDesignerMainComponent::onRedo()
{
    // Mid-drag: the picker keeps sending ticks, so a restore here would be overwritten.
    if (! canUseDocumentHistory() || colourGestureActive_)
        return;
    if (const auto* next = history_.redo())
        applyHistorySnapshot(*next);
}
