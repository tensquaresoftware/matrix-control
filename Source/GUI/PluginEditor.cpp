#include "PluginEditor.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/About/AboutWindow.h"
#include "GUI/Dialogs/BankTransferProgressDialog.h"
#include "GUI/Dialogs/EpromTypePromptDialog.h"
#include "GUI/Dialogs/MasterInitConfirmDialog.h"
#include "GUI/Dialogs/MasterM1kmLoadChoiceDialog.h"
#include "GUI/Dialogs/MutatorHistoryDefragConfirmDialog.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "GUI/Helpers/EditorialUndoRedoShortcuts.h"
#include "GUI/Helpers/EditorChromeShortcuts.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.h"
#include "GUI/Widgets/PatchNameDisplay.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/PatchNameDisplayPanel.h"
#include "GUI/Settings/SettingsPanel.h"
#include "GUI/Settings/SettingsWindow.h"
#include "GUI/Widgets/Slider.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Skins/Skin.h"

namespace
{
    void cancelActiveSliderDragSessions(juce::Component& root)
    {
        if (auto* slider = dynamic_cast<TSS::Slider*>(&root))
            slider->cancelActiveDragSession();

        for (int i = 0; i < root.getNumChildComponents(); ++i)
        {
            if (auto* child = root.getChildComponent(i))
                cancelActiveSliderDragSessions(*child);
        }
    }

    template <typename T>
    void updateOverlayBoundsIfVisible(T& overlay, juce::Rectangle<int> bounds)
    {
        if (overlay != nullptr && overlay->isVisible())
            overlay->setBounds(bounds);
    }

    bool enforceStandaloneFixedSize(PluginEditor& editor,
                                    const GuiLayoutDimensions& layoutDimensions,
                                    float appliedUiScale)
    {
        const int targetWidth = juce::roundToInt(
            static_cast<float>(layoutDimensions.editor.width) * appliedUiScale);
        const int targetHeight = juce::roundToInt(
            static_cast<float>(layoutDimensions.editor.height) * appliedUiScale);

        editor.setResizeLimits(targetWidth, targetHeight, targetWidth, targetHeight);

        if (editor.getWidth() != targetWidth || editor.getHeight() != targetHeight)
        {
            editor.setSize(targetWidth, targetHeight);
            return true;
        }

        return false;
    }
}

using TSS::SkinColourId;

TSS::ISkin& PluginEditor::getActiveSkin() noexcept
{
    jassert(skin_ != nullptr);
    return *skin_;
}

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p)
    , pluginProcessor(p)
{
    // Native alerts remain available for any residual OS paths; product confirms use Matrix chrome.
    juce::LookAndFeel::getDefaultLookAndFeel().setUsingNativeAlertWindows(true);

    wirePatchAndMutatorBindings();
    wireBankTransferBindings();
    createUiShell();
    setMutatorPanelDefragRecoveryBinding();
    restoreAndWireHeader();
    attachEditorRuntimeListeners();

    if (pluginProcessor.isStandalone())
    {
        // Silence when AUDIO FROM is None comes from passthrough, not JUCE muteInput banner.
        Core::StandaloneAudioInputRouter::enableInputMonitoring();
    }
}

PluginEditor::~PluginEditor()
{
    removeKeyListener(this);
    pluginProcessor.setMutatorDefragLimitModalGate({});
    pluginProcessor.setMutatorExportCollisionModalGate({});
    pluginProcessor.setMutatorHistoryGateModalGate({});
    pluginProcessor.setUnsavedEditConfirmModalGate({});
    pluginProcessor.setM1kpSiblingSyxOverwriteConfirmGate({});
    pluginProcessor.setNameRequiredBeforeStoreRequest({});
    pluginProcessor.setMutatorFlushConfirmModalGate({});
    pluginProcessor.setMutatorDeleteConfirmModalGate({});
    pluginProcessor.setPatchNameReconciliationPicker({});
    pluginProcessor.setBankExportFolderPicker({});
    pluginProcessor.setBankImportFolderPicker({});
    pluginProcessor.setBankImportConfirmGate({});
    pluginProcessor.setBankExportOverwriteConfirmGate({});
    pluginProcessor.setBankPasteConfirmGate({});
    pluginProcessor.setBankTransferProgressPresenter({});

    pluginProcessor.getApvts().state.removeListener(this);
    detachStandaloneAudioDeviceListener();
    closeSettingsWindow();
    closeAboutWindow();
}

void PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kHeaderPanelBackground));
}

void PluginEditor::resized()
{
    const int baseWidth = layoutDimensions_.editor.width;
    if (baseWidth <= 0)
        return;

    if (pluginProcessor.isStandalone()
        && enforceStandaloneFixedSize(*this, layoutDimensions_, appliedUiScale_))
    {
        return;
    }

    if (auto* comp = mainComponent_.get())
        comp->setBounds(getLocalBounds());

    const auto bounds = getLocalBounds();
    updateOverlayBoundsIfVisible(settingsWindow_, bounds);
    updateOverlayBoundsIfVisible(aboutWindow_, bounds);

#if JUCE_DEBUG
    layoutUiElementsTestComponent();
#endif
    syncUiScaleFromEditor();

    if (pluginProcessor.isStandalone())
        syncStandaloneWindowSize();
}

void PluginEditor::visibilityChanged()
{
    juce::AudioProcessorEditor::visibilityChanged();

    // Peers often become showing after attachEditorRuntimeListeners' first deferred
    // focus request (Standalone DocumentWindow; some hosts delay editor show). That
    // first grab can no-op and Cmd/Ctrl shortcuts beep until a content click.
    // Each visibilityChanged while showing reschedules the helper (Standalone +
    // hosted) as the follow-up when an earlier async ran while !isShowing(); the
    // helper no-ops when focus is already owned — not an infinite retry loop.
    if (isShowing())
        requestEditorKeyboardFocusIfNeeded();
}

void PluginEditor::requestEditorKeyboardFocusIfNeeded()
{
    juce::MessageManager::callAsync([safeThis = juce::Component::SafePointer<PluginEditor>(this)]
                                    {
                                        if (safeThis == nullptr || ! safeThis->isShowing())
                                            return;

                                        // Visible overlays may not own focus yet (first-run Device Setup).
                                        if (safeThis->isEscapeBlockedByOverlay())
                                            return;

                                        // Do not steal focus from a child that already owns it.
                                        if (! safeThis->hasKeyboardFocus(true))
                                            safeThis->grabKeyboardFocus();
                                    });
}

void PluginEditor::mouseDown(const juce::MouseEvent& event)
{
#if JUCE_DEBUG
    if (uiElementsTestVisible_
        && testComponent_ != nullptr
        && testComponent_->isVisible()
        && testComponent_->getBounds().contains(event.getPosition()))
    {
        testComponent_->grabKeyboardFocus();
        return;
    }
#else
    juce::ignoreUnused(event);
#endif

    unfocusAllComponents();
    grabKeyboardFocus();
}

bool PluginEditor::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        if (isEscapeBlockedByOverlay())
            return false;

        if (! pluginProcessor.clearClipboardFeedbackFromEscape())
            return false;

        return true;
    }

    if (tryHandleEditorialUndoRedoKey(key))
        return true;

    if (tryHandleEditorChromeKey(key))
        return true;

#if JUCE_DEBUG
    if (uiElementsTestVisible_
        && testComponent_ != nullptr
        && testComponent_->isVisible()
        && testComponent_->keyPressed(key))
    {
        return true;
    }
#endif

    return juce::AudioProcessorEditor::keyPressed(key);
}

bool PluginEditor::isEditorialUndoBlockedByModalOverlay() const
{
    const auto visible = [](const auto& component)
    {
        return component != nullptr && component->isVisible();
    };

    return visible(aboutWindow_)
        || visible(masterInitConfirmDialog_)
        || visible(masterM1kmLoadChoiceDialog_)
        || visible(mutatorHistoryDefragConfirmDialog_)
        || visible(epromTypePromptDialog_)
        || visible(bankTransferProgressDialog_);
}

