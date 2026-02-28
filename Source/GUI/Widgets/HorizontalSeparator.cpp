#include "HorizontalSeparator.h"


#include "GUI/Skins/ISkin.h"

using tss::SkinColourId;

namespace tss
{
    HorizontalSeparator::HorizontalSeparator(tss::ISkin& skin, int width, int height)
        : skin_(&skin)
        , width_(width)
        , height_(height)
    {
        setOpaque(false);
        setSize(width_, height_);
        cachedLineColour_ = skin_->getColour(SkinColourId::kHorizontalSeparatorLine);
    }

    void HorizontalSeparator::setSkin(tss::ISkin& skin)
    {
        skin_ = &skin;
        invalidateCache();
    }

    void HorizontalSeparator::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr)
            return;

        if (!cacheValid_)
            regenerateCache();

        if (cachedImage_.isValid())
        {
            g.drawImage(cachedImage_, getLocalBounds().toFloat(),
                       juce::RectanglePlacement::stretchToFit);
        }
    }

    void HorizontalSeparator::resized()
    {
        invalidateCache();
    }

    void HorizontalSeparator::regenerateCache()
    {
        const auto width = getWidth();
        const auto height = getHeight();

        if (width <= 0 || height <= 0)
            return;

        const float pixelScale = getPixelScale();
        const int imageWidth = juce::roundToInt(width * pixelScale);
        const int imageHeight = juce::roundToInt(height * pixelScale);

        cachedImage_ = juce::Image(juce::Image::ARGB, imageWidth, imageHeight, true);
        juce::Graphics g(cachedImage_);
        g.addTransform(juce::AffineTransform::scale(pixelScale));

        const auto bounds = juce::Rectangle<float>(0.0f, 0.0f, 
                                                    static_cast<float>(width), 
                                                    static_cast<float>(height));
        const auto lineY = bounds.getCentreY();
        
        g.setColour(cachedLineColour_);
        g.drawLine(bounds.getX(), lineY, bounds.getRight(), lineY, kLineThickness_);

        cacheValid_ = true;
    }

    void HorizontalSeparator::invalidateCache()
    {
        cacheValid_ = false;
    }

    float HorizontalSeparator::getPixelScale() const
    {
        const auto* display = juce::Desktop::getInstance()
                                  .getDisplays()
                                  .getDisplayForRect(getScreenBounds());
        return display != nullptr ? static_cast<float>(display->scale) : 1.0f;
    }
}

