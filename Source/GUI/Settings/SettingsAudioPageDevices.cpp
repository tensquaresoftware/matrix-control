#include "SettingsAudioPage.h"

#include "GUI/Settings/AudioDeviceSetupSync.h"
#include "GUI/Widgets/RadioButtonGroupLayout.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace
{
    int deviceComboIdForName(const juce::StringArray& names, const juce::String& deviceName)
    {
        if (deviceName.isEmpty())
            return 1;

        const int index = names.indexOf(deviceName);
        if (index >= 0)
            return index + 2;

        // Keep a visible item for the live setup name when the scan omits it.
        return names.size() + 2;
    }

    void populateDeviceCombo(TSS::ComboBox& combo,
                             const juce::StringArray& names,
                             const juce::String& selectedName)
    {
        combo.clear(juce::dontSendNotification);
        combo.addItem(PluginDisplayNames::Settings::kNoDeviceSentinel, 1);
        for (int i = 0; i < names.size(); ++i)
            combo.addItem(names[i].toUpperCase(), i + 2);

        if (selectedName.isNotEmpty() && ! names.contains(selectedName))
            combo.addItem(selectedName.toUpperCase(), names.size() + 2);

        combo.setSelectedId(deviceComboIdForName(names, selectedName), juce::dontSendNotification);
    }
}

void SettingsAudioPage::wireChannelPairComboCallbacks()
{
    const auto wireOne = [this](TSS::ComboBox& combo, bool isInput)
    {
        combo.onChange = [this, &combo, isInput]
        {
            if (updatingUi_)
                return;
            const int id = combo.getSelectedId();
            if (TSS::RadioButtonGroupLayout::isChannelPairComboItemId(id))
                applyChannelPair(isInput,
                                 TSS::RadioButtonGroupLayout::channelPairIndexFromComboItemId(id));
        };
    };
    wireOne(*inputChannelsCombo_, true);
    wireOne(*outputChannelsCombo_, false);
}

void SettingsAudioPage::refreshAllFromDeviceManager()
{
    const juce::ScopedValueSetter<bool> guard(updatingUi_, true);
    refreshDriverTypeCombo();
    refreshDeviceCombos();
    refreshSampleRateAndBufferCombos();
    refreshChannelCombos();
    resized();
}

void SettingsAudioPage::refreshDriverTypeCombo()
{
    driverTypeCombo_->clear(juce::dontSendNotification);
    const auto& types = deviceManager_.getAvailableDeviceTypes();
    const auto current = deviceManager_.getCurrentAudioDeviceType();
    int selectedId = 0;
    for (int i = 0; i < types.size(); ++i)
    {
        const int id = i + 1;
        driverTypeCombo_->addItem(types.getUnchecked(i)->getTypeName().toUpperCase(), id);
        if (types.getUnchecked(i)->getTypeName() == current)
            selectedId = id;
    }
    if (selectedId > 0)
        driverTypeCombo_->setSelectedId(selectedId, juce::dontSendNotification);
}

void SettingsAudioPage::refreshDeviceCombos()
{
    auto* type = deviceManager_.getCurrentDeviceTypeObject();
    inputDeviceCombo_->clear(juce::dontSendNotification);
    outputDeviceCombo_->clear(juce::dontSendNotification);
    if (type == nullptr)
        return;

    type->scanForDevices();
    const auto inputs = type->getDeviceNames(true);
    const auto outputs = type->getDeviceNames(false);
    const auto setup = deviceManager_.getAudioDeviceSetup();

    populateDeviceCombo(*inputDeviceCombo_, inputs, setup.inputDeviceName);
    populateDeviceCombo(*outputDeviceCombo_, outputs, setup.outputDeviceName);
}

void SettingsAudioPage::refreshSampleRateAndBufferCombos()
{
    sampleRateCombo_->clear(juce::dontSendNotification);
    bufferSizeCombo_->clear(juce::dontSendNotification);
    sampleRateValues_.clear();
    bufferSizeValues_.clear();

    auto* device = deviceManager_.getCurrentAudioDevice();
    const auto setup = deviceManager_.getAudioDeviceSetup();
    if (device == nullptr)
        return;

    int selectedRateId = 0;
    for (const auto rate : device->getAvailableSampleRates())
    {
        sampleRateValues_.add(juce::String(rate, 0));
        const int id = sampleRateValues_.size();
        sampleRateCombo_->addItem(sampleRateValues_[id - 1], id);
        if (juce::approximatelyEqual(rate, setup.sampleRate))
            selectedRateId = id;
    }
    if (selectedRateId > 0)
        sampleRateCombo_->setSelectedId(selectedRateId, juce::dontSendNotification);

    int selectedBufferId = 0;
    for (const auto size : device->getAvailableBufferSizes())
    {
        bufferSizeValues_.add(size);
        const int id = bufferSizeValues_.size();
        bufferSizeCombo_->addItem(juce::String(size), id);
        if (size == setup.bufferSize)
            selectedBufferId = id;
    }
    if (selectedBufferId > 0)
        bufferSizeCombo_->setSelectedId(selectedBufferId, juce::dontSendNotification);
}