bool PluginEditor::isEditorialUndoBlockedByTextFocus() const
{
    if (mainComponent_ == nullptr)
        return false;

    auto& patchNameDisplay = mainComponent_->getBodyPanel()
                                 .getPatchEditPanel()
                                 .getPatchEditDisplaysPanel()
                                 .getPatchNameDisplayPanel()
                                 .getPatchNameDisplay();

    if (patchNameDisplay.isEditing())
        return true;

    if (auto* focused = juce::Component::getCurrentlyFocusedComponent())
    {
        for (auto* component = focused; component != nullptr; component = component->getParentComponent())
        {
            if (dynamic_cast<const juce::TextEditor*>(component) != nullptr)
                return true;
        }
    }

    return false;
}

void PluginEditor::prepareEditorialUndoRedo()
{
    if (mainComponent_ != nullptr)
    {
        mainComponent_->getBodyPanel()
            .getPatchEditPanel()
            .getPatchEditDisplaysPanel()
            .endActiveEditGestures();

        cancelActiveSliderDragSessions(*mainComponent_);
    }
}

bool PluginEditor::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent)
{
    juce::ignoreUnused(originatingComponent);
    return tryHandleEditorialUndoRedoKey(key) || tryHandleEditorChromeKey(key);
}

bool PluginEditor::tryHandleEditorChromeKey(const juce::KeyPress& key)
{
    if (const int skinVariantId = TSS::classifySkinVariantShortcut(key); skinVariantId != 0)
    {
        if (isEditorialUndoBlockedByTextFocus())
            return false;

        applySkinFromItemId(skinVariantId, true);
        return true;
    }

    const auto shortcut = TSS::classifyEditorChromeShortcut(key);
    if (shortcut == TSS::EditorChromeShortcut::kNone)
        return false;

    if (isEditorialUndoBlockedByTextFocus())
        return false;

    return performEditorChromeShortcut(shortcut);
}

bool PluginEditor::performEditorChromeShortcut(TSS::EditorChromeShortcut shortcut)
{
    using namespace PluginIDs::Settings::ScaleLevels;

    switch (shortcut)
    {
        case TSS::EditorChromeShortcut::kOpenSettings:
            openSettingsWindow();
            return true;

        case TSS::EditorChromeShortcut::kUiScaleIncrease:
        {
            const int scaleId = pluginProcessor.getGuiScaleId();
            if (scaleId < kMax)
                applyUiScaleFromItemId(scaleId + 1, true);
            return true;
        }

        case TSS::EditorChromeShortcut::kUiScaleDecrease:
        {
            const int scaleId = pluginProcessor.getGuiScaleId();
            if (scaleId > kMin)
                applyUiScaleFromItemId(scaleId - 1, true);
            return true;
        }

        case TSS::EditorChromeShortcut::kUiScaleReset:
            applyUiScaleFromItemId(k100, true);
            return true;

        case TSS::EditorChromeShortcut::kNone:
            break;
    }

    return false;
}

bool PluginEditor::tryHandleEditorialUndoRedoKey(const juce::KeyPress& key)
{
    const auto shortcut = TSS::classifyEditorialUndoRedoShortcut(key);
    if (shortcut == TSS::EditorialUndoRedoShortcut::kNone)
        return false;

    if (isEditorialUndoBlockedByTextFocus())
        return false;

    if (isEditorialUndoBlockedByModalOverlay())
        return true;

    if (! pluginProcessor.isEditorialUndoRedoEnabled())
        return true;

    const bool isRedo = shortcut == TSS::EditorialUndoRedoShortcut::kRedo;

    if (isRedo ? ! pluginProcessor.canPerformEditorialRedo()
               : ! pluginProcessor.canPerformEditorialUndo())
        return true;

    prepareEditorialUndoRedo();

    const bool performed = isRedo ? pluginProcessor.performEditorialRedo()
                                  : pluginProcessor.performEditorialUndo();

    juce::ignoreUnused(performed);

    return true;
}
