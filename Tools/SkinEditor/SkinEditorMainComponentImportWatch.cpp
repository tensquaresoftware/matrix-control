#include "Tools/SkinEditor/SkinEditorMainComponent.h"

#include "GUI/Skins/Dev/DevSkinJson.h"

namespace
{
    juce::String fingerprintJsonFile(const juce::File& file)
    {
        if (! file.existsAsFile())
            return {};
        const auto text = file.loadFileAsString();
        // Content hash (not mtime alone): Dropbox/sync tools can touch mtime without edits,
        // and same-size saves can share a coarse mtime tick.
        return juce::String(text.length()) + "|" + juce::String(text.hashCode64());
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
    watchedImportJsonFingerprint_ = fingerprintJsonFile(file);
    importWatchPendingFingerprint_ = {};
    importWatchPendingStablePolls_ = 0;
    importWatchDialogOpen_ = false;
    importWatchTimer_.onTick = [this] { pollWatchedImportJson(); };
    if (! importWatchTimer_.isTimerRunning())
        importWatchTimer_.startTimer(1000);
}

void SkinEditorMainComponent::clearWatchedImportJson()
{
    importWatchTimer_.stopTimer();
    watchedImportJsonFile_ = juce::File();
    watchedImportJsonFingerprint_ = {};
    importWatchPendingFingerprint_ = {};
    importWatchPendingStablePolls_ = 0;
    importWatchDialogOpen_ = false;
}

void SkinEditorMainComponent::refreshWatchedImportFingerprint()
{
    if (watchedImportJsonFile_ == juce::File())
        return;
    watchedImportJsonFingerprint_ = fingerprintJsonFile(watchedImportJsonFile_);
    importWatchPendingFingerprint_ = {};
    importWatchPendingStablePolls_ = 0;
}

void SkinEditorMainComponent::pollWatchedImportJson()
{
    if (importWatchDialogOpen_ || watchedImportJsonFile_ == juce::File())
        return;

    if (! watchedImportJsonFile_.existsAsFile())
    {
        // Transient rename-while-save is common; keep watching without prompting.
        importWatchPendingFingerprint_ = {};
        importWatchPendingStablePolls_ = 0;
        return;
    }

    const auto now = fingerprintJsonFile(watchedImportJsonFile_);
    if (now.isEmpty() || now == watchedImportJsonFingerprint_)
    {
        importWatchPendingFingerprint_ = {};
        importWatchPendingStablePolls_ = 0;
        return;
    }

    // Debounce: require the new fingerprint to stay stable across two polls (~2 s)
    // so a still-writing external save is less likely to yield truncated JSON.
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
    promptWatchedImportJsonExternalChange();
}

void SkinEditorMainComponent::promptWatchedImportJsonExternalChange()
{
    if (importWatchDialogOpen_)
        return;

    importWatchDialogOpen_ = true;
    const auto path = watchedImportJsonFile_.getFullPathName();

    juce::String message =
        "The JSON file currently loaded in the Skin Editor was modified by another "
        "application. Reload the updated file from disk?\n\n"
        "Reload replaces Editor skins with the contents of that file.";
    if (editorDirty_)
        message += "\n\nUnsaved editor changes will be lost.";
    message += "\n\n" + path;

    constexpr int kReloadResult = 1;
    constexpr int kDiscardResult = 2;

    juce::NativeMessageBox::showForOptionsAsync(
        juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::QuestionIcon)
            .withTitle("JSON File Changed")
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
                safe->reloadWatchedImportJsonFromDisk();
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

    const auto text = watchedImportJsonFile_.loadFileAsString();
    TSS::DevSkin::JsonIoResult result;
    runWithoutLayerChangeNotify([this, &text, &result]
    {
        result = TSS::DevSkin::importFromJson(layer_, text);
    });

    if (! result.ok)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Reload refused",
                                               result.message);
        // Layer untouched on failure (importFromJson replaces only after a full parse).
        refreshWatchedImportFingerprint();
        return;
    }

    {
        const auto active = layer_.activeSkinId();
        if (active.startsWith("dev:"))
            sessionState_.editorSkinId = active;
        else if (layer_.findSkin(sessionState_.editorSkinId) == nullptr)
            sessionState_.editorSkinId = {};
    }
    sessionState_.compareOn = false;
    rememberSkinJsonDirectory(watchedImportJsonFile_);
    refreshWatchedImportFingerprint();
    markEditorClean();
    setStatusMessage("Reloaded: " + watchedImportJsonFile_.getFullPathName());
    refreshAll();
    if (bridge_.isConnected())
        pushVisible();
}

void SkinEditorMainComponent::discardWatchedImportJsonExternalChange()
{
    // Keep in-memory skins; accept the new disk fingerprint until a later change.
    refreshWatchedImportFingerprint();
    setStatusMessage("Kept editor copy; disk JSON changed externally");
}
