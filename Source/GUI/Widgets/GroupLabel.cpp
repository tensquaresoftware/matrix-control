#include "GroupLabel.h"

namespace tss
{
    GroupLabel::GroupLabel(int width, int height, const juce::String& text)
        : width_(width)
        , height_(height)
        , labelText_(text)
    {
        setOpaque(false);
        setSize(width_, height_);
        calculateTextWidth();
    }

    void GroupLabel::setLook(const GroupLabelLook& look)
    {
        look_ = look;
        calculateTextWidth();
        repaint();
    }

    void GroupLabel::setText(const juce::String& text)
    {
        if (labelText_ != text)
        {
            labelText_ = text;
            calculateTextWidth();
            repaint();
        }
    }

    void GroupLabel::paint(juce::Graphics& g)
    {
        if (labelText_.isEmpty())
            return;

        const auto bounds = getLocalBounds().toFloat();
        drawText(g, bounds);
        drawLines(g, bounds, cachedTextWidth_);
    }

    void GroupLabel::resized()
    {
        repaint();
    }

    void GroupLabel::drawText(juce::Graphics& g, const juce::Rectangle<float>& area)
    {
        g.setColour(look_.text);
        g.setFont(look_.font);
        g.drawText(labelText_, area, juce::Justification::centred, false);
    }

    void GroupLabel::drawLines(juce::Graphics& g, const juce::Rectangle<float>& area, float textWidth)
    {
        const auto halfTextWidth = textWidth * 0.5f;
        const auto centreX = area.getCentreX();
        const auto centreY = area.getCentreY();

        g.setColour(look_.line);

        drawLeftLine(g, area, centreX, halfTextWidth, centreY);
        drawRightLine(g, area, centreX, halfTextWidth, centreY);
    }

    void GroupLabel::drawLeftLine(juce::Graphics& g, const juce::Rectangle<float>& area, float centreX, float halfTextWidth, float centreY)
    {
        const auto lineEndX = centreX - halfTextWidth - kTextSpacing_;
        const auto lineWidth = lineEndX - area.getX();

        if (lineWidth > 0.0f)
        {
            const float lineThicknessHalf = static_cast<float>(kLineThickness_) * 0.5f;
            const float lineY = centreY - lineThicknessHalf;
            const auto line = juce::Rectangle<float>(
                area.getX(),
                lineY,
                lineWidth,
                static_cast<float>(kLineThickness_)
            );
            g.fillRect(line);
        }
    }

    void GroupLabel::drawRightLine(juce::Graphics& g, const juce::Rectangle<float>& area, float centreX, float halfTextWidth, float centreY)
    {
        const float lineStartX = centreX + halfTextWidth + kTextSpacing_;
        const float lineWidth = area.getRight() - lineStartX;

        if (lineWidth > 0.0f)
        {
            const float lineThicknessHalf = static_cast<float>(kLineThickness_) * 0.5f;
            const float lineY = centreY - lineThicknessHalf;
            const auto line = juce::Rectangle<float>(
                lineStartX,
                lineY,
                lineWidth,
                static_cast<float>(kLineThickness_)
            );
            g.fillRect(line);
        }
    }

    void GroupLabel::calculateTextWidth()
    {
        if (labelText_.isEmpty())
        {
            cachedTextWidth_ = 0.0f;
            return;
        }

        juce::GlyphArrangement glyphArrangement;
        glyphArrangement.addLineOfText(look_.font, labelText_, 0.0f, 0.0f);
        const auto bounds = glyphArrangement.getBoundingBox(0, -1, true);
        cachedTextWidth_ = bounds.getWidth();
    }
}
