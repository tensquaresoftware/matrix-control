#include "Tools/SkinEditor/SkinEditorMainComponent.h"

#include "GUI/Skins/Dev/DevSkinJson.h"
#include "GUI/Skins/Dev/DevSkinKeyNames.h"

namespace
{
    juce::String askText(juce::Component* /*parent*/,
                         const juce::String& title,
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

    juce::uint32 parseArgb(const juce::String& text, bool& ok)
    {
        auto cleaned = text.trim().trimCharactersAtStart("#");
        if (cleaned.startsWithIgnoreCase("0x"))
            cleaned = cleaned.substring(2);
        if (cleaned.length() == 6)
            cleaned = "FF" + cleaned;
        ok = cleaned.length() == 8;
        if (! ok)
            return 0;
        return static_cast<juce::uint32>(cleaned.getHexValue64());
    }
}

SkinEditorMainComponent::SkinEditorMainComponent()
{
    titleLabel_.setText("Matrix-Control Skin Editor", juce::dontSendNotification);
    titleLabel_.setFont(juce::Font(juce::FontOptions(18.0f)).boldened());
    addAndMakeVisible(titleLabel_);
    addAndMakeVisible(connectionLabel_);
    addAndMakeVisible(lastPushLabel_);
    addAndMakeVisible(readOnlyLabel_);

    auto wireTab = [this](juce::TextButton& button, ListMode mode)
    {
        addAndMakeVisible(button);
        button.onClick = [this, mode]
        {
            listMode_ = mode;
            refreshAll();
        };
    };
    wireTab(skinsTab_, ListMode::kSkins);
    wireTab(paletteTab_, ListMode::kPalette);
    wireTab(mappingsTab_, ListMode::kMappings);
    wireTab(unboundTab_, ListMode::kUnbound);

    listBox_.setRowHeight(22);
    addAndMakeVisible(listBox_);

    auto wire = [this](juce::TextButton& button, auto handler)
    {
        addAndMakeVisible(button);
        button.onClick = handler;
    };
    wire(duplicateButton_, [this] { onDuplicate(); });
    wire(deleteButton_, [this] { onDelete(); });
    wire(renameButton_, [this] { onRename(); });
    wire(addPastilleButton_, [this] { onAddPastille(); });
    wire(editPastilleButton_, [this] { onEditPastille(); });
    wire(editMappingButton_, [this] { onEditMapping(); });
    wire(bindUnboundButton_, [this] { onBindUnbound(); });
    wire(pushButton_, [this] { pushActive(); });
    wire(exportButton_, [this] { onExportJson(); });
    wire(importButton_, [this] { onImportJson(); });

    layer_.onChanged = [this]
    {
        refreshAll();
        if (bridge_.isConnected())
            pushActive();
    };

    bridge_.onConnectionChanged = [this](bool)
    {
        refreshStatus();
        if (bridge_.isConnected())
            pushActive();
    };
    bridge_.onApplyResult = [this](const TSS::DevSkin::ApplyResult& result)
    {
        if (result.ok)
            setStatusMessage("Last push: ack (rev " + juce::String(result.revision) + ")");
        else
            setStatusMessage("Last push: error [" + result.code + "] " + result.message);
        refreshStatus();
    };

    bridge_.start();
    refreshAll();
}

SkinEditorMainComponent::~SkinEditorMainComponent()
{
    bridge_.stop();
    listBox_.setModel(nullptr);
}

void SkinEditorMainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(12);
    titleLabel_.setBounds(bounds.removeFromTop(28));
    connectionLabel_.setBounds(bounds.removeFromTop(22));
    lastPushLabel_.setBounds(bounds.removeFromTop(22));
    readOnlyLabel_.setBounds(bounds.removeFromTop(22));
    bounds.removeFromTop(8);

    auto tabs = bounds.removeFromTop(28);
    const int tabW = tabs.getWidth() / 4;
    skinsTab_.setBounds(tabs.removeFromLeft(tabW).reduced(2, 0));
    paletteTab_.setBounds(tabs.removeFromLeft(tabW).reduced(2, 0));
    mappingsTab_.setBounds(tabs.removeFromLeft(tabW).reduced(2, 0));
    unboundTab_.setBounds(tabs.reduced(2, 0));
    bounds.removeFromTop(8);

