#include "AudioMidiSettingsWindow.h"

#include "Core/Audio/AudioDevicePreferredSetup.h"
#include "Core/Audio/AudioDeviceProfiles.h"
#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/Settings/SettingsWindow.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

namespace
{
    bool deviceSupportsSampleRate(juce::AudioIODevice* device, double sampleRate)
    {
        if (device == nullptr || sampleRate <= 0.0)
            return false;

        for (const auto rate : device->getAvailableSampleRates())
        {
            if (juce::approximatelyEqual(rate, sampleRate))
                return true;
        }

        return false;
    }

    bool deviceSupportsBufferSize(juce::AudioIODevice* device, int bufferSize)
    {
        if (device == nullptr || bufferSize <= 0)
            return false;

        return device->getAvailableBufferSizes().contains(bufferSize);
    }

    Core::AudioDeviceIdentity identityFromSetup(const juce::AudioDeviceManager::AudioDeviceSetup& setup)
    {
        return { .outputDeviceName = setup.outputDeviceName, .inputDeviceName = setup.inputDeviceName };
    }

    Core::AudioDeviceCapabilities capabilitiesFromDevice(juce::AudioIODevice* device)
    {
        Core::AudioDeviceCapabilities capabilities;
        if (device == nullptr)
            return capabilities;

        capabilities.availableInputChannelCount = device->getInputChannelNames().size();
        capabilities.availableOutputChannelCount = device->getOutputChannelNames().size();
        capabilities.sampleRates = device->getAvailableSampleRates();
        capabilities.bufferSizes = device->getAvailableBufferSizes();
        return capabilities;
    }

    Core::AudioDeviceProfileKey profileKeyFromLiveSetup(
        const juce::AudioDeviceManager& deviceManager,
        const juce::AudioDeviceManager::AudioDeviceSetup& setup,
        juce::AudioIODevice* device)
    {
        const auto capabilities = capabilitiesFromDevice(device);
        return Core::buildProfileKey({
            .driverTypeName = deviceManager.getCurrentAudioDeviceType(),
            .inputDeviceName = setup.inputDeviceName,
            .outputDeviceName = setup.outputDeviceName,
            .availableInputChannelCount = capabilities.availableInputChannelCount,
            .availableOutputChannelCount = capabilities.availableOutputChannelCount,
        });
    }

    void applyValidatedProfileToSetup(juce::AudioDeviceManager::AudioDeviceSetup& setup,
                                      const Core::ValidatedAudioDeviceProfile& validated)
    {
        setup.inputChannels = validated.inputChannels;
        setup.outputChannels = validated.outputChannels;

        if (validated.applySampleRate)
            setup.sampleRate = validated.sampleRate;

        if (validated.applyBufferSize)
            setup.bufferSize = validated.bufferSize;
    }

    /** Mutates candidateSetup only. Returns false when device is missing or no matching profile. */
    bool tryRestoreMatchingProfile(const Core::AudioDeviceProfileKey& key,
                                   juce::AudioIODevice* device,
                                   juce::AudioDeviceManager::AudioDeviceSetup& candidateSetup)
    {
        if (device == nullptr)
            return false;

        const auto profiles = Core::loadAudioDeviceProfiles();
        const int index = Core::findProfileIndex(profiles, key);
        if (index < 0)
            return false;

        const auto validated = Core::validateProfileAgainstCapabilities(
            profiles.getReference(index),
            capabilitiesFromDevice(device));
        applyValidatedProfileToSetup(candidateSetup, validated);
        return true;
    }

    void captureLiveSetupAsProfile(const juce::AudioDeviceManager& deviceManager,
                                   const juce::AudioDeviceManager::AudioDeviceSetup& setup,
                                   juce::AudioIODevice* device)
    {
        if (device == nullptr)
            return;

        const auto key = profileKeyFromLiveSetup(deviceManager, setup, device);
        if (! Core::shouldCaptureAudioDeviceProfile(key))
            return;

        Core::AudioDeviceProfile profile;
        profile.key = key;
        profile.inputChannels = setup.inputChannels;
        profile.outputChannels = setup.outputChannels;
        profile.sampleRate = setup.sampleRate;
        profile.bufferSize = setup.bufferSize;
        profile.lastUsedUtcMs = juce::Time::currentTimeMillis();

        auto profiles = Core::upsertProfileLru(Core::loadAudioDeviceProfiles(), profile);
        Core::saveAudioDeviceProfiles(profiles);
    }

    struct LiveSetupMutation
    {
        juce::AudioDeviceManager& deviceManager;
        juce::AudioDeviceManager::AudioDeviceSetup& setup;
        juce::AudioIODevice*& device;
        bool& restoringFlag;
    };

