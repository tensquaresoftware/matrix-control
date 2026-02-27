#include "Button.h"

#include <map>

#include "GUI/Skins/Skin.h"

namespace tss
{
    Button::Button(tss::Skin& skin, int width, int height, const juce::String& text)
        : juce::Button(text)
        , skin_(&skin)
        , width_(width)
        , height_(height)
    {
        setOpaque(true);
        setSize(width_, height_);
    }

    void Button::setSkin(tss::Skin& skin)
    {
        skin_ = &skin;
        invalidateCache();
    }

    void Button::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
    {
        if (skin_ == nullptr)
            return;

        if (!cacheValid_)
            regenerateCache();

        const auto state = getCurrentState(isEnabled(), shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        const auto it = cachedImages_.find(state);

        if (it != cachedImages_.end() && it->second.isValid())
        {
            const auto bounds = getLocalBounds();
            
            // Draw the HiDPI image scaled to logical size
            // Use drawImageWithin to avoid any stretching artifacts
            g.drawImageWithin(it->second, 
                            bounds.getX(), bounds.getY(), 
                            bounds.getWidth(), bounds.getHeight(),
                            juce::RectanglePlacement::stretchToFit,
                            false); // No resampling quality needed
        }
    }

    void Button::resized()
    {
        invalidateCache();
    }

    void Button::regenerateCache()
    {
        const auto localBounds = getLocalBounds();
        const auto width = localBounds.getWidth();
        const auto height = localBounds.getHeight();

        if (width <= 0 || height <= 0)
            return;

        const float pixelScale = getPixelScale();
        const int imageWidth = juce::roundToInt(width * pixelScale);
        const int imageHeight = juce::roundToInt(height * pixelScale);

        // Pre-render all button states at HiDPI resolution
        cachedImages_.clear();

        for (auto state : {ButtonState::Normal, ButtonState::Hover, ButtonState::Pressed, ButtonState::Disabled})
        {
            // Create HiDPI image at physical resolution with clear background
            juce::Image stateImage(juce::Image::ARGB, imageWidth, imageHeight, true);
            juce::Graphics g(stateImage);
            
            // Scale graphics context to match physical resolution
            g.addTransform(juce::AffineTransform::scale(pixelScale));

            renderButtonState(g, state);

            cachedImages_[state] = std::move(stateImage);
        }

        cacheValid_ = true;
    }

    void Button::invalidateCache()
    {
        cacheValid_ = false;
    }

    float Button::getPixelScale() const
    {
        const auto* display = juce::Desktop::getInstance()
                                  .getDisplays()
                                  .getDisplayForRect(getScreenBounds());
        const float displayScale = display != nullptr ? static_cast<float>(display->scale) : 1.0f;
        return displayScale;
    }

    Button::ButtonState Button::getCurrentState(bool enabled, bool isHighlighted, bool isDown) const
    {
        if (!enabled)
            return ButtonState::Disabled;
        if (isDown)
            return ButtonState::Pressed;
        if (isHighlighted)
            return ButtonState::Hover;
        return ButtonState::Normal;
    }

    void Button::renderButtonState(juce::Graphics& g, ButtonState state)
    {
        // Use actual component bounds from layout, not constructor params
        const auto localBounds = getLocalBounds();
        const auto bounds = localBounds.toFloat();
        const auto buttonText = getButtonText();

        bool enabled = (state != ButtonState::Disabled);
        bool isHighlighted = (state == ButtonState::Hover);
        bool isDown = (state == ButtonState::Pressed);

        g.setColour(getBackgroundColour(enabled, isHighlighted, isDown));
        g.fillRect(bounds);

        g.setColour(getBorderColour(enabled));
        g.drawRect(bounds, kBorderThickness_);

        if (!buttonText.isEmpty())
        {
            g.setColour(getTextColour(enabled, isHighlighted, isDown));
            g.setFont(skin_->getBaseFont());
            g.drawText(buttonText, bounds, juce::Justification::centred, false);
        }
    }

    juce::Colour Button::getBackgroundColour(bool enabled, bool isHighlighted, bool isDown) const
    {
        if (!enabled)
            return skin_->getButtonBackgroundColourOff();

        if (isDown)
            return skin_->getButtonBackgroundColourClicked();

        if (isHighlighted)
            return skin_->getButtonBackgroundColourHoover();

        return skin_->getButtonBackgroundColourOn();
    }

    juce::Colour Button::getBorderColour(bool enabled) const
    {
        if (!enabled)
            return skin_->getButtonBorderColourOff();

        return skin_->getButtonBorderColourOn();
    }

    juce::Colour Button::getTextColour(bool enabled, bool isHighlighted, bool isDown) const
    {
        if (!enabled)
            return skin_->getButtonTextColourOff();

        if (isDown)
            return skin_->getButtonTextColourClicked();

        if (isHighlighted)
            return skin_->getButtonTextColourHoover();

        return skin_->getButtonTextColourOn();
    }
}

