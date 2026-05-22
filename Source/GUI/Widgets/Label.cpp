#include "Label.h"

namespace tss
{
    Label::Label(int width, int height, const LabelLook& look, const juce::String& text, tss::LabelStyle style)
        : look_(look)
        , width_(width)
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

    void Label::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;
        
        uiScale_ = uiScale;
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
        const float padding = static_cast<float>(kTextLeftPadding_) * uiScale_;
        textBounds.removeFromLeft(padding);

        g.setColour(look_.text);
        g.setFont(look_.font.withHeight(look_.font.getHeight() * uiScale_));
        g.drawText(labelText_, textBounds, juce::Justification::centredLeft, false);
    }
}