    bool applySetupWithRestoreGuard(LiveSetupMutation& live,
                                    const juce::AudioDeviceManager::AudioDeviceSetup& setup)
    {
        live.restoringFlag = true;
        const auto error = live.deviceManager.setAudioDeviceSetup(setup, true);
        live.restoringFlag = false;
        return error.isEmpty();
    }

    void refreshSetupAfterApply(LiveSetupMutation& live)
    {
        live.setup = live.deviceManager.getAudioDeviceSetup();
        live.device = live.deviceManager.getCurrentAudioDevice();
    }

    bool applyPreferredOverlayIfNeeded(LiveSetupMutation& live,
                                       const Core::PreferredSetupChangePlan& plan)
    {
        if (! plan.shouldRestore)
            return false;

        auto candidate = live.setup;
        candidate.sampleRate = plan.sampleRateToApply;
        candidate.bufferSize = plan.bufferSizeToApply;
        const bool applied = applySetupWithRestoreGuard(live, candidate);
        refreshSetupAfterApply(live);
        return applied;
    }
}

AudioMidiSettingsWindow::AudioMidiSettingsWindow(Config config)
    : onCloseRequested_(std::move(config.onCloseRequested))
    , peakLevelProvider_(std::move(config.peakLevelProvider))
    , skin_(config.skin)
    , deviceManager_(*config.deviceManager)
{
    jassert(config.skin != nullptr);
    jassert(config.deviceManager != nullptr);

    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    closeButton_ = std::make_unique<SettingsCloseButton>();
    closeButton_->setSkin(*skin_);
    closeButton_->onClick = [this]
    {
        if (onCloseRequested_)
            onCloseRequested_();
    };
    addAndMakeVisible(*closeButton_);

    const auto selectorPolicy = Core::matrixAudioMidiSettingsSelectorPolicy();
    deviceSelector_ = std::make_unique<juce::AudioDeviceSelectorComponent>(
        deviceManager_,
        0,
        juce::jmax(0, config.maxInputChannels),
        0,
        juce::jmax(0, config.maxOutputChannels),
        selectorPolicy.showMidiInputOptions,
        selectorPolicy.showMidiOutputSelector,
        selectorPolicy.showChannelsAsStereoPairs,
        selectorPolicy.hideAdvancedOptionsWithButton);
    addAndMakeVisible(*deviceSelector_);

    testButton_ = DialogMatrixHelpers::makeButton(
        *skin_,
        DialogMatrixHelpers::kDefaultButtonWidth,
        PluginDisplayNames::Dialogs::AudioMidiSettings::kTestButton);
    testButton_->onClick = [this] { playTestSound(); };
    addAndMakeVisible(*testButton_);

    peakIndicator_ = std::make_unique<TSS::PeakIndicator>(kPeakWidth_, kPeakHeight_);
    peakIndicator_->setSkin(*skin_);
    addAndMakeVisible(*peakIndicator_);

    syncPreferredSetupFromDeviceManager();
    deviceManager_.addChangeListener(this);
    startTimerHz(30);
}

AudioMidiSettingsWindow::~AudioMidiSettingsWindow()
{
    stopTimer();
    deviceManager_.removeChangeListener(this);
}

void AudioMidiSettingsWindow::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    closeButton_->setSkin(skin);
    DialogMatrixHelpers::applyButtonSkin(*testButton_, skin);
    peakIndicator_->setSkin(skin);
    repaint();
}

void AudioMidiSettingsWindow::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    closeButton_->setUiScale(uiScale);
    DialogMatrixHelpers::applyButtonUiScale(*testButton_, uiScale);
    peakIndicator_->setUiScale(uiScale);
    resized();
    repaint();
}

int AudioMidiSettingsWindow::getBorderThickness() const
{
    return juce::roundToInt(static_cast<float>(DialogMatrixHelpers::kBorderThickness) * uiScale_);
}

juce::Rectangle<int> AudioMidiSettingsWindow::getDialogBounds() const
{
    const int border = getBorderThickness();
    const int dialogWidth = juce::roundToInt(static_cast<float>(kDesignWidth_) * uiScale_) + border * 2;
    const int dialogHeight = juce::roundToInt(static_cast<float>(kDesignContentHeight_) * uiScale_)
                             + juce::roundToInt(static_cast<float>(DialogMatrixHelpers::kTitleBarHeight) * uiScale_)
                             + juce::roundToInt(static_cast<float>(kFooterControlsHeight_) * uiScale_)
                             + border * 2;
    return getLocalBounds().withSizeKeepingCentre(dialogWidth, dialogHeight);
}

void AudioMidiSettingsWindow::paint(juce::Graphics& g)
{
    const auto dialogBounds = getDialogBounds();
    const int border = getBorderThickness();
    const int titleBarHeight = juce::roundToInt(static_cast<float>(DialogMatrixHelpers::kTitleBarHeight) * uiScale_);

    DialogMatrixHelpers::paintMatrixOverlayChrome({
        .g = g,
        .skin = *skin_,
        .dialogBounds = dialogBounds,
        .borderThickness = border,
        .titleBarHeight = titleBarHeight,
        .title = PluginDisplayNames::Dialogs::AudioMidiSettings::kTitle,
        .uiScale = uiScale_ });
}

