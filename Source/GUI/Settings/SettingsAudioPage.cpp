#include "SettingsAudioPage.h"

#include "GUI/Helpers/AudioFromComboItemSet.h"
#include "GUI/Helpers/ComboBoxLiveRefresh.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Settings/SettingsShellMetrics.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Widgets/RadioButtonGroupLayout.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace
{
    [[nodiscard]] bool synthFromItemSetUnchanged(const TSS::ComboBox& combo,
                                                 const std::vector<juce::String>& currentIds,
                                                 const std::vector<juce::String>& nextIds,
                                                 const juce::StringArray& channelNames)
    {
        TSS::AudioFromComboItemSet::ComboTexts texts;
        texts.numItems = combo.getNumItems();
        texts.itemTexts.reserve(static_cast<size_t>(texts.numItems));
        for (int i = 0; i < texts.numItems; ++i)
            texts.itemTexts.push_back(combo.getItemText(i));

        return TSS::AudioFromComboItemSet::itemSetUnchanged(
            currentIds, nextIds, channelNames, texts);
    }
}

SettingsAudioPage::SettingsAudioPage(Config config)
    : config_(std::move(config))
    , skin_(config_.skin)
    , deviceManager_(*config_.deviceManager)
{
    jassert(config_.skin != nullptr);
    jassert(config_.deviceManager != nullptr);

    setOpaque(false);
    buildWidgets();
    wireCallbacks();
    AudioDeviceSetupSync::syncPreferredSetupFromDeviceManager(deviceManager_, syncState_);
    refreshAllFromDeviceManager();
    setMonitoringActive(true);
}

void SettingsAudioPage::buildWidgets()
{
    driverTypeLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kDriverTypeLabel);
    driverTypeCombo_ = makeCombo(*skin_);
    inputDeviceLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kInputDeviceLabel);
    inputDeviceCombo_ = makeCombo(*skin_);
    outputDeviceLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kOutputDeviceLabel);
    outputDeviceCombo_ = makeCombo(*skin_);
    sampleRateLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kSampleRateLabel);
    sampleRateCombo_ = makeCombo(*skin_);
    bufferSizeLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kBufferSizeLabel);
    bufferSizeCombo_ = makeCombo(*skin_);
    inputChannelsLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kInputChannelsLabel);
    inputChannelsGroup_ = std::make_unique<TSS::RadioButtonGroup>();
    inputChannelsGroup_->setSkin(*skin_);
    addAndMakeVisible(*inputChannelsGroup_);
    synthFromLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kSynthFromLabel);
    synthFromCombo_ = makeCombo(*skin_);
    synthFromCombo_->setUsesPortSentinelPopupChrome(true);
    peakIndicator_ = std::make_unique<TSS::PeakIndicator>(SettingsShellMetrics::kPeakWidth,
                                                          SettingsShellMetrics::kControlHeight);
    peakIndicator_->setSkin(*skin_);
    addAndMakeVisible(*peakIndicator_);
    outputChannelsLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kOutputChannelsLabel);
    outputChannelsGroup_ = std::make_unique<TSS::RadioButtonGroup>();
    outputChannelsGroup_->setSkin(*skin_);
    addAndMakeVisible(*outputChannelsGroup_);
    playTestToneButton_ = std::make_unique<TSS::Button>(
        SettingsShellMetrics::kControlColumnWidth,
        SettingsShellMetrics::kControlHeight,
        TSS::buttonLookFromSkin(*skin_),
        PluginDisplayNames::Settings::kPlayTestToneButton);
    addAndMakeVisible(*playTestToneButton_);
}

void SettingsAudioPage::wireCallbacks()
{
    driverTypeCombo_->onChange = [this]
    {
        if (updatingUi_)
            return;
        deviceManager_.setCurrentAudioDeviceType(driverTypeCombo_->getText(), true);
        refreshAllFromDeviceManager();
    };
    inputDeviceCombo_->onChange = [this]
    {
        if (updatingUi_)
            return;
        pendingEndpointChange_ = EndpointChange::kInput;
        applySetupFromUi();
    };
    outputDeviceCombo_->onChange = [this]
    {
        if (updatingUi_)
            return;
        pendingEndpointChange_ = EndpointChange::kOutput;
        applySetupFromUi();
    };
    sampleRateCombo_->onChange = [this]
    {
        if (! updatingUi_)
            applySetupFromUi();
    };
    bufferSizeCombo_->onChange = [this]
    {
        if (! updatingUi_)
            applySetupFromUi();
    };
    inputChannelsGroup_->onSelectionChanged = [this]
    {
        if (! updatingUi_)
            applyChannelPair(true, inputChannelsGroup_->getSelectedIndex());
    };
    outputChannelsGroup_->onSelectionChanged = [this]
    {
        if (! updatingUi_)
            applyChannelPair(false, outputChannelsGroup_->getSelectedIndex());
    };
    playTestToneButton_->onClick = [this] { playTestSound(); };
    synthFromCombo_->onChange = [this]
    {
        if (! updatingUi_ && config_.onSynthFromChanged)
            config_.onSynthFromChanged();
    };
}

