#include "Slider.h"

#include <cmath>

#include "GUI/Layout/ScaledDrawing.h"

namespace TSS
{
    Slider::Slider(int width, int height, const SliderLook& look, const SliderConfig& config)
        : juce::Slider(juce::Slider::LinearBarVertical, juce::Slider::NoTextBox)
        , look_(look)
        , width_(width)
        , height_(height)
        , defaultValue_(config.defaultValue)
        , step_(config.step > 0.0 ? config.step : 1.0)
        , valueDecimalPlaces_(countDecimalPlacesForStep(step_))
        , unit_(config.unit)
        , minimumDisplayText_(config.minimumDisplayText)
        , normalizedFill_(std::move(config.normalizedFill))
        , formatValue_(std::move(config.formatValue))
    {
        setOpaque(false);
        setSize(width_, height_);
        setWantsKeyboardFocus(true);
        setInterceptsMouseClicks(true, false);
        setRange(config.minValue, config.maxValue, step_);
        setValue(defaultValue_, juce::dontSendNotification);
    }

    int Slider::countDecimalPlacesForStep(double step)
    {
        if (step <= 0.0 || juce::approximatelyEqual(step, std::round(step)))
            return 0;

        for (int places = 1; places <= 6; ++places)
        {
            const double scaled = step * std::pow(10.0, static_cast<double>(places));

            if (juce::approximatelyEqual(scaled, std::round(scaled)))
                return places;
        }

        return 2;
    }

    void Slider::setLook(const SliderLook& look)
    {
        look_ = look;
        repaint();
    }

    void Slider::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;

