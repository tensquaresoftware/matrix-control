#include "Tools/SkinEditor/SkinEditorMainComponent.h"

#include "GUI/Skins/Dev/DevSkinJson.h"

#include <map>

namespace
{
    juce::String fingerprintJsonFile(const juce::File& file)
    {
        if (! file.existsAsFile())
            return {};

        const auto size = file.getSize();
        const auto text = file.loadFileAsString();
        // Dropbox / atomic saves can briefly yield an empty read while size > 0.
        if (size > 0 && text.isEmpty())
            return {};

        // Content only (size + hash): mtime-only touches must not look like an edit.
        return juce::String(size) + "|" + juce::String(text.hashCode64());
    }

    juce::int64 fingerprintEditorSkin(const TSS::DevSkin::NamedSkin& skin)
    {
        return juce::String(TSS::DevSkin::exportEditorSkinToJson(skin)).hashCode64();
    }

    struct EditorSkinSnapshot
    {
        juce::String id;
        juce::String name;
        juce::int64 fingerprint = 0;
    };

    std::vector<EditorSkinSnapshot> captureEditorSkinSnapshots(const TSS::DevSkin::Layer& layer)
    {
        std::vector<EditorSkinSnapshot> out;
        for (const auto& skin : layer.skins())
        {
            if (skin.kind != TSS::DevSkin::SkinKind::kDev)
                continue;
            out.push_back({ skin.id, skin.name, fingerprintEditorSkin(skin) });
        }
        return out;
    }

    void collectChangedEditorSkins(const TSS::DevSkin::Layer& layer,
                                   const std::vector<EditorSkinSnapshot>& before,
                                   juce::StringArray& changedIds,
                                   juce::StringArray& changedNames)
    {
        std::map<juce::String, juce::int64> beforeById;
        for (const auto& snap : before)
            beforeById[snap.id] = snap.fingerprint;

        for (const auto& skin : layer.skins())
        {
            if (skin.kind != TSS::DevSkin::SkinKind::kDev)
                continue;
            const auto fp = fingerprintEditorSkin(skin);
            const auto it = beforeById.find(skin.id);
            if (it == beforeById.end() || it->second != fp)
            {
                changedIds.add(skin.id);
                changedNames.add(skin.name.isNotEmpty() ? skin.name : skin.id);
            }
        }
    }

    juce::String pickEditorSkinAfterReload(const TSS::DevSkin::Layer& layer,
                                           const juce::String& previousEditorId,
                                           const juce::StringArray& changedIds)
    {
        const auto* previous = previousEditorId.isNotEmpty() ? layer.findSkin(previousEditorId)
                                                             : nullptr;
        const bool previousStillThere =
            previous != nullptr && previous->kind == TSS::DevSkin::SkinKind::kDev;

        if (previousStillThere)
        {
            if (changedIds.isEmpty() || changedIds.contains(previousEditorId))
                return previousEditorId;
            return changedIds[0];
        }

        if (! changedIds.isEmpty())
            return changedIds[0];

        for (const auto& skin : layer.skins())
            if (skin.kind == TSS::DevSkin::SkinKind::kDev)
                return skin.id;
        return {};
    }

    juce::String reloadStatusMessage(const juce::File& file,
                                     const juce::StringArray& changedNames)
    {
        if (changedNames.isEmpty())
            return "Reloaded: " + file.getFullPathName();
        if (changedNames.size() == 1)
            return "Reloaded: 1 skin updated (" + changedNames[0] + ")";
        return "Reloaded: " + juce::String(changedNames.size()) + " skins updated ("
               + changedNames.joinIntoString(", ") + ")";
    }
}

void SkinEditorMainComponent::armWatchedImportJson(const juce::File& file)
{
    if (! file.existsAsFile())
    {
        clearWatchedImportJson();
        return;
    }

    watchedImportJsonFile_ = file;
    sessionState_.lastImportedJsonPath = file.getFullPathName();
    watchedImportJsonFingerprint_ = fingerprintJsonFile(file);
    sessionState_.lastImportedJsonFingerprint = watchedImportJsonFingerprint_;
    importWatchPendingFingerprint_ = {};
    importWatchPendingStablePolls_ = 0;
    importWatchDialogOpen_ = false;
    syncImportWatchTimerToMode();
    scheduleSessionAutosave();
}

