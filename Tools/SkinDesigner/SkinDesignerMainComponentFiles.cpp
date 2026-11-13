#include "Tools/SkinDesigner/SkinDesignerMainComponent.h"

#include "GUI/Skins/Dev/DevSkinJson.h"
#include "Tools/SkinDesigner/SkinDesignerUiHelpers.h"

namespace
{
    constexpr int kLoadConfirmResult = 1;

    void showFileAlert(const juce::String& title, const juce::String& message)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               title,
                                               message);
    }
}

// Load -------------------------------------------------------------------------------------

void SkinDesignerMainComponent::onLoadJson()
{
    if (fileDialogOpen_)
        return;

    if (! designerDirty_)
    {
        browseForWorkspaceFile();
        return;
    }

    fileDialogOpen_ = true;
    juce::AlertWindow::showAsync(
        juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::QuestionIcon)
            .withTitle("Load JSON")
            .withMessage("Load replaces the current Designer skins.\n\n"
                         "Unsaved Designer changes will be lost.")
            .withButton("Load", kLoadConfirmResult)
            .withButton("Cancel", 0)
            .withAssociatedComponent(this),
        [safe = juce::Component::SafePointer<SkinDesignerMainComponent>(this)](int result)
        {
            if (safe == nullptr)
                return;
            safe->fileDialogOpen_ = false;
            if (result == kLoadConfirmResult)
                safe->browseForWorkspaceFile();
        });
}

void SkinDesignerMainComponent::browseForWorkspaceFile()
{
    fileDialogOpen_ = true;
    auto chooser = std::make_shared<juce::FileChooser>(
        "Load Designer skins JSON",
        suggestedSkinJsonDirectory(),
        "*.json");
    chooser->launchAsync(juce::FileBrowserComponent::openMode
                             | juce::FileBrowserComponent::canSelectFiles,
                         [safe = juce::Component::SafePointer<SkinDesignerMainComponent>(this),
                          chooser](const juce::FileChooser& fc)
                         {
                             if (safe == nullptr)
                                 return;
                             safe->fileDialogOpen_ = false;
                             const auto file = fc.getResult();
                             if (file != juce::File())
                                 safe->loadWorkspaceFromFile(file);
                         });
}

void SkinDesignerMainComponent::showNoDesignerSkinsInJsonAlert(const juce::String& title)
{
    juce::AlertWindow::showAsync(
        juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::WarningIcon)
            .withTitle(title)
            .withMessage(juce::String(TSS::DevSkin::kNoDesignerSkinsInJsonMessage)
                         + "\n\nCreate a new Designer skin or duplicate a Product skin to start.")
            .withButton("Got it", 0)
            .withAssociatedComponent(this),
        [](int) {});
}

void SkinDesignerMainComponent::handleWatchedImportReloadRefusal(const juce::String& message)
{
    // Accept fingerprint so Ask/Auto do not re-fire until the file changes again.
    refreshWatchedImportFingerprint();
    if (message == TSS::DevSkin::kNoDesignerSkinsInJsonMessage)
        showNoDesignerSkinsInJsonAlert("Reload refused");
    else
        showFileAlert("Reload refused", message);
}

void SkinDesignerMainComponent::loadWorkspaceFromFile(const juce::File& file)
{
    const auto text = file.existsAsFile() ? file.loadFileAsString() : juce::String();
    if (text.isEmpty())
    {
        showFileAlert("Load refused",
                      "The file is missing, empty or unreadable:\n" + file.getFullPathName());
        return;
    }

    TSS::DevSkin::JsonIoResult result;
    runWithoutLayerChangeNotify([this, &text, &result]
    {
        result = TSS::DevSkin::replaceDesignerSkinsFromJson(layer_, text);
    });
    if (! result.ok)
    {
        if (result.message == TSS::DevSkin::kNoDesignerSkinsInJsonMessage)
            showNoDesignerSkinsInJsonAlert("Load refused");
        else
            showFileAlert("Load refused", result.message);
        return;
    }

    cancelInFlightColourGesture();

    const auto* active = layer_.findSkin(layer_.activeSkinId());
    const bool activeIsDesigner = active != nullptr && active->kind == TSS::DevSkin::SkinKind::kDev;
    sessionState_.designerSkinId = activeIsDesigner ? layer_.activeSkinId()
                                                    : result.importedSkinIds[0];
    runWithoutLayerChangeNotify([this] { layer_.selectSkin(sessionState_.designerSkinId); });
    sessionState_.compareOn = false;
    selectedPastille_.clear();
    clearMappingModeToIdle();

    rememberSkinJsonDirectory(file);
    armWatchedImportJson(file);
    resetHistoryToCurrentDocument();
    setStatusMessage(juce::String(SkinDesignerUi::kStatusLoaded) + file.getFullPathName());
    refreshAll();
    if (bridge_.isConnected())
        pushVisible();
}

