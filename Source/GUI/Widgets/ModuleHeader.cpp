#include "ModuleHeader.h"

#include "GUI/Themes/Skin.h"
#include "GUI/Themes/SkinValues.h"

namespace tss
{
    ModuleHeader::ModuleHeader(tss::Skin& skin, const juce::String& text, int width, int height, ColourVariant variant)
        : width_(width)
        , height_(height)
        , skin_(&skin)
        , text_(text)
        , colourVariant_(variant)
        , cachedFont_(juce::FontOptions())
    {
        setOpaque(false);
        setSize(width_, height_);
        updateSkinCache();
    }

    void ModuleHeader::setSkin(tss::Skin& skin)
    {
        skin_ = &skin;
        invalidateCache();
    }

    void ModuleHeader::setText(const juce::String& text)
    {
        if (text_ != text)
        {
            text_ = text;
            invalidateCache();
            repaint();
        }
    }

    void ModuleHeader::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr || text_.isEmpty())
            return;

        if (!cacheValid_)
            regenerateCache();

        if (cachedImage_.isValid())
        {
            g.drawImage(cachedImage_, getLocalBounds().toFloat(),
                       juce::RectanglePlacement::stretchToFit);
        }
    }

    void ModuleHeader::resized()
    {
        invalidateCache();
    }

    void ModuleHeader::regenerateCache()
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

        drawText(g, bounds);
        drawLine(g, bounds);

        cacheValid_ = true;
    }

    void ModuleHeader::invalidateCache()
    {
        cacheValid_ = false;
    }

    void ModuleHeader::updateSkinCache()
    {
        if (skin_ == nullptr)
            return;

        cachedTextColour_ = skin_->getModuleHeaderTextColour();
        cachedLineColour_ = getLineColour();
        cachedFont_ = skin_->getBaseFontBold().withHeight(kModuleHeaderFontHeight);
    }

    float ModuleHeader::getPixelScale() const
    {
        const auto* display = juce::Desktop::getInstance()
                                  .getDisplays()
                                  .getDisplayForRect(getScreenBounds());
        const float displayScale = display != nullptr ? static_cast<float>(display->scale) : 1.0f;
        return displayScale;
    }

    void ModuleHeader::drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        if (text_.isEmpty())
            return;

        auto textBounds = bounds;
        textBounds.setHeight(kTextAreaHeight_);
        textBounds.removeFromLeft(kTextLeftPadding_);

        g.setColour(cachedTextColour_);
        g.setFont(cachedFont_);
        g.drawText(text_, textBounds, juce::Justification::centredLeft, false);
    }

    void ModuleHeader::drawLine(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto lineAreaHeight = bounds.getHeight() - kTextAreaHeight_;
        const auto verticalOffset = kTextAreaHeight_ + (lineAreaHeight - kLineThickness_) * 0.5f;
        
        auto lineBounds = bounds;
        lineBounds.setHeight(kLineThickness_);
        lineBounds.translate(0.0f, verticalOffset);

        g.setColour(cachedLineColour_);
        g.fillRect(lineBounds);
    }

    juce::Colour ModuleHeader::getLineColour() const
    {
        return (colourVariant_ == ColourVariant::Blue) 
            ? skin_->getModuleHeaderLineColourBlue() 
            : skin_->getModuleHeaderLineColourOrange();
    }
}

