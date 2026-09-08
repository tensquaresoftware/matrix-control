#pragma once

#include <functional>
#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Core/PluginProcessor.h"
#include "MainComponent.h"
#include "GUI/Dialogs/BankTransferProgressDialog.h"
#include "GUI/Layout/PanelDimensions.h"
#include "Skins/Skin.h"
#include "Core/MIDI/Queue/RealtimeQueuePressureMonitor.h"

#if JUCE_DEBUG
#include "Tests/TestComponent.h"
#endif

class WidgetFactory;
class HeaderPanel;
class SettingsPanel;
class SettingsWindow;
class AboutWindow;
class MasterInitConfirmDialog;
class PatchNameDisplayPanel;

class PluginEditor : public juce::AudioProcessorEditor,
                     public juce::FileDragAndDropTarget,
                     private juce::KeyListener,
                     private juce::ChangeListener,
                     private juce::ValueTree::Listener
{
public:
    explicit PluginEditor(PluginProcessor&);
    ~PluginEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;
    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void fileDragEnter(const juce::StringArray& files, int x, int y) override;
    void fileDragMove(const juce::StringArray& files, int x, int y) override;
    void fileDragExit(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    /** Refresh header MIDI port combos after an OS MIDI device-list change. */
    void refreshMidiPortListsFromOsChange();

private:
    // Nested runtime timers; bodies live in PluginEditorTimers.cpp.
    class HeaderRefreshTimer : private juce::Timer
    {
    public:
        HeaderRefreshTimer(PluginProcessor& processor, HeaderPanel& headerPanel, PluginEditor& owner);

    private:
        void timerCallback() override;

        PluginProcessor& processor_;
        HeaderPanel& headerPanel_;
        PluginEditor& owner_;
        Core::RealtimeQueuePressureMonitor queuePressureMonitor_;
        int audioFromRefreshAttempts_ = 0;
    };

    class ClipboardFeedbackPhaseTimer : private juce::Timer,
                                        private juce::ValueTree::Listener
    {
    public:
        explicit ClipboardFeedbackPhaseTimer(juce::AudioProcessorValueTreeState& apvts);
        ~ClipboardFeedbackPhaseTimer() override;

    private:
        void timerCallback() override;
        void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property) override;
        void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
        void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
        void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
        void valueTreeParentChanged(juce::ValueTree&) override {}
        void valueTreeRedirected(juce::ValueTree&) override;
        void syncTimerFromState();

        juce::ValueTree state_;
    };

    // Bank Utility EXPORT/IMPORT/COPY/PASTE progress modal request (see BankTransferProgressPresenter).
    struct BankTransferProgressShowRequest
    {
        juce::String title;
        juce::String message;
        juce::String detail;
        int totalSteps = 0;
        std::function<void()> onCancelRequested;
        BankTransferProgressDialog::ContentLayout layout = BankTransferProgressDialog::ContentLayout::DualLane;
    };

    // ---- Construction phases (see PluginEditor.cpp ctor for call order) ----
    void wirePatchAndMutatorBindings();
    void wireBankTransferBindings();
    void createUiShell();
    void wirePatchEditDisplayBindings();
    void restoreAndWireHeader();
    void attachEditorRuntimeListeners();

    PatchNameDisplayPanel* getPatchNameDisplayPanelIfPresent();
    void updatePatchNameDragOverlay(const juce::StringArray& files);
    void clearPatchNameDragOverlay();
    void handleSyxFilesDropped(const juce::StringArray& files);

    // wirePatchAndMutatorBindings() sub-bindings (PluginEditorPatchBindings.cpp).
    void setPatchFolderPickerBinding();
    void setMutatorExportFolderPickerBinding();
    void setMutatorDefragLimitGateBinding();
    void setMutatorExportCollisionGateBinding();
    void setMutatorHistoryGateBinding();
    void setUnsavedEditConfirmGateBinding();
    void setMutatorFlushConfirmGateBinding();
    void setMutatorDeleteConfirmGateBinding();
    void setPatchSaveFilePickerBinding();
    void setPatchNameReconciliationPickerBinding();

    // wireBankTransferBindings() sub-bindings (PluginEditorBankBindings.cpp).
    void setBankExportFolderPickerBinding();
    void setBankImportFolderPickerBinding();
    void setBankImportConfirmGateBinding();
    void setBankExportOverwriteConfirmGateBinding();
    void setBankPasteConfirmGateBinding();
    void wireBankTransferProgressPresenter();
    void configureBankTransferProgressShowAndUpdate(Core::BankTransferProgressPresenter& presenter);
    void configureBankTransferProgressMessaging(Core::BankTransferProgressPresenter& presenter);

    // attachEditorRuntimeListeners() sub-binding (PluginEditorUiConstruction.cpp).
    void wireHeaderRuntimeControls(HeaderPanel& headerPanel);

    void refreshAudioFromCombo(HeaderPanel* headerOverride = nullptr);
    void applyAudioCatalogToHeader(HeaderPanel& header,
                                   const juce::StringArray& names,
                                   const juce::StringArray& ids,
                                   juce::String sourceIdToRestore);
    void applyAudioCatalogWithoutHeader(const juce::StringArray& ids, juce::String sourceIdToRestore);
    void attachStandaloneAudioDeviceListener();
    void detachStandaloneAudioDeviceListener();
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void syncMidiPortSelectionFromState(const juce::String& propertyName);
    void scheduleAudioFromRefreshIfNeeded(const juce::String& propertyName);
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    void valueTreeRedirected(juce::ValueTree&) override;

    bool isEscapeBlockedByOverlay() const;
    bool isEditorialUndoBlockedByModalOverlay() const;
    bool isEditorialUndoBlockedByTextFocus() const;
    void prepareEditorialUndoRedo();
    bool tryHandleEditorialUndoRedoKey(const juce::KeyPress& key);

    void openSettingsWindow();
    void closeSettingsWindow();
    void openAboutWindow();
    void closeAboutWindow();
    void openMasterInitConfirmDialog(const juce::String& moduleDisplayName, std::function<void()> onConfirm);
    void openMasterGlobalInitConfirmDialog(std::function<void()> onConfirm);
    void closeMasterInitConfirmDialog();
    void showBankTransferProgressDialog(const BankTransferProgressShowRequest& request);
    void hideBankTransferProgressDialog();
    SettingsPanel* getSettingsPanelIfOpen();
    void wireSettingsPanel(SettingsPanel& panel);
    void wireSettingsInitAndMasterActions(SettingsPanel& panel);
    void wireSettingsMasterFileActions(SettingsPanel& panel);
    void refreshInitTemplateDeleteButtons(SettingsPanel& panel);
    bool confirmDeleteInitTemplate(const juce::String& bodyMessage);
    void wireHeaderPanel(HeaderPanel& headerPanel);
    void wireHeaderEditorialUndoRedoButtons(HeaderPanel& headerPanel);
    void restoreSettingsPanelFromState(SettingsPanel& panel);
    void restoreHeaderPanelFromState(HeaderPanel& headerPanel);
    void updateSettingsWindowLayout(float uiScale);
    void updateAboutWindowLayout(float uiScale);
    void updateMasterInitConfirmDialogLayout(float uiScale);
    void updateBankTransferProgressDialogLayout(float uiScale);

    void applySkinFromItemId(int skinItemId, bool persistToState = true);
    void applyUiScaleFromItemId(int scaleId, bool persistToState = true);

    PluginProcessor& pluginProcessor;

    std::unique_ptr<TSS::Skin> skinBlack_;
    std::unique_ptr<TSS::Skin> skinCream_;
    TSS::Skin* skin_ = nullptr;
    std::unique_ptr<WidgetFactory> widgetFactory_;
    GuiLayoutDimensions layoutDimensions_;
    std::unique_ptr<MainComponent> mainComponent_;
#if JUCE_DEBUG
    std::unique_ptr<TestComponent> testComponent_;
    bool uiElementsTestVisible_ = false;
#endif
    std::unique_ptr<SettingsWindow> settingsWindow_;
    std::unique_ptr<AboutWindow> aboutWindow_;
    std::unique_ptr<MasterInitConfirmDialog> masterInitConfirmDialog_;
    std::unique_ptr<BankTransferProgressDialog> bankTransferProgressDialog_;
    float appliedUiScale_ = 1.0f;
    std::unique_ptr<HeaderRefreshTimer> headerRefreshTimer_;
    std::unique_ptr<ClipboardFeedbackPhaseTimer> clipboardFeedbackPhaseTimer_;
    juce::String lastDragAssessedPath_;
    bool lastDragAssessedValid_ = false;
    juce::String lastDragAssessedPreview_;

    void updateSkin();
    void applyUiScale(float uiScale);
    void syncUiScaleFromEditor();
    void updateOverlayLayoutsForUiScale(float uiScale);
    void syncStandaloneWindowSize();
#if JUCE_DEBUG
    void createUiElementsTestComponent();
    void setUiElementsTestVisible(bool visible);
    void layoutUiElementsTestComponent();
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
