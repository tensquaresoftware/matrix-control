#include "Tools/SkinDesigner/SkinDesignerMainComponent.h"

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

    juce::int64 fingerprintDesignerSkin(const TSS::DevSkin::NamedSkin& skin)
    {
        return juce::String(TSS::DevSkin::exportDesignerSkinToJson(skin)).hashCode64();
    }

    struct DesignerSkinSnapshot
    {
        juce::String id;
        juce::String name;
        juce::int64 fingerprint = 0;
    };

    std::vector<DesignerSkinSnapshot> captureDesignerSkinSnapshots(const TSS::DevSkin::Layer& layer)
    {
        std::vector<DesignerSkinSnapshot> out;
        for (const auto& skin : layer.skins())
        {
            if (skin.kind != TSS::DevSkin::SkinKind::kDev)
                continue;
            out.push_back({ skin.id, skin.name, fingerprintDesignerSkin(skin) });
        }
        return out;
    }

    void collectChangedDesignerSkins(const TSS::DevSkin::Layer& layer,
                                   const std::vector<DesignerSkinSnapshot>& before,
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
            const auto fp = fingerprintDesignerSkin(skin);
            const auto it = beforeById.find(skin.id);
            if (it == beforeById.end() || it->second != fp)
            {
                changedIds.add(skin.id);
                changedNames.add(skin.name.isNotEmpty() ? skin.name : skin.id);
            }
        }
    }

    juce::String pickDesignerSkinAfterReload(const TSS::DevSkin::Layer& layer,
                                           const juce::String& previousDesignerId,
                                           const juce::StringArray& changedIds)
    {
        const auto* previous = previousDesignerId.isNotEmpty() ? layer.findSkin(previousDesignerId)
                                                             : nullptr;
        const bool previousStillThere =
            previous != nullptr && previous->kind == TSS::DevSkin::SkinKind::kDev;

        if (previousStillThere)
        {
            if (changedIds.isEmpty() || changedIds.contains(previousDesignerId))
                return previousDesignerId;
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

void SkinDesignerMainComponent::armWatchedImportJson(const juce::File& file)
{
    if (! file.existsAsFile())
    {
        clearWatchedImportJson();
        return;
    }

    watchedImportJsonFile_ = file;
    watchedImportJsonFingerprint_ = fingerprintJsonFile(file);
    importWatchPendingFingerprint_ = {};
    importWatchPendingStablePolls_ = 0;
    importWatchDialogOpen_ = false;
    syncImportWatchTimerToMode();
}

void SkinDesignerMainComponent::clearWatchedImportJson()
{
    importWatchTimer_.stopTimer();
    watchedImportJsonFile_ = juce::File();
    watchedImportJsonFingerprint_ = {};
    importWatchPendingFingerprint_ = {};
    importWatchPendingStablePolls_ = 0;
    importWatchDialogOpen_ = false;
}

void SkinDesignerMainComponent::refreshWatchedImportFingerprint()
{
    if (watchedImportJsonFile_ == juce::File())
        return;
    const auto fp = fingerprintJsonFile(watchedImportJsonFile_);
    if (fp.isEmpty())
        return; // keep previous baseline across a transient unreadable read
    watchedImportJsonFingerprint_ = fp;
    importWatchPendingFingerprint_ = {};
    importWatchPendingStablePolls_ = 0;
}

void SkinDesignerMainComponent::pollWatchedImportJson()
{
    if (importWatchDialogOpen_ || fileDialogOpen_ || watchedImportJsonFile_ == juce::File())
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
    handleWatchedImportJsonExternalChange();
}

void SkinDesignerMainComponent::syncImportWatchTimerToMode()
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

void SkinDesignerMainComponent::syncJsonSyncComboFromSession()
{
    const juce::ScopedValueSetter<bool> guard(suppressCombo_, true);
    jsonSyncCombo_.setSelectedId(static_cast<int>(sessionState_.jsonSyncMode) + 1,
                                 juce::dontSendNotification);
}

void SkinDesignerMainComponent::applyJsonSyncComboChange()
{
    const auto id = jsonSyncCombo_.getSelectedId();
    if (id < 1)
        return;

    sessionState_.jsonSyncMode = static_cast<TSS::DevSkin::JsonSyncMode>(id - 1);
    syncImportWatchTimerToMode();
    scheduleSessionAutosave();
}

void SkinDesignerMainComponent::applyJsonSyncSessionUi()
{
    syncJsonSyncComboFromSession();
    syncImportWatchTimerToMode();
}

void SkinDesignerMainComponent::handleWatchedImportJsonExternalChange()
{
    using Mode = TSS::DevSkin::JsonSyncMode;
    switch (sessionState_.jsonSyncMode)
    {
        case Mode::kOff:
            return;
        case Mode::kAuto:
            // Disk wins without a modal (including when the Designer was dirty).
            reloadWatchedImportJsonFromDisk();
            return;
        case Mode::kAsk:
            break;
    }
    promptWatchedImportJsonExternalChange();
}

void SkinDesignerMainComponent::promptWatchedImportJsonExternalChange()
{
    if (importWatchDialogOpen_)
        return;

    importWatchDialogOpen_ = true;
    const auto path = watchedImportJsonFile_.getFullPathName();

    juce::String message =
        "The JSON file loaded in the Skin Designer was modified by another "
        "application. Reload the updated file from disk?\n\n"
        "Reload replaces Designer skins with the contents of that file.";
    if (designerDirty_)
        message += "\n\nUnsaved Designer changes will be lost.";
    message += "\n\n" + path;

    constexpr int kReloadResult = 1;
    constexpr int kDiscardResult = 2;

    // JUCE AlertWindow (not NativeMessageBox) so Skin Designer LookAndFeel can theme it.
    juce::AlertWindow::showAsync(
        juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::QuestionIcon)
            .withTitle("JSON File Changed")
            .withMessage(message)
            .withButton("Reload", kReloadResult)
            .withButton("Discard", kDiscardResult)
            .withAssociatedComponent(this),
        [safe = juce::Component::SafePointer<SkinDesignerMainComponent>(this)](int result)
        {
            if (safe == nullptr)
                return;
            safe->importWatchDialogOpen_ = false;
            if (result == kReloadResult)
                safe->reloadWatchedImportJsonFromDisk();
            else
                safe->discardWatchedImportJsonExternalChange();
        });
}

void SkinDesignerMainComponent::reloadWatchedImportJsonFromDisk()
{
    if (watchedImportJsonFile_ == juce::File() || ! watchedImportJsonFile_.existsAsFile())
    {
        clearWatchedImportJson();
        setStatusMessage("Reload failed: watched JSON is missing");
        return;
    }

    const auto file = watchedImportJsonFile_;
    const auto previousDesignerId = sessionState_.designerSkinId;
    const auto beforeSkins = captureDesignerSkinSnapshots(layer_);
    const auto text = file.loadFileAsString();

    TSS::DevSkin::JsonIoResult result;
    runWithoutLayerChangeNotify([this, &text, &result]
    {
        result = TSS::DevSkin::replaceDesignerSkinsFromJson(layer_, text);
    });

    if (! result.ok)
    {
        handleWatchedImportReloadRefusal(result.message);
        return;
    }

    juce::StringArray changedIds;
    juce::StringArray changedNames;
    collectChangedDesignerSkins(layer_, beforeSkins, changedIds, changedNames);

    const auto nextDesignerId = pickDesignerSkinAfterReload(layer_, previousDesignerId, changedIds);
    sessionState_.designerSkinId = nextDesignerId;
    if (nextDesignerId.isNotEmpty())
        runWithoutLayerChangeNotify([this, nextDesignerId] { layer_.selectSkin(nextDesignerId); });

    sessionState_.compareOn = false;
    rememberSkinJsonDirectory(file);
    armWatchedImportJson(file); // re-baseline + ensure timer still running
    cancelInFlightColourGesture();
    resetHistoryToCurrentDocument();
    refreshAll();
    if (bridge_.isConnected())
        pushVisible();

    const auto reloadDetail = reloadStatusMessage(file, changedNames);
    if (sessionState_.jsonSyncMode == TSS::DevSkin::JsonSyncMode::kAuto)
    {
        const auto stamp = juce::Time::getCurrentTime().formatted("%Y-%m-%d %H:%M:%S");
        showJsonSyncReloadNotice("Reloaded at " + stamp + " - " + reloadDetail);
    }
    else
        setStatusMessage(reloadDetail);
}

void SkinDesignerMainComponent::showJsonSyncReloadNotice(const juce::String& detail)
{
    if (! jsonSyncReloadNoticeVisible_)
        statusDetailBeforeJsonSyncNotice_ = statusDetail_;
    jsonSyncReloadNoticeVisible_ = true;
    jsonSyncReloadNoticeText_ = detail;
    resized();
    repaint();
}

void SkinDesignerMainComponent::dismissJsonSyncReloadNotice()
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


void SkinDesignerMainComponent::discardWatchedImportJsonExternalChange()
{
    // Keep in-memory skins; accept the new disk fingerprint until a later change.
    refreshWatchedImportFingerprint();
    setStatusMessage("Kept Designer copy; disk JSON changed externally");
}
