#include "SectionHeader.h"


#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinValues.h"

using tss::SkinColourId;

namespace tss
{
    SectionHeader::SectionHeader(tss::ISkin& skin, int width, int height, const juce::String& text, ColourVariant variant)
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
        calculateTextWidth();
    }

    void SectionHeader::setSkin(tss::ISkin& skin)
    {
        skin_ = &skin;
        invalidateCache();
    }

    void SectionHeader::paint(juce::Graphics& g)
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

    void SectionHeader::resized()
    {
        invalidateCache();
    }

    void SectionHeader::regenerateCache()
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

        auto contentArea = juce::Rectangle<float>(0.0f, 0.0f, 
                                                   static_cast<float>(width), 
                                                   static_cast<float>(height));
        contentArea.setHeight(kContentHeight_);

        drawText(g, contentArea);
        drawLines(g, contentArea);

        cacheValid_ = true;
    }

    void SectionHeader::invalidateCache()
    {
        cacheValid_ = false;
    }

    void SectionHeader::updateSkinCache()
    {
        if (skin_ == nullptr)
            return;

        cachedTextColour_ = skin_->getColour(SkinColourId::kSectionHeaderText);
        cachedLineColour_ = getLineColour();
        cachedFont_ = skin_->getBaseFont().withHeight(kSectionHeaderFontHeight);
    }

    float SectionHeader::getPixelScale() const
    {
        const auto* display = juce::Desktop::getInstance()
                                  .getDisplays()
                                  .getDisplayForRect(getScreenBounds());
        const float displayScale = display != nullptr ? static_cast<float>(display->scale) : 1.0f;
        return displayScale;
    }

    void SectionHeader::drawText(juce::Graphics& g, const juce::Rectangle<float>& contentArea)
    {
        if (text_.isEmpty())
            return;

        auto textBounds = contentArea;
        textBounds.removeFromLeft(kLeftLineWidth_ + kTextSpacing_);
        textBounds.setWidth(cachedTextWidth_);

        g.setColour(cachedTextColour_);
        g.setFont(cachedFont_);
        g.drawText(text_, textBounds, juce::Justification::topLeft, false);
    }

    void SectionHeader::drawLines(juce::Graphics& g, const juce::Rectangle<float>& contentArea)
    {
        g.setColour(cachedLineColour_);
        
        drawLeftLine(g, contentArea);
        drawRightLine(g, contentArea);
    }

    void SectionHeader::drawLeftLine(juce::Graphics& g, const juce::Rectangle<float>& contentArea)
    {
        const auto verticalOffset = (contentArea.getHeight() - kLineHeight_) * 0.5f;
        
        auto line = contentArea;
        line.setWidth(kLeftLineWidth_);
        line.setHeight(kLineHeight_);
        line.translate(0.0f, verticalOffset);
        
        g.fillRect(line);
    }

    void SectionHeader::drawRightLine(juce::Graphics& g, const juce::Rectangle<float>& contentArea)
    {
        const auto lineStartX = kLeftLineWidth_ + kTextSpacing_ + cachedTextWidth_ + kTextSpacing_;
        const auto remainingWidth = contentArea.getWidth() - lineStartX;

        if (remainingWidth > 0.0f)
        {
            const auto verticalOffset = (contentArea.getHeight() - kLineHeight_) * 0.5f;
            
            auto line = contentArea;
            line.removeFromLeft(lineStartX);
            line.setHeight(kLineHeight_);
            line.translate(0.0f, verticalOffset);
            
            g.fillRect(line);
        }
    }

    juce::Colour SectionHeader::getLineColour() const
    {
        return (colourVariant_ == ColourVariant::Blue) 
            ? skin_->getColour(SkinColourId::kSectionHeaderLineBlue) 
            : skin_->getColour(SkinColourId::kSectionHeaderLineOrange);
    }

    void SectionHeader::calculateTextWidth()
    {
        if (text_.isEmpty() || skin_ == nullptr)
        {
            cachedTextWidth_ = 0.0f;
            return;
        }

        juce::GlyphArrangement glyphArrangement;
        glyphArrangement.addLineOfText(cachedFont_, text_, 0.0f, 0.0f);
        cachedTextWidth_ = glyphArrangement.getBoundingBox(0, -1, true).getWidth();
    }
}

