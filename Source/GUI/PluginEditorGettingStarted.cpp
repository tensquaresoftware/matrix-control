// GETTING STARTED wizard overlay lifecycle, live bindings, auto-open / absorb Device Setup.

#include "PluginEditor.h"

#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/Services/DeviceConnectionMachineDefaults.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Core/Services/EpromTypePolicy.h"
#include "Core/Services/GettingStartedMachineDefaults.h"
#include "GUI/Dialogs/GettingStartedWizardDialog.h"
#include "GUI/Layout/ScaledLayout.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    GettingStartedWizardDialog::LiveDeviceStatus makeWizardLiveStatus(const juce::ValueTree& state)
    {
        return {
            .deviceDetected = static_cast<bool>(state.getProperty("deviceDetected", false)),
            .deviceMidiUnresponsive = static_cast<bool>(
                state.getProperty("deviceMidiUnresponsive", false)),
            .deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
                state.getProperty(MatrixDeviceTypes::kApvtsPropertyName)),
            .deviceVersion = state.getProperty("deviceVersion", juce::String()).toString(),
        };
    }

    int preferredEpromTypeForWizard(juce::ValueTree& state, MatrixDeviceTypes::Type deviceType)
    {
        const auto family = Core::EpromTypePolicy::deviceFamilyFromType(deviceType);
        return Core::EpromTypePolicy::preferredForPrompt(
            Core::EpromTypePolicy::suggestFromInquiryVersion(
                state.getProperty("deviceVersion", juce::String()).toString(), family),
            Core::EpromTypePolicy::normalize(static_cast<int>(
                state.getProperty(PluginIDs::Settings::kEpromType,
                                  PluginIDs::Settings::EpromType::kDefault))));
    }
}

void PluginEditor::updateGettingStartedWizardLayout(float uiScale)
{
    if (gettingStartedWizardDialog_ == nullptr)
        return;

    gettingStartedWizardDialog_->setUiScale(uiScale);
    gettingStartedWizardDialog_->setBounds(getLocalBounds());
}

void PluginEditor::markGettingStartedStepDone(GettingStartedWizard::Step step)
{
    auto prefs = Core::GettingStartedMachineDefaults::loadWizardPrefs();
    Core::GettingStartedMachineDefaults::markContentStepDone(
        prefs.flags, static_cast<int>(step));
    Core::GettingStartedMachineDefaults::persistStepFlags(prefs.flags);
    Core::GettingStartedMachineDefaults::persistHasLeftIntro(true);

    if (step != GettingStartedWizard::Step::kSynthCommunication)
        return;

    auto& state = pluginProcessor.getApvts().state;
    const int epromType = Core::EpromTypePolicy::normalize(static_cast<int>(
        state.getProperty(PluginIDs::Settings::kEpromType,
                          PluginIDs::Settings::EpromType::kDefault)));
    Core::markSessionDeviceSetupCompleteAfterWizardSynthDone(state);
    Core::DeviceConnectionMachineDefaults::writeAfterConfirmFromState(state, epromType);
}

void PluginEditor::handleGettingStartedConfigureLater()
{
    auto prefs = Core::GettingStartedMachineDefaults::loadWizardPrefs();
    prefs.configureLaterArm = Core::GettingStartedMachineDefaults::advanceConfigureLaterArm(
        prefs.configureLaterArm);
    // Track format for both OneReminder and Silenced (same-format reminder + Audio rearm).
    prefs.lastSilencedWasPlugin = ! pluginProcessor.isStandalone();
    Core::GettingStartedMachineDefaults::persistConfigureLaterArm(
        prefs.configureLaterArm, prefs.lastSilencedWasPlugin);
}

void PluginEditor::fillGettingStartedBindingState(GettingStartedWizardDialog::HostBindings& bindings)
{
    auto& state = pluginProcessor.getApvts().state;
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    const auto prefs = Core::GettingStartedMachineDefaults::loadWizardPrefs();
    const bool isPluginMode = ! pluginProcessor.isStandalone();

    bindings.scaleId = pluginProcessor.getGuiScaleId();
    bindings.skinId = pluginProcessor.getSkinVariantId();
    bindings.midiFromPortId = state.getProperty("midiInputPortId", juce::String()).toString();
    bindings.midiToPortId = state.getProperty("midiOutputPortId", juce::String()).toString();
    bindings.keyboardFromPortId = state.getProperty("keyboardFromPortId", juce::String()).toString();
    bindings.preferredEpromTypeId = preferredEpromTypeForWizard(state, deviceType);
    bindings.includeFirmwareSuggestionHint =
        state.getProperty("deviceVersion", juce::String()).toString().trim().isNotEmpty()
        && static_cast<bool>(state.getProperty("deviceDetected", false));
    bindings.useAudioResumeCopy =
        GettingStartedWizard::shouldUseAudioResumeCopy(prefs.flags) && ! isPluginMode;
    bindings.deviceStatus = makeWizardLiveStatus(state);

    if (isPluginMode)
        return;

    bindings.audioDeviceManager = Core::StandaloneAudioInputRouter::getAudioDeviceManager();
    bindings.synthFromChannelNames = pluginProcessor.getAudioInputSourceNames();
    bindings.synthFromChannelIds = pluginProcessor.getAudioInputSourceIds();
    bindings.selectedSynthFromSourceId =
        state.getProperty("audioFromSourceId", juce::String()).toString();
}

