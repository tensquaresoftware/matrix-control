#include "PluginEditor.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/About/AboutWindow.h"
#include "GUI/Dialogs/BankTransferProgressDialog.h"
#include "GUI/Dialogs/MasterInitConfirmDialog.h"
#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Helpers/EditorialUndoRedoShortcuts.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.h"
#include "GUI/Widgets/PatchNameDisplay.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/PatchNameDisplayPanel.h"
#include "GUI/Settings/SettingsPanel.h"
#include "GUI/Settings/SettingsWindow.h"
#include "GUI/Widgets/Slider.h"
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
}

using TSS::SkinColourId;

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p)
    , pluginProcessor(p)
{
    // Prefer OS-native alerts where LookAndFeel reports them (macOS/Windows).
    // On Linux, JUCE forces isUsingNativeAlertWindows() false and draws AlertWindow.
    juce::LookAndFeel::getDefaultLookAndFeel().setUsingNativeAlertWindows(true);

    wirePatchAndMutatorBindings();
    wireBankTransferBindings();
    createUiShell();
    restoreAndWireHeader();
    attachEditorRuntimeListeners();
}

PluginEditor::~PluginEditor()
{
    removeKeyListener(this);
    pluginProcessor.setMutatorDefragLimitModalGate({});
    pluginProcessor.setMutatorExportCollisionModalGate({});
    pluginProcessor.setMutatorHistoryGateModalGate({});
    pluginProcessor.setUnsavedEditConfirmModalGate({});
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

    if (pluginProcessor.isStandalone())
    {
        const int targetWidth = juce::roundToInt(static_cast<float>(layoutDimensions_.editor.width) * appliedUiScale_);
        const int targetHeight = juce::roundToInt(static_cast<float>(layoutDimensions_.editor.height) * appliedUiScale_);

        setResizeLimits(targetWidth, targetHeight, targetWidth, targetHeight);

        if (getWidth() != targetWidth || getHeight() != targetHeight)
        {
            setSize(targetWidth, targetHeight);
            return;
        }
    }

    if (auto* comp = mainComponent_.get())
        comp->setBounds(getLocalBounds());

    if (settingsWindow_ != nullptr && settingsWindow_->isVisible())
        settingsWindow_->setBounds(getLocalBounds());

    if (aboutWindow_ != nullptr && aboutWindow_->isVisible())
        aboutWindow_->setBounds(getLocalBounds());

#if JUCE_DEBUG
    layoutUiElementsTestComponent();
#endif
    syncUiScaleFromEditor();

    if (pluginProcessor.isStandalone())
        syncStandaloneWindowSize();
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
    if (aboutWindow_ != nullptr && aboutWindow_->isVisible())
        return true;
    if (masterInitConfirmDialog_ != nullptr && masterInitConfirmDialog_->isVisible())
        return true;
    if (bankTransferProgressDialog_ != nullptr && bankTransferProgressDialog_->isVisible())
        return true;

    return false;
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
    return tryHandleEditorialUndoRedoKey(key);
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
