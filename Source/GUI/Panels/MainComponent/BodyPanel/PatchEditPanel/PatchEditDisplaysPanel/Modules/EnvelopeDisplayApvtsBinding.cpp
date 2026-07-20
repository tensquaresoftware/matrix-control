#include "EnvelopeDisplayApvtsBinding.h"

#include "GUI/Widgets/EnvelopeDisplay.h"

EnvelopeDisplayApvtsBinding::EnvelopeDisplayApvtsBinding(juce::AudioProcessorValueTreeState& apvts,
                                                         TSS::EnvelopeDisplay& display,
                                                         const ParameterIds& parameterIds)
    : apvts_(apvts)
    , display_(display)
    , parameterIds_(parameterIds)
{
    for (const char* parameterId : parameterIds_)
        apvts_.addParameterListener(parameterId, this);

    connectDisplayCallbacks();
    syncAllFromApvts();
}

EnvelopeDisplayApvtsBinding::~EnvelopeDisplayApvtsBinding()
{
    endParameterGesture();

    display_.setOnValueChanged(nullptr);
    display_.setOnEditGestureBegin(nullptr);
    display_.setOnEditGestureEnd(nullptr);

    for (const char* parameterId : parameterIds_)
        apvts_.removeParameterListener(parameterId, this);
}

void EnvelopeDisplayApvtsBinding::connectDisplayCallbacks()
{
    display_.setOnValueChanged([this](int paramIndex, int newValue)
    {
        if (paramIndex < 0 || paramIndex >= kParameterCount)
            return;

        writeIntParameter(parameterIds_[static_cast<size_t>(paramIndex)], newValue);
    });

    display_.setOnEditGestureBegin([this](int paramIndex)
    {
        if (paramIndex < 0 || paramIndex >= kParameterCount)
            return;

        beginParameterGesture(parameterIds_[static_cast<size_t>(paramIndex)]);
    });

    display_.setOnEditGestureEnd([this]
    {
        endParameterGesture();
    });
}

void EnvelopeDisplayApvtsBinding::syncAllFromApvts()
{
    for (int paramIndex = 0; paramIndex < kParameterCount; ++paramIndex)
    {
        applyDisplayValue(
            paramIndex,
            readIntParameter(parameterIds_[static_cast<size_t>(paramIndex)]));
    }
}

int EnvelopeDisplayApvtsBinding::readIntParameter(const juce::String& parameterId) const
{
    auto* param = apvts_.getParameter(parameterId);
    if (param == nullptr)
        return 0;

    if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
    {
        return juce::jlimit(
            0,
            kIntParameterMax,
            juce::roundToInt(rangedParam->convertFrom0to1(param->getValue())));
    }

    return juce::jlimit(
        0,
        kIntParameterMax,
        juce::roundToInt(param->getValue() * static_cast<float>(kIntParameterMax)));
}

void EnvelopeDisplayApvtsBinding::writeIntParameter(const juce::String& parameterId, int value)
{
    auto* param = apvts_.getParameter(parameterId);
    if (param == nullptr)
        return;

    const int clampedValue = juce::jlimit(0, kIntParameterMax, value);

    if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
        param->setValueNotifyingHost(rangedParam->convertTo0to1(static_cast<float>(clampedValue)));
    else
        param->setValueNotifyingHost(static_cast<float>(clampedValue) / static_cast<float>(kIntParameterMax));
}

void EnvelopeDisplayApvtsBinding::beginParameterGesture(const juce::String& parameterId)
{
    endParameterGesture();

    if (auto* param = apvts_.getParameter(parameterId))
    {
        param->beginChangeGesture();
        activeGestureParameter_ = param;
        activeGestureGate_->store(param);
    }
}

void EnvelopeDisplayApvtsBinding::endParameterGesture()
{
    if (activeGestureParameter_ == nullptr)
        return;

    activeGestureParameter_->endChangeGesture();
    activeGestureParameter_ = nullptr;
    activeGestureGate_->store(nullptr);
}

int EnvelopeDisplayApvtsBinding::indexForParameterId(const juce::String& parameterId) const
{
    for (int paramIndex = 0; paramIndex < kParameterCount; ++paramIndex)
    {
        if (parameterId == parameterIds_[static_cast<size_t>(paramIndex)])
            return paramIndex;
    }

    return -1;
}

void EnvelopeDisplayApvtsBinding::applyDisplayValue(int paramIndex, int value)
{
    switch (paramIndex)
    {
        case 0: display_.setDelay(value, false); break;
        case 1: display_.setAttack(value, false); break;
        case 2: display_.setDecay(value, false); break;
        case 3: display_.setSustain(value, false); break;
        case 4: display_.setRelease(value, false); break;
        default: break;
    }
}

void EnvelopeDisplayApvtsBinding::parameterChanged(const juce::String& parameterId, float newValue)
{
    juce::ignoreUnused(newValue);

    const int paramIndex = indexForParameterId(parameterId);
    if (paramIndex < 0)
        return;

    auto* param = apvts_.getParameter(parameterId);
    if (param != nullptr && param == activeGestureGate_->load())
        return;

    const int value = readIntParameter(parameterId);
    auto safeDisplay = juce::Component::SafePointer<TSS::EnvelopeDisplay>(&display_);
    const auto activeGestureGate = activeGestureGate_;

    juce::MessageManager::callAsync(
        [safeDisplay, paramIndex, value, activeGestureGate, param]
        {
            if (safeDisplay == nullptr)
                return;

            // Drop stale async work queued before a drag started, or echoed during it.
            if (param != nullptr && param == activeGestureGate->load())
                return;

            switch (paramIndex)
            {
                case 0: safeDisplay->setDelay(value, false); break;
                case 1: safeDisplay->setAttack(value, false); break;
                case 2: safeDisplay->setDecay(value, false); break;
                case 3: safeDisplay->setSustain(value, false); break;
                case 4: safeDisplay->setRelease(value, false); break;
                default: break;
            }
        });
}
