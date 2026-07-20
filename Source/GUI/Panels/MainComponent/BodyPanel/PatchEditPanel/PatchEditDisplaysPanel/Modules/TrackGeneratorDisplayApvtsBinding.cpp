#include "TrackGeneratorDisplayApvtsBinding.h"

#include "GUI/Widgets/TrackGeneratorDisplay.h"

TrackGeneratorDisplayApvtsBinding::TrackGeneratorDisplayApvtsBinding(
    juce::AudioProcessorValueTreeState& apvts,
    TSS::TrackGeneratorDisplay& display,
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

TrackGeneratorDisplayApvtsBinding::~TrackGeneratorDisplayApvtsBinding()
{
    endParameterGesture();

    display_.setOnValueChanged(nullptr);
    display_.setOnEditGestureBegin(nullptr);
    display_.setOnEditGestureEnd(nullptr);

    for (const char* parameterId : parameterIds_)
        apvts_.removeParameterListener(parameterId, this);
}

void TrackGeneratorDisplayApvtsBinding::connectDisplayCallbacks()
{
    display_.setOnValueChanged([this](int pointIndex, int newValue)
    {
        if (pointIndex < 0 || pointIndex >= kParameterCount)
            return;

        writeIntParameter(parameterIds_[static_cast<size_t>(pointIndex)], newValue);
    });

    display_.setOnEditGestureBegin([this](int pointIndex)
    {
        if (pointIndex < 0 || pointIndex >= kParameterCount)
            return;

        beginParameterGesture(parameterIds_[static_cast<size_t>(pointIndex)]);
    });

    display_.setOnEditGestureEnd([this]
    {
        endParameterGesture();
    });
}

void TrackGeneratorDisplayApvtsBinding::syncAllFromApvts()
{
    for (int pointIndex = 0; pointIndex < kParameterCount; ++pointIndex)
    {
        applyDisplayValue(
            pointIndex,
            readIntParameter(parameterIds_[static_cast<size_t>(pointIndex)]));
    }
}

int TrackGeneratorDisplayApvtsBinding::readIntParameter(const juce::String& parameterId) const
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

void TrackGeneratorDisplayApvtsBinding::writeIntParameter(const juce::String& parameterId, int value)
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

void TrackGeneratorDisplayApvtsBinding::beginParameterGesture(const juce::String& parameterId)
{
    endParameterGesture();

    if (auto* param = apvts_.getParameter(parameterId))
    {
        param->beginChangeGesture();
        activeGestureParameter_ = param;
        activeGestureGate_->store(param);
    }
}

void TrackGeneratorDisplayApvtsBinding::endParameterGesture()
{
    if (activeGestureParameter_ == nullptr)
        return;

    activeGestureParameter_->endChangeGesture();
    activeGestureParameter_ = nullptr;
    activeGestureGate_->store(nullptr);
}

int TrackGeneratorDisplayApvtsBinding::indexForParameterId(const juce::String& parameterId) const
{
    for (int pointIndex = 0; pointIndex < kParameterCount; ++pointIndex)
    {
        if (parameterId == parameterIds_[static_cast<size_t>(pointIndex)])
            return pointIndex;
    }

    return -1;
}

void TrackGeneratorDisplayApvtsBinding::applyDisplayValue(int pointIndex, int value)
{
    switch (pointIndex)
    {
        case 0: display_.setTrackPoint1(value, false); break;
        case 1: display_.setTrackPoint2(value, false); break;
        case 2: display_.setTrackPoint3(value, false); break;
        case 3: display_.setTrackPoint4(value, false); break;
        case 4: display_.setTrackPoint5(value, false); break;
        default: break;
    }
}

void TrackGeneratorDisplayApvtsBinding::parameterChanged(const juce::String& parameterId, float newValue)
{
    juce::ignoreUnused(newValue);

    const int pointIndex = indexForParameterId(parameterId);
    if (pointIndex < 0)
        return;

    auto* param = apvts_.getParameter(parameterId);
    if (param != nullptr && param == activeGestureGate_->load())
        return;

    const int value = readIntParameter(parameterId);
    auto safeDisplay = juce::Component::SafePointer<TSS::TrackGeneratorDisplay>(&display_);
    const auto activeGestureGate = activeGestureGate_;

    juce::MessageManager::callAsync(
        [safeDisplay, pointIndex, value, activeGestureGate, param]
        {
            if (safeDisplay == nullptr)
                return;

            // Drop stale async work queued before a drag started, or echoed during it.
            if (param != nullptr && param == activeGestureGate->load())
                return;

            switch (pointIndex)
            {
                case 0: safeDisplay->setTrackPoint1(value, false); break;
                case 1: safeDisplay->setTrackPoint2(value, false); break;
                case 2: safeDisplay->setTrackPoint3(value, false); break;
                case 3: safeDisplay->setTrackPoint4(value, false); break;
                case 4: safeDisplay->setTrackPoint5(value, false); break;
                default: break;
            }
        });
}
