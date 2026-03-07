#include "Slider.h"

namespace tss
{
    Slider::Slider(int width, int height, double defaultValue)
        : juce::Slider(juce::Slider::LinearBarVertical, juce::Slider::NoTextBox)
        , width_(width)
        , height_(height)
        , defaultValue_(defaultValue)
    {
        setOpaque(false);
        setSize(width_, height_);
        setWantsKeyboardFocus(true);
        setInterceptsMouseClicks(true, false);
    }

    void Slider::setLook(const SliderLook& look)
    {
        look_ = look;
        repaint();
    }

    void Slider::setScalingFactor(float scalingFactor)
    {
        if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
            return;
        
        scalingFactor_ = scalingFactor;
        repaint();
    }

    void Slider::setUnit(const juce::String& unit)
    {
        unit_ = unit;
        repaint();
    }

    juce::String Slider::getUnit() const
    {
        return unit_;
    }

    void Slider::paint(juce::Graphics& g)
    {
        const auto bounds = juce::Rectangle<float>(0.0f, 0.0f, 
                                                    static_cast<float>(getWidth()), 
                                                    static_cast<float>(getHeight()));
        const auto enabled = isEnabled();
        const auto trackBounds = calculateTrackBounds(bounds);
        const auto valueBarBounds = calculateValueBarBounds(trackBounds, enabled);

        drawTrack(g, trackBounds, enabled);
        drawValueBar(g, valueBarBounds, enabled);
        drawText(g, bounds, enabled);
        drawFocusBorderIfNeeded(g, trackBounds, hasFocus_);
    }

    juce::Rectangle<float> Slider::calculateTrackBounds(const juce::Rectangle<float>& bounds) const
    {
        const auto trackHeight = static_cast<float>(kTrackHeight_) * scalingFactor_;
        const auto trackY = (bounds.getHeight() - trackHeight) * 0.5f;
        return juce::Rectangle<float>(bounds.getX(), bounds.getY() + trackY, bounds.getWidth(), trackHeight);
    }

    juce::Rectangle<float> Slider::calculateValueBarBounds(const juce::Rectangle<float>& trackBounds, bool enabled) const
    {
        if (! enabled)
            return juce::Rectangle<float>();

        const auto range = getRange();
        const auto rangeLength = range.getLength();
        
        if (rangeLength <= 0.0)
            return juce::Rectangle<float>();

        const float reduction = std::max(1.0f, 1.0f * scalingFactor_);
        const auto valueBarArea = trackBounds.reduced(reduction);
        const auto value = getValue();
        auto normalizedValue = static_cast<float>((value - range.getStart()) / rangeLength);
        normalizedValue = juce::jlimit(0.0f, 1.0f, normalizedValue);
        const auto valueBarWidth = valueBarArea.getWidth() * normalizedValue;
        
        return juce::Rectangle<float>(valueBarArea.getX(), valueBarArea.getY(), valueBarWidth, valueBarArea.getHeight());
    }

    void Slider::drawFocusBorderIfNeeded(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool hasFocus)
    {
        if (hasFocus)
        {
            const float borderThickness = std::max(1.0f, 1.0f * scalingFactor_);
            g.setColour(look_.focusBorder);
            g.drawRect(bounds, borderThickness);
        }
    }

    void Slider::drawTrack(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled)
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
        auto valueText = juce::String(static_cast<int>(std::round(getValue())));
        
        if (unit_.isNotEmpty())
            valueText += " " + unit_;

        g.setColour(enabled ? look_.textEnabled : look_.textDisabled);
        g.setFont(look_.font.withHeight(kFontSize_ * scalingFactor_));
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
        const auto range = getRange();
        auto newValue = dragStartValue_ + valueDelta;
        newValue = juce::jlimit(range.getStart(), range.getEnd(), newValue);
        
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
        
        const auto range = getRange();
        const auto rangeLength = range.getLength();
        const bool isShiftPressed = key.getModifiers().isShiftDown();
        const auto step = calculateStepForRange(rangeLength, isShiftPressed);
        
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

    double Slider::calculateStepForRange(double rangeLength, bool isShiftPressed) const
    {
        if (isShiftPressed)
            return rangeLength / kShiftKeyStep_;
        
        return 1.0;
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
        const auto range = getRange();
        auto newValue = increment ? currentValue + step : currentValue - step;
        newValue = juce::jlimit(range.getStart(), range.getEnd(), newValue);
        setValue(newValue, juce::sendNotificationSync);
    }
}