    auto actions = bounds.removeFromBottom(96);
    auto row1 = actions.removeFromTop(30);
    auto place = [&row1](juce::TextButton& b, int w)
    {
        b.setBounds(row1.removeFromLeft(w).reduced(2, 0));
    };
    place(duplicateButton_, 110);
    place(deleteButton_, 110);
    place(renameButton_, 110);
    place(pushButton_, 80);
    place(exportButton_, 110);
    place(importButton_, 110);

    auto row2 = actions.removeFromTop(30);
    auto place2 = [&row2](juce::TextButton& b, int w)
    {
        b.setBounds(row2.removeFromLeft(w).reduced(2, 0));
    };
    place2(addPastilleButton_, 120);
    place2(editPastilleButton_, 140);
    place2(editMappingButton_, 120);
    place2(bindUnboundButton_, 120);

    listBox_.setBounds(bounds);
}

int SkinEditorMainComponent::getNumRows()
{
    switch (listMode_)
    {
        case ListMode::kSkins:    return static_cast<int>(layer_.skins().size());
        case ListMode::kPalette:
            if (auto* skin = layer_.findSkin(layer_.activeSkinId()))
                return static_cast<int>(skin->palette.size());
            return 0;
        case ListMode::kMappings:
            if (auto* skin = layer_.findSkin(layer_.activeSkinId()))
                return static_cast<int>(skin->mappings.size());
            return 0;
        case ListMode::kUnbound:
            return static_cast<int>(layer_.unboundKeys(layer_.activeSkinId()).size());
    }
    return 0;
}

void SkinEditorMainComponent::paintListBoxItem(int rowNumber, juce::Graphics& g,
                                               int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.drawText(rowText(rowNumber), 4, 0, width - 8, height, juce::Justification::centredLeft);
}

void SkinEditorMainComponent::selectedRowsChanged(int)
{
    if (listMode_ != ListMode::kSkins)
        return;

    const int row = listBox_.getSelectedRow();
    if (row < 0 || row >= static_cast<int>(layer_.skins().size()))
        return;

    layer_.selectSkin(layer_.skins()[static_cast<size_t>(row)].id);
}

juce::String SkinEditorMainComponent::rowText(int row) const
{
    switch (listMode_)
    {
        case ListMode::kSkins:
        {
            const auto& skin = layer_.skins()[static_cast<size_t>(row)];
            juce::String badge = (skin.kind == TSS::DevSkin::SkinKind::kDev) ? "[Dev] "
                                                                            : "[Read-only] ";
            const juce::String active = (skin.id == layer_.activeSkinId()) ? " *" : "";
            return badge + skin.name + active;
        }
        case ListMode::kPalette:
        {
            const auto* skin = layer_.findSkin(layer_.activeSkinId());
            if (skin == nullptr || row >= static_cast<int>(skin->palette.size()))
                return {};
            const auto& pastille = skin->palette[static_cast<size_t>(row)];
            return pastille.name + "  #" + juce::String::toHexString(static_cast<int>(pastille.argb)).paddedLeft('0', 8);
        }
        case ListMode::kMappings:
        {
            const auto* skin = layer_.findSkin(layer_.activeSkinId());
            if (skin == nullptr)
                return {};
            int i = 0;
            for (const auto& [key, name] : skin->mappings)
            {
                if (i == row)
                    return TSS::DevSkin::keyLabel(key) + "  ->  " + name;
                ++i;
            }
            return {};
        }
        case ListMode::kUnbound:
        {
            const auto keys = layer_.unboundKeys(layer_.activeSkinId());
            if (row < 0 || row >= static_cast<int>(keys.size()))
                return {};
            return TSS::DevSkin::keyLabel(keys[static_cast<size_t>(row)]);
        }
    }
    return {};
}

SkinEditorMainComponent::ListMode SkinEditorMainComponent::currentListMode() const
{
    return listMode_;
}