void SkinEditorMainComponent::clearWatchedImportJson()
{
    importWatchTimer_.stopTimer();
    watchedImportJsonFile_ = juce::File();
    watchedImportJsonFingerprint_ = {};
    importWatchPendingFingerprint_ = {};
    importWatchPendingStablePolls_ = 0;
    importWatchDialogOpen_ = false;
    if (sessionState_.lastImportedJsonPath.isNotEmpty()
        || sessionState_.lastImportedJsonFingerprint.isNotEmpty())
    {
        sessionState_.lastImportedJsonPath = {};
        sessionState_.lastImportedJsonFingerprint = {};
        scheduleSessionAutosave();
    }
}

void SkinEditorMainComponent::refreshWatchedImportFingerprint()
{
    if (watchedImportJsonFile_ == juce::File())
        return;
    const auto fp = fingerprintJsonFile(watchedImportJsonFile_);
    if (fp.isEmpty())
        return; // keep previous baseline across a transient unreadable read
    watchedImportJsonFingerprint_ = fp;
    sessionState_.lastImportedJsonFingerprint = fp;
    importWatchPendingFingerprint_ = {};
    importWatchPendingStablePolls_ = 0;
    scheduleSessionAutosave();
}

void SkinEditorMainComponent::reconcileWatchedImportJsonOnLaunch()
{
    if (sessionState_.lastImportedJsonPath.isEmpty())
    {
        if (sessionState_.lastImportedJsonFingerprint.isNotEmpty())
            sessionState_.lastImportedJsonFingerprint = {};

        // Session load may have dropped a missing Import path in memory only — flush disk.
        const auto sessionFile = TSS::DevSkin::skinEditorSessionFile();
        if (sessionFile.existsAsFile())
        {
            const auto parsed = juce::JSON::parse(sessionFile.loadFileAsString());
            if (const auto* root = parsed.getDynamicObject();
                root != nullptr && root->getProperty("lastImportedJsonPath").toString().isNotEmpty())
            {
                scheduleSessionAutosave();
            }
        }
        return;
    }

    const juce::File file(sessionState_.lastImportedJsonPath);
    if (! file.existsAsFile())
    {
        clearWatchedImportJson();
        return;
    }

    const auto now = fingerprintJsonFile(file);
    const auto stored = sessionState_.lastImportedJsonFingerprint;

    auto armWatchKeepingStoredBaseline = [this, &file, &stored]
    {
        watchedImportJsonFile_ = file;
        watchedImportJsonFingerprint_ = stored;
        importWatchPendingFingerprint_ = {};
        importWatchPendingStablePolls_ = 0;
        importWatchDialogOpen_ = false;
        syncImportWatchTimerToMode();
    };

    // Transient unreadable read with a known baseline: watch without accepting disk yet.
    if (now.isEmpty() && stored.isNotEmpty())
    {
        armWatchKeepingStoredBaseline();
        return;
    }

    // Legacy sessions (no stored fingerprint) or matching disk: accept and arm.
    if (stored.isEmpty() || stored == now)
    {
        armWatchedImportJson(file);
        return;
    }

    // Disk differs from last accepted baseline — keep stored fingerprint until handled.
    armWatchKeepingStoredBaseline();
    handleWatchedImportJsonExternalChange(true);
}

