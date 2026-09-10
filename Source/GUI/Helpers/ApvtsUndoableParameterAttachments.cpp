#include "ApvtsUndoableParameterAttachments.h"

namespace TSS
{
    namespace
    {
        juce::RangedAudioParameter* getRangedParameterOrNull(juce::AudioProcessorValueTreeState& apvts,
                                                             const juce::String& parameterId)
        {
            return dynamic_cast<juce::RangedAudioParameter*>(apvts.getParameter(parameterId));
        }

        void configureSliderRange(juce::Slider& slider, juce::RangedAudioParameter& parameter)
        {
            auto range = parameter.getNormalisableRange();

            auto convertFrom0To1Function = [range](double currentRangeStart,
                                                     double currentRangeEnd,
                                                     double normalisedValue) mutable
            {
                range.start = static_cast<float>(currentRangeStart);
                range.end = static_cast<float>(currentRangeEnd);
                return static_cast<double>(range.convertFrom0to1(static_cast<float>(normalisedValue)));
            };

            auto convertTo0To1Function = [range](double currentRangeStart,
                                                 double currentRangeEnd,
                                                 double mappedValue) mutable
            {
                range.start = static_cast<float>(currentRangeStart);
                range.end = static_cast<float>(currentRangeEnd);
                return static_cast<double>(range.convertTo0to1(static_cast<float>(mappedValue)));
            };

            auto snapToLegalValueFunction = [range](double currentRangeStart,
                                                      double currentRangeEnd,
                                                      double mappedValue) mutable
            {
                range.start = static_cast<float>(currentRangeStart);
                range.end = static_cast<float>(currentRangeEnd);
                return static_cast<double>(range.snapToLegalValue(static_cast<float>(mappedValue)));
            };

            juce::NormalisableRange<double> newRange { static_cast<double>(range.start),
                                                       static_cast<double>(range.end),
                                                       std::move(convertFrom0To1Function),
                                                       std::move(convertTo0To1Function),
                                                       std::move(snapToLegalValueFunction) };
            newRange.interval = range.interval;
            newRange.skew = range.skew;
            newRange.symmetricSkew = range.symmetricSkew;

            slider.setNormalisableRange(newRange);
            // Double-click opens the slider value editor; never reset via JUCE double-click-return.
            slider.setDoubleClickReturnValue(false, static_cast<double>(parameter.convertFrom0to1(parameter.getDefaultValue())));
            slider.valueFromTextFunction = [&parameter](const juce::String& text)
            {
                return static_cast<double>(
                    parameter.convertFrom0to1(parameter.getValueForText(text)));
            };
            slider.textFromValueFunction = [&parameter](double value)
            {
                return parameter.getText(parameter.convertTo0to1(static_cast<float>(value)), 0);
            };
        }

        bool isPerformingEditorialUndoRedo(const juce::AudioProcessorValueTreeState& apvts)
        {
            return apvts.undoManager != nullptr && apvts.undoManager->isPerformingUndoRedo();
        }

        void beginEditorialParameterEdit(juce::AudioProcessorValueTreeState& apvts,
                                         const juce::String& parameterId)
        {
            if (apvts.undoManager != nullptr)
                apvts.undoManager->beginNewTransaction("Parameter edit: " + parameterId);
        }

        float readDenormalisedParameter(juce::AudioProcessorValueTreeState& apvts,
                                        juce::RangedAudioParameter* parameter,
                                        const juce::String& parameterId)
        {
            if (auto* raw = apvts.getRawParameterValue(parameterId))
                return raw->load();

            if (parameter != nullptr)
                return parameter->convertFrom0to1(parameter->getValue());

            return 0.0f;
        }
    }

    ApvtsUndoableSliderAttachment::ApvtsUndoableSliderAttachment(juce::AudioProcessorValueTreeState& apvts,
                                                                 const juce::String& parameterId,
                                                                 juce::Slider& slider)
        : apvts_(apvts)
        , parameterId_(parameterId)
        , slider_(slider)
        , parameter_(getRangedParameterOrNull(apvts_, parameterId_))
    {
        jassert(parameter_ != nullptr);
        configureSliderRange(slider_, *parameter_);

        if (parameter_ != nullptr)
            parameter_->addListener(this);

        apvts_.state.addListener(this);
        rebindParameterValue();
        slider_.addListener(this);
        syncSliderFromParameter();
    }

    ApvtsUndoableSliderAttachment::~ApvtsUndoableSliderAttachment()
    {
        cancelPendingUpdate();
        slider_.removeListener(this);
        parameterValue_.removeListener(this);
        apvts_.state.removeListener(this);

        if (parameter_ != nullptr)
            parameter_->removeListener(this);
    }

    void ApvtsUndoableSliderAttachment::rebindParameterValue()
    {
        parameterValue_.removeListener(this);
        parameterValue_ = apvts_.getParameterAsValue(parameterId_);
        parameterValue_.addListener(this);
    }

    void ApvtsUndoableSliderAttachment::syncSliderFromParameter()
    {
        if (parameter_ == nullptr)
            return;

        const juce::ScopedValueSetter<bool> scope(ignoreCallbacks_, true);
        slider_.setValue(static_cast<double>(readDenormalisedParameter(apvts_, parameter_, parameterId_)),
                         juce::dontSendNotification);
    }

