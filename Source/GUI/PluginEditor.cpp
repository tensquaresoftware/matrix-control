#include "PluginEditor.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include "Core/Audio/AudioPassthroughProcessor.h"
#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/PluginProcessor.h"
#include "Core/Services/PatchFileNameReconciler.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "GUI/Settings/SettingsPanel.h"
#include "GUI/Settings/SettingsWindow.h"
#include "GUI/About/AboutWindow.h"
#include "GUI/Dialogs/MasterInitConfirmDialog.h"
#include "Skins/Skin.h"
#include "Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginAudioConstants.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

using TSS::SkinColourId;

class PluginEditor::HeaderRefreshTimer : private juce::Timer
{
public:
    HeaderRefreshTimer(PluginProcessor& processor, HeaderPanel& headerPanel, PluginEditor& owner)
        : processor_(processor)
        , headerPanel_(headerPanel)
        , owner_(owner)
    {
        startTimerHz(30);
    }

private:
    void timerCallback() override
    {
        if (processor_.isStandalone())
        {
            if (audioFromRefreshAttempts_ < 60)
            {
                const auto names = processor_.getAudioInputSourceNames();

                if (names.isEmpty())
                {
                    ++audioFromRefreshAttempts_;
                    owner_.refreshAudioFromCombo();
                }
            }

            headerPanel_.getPeakIndicator().setLevel(
                processor_.getAudioPassthroughProcessor().getPeakLevel());
        }

        const auto& tracker = processor_.getMidiActivityTracker();
        headerPanel_.getInstrumentActivityLed().setLevel(
            tracker.getActivityLevel(Core::MidiActivityTracker::Path::kInstrument));
        headerPanel_.getEditorActivityLed().setLevel(
            tracker.getActivityLevel(Core::MidiActivityTracker::Path::kMidiFromInbound));
        headerPanel_.getMidiToActivityLed().setLevel(
            tracker.getActivityLevel(Core::MidiActivityTracker::Path::kOutbound));
    }

    PluginProcessor& processor_;
    HeaderPanel& headerPanel_;
    PluginEditor& owner_;
    int audioFromRefreshAttempts_ = 0;
};


PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p)
    , pluginProcessor(p)
{
    pluginProcessor.setPatchFolderPicker([safeThis = juce::Component::SafePointer<PluginEditor>(this)]() -> juce::File
    {
        if (safeThis == nullptr)
            return {};

        juce::File startDirectory;
        const auto persistedPath = safeThis->pluginProcessor.getApvts().state.getProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            juce::String()).toString();

        if (persistedPath.isNotEmpty())
        {
            const juce::File persistedFolder(persistedPath);
            if (persistedFolder.isDirectory())
                startDirectory = persistedFolder;
        }

        juce::FileChooser chooser("Select patch folder",
                                  startDirectory,
                                  juce::String(),
                                  true,
                                  false,
                                  safeThis.getComponent());

        if (chooser.browseForDirectory())
            return chooser.getResult();

        return {};
    });

    pluginProcessor.setMutatorExportFolderPicker(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)]() -> juce::File
        {
            if (safeThis == nullptr)
                return {};

            juce::FileChooser chooser("Select mutation export folder",
                                      juce::File(),
                                      juce::String(),
                                      true,
                                      false,
                                      safeThis.getComponent());

            if (chooser.browseForDirectory())
                return chooser.getResult();

            return {};
        });

    pluginProcessor.setMutatorDefragLimitModalGate(
        [](std::function<void()> onConfirmed)
        {
            namespace Dialog = PluginDisplayNames::Dialogs::MutatorHistoryDefrag;

            juce::AlertWindow alert(Dialog::kTitle, Dialog::kBody, juce::AlertWindow::QuestionIcon);
            alert.addButton(Dialog::kConfirm, 1);
            alert.addButton(Dialog::kCancel, 0);

            if (alert.runModalLoop() == 1 && onConfirmed)
                onConfirmed();
        });

    pluginProcessor.setMutatorExportCollisionModalGate(
        [](std::function<void(Core::ExportCollisionResolution)> onResolved)
        {
            jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());

            namespace Msg = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages;

            juce::AlertWindow alert(Msg::kExportCollisionTitle,
                                    Msg::kExportCollisionMessage,
                                    juce::AlertWindow::QuestionIcon);
            alert.addButton(Msg::kExportCollisionOverwrite, 1);
            alert.addButton(Msg::kExportCollisionKeep, 2);
            alert.addButton(Msg::kExportCollisionCancel, 0);

            if (! onResolved)
                return;

            switch (alert.runModalLoop())
            {
                case 1: onResolved(Core::ExportCollisionResolution::kOverwrite); break;
                case 2: onResolved(Core::ExportCollisionResolution::kKeep); break;
                default: onResolved(Core::ExportCollisionResolution::kCancel); break;
            }
        });

    pluginProcessor.setMutatorHistoryGateModalGate(
        []() -> Core::MutatorHistoryGateChoice
        {
            jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());

            namespace Msg = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages;

            juce::AlertWindow alert(Msg::kHistoryGateTitle,
                                    Msg::kHistoryGateMessage,
                                    juce::AlertWindow::QuestionIcon);
            alert.addButton(Msg::kHistoryGateExport, 1);
            alert.addButton(Msg::kHistoryGateDiscard, 2);
            alert.addButton(Msg::kHistoryGateCancel, 0);

            switch (alert.runModalLoop())
            {
                case 1: return Core::MutatorHistoryGateChoice::kExport;
                case 2: return Core::MutatorHistoryGateChoice::kDiscard;
                default: return Core::MutatorHistoryGateChoice::kCancel;
            }
        });

    pluginProcessor.setPatchSaveFilePicker(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)](
            juce::File suggestedFolder, juce::String suggestedStem) -> juce::File
        {
            if (safeThis == nullptr)
                return {};

            juce::FileChooser chooser("Save patch as",
                                      suggestedFolder.getChildFile(suggestedStem + ".syx"),
                                      "*.syx",
                                      true,
                                      false,
                                      safeThis.getComponent());

            if (chooser.browseForFileToSave(true))
                return chooser.getResult();

            return {};
        });

    pluginProcessor.setPatchNameReconciliationPicker(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)](
            juce::String internalSanitized, juce::String fileSanitized)
            -> std::optional<Core::NameReconciliationChoice>
        {
            if (safeThis == nullptr)
                return std::nullopt;

            namespace Dialog = PluginDisplayNames::Dialogs::PatchNameReconciliation;
            const auto body = juce::String(Dialog::kBodyTemplate)
                                  .replace("{INTERNAL}", internalSanitized)
                                  .replace("{FILENAME}", fileSanitized);

            juce::AlertWindow alert(Dialog::kTitle, body, juce::AlertWindow::QuestionIcon);
            alert.addButton(Dialog::kInternal, 1);
            alert.addButton(Dialog::kFilename, 2);
            alert.addButton(Dialog::kCancel, 0);

            switch (alert.runModalLoop())
            {
                case 1: return Core::NameReconciliationChoice::kInternal;
                case 2: return Core::NameReconciliationChoice::kFilename;
                default: return std::nullopt;
            }
        });

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

    const auto editorWidth = layoutDimensions_.editor.width;
    const auto editorHeight = layoutDimensions_.editor.height;

    mainComponent_ = std::make_unique<MainComponent>(
        *skin_, layoutDimensions_, *widgetFactory_, pluginProcessor.getApvts(), pluginProcessor.getPatchFileService());
    addAndMakeVisible(*mainComponent_);

    mainComponent_->setBusReorderHandler(
        [this](int fromBus, int toBus)
        {
            pluginProcessor.swapMatrixModBusContents(fromBus, toBus);
        });

    mainComponent_->setMasterInitConfirmationGate(
        [this](const juce::String& /*initPropertyId*/,
               const juce::String& moduleDisplayName,
               std::function<void()> onConfirmed)
        {
            openMasterInitConfirmDialog(moduleDisplayName, std::move(onConfirmed));
        });

    testComponent_ = std::make_unique<TestComponent>(
        *skin_,
        pluginProcessor.getApvts(),
        pluginProcessor.getApvts().state,
        editorWidth,
        editorHeight);
    addChildComponent(*testComponent_);
    testComponent_->setVisible(false);

    updateSkin();

    auto& headerPanel = mainComponent_->getHeaderPanel();

    const int savedScaleId = pluginProcessor.getGuiScaleId();
    const float savedUiScale = PluginIDs::Settings::ScaleLevels::getUiScale(savedScaleId);
    applyUiScale(savedUiScale);

    restoreHeaderPanelFromState(headerPanel);
    wireHeaderPanel(headerPanel);

    headerPanel.setPluginMode(!pluginProcessor.isStandalone());
    headerPanel.refreshPortLists();

    const auto savedMidiInputPortId = pluginProcessor.getApvts().state.getProperty("midiInputPortId", juce::String()).toString();
    headerPanel.selectMidiFromPort(savedMidiInputPortId);

    const auto savedMidiOutputPortId = pluginProcessor.getApvts().state.getProperty("midiOutputPortId", juce::String()).toString();
    headerPanel.selectMidiToPort(savedMidiOutputPortId);

    pluginProcessor.restoreMidiPortsForHost();

    if (pluginProcessor.isStandalone())
    {
        const auto savedKeyboardFromPortId = pluginProcessor.getApvts().state.getProperty("keyboardFromPortId", juce::String()).toString();
        headerPanel.selectKeyboardFromPort(savedKeyboardFromPortId);
        pluginProcessor.setKeyboardFromPort(headerPanel.getSelectedKeyboardFromPortIdentifier());

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

    headerRefreshTimer_ = std::make_unique<HeaderRefreshTimer>(pluginProcessor, headerPanel, *this);
    attachStandaloneAudioDeviceListener();
    pluginProcessor.getApvts().state.addListener(this);

    syncUiScaleFromEditor();
    layoutUiElementsTestComponent();
    repaint();
}

void PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kBodyPanelBackground));
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

    layoutUiElementsTestComponent();
    syncUiScaleFromEditor();

    if (pluginProcessor.isStandalone())
        syncStandaloneWindowSize();
}

void PluginEditor::syncStandaloneWindowSize()
{
    if (auto* window = findParentComponentOfClass<juce::DocumentWindow>())
        window->setContentComponentSize(getWidth(), getHeight());
}

void PluginEditor::syncUiScaleFromEditor()
{
    const int baseWidth = layoutDimensions_.editor.width;
    if (baseWidth <= 0)
        return;

    const float uiScale = TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth);

    if (auto* comp = mainComponent_.get())
        comp->setUiScale(uiScale);

    if (auto* comp = mainComponent_.get())
    {
        int matchingScaleId = 0;
        const int layoutPercentRounded = juce::roundToInt(uiScale * 100.0f);
        for (int id = PluginIDs::Settings::ScaleLevels::kMin; id <= PluginIDs::Settings::ScaleLevels::kMax; ++id)
        {
            const int presetPercentRounded = juce::roundToInt(
                PluginIDs::Settings::ScaleLevels::getUiScale(id) * 100.0f);
            if (layoutPercentRounded == presetPercentRounded)
            {
                matchingScaleId = id;
                break;
            }
        }

        if (matchingScaleId != 0)
            comp->getHeaderPanel().setCurrentUiScaleId(matchingScaleId);
    }

    if (settingsWindow_ != nullptr && settingsWindow_->isVisible())
        updateSettingsWindowLayout(uiScale);

    if (aboutWindow_ != nullptr && aboutWindow_->isVisible())
        updateAboutWindowLayout(uiScale);

    if (masterInitConfirmDialog_ != nullptr && masterInitConfirmDialog_->isVisible())
        updateMasterInitConfirmDialogLayout(uiScale);
}

void PluginEditor::updateSettingsWindowLayout(float uiScale)
{
    if (settingsWindow_ == nullptr)
        return;

    settingsWindow_->setUiScale(uiScale);
    settingsWindow_->setBounds(getLocalBounds());
}

void PluginEditor::updateAboutWindowLayout(float uiScale)
{
    if (aboutWindow_ == nullptr)
        return;

    aboutWindow_->setUiScale(uiScale);
    aboutWindow_->setBounds(getLocalBounds());
}

void PluginEditor::updateMasterInitConfirmDialogLayout(float uiScale)
{
    if (masterInitConfirmDialog_ == nullptr)
        return;

    masterInitConfirmDialog_->setUiScale(uiScale);
    masterInitConfirmDialog_->setBounds(getLocalBounds());
}

void PluginEditor::applySkinFromItemId(int skinItemId, bool persistToState)
{
    skin_ = (skinItemId == PluginIDs::Settings::SkinVariants::kBlack)
        ? skinBlack_.get()
        : skinCream_.get();
    updateSkin();

    if (persistToState)
        pluginProcessor.setSkinVariantId(skinItemId);

    if (auto* comp = mainComponent_.get())
        comp->getHeaderPanel().setCurrentSkinItemId(skinItemId);
}