void SkinEditorMainComponent::pollWatchedImportJson()
{
    if (importWatchDialogOpen_ || watchedImportJsonFile_ == juce::File())
        return;

    if (! watchedImportJsonFile_.existsAsFile())
    {
        // Transient rename-while-save is common; keep watching without prompting.
        return;
    }

    const auto now = fingerprintJsonFile(watchedImportJsonFile_);
    if (now.isEmpty())
        return; // keep pending across a brief unreadable read

    if (now == watchedImportJsonFingerprint_)
    {
        importWatchPendingFingerprint_ = {};
        importWatchPendingStablePolls_ = 0;
        return;
    }

    // Debounce: require the new fingerprint to stay stable across two polls (~2 s).
    if (now != importWatchPendingFingerprint_)
    {
        importWatchPendingFingerprint_ = now;
        importWatchPendingStablePolls_ = 1;
        return;
    }

    ++importWatchPendingStablePolls_;
    if (importWatchPendingStablePolls_ < 2)
        return;

    importWatchPendingFingerprint_ = {};
    importWatchPendingStablePolls_ = 0;
    handleWatchedImportJsonExternalChange(false);
}

void SkinEditorMainComponent::syncImportWatchTimerToMode()
{
    if (sessionState_.jsonSyncMode == TSS::DevSkin::JsonSyncMode::kOff
        || watchedImportJsonFile_ == juce::File())
    {
        importWatchTimer_.stopTimer();
        return;
    }

    importWatchTimer_.onTick = [this] { pollWatchedImportJson(); };
    if (! importWatchTimer_.isTimerRunning())
        importWatchTimer_.startTimer(1000);
}

void SkinEditorMainComponent::syncJsonSyncComboFromSession()
{
    const juce::ScopedValueSetter<bool> guard(suppressCombo_, true);
    jsonSyncCombo_.setSelectedId(static_cast<int>(sessionState_.jsonSyncMode) + 1,
                                 juce::dontSendNotification);
}

void SkinEditorMainComponent::applyJsonSyncComboChange()
{
    const auto id = jsonSyncCombo_.getSelectedId();
    if (id < 1)
        return;

    sessionState_.jsonSyncMode = static_cast<TSS::DevSkin::JsonSyncMode>(id - 1);
    syncImportWatchTimerToMode();
    scheduleSessionAutosave();
}

void SkinEditorMainComponent::applyJsonSyncSessionUi()
{
    syncJsonSyncComboFromSession();
    syncImportWatchTimerToMode();
}

void SkinEditorMainComponent::handleWatchedImportJsonExternalChange(bool fromLaunch)
{
    using Mode = TSS::DevSkin::JsonSyncMode;
    switch (sessionState_.jsonSyncMode)
    {
        case Mode::kOff:
            return;
        case Mode::kAuto:
            // Disk wins without a modal (including when the editor was dirty).
            reloadWatchedImportJsonFromDisk();
            return;
        case Mode::kAsk:
            break;
    }
    promptWatchedImportJsonExternalChange(fromLaunch);
}

void SkinEditorMainComponent::promptWatchedImportJsonExternalChange(bool fromLaunch)
{
    if (importWatchDialogOpen_)
        return;

    importWatchDialogOpen_ = true;
    importWatchPromptFromLaunch_ = fromLaunch;
    const auto path = watchedImportJsonFile_.getFullPathName();

    juce::String message =
        fromLaunch
            ? juce::String(
                  "The JSON file from your last session differs from the Editor skins "
                  "currently restored. Reload the file from disk?\n\n"
                  "Reload replaces Editor skins with the contents of that file.")
            : juce::String(
                  "The JSON file currently loaded in the Skin Editor was modified by another "
                  "application. Reload the updated file from disk?\n\n"
                  "Reload replaces Editor skins with the contents of that file.");
    if (editorDirty_)
        message += "\n\nUnsaved editor changes will be lost.";
    message += "\n\n" + path;

    constexpr int kReloadResult = 1;
    constexpr int kDiscardResult = 2;
    const auto title = fromLaunch ? "JSON Out of Sync" : "JSON File Changed";

    // JUCE AlertWindow (not NativeMessageBox) so Skin Editor LookAndFeel can theme it.
    juce::AlertWindow::showAsync(
        juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::QuestionIcon)
            .withTitle(title)
            .withMessage(message)
            .withButton("Reload", kReloadResult)
            .withButton("Discard", kDiscardResult)
            .withAssociatedComponent(this),
        [safe = juce::Component::SafePointer<SkinEditorMainComponent>(this)](int result)
        {
            if (safe == nullptr)
                return;
            safe->importWatchDialogOpen_ = false;
            if (result == kReloadResult)
            {
                safe->importWatchPromptFromLaunch_ = false;
                safe->reloadWatchedImportJsonFromDisk();
            }
            else
                safe->discardWatchedImportJsonExternalChange();
        });
}

