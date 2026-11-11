#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinBridgeClient.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Dev/DevSkinSession.h"
#include "Tools/SkinDesigner/SkinDesignerLookAndFeel.h"
#include "Tools/SkinDesigner/SkinDesignerColourPicker.h"
#include "Tools/SkinDesigner/SkinDesignerMappingModePolicy.h"
#include "Tools/SkinDesigner/SkinDesignerMappingsPanel.h"
#include "Tools/SkinDesigner/SkinDesignerPalettePanel.h"
#include "Tools/SkinDesigner/SkinDesignerSkinsPanel.h"

class SkinDesignerMainComponent final : public juce::Component,
                                      private juce::ComboBox::Listener,
                                      private juce::Timer
{
public:
    SkinDesignerMainComponent();
    ~SkinDesignerMainComponent() override;

    /** Returns false if the user cancelled quit. */
    bool tryQuitWithDirtyGate();

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseUp(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    enum class MappingMode
    {
        kIdle,
        kHeldColour,
        kArmedRow,
        kEditingColour
    };

    // Session (SkinDesignerMainComponentSession.cpp)
    void comboBoxChanged(juce::ComboBox*) override;
    void timerCallback() override;
    void refreshAll();
    void refreshCombos();
    void refreshStatus();
    void refreshSessionButtonsEnabled();
    void pushVisible();
    void setStatusMessage(const juce::String& text);
    void markDesignerDirty();
    void markDesignerClean();
    void scheduleSessionAutosave();
    void flushPendingSessionAutosave();
    /** Throttle refreshAll + pushVisible during continuous COLOR edits (~40 ms). */
    void scheduleColourPreviewCoalesce();
    void flushColourPreviewCoalesce();
    void tickColourPreviewCoalesce();
    void applyColourPreviewRefresh();
    void syncPastilleSelectionAfterRefresh(const juce::String& panelSkinId);
    void wireColourPicker(const std::function<bool()>& escapeMapping);
    /** Writes session file; returns false and sets status on failure. */
    bool autosaveSession();
    void runWithoutLayerChangeNotify(const std::function<void()>& action);
    /** Blank launch: restores preferences only, never Designer skins. */
    void loadSessionOrBaseline();
    juce::String visibleSkinId() const;
    bool isCompareProductView() const;
    bool canEditDesignerSkin() const;
    void applyProductComboChange();
    void applyDesignerComboChange();

    // Actions (SkinDesignerMainComponentActions.cpp)
    void onDuplicateProduct();
    void onDuplicateDesigner();
    void onNew();
    /** Shared Duplicate flow; sourceId is Product or current Designer. True on success. */
    bool duplicateFromSkin(const juce::String& sourceId);
    /** Point session at layer active Dev skin after create/duplicate; no push (Product Duplicate pushes after). */
    void adoptActiveDesignerSkinAfterCreate();
    void onDelete();
    void onRename();
    void onCompareToggle();
    void onAddPastille();
    void onDuplicatePastille();
    void onDeletePastille();
    void onRenamePastille(const juce::String& pastilleName);
    void onColourChanged(juce::Colour colour);
    void onUndo();
    void onRedo();
    void syncCompareButtonChrome();
    void tickCompareBlink();
    // Files (SkinDesignerMainComponentFiles.cpp)
    /** Load: replace Designer skins from a saved pack (or legacy single-skin file). */
    void onLoadJson();
    void browseForWorkspaceFile();
    void loadWorkspaceFromFile(const juce::File& file);
    /** Save: overwrite the watched path, else first-save chooser. Whole pack, no Unbound gate. */
    void onSave();
    /** Save as: always a chooser; arms the watch on the chosen path. */
    void onSaveAs();
    void browseToSavePack(const char* statusPrefix);
    /** Writes the whole Designer pack (all skins + active skin); marks clean on success. */
    bool savePackToFile(const juce::File& chosen, const char* statusPrefix);
    /** Quit-gate Save: watched path, else a modal chooser. False keeps the app open. */
    bool saveForQuit();
    bool hasAnyDesignerSkin() const;
    /** Chooser start: the watched path, else the default pack name. */
    juce::File suggestedSaveAsFile() const;
    /** Starting folder for Load (lastSkinJsonDirectory or Documents). */
    juce::File suggestedSkinJsonDirectory() const;
    /** Remember parent of a successful Load/Save/Save as and autosave preferences. */
    void rememberSkinJsonDirectory(const juce::File& file);

    // JSON watch (SkinDesignerMainComponentImportWatch.cpp)
    /** Start watching a loaded or saved JSON path for external edits. */
    void armWatchedImportJson(const juce::File& file);
    void clearWatchedImportJson();
    void refreshWatchedImportFingerprint();
    void pollWatchedImportJson();
    /** Branch on JSON Sync mode after a stable external fingerprint change. */
    void handleWatchedImportJsonExternalChange();
    void promptWatchedImportJsonExternalChange();
    void reloadWatchedImportJsonFromDisk();
    void discardWatchedImportJsonExternalChange();
    void syncImportWatchTimerToMode();
    void syncJsonSyncComboFromSession();
    void applyJsonSyncComboChange();
    void applyJsonSyncSessionUi();
    /** Auto JSON reload: replace LAST PUSH strip with dismissible WARNING notice. */
    void showJsonSyncReloadNotice(const juce::String& detail);
    void dismissJsonSyncReloadNotice();
    void layoutStatusPushOrNoticeStrip(int badgeY, int badgeH, int gap, const juce::Font& statusFont);

    // Mapping modes (SkinDesignerMainComponentMapping.cpp)
    void onPastilleSelected(const juce::String& name);
    void onMappingRowActivated(TSS::SkinColourKey key);
    /** Mapped colour cell: select the referenced pastille for COLOR / SV edit. */
    void onMappedColourClicked(TSS::SkinColourKey key);
    /** Pastille name a bound mapping resolves to on the visible skin; empty when unbound or missing. */
    juce::String boundPastilleNameFor(TSS::SkinColourKey key) const;
    /** True when the named pastille exists on the designer skin and is still pending. */
    bool isPastillePending(const juce::String& pastilleName) const;
    /**
     * True when another pastille already mapped on the designer skin shares this
     * pastille's ARGB (own name does not count).
     */
    bool isPastilleSameRgbAlreadyMapped(const juce::String& pastilleName) const;
    void enterHeldColour(const juce::String& pastilleName);
    void enterArmedRow(TSS::SkinColourKey key, bool fromUnbound);
    void enterEditingColour(const juce::String& pastilleName);
    void clearMappingModeToIdle();
    void retainMappingModeAfterWrite(TSS::SkinColourKey key, bool fromUnbound);
    void applyMapping(TSS::SkinColourKey key, const juce::String& pastilleName, bool fromUnbound);
    bool isKeyUnboundOnDesigner(TSS::SkinColourKey key) const;
    SkinDesignerMappingPolicy::Mode policyModeFromMappingMode() const noexcept;
    void syncColourPickerToSelection();
    void fillHeldColourWitness();
    void fillArmedRowWitness();
    void fillEditingColourWitness();
    void fillWitnessSwatchForSelectedPastille();
    void setWitnessTexts(const juce::String& role,
                         const juce::String& identity,
                         const juce::String& nextStep);
    void updateMappingWitness();

    // Layout / setup (SkinDesignerMainComponent.cpp + Skins.cpp)
    void setupTitleBarChrome();
    void setupStatusBarChrome();
    void setupSessionWell();
    void setupOperationWell();
    void setupSkinsPanel();
    void setupBodyPanels();
    void setupLayerAndBridgeCallbacks();
    void layoutTitleBar(juce::Rectangle<int> titleBar);
    void layoutSessionWell(juce::Rectangle<int> well);
    void layoutSessionControls(juce::Rectangle<int> well, juce::Rectangle<int> row);
    void layoutOperationWell(juce::Rectangle<int> well);
    void layoutOperationWitness(juce::Rectangle<int> content);
    /** Plain OPERATION coaching when no designer skin (no mapping badge / arrow). */
    void layoutOperationStartHint(juce::Rectangle<int> content);
    void layoutStatusBar(juce::Rectangle<int> statusBar);
    void layoutBodyPanels(juce::Rectangle<int> bounds);
    void paintChromeWells(juce::Graphics& g) const;
    void paintMappingWitnessDecorations(juce::Graphics& g) const;
    /** Integer row top inside a bar — never half-pixel. */
    static int chromeRowY(int barY, int barH, int rowH, int opticalNudgeY) noexcept;

    SkinDesignerChrome::LookAndFeel lookAndFeel_;

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
    /** Dedicated coalesce for colour drag — not the 400 ms session-autosave timer. */
    BlinkTimer colourPreviewCoalesceTimer_;
    bool colourGestureActive_ = false;
    /** Polls the loaded / saved JSON path for external disk changes (~1 s). */
    BlinkTimer importWatchTimer_;
    juce::File watchedImportJsonFile_;
    juce::String watchedImportJsonFingerprint_;
    /** Candidate fingerprint seen while waiting for a stable external write. */
    juce::String importWatchPendingFingerprint_;
    int importWatchPendingStablePolls_ = 0;
    bool importWatchDialogOpen_ = false;
    /** True while a Load confirm / file chooser is open (no stacked dialogs). */
    bool fileDialogOpen_ = false;

    TSS::DevSkin::Layer layer_;
    TSS::DevSkin::BridgeClient bridge_;
    TSS::DevSkin::DesignerSessionState sessionState_;
    bool designerDirty_ = false;
    juce::String selectedPastille_;
    MappingMode mappingMode_ = MappingMode::kIdle;
    TSS::SkinColourKey armedKey_ {};
    bool armedFromUnbound_ = false;
    juce::String statusDetail_;
    bool suppressCombo_ = false;

    juce::Label operationTitleLabel_;
    juce::Label mappingPrefixLabel_;
    juce::Label mappingNameLabel_;
    juce::Label mappingDetailLabel_;
    /** Full OPERATION witness strings (labels may show an elided copy; no hover tooltips). */
    juce::String mappingWitnessRoleFull_;
    juce::String mappingWitnessIdentityFull_;
    juce::String mappingWitnessDetailFull_;
    juce::Rectangle<int> operationWellBounds_;
    juce::Rectangle<int> operationContentBounds_;
    juce::Rectangle<int> mappingBadgeBounds_;
    juce::Rectangle<int> mappingSwatchBounds_;
    juce::Rectangle<int> mappingArrowBounds_;
    bool mappingWitnessActive_ = false;
    /** Idle OPERATION hint when designerSkinId is empty. */
    bool operationStartHintActive_ = false;
    bool mappingWitnessShowSwatch_ = false;
    /** Selected pastille is still pending (empty round swatch in OPERATION witness). */
    bool mappingWitnessPending_ = false;
    /** Held colour RGB already mapped under a different pastille — red SAME COLOUR badge. */
    bool mappingWitnessSameColourCollision_ = false;
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
    /** Click target for the square close badge on the JSON Sync reload notice. */
    struct StatusCloseHitArea final : public juce::Component
    {
        std::function<void()> onClick;
        std::function<void()> onHoverChanged;
        StatusCloseHitArea()
        {
            setInterceptsMouseClicks(true, false);
            setMouseCursor(juce::MouseCursor::PointingHandCursor);
        }
        bool isHovered() const noexcept { return hovered_; }
        void mouseDown(const juce::MouseEvent&) override
        {
            if (onClick != nullptr)
                onClick();
        }
        void mouseEnter(const juce::MouseEvent&) override { setHovered(true); }
        void mouseExit(const juce::MouseEvent&) override { setHovered(false); }
        void visibilityChanged() override
        {
            if (! isVisible())
                setHovered(false);
        }

    private:
        bool hovered_ = false;
        void setHovered(bool next)
        {
            if (hovered_ == next)
                return;
            hovered_ = next;
            if (onHoverChanged != nullptr)
                onHoverChanged();
        }
    };
    StatusCloseHitArea jsonSyncNoticeCloseHit_;
    /** Required for Label::setTooltip to show (JUCE does not create one automatically). */
    juce::TooltipWindow tooltipWindow_ { this };
    juce::Rectangle<int> connectionBadgeBounds_;
    juce::Rectangle<int> lastPushBadgeBounds_;
    juce::Rectangle<int> jsonSyncNoticeCloseBounds_;
    juce::Rectangle<int> appBadgeBounds_;
    /** When true, status mid-strip shows WARNING + reload time instead of LAST PUSH. */
    bool jsonSyncReloadNoticeVisible_ = false;
    juce::String jsonSyncReloadNoticeText_;
    juce::String statusDetailBeforeJsonSyncNotice_;

    juce::Label sessionTitleLabel_;
    juce::TextButton undoButton_ { "Undo" };
    juce::TextButton redoButton_ { "Redo" };
    juce::TextButton loadButton_ { "Load" };
    juce::TextButton saveButton_ { "Save" };
    juce::TextButton saveAsButton_ { "Save as" };
    juce::Label jsonSyncLabel_;
    juce::ComboBox jsonSyncCombo_;
    juce::Rectangle<int> sessionWellBounds_;
    std::vector<int> sessionSeparatorXs_;
    int sessionSeparatorY_ = 0;
    int sessionSeparatorH_ = 0;

    SkinDesignerSkinsPanel skinsPanel_;
    SkinDesignerColourPicker colourPicker_;
    SkinDesignerPalettePanel palettePanel_;
    SkinDesignerMappingsPanel mappingsPanel_;
};