void PluginEditor::applyUiScaleFromItemId(int scaleId, bool persistToState)
{
    const float uiScale = PluginIDs::Settings::ScaleLevels::getUiScale(scaleId);
    applyUiScale(uiScale);
    updateSettingsWindowLayout(uiScale);
    updateAboutWindowLayout(uiScale);

    if (persistToState)
        pluginProcessor.setGuiScaleId(scaleId);

    if (auto* comp = mainComponent_.get())
        comp->getHeaderPanel().setCurrentUiScaleId(scaleId);
}

void PluginEditor::wireHeaderPanel(HeaderPanel& headerPanel)
{
    headerPanel.onSkinSelected = [this](int skinItemId)
    {
        applySkinFromItemId(skinItemId, true);
    };

    headerPanel.onUiScaleSelected = [this](int scaleId)
    {
        applyUiScaleFromItemId(scaleId, true);
    };

    headerPanel.onUiScaleReset = [this]
    {
        applyUiScaleFromItemId(PluginIDs::Settings::ScaleLevels::k100, true);
    };

    headerPanel.onSettingsRequested = [this]
    {
        if (settingsWindow_ != nullptr && settingsWindow_->isVisible())
            closeSettingsWindow();
        else
            openSettingsWindow();
    };

    headerPanel.onAboutRequested = [this]
    {
        if (aboutWindow_ != nullptr && aboutWindow_->isVisible())
            closeAboutWindow();
        else
            openAboutWindow();
    };

    headerPanel.onAudioMidiSettingsRequested = []
    {
        Core::StandaloneAudioInputRouter::showAudioMidiSettingsDialog();
    };

    headerPanel.onUiTestsToggleRequested = [this]
    {
        setUiElementsTestVisible(!uiElementsTestVisible_);
    };
}

void PluginEditor::restoreHeaderPanelFromState(HeaderPanel& headerPanel)
{
    headerPanel.setCurrentUiScaleId(pluginProcessor.getGuiScaleId());
    headerPanel.setCurrentSkinItemId(pluginProcessor.getSkinVariantId());
}

void PluginEditor::mouseDown(const juce::MouseEvent& event)
{
    if (uiElementsTestVisible_
        && testComponent_ != nullptr
        && testComponent_->isVisible()
        && testComponent_->getBounds().contains(event.getPosition()))
    {
        testComponent_->grabKeyboardFocus();
        return;
    }

    unfocusAllComponents();
}

bool PluginEditor::keyPressed(const juce::KeyPress& key)
{
    if (uiElementsTestVisible_
        && testComponent_ != nullptr
        && testComponent_->isVisible()
        && testComponent_->keyPressed(key))
    {
        return true;
    }

    return juce::AudioProcessorEditor::keyPressed(key);
}

void PluginEditor::updateSkin()
{
    if (auto* widget = mainComponent_.get())
        widget->setSkin(*skin_);

    if (settingsWindow_ != nullptr)
        settingsWindow_->setSkin(*skin_);

    if (aboutWindow_ != nullptr)
        aboutWindow_->setSkin(*skin_);

    if (testComponent_ != nullptr)
        testComponent_->setSkin(*skin_);

    repaint();
}

void PluginEditor::applyUiScale(float uiScale)
{
    appliedUiScale_ = uiScale;

    const int targetWidth = juce::roundToInt(static_cast<float>(layoutDimensions_.editor.width) * uiScale);
    const int targetHeight = juce::roundToInt(static_cast<float>(layoutDimensions_.editor.height) * uiScale);

    if (pluginProcessor.isStandalone())
        setResizeLimits(targetWidth, targetHeight, targetWidth, targetHeight);

    setSize(targetWidth, targetHeight);

    if (pluginProcessor.isStandalone())
        syncStandaloneWindowSize();
}

void PluginEditor::setUiElementsTestVisible(bool visible)
{
    if (uiElementsTestVisible_ == visible)
        return;

    uiElementsTestVisible_ = visible;

    if (auto* main = mainComponent_.get())
        main->setUiElementsTestVisible(visible);

    if (auto* test = testComponent_.get())
    {
        test->setVisible(visible);
        if (visible)
        {
            test->toFront(false);
            test->grabKeyboardFocus();
        }
    }

    layoutUiElementsTestComponent();
    repaint();
}

