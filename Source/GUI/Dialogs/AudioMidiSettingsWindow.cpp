#include "AudioMidiSettingsWindow.h"

#include "Core/Audio/AudioDevicePreferredSetup.h"
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

    const auto plan = Core::planPreferredSetupChange({
        .previousIdentity = lastDeviceIdentity_,
        .currentIdentity = currentIdentity,
        .preferred = preferred_,
        .liveSampleRate = setup.sampleRate,
        .liveBufferSize = setup.bufferSize,
        .preferredRateSupported = deviceSupportsSampleRate(device, preferred_.sampleRate),
        .preferredBufferSupported = deviceSupportsBufferSize(device, preferred_.bufferSize),
    });

    if (plan.shouldRestore)
    {
        setup.sampleRate = plan.sampleRateToApply;
        setup.bufferSize = plan.bufferSizeToApply;
        restoringSetup_ = true;
        deviceManager_.setAudioDeviceSetup(setup, true);
        restoringSetup_ = false;
    }

    preferred_ = plan.preferredAfterCapture;
    lastDeviceIdentity_ = plan.identityToStore;
}

void AudioMidiSettingsWindow::playTestSound()
{
    deviceManager_.playTestSound();
}
