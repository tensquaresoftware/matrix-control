#include "TrackGeneratorDisplay.h"

namespace tss
{
    TrackGeneratorDisplay::TrackGeneratorDisplay(int width, int height, const TrackGeneratorDisplayLook& look)
        : look_(look)
        , width_(width)
        , height_(height)
    {
        setOpaque(false);
        setSize(width_, height_);
    }

    void TrackGeneratorDisplay::setLook(const TrackGeneratorDisplayLook& look)
    {
        look_ = look;
        repaint();
    }

    void TrackGeneratorDisplay::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;
        
        uiScale_ = uiScale;
        repaint();
    }

    void TrackGeneratorDisplay::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto contentBounds = bounds.withTrimmedTop(static_cast<float>(kWidgetPaddingTop_) * uiScale_)
            .withTrimmedBottom(static_cast<float>(kWidgetPaddingBottom_) * uiScale_);

        drawBackground(g, contentBounds);
        drawBorder(g, contentBounds);
        drawTriangle(g, contentBounds);
        drawCurve(g, contentBounds);
    }

    void TrackGeneratorDisplay::resized()
    {
        repaint();
    }


    void TrackGeneratorDisplay::drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        g.setColour(look_.background);
        g.fillRect(bounds);
    }

    void TrackGeneratorDisplay::drawBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        g.setColour(look_.border);
        g.drawRect(bounds, std::max(1.0f, static_cast<float>(kWidgetBorderThickness_) * uiScale_));
    }

    void TrackGeneratorDisplay::drawTriangle(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto triangleColour = look_.border;
        const float triangleBase = kWidgetTriangleBase_ * uiScale_;
        const float triangleHeight = triangleBase * std::sqrt(3.0f) * 0.5f;
        const float centreX = bounds.getCentreX();
        const float baseY = bounds.getY();

        juce::Path triangle;
        triangle.addTriangle(centreX - triangleBase * 0.5f, baseY,
                             centreX + triangleBase * 0.5f, baseY,
                             centreX, baseY - triangleHeight);

        g.setColour(triangleColour);
        g.fillPath(triangle);
    }
    
    void TrackGeneratorDisplay::setOnValueChanged(ValueChangedCallback callback)
    {
        onValueChanged_ = std::move(callback);
    }
    
    void TrackGeneratorDisplay::setTrackPoint1(int value)
    {
        setTrackPoint1(value, true);
    }
    
    void TrackGeneratorDisplay::setTrackPoint1(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (pointValues_[0] != clampedValue)
        {
            pointValues_[0] = clampedValue;
            repaint();

            if (notify && onValueChanged_)
                onValueChanged_(0, clampedValue);
        }
    }
    
    void TrackGeneratorDisplay::setTrackPoint2(int value)
    {
        setTrackPoint2(value, true);
    }
    
    void TrackGeneratorDisplay::setTrackPoint2(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (pointValues_[1] != clampedValue)
        {
            pointValues_[1] = clampedValue;
            repaint();

            if (notify && onValueChanged_)
                onValueChanged_(1, clampedValue);
        }
    }
    
    void TrackGeneratorDisplay::setTrackPoint3(int value)
    {
        setTrackPoint3(value, true);
    }
    
    void TrackGeneratorDisplay::setTrackPoint3(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (pointValues_[2] != clampedValue)
        {
            pointValues_[2] = clampedValue;
            repaint();

            if (notify && onValueChanged_)
                onValueChanged_(2, clampedValue);
        }
    }
    
    void TrackGeneratorDisplay::setTrackPoint4(int value)
    {
        setTrackPoint4(value, true);
    }
    
    void TrackGeneratorDisplay::setTrackPoint4(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (pointValues_[3] != clampedValue)
        {
            pointValues_[3] = clampedValue;
            repaint();

            if (notify && onValueChanged_)
                onValueChanged_(3, clampedValue);
        }
    }
    
    void TrackGeneratorDisplay::setTrackPoint5(int value)
    {
        setTrackPoint5(value, true);
    }
    
    void TrackGeneratorDisplay::setTrackPoint5(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (pointValues_[4] != clampedValue)
        {
            pointValues_[4] = clampedValue;
            repaint();

            if (notify && onValueChanged_)
                onValueChanged_(4, clampedValue);
        }
    }
    
    
    void TrackGeneratorDisplay::drawCurve(juce::Graphics& g, const juce::Rectangle<float>& innerBounds)
    {
        const auto centerBounds = getCurveCenterBounds(innerBounds);

        if (centerBounds.getWidth() <= 0.0f || centerBounds.getHeight() <= 0.0f)
            return;

        const float lineThickness = std::max(1.0f, kCurveLineThickness_ * uiScale_);
        
        g.setColour(look_.curve);

        for (int i = 0; i < kCurvePointCount_; ++i)
        {
            const auto p = calculatePointPosition(i, centerBounds);
            
            if (i < kCurvePointCount_ - 1)
            {
                const auto p2 = calculatePointPosition(i + 1, centerBounds);
                g.drawLine(p.x, p.y, p2.x, p2.y, lineThickness);
            }
        }
        
        const auto hollowPointFillColour = look_.curve.withAlpha(0.4f);
        const float pointRadius = kCurvePointRadius_ * uiScale_;

        for (int i = 0; i < kCurvePointCount_; ++i)
        {
            const auto p = calculatePointPosition(i, centerBounds);
            
            g.setColour(hollowPointFillColour);
            g.fillEllipse(p.x - pointRadius, p.y - pointRadius,
                         pointRadius * 2.0f, pointRadius * 2.0f);
            
            g.setColour(look_.curve);
            g.drawEllipse(p.x - pointRadius, p.y - pointRadius,
                         pointRadius * 2.0f, pointRadius * 2.0f,
                         lineThickness);
        }
    }
    
    juce::Rectangle<float> TrackGeneratorDisplay::getCurveCenterBounds(const juce::Rectangle<float>& innerBounds) const
    {
        const float totalPadding = (kCurvePadding_ + static_cast<float>(kWidgetBorderThickness_)) * uiScale_;
        return innerBounds.reduced(totalPadding);
    }

    juce::Point<float> TrackGeneratorDisplay::calculatePointPosition(int pointIndex,
                                                                      const juce::Rectangle<float>& centerBounds) const
    {
        const float xPosition = computePointX(pointIndex, centerBounds);
        const float normalizedValue = static_cast<float>(pointValues_[static_cast<size_t>(pointIndex)]) / kPointMaxValue_;
        const float yPosition = centerBounds.getBottom() - (centerBounds.getHeight() * normalizedValue);

        return juce::Point<float>(xPosition, yPosition);
    }

    float TrackGeneratorDisplay::computePointX(int pointIndex, const juce::Rectangle<float>& centerBounds) const
    {
        if (pointIndex == 0)
            return centerBounds.getX();
        if (pointIndex == kCurvePointCount_ - 1)
            return centerBounds.getRight();
        return centerBounds.getX() + (centerBounds.getWidth() * pointIndex) / (kCurvePointCount_ - 1);
    }
    
    int TrackGeneratorDisplay::findPointAtPosition(const juce::Point<float>& position,
                                                    const juce::Rectangle<float>& innerBounds) const
    {
        const auto centerBounds = getCurveCenterBounds(innerBounds);
        const float hitZoneRadius = kPointHitZoneRadius_ * uiScale_;

        for (int i = 0; i < kCurvePointCount_; ++i)
        {
            const auto pointPos = calculatePointPosition(i, centerBounds);
            const float distance = position.getDistanceFrom(pointPos);
            
            if (distance <= hitZoneRadius)
                return i;
        }
        
        return -1;
    }
    
    void TrackGeneratorDisplay::mouseDown(const juce::MouseEvent& e)
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto innerBounds = bounds.withTrimmedTop(static_cast<float>(kWidgetPaddingTop_) * uiScale_)
            .withTrimmedBottom(static_cast<float>(kWidgetPaddingBottom_) * uiScale_);

        draggedPointIndex_ = findPointAtPosition(e.position, innerBounds);
    }
    
    void TrackGeneratorDisplay::mouseDrag(const juce::MouseEvent& e)
    {
        if (draggedPointIndex_ < 0)
            return;
        
        const auto bounds = getLocalBounds().toFloat();
        const auto innerBounds = bounds.withTrimmedTop(static_cast<float>(kWidgetPaddingTop_) * uiScale_)
            .withTrimmedBottom(static_cast<float>(kWidgetPaddingBottom_) * uiScale_);
        const auto centerBounds = getCurveCenterBounds(innerBounds);

        const float relativeY = e.position.y - centerBounds.getY();
        const float normalizedValue = 1.0f - (relativeY / centerBounds.getHeight());
        
        const int newValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, 
                                          static_cast<int>(normalizedValue * kPointMaxValue_ + 0.5f));
        
        const auto index = static_cast<size_t>(draggedPointIndex_);
        if (pointValues_[index] != newValue)
        {
            pointValues_[index] = newValue;
            repaint();

            if (onValueChanged_)
                onValueChanged_(draggedPointIndex_, newValue);
        }
    }
    
    void TrackGeneratorDisplay::mouseUp(const juce::MouseEvent&)
    {
        draggedPointIndex_ = -1;
    }
}
