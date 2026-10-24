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
