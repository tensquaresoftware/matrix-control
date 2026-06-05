#include "PeakIndicator.h"

#include "GUI/Skins/Skin.h"

using tss::SkinColourId;

namespace tss
{
    PeakIndicator::PeakIndicator(int width, int height)
        : width_(width)
        , height_(height)
    {
        setOpaque(false);
    }

    void PeakIndicator::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr)
            return;

        const auto bounds = getLocalBounds().toFloat();
        const auto fillColour = skin_->getColour(SkinColourId::kEnvelopeDisplayEnvelope);
        const auto backgroundColour = fillColour.withAlpha(0.15f);

        g.setColour(backgroundColour);
        g.fillRect(bounds);

        const float clampedLevel = juce::jlimit(0.0f, 1.0f, level_);
        const float fillHeight = bounds.getHeight() * clampedLevel;
        const auto fillBounds = juce::Rectangle<float>(bounds.getX(),
                                                       bounds.getBottom() - fillHeight,
                                                       bounds.getWidth(),
                                                       fillHeight);

        g.setColour(fillColour);
        g.fillRect(fillBounds);
    }

    void PeakIndicator::setSkin(ISkin& skin)
    {
        skin_ = &skin;
        repaint();
    }

    void PeakIndicator::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;

        uiScale_ = uiScale;
        repaint();
    }

    void PeakIndicator::setLevel(float normalisedLevel)
    {
        const float clamped = juce::jlimit(0.0f, 1.0f, normalisedLevel);

        if (juce::approximatelyEqual(level_, clamped))
            return;

        level_ = clamped;
        repaint();
    }
}