void SettingsAudioPage::refreshChannelCombos()
{
    auto* device = deviceManager_.getCurrentAudioDevice();
    const auto setup = deviceManager_.getAudioDeviceSetup();
    const int inPairs = device != nullptr
        ? TSS::RadioButtonGroupLayout::stereoPairCount(device->getInputChannelNames().size())
        : 0;
    const int outPairs = device != nullptr
        ? TSS::RadioButtonGroupLayout::stereoPairCount(device->getOutputChannelNames().size())
        : 0;

    const auto populatePairCombo = [](TSS::ComboBox& combo, int pairCount, int selectedPair)
    {
        combo.clear(juce::dontSendNotification);
        for (int i = 0; i < pairCount; ++i)
            combo.addItem(TSS::RadioButtonGroupLayout::stereoPairLabel(i),
                          TSS::RadioButtonGroupLayout::channelPairComboItemId(i));

        combo.setEnabled(pairCount > 0);
        if (selectedPair >= 0 && selectedPair < pairCount)
        {
            combo.setSelectedId(TSS::RadioButtonGroupLayout::channelPairComboItemId(selectedPair),
                                juce::dontSendNotification);
        }
    };

    populatePairCombo(*inputChannelsCombo_,
                      inPairs,
                      TSS::RadioButtonGroupLayout::selectedStereoPairIndex(setup.inputChannels, inPairs));
    populatePairCombo(*outputChannelsCombo_,
                      outPairs,
                      TSS::RadioButtonGroupLayout::selectedStereoPairIndex(setup.outputChannels, outPairs));
}

namespace
{
    juce::String selectedDeviceName(const TSS::ComboBox& combo,
                                    const juce::StringArray& scannedNames,
                                    const juce::String& liveSetupName)
    {
        const int id = combo.getSelectedId();
        if (id <= 1)
            return {};

        if (id >= 2 && id - 2 < scannedNames.size())
            return scannedNames[id - 2];

        // Orphan combo item is display-uppercased; keep the live setup spelling for apply.
        // Unexpected ids (stale selection after a scan shrink) also keep the live name
        // instead of clearing the endpoint or applying uppercased display text.
        return liveSetupName;
    }

}

void SettingsAudioPage::applySetupFromUi()
{
    auto setup = deviceManager_.getAudioDeviceSetup();
    auto* type = deviceManager_.getCurrentDeviceTypeObject();
    if (type == nullptr)
        return;

    juce::String inputName = selectedDeviceName(*inputDeviceCombo_,
                                                type->getDeviceNames(true),
                                                setup.inputDeviceName);
    juce::String outputName = selectedDeviceName(*outputDeviceCombo_,
                                                 type->getDeviceNames(false),
                                                 setup.outputDeviceName);

    AudioDeviceSetupSync::resolveEndpointNamesForApply(
        deviceManager_.getCurrentAudioDeviceType(),
        inputName,
        outputName,
        pendingEndpointChange_ == EndpointChange::kOutput);
    pendingEndpointChange_ = EndpointChange::kNone;

    const bool inputDeviceChanged = setup.inputDeviceName != inputName;
    const bool outputDeviceChanged = setup.outputDeviceName != outputName;
    setup.inputDeviceName = inputName;
    setup.outputDeviceName = outputName;
    setup.useDefaultInputChannels = false;
    setup.useDefaultOutputChannels = false;

    if (inputDeviceChanged)
        AudioDeviceSetupSync::applyStereoPairToSetup(setup, true, 0);
    if (outputDeviceChanged)
        AudioDeviceSetupSync::applyStereoPairToSetup(setup, false, 0);

    const int rateId = sampleRateCombo_->getSelectedId();
    if (rateId > 0 && rateId <= sampleRateValues_.size())
        setup.sampleRate = sampleRateValues_[rateId - 1].getDoubleValue();

    const int bufferId = bufferSizeCombo_->getSelectedId();
    if (bufferId > 0 && bufferId <= bufferSizeValues_.size())
        setup.bufferSize = bufferSizeValues_[bufferId - 1];

    deviceManager_.setAudioDeviceSetup(setup, true);
    AudioDeviceSetupSync::syncPreferredSetupFromDeviceManager(deviceManager_, syncState_);
    refreshAllFromDeviceManager();
}

void SettingsAudioPage::applyChannelPair(bool isInput, int pairIndex)
{
    if (pairIndex < 0)
        return;

    auto setup = deviceManager_.getAudioDeviceSetup();
    AudioDeviceSetupSync::applyStereoPairToSetup(setup, isInput, pairIndex);
    deviceManager_.setAudioDeviceSetup(setup, true);
    AudioDeviceSetupSync::syncPreferredSetupFromDeviceManager(deviceManager_, syncState_);
    refreshAllFromDeviceManager();
}

void SettingsAudioPage::playTestSound()
{
    deviceManager_.playTestSound();
}