SettingsAudioPage::~SettingsAudioPage()
{
    setMonitoringActive(false);
}

void SettingsAudioPage::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    const auto labelLook = TSS::labelLookFromSkin(skin);
    const auto comboLook = TSS::comboBoxLookFromSkin(skin);
    const auto popupLook = TSS::popupMenuLookFromSkin(skin);
    const auto buttonLook = TSS::buttonLookFromSkin(skin);

    driverTypeLabel_->setLook(labelLook);
    inputDeviceLabel_->setLook(labelLook);
    outputDeviceLabel_->setLook(labelLook);
    sampleRateLabel_->setLook(labelLook);
    bufferSizeLabel_->setLook(labelLook);
    inputChannelsLabel_->setLook(labelLook);
    synthFromLabel_->setLook(labelLook);
    outputChannelsLabel_->setLook(labelLook);

    for (auto* combo : { driverTypeCombo_.get(), inputDeviceCombo_.get(), outputDeviceCombo_.get(),
                         sampleRateCombo_.get(), bufferSizeCombo_.get(), synthFromCombo_.get() })
    {
        combo->setLook(comboLook);
        combo->setPopupMenuLook(popupLook);
    }

    inputChannelsGroup_->setSkin(skin);
    outputChannelsGroup_->setSkin(skin);
    peakIndicator_->setSkin(skin);
    playTestToneButton_->setLook(buttonLook);
    repaint();
}

void SettingsAudioPage::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    inputChannelsGroup_->setUiScale(uiScale);
    outputChannelsGroup_->setUiScale(uiScale);
    peakIndicator_->setUiScale(uiScale);
    resized();
    repaint();
}

void SettingsAudioPage::setVisible(bool shouldBeVisible)
{
    Component::setVisible(shouldBeVisible);
    setMonitoringActive(shouldBeVisible);
    if (shouldBeVisible)
        refreshAllFromDeviceManager();
}

void SettingsAudioPage::setMonitoringActive(bool shouldBeActive)
{
    if (shouldBeActive == monitoringActive_)
        return;

    monitoringActive_ = shouldBeActive;
    if (monitoringActive_)
    {
        deviceManager_.addChangeListener(this);
        startTimerHz(30);
        return;
    }

    stopTimer();
    deviceManager_.removeChangeListener(this);
}

void SettingsAudioPage::registerContextualHelp(TSS::ContextualHelpBinder& binder)
{
    namespace Help = PluginDisplayNames::Settings::ContextualHelp;
    binder.bind(driverTypeCombo_.get(), Help::kDriverType);
    binder.bind(inputDeviceCombo_.get(), Help::kInputDevice);
    binder.bind(outputDeviceCombo_.get(), Help::kOutputDevice);
    binder.bind(sampleRateCombo_.get(), Help::kSampleRate);
    binder.bind(bufferSizeCombo_.get(), Help::kBufferSize);
    binder.bind(inputChannelsGroup_.get(), Help::kInputChannels);
    binder.bind(synthFromCombo_.get(), Help::kSynthFrom);
    binder.bind(peakIndicator_.get(), Help::kAudioPeakIndicator);
    binder.bind(outputChannelsGroup_.get(), Help::kOutputChannels);
    binder.bind(playTestToneButton_.get(), Help::kPlayTestTone);
}

void SettingsAudioPage::paint(juce::Graphics&)
{
}

void SettingsAudioPage::timerCallback()
{
    if (config_.peakLevelProvider)
        peakIndicator_->setLevel(config_.peakLevelProvider());
}

