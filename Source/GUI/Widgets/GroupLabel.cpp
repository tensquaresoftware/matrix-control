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

    void GroupLabel::setScalingFactor(float scalingFactor)
    {
        if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
            return;

        scalingFactor_ = scalingFactor;
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
        g.setFont(look_.font.withHeight(look_.font.getHeight() * scalingFactor_));
        g.drawText(labelText_, area, juce::Justification::centred, false);
    }

    void GroupLabel::drawLines(juce::Graphics& g, const juce::Rectangle<float>& area, float textWidth)
    {
        const auto halfTextWidth = textWidth * 0.5f;
        const auto centreX = area.getCentreX();
        const auto centreY = area.getCentreY();
        const float lineThickness = std::max(1.0f, static_cast<float>(kLineThickness_) * scalingFactor_);
        const float textSpacing = static_cast<float>(kTextSpacing_) * scalingFactor_;

        g.setColour(look_.line);

        drawLeftLine(g, area, centreX, halfTextWidth, centreY, lineThickness, textSpacing);
        drawRightLine(g, area, centreX, halfTextWidth, centreY, lineThickness, textSpacing);
    }

    void GroupLabel::drawLeftLine(juce::Graphics& g, const juce::Rectangle<float>& area, float centreX, float halfTextWidth, float centreY, float lineThickness, float textSpacing)
    {
        const auto lineEndX = centreX - halfTextWidth - textSpacing;
        const auto lineWidth = lineEndX - area.getX();

        if (lineWidth > 0.0f)
        {
            const float lineY = centreY - lineThickness * 0.5f;
            g.fillRect(juce::Rectangle<float>(area.getX(), lineY, lineWidth, lineThickness));
        }
    }

    void GroupLabel::drawRightLine(juce::Graphics& g, const juce::Rectangle<float>& area, float centreX, float halfTextWidth, float centreY, float lineThickness, float textSpacing)
    {
        const float lineStartX = centreX + halfTextWidth + textSpacing;
        const float lineWidth = area.getRight() - lineStartX;

        if (lineWidth > 0.0f)
        {
            const float lineY = centreY - lineThickness * 0.5f;
            g.fillRect(juce::Rectangle<float>(lineStartX, lineY, lineWidth, lineThickness));
        }
    }

    void GroupLabel::calculateTextWidth()
    {
        if (labelText_.isEmpty())
        {
            cachedTextWidth_ = 0.0f;
            return;
        }

        const auto scaledFont = look_.font.withHeight(look_.font.getHeight() * scalingFactor_);
        juce::GlyphArrangement glyphArrangement;
        glyphArrangement.addLineOfText(scaledFont, labelText_, 0.0f, 0.0f);
        const auto bounds = glyphArrangement.getBoundingBox(0, -1, true);
        cachedTextWidth_ = bounds.getWidth();
    }
}
