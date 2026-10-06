// Extracted from PluginEditor.cpp for modular maintenance.
// Construction phases: main UI shell creation, header restore/wiring, runtime listener attachment.

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "Core/Services/DeviceConnectionMachineDefaults.h"
#include "Core/Services/DeviceSetupDeviceRow.h"
#include "Core/Services/PatchNameEditRules.h"
#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/PatchNameDisplayPanel.h"
#include "GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "GUI/Settings/SettingsAudioPage.h"
#include "GUI/Widgets/ComboBox.h"
#include "Shared/Definitions/PluginAudioConstants.h"
#include "Shared/Definitions/PluginIDs.h"

void PluginEditor::createUiShell()
{
    skinBlack_ = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
    skinCream_ = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);

    const int savedSkinId = pluginProcessor.getSkinVariantId();
    skin_ = (savedSkinId == PluginIDs::Settings::SkinVariants::kCream)
        ? skinCream_.get()
        : skinBlack_.get();

    widgetFactory_ = std::make_unique<WidgetFactory>(pluginProcessor.getApvts());
    layoutDimensions_ = WidgetFactory::buildGuiLayoutDimensions();
    TSS::ComboBox::setPopupLayoutDimensions(layoutDimensions_.popupMenu);

    setOpaque(true);
    setWantsKeyboardFocus(false);
    setInterceptsMouseClicks(true, true);

    mainComponent_ = std::make_unique<MainComponent>(MainComponentConstructionArgs{
        *skin_,
        layoutDimensions_,
        *widgetFactory_,
        pluginProcessor.getApvts(),
        pluginProcessor.getPatchFileService()});
    addAndMakeVisible(*mainComponent_);
    mainComponent_->attachLockDimmingFilm(pluginProcessor.getApvts());

    mainComponent_->setBusReorderHandler(
        [this](int fromBus, int toBus)
        {
            pluginProcessor.swapMatrixModBusContents(fromBus, toBus);
        });

    wirePatchEditDisplayBindings();

    mainComponent_->setMasterInitConfirmationGate(
        [this](const juce::String& /*initPropertyId*/,
               const juce::String& moduleDisplayName,
               std::function<void()> onConfirmed)
        {
            openMasterInitConfirmDialog(moduleDisplayName, std::move(onConfirmed));
        });

#if JUCE_DEBUG
    createUiElementsTestComponent();
#endif

    updateSkin();
}

void PluginEditor::wirePatchEditDisplayBindings()
{
    auto& displaysPanel = mainComponent_->getBodyPanel()
        .getPatchEditPanel()
        .getPatchEditDisplaysPanel();
    auto& patchNameDisplayPanel = displaysPanel.getPatchNameDisplayPanel();

    patchNameDisplayPanel.setCanEditProvider(
        [this]() { return pluginProcessor.canEditPatchName(); });

    patchNameDisplayPanel.setRenameCommitHandler(
        [this](const juce::String& newName)
        {
            // Pending STORE: commit name locally only — STORE sendPatch is the one device write.
            pluginProcessor.commitPatchNameRename(
                newName,
                Core::PatchNameEditRules::shouldSuppressRenameAuditionForPendingStore(
                    pendingInternalStore_));
        });

    patchNameDisplayPanel.setNameRequiredOutcomeHandler(
        [this](bool success)
        {
            if (! pendingInternalStore_)
                return;

            const bool completeStore =
                Core::PatchNameEditRules::shouldCompletePendingStore(pendingInternalStore_, success);
            pendingInternalStore_ = false;

            if (completeStore)
                pluginProcessor.executeInternalPatchStore();
        });

    // Core STORE gate → arm name-required. Set pending first so a cancel-during-rearm
    // outcome cannot clear the new pending STORE before arm completes.
    pluginProcessor.setNameRequiredBeforeStoreRequest(
        [this]()
        {
            auto* panel = getPatchNameDisplayPanelIfPresent();
            if (panel == nullptr)
                return;

            // Set pending only when name-required can actually arm (avoids orphan pending).
            pendingInternalStore_ = true;
            // Re-click while armed: armNameRequired is idempotent while editing; pending stays true.
            panel->armNameRequired();
        });

    displaysPanel.setBeginEditorialTransaction(
        [this](const juce::String& name)
        {
            pluginProcessor.beginEditorialTransaction(name);
        });
}

#if JUCE_DEBUG
void PluginEditor::createUiElementsTestComponent()
{
    testComponent_ = std::make_unique<TestComponent>(TestComponent::Options{
        *skin_,
        pluginProcessor.getApvts(),
        pluginProcessor.getApvts().state,
        layoutDimensions_.editor.width,
        layoutDimensions_.editor.height});
    addChildComponent(*testComponent_);
    testComponent_->setVisible(false);
}
#endif

