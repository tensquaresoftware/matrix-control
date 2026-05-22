#include "ComboBox.h"
#include "MultiColumnPopupMenu.h"
#include "ScrollablePopupMenu.h"

#include "GUI/Skins/ColourChart.h"

namespace tss
{
    ComboBox::ComboBox(int width, int height, const ComboBoxLook& look, Style style)
        : juce::ComboBox()
        , look_(look)
        , width_(width)
        , height_(height)
        , style_(style)
    {
        setOpaque(false);
        setSize(width_, height_);
        setWantsKeyboardFocus(true);
        setColour(juce::ComboBox::textColourId, juce::Colour(ColourChart::kTransparent));
    }

    void ComboBox::setLook(const ComboBoxLook& look)
    {
        look_ = look;
        repaint();
    }

    void ComboBox::setPopupMenuLook(const PopupMenuLook& look)
    {
        popupLook_ = look;
    }

    void ComboBox::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;
        
        uiScale_ = uiScale;
        repaint();
    }

    void ComboBox::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto enabled = isEnabled();
        const auto hasFocus = hasFocus_ || isPopupOpen_;
        const auto backgroundBounds = calculateBackgroundBounds(bounds);

        drawBackground(g, backgroundBounds, enabled);
        drawText(g, bounds, enabled);
        drawTriangle(g, bounds, enabled);
        drawBorderIfNeeded(g, bounds, backgroundBounds, enabled, hasFocus);
    }

    void ComboBox::drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled)
    {
        g.setColour(getBackgroundColourForCurrentStyle(enabled));
        g.fillRect(bounds);
    }

    void ComboBox::drawBorderIfNeeded(juce::Graphics& g, const juce::Rectangle<float>& bounds, const juce::Rectangle<float>& backgroundBounds, bool enabled, bool hasFocus)
    {
        if (style_ == Style::ButtonLike)
        {
            const float thickness = std::max(1.0f, static_cast<float>(kBorderThicknessButtonLike_) * uiScale_);
            g.setColour(getBorderColourForCurrentStyle(enabled));
            g.drawRect(bounds, thickness);
            return;
        }

        if (hasFocus)
        {
            const float thickness = std::max(1.0f, static_cast<float>(kBorderThickness_) * uiScale_);
            g.setColour(getFocusBorderColourForCurrentStyle());
            g.drawRect(backgroundBounds, thickness);
        }
    }

    void ComboBox::drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled)
    {
        const auto text = getSelectedItemText();
        const auto textColour = getTextColourForCurrentStyle(enabled);
        const auto textBounds = calculateTextBounds(bounds);

        g.setColour(textColour);
        g.setFont(look_.font.withHeight(look_.font.getHeight() * uiScale_));
        g.drawText(text, textBounds, juce::Justification::centredLeft, false);
    }

    juce::String ComboBox::getSelectedItemText() const
    {
        const auto selectedIndex = getSelectedItemIndex();
        if (selectedIndex >= 0)
            return getItemText(selectedIndex);

        return juce::String();
    }

    juce::Colour ComboBox::getTextColourForCurrentStyle(bool enabled) const
    {
        if (style_ == Style::ButtonLike)
            return enabled ? look_.buttonLikeText : look_.buttonLikeTextDisabled;
        
        return enabled ? look_.textEnabled : look_.textDisabled;
    }

    juce::Colour ComboBox::getTriangleColourForCurrentStyle(bool enabled) const
    {
        if (style_ == Style::ButtonLike)
            return enabled ? look_.buttonLikeTriangle : look_.buttonLikeTriangleDisabled;
        
        return enabled ? look_.triangleEnabled : look_.triangleDisabled;
    }

    juce::Colour ComboBox::getBackgroundColourForCurrentStyle(bool enabled) const
    {
        if (style_ == Style::ButtonLike)
            return enabled ? look_.buttonLikeBackground : look_.buttonLikeBackgroundDisabled;
        
        return enabled ? look_.backgroundEnabled : look_.backgroundDisabled;
    }

    juce::Colour ComboBox::getBorderColourForCurrentStyle(bool enabled) const
    {
        if (style_ == Style::ButtonLike)
            return enabled ? look_.buttonLikeBorder : look_.buttonLikeBorderDisabled;
        
        return enabled ? look_.borderEnabled : look_.borderDisabled;
    }

    juce::Colour ComboBox::getFocusBorderColourForCurrentStyle() const
    {
        return look_.focusBorder;
    }

    juce::Rectangle<float> ComboBox::calculateTextBounds(const juce::Rectangle<float>& bounds) const
    {
        auto textBounds = bounds;
        const float leftPad = static_cast<float>(kLeftPadding_) * uiScale_;
        const float triangleSpace = static_cast<float>(kTriangleBaseSize_) * uiScale_;
        const float rightPad = static_cast<float>(kRightPadding_) * uiScale_;
        
        textBounds.removeFromLeft(leftPad);
        textBounds.removeFromRight(triangleSpace);
        textBounds.removeFromRight(rightPad);
        return textBounds;
    }

    void ComboBox::drawTriangle(juce::Graphics& g, const juce::Rectangle<float>& bounds, bool enabled)
    {
        const auto triangleColour = getTriangleColourForCurrentStyle(enabled);
        g.setColour(triangleColour);

        const float triangleBaseSize = static_cast<float>(kTriangleBaseSize_) * uiScale_;
        const float triangleHeight = triangleBaseSize * kTriangleHeightFactor_;
        const float rightPad = static_cast<float>(kRightPadding_) * uiScale_;
        const float triangleX = bounds.getRight() - triangleBaseSize - rightPad;
        const float triangleY = bounds.getCentreY() - triangleHeight * 0.5f;

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
        
        const auto backgroundHeight = static_cast<float>(kBackgroundHeight_) * uiScale_;
        const auto backgroundY = (bounds.getHeight() - backgroundHeight) * 0.5f;
        return juce::Rectangle<float>(bounds.getX(), bounds.getY() + backgroundY, bounds.getWidth(), backgroundHeight);
    }

    void ComboBox::showPopup()
    {
        if (! canShowPopup())
            return;

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
        if (isEnabled() && ! hasFocus_)
        {
            hasFocus_ = true;
            repaint();
        }
    }

    void ComboBox::focusLost(juce::Component::FocusChangeType)
    {
        hasFocus_ = false;
        repaint();
    }
}