// Save / Save as ---------------------------------------------------------------------------

bool SkinDesignerMainComponent::hasAnyDesignerSkin() const
{
    for (const auto& skin : layer_.skins())
        if (skin.kind == TSS::DevSkin::SkinKind::kDev)
            return true;
    return false;
}

void SkinDesignerMainComponent::onSave()
{
    if (! hasAnyDesignerSkin() || fileDialogOpen_)
        return;

    // Known path (Load / earlier Save): overwrite it without a chooser.
    if (watchedImportJsonFile_ != juce::File())
        savePackToFile(watchedImportJsonFile_, SkinDesignerUi::kStatusSaved);
    else
        browseToSavePack(SkinDesignerUi::kStatusSaved);
}

void SkinDesignerMainComponent::onSaveAs()
{
    if (hasAnyDesignerSkin() && ! fileDialogOpen_)
        browseToSavePack(SkinDesignerUi::kStatusSavedAs);
}

void SkinDesignerMainComponent::browseToSavePack(const char* statusPrefix)
{
    fileDialogOpen_ = true;
    auto chooser = std::make_shared<juce::FileChooser>(
        "Save Designer skins JSON",
        suggestedSaveAsFile(),
        "*.json");
    chooser->launchAsync(juce::FileBrowserComponent::saveMode
                             | juce::FileBrowserComponent::canSelectFiles
                             | juce::FileBrowserComponent::warnAboutOverwriting,
                         [safe = juce::Component::SafePointer<SkinDesignerMainComponent>(this),
                          chooser, statusPrefix](const juce::FileChooser& fc)
                         {
                             if (safe == nullptr)
                                 return;
                             safe->fileDialogOpen_ = false;
                             const auto file = fc.getResult();
                             if (file != juce::File())
                                 safe->savePackToFile(file, statusPrefix);
                         });
}

bool SkinDesignerMainComponent::savePackToFile(const juce::File& chosen, const char* statusPrefix)
{
    const auto file = SkinDesignerUi::withJsonExtension(chosen);

    // The pack records activeSkinId from the layer; keep it on the Designer skin in use.
    if (sessionState_.designerSkinId.isNotEmpty())
    {
        runWithoutLayerChangeNotify([this] { layer_.selectSkin(sessionState_.designerSkinId); });
        syncHistoryCurrentSelection();
    }

    if (! file.replaceWithText(TSS::DevSkin::exportToJson(layer_)))
    {
        showFileAlert("Save failed", "Could not write file:\n" + file.getFullPathName());
        return false;
    }

    rememberSkinJsonDirectory(file);
    // Our own write: re-baseline the watch on this path without prompting.
    armWatchedImportJson(file);
    sealColourGestureBeforeSave();
    markDesignerClean();
    setStatusMessage(juce::String(statusPrefix) + file.getFullPathName());
    refreshStatus();
    return true;
}

bool SkinDesignerMainComponent::saveForQuit()
{
    if (watchedImportJsonFile_ != juce::File())
        return savePackToFile(watchedImportJsonFile_, SkinDesignerUi::kStatusSaved);

    // Modal sync chooser for the quit path; Cancel keeps the app open.
    auto chooser = std::make_shared<juce::FileChooser>(
        "Save Designer skins JSON",
        suggestedSaveAsFile(),
        "*.json");
    return chooser->browseForFileToSave(true)
           && savePackToFile(chooser->getResult(), SkinDesignerUi::kStatusSaved);
}

juce::File SkinDesignerMainComponent::suggestedSaveAsFile() const
{
    if (watchedImportJsonFile_ != juce::File())
        return watchedImportJsonFile_;
    return SkinDesignerUi::defaultPackFile(SkinDesignerUi::kDefaultWorkspaceName,
                                           sessionState_.lastSkinJsonDirectory);
}

// Shared -----------------------------------------------------------------------------------

juce::File SkinDesignerMainComponent::suggestedSkinJsonDirectory() const
{
    return SkinDesignerUi::resolveSkinJsonDirectory(sessionState_.lastSkinJsonDirectory);
}

void SkinDesignerMainComponent::rememberSkinJsonDirectory(const juce::File& file)
{
    const auto parent = file.getParentDirectory();
    if (! parent.isDirectory())
        return;
    sessionState_.lastSkinJsonDirectory = parent.getFullPathName();
    autosaveSession();
}
