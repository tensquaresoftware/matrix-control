#include "PeakIndicator.h"

#include <cmath>

#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/Skin.h"

namespace TSS
{
    PeakIndicator::PeakIndicator(int width, int height)
        : width_(width)
        , height_(height)
    {
        setOpaque(true);
        setSize(width_, height_);
    }

    void PeakIndicator::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr)
            return;

        const auto bounds = getLocalBounds().toFloat();
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            kBorderThicknessDesign_,
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kFloor);
        const float maxBorder = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.49f;
        const float safeBorder = juce::jmin(borderThickness, maxBorder);
        const auto innerBounds = bounds.reduced(safeBorder).toNearestInt();

        g.setColour(buttonLook_.backgroundOff);
        g.fillRect(innerBounds);

        const float clampedLevel = juce::jlimit(0.0f, 1.0f, displayedLevel_);

        if (clampedLevel > 0.0f)
        {
            const int fillHeightPx = juce::jlimit(1,
                                                  innerBounds.getHeight(),
                                                  juce::roundToInt(static_cast<float>(innerBounds.getHeight()) * clampedLevel));
            const auto fillBounds = juce::Rectangle<int>(innerBounds.getX(),
                                                           innerBounds.getBottom() - fillHeightPx,
                                                           innerBounds.getWidth(),
                                                           fillHeightPx);

            g.setColour(sliderLook_.textEnabled);
            g.fillRect(fillBounds);
        }

        g.setColour(buttonLook_.borderOff);
        g.drawRect(bounds, borderThickness);
    }

    void PeakIndicator::setSkin(ISkin& skin)
    {
        skin_ = &skin;
        buttonLook_ = buttonLookFromSkin(skin);
        sliderLook_ = sliderLookFromSkin(skin);
        repaint();
    }

    void PeakIndicator::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;

        uiScale_ = uiScale;
        repaint();
    }

    void PeakIndicator::advanceRelease(juce::int64 nowMs)
    {
        if (lastUpdateMs_ <= 0 || displayedLevel_ <= 0.0f)
            return;

        const auto elapsed = nowMs - lastUpdateMs_;

        if (elapsed <= 0)
            return;

        const float decayRatio = static_cast<float>(elapsed) / static_cast<float>(kReleaseTimeMs_);
        displayedLevel_ *= std::exp(-decayRatio * 5.0f);
    }

    void PeakIndicator::updateBallistics(juce::int64 nowMs, float normalisedLevel)
    {
        advanceRelease(nowMs);

        const float clamped = juce::jlimit(0.0f, 1.0f, normalisedLevel);

        if (clamped > lastInputLevel_)
            displayedLevel_ = juce::jmax(displayedLevel_, clamped);

        lastInputLevel_ = clamped;
        lastUpdateMs_ = nowMs;
    }

    void PeakIndicator::ensureAnimationTimerRunning()
    {
        if (! isTimerRunning())
            startTimerHz(kAnimationHz_);
    }

    void PeakIndicator::timerCallback()
    {
        const auto nowMs = static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes());
        const float levelBefore = displayedLevel_;

        advanceRelease(nowMs);
        lastUpdateMs_ = nowMs;

        if (displayedLevel_ < 0.001f)
        {
            displayedLevel_ = 0.0f;
            stopTimer();
        }

        if (! juce::approximatelyEqual(levelBefore, displayedLevel_))
            repaint();
    }

    void PeakIndicator::setLevel(float normalisedLevel)
    {
        const auto nowMs = static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes());
        updateBallistics(nowMs, normalisedLevel);

        if (displayedLevel_ > 0.0f)
            ensureAnimationTimerRunning();

        repaint();
    }
}