void PluginEditor::wireGettingStartedAppearanceCallbacks(
    GettingStartedWizardDialog::HostBindings& bindings)
{
    bindings.onScaleChanged = [this](int scaleId)
    {
        using namespace PluginIDs::Settings::ScaleLevels;
        if (scaleId >= kMin && scaleId <= kMax)
            applyUiScaleFromItemId(scaleId, true);
    };
    bindings.onSkinChanged = [this](int skinId)
    {
        using namespace PluginIDs::Settings::SkinVariants;
        if (skinId == kBlack || skinId == kCream)
            applySkinFromItemId(skinId, true);
    };
}

void PluginEditor::wireGettingStartedPortCallbacks(GettingStartedWizardDialog::HostBindings& bindings)
{
    bindings.onMidiFromChanged = [this](const juce::String& portId)
    {
        applyEpromTypePromptMidiPortChange(true, portId);
    };
    bindings.onMidiToChanged = [this](const juce::String& portId)
    {
        applyEpromTypePromptMidiPortChange(false, portId);
    };
    bindings.onEpromChanged = [this](int epromTypeId)
    {
        const int normalized = Core::EpromTypePolicy::normalize(epromTypeId);
        pluginProcessor.getApvts().state.setProperty(
            PluginIDs::Settings::kEpromType, normalized, nullptr);
        Core::DeviceConnectionMachineDefaults::writeEpromType(normalized);
        pluginProcessor.getMidiManager().refreshSysExDelayFromSettings();
    };
    bindings.onSearchingWindowStarted = [this]
    {
        pluginProcessor.getMidiManager().refreshDeviceInquiryAfterPortSync();
    };
    bindings.onKeyboardFromChanged = [this](const juce::String& portId)
    {
        if (pluginProcessor.isStandalone()
            && ! pluginProcessor.setKeyboardFromPort(portId) && portId.isNotEmpty())
            pluginProcessor.setKeyboardFromPort({});
    };
    bindings.onSynthFromChanged = [this](const juce::String& sourceId)
    {
        if (! pluginProcessor.isStandalone())
            return;
        pluginProcessor.setAudioFromSourceId(sourceId);
        if (sourceId.isNotEmpty())
            pluginProcessor.bindAudioFromInputDeviceIdentity(
                Core::StandaloneAudioInputRouter::getCurrentInputDeviceName());
    };
}

void PluginEditor::wireGettingStartedNavCallbacks(GettingStartedWizardDialog::HostBindings& bindings)
{
    bindings.onContentStepCompleted = [this](GettingStartedWizard::Step step)
    {
        markGettingStartedStepDone(step);
    };
    bindings.onConfigureLater = [this] { handleGettingStartedConfigureLater(); };
    bindings.onContinuedFromIntro = []
    {
        Core::GettingStartedMachineDefaults::persistHasLeftIntro(true);
    };
}

void PluginEditor::wireGettingStartedBindingCallbacks(GettingStartedWizardDialog::HostBindings& bindings)
{
    wireGettingStartedAppearanceCallbacks(bindings);
    wireGettingStartedPortCallbacks(bindings);
    wireGettingStartedNavCallbacks(bindings);
}

GettingStartedWizardDialog::HostBindings PluginEditor::makeGettingStartedHostBindings()
{
    GettingStartedWizardDialog::HostBindings bindings;
    fillGettingStartedBindingState(bindings);
    wireGettingStartedBindingCallbacks(bindings);
    return bindings;
}

void PluginEditor::openGettingStartedWizard(GettingStartedWizard::Step startStep)
{
    closeSettingsWindow();
    closeAboutWindow();
    closeMasterInitConfirmDialog();
    closeMasterM1kmLoadChoiceDialog();
    closeMutatorHistoryDefragConfirmDialog();
    closeEpromTypePromptDialog();
    hideBankTransferProgressDialog();

    if (gettingStartedWizardDialog_ == nullptr)
    {
        gettingStartedWizardDialog_ = std::make_unique<GettingStartedWizardDialog>(
            *skin_,
            ! pluginProcessor.isStandalone(),
            [this] { closeGettingStartedWizard(); });
        addChildComponent(*gettingStartedWizardDialog_);
    }
    else
    {
        gettingStartedWizardDialog_->setSkin(*skin_);
    }

    gettingStartedWizardDialog_->prepareForShow(startStep, makeGettingStartedHostBindings());

    const int baseWidth = layoutDimensions_.editor.width;
    const float uiScale = (baseWidth > 0)
        ? TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth)
        : 1.0f;
    updateGettingStartedWizardLayout(uiScale);

    gettingStartedWizardDialog_->setVisible(true);
    gettingStartedWizardDialog_->toFront(true);
    gettingStartedWizardDialog_->grabKeyboardFocus();
}

