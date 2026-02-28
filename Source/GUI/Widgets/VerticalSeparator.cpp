#include "VerticalSeparator.h"


#include "GUI/Skins/ISkin.h"

using tss::SkinColourId;

namespace tss
{
    VerticalSeparator::VerticalSeparator(tss::ISkin& skin, int width, int height)
        : skin_(&skin)
        , width_(width)
        , height_(height)
    {
        setOpaque(false);
        setSize(width_, height_);
        cachedLineColour_ = skin_->getColour(SkinColourId::kVerticalSeparatorLine);
    }

    void VerticalSeparator::setSkin(tss::ISkin& skin)
    {
        skin_ = &skin;
        invalidateCache();
    }

    void VerticalSeparator::paint(juce::Graphics& g)
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

    void VerticalSeparator::resized()
    {
        invalidateCache();
    }

    void VerticalSeparator::regenerateCache()
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
        const auto lineX = std::round(bounds.getCentreX() - kLineWidth_ * 0.5f);
        
        auto line = bounds;
        line.removeFromTop(kTopPadding_);
        line.removeFromBottom(kBottomPadding_);
        line.setX(lineX);
        line.setWidth(kLineWidth_);
        
        g.setColour(cachedLineColour_);
        g.fillRect(line);

        cacheValid_ = true;
    }

    void VerticalSeparator::invalidateCache()
    {
        cacheValid_ = false;
    }

    float VerticalSeparator::getPixelScale() const
    {
        const auto* display = juce::Desktop::getInstance()
                                  .getDisplays()
                                  .getDisplayForRect(getScreenBounds());
        return display != nullptr ? static_cast<float>(display->scale) : 1.0f;
    }
}

