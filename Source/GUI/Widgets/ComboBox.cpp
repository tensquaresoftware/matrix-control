#include "ComboBox.h"
#include "MultiColumnPopupMenu.h"
#include "ScrollablePopupMenu.h"

#include "GUI/Skins/ISkin.h"

namespace tss
{
    ComboBox::ComboBox(tss::ISkin& skin, int width, int height, Style style)
        : juce::ComboBox()
        , skin_(&skin)
        , width_(width)
        , height_(height)
        , style_(style)
        , cachedFont_(juce::FontOptions())
    {
        setOpaque(false);
        setSize(width_, height_);
        setWantsKeyboardFocus(true);
        setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
        updateSkinCache();
    }

    void ComboBox::setSkin(tss::ISkin& skin)
    {
        skin_ = &skin;
        setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
        invalidateCache();
    }

    void ComboBox::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr)
            return;

        // Invalidate cache if selection changed
        const auto currentIndex = getSelectedItemIndex();
        
        if (currentIndex != cachedSelectedIndex_ || !cacheValid_)
            invalidateCache();

        if (!cacheValid_)
            regenerateCache();

        if (cachedImage_.isValid())
        {
            g.drawImage(cachedImage_, getLocalBounds().toFloat(),
                       juce::RectanglePlacement::stretchToFit);
        }
    }

    void ComboBox::resized()
    {
        invalidateCache();
    }

    void ComboBox::regenerateCache()
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
        const auto hasFocus = hasFocus_ || isPopupOpen_;
        const auto backgroundBounds = calculateBackgroundBounds(bounds);

        drawBackground(g, backgroundBounds, enabled);
        drawText(g, bounds, enabled);
        drawTriangle(g, bounds, enabled);
        drawBorderIfNeeded(g, bounds, backgroundBounds, enabled, hasFocus);

        cachedSelectedIndex_ = getSelectedItemIndex();
        cacheValid_ = true;
    }

    void ComboBox::invalidateCache()
    {
        cacheValid_ = false;
    }

    void ComboBox::updateSkinCache()
    {
        if (skin_ == nullptr)
            return;

        const bool enabled = isEnabled();
        const bool isButtonLike = (style_ == Style::ButtonLike);
        
        cachedBackgroundColour_ = skin_->getComboBoxBackgroundColour(enabled, isButtonLike);
        cachedTextColour_ = skin_->getComboBoxTextColour(enabled, isButtonLike);
        cachedBorderColour_ = skin_->getComboBoxBorderColour(enabled, isButtonLike);
        cachedFocusBorderColour_ = skin_->getComboBoxFocusBorderColour(isButtonLike);
        cachedFont_ = skin_->getBaseFont();
    }

    float ComboBox::getPixelScale() const
    {
        const auto* display = juce::Desktop::getInstance()
                                  .getDisplays()
                                  .getDisplayForRect(getScreenBounds());
        const float displayScale = display != nullptr ? static_cast<float>(display->scale) : 1.0f;
        return displayScale;
    }


    void ComboBox::drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool /*enabled*/)
    {
        g.setColour(cachedBackgroundColour_);
        g.fillRect(bounds);
    }

    void ComboBox::drawBorderIfNeeded(juce::Graphics& g, const juce::Rectangle<float>& bounds, const juce::Rectangle<float>& backgroundBounds, bool /*enabled*/, bool hasFocus)
    {
        const auto isButtonLike = (style_ == Style::ButtonLike);
        
        if (isButtonLike)
        {
            g.setColour(cachedBorderColour_);
            g.drawRect(bounds, static_cast<float>(kBorderThicknessButtonLike_));
            return;
        }

        if (hasFocus)
        {
            g.setColour(cachedFocusBorderColour_);
            g.drawRect(backgroundBounds, static_cast<float>(kBorderThickness_));
        }
    }

    void ComboBox::drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled)
    {
        const auto text = getSelectedItemText();
        const auto textColour = getTextColourForCurrentStyle(enabled);
        const auto textBounds = calculateTextBounds(bounds);
        drawTextInBounds(g, text, textBounds, textColour);
    }

    juce::String ComboBox::getSelectedItemText() const
    {
        const auto selectedIndex = getSelectedItemIndex();
        if (selectedIndex >= 0)
        {
            return getItemText(selectedIndex);
        }
        return juce::String();
    }

    juce::Colour ComboBox::getTextColourForCurrentStyle(bool /*enabled*/) const
    {
        return cachedTextColour_;
    }

    juce::Rectangle<float> ComboBox::calculateTextBounds(const juce::Rectangle<float>& bounds) const
    {
        auto textBounds = bounds;
        textBounds.removeFromLeft(kLeftPadding_);
        textBounds.removeFromRight(kTriangleBaseSize_);
        textBounds.removeFromRight(kRightPadding_);
        return textBounds;
    }

    void ComboBox::drawTextInBounds(juce::Graphics& g, const juce::String& text, const juce::Rectangle<float>& textBounds, const juce::Colour& textColour) const
    {
        g.setColour(textColour);
        g.setFont(cachedFont_);
        g.drawText(text, textBounds, juce::Justification::centredLeft, false);
    }

    void ComboBox::drawTriangle(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled)
    {
        const auto isButtonLike = (style_ == Style::ButtonLike);
        const auto triangleColour = skin_->getComboBoxTriangleColour(enabled, isButtonLike);
        g.setColour(triangleColour);

        const auto triangleBaseSize = kTriangleBaseSize_;
        const auto triangleHeight = triangleBaseSize * kTriangleHeightFactor_;
        const auto triangleX = bounds.getRight() - triangleBaseSize - kRightPadding_;
        const auto triangleY = bounds.getCentreY() - triangleHeight * 0.5f;

        const auto trianglePath = createTrianglePath(triangleX, triangleY, triangleBaseSize);
        g.fillPath(trianglePath);
    }

    juce::Path ComboBox::createTrianglePath(float x, float y, float baseSize) const
    {
        juce::Path path;
        
        const auto height = baseSize * kTriangleHeightFactor_;
        
        path.startNewSubPath(x, y);
        path.lineTo(x + baseSize, y);
        path.lineTo(x + baseSize * 0.5f, y + height);
        path.closeSubPath();
        
        return path;
    }

    juce::Rectangle<float> ComboBox::calculateBackgroundBounds(const juce::Rectangle<float>& bounds) const
    {
        if (style_ == Style::ButtonLike)
            return bounds;
        
        const auto backgroundHeight = static_cast<float>(kBackgroundHeight_);
        const auto backgroundY = (bounds.getHeight() - backgroundHeight) * 0.5f;
        return juce::Rectangle<float>(bounds.getX(), bounds.getY() + backgroundY, bounds.getWidth(), backgroundHeight);
    }

    void ComboBox::showPopup()
    {
        if (! canShowPopup())
        {
            return;
        }

        showPopupAsynchronously();
        repaint();
    }

    bool ComboBox::canShowPopup() const
    {
        return isEnabled() && getNumItems() > 0;
    }

    void ComboBox::showPopupAsynchronously()
    {
        const auto useScrollableMode = (style_ == Style::ButtonLike);
        
        juce::MessageManager::callAsync([safePointer = SafePointer<ComboBox>(this), useScrollableMode]()
        {
            if (safePointer != nullptr && safePointer->canShowPopup())
            {
                safePointer->isPopupOpen_ = true;
                
                if (useScrollableMode)
                {
                    ScrollablePopupMenu::show(*safePointer);
                }
                else
                {
                    MultiColumnPopupMenu::show(*safePointer);
                }
            }
        });
    }

    void ComboBox::mouseDown(const juce::MouseEvent& e)
    {
        if (isEnabled())
        {
            if (e.mods.isLeftButtonDown())
            {
                showPopup();
            }
        }
    }

    void ComboBox::focusGained(juce::Component::FocusChangeType)
    {
        if (isEnabled() && !hasFocus_)
        {
            hasFocus_ = true;
            invalidateCache();
            repaint();
        }
    }

    void ComboBox::focusLost(juce::Component::FocusChangeType)
    {
        hasFocus_ = false;
        invalidateCache();
        repaint();
    }
}

