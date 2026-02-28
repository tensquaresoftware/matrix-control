#include "Label.h"


#include "GUI/Skins/ISkin.h"

using tss::SkinColourId;

namespace tss
{
    Label::Label(tss::ISkin& skin, int width, int height, const juce::String& text, tss::LabelStyle style)
        : skin_(&skin)
        , width_(width)
        , height_(height)
        , labelText_(text)
        , style_(style)
        , cachedFont_(juce::FontOptions())
    {
        setOpaque(false);
        setSize(width_, height_);
        updateSkinCache();
    }

    void Label::setSkin(tss::ISkin& skin)
    {
        skin_ = &skin;
        invalidateCache();
    }

    void Label::setText(const juce::String& text)
    {
        if (labelText_ != text)
        {
            labelText_ = text;
            invalidateCache();
            repaint();
        }
    }

    void Label::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr || labelText_.isEmpty())
            return;

        if (!cacheValid_)
            regenerateCache();

        if (cachedImage_.isValid())
        {
            g.drawImage(cachedImage_, getLocalBounds().toFloat(),
                       juce::RectanglePlacement::stretchToFit);
        }
    }

    void Label::resized()
    {
        invalidateCache();
    }

    void Label::regenerateCache()
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

        auto textBounds = juce::Rectangle<float>(0.0f, 0.0f, 
                                                   static_cast<float>(width), 
                                                   static_cast<float>(height));
        textBounds.removeFromLeft(static_cast<float>(kTextLeftPadding_));

        g.setColour(cachedTextColour_);
        g.setFont(cachedFont_);
        g.drawText(labelText_, textBounds, juce::Justification::centredLeft, false);

        cacheValid_ = true;
    }

    void Label::invalidateCache()
    {
        cacheValid_ = false;
    }

    void Label::updateSkinCache()
    {
        if (skin_ == nullptr)
            return;

        cachedTextColour_ = (style_ == LabelStyle::HeaderPanel)
            ? skin_->getColour(SkinColourId::kHeaderPanelLabelText)
            : skin_->getColour(SkinColourId::kLabelText);
        cachedFont_ = skin_->getBaseFont();
    }

    float Label::getPixelScale() const
    {
        const auto* display = juce::Desktop::getInstance()
                                  .getDisplays()
                                  .getDisplayForRect(getScreenBounds());

        return display != nullptr ? static_cast<float>(display->scale) : 1.0f;
    }
}

