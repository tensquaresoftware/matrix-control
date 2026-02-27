#include "GroupLabel.h"

#include "GUI/Skins/Skin.h"

namespace tss
{
    GroupLabel::GroupLabel(tss::Skin& skin, int width, int height, const juce::String& text)
        : skin_(&skin)
        , width_(width)
        , height_(height)
        , labelText_(text)
        , cachedFont_(juce::FontOptions())
    {
        setOpaque(false);
        setSize(width_, height_);
        updateSkinCache();
        calculateTextWidth();
    }

    void GroupLabel::setSkin(tss::Skin& skin)
    {
        skin_ = &skin;
        invalidateCache();
    }

    void GroupLabel::setText(const juce::String& text)
    {
        if (labelText_ != text)
        {
            labelText_ = text;
            calculateTextWidth();
            invalidateCache();
            repaint();
        }
    }

    void GroupLabel::paint(juce::Graphics& g)
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

    void GroupLabel::resized()
    {
        invalidateCache();
    }

    void GroupLabel::regenerateCache()
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

        const auto bounds = getLocalBounds().toFloat();
        drawText(g, bounds);
        drawLines(g, bounds, cachedTextWidth_);

        cacheValid_ = true;
    }

    void GroupLabel::invalidateCache()
    {
        cacheValid_ = false;
    }

    void GroupLabel::updateSkinCache()
    {
        if (skin_ == nullptr)
            return;

        cachedTextColour_ = skin_->getGroupLabelTextColour();
        cachedLineColour_ = skin_->getGroupLabelLineColour();
        cachedFont_ = skin_->getBaseFont();
    }

    float GroupLabel::getPixelScale() const
    {
        const auto* display = juce::Desktop::getInstance()
                                  .getDisplays()
                                  .getDisplayForRect(getScreenBounds());

        return display != nullptr ? static_cast<float>(display->scale) : 1.0f;
    }

    void GroupLabel::drawText(juce::Graphics& g, const juce::Rectangle<float>& area)
    {
        g.setColour(cachedTextColour_);
        g.setFont(cachedFont_);
        g.drawText(labelText_, area, juce::Justification::centred, false);
    }

    void GroupLabel::drawLines(juce::Graphics& g, const juce::Rectangle<float>& area, float textWidth)
    {
        const auto halfTextWidth = textWidth * 0.5f;
        const auto centreX = area.getCentreX();
        const auto centreY = area.getCentreY();

        g.setColour(cachedLineColour_);

        drawLeftLine(g, area, centreX, halfTextWidth, centreY);
        drawRightLine(g, area, centreX, halfTextWidth, centreY);
    }

    void GroupLabel::drawLeftLine(juce::Graphics& g, const juce::Rectangle<float>& area, float centreX, float halfTextWidth, float centreY)
    {
        const auto lineEndX = centreX - halfTextWidth - kTextSpacing_;
        const auto lineWidth = lineEndX - area.getX();

        if (lineWidth > 0.0f)
        {
            const auto lineThicknessHalf = kLineThickness_ * 0.5f;
            const auto lineY = std::round(centreY - lineThicknessHalf);
            const auto line = juce::Rectangle<float>(
                area.getX(),
                lineY,
                lineWidth,
                kLineThickness_
            );
            g.fillRect(line);
        }
    }

    void GroupLabel::drawRightLine(juce::Graphics& g, const juce::Rectangle<float>& area, float centreX, float halfTextWidth, float centreY)
    {
        const auto lineStartX = centreX + halfTextWidth + kTextSpacing_;
        const auto lineWidth = area.getRight() - lineStartX;

        if (lineWidth > 0.0f)
        {
            const auto lineThicknessHalf = kLineThickness_ * 0.5f;
            const auto lineY = std::round(centreY - lineThicknessHalf);
            const auto line = juce::Rectangle<float>(
                lineStartX,
                lineY,
                lineWidth,
                kLineThickness_
            );
            g.fillRect(line);
        }
    }

    void GroupLabel::calculateTextWidth()
    {
        if (labelText_.isEmpty() || skin_ == nullptr)
        {
            cachedTextWidth_ = 0.0f;
            return;
        }

        juce::GlyphArrangement glyphArrangement;
        glyphArrangement.addLineOfText(cachedFont_, labelText_, 0.0f, 0.0f);
        const auto bounds = glyphArrangement.getBoundingBox(0, -1, true);
        cachedTextWidth_ = bounds.getWidth();
    }
}

