#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinBridgeClient.h"
#include "GUI/Skins/Dev/DevSkinJson.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Dev/DevSkinSession.h"
#include "Tools/SkinEditor/SkinEditorChrome.h"
#include "Tools/SkinEditor/SkinEditorLookAndFeel.h"
#include "Tools/SkinEditor/SkinEditorColourPicker.h"
#include "Tools/SkinEditor/SkinEditorMappingsPanel.h"
#include "Tools/SkinEditor/SkinEditorPalettePanel.h"
#include "Tools/SkinEditor/SkinEditorUnboundPanel.h"

class SkinEditorMainComponent final : public juce::Component,
                                      private juce::ComboBox::Listener,
                                      private juce::Timer
{
public:
    SkinEditorMainComponent();
    ~SkinEditorMainComponent() override;

    /** Returns false if the user cancelled quit. */
    bool tryQuitWithDirtyGate();
    bool isSessionDirty() const noexcept { return sessionDirty_; }

    void paint(juce::Graphics& g) override;
    void resized() override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    void comboBoxChanged(juce::ComboBox*) override;
    void timerCallback() override;

    void refreshAll();
    void refreshCombos();
    void refreshStatus();
    void refreshExportEnabled();
    void pushVisible();
    void setStatusMessage(const juce::String& text);
    void markDirty();
    void markClean();
    void autosaveSession();
    void loadSessionOrBaseline();

    juce::String visibleSkinId() const;
    bool isCompareProductView() const;
    bool canEditEditorSkin() const;

    void onDuplicate();
    void onNew();
    void onDelete();
    void onRename();
    void onCompareToggle();
    void onPush();
    void onExportJson();
    void onImportJson();
    void onAddPastille();
    void onDeletePastille();
    void onColourChanged(juce::Colour colour);
    void onPastilleSelected(const juce::String& name);
    void onUndo();
    void onRedo();

    void syncCompareButtonChrome();
    void tickCompareBlink();

    /** Export current Editor skin; returns true on successful write. */
    bool exportCurrentEditorSkinToFile(const juce::File& file);

    SkinEditorChrome::LookAndFeel lookAndFeel_;

    struct BlinkTimer final : juce::Timer
    {
        std::function<void()> onTick;
        void timerCallback() override
        {
            if (onTick != nullptr)
                onTick();
        }
    };
    BlinkTimer compareBlinkTimer_;
    bool compareBlinkLit_ = true;

    TSS::DevSkin::Layer layer_;
    TSS::DevSkin::BridgeClient bridge_;
    TSS::DevSkin::EditorSessionState sessionState_;
    bool sessionDirty_ = false;
    juce::String selectedPastille_;
    juce::String statusDetail_;
    bool suppressCombo_ = false;

    juce::Label titleLabel_;
    juce::Label connectionLabel_;
    juce::Label lastPushLabel_;
    juce::Label versionLabel_;
    int titleBarSeparatorX_ = -1;
    int titleBarSeparatorY_ = 0;
    int titleBarSeparatorH_ = 0;

    juce::Label skinsTitleLabel_;
    juce::Label productLabel_ { {}, "Product:" };
    juce::ComboBox productCombo_;
    juce::Label editorLabel_ { {}, "Editor:" };
    juce::ComboBox editorCombo_;

    juce::TextButton newButton_ { "New" };
    juce::TextButton duplicateButton_ { "Duplicate" };
    juce::TextButton renameButton_ { "Rename" };
    juce::TextButton deleteButton_ { "Delete" };
    juce::TextButton undoButton_ { "Undo" };
    juce::TextButton redoButton_ { "Redo" };
    juce::TextButton compareButton_ { "Compare" };
    juce::TextButton pushButton_ { "Push" };
    juce::TextButton exportButton_ { "Export" };
    juce::TextButton importButton_ { "Import" };

    SkinEditorColourPicker colourPicker_;
    SkinEditorPalettePanel palettePanel_;
    SkinEditorMappingsPanel mappingsPanel_;
    SkinEditorUnboundPanel unboundPanel_;

    std::vector<int> separatorXs_;
    int separatorY_ = 0;
    int separatorH_ = SkinEditorChrome::kButtonHeight;
};