void SkinEditorMainComponent::reloadWatchedImportJsonFromDisk()
{
    if (watchedImportJsonFile_ == juce::File() || ! watchedImportJsonFile_.existsAsFile())
    {
        clearWatchedImportJson();
        setStatusMessage("Reload failed: watched JSON is missing");
        return;
    }

    const auto file = watchedImportJsonFile_;
    const auto previousEditorId = sessionState_.editorSkinId;
    const auto beforeSkins = captureEditorSkinSnapshots(layer_);
    const auto text = file.loadFileAsString();

    TSS::DevSkin::JsonIoResult result;
    runWithoutLayerChangeNotify([this, &text, &result]
    {
        result = TSS::DevSkin::importFromJson(layer_, text);
    });

    if (! result.ok)
    {
        // Keep the previous accepted baseline so the watch can prompt again
        // (do not silently accept the refused on-disk bytes).
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Reload refused",
                                               result.message);
        return;
    }

    juce::StringArray changedIds;
    juce::StringArray changedNames;
    collectChangedEditorSkins(layer_, beforeSkins, changedIds, changedNames);

    const auto nextEditorId = pickEditorSkinAfterReload(layer_, previousEditorId, changedIds);
    sessionState_.editorSkinId = nextEditorId;
    if (nextEditorId.isNotEmpty())
        runWithoutLayerChangeNotify([this, nextEditorId] { layer_.selectSkin(nextEditorId); });

    sessionState_.compareOn = false;
    rememberSkinJsonDirectory(file);
    armWatchedImportJson(file); // re-baseline + ensure timer still running
    markEditorClean();
    refreshAll();
    if (bridge_.isConnected())
        pushVisible();

    const auto reloadDetail = reloadStatusMessage(file, changedNames);
    if (sessionState_.jsonSyncMode == TSS::DevSkin::JsonSyncMode::kAuto)
    {
        const auto stamp = juce::Time::getCurrentTime().formatted("%Y-%m-%d %H:%M:%S");
        showJsonSyncReloadNotice("Reloaded at " + stamp + " — " + reloadDetail);
    }
    else
        setStatusMessage(reloadDetail);
}

void SkinEditorMainComponent::showJsonSyncReloadNotice(const juce::String& detail)
{
    if (! jsonSyncReloadNoticeVisible_)
        statusDetailBeforeJsonSyncNotice_ = statusDetail_;
    jsonSyncReloadNoticeVisible_ = true;
    jsonSyncReloadNoticeText_ = detail;
    resized();
    repaint();
}

void SkinEditorMainComponent::dismissJsonSyncReloadNotice()
{
    if (! jsonSyncReloadNoticeVisible_)
        return;

    jsonSyncReloadNoticeVisible_ = false;
    jsonSyncReloadNoticeText_.clear();
    statusDetail_ = statusDetailBeforeJsonSyncNotice_;
    statusDetailBeforeJsonSyncNotice_.clear();
    resized();
    repaint();
}


void SkinEditorMainComponent::discardWatchedImportJsonExternalChange()
{
    // Keep in-memory skins; accept the new disk fingerprint until a later change.
    const bool fromLaunch = importWatchPromptFromLaunch_;
    importWatchPromptFromLaunch_ = false;
    refreshWatchedImportFingerprint();
    setStatusMessage(fromLaunch ? "Kept session skins; Import JSON left unchanged on disk"
                                : "Kept editor copy; disk JSON changed externally");
}
