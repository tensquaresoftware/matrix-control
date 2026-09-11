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
        setDoubleClickReturnValue(false, defaultValue_);
    }

    Slider::~Slider()
    {
        stopTimer();
        hideValueEditor();
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
        applyEditorAppearance();
        repaint();
    }

    void Slider::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;

        uiScale_ = uiScale;
        applyEditorAppearance();
        layoutEditor();
        repaint();
    }

    void Slider::openArrowKeyDragSessionIfNeeded()
    {
        if (arrowKeyDragNotification_ != nullptr)
            return;

        arrowKeyDragNotification_ = std::make_unique<juce::Slider::ScopedDragNotification>(*this);
    }

    juce::String Slider::getUnit() const
    {
        return unit_;
    }

    bool Slider::isValueEditorOpen() const
    {
        return editor_ != nullptr;
    }

    void Slider::paint(juce::Graphics& g)
    {
        const bool enabled = isEnabled();
        const bool editing = isValueEditorOpen();
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

        if (! editing)
        {
            const auto valueBarBounds = calculateValueBarBounds(trackBoundsInt, insetPerSide);
            drawValueBar(g, valueBarBounds, enabled);
            drawText(g, trackBoundsFloat, enabled);
        }

        drawFocusBorderIfNeeded(g, trackBoundsFloat, hasFocus_ && ! editing);
    }

    void Slider::paintOverChildren(juce::Graphics& g)
    {
        if (editor_ == nullptr || ! editCaretOn_)
            return;

        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const int insetPerSide = ScaledDrawing::logicalInsetPixelsFromDesign(
            static_cast<float>(kValueBarPadding_),
            uiScale_,
            systemDisplayScale);
        const float inset = static_cast<float>(insetPerSide);
        const float height = static_cast<float>(getLocalBounds().getHeight());
        const float caretHeight = height - 2.0f * inset;

        if (caretHeight <= 0.0f)
            return;

        const float thickness = sliderCaretThickness();
        const float width = static_cast<float>(getLocalBounds().getWidth());

        if (thickness <= 0.0f || width < thickness)
            return;

        g.setColour(look_.editorCaret);
        g.fillRect(editCaretX(thickness), inset, thickness, caretHeight);
    }

    void Slider::resized()
    {
        layoutEditor();
        repaint();
    }

    void Slider::enablementChanged()
    {
        if (! isEnabled() && isValueEditorOpen())
            hideValueEditor();

        repaint();
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
        g.setFont(scaledValueFont());
        g.drawText(valueText, bounds, juce::Justification::centred, false);
    }

    juce::Font Slider::scaledValueFont() const
    {
        return look_.font.withHeight(look_.font.getHeight() * uiScale_);
    }

    bool Slider::isCommandOrCtrlClick(const juce::MouseEvent& e)
    {
        return e.mods.isCommandDown();
    }

    void Slider::mouseDown(const juce::MouseEvent& e)
    {
        if (! isEnabled())
            return;

        if (isValueEditorOpen())
        {
            if (isCommandOrCtrlClick(e))
            {
                hideValueEditor();
                resetToDefaultValue();
                return;
            }

            hideValueEditor();
            return;
        }

        if (isCommandOrCtrlClick(e))
        {
            grabKeyboardFocus();
            resetToDefaultValue();
            return;
        }

        grabKeyboardFocus();
        dragStartValue_ = getValue();
        dragStartPosition_ = e.getPosition();
        dragNotification_ = std::make_unique<juce::Slider::ScopedDragNotification>(*this);
    }

    void Slider::mouseDrag(const juce::MouseEvent& e)
    {
        if (! isEnabled() || isValueEditorOpen() || dragNotification_ == nullptr)
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
        dragNotification_.reset();
    }

    void Slider::mouseDoubleClick(const juce::MouseEvent& e)
    {
        if (! isEnabled() || isCommandOrCtrlClick(e))
            return;

        showValueEditor();
    }

    void Slider::resetToDefaultValue()
    {
        if (juce::approximatelyEqual(getValue(), defaultValue_))
            return;

        const juce::Slider::ScopedDragNotification dragSession(*this);
        setValue(defaultValue_, juce::sendNotificationSync);
    }

    void Slider::cancelActiveDragSession()
    {
        dragNotification_.reset();
        arrowKeyDragNotification_.reset();
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
        arrowKeyDragNotification_.reset();
        repaint();
    }

    bool Slider::keyPressed(const juce::KeyPress& key)
    {
        if (! isEnabled() || ! hasFocus_ || isValueEditorOpen())
            return false;

        const bool isShiftPressed = key.getModifiers().isShiftDown();
        const auto step = calculateStep(isShiftPressed);

        if (isIncrementKey(key.getKeyCode()))
        {
            openArrowKeyDragSessionIfNeeded();
            updateValueWithStep(step, true);
            return true;
        }

        if (isDecrementKey(key.getKeyCode()))
        {
            openArrowKeyDragSessionIfNeeded();
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
