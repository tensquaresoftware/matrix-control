#include "ModuleHeader.h"

namespace tss
{
    ModuleHeader::ModuleHeader(const juce::String& text, int width, int height, ColourVariant variant)
        : width_(width)
        , height_(height)
        , text_(text)
        , colourVariant_(variant)
    {
        setOpaque(false);
        setSize(width_, height_);
    }

    void ModuleHeader::setLook(const ModuleHeaderLook& look)
    {
        look_ = look;
        repaint();
    }

    void ModuleHeader::setDisplayScale(float displayScale)
    {
        if (juce::approximatelyEqual(displayScale_, displayScale))
            return;
        
        displayScale_ = displayScale;
        repaint();
    }

    void ModuleHeader::setText(const juce::String& text)
    {
        if (text_ != text)
        {
            text_ = text;
            repaint();
        }
    }

    void ModuleHeader::paint(juce::Graphics& g)
    {
        if (text_.isEmpty())
            return;

        const auto bounds = getLocalBounds().toFloat();
        drawText(g, bounds);
        drawLine(g, bounds);
    }

    void ModuleHeader::resized()
    {
        repaint();
    }

    void ModuleHeader::drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        if (text_.isEmpty())
            return;

        const float textAreaHeight = static_cast<float>(kTextAreaHeight_) * displayScale_;
        const float textLeftPadding = static_cast<float>(kTextLeftPadding_) * displayScale_;
        
        auto textBounds = bounds;
        textBounds.setHeight(textAreaHeight);
        textBounds.removeFromLeft(textLeftPadding);

        g.setColour(look_.text);
        g.setFont(look_.font.withHeight(look_.font.getHeight() * displayScale_));
        g.drawText(text_, textBounds, juce::Justification::centredLeft, false);
    }

    void ModuleHeader::drawLine(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const float textAreaHeight = static_cast<float>(kTextAreaHeight_) * displayScale_;
        const float lineThickness = std::max(1.0f, static_cast<float>(kLineThickness_) * displayScale_);
        const auto lineAreaHeight = bounds.getHeight() - textAreaHeight;
        const auto verticalOffset = textAreaHeight + (lineAreaHeight - lineThickness) * 0.5f;

        auto lineBounds = bounds;
        lineBounds.setHeight(lineThickness);
        lineBounds.translate(0.0f, verticalOffset);

        g.setColour(getLineColour());
        g.fillRect(lineBounds);
    }

    juce::Colour ModuleHeader::getLineColour() const
    {
        return (colourVariant_ == ColourVariant::Blue) ? look_.lineBlue : look_.lineOrange;
    }
}
