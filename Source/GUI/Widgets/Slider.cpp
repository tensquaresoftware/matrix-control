#include "Slider.h"

#include "GUI/Skins/Skin.h"

namespace tss
{
    Slider::Slider(tss::Skin& skin, int width, int height, double defaultValue)
        : juce::Slider(juce::Slider::LinearBarVertical, juce::Slider::NoTextBox)
        , skin_(&skin)
        , width_(width)
        , height_(height)
        , defaultValue_(defaultValue)
        , cachedFont_(juce::FontOptions())
    {
        setOpaque(false);
        setSize(width_, height_);
        setWantsKeyboardFocus(true);
        setInterceptsMouseClicks(true, false);
        updateSkinCache();
    }

    void Slider::setSkin(tss::Skin& skin)
    {
        skin_ = &skin;
        invalidateCache();
    }

    void Slider::setUnit(const juce::String& unit)
    {
        unit_ = unit;
        invalidateCache();
        repaint();
    }

    juce::String Slider::getUnit() const
    {
        return unit_;
    }

    void Slider::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr)
            return;

        // Invalidate cache if value changed (Slider is dynamic)
        if (!juce::approximatelyEqual(getValue(), cachedValue_))
            invalidateCache();

        if (!cacheValid_)
            regenerateCache();

        if (cachedImage_.isValid())
        {
            g.drawImage(cachedImage_, getLocalBounds().toFloat(),
                       juce::RectanglePlacement::stretchToFit);
        }
    }

    void Slider::resized()
    {
        invalidateCache();
    }

    void Slider::regenerateCache()
    {
        const auto width = getWidth();
        const auto height = getHeight();

        if (width <= 0 || height <= 0)
            return;

        const float pixelScale = getPixelScale();
        const int imageWidth = juce::roundToInt(width * pixelScale);
        const int imageHeight = juce::roundToInt(height * pixelScale);

        // Create HiDPI image at physical resolution
        cachedImage_ = juce::Image(juce::Image::ARGB, imageWidth, imageHeight, true);
        juce::Graphics g(cachedImage_);
        
        // Scale graphics context to match physical resolution
        g.addTransform(juce::AffineTransform::scale(pixelScale));

        const auto bounds = juce::Rectangle<float>(0.0f, 0.0f, 
                                                    static_cast<float>(width), 
                                                    static_cast<float>(height));
        const auto enabled = isEnabled();
        const auto trackBounds = calculateTrackBounds(bounds);
        const auto valueBarBounds = calculateValueBarBounds(trackBounds, enabled);

        drawTrack(g, trackBounds, enabled);
        drawValueBar(g, valueBarBounds, enabled);
        drawText(g, bounds, enabled);
        drawFocusBorderIfNeeded(g, trackBounds, hasFocus_);

        cachedValue_ = getValue();
        cacheValid_ = true;
    }

    void Slider::invalidateCache()
    {
        cacheValid_ = false;
    }

    void Slider::updateSkinCache()
    {
        if (skin_ == nullptr)
            return;

        const bool enabled = isEnabled();
        cachedTrackColour_ = skin_->getSliderTrackColour(enabled);
        cachedValueBarColour_ = skin_->getSliderValueBarColour(enabled);
        cachedTextColour_ = skin_->getSliderTextColour(enabled);
        cachedFocusBorderColour_ = skin_->getSliderFocusBorderColour();
        cachedFont_ = skin_->getBaseFont();
    }

    float Slider::getPixelScale() const
    {
        const auto* display = juce::Desktop::getInstance()
                                  .getDisplays()
                                  .getDisplayForRect(getScreenBounds());
        const float displayScale = display != nullptr ? static_cast<float>(display->scale) : 1.0f;
        return displayScale;
    }

    juce::Rectangle<float> Slider::calculateTrackBounds(const juce::Rectangle<float>& bounds) const
    {
        const auto trackHeight = static_cast<float>(kTrackHeight_);
        const auto trackY = (bounds.getHeight() - trackHeight) * 0.5f;
        return juce::Rectangle<float>(bounds.getX(), bounds.getY() + trackY, bounds.getWidth(), trackHeight);
    }

    juce::Rectangle<float> Slider::calculateValueBarBounds(const juce::Rectangle<float>& trackBounds, bool enabled) const
    {
        if (!enabled)
            return juce::Rectangle<float>();

        const auto range = getRange();
        const auto rangeLength = range.getLength();
        
        if (rangeLength <= 0.0)
            return juce::Rectangle<float>();

        const auto valueBarArea = trackBounds.reduced(1.0f);
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
            g.setColour(cachedFocusBorderColour_);
            g.drawRect(bounds, 1.0f);
        }
    }

    void Slider::drawTrack(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool /*enabled*/)
    {
        g.setColour(cachedTrackColour_);
        g.fillRect(bounds);
    }

    void Slider::drawValueBar(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool /*enabled*/)
    {
        if (bounds.isEmpty())
            return;

        g.setColour(cachedValueBarColour_);
        g.fillRect(bounds);
    }

    void Slider::drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool /*enabled*/)
    {
        auto valueText = juce::String(static_cast<int>(std::round(getValue())));
        
        if (unit_.isNotEmpty())
            valueText += " " + unit_;

        g.setColour(cachedTextColour_);
        g.setFont(cachedFont_);
        g.drawText(valueText, bounds, juce::Justification::centred, false);
    }

    void Slider::mouseDown(const juce::MouseEvent& e)
    {
        if (!isEnabled())
            return;
        
        grabKeyboardFocus();
        dragStartValue_ = getValue();
        dragStartPosition_ = e.getPosition();
    }

    void Slider::mouseDrag(const juce::MouseEvent& e)
    {
        if (!isEnabled())
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
        // Focus is handled by focusGained/focusLost callbacks
    }

    void Slider::mouseDoubleClick(const juce::MouseEvent&)
    {
        if (!isEnabled())
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
            invalidateCache();
            repaint();
        }
    }

    void Slider::focusLost(juce::Component::FocusChangeType)
    {
        hasFocus_ = false;
        invalidateCache();
        repaint();
    }

    bool Slider::keyPressed(const juce::KeyPress& key)
    {
        if (!isEnabled() || !hasKeyboardFocus(true))
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
        {
            return kShiftKeyStep_;
        }
        
        if (rangeLength <= 100.0)
        {
            return 1.0;
        }
        
        return rangeLength / 100.0;
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
        const auto range = getRange();
        const auto currentValue = getValue();
        auto newValue = increment ? currentValue + step : currentValue - step;
        newValue = juce::jlimit(range.getStart(), range.getEnd(), newValue);
        setValue(newValue, juce::sendNotificationSync);
    }
}