void AudioMidiSettingsWindow::resized()
{
    auto inner = getDialogBounds().reduced(getBorderThickness());
    const int titleBarHeight = juce::roundToInt(static_cast<float>(DialogMatrixHelpers::kTitleBarHeight) * uiScale_);
    const int closeButtonWidth = juce::roundToInt(static_cast<float>(titleBarHeight) * 1.2f);
    const int footerHeight = juce::roundToInt(static_cast<float>(kFooterControlsHeight_) * uiScale_);
    const int padding = juce::roundToInt(12.0f * uiScale_);

    auto titleBar = inner.removeFromTop(titleBarHeight);
    closeButton_->setBounds(titleBar.removeFromRight(closeButtonWidth));

    auto footer = inner.removeFromBottom(footerHeight).reduced(padding, juce::roundToInt(8.0f * uiScale_));
    deviceSelector_->setBounds(inner);

    const int buttonHeight = juce::roundToInt(
        static_cast<float>(DialogMatrixHelpers::kDefaultButtonHeight) * uiScale_);
    const int buttonWidth = juce::roundToInt(
        static_cast<float>(DialogMatrixHelpers::kDefaultButtonWidth) * uiScale_);
    const int peakWidth = juce::roundToInt(static_cast<float>(kPeakWidth_) * uiScale_);
    const int peakHeight = juce::roundToInt(static_cast<float>(kPeakHeight_) * uiScale_);
    const int gap = juce::roundToInt(12.0f * uiScale_);

    auto peakBounds = footer.removeFromRight(peakWidth).withSizeKeepingCentre(peakWidth, peakHeight);
    peakIndicator_->setBounds(peakBounds);
    footer.removeFromRight(gap);
    testButton_->setBounds(footer.removeFromRight(buttonWidth).withSizeKeepingCentre(buttonWidth, buttonHeight));
}

void AudioMidiSettingsWindow::mouseDown(const juce::MouseEvent& e)
{
    if (! getDialogBounds().contains(e.getPosition()) && onCloseRequested_)
        onCloseRequested_();
}

bool AudioMidiSettingsWindow::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        if (onCloseRequested_)
            onCloseRequested_();
        return true;
    }

    return Component::keyPressed(key);
}

void AudioMidiSettingsWindow::timerCallback()
{
    if (peakLevelProvider_)
        peakIndicator_->setLevel(peakLevelProvider_());
}

void AudioMidiSettingsWindow::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    juce::ignoreUnused(source);

    if (restoringSetup_)
        return;

    syncPreferredSetupFromDeviceManager();
}

void AudioMidiSettingsWindow::syncPreferredSetupFromDeviceManager()
{
    auto setup = deviceManager_.getAudioDeviceSetup();
    auto* device = deviceManager_.getCurrentAudioDevice();
    const auto currentIdentity = identityFromSetup(setup);
    LiveSetupMutation live { deviceManager_, setup, device, restoringSetup_ };
    bool restoredFromProfile = false;

    if (Core::shouldRestoreAudioDeviceProfile(hasSeededDeviceIdentity_,
                                              lastDeviceIdentity_,
                                              currentIdentity))
    {
        const auto key = profileKeyFromLiveSetup(deviceManager_, setup, device);
        auto candidate = setup;
        if (tryRestoreMatchingProfile(key, device, candidate))
        {
            if (applySetupWithRestoreGuard(live, candidate))
                restoredFromProfile = true;

            refreshSetupAfterApply(live);
        }
    }

    if (! restoredFromProfile)
    {
        const auto plan = Core::planPreferredSetupChange({
            .previousIdentity = lastDeviceIdentity_,
            .currentIdentity = currentIdentity,
            .preferred = preferred_,
            .liveSampleRate = setup.sampleRate,
            .liveBufferSize = setup.bufferSize,
            .preferredRateSupported = deviceSupportsSampleRate(device, preferred_.sampleRate),
            .preferredBufferSupported = deviceSupportsBufferSize(device, preferred_.bufferSize),
        });
        applyPreferredOverlayIfNeeded(live, plan);
        preferred_ = plan.preferredAfterCapture;
    }
    else
    {
        preferred_ = Core::capturePreferredSetup(preferred_, setup.sampleRate, setup.bufferSize);
    }

    lastDeviceIdentity_ = currentIdentity;
    hasSeededDeviceIdentity_ = true;
    captureLiveSetupAsProfile(deviceManager_, setup, device);
}

void AudioMidiSettingsWindow::playTestSound()
{
    deviceManager_.playTestSound();
}
