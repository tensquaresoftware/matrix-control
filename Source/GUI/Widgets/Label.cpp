#include "Label.h"

namespace tss
{
    Label::Label(int width, int height, const juce::String& text, tss::LabelStyle style)
        : width_(width)
        , height_(height)
        , labelText_(text)
        , style_(style)
    {
        setOpaque(false);
        setSize(width_, height_);
    }

    void Label::setLook(const LabelLook& look)
    {
        look_ = look;
        repaint();
    }

    void Label::setDisplayScale(float displayScale)
    {
        if (juce::approximatelyEqual(displayScale_, displayScale))
            return;
        
        displayScale_ = displayScale;
        repaint();
    }

    void Label::setText(const juce::String& text)
    {
        if (labelText_ != text)
        {
            labelText_ = text;
            repaint();
        }
    }

    void Label::paint(juce::Graphics& g)
    {
        if (labelText_.isEmpty())
            return;

        auto textBounds = getLocalBounds().toFloat();
        const float padding = static_cast<float>(kTextLeftPadding_) * displayScale_;
        textBounds.removeFromLeft(padding);

        g.setColour(look_.text);
        g.setFont(look_.font.withHeight(kFontSize_ * displayScale_));
        g.drawText(labelText_, textBounds, juce::Justification::centredLeft, false);
    }
}
