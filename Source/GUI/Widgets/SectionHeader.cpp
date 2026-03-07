#include "SectionHeader.h"

namespace tss
{
    SectionHeader::SectionHeader(int width, int height, const juce::String& text, ColourVariant variant)
        : width_(width)
        , height_(height)
        , text_(text)
        , colourVariant_(variant)
    {
        setOpaque(false);
        setSize(width_, height_);
        calculateTextWidth();
    }

    void SectionHeader::setLook(const SectionHeaderLook& look)
    {
        look_ = look;
        calculateTextWidth();
        repaint();
    }

    void SectionHeader::setScalingFactor(float scalingFactor)
    {
        if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
            return;
        
        scalingFactor_ = scalingFactor;
        calculateTextWidth();
        repaint();
    }

    void SectionHeader::paint(juce::Graphics& g)
    {
        if (text_.isEmpty())
            return;

        auto contentArea = getLocalBounds().toFloat();
        contentArea.setHeight(kContentHeight_ * scalingFactor_);

        drawText(g, contentArea);
        drawLines(g, contentArea);
    }

    void SectionHeader::resized()
    {
        repaint();
    }

    void SectionHeader::calculateTextWidth()
    {
        if (text_.isEmpty())
        {
            cachedTextWidth_ = 0.0f;
            return;
        }

        const auto scaledFont = look_.font.withHeight(look_.font.getHeight() * scalingFactor_);
        juce::GlyphArrangement glyphArrangement;
        glyphArrangement.addLineOfText(scaledFont, text_, 0.0f, 0.0f);
        cachedTextWidth_ = glyphArrangement.getBoundingBox(0, -1, true).getWidth();
    }

    void SectionHeader::drawText(juce::Graphics& g, const juce::Rectangle<float>& contentArea)
    {
        if (text_.isEmpty())
            return;

        const float leftLineWidth = kLeftLineWidth_ * scalingFactor_;
        const float textSpacing = kTextSpacing_ * scalingFactor_;
        
        auto textBounds = contentArea;
        textBounds.removeFromLeft(leftLineWidth + textSpacing);
        textBounds.setWidth(cachedTextWidth_);

        g.setColour(look_.text);
        g.setFont(look_.font.withHeight(look_.font.getHeight() * scalingFactor_));
        g.drawText(text_, textBounds, juce::Justification::topLeft, false);
    }

    void SectionHeader::drawLines(juce::Graphics& g, const juce::Rectangle<float>& contentArea)
    {
        g.setColour(getLineColour());

        drawLeftLine(g, contentArea);
        drawRightLine(g, contentArea);
    }

    void SectionHeader::drawLeftLine(juce::Graphics& g, const juce::Rectangle<float>& contentArea)
    {
        const float lineHeight = std::max(1.0f, kLineHeight_ * scalingFactor_);
        const auto verticalOffset = (contentArea.getHeight() - lineHeight) * 0.5f;
        
        auto line = contentArea;
        line.setWidth(kLeftLineWidth_ * scalingFactor_);
        line.setHeight(lineHeight);
        line.translate(0.0f, verticalOffset);
        
        g.fillRect(line);
    }

    void SectionHeader::drawRightLine(juce::Graphics& g, const juce::Rectangle<float>& contentArea)
    {
        const float leftLineWidth = kLeftLineWidth_ * scalingFactor_;
        const float textSpacing = kTextSpacing_ * scalingFactor_;
        const float lineHeight = std::max(1.0f, kLineHeight_ * scalingFactor_);
        const auto lineStartX = leftLineWidth + textSpacing + cachedTextWidth_ + textSpacing;
        const auto remainingWidth = contentArea.getWidth() - lineStartX;

        if (remainingWidth > 0.0f)
        {
            const auto verticalOffset = (contentArea.getHeight() - lineHeight) * 0.5f;
            
            auto line = contentArea;
            line.removeFromLeft(lineStartX);
            line.setHeight(lineHeight);
            line.translate(0.0f, verticalOffset);
            
            g.fillRect(line);
        }
    }

    juce::Colour SectionHeader::getLineColour() const
    {
        return (colourVariant_ == ColourVariant::Blue) ? look_.lineBlue : look_.lineOrange;
    }
}