    void ApvtsUndoableSliderAttachment::writeSliderValueToParameter(bool beginNewEditorialStep)
    {
        if (parameter_ == nullptr || isPerformingEditorialUndoRedo(apvts_))
            return;

        const auto denormalised = static_cast<float>(slider_.getValue());

        if (juce::approximatelyEqual(readDenormalisedParameter(apvts_, parameter_, parameterId_),
                                     denormalised))
            return;

        if (beginNewEditorialStep)
            beginEditorialParameterEdit(apvts_, parameterId_);

        rebindParameterValue();
        parameterValue_.setValue(denormalised);
    }

    void ApvtsUndoableSliderAttachment::beginSliderGesture()
    {
        if (parameter_ == nullptr || isPerformingEditorialUndoRedo(apvts_))
            return;

        beginEditorialParameterEdit(apvts_, parameterId_);
        parameter_->beginChangeGesture();
        changeGestureActive_ = true;
    }

    void ApvtsUndoableSliderAttachment::endSliderGesture()
    {
        if (! changeGestureActive_ || parameter_ == nullptr)
            return;

        parameter_->endChangeGesture();
        changeGestureActive_ = false;
    }

    void ApvtsUndoableSliderAttachment::valueChanged(juce::Value& value)
    {
        juce::ignoreUnused(value);

        if (ignoreCallbacks_)
            return;

        syncSliderFromParameter();
    }

    void ApvtsUndoableSliderAttachment::parameterValueChanged(int, float)
    {
        triggerAsyncUpdate();
    }

    void ApvtsUndoableSliderAttachment::parameterGestureChanged(int, bool) {}

    void ApvtsUndoableSliderAttachment::handleAsyncUpdate()
    {
        syncSliderFromParameter();
    }

    void ApvtsUndoableSliderAttachment::valueTreeRedirected(juce::ValueTree&)
    {
        rebindParameterValue();
        syncSliderFromParameter();
    }

    void ApvtsUndoableSliderAttachment::sliderDragStarted(juce::Slider*)
    {
        beginSliderGesture();
    }

    void ApvtsUndoableSliderAttachment::sliderValueChanged(juce::Slider*)
    {
        if (ignoreCallbacks_)
            return;

        writeSliderValueToParameter(! changeGestureActive_);
    }

    void ApvtsUndoableSliderAttachment::sliderDragEnded(juce::Slider*)
    {
        endSliderGesture();
    }

    ApvtsUndoableComboBoxAttachment::ApvtsUndoableComboBoxAttachment(
        juce::AudioProcessorValueTreeState& apvts,
        const juce::String& parameterId,
        juce::ComboBox& comboBox)
        : apvts_(apvts)
        , parameterId_(parameterId)
        , comboBox_(comboBox)
    {
        if (auto* parameter = apvts_.getParameter(parameterId_))
            parameter->addListener(this);

        apvts_.state.addListener(this);
        rebindParameterValue();
        comboBox_.addListener(this);
        syncComboBoxFromParameter();
    }

    ApvtsUndoableComboBoxAttachment::~ApvtsUndoableComboBoxAttachment()
    {
        cancelPendingUpdate();
        comboBox_.removeListener(this);
        parameterValue_.removeListener(this);
        apvts_.state.removeListener(this);

        if (auto* parameter = apvts_.getParameter(parameterId_))
            parameter->removeListener(this);
    }

    void ApvtsUndoableComboBoxAttachment::rebindParameterValue()
    {
        parameterValue_.removeListener(this);
        parameterValue_ = apvts_.getParameterAsValue(parameterId_);
        parameterValue_.addListener(this);
    }

    int ApvtsUndoableComboBoxAttachment::readChoiceIndexFromParameter() const
    {
        if (const auto* choice = dynamic_cast<const juce::AudioParameterChoice*>(
                apvts_.getParameter(parameterId_)))
            return choice->getIndex();

        if (auto* raw = apvts_.getRawParameterValue(parameterId_))
            return juce::roundToInt(raw->load());

        return 0;
    }

    void ApvtsUndoableComboBoxAttachment::syncComboBoxFromParameter()
    {
        const juce::ScopedValueSetter<bool> scope(ignoreCallbacks_, true);
        comboBox_.setSelectedItemIndex(readChoiceIndexFromParameter(), juce::dontSendNotification);
    }

    void ApvtsUndoableComboBoxAttachment::writeComboBoxSelectionToParameter()
    {
        if (isPerformingEditorialUndoRedo(apvts_))
            return;

        const auto selectedIndex = comboBox_.getSelectedItemIndex();
        if (selectedIndex < 0)
            return;

        if (readChoiceIndexFromParameter() == selectedIndex)
            return;

        beginEditorialParameterEdit(apvts_, parameterId_);
        rebindParameterValue();
        parameterValue_.setValue(static_cast<float>(selectedIndex));
    }

    void ApvtsUndoableComboBoxAttachment::valueChanged(juce::Value& value)
    {
        juce::ignoreUnused(value);

        if (ignoreCallbacks_)
            return;

        syncComboBoxFromParameter();
    }

    void ApvtsUndoableComboBoxAttachment::parameterValueChanged(int, float)
    {
        triggerAsyncUpdate();
    }

    void ApvtsUndoableComboBoxAttachment::parameterGestureChanged(int, bool) {}

    void ApvtsUndoableComboBoxAttachment::handleAsyncUpdate()
    {
        syncComboBoxFromParameter();
    }

    void ApvtsUndoableComboBoxAttachment::valueTreeRedirected(juce::ValueTree&)
    {
        rebindParameterValue();
        syncComboBoxFromParameter();
    }

    void ApvtsUndoableComboBoxAttachment::comboBoxChanged(juce::ComboBox*)
    {
        if (ignoreCallbacks_)
            return;

        writeComboBoxSelectionToParameter();
    }
}
