// Extracted from PluginEditor.cpp for modular maintenance.
// Construction phases: main UI shell creation, header restore/wiring, runtime listener attachment.

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "Core/Services/PatchNameEditRules.h"
#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/PatchNameDisplayPanel.h"
#include "GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h"
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

    mainComponent_ = std::make_unique<MainComponent>(
        *skin_, layoutDimensions_, *widgetFactory_, pluginProcessor.getApvts(), pluginProcessor.getPatchFileService());
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
    headerPanel.refreshPortLists();

    pluginProcessor.restoreMidiPortsForHost();

    // Combo must mirror open reality after restore/sync (APVTS may have been cleared on failure).
    headerPanel.selectMidiFromPort(
        pluginProcessor.getApvts().state.getProperty("midiInputPortId", juce::String()).toString());
    headerPanel.selectMidiToPort(
        pluginProcessor.getApvts().state.getProperty("midiOutputPortId", juce::String()).toString());

    if (pluginProcessor.isStandalone())
    {
        const auto savedKeyboardFromPortId = pluginProcessor.getApvts().state.getProperty("keyboardFromPortId", juce::String()).toString();
        if (savedKeyboardFromPortId.isNotEmpty()
            && ! pluginProcessor.setKeyboardFromPort(savedKeyboardFromPortId))
        {
            // Open failure or MIDI From conflict — drop the dead selection.
            pluginProcessor.setKeyboardFromPort({});
        }

        headerPanel.selectKeyboardFromPort(
            pluginProcessor.getApvts().state.getProperty("keyboardFromPortId", juce::String()).toString());

        refreshAudioFromCombo(&headerPanel);

        const auto savedAudioFromSourceId = pluginProcessor.getApvts().state.getProperty("audioFromSourceId", juce::String()).toString();
        if (savedAudioFromSourceId.isNotEmpty())
        {
            headerPanel.selectAudioFromSourceId(savedAudioFromSourceId);
            pluginProcessor.setAudioFromSourceId(savedAudioFromSourceId);
        }

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
    headerPanel.getMidiFromComboBox().onChange = [this, &headerPanel]
    {
        const auto previousPortId = pluginProcessor.getApvts().state.getProperty("midiInputPortId", juce::String()).toString();
        const auto selectedPortId = headerPanel.getSelectedMidiFromPortIdentifier();

        if (pluginProcessor.setMidiInputPort(selectedPortId))
            return;

        headerPanel.selectMidiFromPort(previousPortId);
        if (previousPortId.isNotEmpty())
            pluginProcessor.setMidiInputPort(previousPortId);
    };

    headerPanel.getMidiToComboBox().onChange = [this, &headerPanel]
    {
        const auto previousPortId = pluginProcessor.getApvts().state.getProperty("midiOutputPortId", juce::String()).toString();
        const auto selectedPortId = headerPanel.getSelectedMidiToPortIdentifier();

        if (pluginProcessor.setMidiOutputPort(selectedPortId))
            return;

        headerPanel.selectMidiToPort(previousPortId);
        if (previousPortId.isNotEmpty())
            pluginProcessor.setMidiOutputPort(previousPortId);
    };

    headerPanel.getKeyboardFromComboBox().onChange = [this, &headerPanel]
    {
        if (!pluginProcessor.isStandalone())
            return;

        const auto previousPortId = pluginProcessor.getApvts().state.getProperty("keyboardFromPortId", juce::String()).toString();
        const auto selectedPortId = headerPanel.getSelectedKeyboardFromPortIdentifier();

        if (pluginProcessor.setKeyboardFromPort(selectedPortId))
            return;

        headerPanel.selectKeyboardFromPort(previousPortId);
        if (previousPortId.isNotEmpty())
            pluginProcessor.setKeyboardFromPort(previousPortId);
    };

    headerPanel.getInputGainSlider().onValueChange = [this, &headerPanel]
    {
        if (!pluginProcessor.isStandalone())
            return;

        const int index = static_cast<int>(std::round(headerPanel.getInputGainSlider().getValue()));
        pluginProcessor.setInputGainDb(PluginAudioConstants::inputGainIndexToDb(index));
    };

    headerPanel.getAudioFromComboBox().onChange = [this, &headerPanel]
    {
        if (!pluginProcessor.isStandalone())
            return;

        pluginProcessor.setAudioFromSourceId(headerPanel.getSelectedAudioFromSourceId());
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

    setWantsKeyboardFocus(true);
    setFocusContainerType(juce::Component::FocusContainerType::keyboardFocusContainer);
    addKeyListener(this);
    mainComponent_->addKeyListener(this);
    mainComponent_->setEditorialUndoRedoKeyHandler(
        [this](const juce::KeyPress& key) { return tryHandleEditorialUndoRedoKey(key); });
    syncUiScaleFromEditor();
#if JUCE_DEBUG
    layoutUiElementsTestComponent();
#endif
    repaint();
}