void PluginEditor::closeGettingStartedWizard()
{
    if (gettingStartedWizardDialog_ != nullptr)
    {
        gettingStartedWizardDialog_->stopLiveTimers();
        gettingStartedWizardDialog_->setVisible(false);
    }

    requestEditorKeyboardFocusIfNeeded();
}

void PluginEditor::refreshGettingStartedWizardLiveState()
{
    if (gettingStartedWizardDialog_ == nullptr || ! gettingStartedWizardDialog_->isVisible())
        return;

    gettingStartedWizardDialog_->updateLiveDeviceStatus(
        makeWizardLiveStatus(pluginProcessor.getApvts().state));
}

void PluginEditor::refreshGettingStartedWizardPorts()
{
    if (gettingStartedWizardDialog_ == nullptr || ! gettingStartedWizardDialog_->isVisible())
        return;

    auto& state = pluginProcessor.getApvts().state;
    gettingStartedWizardDialog_->syncPortsFromHost(
        state.getProperty("midiInputPortId", juce::String()).toString(),
        state.getProperty("midiOutputPortId", juce::String()).toString(),
        true);
}

void PluginEditor::refreshGettingStartedWizardKeyboardFrom()
{
    if (gettingStartedWizardDialog_ == nullptr || ! gettingStartedWizardDialog_->isVisible())
        return;
    if (! pluginProcessor.isStandalone())
        return;

    gettingStartedWizardDialog_->syncKeyboardFromHost(
        pluginProcessor.getApvts().state.getProperty("keyboardFromPortId", juce::String()).toString(),
        true);
}

void PluginEditor::refreshGettingStartedWizardSuggestion()
{
    if (gettingStartedWizardDialog_ == nullptr || ! gettingStartedWizardDialog_->isVisible())
        return;

    auto& state = pluginProcessor.getApvts().state;
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    gettingStartedWizardDialog_->refreshEpromSuggestion(
        deviceType, preferredEpromTypeForWizard(state, deviceType));
}

void PluginEditor::refreshGettingStartedWizardSynthFrom()
{
    if (gettingStartedWizardDialog_ == nullptr || ! gettingStartedWizardDialog_->isVisible())
        return;
    if (! pluginProcessor.isStandalone())
        return;

    auto& state = pluginProcessor.getApvts().state;
    gettingStartedWizardDialog_->populateSynthFromChannels(
        pluginProcessor.getAudioInputSourceNames(),
        pluginProcessor.getAudioInputSourceIds(),
        state.getProperty("audioFromSourceId", juce::String()).toString());
}

void PluginEditor::maybeAutoOpenGettingStartedWizard()
{
    using namespace Core::GettingStartedMachineDefaults;

    const bool isPluginMode = ! pluginProcessor.isStandalone();
    const bool sessionPromptDone = static_cast<bool>(
        pluginProcessor.getApvts().state.getProperty(
            PluginIDs::Settings::kEpromTypePromptDone, false));
    const bool machinePromptDone = Core::DeviceConnectionMachineDefaults::load().promptDone;

    const auto loaded = loadAndMigrate(sessionPromptDone, machinePromptDone);
    if (! loaded.has_value())
        return;

    auto prefs = *loaded;
    const auto previousArm = prefs.configureLaterArm;
    prefs.configureLaterArm = rearmIfNewlyApplicable(
        prefs.configureLaterArm, prefs.lastSilencedWasPlugin, isPluginMode, prefs.flags);
    if (prefs.configureLaterArm != previousArm)
        persistConfigureLaterArm(prefs.configureLaterArm, prefs.lastSilencedWasPlugin);

    const auto decision = decideAutoOpen(prefs, isPluginMode);
    if (! decision.shouldOpen)
        return;

    if (decision.consumeOneReminder)
        persistConfigureLaterArm(ConfigureLaterArm::kSilenced, isPluginMode);

    const auto startStep = static_cast<GettingStartedWizard::Step>(decision.startStepIndex);
    juce::MessageManager::callAsync(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this), startStep]
        {
            if (safeThis != nullptr)
                safeThis->openGettingStartedWizard(startStep);
        });
}
