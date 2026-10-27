#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinBridgeClient.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Dev/DevSkinSession.h"
#include "Tools/SkinEditor/SkinEditorLookAndFeel.h"
#include "Tools/SkinEditor/SkinEditorColourPicker.h"
#include "Tools/SkinEditor/SkinEditorMappingsPanel.h"
#include "Tools/SkinEditor/SkinEditorPalettePanel.h"
#include "Tools/SkinEditor/SkinEditorSkinsPanel.h"

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
    enum class MappingMode
    {
        kIdle,
        kHeldColour,
        kArmedRow
    };

    // Session (SkinEditorMainComponentSession.cpp)
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
    void applyProductComboChange();
    void applyEditorComboChange();

    // Actions (SkinEditorMainComponentActions.cpp)
    void onDuplicateProduct();
    void onDuplicateEditor();
    void onNew();
    /** Shared Duplicate flow; sourceId is Product or current Editor. */
    void duplicateFromSkin(const juce::String& sourceId);
    void onDelete();
    void onRename();
    void onCompareToggle();
    void onPush();
    void onExportJson();
    void onImportJson();
    void onAddPastille();
    void onDeletePastille();
    void onRenamePastille(const juce::String& pastilleName);
    void onColourChanged(juce::Colour colour);
    void onUndo();
    void onRedo();
    void syncCompareButtonChrome();
    void tickCompareBlink();
    /** Export current Editor skin; returns true on successful write. */
    bool exportCurrentEditorSkinToFile(const juce::File& file);
    /** Suggested Export path using lastSkinJsonDirectory when still valid. */
    juce::File suggestedEditorSkinExportFile() const;
    /** Starting folder for Import (lastSkinJsonDirectory or Documents). */
    juce::File suggestedSkinJsonDirectory() const;
    /** Remember parent of a successful Export/Import and autosave session. */
    void rememberSkinJsonDirectory(const juce::File& file);

    // Mapping modes (SkinEditorMainComponentMapping.cpp)
    void onPastilleSelected(const juce::String& name);
    void onMappingRowActivated(TSS::SkinColourKey key);
    void enterHeldColour(const juce::String& pastilleName);
    void enterArmedRow(TSS::SkinColourKey key, bool fromUnbound);
    void clearMappingModeToIdle();
    void retainMappingModeAfterWrite(TSS::SkinColourKey key, bool fromUnbound);
    void applyMapping(TSS::SkinColourKey key, const juce::String& pastilleName, bool fromUnbound);
    bool isKeyUnboundOnEditor(TSS::SkinColourKey key) const;
    void syncColourPickerToSelection();
    void fillHeldColourWitness();
    void fillArmedRowWitness();
    void updateMappingWitness();

    // Layout / setup (SkinEditorMainComponent.cpp + Skins.cpp)
    void setupTitleBarChrome();
    void setupStatusBarChrome();
    void setupSessionWell();
    void setupOperationWell();
    void setupSkinsPanel();
    void setupBodyPanels();
    void setupLayerAndBridgeCallbacks();
    void layoutTitleBar(juce::Rectangle<int> titleBar);
    void layoutSessionWell(juce::Rectangle<int> titleBar, int titleRight);
    void layoutSessionControls(juce::Rectangle<int> well, juce::Rectangle<int> row);
    void layoutOperationWell(juce::Rectangle<int> band);
    void layoutOperationWitness(juce::Rectangle<int> content);
    void layoutStatusBar(juce::Rectangle<int> statusBar);
    void layoutBodyPanels(juce::Rectangle<int> bounds);
    void paintChromeWells(juce::Graphics& g) const;
    /** Integer row top inside a bar — never half-pixel. */
    static int chromeRowY(int barY, int barH, int rowH, int opticalNudgeY) noexcept;

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
    MappingMode mappingMode_ = MappingMode::kIdle;
    TSS::SkinColourKey armedKey_ {};
    bool armedFromUnbound_ = false;
    juce::String statusDetail_;
    bool suppressCombo_ = false;

    juce::Label titleLabel_;
    juce::Label operationTitleLabel_;
    juce::Label mappingPrefixLabel_;
    juce::Label mappingDetailLabel_;
    juce::Rectangle<int> operationWellBounds_;
    juce::Rectangle<int> operationContentBounds_;
    juce::Rectangle<int> mappingBadgeBounds_;
    juce::Rectangle<int> mappingSwatchBounds_;
    bool mappingWitnessActive_ = false;
    bool mappingWitnessShowSwatch_ = false;
    bool mappingWitnessPending_ = false;
    juce::Colour mappingWitnessColour_;

    juce::Label connectionLabel_;
    juce::Label lastPushLabel_;
    juce::Label versionLabel_;
    /** Invisible hit target so LAST PUSH badge (paint-only) can show the path tooltip. */
    struct StatusTipHitArea final : public juce::Component,
                                    public juce::SettableTooltipClient
    {
        StatusTipHitArea() { setInterceptsMouseClicks(true, false); }
    };
    StatusTipHitArea lastPushBadgeHit_;
    /** Required for Label::setTooltip to show (JUCE does not create one automatically). */
    juce::TooltipWindow tooltipWindow_ { this };
    juce::Rectangle<int> connectionBadgeBounds_;
    juce::Rectangle<int> lastPushBadgeBounds_;
    juce::Rectangle<int> appBadgeBounds_;

    juce::Label sessionTitleLabel_;
    juce::TextButton undoButton_ { "Undo" };
    juce::TextButton redoButton_ { "Redo" };
    juce::TextButton compareButton_ { "Compare" };
    juce::TextButton pushButton_ { "Push" };
    juce::TextButton exportButton_ { "Export" };
    juce::TextButton importButton_ { "Import" };
    juce::Rectangle<int> sessionWellBounds_;
    std::vector<int> sessionSeparatorXs_;
    int sessionSeparatorY_ = 0;
    int sessionSeparatorH_ = 0;

    SkinEditorSkinsPanel skinsPanel_;
    SkinEditorColourPicker colourPicker_;
    SkinEditorPalettePanel palettePanel_;
    SkinEditorMappingsPanel mappingsPanel_;
};