void PluginEditor::restoreAndWireHeader()
{
    auto& headerPanel = mainComponent_->getHeaderPanel();

    const int savedScaleId = pluginProcessor.getGuiScaleId();
    const float savedUiScale = PluginIDs::Settings::ScaleLevels::getUiScale(savedScaleId);
    applyUiScale(savedUiScale);

    restoreHeaderPanelFromState(headerPanel);
    wireHeaderPanel(headerPanel);

    headerPanel.setPluginMode(!pluginProcessor.isStandalone());

    pluginProcessor.restoreMidiPortsForHost();
    syncPanicFromMidiOutputState();

    if (pluginProcessor.isStandalone())
    {
        const auto savedKeyboardFromPortId = pluginProcessor.getApvts().state.getProperty("keyboardFromPortId", juce::String()).toString();
        if (savedKeyboardFromPortId.isNotEmpty()
            && ! pluginProcessor.setKeyboardFromPort(savedKeyboardFromPortId))
        {
            // Open failure or MIDI From conflict — drop the dead selection.
            pluginProcessor.setKeyboardFromPort({});
        }

        // First-run criterion C: Input None + AUDIO FROM empty once, then set flag.
        if (Core::StandaloneAudioInputRouter::applySceneAudioSafetyDefaultsIfNeeded())
            pluginProcessor.setAudioFromSourceId({});

        refreshAudioFromCombo();

        const float savedInputGainDb = static_cast<float>(
            pluginProcessor.getApvts().state.getProperty("inputGainDb", 0.0f));
        const int gainIndex = PluginAudioConstants::inputGainDbToIndex(savedInputGainDb);
        headerPanel.getInputGainSlider().setValue(gainIndex, juce::dontSendNotification);
        pluginProcessor.setInputGainDb(PluginAudioConstants::inputGainIndexToDb(gainIndex));
    }

    pluginProcessor.syncHardwareLatencyFromState();

    setResizable(false, false);
}

void PluginEditor::wireHeaderRuntimeControls(HeaderPanel& headerPanel)
{
    headerPanel.getInputGainSlider().onValueChange = [this, &headerPanel]
    {
        if (!pluginProcessor.isStandalone())
            return;

        const int index = static_cast<int>(std::round(headerPanel.getInputGainSlider().getValue()));
        pluginProcessor.setInputGainDb(PluginAudioConstants::inputGainIndexToDb(index));
    };
}

void PluginEditor::wireSynthFromComboChange(SettingsAudioPage& audioPage)
{
    audioPage.getSynthFromCombo().onChange = [this, &audioPage]
    {
        if (!pluginProcessor.isStandalone())
            return;

        const auto sourceId = audioPage.getSelectedSynthFromSourceId();
        pluginProcessor.setAudioFromSourceId(sourceId);

        if (sourceId.isNotEmpty())
        {
            pluginProcessor.bindAudioFromInputDeviceIdentity(
                Core::StandaloneAudioInputRouter::getCurrentInputDeviceName());
        }
    };
}

void PluginEditor::attachEditorRuntimeListeners()
{
    auto& headerPanel = mainComponent_->getHeaderPanel();

    wireHeaderRuntimeControls(headerPanel);

    headerRefreshTimer_ = std::make_unique<HeaderRefreshTimer>(pluginProcessor, headerPanel, *this);
    clipboardFeedbackPhaseTimer_ = std::make_unique<ClipboardFeedbackPhaseTimer>(pluginProcessor.getApvts());
    attachStandaloneAudioDeviceListener();
    pluginProcessor.getApvts().state.addListener(this);

    if (Core::shouldOpenDeviceSetupAssistant(
            static_cast<bool>(pluginProcessor.getApvts().state.getProperty(
                PluginIDs::Settings::kEpromTypePromptDone, false)),
            false,
            Core::DeviceConnectionMachineDefaults::load().promptDone))
    {
        juce::MessageManager::callAsync(
            [safeThis = juce::Component::SafePointer<PluginEditor>(this)]
            {
                if (safeThis != nullptr)
                    safeThis->openEpromTypePromptDialog();
            });
    }

    setWantsKeyboardFocus(true);
    setFocusContainerType(juce::Component::FocusContainerType::keyboardFocusContainer);
    addKeyListener(this);
    mainComponent_->addKeyListener(this);
    mainComponent_->setEditorialUndoRedoKeyHandler(
        [this](const juce::KeyPress& key)
        {
            return tryHandleEditorialUndoRedoKey(key) || tryHandleEditorChromeKey(key);
        });
    syncUiScaleFromEditor();
#if JUCE_DEBUG
    layoutUiElementsTestComponent();
#endif
    repaint();

    // With no focused descendant, Cmd/Ctrl shortcuts never reach PluginEditor /
    // MainComponent and the OS beeps instead. Request for Standalone and hosted;
    // visibilityChanged retries when the peer becomes showing later (cold-show race).
    requestEditorKeyboardFocusIfNeeded();
}
