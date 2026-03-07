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

    void Label::setScalingFactor(float scalingFactor)
    {
        if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
            return;
        
        scalingFactor_ = scalingFactor;
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
        const float padding = static_cast<float>(kTextLeftPadding_) * scalingFactor_;
        textBounds.removeFromLeft(padding);

        g.setColour(look_.text);
        g.setFont(look_.font.withHeight(kFontSize_ * scalingFactor_));
        g.drawText(labelText_, textBounds, juce::Justification::centredLeft, false);
    }
}
