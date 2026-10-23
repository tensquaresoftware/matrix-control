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
    titleLabel_.setText("Matrix-Control Skin Editor", juce::dontSendNotification);
    titleLabel_.setFont(juce::Font(juce::FontOptions(18.0f)).boldened());
    titleLabel_.setColour(juce::Label::textColourId, SkinEditorChrome::textPrimary());
    addAndMakeVisible(titleLabel_);
    connectionLabel_.setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
    lastPushLabel_.setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
    addAndMakeVisible(connectionLabel_);
    addAndMakeVisible(lastPushLabel_);

    for (auto* label : { &productLabel_, &editorLabel_ })
    {
        label->setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
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
    wire(duplicateButton_, [this] { onDuplicate(); });
    wire(newButton_, [this] { onNew(); });
    wire(renameButton_, [this] { onRename(); });
    wire(deleteButton_, [this] { onDelete(); });
    wire(compareButton_, [this] { onCompareToggle(); });
    wire(pushButton_, [this] { onPush(); });
    wire(exportButton_, [this] { onExportJson(); });
    wire(importButton_, [this] { onImportJson(); });

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
    auto band = getLocalBounds().removeFromTop(SkinEditorChrome::kBandHeight);
    g.setColour(SkinEditorChrome::surfaceBand());
    g.fillRect(band);
}

void SkinEditorMainComponent::resized()
{
    auto bounds = getLocalBounds();
    auto band = bounds.removeFromTop(SkinEditorChrome::kBandHeight).reduced(SkinEditorChrome::kPanelPad);

    auto titleRow = band.removeFromTop(26);
    titleLabel_.setBounds(titleRow.removeFromLeft(320));
    connectionLabel_.setBounds(titleRow.removeFromLeft(220));
    lastPushLabel_.setBounds(titleRow);

    auto comboRow = band.removeFromTop(28);
    productLabel_.setBounds(comboRow.removeFromLeft(90));
    productCombo_.setBounds(comboRow.removeFromLeft(140).reduced(2, 0));
    comboRow.removeFromLeft(12);
    editorLabel_.setBounds(comboRow.removeFromLeft(80));
    editorCombo_.setBounds(comboRow.removeFromLeft(180).reduced(2, 0));

    auto actions = band;
    auto place = [&actions](juce::TextButton& b, int w)
    {
        b.setBounds(actions.removeFromLeft(w).reduced(2, 0));
    };
    place(duplicateButton_, 90);
    place(newButton_, 60);
    place(renameButton_, 80);
    place(deleteButton_, 70);
    place(compareButton_, 90);
    place(pushButton_, 70);
    place(exportButton_, 80);
    place(importButton_, 80);

    bounds.reduce(0, SkinEditorChrome::kGutter);
    const int gutter = SkinEditorChrome::kGutter;
    const int colW = (bounds.getWidth() - gutter * 3) / 4;
    auto placeCol = [&bounds, colW](juce::Component& c)
    {
        c.setBounds(bounds.removeFromLeft(colW));
        bounds.removeFromLeft(SkinEditorChrome::kGutter);
    };
    placeCol(colourPicker_);
    placeCol(palettePanel_);
    placeCol(mappingsPanel_);
    unboundPanel_.setBounds(bounds);
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
        const auto id = editorCombo_.getText();
        for (const auto& skin : layer_.skins())
        {
            if (skin.kind == TSS::DevSkin::SkinKind::kDev && skin.name == id)
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

    compareButton_.setToggleState(sessionState_.compareOn, juce::dontSendNotification);
    compareButton_.setButtonText(sessionState_.compareOn ? "Compare: ON" : "Compare");

    renameButton_.setEnabled(editable);
    deleteButton_.setEnabled(editable);
    newButton_.setEnabled(true);
    duplicateButton_.setEnabled(true);

    palettePanel_.setActiveSkinId(visibleId);
    palettePanel_.setReadOnly(! editable);
    palettePanel_.refresh();

    mappingsPanel_.setActiveSkinId(visibleId);
    mappingsPanel_.setReadOnly(compareProduct || ! editable);
    mappingsPanel_.refresh();

    unboundPanel_.setActiveSkinId(visibleId);
    unboundPanel_.setForcedEmpty(compareProduct);
    unboundPanel_.setReadOnly(compareProduct || ! editable);
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
    productCombo_.addItem("Black", 1);
    productCombo_.addItem("Cream", 2);
    productCombo_.setSelectedId(sessionState_.productSkinId == TSS::DevSkin::kCompiledBlackId ? 1 : 2,
                                juce::dontSendNotification);

    editorCombo_.clear(juce::dontSendNotification);
    int selected = 0;
    int index = 1;
    for (const auto& skin : layer_.skins())
    {
        if (skin.kind != TSS::DevSkin::SkinKind::kDev)
            continue;
        editorCombo_.addItem(skin.name, index);
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
            if (skin.kind == TSS::DevSkin::SkinKind::kDev && skin.name == editorCombo_.getText())
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