        uiScale_ = uiScale;
        repaint();
    }

    juce::String Slider::getUnit() const
    {
        return unit_;
    }

    void Slider::paint(juce::Graphics& g)
    {
        const bool enabled = isEnabled();
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const int insetPerSide = ScaledDrawing::logicalInsetPixelsFromDesign(
            static_cast<float>(kValueBarPadding_),
            uiScale_,
            systemDisplayScale);

        const auto trackBoundsInt = getLocalBounds();
        const auto trackBoundsFloat = trackBoundsInt.toFloat();

        if (enabled)
        {
            drawTrack(g, trackBoundsInt, true);
        }
        else
        {
            g.setColour(look_.backgroundDisabled);
            g.fillRect(trackBoundsInt);

            const auto railBounds = trackBoundsInt.reduced(insetPerSide);
            if (! railBounds.isEmpty())
            {
                g.setColour(look_.trackDisabled);
                g.fillRect(railBounds);
            }
        }

        const auto valueBarBounds = calculateValueBarBounds(trackBoundsInt, insetPerSide);
        drawValueBar(g, valueBarBounds, enabled);

        drawText(g, trackBoundsFloat, enabled);
        drawFocusBorderIfNeeded(g, trackBoundsFloat, hasFocus_);
    }

    juce::Rectangle<float> Slider::calculateValueBarBounds(const juce::Rectangle<int>& trackBoundsInt, int insetPerSide) const
    {
        const auto range = getRange();
        const auto rangeLength = range.getLength();

        if (rangeLength <= 0.0)
            return {};

        auto valueBarArea = trackBoundsInt.reduced(insetPerSide);

        if (valueBarArea.isEmpty())
            return {};

        const auto value = getValue();
        float normalizedValue = 0.0f;

        if (normalizedFill_)
        {
            normalizedValue = normalizedFill_(value);
        }
        else
        {
            normalizedValue = static_cast<float>((value - range.getStart()) / rangeLength);
        }

        normalizedValue = juce::jlimit(0.0f, 1.0f, normalizedValue);
        const int widthFull = valueBarArea.getWidth();
        const int valueBarWidth = juce::jmax(0, juce::roundToInt(static_cast<float>(widthFull) * normalizedValue));

        return juce::Rectangle<float>(
            static_cast<float>(valueBarArea.getX()),
            static_cast<float>(valueBarArea.getY()),
            static_cast<float>(valueBarWidth),
            static_cast<float>(valueBarArea.getHeight()));
    }

    void Slider::drawFocusBorderIfNeeded(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool hasFocus)
    {
        if (hasFocus)
        {
            const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
            const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
                kFocusBorderThickness_,
                uiScale_,
                systemDisplayScale,
                ScaledDrawing::StrokeSnapPolicy::kFloor);
            g.setColour(look_.focusBorder);
            g.drawRect(bounds, borderThickness);
        }
    }

    void Slider::drawTrack(juce::Graphics& g, const juce::Rectangle<int>& bounds, bool enabled)
    {
        g.setColour(enabled ? look_.trackEnabled : look_.trackDisabled);
        g.fillRect(bounds);
    }

    void Slider::drawValueBar(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled)
    {
        if (bounds.isEmpty())
            return;

        g.setColour(enabled ? look_.valueBarEnabled : look_.valueBarDisabled);
        g.fillRect(bounds);
    }

    void Slider::drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled)
    {
        juce::String valueText;

        if (formatValue_)
        {
            valueText = formatValue_(getValue());
        }
        else if (minimumDisplayText_.isNotEmpty() && juce::approximatelyEqual(getValue(), getMinimum()))
            valueText = minimumDisplayText_;
        else if (valueDecimalPlaces_ > 0)
            valueText = juce::String(getValue(), valueDecimalPlaces_);
        else
            valueText = juce::String(static_cast<int>(std::round(getValue())));

        if (! formatValue_ && unit_.isNotEmpty())
            valueText += " " + unit_;

        g.setColour(enabled ? look_.textEnabled : look_.textDisabled);
        g.setFont(look_.font.withHeight(look_.font.getHeight() * uiScale_));
        g.drawText(valueText, bounds, juce::Justification::centred, false);
    }

    void Slider::mouseDown(const juce::MouseEvent& e)
    {
        if (! isEnabled())
            return;

        grabKeyboardFocus();
        dragStartValue_ = getValue();
        dragStartPosition_ = e.getPosition();
    }

    void Slider::mouseDrag(const juce::MouseEvent& e)
    {
        if (! isEnabled())
            return;

        const auto dragDistance = dragStartPosition_.y - e.getPosition().y;
        const auto valueDelta = dragDistance * kDragSensitivity_;
        const auto range = getNormalisableRange();
        auto newValue = dragStartValue_ + valueDelta;
        newValue = juce::jlimit(range.start, range.end, newValue);
        newValue = range.snapToLegalValue(newValue);

        setValue(newValue, juce::sendNotificationSync);
    }

    void Slider::mouseUp(const juce::MouseEvent&)
    {
    }

    void Slider::mouseDoubleClick(const juce::MouseEvent&)
    {
        if (! isEnabled())
            return;

        resetToDefaultValue();
    }

    void Slider::resetToDefaultValue()
    {
        setValue(defaultValue_, juce::sendNotificationSync);
    }

    void Slider::focusGained(juce::Component::FocusChangeType)
    {
        if (isEnabled())
        {
            hasFocus_ = true;
            repaint();
        }
    }

    void Slider::focusLost(juce::Component::FocusChangeType)
    {
        hasFocus_ = false;
        repaint();
    }

    bool Slider::keyPressed(const juce::KeyPress& key)
    {
        if (! isEnabled() || ! hasKeyboardFocus(true))
            return false;

        if (key == juce::KeyPress::returnKey)
        {
            resetToDefaultValue();
            return true;
        }

        const bool isShiftPressed = key.getModifiers().isShiftDown();
        const auto step = calculateStep(isShiftPressed);

        if (isIncrementKey(key.getKeyCode()))
        {
            updateValueWithStep(step, true);
            return true;
        }

        if (isDecrementKey(key.getKeyCode()))
        {
            updateValueWithStep(step, false);
            return true;
        }

        return false;
    }

    double Slider::calculateStep(bool isShiftPressed) const
    {
        return isShiftPressed ? step_ * kShiftStepMultiplier_ : step_;
    }

    bool Slider::isIncrementKey(int keyCode) const
    {
        return keyCode == juce::KeyPress::upKey || keyCode == juce::KeyPress::rightKey;
    }

    bool Slider::isDecrementKey(int keyCode) const
    {
        return keyCode == juce::KeyPress::downKey || keyCode == juce::KeyPress::leftKey;
    }

    void Slider::updateValueWithStep(double step, bool increment)
    {
        const auto currentValue = getValue();
        const auto range = getNormalisableRange();
        auto newValue = increment ? currentValue + step : currentValue - step;
        newValue = juce::jlimit(range.start, range.end, newValue);
        newValue = range.snapToLegalValue(newValue);
        setValue(newValue, juce::sendNotificationSync);
    }
}
