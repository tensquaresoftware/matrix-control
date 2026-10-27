#include "Tools/SkinEditor/SkinEditorMainComponent.h"

#include "GUI/Skins/Dev/DevSkinJson.h"
#include "Tools/SkinEditor/SkinEditorChromeTokens.h"
#include "Tools/SkinEditor/SkinEditorUiHelpers.h"

void SkinEditorMainComponent::onDuplicateProduct()
{
    duplicateFromSkin(sessionState_.productSkinId);
}

void SkinEditorMainComponent::onDuplicateEditor()
{
    if (sessionState_.editorSkinId.isEmpty())
        return;
    duplicateFromSkin(sessionState_.editorSkinId);
}

void SkinEditorMainComponent::duplicateFromSkin(const juce::String& sourceId)
{
    const auto* source = layer_.findSkin(sourceId);
    const auto sourceName = source != nullptr ? source->name : juce::String("My Skin");
    const auto suggested = SkinEditorUi::suggestUniqueSkinName(layer_, sourceName);
    const auto name = SkinEditorUi::askText("Duplicate",
                                            "Duplicate \"" + sourceName + "\" as:",
                                            suggested);
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
    clearMappingModeToIdle();
    // onChanged already refreshed with the previous Editor id — select the new skin now.
    refreshAll();
}

void SkinEditorMainComponent::onNew()
{
    // Product palette + custom Editor pastilles; mappings cleared → full Unbound list.
    const auto name = SkinEditorUi::askText("New Editor skin", "Name:",
                                            SkinEditorUi::suggestUniqueSkinName(layer_, "New Skin"));
    if (name.isEmpty())
        return;
    const auto result = layer_.createEditorSkinWithUnboundMappings(
        sessionState_.productSkinId, name, sessionState_.editorSkinId);
    if (result.failed())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "New failed", result.getErrorMessage());
        return;
    }
    sessionState_.editorSkinId = layer_.activeSkinId();
    sessionState_.compareOn = false;
    clearMappingModeToIdle();
    refreshAll();
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
    clearMappingModeToIdle();
}

void SkinEditorMainComponent::onRename()
{
    if (! canEditEditorSkin())
        return;
    const auto* skin = layer_.findSkin(sessionState_.editorSkinId);
    if (skin == nullptr)
        return;
    const auto name = SkinEditorUi::askText("Rename Editor skin", "New name:", skin->name);
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
        suggestedEditorSkinExportFile(),
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
        suggestedSkinJsonDirectory(),
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
                             rememberSkinJsonDirectory(file);
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

    const auto* skin = layer_.findSkin(sessionState_.editorSkinId);
    if (skin == nullptr)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Add failed", "Editor skin not found");
        return;
    }

    const auto name = SkinEditorUi::suggestUniqueTempPastilleName(*skin);
    const auto argb = SkinEditorUi::suggestUniqueOpaqueArgb(*skin);
    // Select before add so onChanged refresh highlights the new row without mapping click.
    const auto previousSelection = selectedPastille_;
    selectedPastille_ = name;
    const auto result = layer_.addPastille(sessionState_.editorSkinId, name, argb, true);
    if (result.failed())
    {
        selectedPastille_ = previousSelection;
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Add failed", result.getErrorMessage());
        return;
    }
    // refreshAll already quiet-selected; keep picker in sync if mapping mode was idle.
    syncColourPickerToSelection();
}

void SkinEditorMainComponent::onRenamePastille(const juce::String& pastilleName)
{
    if (! canEditEditorSkin() || pastilleName.isEmpty())
        return;
    if (layer_.isProductPastilleName(pastilleName))
        return;

    const auto name = SkinEditorUi::askText("Rename colour", "New name:", pastilleName);
    if (name.isEmpty() || name == pastilleName)
        return;

    // Update selection before rename so onChanged refresh finds the new name.
    const bool wasSelected = selectedPastille_ == pastilleName;
    if (wasSelected)
        selectedPastille_ = name;

    const auto result = layer_.renamePastille(sessionState_.editorSkinId, pastilleName, name);
    if (result.failed())
    {
        if (wasSelected)
            selectedPastille_ = pastilleName;
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Rename failed", result.getErrorMessage());
    }
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
    {
        if (mappingMode_ == MappingMode::kHeldColour)
            clearMappingModeToIdle();
        else
            selectedPastille_.clear();
    }
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
    else if (mappingMode_ == MappingMode::kHeldColour)
        updateMappingWitness();
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
    rememberSkinJsonDirectory(file);
    return true;
}

juce::File SkinEditorMainComponent::suggestedEditorSkinExportFile() const
{
    const auto* skin = layer_.findSkin(sessionState_.editorSkinId);
    return SkinEditorUi::defaultEditorSkinExportFile(
        skin != nullptr ? skin->name : juce::String(),
        sessionState_.lastSkinJsonDirectory);
}

juce::File SkinEditorMainComponent::suggestedSkinJsonDirectory() const
{
    return SkinEditorUi::resolveSkinJsonDirectory(sessionState_.lastSkinJsonDirectory);
}

void SkinEditorMainComponent::rememberSkinJsonDirectory(const juce::File& file)
{
    const auto parent = file.getParentDirectory();
    if (! parent.isDirectory())
        return;
    sessionState_.lastSkinJsonDirectory = parent.getFullPathName();
    autosaveSession();
}
