#include "SectionHeader.h"

#include "GUI/Layout/ScaledDrawing.h"

namespace TSS
{
    SectionHeader::SectionHeader(int width, int height, const SectionHeaderLook& look, const juce::String& text, ColourVariant variant)
        : look_(look)
        , width_(width)
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

    void SectionHeader::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;
        
        uiScale_ = uiScale;
        calculateTextWidth();
        repaint();
    }

    void SectionHeader::paint(juce::Graphics& g)
    {
        if (text_.isEmpty())
            return;

        auto contentArea = getLocalBounds().toFloat();
        contentArea.setHeight(kContentHeight_ * uiScale_);

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

        const auto scaledFont = look_.font.withHeight(look_.font.getHeight() * uiScale_);
        juce::GlyphArrangement glyphArrangement;
        glyphArrangement.addLineOfText(scaledFont, text_, 0.0f, 0.0f);
        cachedTextWidth_ = glyphArrangement.getBoundingBox(0, -1, true).getWidth();
    }

    void SectionHeader::drawText(juce::Graphics& g, const juce::Rectangle<float>& contentArea)
    {
        if (text_.isEmpty())
            return;

        const float leftLineWidth = kLeftLineWidth_ * uiScale_;
        const float textSpacing = kTextSpacing_ * uiScale_;
        
        auto textBounds = contentArea;
        textBounds.removeFromLeft(leftLineWidth + textSpacing);
        textBounds.setWidth(cachedTextWidth_);

        g.setColour(look_.text);
        g.setFont(look_.font.withHeight(look_.font.getHeight() * uiScale_));
        g.drawText(text_, textBounds, juce::Justification::topLeft, false);
    }

    void SectionHeader::drawLines(juce::Graphics& g, const juce::Rectangle<float>& contentArea)
    {
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float lineThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            kLineHeight_,
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);

        g.setColour(getLineColour());

        drawLeftLine(g, contentArea, lineThickness);
        drawRightLine(g, contentArea, lineThickness);
    }

    void SectionHeader::drawLeftLine(juce::Graphics& g, const juce::Rectangle<float>& contentArea, float lineThickness)
    {
        auto line = contentArea;
        line.setWidth(kLeftLineWidth_ * uiScale_);
        line.setHeight(lineThickness);
        line.setY(contentArea.getCentreY() - lineThickness * 0.5f);

        g.fillRect(line);
    }

    void SectionHeader::drawRightLine(juce::Graphics& g, const juce::Rectangle<float>& contentArea, float lineThickness)
    {
        const float leftLineWidth = kLeftLineWidth_ * uiScale_;
        const float textSpacing = kTextSpacing_ * uiScale_;
        const auto lineStartX = leftLineWidth + textSpacing + cachedTextWidth_ + textSpacing;
        const auto remainingWidth = contentArea.getWidth() - lineStartX;

        if (remainingWidth > 0.0f)
        {
            auto line = contentArea;
            line.removeFromLeft(lineStartX);
            line.setHeight(lineThickness);
            line.setY(contentArea.getCentreY() - lineThickness * 0.5f);

            g.fillRect(line);
        }
    }

    juce::Colour SectionHeader::getLineColour() const
    {
        return (colourVariant_ == ColourVariant::Blue) ? look_.lineBlue : look_.lineOrange;
    }
}
