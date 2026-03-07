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

    void SectionHeader::paint(juce::Graphics& g)
    {
        if (text_.isEmpty())
            return;

        auto contentArea = getLocalBounds().toFloat();
        contentArea.setHeight(kContentHeight_);

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

        juce::GlyphArrangement glyphArrangement;
        glyphArrangement.addLineOfText(look_.font, text_, 0.0f, 0.0f);
        cachedTextWidth_ = glyphArrangement.getBoundingBox(0, -1, true).getWidth();
    }

    void SectionHeader::drawText(juce::Graphics& g, const juce::Rectangle<float>& contentArea)
    {
        if (text_.isEmpty())
            return;

        auto textBounds = contentArea;
        textBounds.removeFromLeft(kLeftLineWidth_ + kTextSpacing_);
        textBounds.setWidth(cachedTextWidth_);

        g.setColour(look_.text);
        g.setFont(look_.font);
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
        return (colourVariant_ == ColourVariant::Blue) ? look_.lineBlue : look_.lineOrange;
    }
}