void SkinEditorMainComponent::refreshAll()
{
    listBox_.updateContent();
    listBox_.repaint();
    refreshStatus();

    const bool readOnly = layer_.isActiveReadOnly();
    readOnlyLabel_.setText(readOnly
                               ? "Active skin is compiled Black/Cream (read-only). Duplicate to edit."
                               : "Active skin is a Dev skin (editable).",
                           juce::dontSendNotification);

    deleteButton_.setEnabled(! readOnly);
    renameButton_.setEnabled(! readOnly);
    addPastilleButton_.setEnabled(! readOnly);
    editPastilleButton_.setEnabled(! readOnly);
    editMappingButton_.setEnabled(! readOnly);
    bindUnboundButton_.setEnabled(! readOnly);
}

void SkinEditorMainComponent::refreshStatus()
{
    connectionLabel_.setText(bridge_.session().connectionStatusText(),
                             juce::dontSendNotification);
    lastPushLabel_.setText(statusDetail_.isNotEmpty() ? statusDetail_
                                                      : "Last push: none",
                           juce::dontSendNotification);
}

void SkinEditorMainComponent::setStatusMessage(const juce::String& text)
{
    statusDetail_ = text;
}

void SkinEditorMainComponent::pushActive()
{
    bridge_.push(layer_.makePushPayload());
    if (! bridge_.isConnected())
        setStatusMessage("Last push: queued until Standalone Dev connects");
    refreshStatus();
}

void SkinEditorMainComponent::onDuplicate()
{
    const auto name = askText(this, "Duplicate Skin",
                              "New Dev skin name (prefer future Matrix-Control display name):",
                              "My Skin");
    if (name.isEmpty())
        return;

    const auto result = layer_.duplicateSkin(layer_.activeSkinId(), name);
    if (result.failed())
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Duplicate failed", result.getErrorMessage());
}

void SkinEditorMainComponent::onDelete()
{
    if (layer_.isActiveReadOnly())
        return;
    const auto result = layer_.deleteDevSkin(layer_.activeSkinId());
    if (result.failed())
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Delete failed", result.getErrorMessage());
}

void SkinEditorMainComponent::onRename()
{
    if (layer_.isActiveReadOnly())
        return;
    const auto* skin = layer_.findSkin(layer_.activeSkinId());
    if (skin == nullptr)
        return;
    const auto name = askText(this, "Rename Dev Skin", "New name:", skin->name);
    if (name.isEmpty())
        return;
    const auto result = layer_.renameDevSkin(skin->id, name);
    if (result.failed())
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Rename failed", result.getErrorMessage());
}

void SkinEditorMainComponent::onAddPastille()
{
    if (layer_.isActiveReadOnly())
        return;
    const auto name = askText(this, "Add Pastille", "Pastille name:", "kCustomAccent");
    if (name.isEmpty())
        return;
    const auto hex = askText(this, "Add Pastille", "ARGB hex (e.g. FFFF0000 or #FF0000):", "FFFF0000");
    bool ok = false;
    const auto argb = parseArgb(hex, ok);
    if (! ok)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Invalid colour", "Could not parse ARGB hex");
        return;
    }
    const auto result = layer_.addPastille(layer_.activeSkinId(), name, argb);
    if (result.failed())
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Add pastille failed", result.getErrorMessage());
}

void SkinEditorMainComponent::onEditPastille()
{
    if (layer_.isActiveReadOnly() || listMode_ != ListMode::kPalette)
        return;
    const int row = listBox_.getSelectedRow();
    const auto* skin = layer_.findSkin(layer_.activeSkinId());
    if (skin == nullptr || row < 0 || row >= static_cast<int>(skin->palette.size()))
        return;

    const auto& pastille = skin->palette[static_cast<size_t>(row)];
    const auto hex = askText(this, "Edit Pastille",
                             "ARGB hex for " + pastille.name + ":",
                             juce::String::toHexString(static_cast<int>(pastille.argb)).paddedLeft('0', 8));
    bool ok = false;
    const auto argb = parseArgb(hex, ok);
    if (! ok)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Invalid colour", "Could not parse ARGB hex");
        return;
    }
    const auto result = layer_.setPastilleArgb(skin->id, pastille.name, argb);
    if (result.failed())
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Edit failed", result.getErrorMessage());
}