void PluginEditor::layoutUiElementsTestComponent()
{
    if (!uiElementsTestVisible_ || mainComponent_ == nullptr || testComponent_ == nullptr)
        return;

    testComponent_->setBounds(mainComponent_->getUiElementsTestAreaBounds());
}

PluginEditor::~PluginEditor()
{
    pluginProcessor.getApvts().state.removeListener(this);
    detachStandaloneAudioDeviceListener();
    closeSettingsWindow();
    closeAboutWindow();
}

SettingsPanel* PluginEditor::getSettingsPanelIfOpen()
{
    if (settingsWindow_ == nullptr || !settingsWindow_->isVisible())
        return nullptr;

    return &settingsWindow_->getSettingsPanel();
}

void PluginEditor::openSettingsWindow()
{
    closeAboutWindow();

    if (settingsWindow_ == nullptr)
    {
        const bool isPluginMode = !pluginProcessor.isStandalone();
        settingsWindow_ = std::make_unique<SettingsWindow>(
            *skin_,
            isPluginMode,
            [this](SettingsPanel& panel)
            {
                wireSettingsPanel(panel);
            },
            [this] { closeSettingsWindow(); });
        addChildComponent(*settingsWindow_);
    }
    else
    {
        settingsWindow_->setSkin(*skin_);
    }

    const int baseWidth = layoutDimensions_.editor.width;
    const float uiScale = (baseWidth > 0)
        ? TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth)
        : 1.0f;
    updateSettingsWindowLayout(uiScale);

    settingsWindow_->setVisible(true);
    restoreSettingsPanelFromState(settingsWindow_->getSettingsPanel());
    settingsWindow_->toFront(true);
    settingsWindow_->grabKeyboardFocus();
}

void PluginEditor::closeSettingsWindow()
{
    if (settingsWindow_ != nullptr)
        settingsWindow_->setVisible(false);
}

void PluginEditor::openAboutWindow()
{
    closeSettingsWindow();

    if (aboutWindow_ == nullptr)
    {
        aboutWindow_ = std::make_unique<AboutWindow>(
            *skin_,
            [this] { closeAboutWindow(); });
        addChildComponent(*aboutWindow_);
    }
    else
    {
        aboutWindow_->setSkin(*skin_);
    }

    const int baseWidth = layoutDimensions_.editor.width;
    const float uiScale = (baseWidth > 0)
        ? TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth)
        : 1.0f;
    updateAboutWindowLayout(uiScale);

    aboutWindow_->setVisible(true);
    aboutWindow_->toFront(true);
    aboutWindow_->grabKeyboardFocus();
}

void PluginEditor::closeAboutWindow()
{
    if (aboutWindow_ != nullptr)
        aboutWindow_->setVisible(false);
}

void PluginEditor::openMasterInitConfirmDialog(const juce::String& moduleDisplayName,
                                               std::function<void()> onConfirm)
{
    closeSettingsWindow();
    closeAboutWindow();

    if (masterInitConfirmDialog_ == nullptr)
    {
        masterInitConfirmDialog_ = std::make_unique<MasterInitConfirmDialog>(
            *skin_,
            [this] { closeMasterInitConfirmDialog(); });
        addChildComponent(*masterInitConfirmDialog_);
    }
    else
    {
        masterInitConfirmDialog_->setSkin(*skin_);
    }

    masterInitConfirmDialog_->prepareForShow(moduleDisplayName, std::move(onConfirm));

    const int baseWidth = layoutDimensions_.editor.width;
    const float uiScale = (baseWidth > 0)
        ? TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth)
        : 1.0f;
    updateMasterInitConfirmDialogLayout(uiScale);

    masterInitConfirmDialog_->setVisible(true);
    masterInitConfirmDialog_->toFront(true);
    masterInitConfirmDialog_->grabKeyboardFocus();
}

void PluginEditor::closeMasterInitConfirmDialog()
{
    if (masterInitConfirmDialog_ != nullptr)
        masterInitConfirmDialog_->setVisible(false);
}

void PluginEditor::restoreSettingsPanelFromState(SettingsPanel& panel)
{
    if (!pluginProcessor.isStandalone())
        panel.getHardwareLatencySlider().setValue(pluginProcessor.getHardwareLatencyMs(), juce::dontSendNotification);

    const int policy = static_cast<int>(pluginProcessor.getApvts().state.getProperty(
        PluginIDs::Settings::kComputerPatchesNameReconciliationPolicy,
        PluginIDs::Settings::NameReconciliationPolicy::kDefault));
    panel.getNameReconciliationPolicyCombo().setSelectedId(policy, juce::dontSendNotification);
}

