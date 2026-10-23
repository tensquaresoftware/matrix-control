#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinBridgeClient.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"

class SkinEditorMainComponent final : public juce::Component,
                                      private juce::ListBoxModel
{
public:
    SkinEditorMainComponent();
    ~SkinEditorMainComponent() override;

    void resized() override;

private:
    enum class ListMode
    {
        kSkins,
        kPalette,
        kMappings,
        kUnbound
    };

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;

    void refreshAll();
    void refreshStatus();
    void pushActive();
    void setStatusMessage(const juce::String& text);

    void onDuplicate();
    void onDelete();
    void onRename();
    void onAddPastille();
    void onEditPastille();
    void onEditMapping();
    void onBindUnbound();
    void onExportJson();
    void onImportJson();

    juce::String rowText(int row) const;
    ListMode currentListMode() const;

    TSS::DevSkin::Layer layer_;
    TSS::DevSkin::BridgeClient bridge_;

    juce::Label titleLabel_;
    juce::Label connectionLabel_;
    juce::Label lastPushLabel_;
    juce::Label readOnlyLabel_;

    juce::TextButton skinsTab_ { "Skins" };
    juce::TextButton paletteTab_ { "Palette" };
    juce::TextButton mappingsTab_ { "Mappings" };
    juce::TextButton unboundTab_ { "Unbound" };

    juce::ListBox listBox_ { "SkinEditorList", this };

    juce::TextButton duplicateButton_ { "Duplicate" };
    juce::TextButton deleteButton_ { "Delete Dev" };
    juce::TextButton renameButton_ { "Rename Dev" };
    juce::TextButton addPastilleButton_ { "Add Pastille" };
    juce::TextButton editPastilleButton_ { "Edit Pastille Hex" };
    juce::TextButton editMappingButton_ { "Set Mapping" };
    juce::TextButton bindUnboundButton_ { "Bind Unbound" };
    juce::TextButton pushButton_ { "Push" };
    juce::TextButton exportButton_ { "Export JSON" };
    juce::TextButton importButton_ { "Import JSON" };

    ListMode listMode_ = ListMode::kSkins;
    juce::String statusDetail_;
};