void SkinEditorMainComponent::onEditMapping()
{
    if (layer_.isActiveReadOnly() || listMode_ != ListMode::kMappings)
        return;
    const int row = listBox_.getSelectedRow();
    const auto* skin = layer_.findSkin(layer_.activeSkinId());
    if (skin == nullptr || row < 0)
        return;

    int i = 0;
    TSS::SkinColourKey key {};
    juce::String current;
    for (const auto& entry : skin->mappings)
    {
        if (i == row)
        {
            key = entry.first;
            current = entry.second;
            break;
        }
        ++i;
    }
    if (current.isEmpty() && i != row)
        return;

    const auto pastille = askText(this, "Set Mapping",
                                  TSS::DevSkin::keyLabel(key) + "\nPastille name:",
                                  current);
    if (pastille.isEmpty())
        return;
    const auto result = layer_.setMapping(skin->id, key, pastille);
    if (result.failed())
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Mapping failed", result.getErrorMessage());
}

void SkinEditorMainComponent::onBindUnbound()
{
    if (layer_.isActiveReadOnly() || listMode_ != ListMode::kUnbound)
        return;
    const int row = listBox_.getSelectedRow();
    const auto keys = layer_.unboundKeys(layer_.activeSkinId());
    if (row < 0 || row >= static_cast<int>(keys.size()))
        return;

    const auto pastille = askText(this, "Bind Unbound",
                                  TSS::DevSkin::keyLabel(keys[static_cast<size_t>(row)])
                                      + "\nPastille name:",
                                  "kGrey500");
    if (pastille.isEmpty())
        return;
    const auto result = layer_.bindUnbound(layer_.activeSkinId(),
                                           keys[static_cast<size_t>(row)],
                                           pastille);
    if (result.failed())
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Bind failed", result.getErrorMessage());
}

void SkinEditorMainComponent::onExportJson()
{
    auto chooser = std::make_shared<juce::FileChooser>(
        "Export Dev layer JSON",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
            .getChildFile("matrix-control-dev-skins.json"),
        "*.json");
    chooser->launchAsync(juce::FileBrowserComponent::saveMode
                             | juce::FileBrowserComponent::canSelectFiles
                             | juce::FileBrowserComponent::warnAboutOverwriting,
                         [this, chooser](const juce::FileChooser& fc)
                         {
                             const auto file = fc.getResult();
                             if (file == juce::File())
                                 return;
                             if (! file.replaceWithText(TSS::DevSkin::exportToJson(layer_)))
                             {
                                 juce::AlertWindow::showMessageBoxAsync(
                                     juce::MessageBoxIconType::WarningIcon,
                                     "Export failed",
                                     "Could not write file");
                                 return;
                             }
                             setStatusMessage("Exported: " + file.getFullPathName());
                             refreshStatus();
                         });
}

void SkinEditorMainComponent::onImportJson()
{
    auto chooser = std::make_shared<juce::FileChooser>(
        "Import Dev layer JSON",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
        "*.json");
    chooser->launchAsync(juce::FileBrowserComponent::openMode
                             | juce::FileBrowserComponent::canSelectFiles,
                         [this, chooser](const juce::FileChooser& fc)
                         {
                             const auto file = fc.getResult();
                             if (file == juce::File())
                                 return;
                             const auto text = file.loadFileAsString();
                             const auto result = TSS::DevSkin::importFromJson(layer_, text);
                             if (! result.ok)
                             {
                                 juce::AlertWindow::showMessageBoxAsync(
                                     juce::MessageBoxIconType::WarningIcon,
                                     "Import refused",
                                     result.message);
                                 return;
                             }
                             setStatusMessage("Imported: " + file.getFullPathName());
                             refreshAll();
                             if (bridge_.isConnected())
                                 pushActive();
                         });
}