void SettingsAudioPage::changeListenerCallback(juce::ChangeBroadcaster*)
{
    if (syncState_.restoringSetup || updatingUi_)
        return;

    AudioDeviceSetupSync::syncPreferredSetupFromDeviceManager(deviceManager_, syncState_);
    refreshAllFromDeviceManager();
}


void SettingsAudioPage::populateSynthFromCombo(const juce::StringArray& channelNames,
                                              const juce::StringArray& channelIds)
{
    const auto previousSourceId = getSelectedSynthFromSourceId();
    const int count = juce::jmin(channelNames.size(), channelIds.size());
    std::vector<juce::String> nextIds;
    nextIds.reserve(static_cast<size_t>(count));
    for (int i = 0; i < count; ++i)
        nextIds.push_back(channelIds[i]);

    const bool itemSetUnchanged = synthFromItemSetUnchanged(
        *synthFromCombo_, synthFromSourceIdentifiers_, nextIds, channelNames);
    const auto action = TSS::ComboBoxLiveRefresh::planRefresh(
        synthFromCombo_->isPopupOpen(), itemSetUnchanged);

    if (action == TSS::ComboBoxLiveRefresh::Action::kSkipRebuild)
    {
        if (count == 0)
            synthFromCombo_->setSelectedId(kPortSentinelItemId, juce::dontSendNotification);
        else
            selectSynthFromSourceId(previousSourceId);
        return;
    }

    const auto rebuild = [this, &channelNames, &nextIds, count, &previousSourceId]()
    {
        const juce::ScopedValueSetter<bool> guard(updatingUi_, true);
        synthFromCombo_->clear(juce::dontSendNotification);
        synthFromSourceIdentifiers_ = nextIds;
        synthFromCombo_->addItem(PluginDisplayNames::HeaderPanel::kNoInputSentinel, kPortSentinelItemId);
        for (int i = 0; i < count; ++i)
            synthFromCombo_->addItem(channelNames[i].toUpperCase(), i + kFirstDeviceItemId);
        if (count == 0)
            synthFromCombo_->setSelectedId(kPortSentinelItemId, juce::dontSendNotification);
        else
            selectSynthFromSourceId(previousSourceId);
    };

    if (action == TSS::ComboBoxLiveRefresh::Action::kDismissRebuildReopen)
        TSS::ComboBoxLiveRefresh::rebuildPreservingOpenPopup(*synthFromCombo_, rebuild);
    else
        rebuild();
}

juce::String SettingsAudioPage::getSelectedSynthFromSourceId() const
{
    const int itemId = synthFromCombo_->getSelectedId();
    if (itemId < kFirstDeviceItemId)
        return {};

    const auto index = static_cast<size_t>(itemId - kFirstDeviceItemId);
    if (index >= synthFromSourceIdentifiers_.size())
        return {};

    return synthFromSourceIdentifiers_[index];
}

void SettingsAudioPage::selectSynthFromSourceId(const juce::String& sourceId)
{
    const juce::ScopedValueSetter<bool> guard(updatingUi_, true);
    if (sourceId.isEmpty())
    {
        synthFromCombo_->setSelectedId(kPortSentinelItemId, juce::dontSendNotification);
        return;
    }

    for (size_t i = 0; i < synthFromSourceIdentifiers_.size(); ++i)
    {
        if (synthFromSourceIdentifiers_[i] == sourceId)
        {
            synthFromCombo_->setSelectedId(static_cast<int>(i) + kFirstDeviceItemId,
                                           juce::dontSendNotification);
            return;
        }
    }

    synthFromCombo_->setSelectedId(kPortSentinelItemId, juce::dontSendNotification);
}

std::unique_ptr<TSS::Label> SettingsAudioPage::makeLabel(TSS::ISkin& skin, const juce::String& text)
{
    auto label = std::make_unique<TSS::Label>(
        SettingsShellMetrics::kLabelWidth,
        SettingsShellMetrics::kControlHeight,
        TSS::labelLookFromSkin(skin),
        text);
    addAndMakeVisible(*label);
    return label;
}

std::unique_ptr<TSS::ComboBox> SettingsAudioPage::makeCombo(TSS::ISkin& skin)
{
    auto combo = std::make_unique<TSS::ComboBox>(
        SettingsShellMetrics::kControlColumnWidth,
        SettingsShellMetrics::kControlHeight,
        TSS::comboBoxLookFromSkin(skin),
        TSS::ComboBox::Style::ButtonLike);
    combo->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    addAndMakeVisible(*combo);
    return combo;
}