void PluginEditor::wireSettingsPanel(SettingsPanel& panel)
{
    panel.getHardwareLatencySlider().onValueChange = [this, &panel]
    {
        pluginProcessor.setHardwareLatencyMs(static_cast<float>(panel.getHardwareLatencySlider().getValue()));
    };

    panel.getNameReconciliationPolicyCombo().onChange = [this, &panel]
    {
        pluginProcessor.getApvts().state.setProperty(
            PluginIDs::Settings::kComputerPatchesNameReconciliationPolicy,
            panel.getNameReconciliationPolicyCombo().getSelectedId(),
            nullptr);
    };
}

void PluginEditor::refreshAudioFromCombo(HeaderPanel* headerOverride)
{
    if (!pluginProcessor.isStandalone())
        return;

    const auto names = pluginProcessor.getAudioInputSourceNames();
    const auto ids = pluginProcessor.getAudioInputSourceIds();

    auto* header = headerOverride;

    if (header == nullptr && mainComponent_ != nullptr)
        header = &mainComponent_->getHeaderPanel();

    juce::String sourceIdToRestore = pluginProcessor.getApvts().state.getProperty(
        "audioFromSourceId", juce::String()).toString();

    if (header != nullptr)
    {
        if (sourceIdToRestore.isEmpty())
            sourceIdToRestore = header->getSelectedAudioFromSourceId();

        header->populateAudioFromCombo(names, ids);

        if (sourceIdToRestore.isNotEmpty() && ids.contains(sourceIdToRestore))
        {
            header->selectAudioFromSourceId(sourceIdToRestore);
            pluginProcessor.setAudioFromSourceId(sourceIdToRestore);
            return;
        }

        if (sourceIdToRestore.isEmpty())
        {
            const auto effectiveSourceId = header->getSelectedAudioFromSourceId();

            if (effectiveSourceId.isNotEmpty())
                pluginProcessor.setAudioFromSourceId(effectiveSourceId);
            else if (! ids.isEmpty())
                pluginProcessor.setAudioFromSourceId(ids[0]);
        }
        else
        {
            // Stale id not in catalog: keep APVTS preference; avoid writing combo fallback of the wrong kind.
            header->selectAudioFromSourceId(sourceIdToRestore);
        }

        return;
    }

    if (sourceIdToRestore.isEmpty() && ! ids.isEmpty())
        sourceIdToRestore = ids[0];

    if (sourceIdToRestore.isNotEmpty() && (ids.isEmpty() || ids.contains(sourceIdToRestore)))
        pluginProcessor.setAudioFromSourceId(sourceIdToRestore);
}

void PluginEditor::attachStandaloneAudioDeviceListener()
{
    if (!pluginProcessor.isStandalone())
        return;

    Core::StandaloneAudioInputRouter::addAudioDeviceChangeListener(*this);
}

void PluginEditor::detachStandaloneAudioDeviceListener()
{
    Core::StandaloneAudioInputRouter::removeAudioDeviceChangeListener(*this);
}

void PluginEditor::changeListenerCallback(juce::ChangeBroadcaster*)
{
    refreshAudioFromCombo();

    if (! pluginProcessor.isStandalone())
        return;

    const auto sourceId = pluginProcessor.getApvts().state.getProperty("audioFromSourceId", juce::String()).toString();
    if (sourceId.isNotEmpty())
        Core::StandaloneAudioInputRouter::enableInputMonitoring();
}

void PluginEditor::valueTreePropertyChanged(juce::ValueTree&,
                                            const juce::Identifier& property)
{
    if (! pluginProcessor.isStandalone())
        return;

    const auto propertyName = property.toString();
    if (propertyName == MatrixDeviceTypes::kApvtsPropertyName
        || propertyName == "deviceDetected")
    {
        juce::MessageManager::callAsync(
            [safeThis = juce::Component::SafePointer<PluginEditor>(this)]
            {
                if (safeThis != nullptr)
                    safeThis->refreshAudioFromCombo();
            });
    }
}

void PluginEditor::valueTreeRedirected(juce::ValueTree&)
{
    if (! pluginProcessor.isStandalone())
        return;

    juce::MessageManager::callAsync(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)]
        {
            if (safeThis != nullptr)
                safeThis->refreshAudioFromCombo();
        });
}
